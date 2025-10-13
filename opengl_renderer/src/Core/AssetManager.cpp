//
// Created by pointerlost on 10/4/25.
//
#include "Core/AssetManager.h"
#include "Core/file_manager.h"
#include "Core/Logger.h"
#include "Core/Utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Real {

    void AssetManager::LoadShader(const std::string &vertexPath, const std::string &fragmentPath,
        const std::string& name)
    {
        const auto vert = File::ReadFromFile(vertexPath);
        const auto frag = File::ReadFromFile(fragmentPath);
        m_Shaders[name] = Shader{vert, frag, name};
    }

    const Shader& AssetManager::GetShader(const std::string &name) {
        if (!m_Shaders.contains(name)) {
            Warn(ConcatStr("Shader Doesn't exists! Warn from the file: ", __FILE__));
        }
        return m_Shaders.at(name);
    }

    Ref<Texture> AssetManager::LoadTexture(const std::string &filePath, const std::string& name) {
        if (m_Textures.contains(name)) return m_Textures[name];
        auto texture = CreateRef<Texture>();

        const auto handle = texture->Create(filePath, name);

        m_Textures[name] = texture;
        m_TextureHandles[name] = handle;
        return texture;
    }
}
