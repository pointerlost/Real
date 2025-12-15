//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>

#include "CmakeConfig.h"
#include "UUID.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

namespace Real {
    struct Model;
}

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
        void Update();
        [[maybe_unused]] nlohmann::json GetAssetDB();
        void LoadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& name);
        std::string PreprocessorForShaders(const std::string& filePath);
        [[nodiscard]] const Shader &GetShader(const std::string& name);

        void LoadTextureArraysToGPU() const;
        [[nodiscard]] Ref<OpenGLTexture> GetOrCreateDefaultTexture(TextureType type);
        [[maybe_unused]] bool IsTextureCompressed(const std::string& name) const;
        [[nodiscard]] Ref<OpenGLTexture> LoadTextureOnlyCPUData(const FileInfo& file, TextureType type, ImageFormatState imageState, const UUID& uuid);
        [[nodiscard]] Ref<OpenGLTexture> LoadTextureOnlyCPUData(const std::string& path, TextureType type, ImageFormatState imageState, const UUID& uuid);

        TextureData LoadTextureFromFile(const std::string& path);

        void LoadAssets();
        void DeleteCPUTexture(const UUID& uuid);
        [[nodiscard]] Ref<OpenGLTexture> GetTexture(const UUID& uuid, TextureType type);
        [[nodiscard]] Ref<Material>& GetOrCreateMaterialBase(const UUID& uuid);
        Ref<Material> GetOrCreateMaterialBase(const std::string &name);

        [[nodiscard]] Ref<MaterialInstance>& GetOrCreateMaterialInstance(const UUID& uuid);
        [[nodiscard]] std::vector<GLuint64> UploadTexturesToGPU() const;
        void MakeTexturesResident() const;

        // TODO: Load fonts from file!!
        void AddFontStyle(const std::string& fontName, ImFont* font);
        ImFont* GetFontStyle(const std::string& fontName);

        /************************************  MATERIAL STATE *************************************/
        void SaveModelToAssetDB(const Ref<Model>& model);
        void SaveModelCPU(const Ref<Model>& model);
        void LoadModelsFromAssetDB();
        void LoadMaterialsFromAssetDB();
        void SaveMaterialToAssetDB(const Ref<Material>& mat);
        void SaveTextureToAssetDB(const OpenGLTexture* texture);
        void LoadTexturesFromAssetDB();
        void RenameMaterial(const std::string& newName, const UUID& uuid);

    private:
        std::unordered_map<std::string, Shader> m_Shaders; // Should we store with UUID??? maybe next time
        std::unordered_map<UUID, Ref<OpenGLTexture>> m_Textures;
        std::unordered_set<std::string> m_LoadedTexturesPath;
        std::unordered_map<UUID, Ref<Material>> m_Materials;
        std::unordered_map<std::string, UUID> m_MaterialNameToUUID;
        std::unordered_map<UUID, Ref<MaterialInstance>> m_MaterialInstances;
        std::unordered_map<UUID, Ref<Model>> m_Models;
        std::unordered_map<TextureType, Ref<OpenGLTexture>> m_DefaultTextures;
        std::unordered_map<std::string, ImFont*> m_Fonts;
        static constexpr auto ASSET_DB_PATH = ASSETS_DIR "asset_database/asset_database.json";
        nlohmann::json m_AssetDB{};
        bool m_AssetDBDirty  = false;
        inline static bool s_LoadedAssetDB = false;

    private:
        void MarkDirtyAssetDB();
        void UpdateAssetDB();
    };
}
