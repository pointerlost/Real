//
// Created by pointerlost on 10/12/25.
//
#include "Graphics/Texture.h"
#include <utility>

#include "Core/Logger.h"
#include "stb/stb_image.h"

namespace Real {

    Texture::Texture(ImageFormatState format, bool isDefaultTex)
        : m_Type(TextureType::UNDEFINED), m_ImageFormatState(format), m_IsDefault(isDefaultTex)
    {
    }

    Texture::Texture(const Ref<OpenGLTextureData> &data, TextureType type, bool isDefaultTex)
        : m_Data(data.get()), m_Type(type), m_IsDefault(isDefaultTex)
    {
    }

    void Texture::SetData(OpenGLTextureData data) {
        m_Data = data;
    }

    void Texture::SetFileInfo(FileInfo info) {
        m_FileInfo = std::move(info);
    }

    void Texture::SetType(TextureType type) {
        m_Type = type;
    }

    void Texture::SetIndex(int idx) {
        m_Index = idx;
    }

    void Texture::SetImageFormat(ImageFormatState format) {
        m_ImageFormatState = format;
    }

    std::string Texture::GetTypeAsString(TextureType type) const {
        switch (type) {
            case TextureType::ALB:
                return "ALB";
            case TextureType::NRM:
                return "NRM";
            case TextureType::RGH:
                return "RGH";
            case TextureType::MTL:
                return "MTL";
            case TextureType::AO:
                return "AO";
            case TextureType::RMA:
                return "RMA";
            case TextureType::HEIGHT:
                return "HEIGHT";

            default:
                return "UNDEFINED";
        }
    }

    void Texture::Create() {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_Data.m_Handle);
        glTextureStorage2D(m_Data.m_Handle, 1, m_Data.m_InternalFormat, m_Data.m_Width, m_Data.m_Height);

        // TODO: Need mipLevels???
        if (m_Data.m_Data) {
            if (m_ImageFormatState == ImageFormatState::COMPRESSED) {
                glCompressedTextureSubImage2D(m_Data.m_Handle, 0, 0, 0,
                    m_Data.m_Width, m_Data.m_Height, m_Data.m_Format, m_Data.m_ImageSize, m_Data.m_Data
                );
            }
            else {
                glTexSubImage2D(GL_TEXTURE_2D, 1, 0, 0,
                    m_Data.m_Width, m_Data.m_Height, m_Data.m_Format, GL_UNSIGNED_BYTE, m_Data.m_Data
                );
            }

            // if mipLevels > 1
            // glGenerateTextureMipmap(m_Data.m_Handle);

            // These setters arguments can change when i add mipLevels
            glTextureParameteri(m_Data.m_Handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(m_Data.m_Handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_Data.m_Handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(m_Data.m_Handle, GL_TEXTURE_WRAP_T, GL_REPEAT);

            stbi_image_free(m_Data.m_Data);
            m_Data.m_Data = nullptr;
        }

        CreateBindlessAndMakeResident();
    }

    void Texture::CreateBindlessAndMakeResident() {
        // Create Texture bindless handle
        m_glBindlessHandle = glGetTextureHandleARB(m_Data.m_Handle);
        if (m_glBindlessHandle == 0) {
            Warn("Bindless Handle can't created! tex name: " + m_FileInfo.name);
            return;
        }

        glMakeTextureHandleResidentARB(m_glBindlessHandle);
    }
}
