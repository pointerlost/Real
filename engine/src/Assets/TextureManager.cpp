//
// Created by pointerlost on 3/23/26.
//
#include "Assets/TextureManager.h"
#include "stb/stb_image.h"
#include "Assets/FileManager.h"
#include "Core/CMakeConfig.h"
#include "Core/Logger.h"
#include "Graphics/Material.h"
#include "Graphics/Texture/Texture.h"
#include "Math/iVec2.h"
#include <ranges>

#include "Assets/TextureUtils.h"
#include "Platform/OpenGL/OpenGLUtils.h"

namespace Real::assets {

    TextureManager::TextureManager() {
        LoadDefaultTextures();
    }

    void TextureManager::Register(const Ref<platform::opengl::OpenGLTexture>& tex) {
        if (!m_Textures.contains(tex->GetUUID()))
            m_Textures.emplace(tex->GetUUID(), tex);
    }

    void TextureManager::DeleteCPU(const UUID& uuid) {
        if (m_Textures.contains(uuid))
            m_Textures.erase(uuid);
    }

    graphics::TextureData TextureManager::LoadFromFile(const String& path, TextureType type) {
        if (!fs::File::Exists(path)) { Warn("There is no texture: " + path); }

        const int desiredChannels = type != TextureType::UNDEFINED
            ? util::texture::TextureTypeToChannelCount(type)
            : 0;

        graphics::TextureData data;
        data.data           = stbi_load(path.c_str(), &data.width, &data.height, &data.channelCount, desiredChannels);
        data.channelCount   = desiredChannels != 0 ? desiredChannels : data.channelCount;
        data.dataSize       = data.width * data.height * data.channelCount * 1;
        data.format         = util::opengl::GetGLFormat(data.channelCount);
        data.internalFormat = util::opengl::GetGLInternalFormat(data.channelCount);

        if (!data.data)             { Warn("[LoadFromFile] stbi_load returned nullptr!"); }
        if (data.channelCount == 0) { Warn("[LoadFromFile] channel count is 0!");         }

        return data;
    }

    const Ref<platform::opengl::OpenGLTexture>& TextureManager::GetTexture(const UUID& uuid, TextureType type) {
        if (!m_Textures.contains(uuid)) {
            const auto& tex = GetOrCreateDefault(type);
            m_Textures[tex->GetUUID()] = tex;
            return tex;
        }
        return m_Textures[uuid];
    }

    Ref<platform::opengl::OpenGLTexture>& TextureManager::GetOrCreateDefault(TextureType type) {
        if (m_DefaultTextures.contains(type))
            return m_DefaultTextures[type];

        const auto channelCount = util::texture::TextureTypeToChannelCount(type);
        constexpr math::iVec2 resolution{1, 1};

        const Ref<platform::opengl::OpenGLTexture> defaultTex = CreateRef<platform::opengl::OpenGLTexture>();

        u8 channelColor[4] = {UINT8_MAX};
        switch (type) {
            case TextureType::ALBEDO:
                channelColor[0] = 128; channelColor[1] = 128;
                channelColor[2] = 128; channelColor[3] = 255;
                break;
            case TextureType::NORMAL:
                channelColor[0] = 128; channelColor[1] = 128;
                channelColor[2] = 255; channelColor[3] = 255;
                break;
            case TextureType::EMISSIVE:
                channelColor[0] = 0; channelColor[1] = 0;
                channelColor[2] = 0; channelColor[3] = 255;
                break;
            case TextureType::ROUGHNESS:
            case TextureType::AMBIENT_OCCLUSION:
                channelColor[0] = 255;
                break;
            case TextureType::METALLIC:
            case TextureType::HEIGHT:
                channelColor[0] = 0;
                break;
            default:
                channelColor[0] = UINT8_MAX; channelColor[1] = UINT8_MAX;
                channelColor[2] = UINT8_MAX; channelColor[3] = UINT8_MAX;
        }

        const auto imageSize = resolution.x * resolution.y * channelCount;
        graphics::TextureData data;
        data.data = new u8[imageSize];
        auto* imageData = static_cast<u8*>(data.data);

        switch (channelCount) {
            case 1:
                for (size_t i = 0; i < imageSize; i += channelCount)
                    imageData[i] = channelColor[0];
                break;
            case 2:
                for (size_t i = 0; i < imageSize; i += channelCount) {
                    imageData[i + 0] = channelColor[0];
                    imageData[i + 1] = channelColor[1];
                }
                break;
            case 3:
            case 4:
                for (size_t i = 0; i < imageSize; i += channelCount) {
                    imageData[i + 0] = channelColor[0];
                    imageData[i + 1] = channelColor[1];
                    imageData[i + 2] = channelColor[2];
                    imageData[i + 3] = channelColor[3];
                }
                break;
            default:
                Warn("Channel count mismatch!");
        }

        data.channelCount   = channelCount;
        data.dataSize       = imageSize;
        data.width          = resolution.x;
        data.height         = resolution.y;
        data.format         = util::opengl::GetGLFormat(channelCount);
        data.internalFormat = util::opengl::GetGLInternalFormat(channelCount);

        defaultTex->SetImageFormatState(ImageFormatState::DEFAULT);
        defaultTex->CreateFromData(data, type);
        m_Textures[defaultTex->GetUUID()] = defaultTex;
        return m_DefaultTextures[type] = defaultTex;
    }

    bool TextureManager::IsCompressed(const String& stem) const {
        // TODO: Need to find a better way to improve performance. This looks bad.
        return fs::File::Exists(String(ASSETS_DIR) + "textures/compressed/" + stem + ".dds");
    }

    Vector<graphics::BindlessHandle> TextureManager::UploadToGPU() {
        Vector<graphics::BindlessHandle> bindlessIDs;
        for (const auto& tex : std::views::values(m_Textures)) {
            if (tex->GetImageFormatState() == ImageFormatState::DEFAULT) continue;
            tex->PrepareOptionsAndUploadToGPU();
            tex->SetIndex(bindlessIDs.size());
            bindlessIDs.push_back(tex->GetBindlessHandle());
        }
        return bindlessIDs;
    }

    size_t TextureManager::GetNextBindlessIndex() const {
        return m_BindlessHandles.size();
    }

    Vector<Ref<platform::opengl::OpenGLTexture>> TextureManager::GetMaterialTextures(const graphics::Material* mat) const
    {
        Vector<Ref<platform::opengl::OpenGLTexture>> textures;

        auto tryAdd = [this](const UUID id, Vector<Ref<platform::opengl::OpenGLTexture>>& out) {
            if (id == 0) return;
            const auto it = m_Textures.find(id);
            if (it != m_Textures.end() && it->second)
                out.push_back(it->second);
        };

        tryAdd(mat->m_Albedo,   textures);
        tryAdd(mat->m_Normal,   textures);
        tryAdd(mat->m_ORM,      textures);
        tryAdd(mat->m_Height,   textures);
        tryAdd(mat->m_Emissive, textures);

        return textures;
    }

    void TextureManager::Update() {
    }

    void TextureManager::LoadDefaultTextures() {
        for (int i = 0; i <= static_cast<int>(TextureType::EMISSIVE); i++)
            GetOrCreateDefault(static_cast<TextureType>(i));
    }

}
