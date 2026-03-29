//
// Created by pointerlost on 3/28/26.
//
#include "Platform/OpenGL/OpenGLTexture.h"

#include <cassert>
#include <cmath>

#include "Core/Logger.h"
#include "glad/glad.h"
#include "Platform/OpenGL/OpenGLUtils.h"
#include "stb/stb_image.h"
#include "Tools/Image/ImageTools.h"
#include "Tools/Image/TextureUtils.h"

namespace Real::platform::opengl {

    void OpenGLTexture::LoadFromFile(const String& path) {
        m_Origin   = TextureOrigin::File;
        m_FileInfo = fs::FileInfoFromPath(path);

        if (m_FileInfo.ext == ".dds") {
            m_ImageFormatState = ImageFormatState::COMPRESSED;
            Vector<TextureData> levels;
            if (!tools::ReadDDS(path, levels)) {
                Warn("[LoadFromFile] Failed to read DDS: " + path);
                return;
            }
            CreateMipmapsFromDDS(levels);
        } else {
            m_ImageFormatState = ImageFormatState::UNCOMPRESSED;
            m_IsSTBAllocated   = true;

            const int desiredChannels = (m_Type != TextureType::UNDEFINED)
                ? util::texture::TextureTypeToChannelCount(m_Type)
                : 0;

            TextureData data;
            data.data = stbi_load(path.c_str(),
                &data.width, &data.height, &data.channelCount, desiredChannels);
            data.channelCount = desiredChannels != 0 ? desiredChannels : data.channelCount;
            data.dataSize     = data.width * data.height * data.channelCount;

            if (!data.data) {
                Warn("[LoadFromFile] stbi_load failed: " + path);
                return;
            }

            data.format         = util::opengl::GetGLFormat(data.channelCount);
            data.internalFormat = util::opengl::GetGLInternalFormat(data.channelCount);
            m_MipLevels.push_back(data);
        }
    }

    void OpenGLTexture::CreateFromData(const TextureData& data, TextureType type) {
        // Used for Generated (defaults) and Packed (ORM) origins.
        // Caller must set m_Origin and m_ImageState before calling this.
        m_Type = type;
        TextureData d       = data;
        d.format            = util::opengl::GetGLFormat(d.channelCount);
        d.internalFormat    = util::opengl::GetGLInternalFormat(d.channelCount);
        m_MipLevels.push_back(d);
    }

    void OpenGLTexture::SetMipLevels(const Vector<TextureData>& levels) {
        CreateMipmapsFromDDS(levels);
    }

    TextureData& OpenGLTexture::GetMipLevel(int level) {
        assert(!m_MipLevels.empty() && "Texture has no mip data");
        const int clamped = (level < static_cast<int>(m_MipLevels.size())) ? level : 0;
        return m_MipLevels[clamped];
    }

    void OpenGLTexture::UploadToGPU() {
        if (m_IsUploadedToGPU) return;

        CreateHandle();
        UploadMipLevels();
        SetTextureParameters();
        CreateBindless();
        MakeResident();
        CleanUpCPUData();

        m_IsUploadedToGPU = true;
    }

    void OpenGLTexture::UploadMipLevels() const {
        if (m_MipLevels.empty()) {
            Warn("[UploadMipLevels] No mip data - name: " + GetDebugName());
            return;
        }

        const auto& base = m_MipLevels[0];

        switch (m_ImageFormatState) {
            case ImageFormatState::COMPRESSED: {
                glTextureStorage2D(m_Handle.value,
                    static_cast<int>(m_MipLevels.size()),
                    base.internalFormat, base.width, base.height);

                for (int lvl = 0; lvl < static_cast<int>(m_MipLevels.size()); lvl++) {
                    const auto& d = m_MipLevels[lvl];
                    glCompressedTextureSubImage2D(
                        m_Handle.value, lvl, 0, 0,
                        d.width, d.height, d.internalFormat,
                        d.dataSize, d.data
                    );
                }
            } break;

            case ImageFormatState::UNCOMPRESSED: {
                const int mipCount = CalculateMaxMipLevels(base.width, base.height);
                glTextureStorage2D(m_Handle.value, mipCount, base.internalFormat, base.width, base.height);
                glTextureSubImage2D(
                    m_Handle.value, 0, 0, 0,
                    base.width, base.height, base.format, GL_UNSIGNED_BYTE, base.data
                );
                glGenerateTextureMipmap(m_Handle.value);
            } break;

            case ImageFormatState::DEFAULT: {
                // 1x1 (no mip generation needed)
                glTextureStorage2D(m_Handle.value, 1,
                    base.internalFormat, 1, 1);
                glTextureSubImage2D(m_Handle.value, 0, 0, 0,
                    1, 1, base.format, GL_UNSIGNED_BYTE, base.data);
            } break;

            case ImageFormatState::UNDEFINED:
                Warn("[UploadMipLevels] State is UNDEFINED - name: " + GetDebugName());
                break;

            default: break;
        }
    }

