//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>

#include "UUID.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

namespace Real {
    enum class TextureResolution;
    struct Material;
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
        Ref<OpenGLTexture> GetOrCreateDefaultTexture(const std::string& name, TextureType type,
            const glm::ivec2& resolution = {1024, 1024}, int channelCount = 4
        );
        [[nodiscard]] bool IsTextureCompressed(const std::string& name) const;
        Ref<OpenGLTexture> LoadTextureOnlyCPUData(const FileInfo& file, TextureType type, ImageFormatState imageState);
        Ref<OpenGLTexture> LoadTextureOnlyCPUData(const std::string& path, TextureType type, ImageFormatState imageState);

        TextureData LoadTextureFromFile(const std::string& path);

        void LoadAssets();
        void LoadTextures();
        void DeleteCPUTexture(const std::string& name);
        Ref<OpenGLTexture>& GetTexture(const UUID& uuid);
        Ref<Material>& GetOrCreateMaterialBase(const UUID& uuid);
        Ref<MaterialInstance>& GetOrCreateMaterialInstance(const std::string& name);

        std::vector<GLuint64> UploadTexturesToGPU() const;
        void MakeTexturesResident() const;

        // TODO: Load fonts from file!!
        void AddFontStyle(const std::string& fontName, ImFont* font);
        ImFont* GetFontStyle(const std::string& fontName);

        /************************************  MATERIAL STATE *************************************/
        nlohmann::json LoadMaterialsFromAssetDB();
        nlohmann::json SaveMaterialToAssetDB(const Ref<Material>& mat);
        nlohmann::json SaveTextureToAssetDB(const OpenGLTexture* texture);

    private:
        std::unordered_map<std::string, Shader> m_Shaders; // Should we store with UUID??? maybe next time
        std::unordered_map<UUID, Ref<OpenGLTexture>> m_Textures;
        std::unordered_map<UUID, Ref<Material>> m_Materials;
        std::unordered_map<UUID, Ref<MaterialInstance>> m_MaterialInstances;
        std::unordered_map<std::string, Ref<OpenGLTexture>> m_DefaultTextures;
        std::unordered_map<std::string, ImFont*> m_Fonts;
    };
}
