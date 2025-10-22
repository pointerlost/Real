//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <unordered_map>
#include <vector>

#include "Logger.h"
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

        void LoadTextures();
        Ref<Texture>& GetTexture(const std::string& name) {
            if (!m_Textures.contains(name)) Warn("Texture '" + name + "' can't find!");
            return m_Textures[name];
        }
        Ref<MaterialInstance> GetDefaultMat();
        [[nodiscard]] bool IsTextureExists(const std::string& name) const { return m_Textures.contains(name); }
        Ref<MaterialInstance>& CreateMaterialInstance(const std::string& name);

        void BindTextureArray() const;

    private:
        std::unordered_map<std::string, Shader> m_Shaders;
        std::unordered_map<std::string, Ref<Texture>> m_Textures;
        GLuint m_GPUTextureArray = 0;
        std::vector<Ref<Texture>> m_TextureArrays;
        std::unordered_map<std::string, Ref<MaterialInstance>> m_Materials;

    private:
        Ref<Texture> LoadTexture(const std::string& filePath, const std::string& name);
    };
}
