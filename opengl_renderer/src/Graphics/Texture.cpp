//
// Created by pointerlost on 10/12/25.
//
#include "Graphics/Texture.h"
#include <stb_image_resize2.h>
#include <utility>
#include <GL/glext.h>

#include "Core/AssetManager.h"
#include "Core/CmakeConfig.h"
#include "Core/Logger.h"
#include "Math/Math.h"
#include "stb/stb_image.h"
#include "Tools/ImageTools.h"
#include "Util/Util.h"

namespace Real {

    OpenGLTexture::OpenGLTexture(ImageFormatState format, TextureType type)
        : m_ImageFormatState(format), m_Type(type)
    {
    }

    void OpenGLTexture::AddLevelData(TextureData data, int mipLevel) {
        if (mipLevel >= m_MipLevelsData.size() || mipLevel < 0 || m_MipLevelsData.empty()) {
            Warn("[AddLevelData] mipLevel mismatch!");
            return;
        }
        m_MipLevelsData[mipLevel] = data;
    }

    void OpenGLTexture::SetLevelData(void *data, int mipLevel) {
        if (mipLevel >= m_MipLevelsData.size() || mipLevel < 0 || m_MipLevelsData.empty()) {
            Warn("[SetLevelData] mipLevel mismatch!");
            return;
        }
        m_MipLevelsData[mipLevel].m_Data = data;
    }

    void OpenGLTexture::SetFileInfo(FileInfo info) {
        m_FileInfo = std::move(info);
    }

    void OpenGLTexture::SetType(TextureType type) {
        m_Type = type;
    }

    void OpenGLTexture::SetIndex(int idx) {
        m_Index = idx;
    }

    void OpenGLTexture::SetFormat(int format, int mipLevel) {
        m_MipLevelsData[mipLevel].m_Format = format;
    }

    void OpenGLTexture::SetChannelCount(int count, int mipLevel) {
        m_MipLevelsData[mipLevel].m_ChannelCount = count;
    }

    void OpenGLTexture::SetImageFormatState(ImageFormatState format) {
        m_ImageFormatState = format;
    }

    void OpenGLTexture::SetInternalFormat(int format, int mipLevel) {
        m_MipLevelsData[mipLevel].m_InternalFormat = format;
    }

    void OpenGLTexture::SetResolution(const glm::ivec2 &res, int mipLevel) {
        m_MipLevelsData[mipLevel].m_Width  = res.x;
        m_MipLevelsData[mipLevel].m_Height = res.y;
    }

    void OpenGLTexture::SetDataSize(int size, int mipLevel) {
        m_MipLevelsData[mipLevel].m_DataSize = size;
    }

