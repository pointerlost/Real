//
// Created by pointerlost on 10/12/25.
//
#include "Graphics/Texture.h"

#include <stb_image_resize2.h>
#include <utility>

#include "Core/AssetManager.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Math/Math.h"
#include "stb/stb_image.h"
#include "Tools/ImageTools.h"
#include "Util/Util.h"

namespace Real {

    OpenGLTexture::OpenGLTexture(ImageFormatState format, bool isDefaultTex)
        : m_IsDefault(isDefaultTex), m_ImageFormatState(format)
    {
    }

    OpenGLTexture::OpenGLTexture(TextureType type, bool isDefaultTex)
        : m_IsDefault(isDefaultTex), m_Type(type)
    {
    }

    void OpenGLTexture::AddLevelData(TextureData data) {
        m_MipLevelDatas.push_back(data);
    }

    void OpenGLTexture::SetLevelData(void *data, int mipLevel) {
        m_MipLevelDatas[mipLevel].m_Data = data;
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

    void OpenGLTexture::SetFormat(GLenum format, int mipLevel) {
        m_MipLevelDatas[mipLevel].m_Format = format;
    }

    void OpenGLTexture::SetChannelCount(int count, int mipLevel) {
        m_MipLevelDatas[mipLevel].m_ChannelCount = count;
    }

    void OpenGLTexture::SetImageFormat(ImageFormatState format) {
        m_ImageFormatState = format;
    }

    void OpenGLTexture::SetInternalFormat(GLenum format, int mipLevel) {
        m_MipLevelDatas[mipLevel].m_InternalFormat = format;
    }

    void OpenGLTexture::SetResolution(const glm::ivec2 &res, int mipLevel) {
        m_MipLevelDatas[mipLevel].m_Width  = res.x;
        m_MipLevelDatas[mipLevel].m_Height = res.y;
    }

    void OpenGLTexture::SetDataSize(int size, int mipLevel) {
        m_MipLevelDatas[mipLevel].m_DataSize = size;
    }

    void OpenGLTexture::SetTextureParameters(GLenum target) {
        glTextureParameteri(target, GL_TEXTURE_MIN_FILTER, (int)util::TextureWrapModeToGLEnum(m_WrapMode));
        glTextureParameteri(target, GL_TEXTURE_MAG_FILTER, (int)util::TextureFilterModeToGLEnum(m_FilterMode));
        glTextureParameteri(target, GL_TEXTURE_WRAP_S,     (int)util::TextureWrapModeToGLEnum(m_WrapMode));
        glTextureParameteri(target, GL_TEXTURE_WRAP_T,     (int)util::TextureWrapModeToGLEnum(m_WrapMode));
    }

    void OpenGLTexture::SetWrapMode(TextureWrapMode mode) {
        m_WrapMode = mode;
    }

    void OpenGLTexture::SetFilterMode(TextureFilterMode mode) {
        m_FilterMode = mode;
    }

    void OpenGLTexture::SetCompressionType(ImageCompressedType type) {
        m_ImageCompressType = type;
    }

    std::pair<int, int> OpenGLTexture::GetResolution(int mipLevel) const {
        if (mipLevel < 0 || mipLevel >= m_MipLevelDatas.size())
            Warn("MipLevel index mismatch!!! name: " + m_FileInfo.name);
        return std::make_pair(m_MipLevelDatas[mipLevel].m_Width, m_MipLevelDatas[mipLevel].m_Height);
    }

    TextureData& OpenGLTexture::GetLevelData(int mipLevel) {
        if (mipLevel < 0 || mipLevel >= m_MipLevelDatas.size())
            Warn("MipLevel index mismatch!!! name: " + m_FileInfo.name);
        return mipLevel > m_MipLevelDatas.size() ? m_MipLevelDatas[0] : m_MipLevelDatas[mipLevel];
    }

    TextureData OpenGLTexture::LoadFromFile(const std::string &path) {
        TextureData data;
        data.m_Data = stbi_load(path.c_str(), &data.m_Width, &data.m_Height, &data.m_ChannelCount, 0);
        // DataSize = TexPixelCount * ChannelCount * Byte-Per-Channel
        data.m_DataSize = data.m_Width * data.m_Height * data.m_ChannelCount * 1;
        return data;
    }

    void OpenGLTexture::Create() {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);
        if (m_Handle == 0) {
            Warn("Texture can't created! :" + m_FileInfo.name);
            return;
        }

        if (m_ImageFormatState != ImageFormatState::COMPRESSED) {
            const auto texData = LoadFromFile(m_FileInfo.path);
            m_MipLevelCount = CalculateMipMapLevels(texData.m_Width, texData.m_Height);
            CreateMipMaps(texData.m_Width, texData.m_Height);
            // Set the first data to the first level of the mipmap
            m_MipLevelDatas[0].m_Width    = texData.m_Width;
            m_MipLevelDatas[0].m_Height   = texData.m_Height;
            m_MipLevelDatas[0].m_Data     = texData.m_Data;
            m_MipLevelDatas[0].m_DataSize = texData.m_DataSize;
            m_MipLevelDatas[0].m_ChannelCount = texData.m_ChannelCount;
        } else {
            const std::vector<TextureData> mipLevels = tools::ReadCompressedDataFromDDSFile(this);
            m_MipLevelCount = CalculateMipMapLevels(mipLevels[0].m_Width, mipLevels[0].m_Height);
            if (mipLevels.size() != m_MipLevelCount) {
                Warn("Mipmap level mismatch!!! name: " + m_FileInfo.name);
                return;
            }
        }

        // Allocate enough memory for all the mip levels
        glTextureStorage2D(m_Handle, m_MipLevelCount, util::ImageCompressTypeToGLEnum(m_ImageCompressType),
            m_MipLevelDatas[0].m_Width, m_MipLevelDatas[0].m_Height
        );

        UploadMipLevels(m_ImageFormatState);
        SetTextureParameters(GL_TEXTURE_2D);
        CreateBindlessAndMakeResident();
    }

