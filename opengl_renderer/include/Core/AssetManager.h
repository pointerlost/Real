//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <unordered_map>
#include "Utils.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

namespace Real {
    struct MaterialInstance;
}

namespace Real {

    class AssetManager {
    public:
        void LoadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& name);
        std::string PreprocessorForShaders(const std::string& filePath);
        const Shader &GetShader(const std::string& name);
        Ref<Texture> LoadTexture(const std::string& filePath, const std::string& name);
        Ref<Texture>& GetTexture(const std::string& name) { return m_Textures[name]; }
        Ref<MaterialInstance> GetDefaultMat();

    private:
        std::unordered_map<std::string, Shader> m_Shaders;
        std::unordered_map<std::string, Ref<Texture>> m_Textures;
        std::unordered_map<std::string, GLuint> m_TextureHandles;
        std::unordered_map<std::string, Ref<MaterialInstance>> m_Materials;
    };
}
