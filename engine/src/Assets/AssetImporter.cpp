//
// Created by pointerlost on 12/22/25.
//
#include "Assets/AssetImporter.h"
#include "Common/Macros.h"
#include "Assets/AssetManager.h"
#include "../../include/Core/FileManager.h"
#include "Assets/MaterialManager.h"
#include "Assets/MeshManager.h"
#include "Assets/ModelManager.h"
#include "Assets/TextureManager.h"
#include "Tools/Image/TextureUtils.h"
#include "Common/StringUtils.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Graphics/Material.h"
#include "Graphics/Model.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Serialization/Binary.h"
#include "Serialization/Json.h"
#include "Tools/Compression/CompressionUtils.h"
#include "Tools/Image/ImageTools.h"

namespace Real::assets {

    AssetImporter::AssetImporter() {
        m_AssetDB = serialization::json::Load(ASSET_DB_PATH);

        if (!m_AssetDB.contains("textures") || !m_AssetDB["textures"].is_object())
            m_AssetDB["textures"] = nlohmann::json::object();

        if (!m_AssetDB.contains("materials") || !m_AssetDB["materials"].is_object())
            m_AssetDB["materials"] = nlohmann::json::object();

        if (!m_AssetDB.contains("meshes") || !m_AssetDB["meshes"].is_object())
            m_AssetDB["meshes"] = nlohmann::json::object();

        if (!m_AssetDB.contains("models") || !m_AssetDB["models"].is_object())
            m_AssetDB["models"] = nlohmann::json::object();

        BuildCachesFromDB();
    }

    nlohmann::json& AssetImporter::GetAssetDB() {
        return m_AssetDB;
    }

    void AssetImporter::SaveTextureToAssetDB(const platform::opengl::OpenGLTexture* texture) {
        const auto [name, stem, path, ext] = texture->GetFileInfo();
        if (HasAssetWithPath(path))
            return;

        const String uuidStr = std::to_string(texture->GetUUID());
        nlohmann::json& tex = m_AssetDB["textures"][uuidStr];

        tex["name"]               = name;
        tex["stem"]               = stem;
        tex["path"]               = path;
        tex["extension"]          = ext;
        tex["type"]               = util::texture::TextureType_EnumToString(texture->GetType());
        tex["image_format_state"] = util::compression::ImageFormatState_EnumToString(texture->GetImageFormatState());

        CacheAssetWithPath(path, texture->GetUUID());
        MarkDirtyAssetDB();
    }

    void AssetImporter::SaveMaterialToAssetDB(const Ref<Material> &mat) {
        if (mat->id.IsNull())
            mat->id = UUID{};

        const String uuidStr        = std::to_string(mat->id);
        nlohmann::json& entry       = m_AssetDB["materials"][uuidStr];
        entry["name"]               = mat->name;

        // Resolve handle -> texture -> UUID for DB storage
        auto handleToUUID = [&](const GLTextureResourceHandle& handle) -> u64 {
            if (!handle.IsValid())
                return 0ULL;
            const auto* tex = static_cast<GLTexture*>(handle.Get());
            return tex ? static_cast<u64>(tex->GetUUID()) : 0ULL;
        };

        entry["textures"] = {
            { "albedo",   handleToUUID(mat->albedo)   },
            { "normal",   handleToUUID(mat->normal)   },
            { "orm",      handleToUUID(mat->orm)       },
            { "height",   handleToUUID(mat->height)   },
            { "emissive", handleToUUID(mat->emissive) }
        };

        CacheAssetWithName(mat->name, mat->id);
        MarkDirtyAssetDB();
    }

    void AssetImporter::SaveModelToAssetDB(const Ref<Model> &model) {
        if (HasAssetWithPath(model->m_FileInfo.path))
            return;

        const String uuidStr = std::to_string(model->m_UUID);
        nlohmann::json& m = m_AssetDB["models"][uuidStr];

        // Binary file path
        m["binary"] = String(ASSETS_RUNTIME_DIR) + "models/" + model->m_Name + ".model";
        m["name"]   = model->m_Name; // Engine asset name

        // File info
        m["file_name"]      = model->m_FileInfo.name;
        m["file_stem"]      = model->m_FileInfo.stem;
        m["file_path"]      = model->m_FileInfo.path;
        m["file_extension"] = model->m_FileInfo.ext;

        CacheAssetWithPath(model->m_FileInfo.path, model->m_UUID);
        MarkDirtyAssetDB();
        Services::GetModelManager().RegisterModel(model);
    }

