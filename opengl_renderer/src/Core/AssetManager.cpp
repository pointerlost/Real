//
// Created by pointerlost on 10/4/25.
//
#include "Core/AssetManager.h"
#include "Core/file_manager.h"
#include "Core/Logger.h"
#include "Core/Utils.h"
// #define STB_IMAGE_IMPLEMENTATION
// #include <stb/stb_image.h>
#include <fstream>
#include <stack>
#include <Core/Config.h>

#include "Graphics/Material.h"
#include "stb/stb_image.h"

namespace Real {

    void AssetManager::LoadShader(const std::string &vertexPath, const std::string &fragmentPath,
        const std::string& name)
    {
        const auto vertPath = PreprocessorForShaders(vertexPath);
        const auto fragPath = PreprocessorForShaders(fragmentPath);
        m_Shaders[name] = Shader{vertPath, fragPath, name};

        // Without preprocessors
        // const auto vert = File::ReadFromFile(vertexPath);
        // const auto frag = File::ReadFromFile(fragmentPath);
        // m_Shaders[name] = Shader{vert, frag, name};
    }

    std::string AssetManager::PreprocessorForShaders(const std::string &filePath) {
        using namespace std;

        std::string result;
        stack<string> glslContentQueue;

        if (!File::Exists(filePath)) {
            Warn("Path doesn't exists: " + filePath);
            return{};
        }

        ifstream stream(filePath, ios::in);
        if (!stream.is_open()) {
            Warn("File can't opening from: " + filePath);
            return{};
        }

        // Load main files like (vertex, fragment)
        std::string mainFileResult;
        std::string line;
        while (getline(stream, line)) {
            if (line.substr(0, 8) == "#include") {
                size_t pathFirstLine = 10;
                glslContentQueue.push(ConcatStr(SHADERS_DIR + line.substr(pathFirstLine, line.size() - pathFirstLine - 1)));
            } else if (line.substr(0, 1) != "#") {
                mainFileResult.append(ConcatStr(line + "\n"));
            } else if (line.substr(0, 7) != "#version") {
                result.append(ConcatStr(line + "\n"));
            }
        }

        // Merge all files containing '#include'
        while (!glslContentQueue.empty()) {
            line = "";
            std::string queuePath = glslContentQueue.top();
            glslContentQueue.pop();

            ifstream newStream(queuePath, ios::in);
            while (getline(newStream, line)) {
                if (line.substr(0, 8) == "#include") {
                    size_t pathFirstLine = 10;
                    glslContentQueue.push(ConcatStr(SHADERS_DIR + line.substr(pathFirstLine, line.size() - pathFirstLine - 1)));
                } else if (line.substr(0, 7) != "#version" || line.substr(0, 1) != "#") {
                    // Load into the beginning because main section should at the end
                    result.append(ConcatStr(line + "\n"));
                }
            }
        }

        return ConcatStr(result + mainFileResult);
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

        texture->Load(filePath);
        texture->m_Index = static_cast<int>(m_TextureArrays.size());

        m_TextureArrays.push_back(texture);
        m_Textures[name] = texture;
        return texture;
    }

    void AssetManager::LoadTextures() {
        // Load all textures
        LoadTexture("assets/textures/container.jpg", "container");

        // Create texture array
        GLuint textureArray;
        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &textureArray);
        // Allocate the storage
        glTextureStorage3D(textureArray, 1, GL_RGBA8, m_TextureArrays[0]->m_Width, m_TextureArrays[0]->m_Height, m_TextureArrays.size());

        for (int i = 0; i < m_TextureArrays.size(); i++) {
            auto tex = m_TextureArrays[i];
            glTextureSubImage3D(textureArray, 0, 0, 0, i, tex->m_Width, tex->m_Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, tex->m_Data);
            stbi_image_free(tex->m_Data); // Clean up vRAM
            tex->m_Data = nullptr;
        }
        // Set parameters
        glTextureParameteri(textureArray, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(textureArray, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(textureArray, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(textureArray, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Bind texture array (binding point = 6)
        glBindTextureUnit(6, textureArray);
    }

    Ref<MaterialInstance> AssetManager::GetDefaultMat() {
        const auto mat = CreateRef<Material>();
        const auto instance = CreateRef<MaterialInstance>();
        instance->m_Base = mat;
        m_Materials["default"] = instance;
        return m_Materials["default"];
    }
}
