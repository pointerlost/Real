//
// Created by pointerlost on 3/23/26.
//
#include "Assets/TextureManager.h"
#include <cstring>
#include "../../include/Core/FileManager.h"
#include "Core/CMakeConfig.h"
#include "Core/Logger.h"
#include "Graphics/Material.h"
#include <ranges>
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Tools/Image/TextureUtils.h"

namespace Real::assets {

    TextureManager::TextureManager() {
        InitDefaults();
    }

    void* TextureManager::Get(core::SlotHandle handle) {
        const auto* ref = m_Textures.Get(handle);  // GLTextureReference* = Ref<OpenGLTexture>*
        return ref ? ref->get() : nullptr;         // .get() -> raw OpenGLTexture* stored as void*
    }

    void TextureManager::Release(core::SlotHandle handle) {
        m_Textures.Remove(handle);
    }

    GLTextureResourceHandle TextureManager::Register(const GLTextureReference& texture) {
        core::SlotHandle slot = m_Textures.Add(texture);
        m_UUIDToSlot[texture->GetUUID()] = slot;
        m_PendingUpload.push_back(texture);

        GLTextureResourceHandle handle(this, slot);
        return handle;
    }

    GLTextureResourceHandle TextureManager::FindByUUID(const UUID& uuid) {
        if (!m_UUIDToSlot.contains(uuid))
            return {};

        GLTextureResourceHandle handle(this, m_UUIDToSlot[uuid]);
        return handle;
    }

    void TextureManager::DeleteCPU(core::SlotHandle slot) {
        if (const auto* ref = m_Textures.Get(slot))
            m_UUIDToSlot.erase((*ref)->GetUUID());
        m_Textures.Remove(slot);
    }

    GLTextureReference TextureManager::GetTexture(const GLTextureResourceHandle& handle) {
        if (!handle.IsValid())
            return GetOrCreateDefault(TextureType::ALPHA); // Debug texture type is ALPHA
        const auto* ref = m_Textures.Get(handle.GetHandle());
        if (ref && *ref) return *ref;
        return GetOrCreateDefault(TextureType::ALPHA);     // Debug texture type is ALPHA
    }

    GLTextureReference TextureManager::GetTexture(const UUID &id) {
        return GetTexture(FindByUUID(id));
    }

    GLTextureReference& TextureManager::GetOrCreateDefault(TextureType type) {
        if (m_DefaultTextures.contains(type))
            return m_DefaultTextures[type];

        const int channelCount = util::texture::TextureTypeToChannelCount(type);
        u8 r = 255, g = 255, b = 255, a = 255;
        switch (type) {
            case TextureType::ALBEDO:            r=128; g=128; b=128; a=255; break;
            case TextureType::NORMAL:            r=128; g=128; b=255; a=255; break;
            case TextureType::EMISSIVE:          r=0;   g=0;   b=0;   a=255; break;
            case TextureType::ROUGHNESS:
            case TextureType::AMBIENT_OCCLUSION: r=255; g=0;   b=0;   a=0;   break;
            case TextureType::METALLIC:
            case TextureType::HEIGHT:            r=0;   g=0;   b=0;   a=0;   break;
            case TextureType::ORM:               r=255; g=255; b=0;   a=255; break;

            default:                             r=255; g=255; b=255; a=255; break; // Use ALPHA as default
        }

        auto* imageData   = new u8[channelCount];
        const u8 pixel[4] = { r, g, b, a };
        memcpy(imageData, pixel, channelCount);

        TextureData data{};
        data.data         = imageData;
        data.channelCount = channelCount;
        data.dataSize     = channelCount;
        data.width        = 1;
        data.height       = 1;

        auto tex = CreateRef<platform::opengl::OpenGLTexture>();
        tex->SetOrigin(platform::opengl::OpenGLTexture::TextureOrigin::Generated);
        tex->SetImageFormatState(ImageFormatState::DEFAULT);
        tex->CreateFromData(data, type);

        // Pinned - gets a slot, never removed
        core::SlotHandle slot = m_Textures.Add(tex);
        m_UUIDToSlot[tex->GetUUID()] = slot;

        return m_DefaultTextures[type] = tex;
    }

    bool TextureManager::IsCompressed(const String& stem) const {
        return fs::File::Exists(String(ASSETS_DIR) + "textures/compressed/" + stem + ".dds");
    }

    Vector<BindlessHandle> TextureManager::FlushPendingUploads() {
        Vector<BindlessHandle> newHandles;
        newHandles.reserve(m_PendingUpload.size());

        Vector<GLTextureReference> stillPending;

        for (const auto& tex : m_PendingUpload) {
            if (!tex->IsReadyForUpload()) {
                stillPending.push_back(tex);
                continue;
            }
            tex->UploadToGPU();
            tex->SetGPUIndex(static_cast<GPUIndex>(m_BindlessHandles.size()));
            m_BindlessHandles.push_back(tex->GetBindless());
            newHandles.push_back(tex->GetBindless());
        }

        m_PendingUpload = std::move(stillPending);  // failed ones stay for next flush
        return newHandles;
    }

    size_t TextureManager::GetNextBindlessIndex() const {
        return m_BindlessHandles.size();
    }

    Vector<GLTextureReference> TextureManager::GetMaterialTextures(const Material* mat) {
        Vector<GLTextureReference> textures;

        auto tryAdd = [&](const GLTextureResourceHandle& handle) {
            if (!handle.IsValid()) return;
            auto tex = GetTexture(handle);
            if (tex) textures.push_back(tex);
        };

        tryAdd(mat->albedo);
        tryAdd(mat->normal);
        tryAdd(mat->orm);
        tryAdd(mat->height);
        tryAdd(mat->emissive);
        return textures;
    }

    void TextureManager::InitDefaults() {
        constexpr TextureType types[] = {
            TextureType::ALBEDO,            TextureType::NORMAL,
            TextureType::ROUGHNESS,         TextureType::METALLIC,
            TextureType::AMBIENT_OCCLUSION, TextureType::ORM,
            TextureType::HEIGHT,            TextureType::EMISSIVE,
            TextureType::ALPHA,
        };
        for (const auto t : types)
            GetOrCreateDefault(t);
    }
}