    void AssetImporter::SaveMeshToAssetDB(const MeshBinaryHeader &header, const String &name) {
        const auto binaryPath = String(ASSETS_RUNTIME_DIR) + "meshes/" + name + ".mesh";
        if (HasAssetWithPath(binaryPath))
            return;

        const String uuidStr = std::to_string(header.uuid);
        nlohmann::json& m = m_AssetDB["meshes"][uuidStr];

        // Binary file path
        m["binary"] = binaryPath;
        m["name"]   = name; // Engine asset name

        CacheAssetWithPath(binaryPath, UUID(header.uuid));
        MarkDirtyAssetDB();
    }

    void AssetImporter::ImportFromDatabase() {
        // Import from DB
        ImportTextures();
        ImportMaterials();
        ImportMeshes();
        ImportModels();

        // Iterate folder if there is missing new textures
        LoadTexturesFromFolder();
        // TODO: is a safety check required??
    }

    void AssetImporter::ImportTextures() {
        auto& tm = Services::GetTextureManager();

        for (const auto& [uuidStr, texData] : m_AssetDB["textures"].items()) {
            UUID uuid;
            if (!util::TryParseUUID(uuidStr, uuid)) {
                Warn("[ImportTextures] Invalid UUID in Asset DB");
                continue;
            }

            const auto type = util::texture::TextureType_StringToEnum(texData["type"]);
            const auto ifs  = util::compression::ImageFormatState_StringToEnum(texData["image_format_state"]);

            fs::FileInfo fi;
            fi.name = texData.value("name", "null");
            fi.stem = texData.value("stem", "null");
            fi.path = texData.value("path", "null");
            fi.ext  = texData.value("extension", "null");

            if (ifs == ImageFormatState::UNDEFINED) {
                Warn("[ImportTextures] UNDEFINED state: " + fi.path);
                continue;
            }

            auto texture = CreateRef<platform::opengl::OpenGLTexture>();
            texture->SetType(type);
            texture->SetID(uuid); // stamp the DB uuid before loading
            texture->LoadFromFile(fi.path);

            const bool needsCompression =
                (ifs == ImageFormatState::COMPRESS_ME) ||
                (ifs == ImageFormatState::UNCOMPRESSED && !tm.IsCompressed(fi.stem));

            if (needsCompression) {
                if (tools::CompressTexture(texture.get()))
                    UpdateTextureInAssetDB(texture.get());
                else
                    Warn("[ImportTextures] Compression failed: " + fi.path);
            }

            if (!texture->IsReadyForUpload()) {
                Warn("[ImportTextures] Not ready: " + fi.path);
                continue;
            }

            tm.Register(texture);  // UUID already set on texture, tracked via m_UUIDToSlot
        }

        Info("Textures imported from Asset DB successfully!");
    }

    void AssetImporter::ImportMeshes() {
        for (const auto& [uuidStr, mesh_data] : m_AssetDB["meshes"].items()) {
            UUID uuid;
            if (!util::TryParseUUID(uuidStr, uuid)) {
                Warn("Invalid UUID in Material DB");
                continue;
            }
            const auto& bPath = mesh_data["binary"];
            // Save meshes to mesh manager
            const auto& [header, vertices, indices] = serialization::binary::LoadMesh(bPath);
            UUID meshUUID{header.uuid};
            Services::GetMeshManager().CreateSingleMesh(vertices, indices, meshUUID);
        }
        Info("Meshes imported from ASSETS_DB successfully!");
    }

    void AssetImporter::ImportModels() {
        auto& mm = Services::GetModelManager();
        for (const auto& [uuidStr, modelData] : m_AssetDB["models"].items()) {
            UUID uuid;
            if (!util::TryParseUUID(uuidStr, uuid)) {
                Warn("Invalid UUID in material DB");
                continue;
            }

            fs::FileInfo info;
            info.name = modelData["file_name"];
            info.stem = modelData["file_stem"];
            info.path = modelData["file_path"];
            info.ext  = modelData["file_extension"];

            const auto& bPath = modelData["binary"];
            const auto& [header, meshUUIDs, matUUIDs] = serialization::binary::LoadModel(bPath);

            const Ref<Model> model = CreateRef<Model>(uuid, info);
            model->m_MeshUUIDs = meshUUIDs;
            model->m_MaterialAssetUUIDs = matUUIDs;
            model->m_Name = modelData["name"];

            if (header.uuid != 0 && header.uuid != uuid) {
                Warn("[AssetImporter] Model UUID mismatch!!! Binary UUID != AssetDbUUID fix it!");
            }

            mm.RegisterModel(model);
        }
        Info("Models imported from ASSETS_DB successfully!");
    }

