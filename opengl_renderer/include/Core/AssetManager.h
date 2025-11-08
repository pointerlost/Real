//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <unordered_map>
#include <vector>
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

struct ImFont;
namespace Real { struct MaterialInstance; }

namespace Real {

    class AssetManager {
    public:
        AssetManager();
        void LoadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& name);
        std::string PreprocessorForShaders(const std::string& filePath);
        const Shader &GetShader(const std::string& name);

        Ref<Texture> CreateDefaultTexture(const std::string& name, TextureType type, const glm::ivec2& resolution, int channelCount);
        Ref<Texture> GetDefaultTexture(const std::string& name);
        [[nodiscard]] bool IsTextureCompressed(const std::string& name) const;
        Ref<Texture> LoadUncompressedTexture(const std::string& name, const std::string& filePath, TextureType type, const FileInfo& info = FileInfo());
        Ref<Texture> LoadUncompressedTexture(const std::string& name, const FileInfo &info, TextureType type);
        Ref<Texture> LoadUncompressedTextures(const std::string& name, const Ref<Texture>& mixedTextures, const glm::ivec2& resolution, TextureType type = TextureType::RMA);
        void CreateCompressedTextureArray(const glm::ivec2& resolution, const std::vector<Ref<Texture>>& textures);
        void CreateTextureArray(const glm::ivec2& resolution, const std::vector<Ref<Texture>>& textures);
        void LoadTextures();
        Ref<Texture>& GetTexture(const std::string& name);
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