    void OpenGLTexture::SetTextureParameters() {
        glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, (int)util::TextureFilterModeToGLEnum(m_FilterMode));
        glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, (int)util::TextureFilterModeToGLEnum(m_FilterMode));
        glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S,     (int)util::TextureWrapModeToGLEnum(m_WrapMode));
        glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T,     (int)util::TextureWrapModeToGLEnum(m_WrapMode));
    }

    void OpenGLTexture::SetWrapMode(TextureWrapMode mode) {
        m_WrapMode = mode;
    }

    void OpenGLTexture::SetFilterMode(TextureFilterMode mode) {
        m_FilterMode = mode;
    }

    void OpenGLTexture::SetMipLevelsData(const std::vector<TextureData> &mipLevels) {
        CreateMipmaps(mipLevels);
    }

    std::pair<int, int> OpenGLTexture::GetResolution(int mipLevel) {
        if (mipLevel < 0 || (mipLevel >= m_MipLevelsData.size() && !m_MipLevelsData.empty()))
            Warn("MipLevel index mismatch!!! name: " + m_FileInfo.name);
        return std::make_pair(m_MipLevelsData[mipLevel].m_Width, m_MipLevelsData[mipLevel].m_Height);
    }

    TextureData& OpenGLTexture::GetLevelData(int mipLevel) {
        if (mipLevel < 0 || (mipLevel >= m_MipLevelsData.size() && !m_MipLevelsData.empty()))
            Warn("MipLevel index mismatch!!! name: " + m_FileInfo.name);
        return m_MipLevelsData[mipLevel];
    }

    TextureData OpenGLTexture::LoadFromFile(const std::string &path) {
        if (!File::Exists(path)) {
            Warn("File path can't find! path: " + path);
            return {};
        }
        TextureData data;
        data.m_Data = stbi_load(path.c_str(), &data.m_Width, &data.m_Height, &data.m_ChannelCount, 0);
        if (data.m_ChannelCount == 3) {
            const auto pixelCount = data.m_Width * data.m_Height;
            const auto* rawData = static_cast<uint8_t*>(data.m_Data);
            auto* rgbaRawData = new uint8_t[pixelCount * 4];

            for (size_t i = 0; i < pixelCount; i++) {
                rgbaRawData[i * 4 + 0] = rawData[i * 3 + 0];
                rgbaRawData[i * 4 + 1] = rawData[i * 3 + 1];
                rgbaRawData[i * 4 + 2] = rawData[i * 3 + 2];
                rgbaRawData[i * 4 + 3] = 255;
            }

            stbi_image_free(data.m_Data);
            data.m_Data = rgbaRawData;
            data.m_ChannelCount = 4;
        }
        // DataSize = TexPixelCount * ChannelCount * Byte-Per-Channel
        data.m_DataSize = data.m_Width * data.m_Height * data.m_ChannelCount * 1;
        return data;
    }

    void OpenGLTexture::Create() {
        if (m_Handle == 0) {
            CreateHandle();
        }
        const auto data = LoadFromFile(m_FileInfo.path);
        CreateFromData(data, m_Type);
    }

    void OpenGLTexture::CreateFromData(TextureData data, TextureType type) {
        if (m_Handle == 0) {
            CreateHandle();
        }
        m_Type = type;
        data.m_Format = util::ConvertChannelCountToGLFormat(data.m_ChannelCount);
        data.m_InternalFormat = util::ConvertChannelCountToGLInternalFormat(data.m_ChannelCount);
        CreateMipmaps(data);
    }

    void OpenGLTexture::PrepareOptionsAndUploadToGPU() {
        // We are packing these textures into nrChannels, so we don't need to send to GPU
        if (m_Type == TextureType::RGH || m_Type == TextureType::MTL || m_Type == TextureType::AO) return;

        UploadMipLevels();
        SetTextureParameters();
        CreateBindlessAndMakeResident();
    }

    void OpenGLTexture::CreateHandle() {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);
        if (m_Handle == 0) {
            Warn("Texture can't created! :" + m_FileInfo.name);
        }
    }

    void OpenGLTexture::CreateBindlessAndMakeResident() {
        // Create Texture bindless handle
        if (m_Handle == 0) {
            Warn("Texture handle is not exists!");
            return;
        }
        m_BindlessHandleID = glGetTextureHandleARB(m_Handle);
        if (m_BindlessHandleID == 0) {
            Warn("Bindless Handle can't created! tex name: " + m_FileInfo.name);
        }
        MakeResident(m_BindlessHandleID);
    }

    void OpenGLTexture::UploadMipLevels() {
        if (m_MipLevelCount > m_MipLevelsData.size() || m_MipLevelCount < 0) {
            Warn("Mipmap level mismatch!!! name: " + m_FileInfo.name);
            return;
        }

        // Allocate enough memory for all the mip levels
        glTextureStorage2D(m_Handle, m_MipLevelCount, m_MipLevelsData[0].m_InternalFormat,
            m_MipLevelsData[0].m_Width, m_MipLevelsData[0].m_Height
        );

        for (int lvl = 0; lvl < m_MipLevelCount; lvl++) {
            auto& data = m_MipLevelsData[lvl];
            glCompressedTextureSubImage2D(m_Handle, lvl, 0, 0, data.m_Width, data.m_Height,
                data.m_InternalFormat, (int)data.m_DataSize, data.m_Data
            );
            if (data.m_Data) {
                data.m_Data = nullptr;
                stbi_image_free(data.m_Data);
            }
        }
    }

    void OpenGLTexture::CreateMipmaps(const std::vector<TextureData> &levelsData) {
        if (levelsData.empty()) {
            Warn("Texture data empty!");
            return;
        }

        m_MipLevelCount = (int)levelsData.size();
        if (m_MipLevelCount < 1) {
            Warn("mip levels data is empty!");
        }
        m_MipLevelsData.resize(m_MipLevelCount);
        m_MipLevelsData = levelsData;
    }

    void OpenGLTexture::CreateMipmaps(const TextureData &data) {
        auto width  = data.m_Width;
        auto height = data.m_Height;

        m_MipLevelCount = CalculateMaxMipMapLevels(width, height);
        m_MipLevelsData.resize(m_MipLevelCount, data);

        // We are starting from level=1, so shit for once at the beginning
        width  = math::FindMax(1u, width  >> 1);
        height = math::FindMax(1u, height >> 1);

        for (size_t level = 1; level < m_MipLevelCount; level++) {
            m_MipLevelsData[level].m_Width  = width;
            m_MipLevelsData[level].m_Height = height;
            m_MipLevelsData[level].m_DataSize = width * height * m_MipLevelsData[0].m_ChannelCount;

            width  = math::FindMax(1u, width  >> 1);
            height = math::FindMax(1u, height >> 1);
        }
    }

    int OpenGLTexture::CalculateMaxMipMapLevels(int width, int height) {
        if (width == 0 || height == 0) {
            Warn("Width or Height can't be equal to zero for the mip level calculation!");
            return {};
        }
        const int maxDimension = math::FindMax(width, height);
        return static_cast<int>(std::log2(maxDimension)) + 1;
    }

    int OpenGLTexture::CalculateMaxMipMapLevels(const glm::ivec2 &res) {
        return CalculateMaxMipMapLevels(res.x, res.y);
    }

    void OpenGLTexture::Resize(const glm::ivec2& resolution, int mipLevel, bool srgbSpace) {
        // Pick a channel flag for Texture resizing
        if (resolution.x == 0 || resolution.y == 0) {
            Warn("Resolution mismatch for: " + m_FileInfo.name);
            return;
        }
        if (m_MipLevelsData.empty())
            Warn("There is no mipmap!! name: " + m_FileInfo.name);
        auto& data = m_MipLevelsData[mipLevel];

        stbir_pixel_layout channelFlag;
        switch (m_MipLevelsData[mipLevel].m_ChannelCount) {
            case 1: channelFlag = STBIR_1CHANNEL; break;
            case 2: channelFlag = STBIR_2CHANNEL; break;
            case 4: channelFlag = STBIR_RGBA;     break;
            default: channelFlag = STBIR_RGB;
        }

        const auto& rawData = data.m_Data;

        auto* tempData = new uint8_t[data.m_DataSize];
        memcpy(tempData, data.m_Data, data.m_DataSize);

        if (srgbSpace) {
            data.m_Data = stbir_resize_uint8_srgb(tempData, data.m_Width, data.m_Height, 0,
                static_cast<unsigned char*>(rawData), resolution.x, resolution.y, 0, channelFlag
            );
        } else {
            data.m_Data = stbir_resize_uint8_linear(tempData, data.m_Width, data.m_Height, 0,
                static_cast<unsigned char*>(rawData), resolution.x, resolution.y, 0, channelFlag
            );
        }
        delete[] tempData;

        data.m_Width  = resolution.x;
        data.m_Height = resolution.y;
    }

    void OpenGLTexture::MakeResident(GLuint id) const {
        glMakeTextureHandleResidentARB(id);
    }

    void OpenGLTexture::MakeNonResident(GLuint id) const {
        if (glIsTextureHandleResidentARB(id)) {
            glMakeTextureHandleNonResidentARB(id);
        }
    }
}
