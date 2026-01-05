//
// Created by pointerlost on 12/22/25.
//
#pragma once
#include <Core/CMakeConfig.h>
#include <nlohmann/json.hpp>
#include "Utils.h"
#include "UUID.h"

namespace Real {
    struct MeshBinaryHeader;
    struct Model;
    struct Material;
    struct OpenGLTexture;
}

namespace Real {

    // This is a COMPILE-TIME class to bring files from disk
    class AssetImporter {
    public:
        AssetImporter();

        [[maybe_unused]] nlohmann::json& GetAssetDB();
        void SaveTextureToAssetDB(const OpenGLTexture* texture);
        void SaveMaterialToAssetDB(const Ref<Material>& mat);
        void SaveModelToAssetDB(const Ref<Model>& model);
        void SaveMeshToAssetDB(const MeshBinaryHeader &header, const std::string& name);

        void ImportFromDatabase();

        void MarkDirtyAssetDB();
        void UpdateAssetDB();
        void UpdateTextureInAssetDB(const OpenGLTexture* texture);
        void LoadNewAssetsToDataBase();

        void LoadTexturesFromFolder();

        void Update();
        bool HasAssetWithName(const std::string& sourceName) const;
        bool HasAssetWithPath(const std::string& sourcePath) const;

    private:
        static constexpr auto ASSET_DB_PATH = ASSETS_DIR "asset_database/asset_database.json";
        nlohmann::json m_AssetDB{};
        bool m_AssetDBDirty  = false;

        // Cache paths with UUIDs to check when new assets are added (Textures, models etc.)
        std::unordered_map<std::string, UUID> m_PathToUUID;
        // Cache paths with UUIDs to check when new assets are added (Materials, meshes etc.)
        std::unordered_map<std::string, UUID> m_NameToUUID;

    private:
        void ImportTextures();
        void ImportMeshes();
        void ImportModels();
        void ImportMaterials();
        void BuildCachesFromDB();

        void CacheAssetWithName(const std::string& name, const UUID& uuid);
        void CacheAssetWithPath(const std::string& path, const UUID& uuid);
    };
}
