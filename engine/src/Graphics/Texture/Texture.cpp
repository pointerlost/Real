//
// Created by pointerlost on 10/12/25.
//
#include "glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <stb_image_resize2.h>
#include "Graphics/Texture/Texture.h"
#include <utility>
#include "Assets/AssetManager.h"
#include "Core/Logger.h"
#include "Assets/FileManager.h"
#include "Graphics/Texture/TextureTypes.h"
#include "Math/iVec2.h"
#include "Platform/OpenGL/OpenGLUtils.h"
#include "Tools/ImageTools.h"

namespace Real::platform::opengl {

    OpenGLTexture::OpenGLTexture(const graphics::TextureData &data, bool isSTBAllocated, TextureType type,
        ImageFormatState image_state, fs::FileInfo info, UUID uuid)
        : m_UUID(uuid), m_IsSTBAllocated(isSTBAllocated), m_ImageFormatState(image_state), m_FileInfo(std::move(info))
    {
        CreateFromData(data, type);
    }

    OpenGLTexture::OpenGLTexture(const TextureAsset &asset) {
        // m_MipLevelsData = asset.cpuData;
    }

    OpenGLTexture::OpenGLTexture(const Vector<graphics::TextureData> &data, fs::FileInfo info) : m_FileInfo(std::move(info)) {
        CreateMipmapsFromDDS(data);
    }

    OpenGLTexture::OpenGLTexture(fs::FileInfo fileinfo, bool isSTBAllocated, ImageFormatState imagestate)
        : m_IsSTBAllocated(isSTBAllocated), m_ImageFormatState(imagestate), m_FileInfo(std::move(fileinfo))
    {
    }

    OpenGLTexture::OpenGLTexture(bool isSTBAllocated, TextureType type)
        : m_IsSTBAllocated(isSTBAllocated), m_Type(type)
    {
        // Don't create handle for default textures!!!!
    }

    OpenGLTexture::~OpenGLTexture() {
        CleanUpCPUData(); // Clean if it has not already been cleaned

        if (m_Handle.IsValid()) {
            glDeleteTextures(1, &m_Handle.value);
        }
    }

    void OpenGLTexture::AddLevelData(const graphics::TextureData &data, int mipLevel) {
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
        m_MipLevelsData[mipLevel].data = data;
    }

    void OpenGLTexture::SetFileInfo(fs::FileInfo info) {
        m_FileInfo = std::move(info);
    }

    void OpenGLTexture::SetType(TextureType type) {
        m_Type = type;
    }

    void OpenGLTexture::SetIndex(u32 idx) {
        m_GPUIndex = idx;
    }

    void OpenGLTexture::SetFormat(int format, int mipLevel) {
        m_MipLevelsData[mipLevel].format = format;
    }

    void OpenGLTexture::SetChannelCount(int count, int mipLevel) {
        m_MipLevelsData[mipLevel].channelCount = count;
    }

    void OpenGLTexture::SetImageFormatState(ImageFormatState format) {
        m_ImageFormatState = format;
    }

    void OpenGLTexture::SetInternalFormat(int format, int mipLevel) {
        m_MipLevelsData[mipLevel].internalFormat = format;
    }

    void OpenGLTexture::SetResolution(const math::iVec2& res, int mipLevel) {
        m_MipLevelsData[mipLevel].width  = res.x;
        m_MipLevelsData[mipLevel].height = res.y;
    }

    void OpenGLTexture::SetDataSize(int size, int mipLevel) {
        m_MipLevelsData[mipLevel].dataSize = size;
    }

    void OpenGLTexture::SetTextureParameters() {
        glTextureParameteri(
            m_Handle.value,
            GL_TEXTURE_MIN_FILTER,
            static_cast<int>(util::opengl::TextureFilterModeToGLEnum(m_FilterMode))
        );
        glTextureParameteri(
            m_Handle.value,
            GL_TEXTURE_MAG_FILTER,
            static_cast<int>(util::opengl::TextureFilterModeToGLEnum(m_FilterMode))
        );
        glTextureParameteri(
            m_Handle.value,
            GL_TEXTURE_WRAP_S,
            static_cast<int>(util::opengl::TextureWrapModeToGLEnum(m_WrapMode))
        );
        glTextureParameteri(
            m_Handle.value,
            GL_TEXTURE_WRAP_T,
            static_cast<int>(util::opengl::TextureWrapModeToGLEnum(m_WrapMode))
        );
    }

