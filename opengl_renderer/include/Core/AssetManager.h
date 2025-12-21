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
#include "Graphics/Material.h"
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

        /* *********************************** TEXTURE STATE ************************************ */
        void SaveCPUTexture(const Ref<OpenGLTexture> &tex);
        void LoadTextureArraysToGPU() const;
        [[maybe_unused]] Ref<OpenGLTexture>& GetOrCreateDefaultTexture(TextureType type);
        [[maybe_unused]] bool IsTextureCompressed(const std::string& stem) const;
        TextureData LoadTextureFromFile(const std::string& path, TextureType type = TextureType::UNDEFINED);
        void DeleteCPUTexture(const UUID& uuid);
        [[nodiscard]] const Ref<OpenGLTexture>& GetTexture(const UUID& uuid, TextureType type);

        /* *********************************** MATERIAL STATE ************************************ */
        [[maybe_unused]] Ref<Material>& GetOrCreateMaterialBase(const UUID& uuid);
        [[maybe_unused]] Ref<Material>& GetOrCreateMaterialBase(const std::string &name);
        [[maybe_unused]] Ref<MaterialInstance>& GetOrCreateMaterialInstance(const UUID& uuid);

        /* *********************************** DATABASE STATE ************************************ */
        [[maybe_unused]] nlohmann::json& GetAssetDB();
        void SaveTextureToAssetDB(const OpenGLTexture* texture);
        void SaveMaterialToAssetDB(const Ref<Material>& mat);
        void SaveModelToAssetDB(const Ref<Model>& model);
        void SaveMeshToAssetDB(const MeshBinaryHeader &header, const std::string& name);

        /* *********************************** GENERAL STATE ************************************ */
        [[nodiscard]] const Shader &GetShader(const std::string& name);
        bool IsModelExist(const std::string& name);
        bool IsMaterialExist(const std::string& name);
        void RenameMaterial(const std::string& newName, const UUID& uuid);
        void SaveModelCPU(const Ref<Model>& model);

        // TODO: Load fonts from file!!
        void AddFontStyle(const std::string& fontName, ImFont* font);
        ImFont* GetFontStyle(const std::string& fontName);

        /* ********************************** LOADING STATE ************************************ */
        void Update();
        void LoadNewAssetsToDataBase();
        void LoadAssetsFromDataBase();
        void LoadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& name);
        [[nodiscard]] std::vector<GLuint64> UploadTexturesToGPU() const;

    private:
        std::unordered_map<std::string, Shader> m_Shaders; // Should we store with UUID??? maybe next time
        std::unordered_map<UUID, Ref<OpenGLTexture>> m_Textures;
        std::unordered_set<std::string> m_LoadedTexturesPath;
        std::unordered_map<UUID, Ref<Material>> m_Materials;
        std::unordered_map<std::string, UUID> m_MaterialNameToUUID;
        std::unordered_map<UUID, Ref<MaterialInstance>> m_MaterialInstances;
        std::unordered_map<UUID, Ref<Model>> m_Models;
        std::unordered_map<std::string, UUID> m_ModelNameToUUID;
        std::unordered_map<TextureType, Ref<OpenGLTexture>> m_DefaultTextures;
        std::unordered_map<std::string, ImFont*> m_Fonts;
        static constexpr auto ASSET_DB_PATH = ASSETS_DIR "asset_database/asset_database.json";
        nlohmann::json m_AssetDB{};
        bool m_AssetDBDirty  = false;
        inline static bool s_LoadedAssetDB = false;

    private:
        void MarkDirtyAssetDB();
        void UpdateAssetDB();
        void LoadTexturesFromAssetDB();
        void LoadMaterialsFromAssetDB();
        void LoadModelsFromAssetDB();
        void LoadMeshesFromAssetDB();

        void LoadDefaultTextures();
        std::vector<Ref<OpenGLTexture>> GetMaterialTextures(const Material* mat);
        std::string PreprocessorForShaders(const std::string& filePath);
    };
}