    void OpenGLTexture::CreateHandle() {
        if (m_Handle.IsValid()) return;
        glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle.value);
        if (!m_Handle.IsValid())
            Warn("[CreateHandle] Failed - name: " + GetDebugName());
    }

    void OpenGLTexture::CreateBindless() {
        if (!m_Handle.IsValid()) {
            Warn("[CreateBindless] No handle - name: " + GetDebugName());
            return;
        }
        if (m_Bindless != 0) return;

        if (GLAD_GL_ARB_bindless_texture && GLAD_GL_ARB_gpu_shader_int64) {
            m_Bindless = glGetTextureHandleARB(m_Handle.value);
            if (m_Bindless == 0)
                Warn("[CreateBindless] Failed - name: " + GetDebugName());
        }
    }

    void OpenGLTexture::SetTextureParameters() {
        const int minFilter = util::opengl::TextureFilterModeToGLEnum(m_FilterMode);
        const int wrap      = util::opengl::TextureWrapModeToGLEnum(m_WrapMode);

        glTextureParameteri(m_Handle.value, GL_TEXTURE_MIN_FILTER, minFilter);
        glTextureParameteri(m_Handle.value, GL_TEXTURE_MAG_FILTER, minFilter);
        glTextureParameteri(m_Handle.value, GL_TEXTURE_WRAP_S, wrap);
        glTextureParameteri(m_Handle.value, GL_TEXTURE_WRAP_T, wrap);
    }

    void OpenGLTexture::MakeResident() const {
        if (m_Bindless == 0) {
            Warn("[MakeResident] No bindless handle - name: " + GetDebugName());
            return;
        }
        if (GLAD_GL_ARB_bindless_texture && !glIsTextureHandleResidentARB(m_Bindless))
            glMakeTextureHandleResidentARB(m_Bindless);
    }

    void OpenGLTexture::MakeNonResident() const {
        if (m_Bindless != 0 && GLAD_GL_ARB_bindless_texture && glIsTextureHandleResidentARB(m_Bindless))
            glMakeTextureHandleNonResidentARB(m_Bindless);
    }

    void OpenGLTexture::CleanUpCPUData() {
        for (auto& level : m_MipLevels) {
            if (!level.data) continue;
            if (m_IsSTBAllocated)
                stbi_image_free(level.data);
            else
                delete[] static_cast<u8*>(level.data);
            level.data = nullptr;
        }
    }

    void OpenGLTexture::CreateMipmapsFromDDS(const Vector<TextureData>& levels) {
        if (levels.empty()) {
            Warn("[CreateMipmapsFromDDS] Empty levels - name: " + GetDebugName());
            return;
        }
        m_MipLevels = levels;
    }

    int OpenGLTexture::CalculateMaxMipLevels(int width, int height) const {
        if (m_ImageFormatState == ImageFormatState::COMPRESSED) {
            int levels = 1;
            int w = width, h = height;
            while (w > 4 || h > 4) {
                w = std::max(1, w >> 1);
                h = std::max(1, h >> 1);
                levels++;
            }
            return levels;
        }
        return static_cast<int>(std::floor(std::log2(std::max(width, height)))) + 1;
    }
}