    void AssetImporter::ImportMaterials() {
        auto& mm = Services::GetMaterialManager();
        auto& tm = Services::GetTextureManager();

        for (const auto& [uuidStr, matData] : m_AssetDB["materials"].items()) {
            UUID uuid;
            if (!util::TryParseUUID(uuidStr, uuid)) {
                Warn("[ImportMaterials] Invalid UUID in Material DB");
                continue;
            }

            const auto& mat = mm.LoadBaseAsset(uuid, matData.value("name", "Material"));

            if (matData.contains("textures")) {
                const auto& t = matData["textures"];

                // Read UUID from DB, resolve to runtime handle
                auto resolve = [&](const String& key) -> GLTextureResourceHandle {
                    const UUID texUUID(t.value(key, 0ULL));
                    if (texUUID == UUID(0)) return {};
                    return tm.FindByUUID(texUUID);
                };

                mat->albedo   = resolve("albedo");
                mat->normal   = resolve("normal");
                mat->orm      = resolve("orm");
                mat->height   = resolve("height");
                mat->emissive = resolve("emissive");
            }

            mm.RegisterBase(mat);
        }

        Info("Materials imported from Asset DB successfully!");
    }

    void AssetImporter::BuildCachesFromDB() {
        for (auto& [uuidStr, tex] : m_AssetDB["textures"].items()) {
            UUID uuid;
            if (!util::TryParseUUID(uuidStr, uuid)) {
                Warn("Invalid UUID in Material DB");
                continue;
            }
            CacheAssetWithPath(tex["path"], uuid);
        }

        for (const auto& [uuidStr, material] : m_AssetDB["materials"].items()) {
            UUID uuid;
            if (!util::TryParseUUID(uuidStr, uuid)) {
                Warn("Invalid UUID in Material DB");
                continue;
            }
            const String name = material.value("name", "Material");
            CacheAssetWithName(name, uuid);
        }

        for (auto& [uuidStr, mesh] : m_AssetDB["meshes"].items()) {
            UUID uuid;
            if (!util::TryParseUUID(uuidStr, uuid)) {
                Warn("Invalid UUID in Mesh DB");
                continue;
            }
            CacheAssetWithPath(mesh["binary"], uuid);
        }

        for (auto& [uuidStr, model] : m_AssetDB["models"].items()) {
            UUID uuid;
            if (!util::TryParseUUID(uuidStr, uuid)) {
                Warn("Invalid UUID in Model DB");
                continue;
            }
            CacheAssetWithPath(model["file_path"], uuid);
        }
    }

    void AssetImporter::CacheAssetWithName(const String &name, const UUID &uuid) {
        if (!HasAssetWithName(name)) {
            m_NameToUUID.emplace(name, uuid);
        }
    }

    void AssetImporter::CacheAssetWithPath(const String &path, const UUID &uuid) {
        if (!HasAssetWithPath(path)) {
            m_PathToUUID.emplace(path, uuid);
        }
    }

    void AssetImporter::MarkDirtyAssetDB() {
        m_AssetDBDirty = true;
    }

    void AssetImporter::UpdateAssetDB() {
        if (!m_AssetDBDirty) return;
        serialization::json::Save(ASSET_DB_PATH, m_AssetDB);
        m_AssetDBDirty = false;
    }

    void AssetImporter::UpdateTextureInAssetDB(const platform::opengl::OpenGLTexture *texture) {
        const auto [name, stem, path, ext] = texture->GetFileInfo();
        const String uuidStr               = std::to_string(texture->GetUUID());

        auto& tex = m_AssetDB["textures"][uuidStr];

        tex["name"]               = name;
        tex["stem"]               = stem;
        tex["path"]               = path;
        tex["extension"]          = ext;
        tex["type"]               = util::texture::TextureType_EnumToString(texture->GetType());
        tex["image_format_state"] = util::compression::ImageFormatState_EnumToString(texture->GetImageFormatState());

        MarkDirtyAssetDB();
    }

