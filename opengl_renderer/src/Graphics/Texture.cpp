//
// Created by pointerlost on 10/12/25.
//
#include "Graphics/Texture.h"
#include "Core/AssetManager.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Core/Utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace Real {

    void Texture::Load(const std::string& filePath) {
        m_Data = stbi_load(filePath.c_str(), &m_Width, &m_Height, nullptr, 4);
        if (!m_Data) {
            Warn("Texture can't be loaded from: " + filePath);
        }
    }

}
