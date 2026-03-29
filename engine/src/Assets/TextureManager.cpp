//
// Created by pointerlost on 3/23/26.
//
#include "Assets/TextureManager.h"
#include <cstring>
#include "Assets/FileManager.h"
#include "Core/CMakeConfig.h"
#include "Core/Logger.h"
#include "Graphics/Material.h"
#include "Math/iVec2.h"
#include <ranges>
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Tools/Image/TextureUtils.h"

namespace Real::assets {

    TextureManager::TextureManager() {
        InitDefaults();
    }

    void TextureManager::Register(const UUID& uuid, Ref<platform::opengl::OpenGLTexture> texture) {
        if (m_Textures.contains(uuid)) return;
        texture->SetID(uuid);
        m_Textures[uuid] = texture;
        m_PendingUpload.push_back(texture);
    }

    void TextureManager::DeleteCPU(const UUID& uuid) {
        m_Textures.erase(uuid);
    }

    const Ref<platform::opengl::OpenGLTexture>& TextureManager::GetTexture(
        const UUID& uuid, TextureType type)
    {
        const auto it = m_Textures.find(uuid);
        if (it != m_Textures.end() && it->second)
            return it->second;

        return GetOrCreateDefault(type);
    }

    Ref<platform::opengl::OpenGLTexture>& TextureManager::GetOrCreateDefault(TextureType type) {
        if (m_DefaultTextures.contains(type))
            return m_DefaultTextures[type];

        const int channelCount = util::texture::TextureTypeToChannelCount(type);

        // Default values per type
        u8 r = 255, g = 255, b = 255, a = 255;
        switch (type) {
            case TextureType::ALBEDO:            r=128; g=128; b=128; a=255; break;
            case TextureType::NORMAL:            r=128; g=128; b=255; a=255; break;
            case TextureType::EMISSIVE:          r=0;   g=0;   b=0;   a=255; break;
            case TextureType::ROUGHNESS:
            case TextureType::AMBIENT_OCCLUSION: r=255; g=0;   b=0;   a=0;   break;
            case TextureType::METALLIC:
            case TextureType::HEIGHT:            r=0;   g=0;   b=0;   a=0;   break;
            case TextureType::ALPHA:             r=255; g=255; b=255; a=255; break;

            case TextureType::ORM:  // Occlusion, Roughness, Metallic packed
                // Default neutral values:
                // R (Occlusion) = 255 (full occlusion)
                // G (Roughness) = 255 (fully rough)
                // B (Metallic)  = 0   (non-metallic)
                r=255; g=255; b=0; a=255; break;

            default:                             r=255; g=255; b=255; a=255; break;
        }

        // Heap allocated - CleanUpCPUData will delete[] it
        const int imageSize = channelCount;  // 1x1
        auto* imageData = new u8[imageSize];
        const u8 pixel[4] = { r, g, b, a };
        memcpy(imageData, pixel, imageSize);

        TextureData data{};
        data.data         = imageData;
        data.channelCount = channelCount;
        data.dataSize     = imageSize;
        data.width        = 1;
        data.height       = 1;

        auto tex = CreateRef<platform::opengl::OpenGLTexture>();
        tex->SetOrigin(platform::opengl::OpenGLTexture::TextureOrigin::Generated);
        tex->SetImageFormatState(ImageFormatState::DEFAULT);
        tex->CreateFromData(data, type);

        m_Textures[tex->GetUUID()] = tex;
        return m_DefaultTextures[type] = tex;
    }

    bool TextureManager::IsCompressed(const String& stem) const {
        return fs::File::Exists(String(ASSETS_DIR) + "textures/compressed/" + stem + ".dds");
    }

    // Call this on the main thread after loading textures on any thread
    Vector<BindlessHandle> TextureManager::FlushPendingUploads() {
        Vector<BindlessHandle> newHandles;
        newHandles.reserve(m_PendingUpload.size());

        for (auto& tex : m_PendingUpload) {
            if (!tex->IsReadyForUpload()) {
                Warn("[FlushPendingUploads] Not ready: " + tex->GetDebugName());
                continue;
            }
            tex->UploadToGPU();
            tex->SetGPUIndex(static_cast<GPUIndex>(m_BindlessHandles.size()));
            m_BindlessHandles.push_back(tex->GetBindless());
            newHandles.push_back(tex->GetBindless());
        }

        m_PendingUpload.clear();
        return newHandles;
    }

    size_t TextureManager::GetNextBindlessIndex() const {
        return m_BindlessHandles.size();
    }

    Vector<Ref<platform::opengl::OpenGLTexture>> TextureManager::GetMaterialTextures(
        const Material* mat) const
    {
        Vector<Ref<platform::opengl::OpenGLTexture>> textures;

        auto tryAdd = [&](UUID id) {
            if (id == 0) return;
            const auto it = m_Textures.find(id);
            if (it != m_Textures.end() && it->second)
                textures.push_back(it->second);
        };

        tryAdd(mat->m_Albedo);
        tryAdd(mat->m_Normal);
        tryAdd(mat->m_ORM);
        tryAdd(mat->m_Height);
        tryAdd(mat->m_Emissive);
        return textures;
    }

    void TextureManager::Update() {}

    void TextureManager::InitDefaults() {
        // Pre-create all known default types at startup
        constexpr TextureType types[] = {
            TextureType::ALBEDO,
            TextureType::NORMAL,
            TextureType::AMBIENT_OCCLUSION,
            TextureType::ROUGHNESS,
            TextureType::METALLIC,
            TextureType::ORM,
            TextureType::HEIGHT,
            TextureType::EMISSIVE,
            TextureType::ALPHA,
        };
        for (const auto t : types)
            GetOrCreateDefault(t);
    }
}