//
// Created by pointerlost on 10/12/25.
//
#include "Graphics/Texture.h"
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "stb/stb_image.h"

namespace Real {

    bool Texture::Create(const std::string& filePath, const std::string& name) {
        int nrChannels;
        // need update for m_NrChannel!!!
        stbi_set_flip_vertically_on_load(true);
        auto* data = stbi_load(filePath.c_str(), &m_Width, &m_Height, &nrChannels, 0);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

        // Set texture parameters
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        GLenum format = 0, internalFormat = 0;
        if (nrChannels == 4) {
            format = GL_RGBA;
            internalFormat = GL_RGBA8;
        } else if (nrChannels == 3) {
            format = GL_RGB;
            internalFormat = GL_R8;
        } else if (nrChannels == 1) {
            format = GL_RED;
            internalFormat = GL_R8;
        } else {
            stbi_image_free(data);
            Warn("nrChannel size mismatch! from: " + std::string(__FILE__));
            // TODO: Return default texture!!!
        }

        // Allocate GPU memory and uploads the pixels
        if (data) {
            // Allocate storage
            glTextureStorage2D(m_ID, 1, internalFormat, m_Width, m_Height);
            // Upload tex data to GPU
            glTextureSubImage2D(m_ID, 0, 0 ,0, m_Width, m_Height, format, GL_UNSIGNED_BYTE, data);
            GenerateMipMaps();
        } else {
            Warn("Failed to load texture! from: " + std::string(__FILE__));
            // TODO: Return default texture!!!
        }
        stbi_image_free(data);

        // Create handle from texture
        m_BindlessHandle = glGetTextureHandleARB(m_ID);
        return m_BindlessHandle != 0;
    }

    void Texture::CheckOrMakeResident() {
        if (GLAD_GL_ARB_bindless_texture) {
            if (m_BindlessHandle == 0) {
                m_BindlessHandle = glGetTextureHandleARB(m_ID);
            }
            glMakeTextureHandleResidentARB(m_BindlessHandle);
            m_IsResident = true;
        }
    }

    void Texture::CheckOrMakeNonResident() {
        if (GLAD_GL_ARB_bindless_texture) {
            if (m_IsResident && m_BindlessHandle) {
                glMakeTextureHandleNonResidentARB(m_BindlessHandle);
                m_IsResident = false;
                m_BindlessHandle = 0;
            }
        }
    }

    void Texture::GenerateMipMaps() {
        glGenerateTextureMipmap(m_ID);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}
