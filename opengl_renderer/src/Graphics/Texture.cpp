//
// Created by pointerlost on 10/12/25.
//
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "Graphics/Texture.h"
#include <utility>
#include "Core/AssetManager.h"
#include "Core/Logger.h"
#include "stb/stb_image.h"
#include <stb_image_resize2.h>

#include "Core/file_manager.h"
#include "Tools/ImageTools.h"
#include "Util/Util.h"

namespace Real {

    OpenGLTexture::OpenGLTexture(const TextureData &data, bool isSTBAllocated, TextureType type,
        ImageFormatState image_state, FileInfo info, UUID uuid)
        : m_UUID(uuid), m_IsSTBAllocated(isSTBAllocated), m_ImageFormatState(image_state), m_FileInfo(std::move(info))
    {
        CreateFromData(data, type);
    }

    OpenGLTexture::OpenGLTexture(const std::vector<TextureData> &data) {
        CreateMipmapsFromDDS(data);
    }

    OpenGLTexture::OpenGLTexture(FileInfo fileinfo, bool isSTBAllocated, ImageFormatState imagestate)
        : m_IsSTBAllocated(isSTBAllocated), m_ImageFormatState(imagestate), m_FileInfo(std::move(fileinfo)) {}

    OpenGLTexture::OpenGLTexture(bool isSTBAllocated, TextureType type)
        : m_IsSTBAllocated(isSTBAllocated), m_Type(type) {}

    OpenGLTexture::~OpenGLTexture() {
        CleanUpCPUData(); // Clean if it has not already been cleaned

        if (m_Handle != 0) {
            glDeleteTextures(1, &m_Handle);
        }
    }

    void OpenGLTexture::AddLevelData(const TextureData &data, int mipLevel) {
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
        m_GPUIndex = idx;
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
        CreateMipmapsFromDDS(mipLevels);
    }

    void OpenGLTexture::SetUUID(uint64_t uuid) {
        m_UUID = UUID(uuid);
    }

    void OpenGLTexture::SetUUID(const UUID &uuid) {
        m_UUID = uuid;
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
        if (!fs::File::Exists(path)) {
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
        data.m_Format   = util::GetGLFormat(data.m_ChannelCount);
        data.m_InternalFormat = util::GetGLInternalFormat(data.m_ChannelCount);
        m_IsSTBAllocated = true;
        return data;
    }

    void OpenGLTexture::Create() {
        if (m_Handle == 0) {
            CreateHandle();
        }
        const auto data = LoadFromFile(m_FileInfo.path);
        CreateFromData(data, m_Type);
    }

    void OpenGLTexture::CreateFromData(const TextureData &data, TextureType type) {
        if (m_Handle == 0) {
            CreateHandle();
        }
        m_Type = type;
        // One mip level is enough for CPU-generated textures
        m_MipLevelsData.push_back(data);
        if (m_ImageFormatState == ImageFormatState::COMPRESS_ME || m_ImageFormatState == ImageFormatState::COMPRESSED) {
            m_MipLevelsData[0].m_InternalFormat = util::GetCompressedInternalFormat(m_MipLevelsData[0].m_ChannelCount);
        } else {
            m_MipLevelsData[0].m_InternalFormat = util::GetGLInternalFormat(m_MipLevelsData[0].m_ChannelCount);
        }
        m_MipLevelsData[0].m_Format = util::GetGLFormat(m_MipLevelsData[0].m_ChannelCount);
    }

    void OpenGLTexture::CleanUpCPUData() {
        for (auto& level : m_MipLevelsData) {
            if (level.m_Data) {
                if (m_IsSTBAllocated) {
                    stbi_image_free(level.m_Data);
                } else {
                    delete[] static_cast<uint8_t*>(level.m_Data);
                }
                level.m_Data = nullptr;
            }
        }
    }

    void OpenGLTexture::PrepareOptionsAndUploadToGPU() {
        UploadMipLevels();
        SetTextureParameters();
        CreateBindless();
        MakeResident();
        // Clean the texture data after uploading it to the GPU
        CleanUpCPUData();
    }

    void OpenGLTexture::CreateHandle() {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);
        if (m_Handle == 0) {
            Warn("Texture can't created! :" + m_FileInfo.name);
        }
    }

    void OpenGLTexture::CreateBindless() {
        // Create Texture bindless handle
        if (m_Handle == 0) {
            Warn("Texture handle is not exists!");
            return;
        }
        if (m_BindlessHandleID == 0) {
            if (GLAD_GL_ARB_bindless_texture && GLAD_GL_ARB_gpu_shader_int64) {
                m_BindlessHandleID = glGetTextureHandleARB(m_Handle);
                if (m_BindlessHandleID == 0) {
                    Warn("Bindless Handle can't created! tex name: " + m_FileInfo.name);
                }
            }
        }
    }