    void OpenGLTexture::SetWrapMode(TextureWrapMode mode) {
        m_WrapMode = mode;
    }

    void OpenGLTexture::SetFilterMode(TextureFilterMode mode) {
        m_FilterMode = mode;
    }

    void OpenGLTexture::SetMipLevelsData(const Vector<graphics::TextureData> &mipLevels) {
        CreateMipmapsFromDDS(mipLevels);
    }

    void OpenGLTexture::SetUUID(u64 uuid) {
        m_UUID = UUID(uuid);
    }

    void OpenGLTexture::SetUUID(const UUID &uuid) {
        m_UUID = uuid;
    }

    std::pair<int, int> OpenGLTexture::GetResolution(int mipLevel) {
        if (mipLevel < 0 || (mipLevel >= m_MipLevelsData.size() && !m_MipLevelsData.empty()))
            Warn("MipLevel index mismatch!!! name: " + m_FileInfo.name);
        return std::make_pair(m_MipLevelsData[mipLevel].width, m_MipLevelsData[mipLevel].height);
    }

    graphics::TextureData& OpenGLTexture::GetLevelData(int mipLevel) {
        if (mipLevel < 0 || (mipLevel >= m_MipLevelsData.size() && !m_MipLevelsData.empty()))
            Warn("MipLevel index mismatch!!! name: " + m_FileInfo.name);
        return m_MipLevelsData[mipLevel];
    }

    graphics::TextureData OpenGLTexture::LoadFromFile(const String &path) {
        if (!fs::File::Exists(path)) {
            Warn("File path can't find! path: " + path);
            return {};
        }
        graphics::TextureData data;
        data.data = stbi_load(path.c_str(), &data.width, &data.height, &data.channelCount, 0);
        if (data.channelCount == 3) {
            const auto pixelCount = data.width * data.height;
            const auto* rawData = static_cast<u8*>(data.data);
            auto* rgbaRawData = new u8[pixelCount * 4];

            for (size_t i = 0; i < pixelCount; i++) {
                rgbaRawData[i * 4 + 0] = rawData[i * 3 + 0];
                rgbaRawData[i * 4 + 1] = rawData[i * 3 + 1];
                rgbaRawData[i * 4 + 2] = rawData[i * 3 + 2];
                rgbaRawData[i * 4 + 3] = 255;
            }

            stbi_image_free(data.data);
            data.data = rgbaRawData;
            data.channelCount = 4;
        }
        // DataSize = TexPixelCount * ChannelCount * Byte-Per-Channel
        data.dataSize       = data.width * data.height * data.channelCount * 1;
        data.format         = util::opengl::GetGLFormat(data.channelCount);
        data.internalFormat = util::opengl::GetGLInternalFormat(data.channelCount);
        m_IsSTBAllocated    = true;

        return data;
    }

    void OpenGLTexture::Create() {
        const auto data = LoadFromFile(m_FileInfo.path);
        CreateFromData(data, m_Type);
    }

    void OpenGLTexture::CreateFromData(const graphics::TextureData &data, TextureType type) {
        m_Type = type;
        // One mip level is enough for CPU-generated textures
        m_MipLevelsData.push_back(data);
        if (m_ImageFormatState == ImageFormatState::COMPRESS_ME || m_ImageFormatState == ImageFormatState::COMPRESSED) {
            m_MipLevelsData[0].internalFormat = util::opengl::GetCompressedInternalFormat(m_MipLevelsData[0].channelCount);
        } else {
            m_MipLevelsData[0].internalFormat = util::opengl::GetGLInternalFormat(m_MipLevelsData[0].channelCount);
        }
        m_MipLevelsData[0].format = util::opengl::GetGLFormat(m_MipLevelsData[0].channelCount);
    }

    void OpenGLTexture::CleanUpCPUData() {
        for (auto& level : m_MipLevelsData) {
            if (level.data) {
                if (m_IsSTBAllocated) {
                    stbi_image_free(level.data);
                } else {
                    delete[] static_cast<u8*>(level.data);
                }
                level.data = nullptr;
            }
        }
    }

    void OpenGLTexture::PrepareOptionsAndUploadToGPU() {
        if (m_IsUploadedToGPU)
            return;

        CreateHandle();
        UploadMipLevels();
        SetTextureParameters();
        CreateBindless();
        MakeResident();
        // Clean the texture data after uploading it to the GPU
        CleanUpCPUData();

        m_IsUploadedToGPU = true;
    }

