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

struct ImFont;
namespace Real { struct MaterialInstance; }

namespace Real {

    class AssetManager {
    public:
        void LoadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& name);
        std::string PreprocessorForShaders(const std::string& filePath);
        const Shader &GetShader(const std::string& name);

        [[nodiscard]] bool IsTextureCompressed(const std::string& name) const;
        void LoadUncompressedTexture(const std::string& name, const std::string& filePath, TextureType type, const FileInfo& info);
        void LoadUncompressedTexture(const std::string& name, const std::string& filePath, TextureType type);
        void LoadUncompressedTexture(const std::string& name, const FileInfo &info, TextureType type);
        void LoadUncompressedTextures(const std::string& name, void* mixedData, TextureType type);
        void CreateTextureArray(const glm::ivec2& resolution, const std::vector<Ref<Texture>>& textures);
        void LoadTextures();
        Ref<Texture>& GetTexture(const std::string& name) {
            if (!IsTextureExists(name)) Warn("Texture '" + name + "' can't find!");
            return m_Textures[name];
        }
        Ref<MaterialInstance> GetDefaultMat();
        [[nodiscard]] bool IsTextureExists(const std::string& name) const { return m_Textures.contains(name); }
        Ref<MaterialInstance>& CreateMaterialInstance(const std::string& name);

        void BindTextureArray() const;

        // TODO: Load fonts from file!!
        void AddFontStyle(const std::string& fontName, ImFont* font);
        ImFont* GetFontStyle(const std::string& fontName);

    private:
        std::unordered_map<std::string, Shader> m_Shaders;
        std::unordered_map<std::string, Ref<Texture>> m_Textures;
        std::unordered_map<std::string, Ref<MaterialInstance>> m_Materials;
        std::unordered_map<std::string, ImFont*> m_Fonts;
    };
}