    void OpenGLTexture::CreateBindlessAndMakeResident() {
        // Create Texture bindless handle
        m_BindlessHandleID = glGetTextureHandleARB(m_Handle);
        if (m_BindlessHandleID == 0) {
            Warn("Bindless Handle can't created! tex name: " + m_FileInfo.name);
            return;
        }
        MakeResident(m_BindlessHandleID);
    }

    void OpenGLTexture::UploadMipLevels(ImageFormatState state) {
        if (m_MipLevelCount > m_MipLevelDatas.size() || m_MipLevelCount < 0) {
            Warn("Mipmap level mismatch!!! name: " + m_FileInfo.name);
            return;
        }

        if (state == ImageFormatState::COMPRESSED) {
            for (int lvl = 0; lvl < m_MipLevelCount; lvl++) {
                auto& data = m_MipLevelDatas[lvl];
                glCompressedTextureSubImage2D(m_Handle, lvl, 0, 0, data.m_Width, data.m_Height,
                    util::ImageCompressTypeToGLEnum(m_ImageCompressType), data.m_DataSize, data.m_Data
                );
                stbi_image_free(data.m_Data);
                data.m_Data = nullptr;
            }
        }
        else if (state == ImageFormatState::UNCOMPRESSED) {
            // The first data set is ready, so skip it
            for (size_t level = 1; level < m_MipLevelCount; level++) {
                auto& data = m_MipLevelDatas[level];
                glTextureSubImage2D(m_Handle, (int)level, 0, 0,
                    data.m_Width, data.m_Height, data.m_Format, GL_UNSIGNED_BYTE, data.m_Data
                );
                // Clean up VRAM
                stbi_image_free(data.m_Data);
                data.m_Data = nullptr;
            }
        }
        else {
            Warn("ImageFormatState::UNDEFINED!!");
        }
    }

    void OpenGLTexture::CreateMipMaps(int width, int height) {
        m_MipLevelCount = CalculateMipMapLevels(width, height);
        if (m_MipLevelCount <= 0) {
            Warn("Mipmap levels creation failed!");
            return;
        }

        for (size_t level = 0; level < m_MipLevelCount; level++) {
            m_MipLevelDatas[level].m_Width  = width;
            m_MipLevelDatas[level].m_Height = height;

            // Set next mip level
            width  = width  >> level;
            height = height >> level;

            math::FindMax(1u, width);
            math::FindMax(1u, height);
        }
    }

    int OpenGLTexture::CalculateMipMapLevels(int width, int height) {
        const int maxDimension = math::FindMax(width, height);
        return static_cast<int>(std::log2(maxDimension)) + 1;
    }

    int OpenGLTexture::CalculateMipMapLevels(const glm::ivec2 &res) {
        return CalculateMipMapLevels(res.x, res.y);
    }

    void OpenGLTexture::Resize(const glm::ivec2& resolution, int mipLevel, bool srgbSpace) {
        // Pick a channel flag for Texture resizing
        if (resolution.x == 0 || resolution.y == 0) {
            Warn("Resolution mismatch for: " + m_FileInfo.name);
            return;
        }
        if (m_MipLevelDatas.empty())
            Warn("There is no mipmap!! name: " + m_FileInfo.name);
        auto& data = m_MipLevelDatas[mipLevel];

        stbir_pixel_layout channelFlag;
        switch (m_MipLevelDatas[mipLevel].m_ChannelCount) {
            case 1:
                channelFlag = STBIR_1CHANNEL;
                break;
            case 2:
                channelFlag = STBIR_2CHANNEL;
                break;
            case 4:
                channelFlag = STBIR_RGBA;
                break;

            default: // channelCount = 3 = default case
                channelFlag = STBIR_RGB;
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

        data.m_Width = resolution.x;
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