    void AssetImporter::LoadTexturesFromFolder() {
        auto& tm = Services::GetTextureManager();
        auto& mm = Services::GetMaterialManager();
    
        std::unordered_map<String, std::array<Ref<platform::opengl::OpenGLTexture>, 3>> ormPack;
    
        const auto ProcessTexture = [&](const fs::FileInfo& file, ImageFormatState imageFormatState)
        {
            if (HasAssetWithPath(file.path) || tm.IsCompressed(file.stem))
                return;
    
            const auto dashPos = file.stem.find('_');
            if (dashPos == String::npos) {
                Warn("[LoadTexturesFromFolder] No underscore in stem: " + file.stem);
                return;
            }
    
            const auto matName = file.stem.substr(0, dashPos);
            const auto typeStr = file.stem.substr(dashPos + 1);
            const auto type    = util::texture::TextureType_StringToEnum(typeStr);
    
            auto texture = CreateRef<platform::opengl::OpenGLTexture>();
            texture->SetType(type);
            texture->LoadFromFile(file.path);
    
            // Collect ORM channels - pack + compress later as a unit
            if (type == TextureType::AMBIENT_OCCLUSION) { ormPack[matName][0] = texture; return; }
            if (type == TextureType::ROUGHNESS)         { ormPack[matName][1] = texture; return; }
            if (type == TextureType::METALLIC)          { ormPack[matName][2] = texture; return; }
    
            // Compress non-ORM textures individually
            if (imageFormatState == ImageFormatState::COMPRESS_ME ||
                imageFormatState == ImageFormatState::UNCOMPRESSED)
            {
                if (!tools::CompressTexture(texture.get()))
                    Warn("[LoadTexturesFromFolder] Compression failed: " + file.path);
            }
    
            if (!texture->IsReadyForUpload()) {
                Warn("[LoadTexturesFromFolder] Texture not ready: " + file.path);
                return;
            }
    
            auto texHandle = tm.Register(texture);
            UpdateTextureInAssetDB(texture.get());

            const auto mat = mm.GetOrCreateBase(matName);
            switch (type) {
                case TextureType::ALBEDO:   mat->albedo   = texHandle; break;
                case TextureType::NORMAL:   mat->normal   = texHandle; break;
                case TextureType::ORM:      mat->orm      = texHandle; break;
                case TextureType::HEIGHT:   mat->height   = texHandle; break;
                case TextureType::EMISSIVE: mat->emissive = texHandle; break;
                default: break;
            }
            mm.RegisterBase(mat);
        };
    
        for (const auto& file : fs::IterateDirectory(ConcatStr(ASSETS_DIR, "textures/uncompressed/")))
            ProcessTexture(file, ImageFormatState::UNCOMPRESSED);
    
        for (const auto& file : fs::IterateDirectory(ConcatStr(ASSETS_DIR, "textures/compress_me/")))
            ProcessTexture(file, ImageFormatState::COMPRESS_ME);
    
        // Pack and compress ORM after all channels are collected
        for (const auto& [matName, pack] : ormPack) {
            const auto& ao  = pack[0] ? pack[0] : tm.GetOrCreateDefault(TextureType::AMBIENT_OCCLUSION);
            const auto& rgh = pack[1] ? pack[1] : tm.GetOrCreateDefault(TextureType::ROUGHNESS);
            const auto& mtl = pack[2] ? pack[2] : tm.GetOrCreateDefault(TextureType::METALLIC);
    
            // PackORM packs + compresses internally, returns CPU-ready texture
            const auto orm = tools::PackORM(ao, rgh, mtl, matName);
            if (!orm || orm->IsDefault()) continue;
    
            if (!orm->IsReadyForUpload()) {
                Warn("[LoadTexturesFromFolder] ORM not ready: " + matName);
                continue;
            }
    
            auto ormHandle = tm.Register(orm);
            UpdateTextureInAssetDB(orm.get());
            mm.GetOrCreateBase(matName)->orm = ormHandle;
        }
    
        Info("Textures loaded from folder successfully!");
    }

    void AssetImporter::Update() {
        UpdateAssetDB();
    }

    bool AssetImporter::HasAssetWithName(const String &sourceName) const {
        return m_NameToUUID.contains(sourceName);
    }

    bool AssetImporter::HasAssetWithPath(const String &sourcePath) const {
        return m_PathToUUID.contains(fs::NormalizePath(sourcePath));
    }
}