    void OpenGLTexture::UploadMipLevels() {
        if (m_MipLevelCount > m_MipLevelsData.size() || m_MipLevelCount < 0) {
            Warn("Mipmap level mismatch!!! name: " + m_FileInfo.name);
            return;
        }

        switch (m_ImageFormatState) {
            case ImageFormatState::COMPRESS_ME:
                tools::CompressTextureAndReadFromFile(this);
                // Don't break the switch statement and load compressed state!

            case ImageFormatState::COMPRESSED: {
                // Allocate enough memory for all the mip levels
                glTextureStorage2D(m_Handle, m_MipLevelCount, m_MipLevelsData[0].m_InternalFormat,
                    m_MipLevelsData[0].m_Width, m_MipLevelsData[0].m_Height
                );

                for (int lvl = 0; lvl < m_MipLevelCount; lvl++) {
                    const auto& data = m_MipLevelsData[lvl];
                    if (data.m_Width % 4 != 0 || data.m_Height % 4 != 0) {
                        Warn("Compressed mip level size mismatch, texture name: " + GetName());
                        break;
                    }
                    glCompressedTextureSubImage2D(m_Handle, lvl, 0, 0, data.m_Width, data.m_Height,
                        data.m_InternalFormat, (int)data.m_DataSize, data.m_Data
                    );
                }
            } break;

            case ImageFormatState::UNCOMPRESSED: {
                // Allocate memory for uncompressed data
                const auto& data = m_MipLevelsData[0];
                m_MipLevelCount = CalculateMaxMipMapLevels(data.m_Width, data.m_Height);

                // Allocate for all the mip levels
                glTextureStorage2D(m_Handle, m_MipLevelCount, data.m_InternalFormat, data.m_Width, data.m_Height);
                // Load first mip level data
                glTextureSubImage2D(m_Handle, 0, 0, 0, data.m_Width, data.m_Height, data.m_Format, GL_UNSIGNED_BYTE, data.m_Data);
                // Generate other mipmap levels
                glGenerateTextureMipmap(m_Handle);
            } break;

            case ImageFormatState::UNDEFINED: Warn("Texture format state is UNDEFINED!");
            default: ;
        }
    }

    void OpenGLTexture::CreateMipmapsFromDDS(const std::vector<TextureData> &levelsData) {
        if (levelsData.empty()) {
            Warn("Texture data empty!");
            return;
        }
        m_MipLevelsData.clear();

        m_MipLevelCount = (int)levelsData.size();
        if (m_MipLevelCount < 1) {
            Warn("mip levels data is empty!");
        }
        m_MipLevelsData.resize(m_MipLevelCount);
        m_MipLevelsData = levelsData;
    }

    int OpenGLTexture::CalculateMaxMipMapLevels(int width, int height) {
        if (m_ImageFormatState == ImageFormatState::COMPRESSED) {
            int levels = 1; // Level 0 (original)

            int w = width;
            int h = height;

            while (w > 4 || h > 4) {
                w = std::max(1, w >> 1);
                h = std::max(1, h >> 1);
                levels++;
            }
            return levels;
        }
        // Uncompressed state
        const int maxDimension = std::max(width, height);
        return static_cast<int>(std::floor(std::log2(maxDimension))) + 1;
    }

    int OpenGLTexture::CalculateMaxMipMapLevels(const glm::ivec2 &res) {
        return CalculateMaxMipMapLevels(res.x, res.y);
    }

    // TODO: we can use resizing for cubemaps to get same width and height (don't remove this shit for now!)
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
            case 1: channelFlag  = STBIR_1CHANNEL; break;
            case 2: channelFlag  = STBIR_2CHANNEL; break;
            case 4: channelFlag  = STBIR_RGBA;     break;
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

    void OpenGLTexture::MakeResident() const {
        if (m_BindlessHandleID != 0 && GLAD_GL_ARB_bindless_texture && GLAD_GL_ARB_gpu_shader_int64) {
            if (!glIsTextureHandleResidentARB(m_BindlessHandleID)) {
                glMakeTextureHandleResidentARB(m_BindlessHandleID);
                return;
            }
            Warn("Texture is already resident!");
            return;
        }

        Warn("There is no bindless handle! nor support for bindless handle!");
    }

    void OpenGLTexture::MakeNonResident() const {
        if (glIsTextureHandleResidentARB(m_BindlessHandleID)) {
            glMakeTextureHandleNonResidentARB(m_BindlessHandleID);
        }
    }

    bool OpenGLTexture::IsCPUGenerated() const {
        return !m_IsSTBAllocated;
    }
}
