//
// Created by pointerlost on 12/22/25.
//
#pragma once
#include <Core/CMakeConfig.h>
#include "Common/Utils.h"
#include <nlohmann/json.hpp>
#include "Core/UUID.h"

namespace Real {
    namespace graphics {
        struct Material;
        struct Model;
    }

    namespace platform::opengl {
        struct OpenGLTexture;
    }

    namespace assets { struct MeshBinaryHeader; }
}

namespace Real::assets {

    // This is a COMPILE-TIME class to bring files from disk
    class AssetImporter {
    public:
        AssetImporter();

        [[maybe_unused]] nlohmann::json& GetAssetDB();
        void SaveTextureToAssetDB(const platform::opengl::OpenGLTexture* texture);
        void SaveMaterialToAssetDB(const Ref<graphics::Material>& mat);
        void SaveModelToAssetDB(const Ref<graphics::Model>& model);
        void SaveMeshToAssetDB(const MeshBinaryHeader &header, const String& name);

        void ImportFromDatabase();

        void MarkDirtyAssetDB();
        void UpdateAssetDB();
        void UpdateTextureInAssetDB(const platform::opengl::OpenGLTexture* texture);

        void LoadTexturesFromFolder();

        void Update();
        bool HasAssetWithName(const String& sourceName) const;
        bool HasAssetWithPath(const String& sourcePath) const;

    private:
        static constexpr auto ASSET_DB_PATH = ASSETS_DIR "asset_database/asset_database.json";
        nlohmann::json m_AssetDB{};
        bool m_AssetDBDirty = false;

        // Cache paths with UUIDs to check when new assets are added (Textures, models etc.)
        std::unordered_map<String, UUID> m_PathToUUID;
        // Cache paths with UUIDs to check when new assets are added (Materials, meshes etc.)
        std::unordered_map<String, UUID> m_NameToUUID;

    private:
        void ImportTextures();
        void ImportMeshes();
        void ImportModels();
        void ImportMaterials();
        void BuildCachesFromDB();

        void CacheAssetWithName(const String& name, const UUID& uuid);
        void CacheAssetWithPath(const String& path, const UUID& uuid);
    };
}