    void OpenGLTexture::CreateHandle() {
        if (!m_Handle.IsValid()) {
            glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle.value);
        }
        if (!m_Handle.IsValid()) {
            Warn("Texture handle can't created! name: " + GetName());
        }
    }

    void OpenGLTexture::CreateBindless() {
        // Create Texture bindless handle
        if (!m_Handle.IsValid()) {
            Warn("Texture handle is not exists!");
            return;
        }
        if (m_BindlessHandleID == 0) {
            if (GLAD_GL_ARB_bindless_texture && GLAD_GL_ARB_gpu_shader_int64) {
                m_BindlessHandleID = glGetTextureHandleARB(m_Handle.value);
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
                glTextureStorage2D(m_Handle.value, m_MipLevelCount, m_MipLevelsData[0].internalFormat,
                    m_MipLevelsData[0].width, m_MipLevelsData[0].height
                );

                for (int lvl = 0; lvl < m_MipLevelCount; lvl++) {
                    const auto& data = m_MipLevelsData[lvl];
                    if (data.width % 4 != 0 || data.height % 4 != 0) {
                        Warn("Compressed mip level size mismatch, texture name: " + GetName());
                        break;
                    }
                    glCompressedTextureSubImage2D(m_Handle.value, lvl, 0, 0, data.width, data.height,
                        data.internalFormat, static_cast<int>(data.dataSize), data.data
                    );
                }
            } break;

            case ImageFormatState::UNCOMPRESSED: {
                // Allocate memory for uncompressed data
                const auto& data = m_MipLevelsData[0];
                m_MipLevelCount = CalculateMaxMipMapLevels(data.width, data.height);

                // Allocate for all the mip levels
                glTextureStorage2D(m_Handle.value, m_MipLevelCount, data.internalFormat, data.width, data.height);
                // Load first mip level data
                glTextureSubImage2D(m_Handle.value, 0, 0, 0, data.width, data.height, data.format, GL_UNSIGNED_BYTE, data.data);
                // Generate other mipmap levels
                glGenerateTextureMipmap(m_Handle.value);
            } break;

            case ImageFormatState::UNDEFINED: Warn("Texture format state is UNDEFINED!");
            default: ;
        }
    }

    void OpenGLTexture::CreateMipmapsFromDDS(const Vector<graphics::TextureData> &levelsData) {
        if (levelsData.empty()) {
            Warn("Texture data empty!");
            return;
        }
        m_MipLevelsData.clear();

        m_MipLevelCount = static_cast<int>(levelsData.size());
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

    int OpenGLTexture::CalculateMaxMipMapLevels(const math::iVec2& res) {
        return CalculateMaxMipMapLevels(res.x, res.y);
    }

    // TODO: we can use resizing for cubemaps to get same width and height (don't remove this shit for now!)
    void OpenGLTexture::Resize(const math::iVec2& resolution, int mipLevel, bool srgbSpace) {
        // Pick a channel flag for Texture resizing
        if (resolution.x == 0 || resolution.y == 0) {
            Warn("Resolution mismatch for: " + m_FileInfo.name);
            return;
        }
        if (m_MipLevelsData.empty())
            Warn("There is no mipmap!! name: " + m_FileInfo.name);
        auto& data = m_MipLevelsData[mipLevel];

        stbir_pixel_layout channelFlag;
        switch (m_MipLevelsData[mipLevel].channelCount) {
            case 1: channelFlag  = STBIR_1CHANNEL; break;
            case 2: channelFlag  = STBIR_2CHANNEL; break;
            case 4: channelFlag  = STBIR_RGBA;     break;
            default: channelFlag = STBIR_RGB;
        }

        const auto& rawData = data.data;

        auto* tempData = new u8[data.dataSize];
        memcpy(tempData, data.data, data.dataSize);

        if (srgbSpace) {
            data.data = stbir_resize_uint8_srgb(tempData, data.width, data.height, 0,
                static_cast<unsigned char*>(rawData), resolution.x, resolution.y, 0, channelFlag
            );
        } else {
            data.data = stbir_resize_uint8_linear(tempData, data.width, data.height, 0,
                static_cast<unsigned char*>(rawData), resolution.x, resolution.y, 0, channelFlag
            );
        }
        delete[] tempData;

        data.width  = resolution.x;
        data.height = resolution.y;
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

    bool OpenGLTexture::IsHandleExist() const {
        return m_Handle.IsValid();
    }
}
