//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <unordered_map>
#include <vector>
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

namespace Real {
    enum class TextureResolution;
    struct MaterialInstance;
}
struct ImFont;

namespace Real {

    class AssetManager {
    public:
        AssetManager();
        void LoadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& name);
        std::string PreprocessorForShaders(const std::string& filePath);
        const Shader &GetShader(const std::string& name);

        void LoadTextureArraysToGPU() const;
        Ref<OpenGLTexture> CreateDefaultTexture(const std::string& name, TextureType type, const glm::ivec2& resolution, int channelCount);
        Ref<OpenGLTexture> GetDefaultTexture(const std::string& name);
        [[nodiscard]] bool IsTextureCompressed(const std::string& name) const;
        Ref<OpenGLTexture> LoadTextureCPUData(const std::string& name, TextureType type, const FileInfo& info = FileInfo());
        Ref<OpenGLTexture> LoadPackedTexturesCPUData(const std::string& name, const Ref<OpenGLTexture>& texture);

        void LoadTexturesFromFile();
        Ref<OpenGLTexture>& GetTexture(const std::string& name);
        [[nodiscard]] bool IsTextureExists(const std::string& name) const { return m_Textures.contains(name); }
        Ref<MaterialInstance>& CreateMaterialInstance(const std::string& name, const std::array<std::string, 4> &fileFormats);

        // TODO: Load fonts from file!!
        void AddFontStyle(const std::string& fontName, ImFont* font);
        ImFont* GetFontStyle(const std::string& fontName);

    private:
        std::unordered_map<std::string, Shader> m_Shaders;
        std::unordered_map<std::string, Ref<OpenGLTexture>> m_Textures;
        std::unordered_map<std::string, Ref<MaterialInstance>> m_Materials;
        std::unordered_map<std::string, ImFont*> m_Fonts;
    };
}
