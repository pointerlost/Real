//
// Created by pointerlost on 12/22/25.
//
#include <Core/AssetImporter.h>

#include "Core/AssetManager.h"
#include "Core/file_manager.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Graphics/Material.h"
#include "Graphics/MeshManager.h"
#include "Graphics/Model.h"
#include "Graphics/Texture.h"
#include "Serialization/Binary.h"
#include "Serialization/Json.h"
#include "Tools/ImageTools.h"
#include "Util/Util.h"

namespace Real {

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

    void AssetImporter::SaveTextureToAssetDB(const OpenGLTexture *texture) {
        if (HasAssetWithPath(texture->GetPath()))
            return;

        const std::string uuidStr = std::to_string(texture->GetUUID());
        nlohmann::json& tex = m_AssetDB["textures"][uuidStr];

        tex["name"]      = texture->GetName();
        tex["stem"]      = texture->GetStem();
        tex["path"]      = texture->GetPath();
        tex["extension"] = texture->GetExtension();
        tex["type"]      = util::TextureType_EnumToString(texture->GetType());
        tex["image_format_state"] = util::ImageFormatState_EnumToString(texture->GetImageFormatState());

        CacheAssetWithPath(texture->GetPath(), texture->GetUUID());
        MarkDirtyAssetDB();
    }

    void AssetImporter::SaveMaterialToAssetDB(const Ref<Material> &mat) {
        if (HasAssetWithName(mat->m_Name))
            return;

        const std::string uuidStr = std::to_string(mat->m_UUID);
        nlohmann::json& material = m_AssetDB["materials"][uuidStr];
        material["name"] = mat->m_Name;

        material["textures"] = {
            { "albedo",   static_cast<uint64_t>(mat->m_Albedo)   },
            { "normal",   static_cast<uint64_t>(mat->m_Normal)   },
            { "orm",      static_cast<uint64_t>(mat->m_ORM)      },
            { "height",   static_cast<uint64_t>(mat->m_Height)   },
            { "emissive", static_cast<uint64_t>(mat->m_Emissive) }
        };

        CacheAssetWithName(mat->m_Name, mat->m_UUID);
        MarkDirtyAssetDB();
    }

    void AssetImporter::SaveModelToAssetDB(const Ref<Model> &model) {
        if (HasAssetWithPath(model->m_FileInfo.path))
            return;

        const std::string uuidStr = std::to_string(model->m_UUID);
        nlohmann::json& m = m_AssetDB["models"][uuidStr];

        // Binary file path
        m["binary"] = std::string(ASSETS_RUNTIME_DIR) + "models/" + model->m_Name + ".model";
        m["name"]   = model->m_Name; // Engine asset name

        // File info
        m["file_name"]      = model->m_FileInfo.name;
        m["file_stem"]      = model->m_FileInfo.stem;
        m["file_path"]      = model->m_FileInfo.path;
        m["file_extension"] = model->m_FileInfo.ext;

        CacheAssetWithPath(model->m_FileInfo.path, model->m_UUID);
        MarkDirtyAssetDB();
        Services::GetAssetManager()->SaveModelCPU(model);
    }

    void AssetImporter::SaveMeshToAssetDB(const MeshBinaryHeader &header, const std::string &name) {
        const auto binaryPath = std::string(ASSETS_RUNTIME_DIR) + "meshes/" + name + ".mesh";
        if (HasAssetWithPath(binaryPath))
            return;

        const std::string uuidStr = std::to_string(header.m_UUID);
        nlohmann::json& m = m_AssetDB["meshes"][uuidStr];

        // Binary file path
        m["binary"] = binaryPath;
        m["name"]   = name; // Engine asset name

        CacheAssetWithPath(binaryPath, UUID(header.m_UUID));
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
        const auto& am = Services::GetAssetManager();

        for (const auto& [uuidStr, tex_data] : m_AssetDB["textures"].items()) {
            UUID uuid;
            if (!util::TryParseUUID(uuidStr, uuid)) {
                Warn("Invalid UUID in Material DB");
                continue;
            }
            const auto type = util::TextureType_StringToEnum(tex_data["type"]);
            const auto ifs  = util::ImageFormatState_StringToEnum(tex_data["image_format_state"]);

            FileInfo fi;
            fi.name = tex_data.value("name", "null");
            fi.stem = tex_data.value("stem", "null");
            fi.path = tex_data.value("path", "null");
            fi.ext  = tex_data.value("extension", "null");

            Ref<OpenGLTexture> texture;
            if (ifs == ImageFormatState::COMPRESS_ME) {
                const auto td   = am->LoadTextureFromFile(fi.path, type);
                texture = CreateRef<OpenGLTexture>(td, true, type, ifs, fi, uuid);
                tools::CompressTextureAndReadFromFile(texture.get());
                UpdateTextureInAssetDB(texture.get());
            }
            else if (ifs == ImageFormatState::UNCOMPRESSED) {
                const auto td   = am->LoadTextureFromFile(fi.path, type);
                texture = CreateRef<OpenGLTexture>(td, true, type, ifs, fi, uuid);
            }
            else if (ifs == ImageFormatState::COMPRESSED) {
                texture = tools::ReadCompressedDataFromDDSFile(fi.path);
                texture->SetType(type);
                texture->SetImageFormatState(ifs);
            } else {
                Warn("[LoadTexturesFromAssetDB] Image format state is UNDEFINED: " + fi.path);
                return;
            }

            am->SaveTextureCPU(texture);
        }
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
            UUID meshUUID{header.m_UUID};
            Services::GetMeshManager()->CreateSingleMesh(vertices, indices, meshUUID);
        }
    }

    void AssetImporter::ImportModels() {
        const auto& am = Services::GetAssetManager();
        for (const auto& [uuidStr, modeldata] : m_AssetDB["models"].items()) {
            UUID uuid;
            if (!util::TryParseUUID(uuidStr, uuid)) {
                Warn("Invalid UUID in material DB");
                continue;
            }

            FileInfo info;
            info.name = modeldata["file_name"];
            info.stem = modeldata["file_stem"];
            info.path = modeldata["file_path"];
            info.ext  = modeldata["file_extension"];

            const auto& bPath = modeldata["binary"];
            const auto& [header, meshUUIDs, matUUIDs] = serialization::binary::LoadModel(bPath);

            const Ref<Model> model = CreateRef<Model>(uuid, info);
            model->m_MeshUUIDs = meshUUIDs;
            model->m_MaterialAssetUUIDs = matUUIDs;
            model->m_Name = modeldata["name"];

            if (header.m_UUID != 0 && header.m_UUID != uuid) {
                Warn("[AssetImporter] Model UUID mismatch!!! Binary UUID != AssetDbUUID fix it!");
            }

            am->SaveModelCPU(model);
        }
    }

    void AssetImporter::ImportMaterials() {
        const auto& am = Services::GetAssetManager();
        for (const auto& [uuidStr, mat_data] : m_AssetDB["materials"].items()) {
            UUID uuid;
            if (!util::TryParseUUID(uuidStr, uuid)) {
                Warn("Invalid UUID in Material DB");
                continue;
            }
            const std::string name = mat_data.value("name", "Material");

            const auto& mat = am->LoadMaterialBaseAsset(uuid, name);

            if (mat_data.contains("textures")) {
                const nlohmann::json& t = mat_data["textures"];

                mat->m_Albedo   = UUID(t.value("albedo",   0ULL));
                mat->m_Normal   = UUID(t.value("normal",   0ULL));
                mat->m_ORM      = UUID(t.value("orm",      0ULL));
                mat->m_Height   = UUID(t.value("height",   0ULL));
                mat->m_Emissive = UUID(t.value("emissive", 0ULL));
            }

            am->SaveMaterialCPU(mat);
        }
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
            const std::string name = material.value("name", "Material");
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

    void AssetImporter::CacheAssetWithName(const std::string &name, const UUID &uuid) {
        if (!m_NameToUUID.contains(name)) {
            m_NameToUUID.emplace(name, uuid);
        }
    }

    void AssetImporter::CacheAssetWithPath(const std::string &path, const UUID &uuid) {
        const std::string normalized = fs::NormalizePath(path);
        if (!m_PathToUUID.contains(normalized)) {
            m_PathToUUID.emplace(normalized, uuid);
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

    void AssetImporter::UpdateTextureInAssetDB(const OpenGLTexture *texture) {
        const std::string uuidStr = std::to_string(texture->GetUUID());
        auto& tex = m_AssetDB["textures"][uuidStr];

        tex["name"]  = texture->GetName();
        tex["stem"]  = texture->GetStem();
        tex["path"]  = texture->GetPath();
        tex["extension"] = texture->GetExtension();
        tex["type"]  = util::TextureType_EnumToString(texture->GetType());
        tex["image_format_state"] = util::ImageFormatState_EnumToString(texture->GetImageFormatState());

        MarkDirtyAssetDB();
    }

    void AssetImporter::LoadNewAssetsToDataBase() {
        // Update DB first if there is new assets
        UpdateAssetDB();

        for (const auto& mat : std::views::values(Services::GetAssetManager()->GetBaseMaterials())) {
            if (HasAssetWithName(mat->m_Name)) continue;

            for (const auto& tex : Services::GetAssetManager()->GetMaterialTextures(mat.get())) {
                if (HasAssetWithPath(tex->GetPath())) continue;

                if (tex->GetImageFormatState() == ImageFormatState::COMPRESS_ME) {
                    tools::CompressTextureAndReadFromFile(tex.get());
                }
                SaveTextureToAssetDB(tex.get());
            }
            SaveMaterialToAssetDB(mat);
        }

        UpdateAssetDB();
    }

    void AssetImporter::LoadTexturesFromFolder() {
        const auto& am = Services::GetAssetManager();
        std::unordered_map<std::string, std::array<Ref<OpenGLTexture>, 3>> m_ormPack;

        const auto SaveTexture = [this, &m_ormPack, am](const FileInfo& file, ImageFormatState imageFormatState) {
            auto& stem = file.stem;
            if (HasAssetWithPath(file.path) || am->IsTextureCompressed(stem))
                return;

            const auto dashPos = stem.find('_');
            const auto matName = stem.substr(0, dashPos);
            const TextureType type = util::TextureType_StringToEnum(stem.substr(dashPos + 1));

            const auto texData = am->LoadTextureFromFile(file.path, type);
            const auto texture = CreateRef<OpenGLTexture>(texData, true, type, imageFormatState, file);

            if (type == TextureType::AMBIENT_OCCLUSION) {
                m_ormPack[matName][0] = texture;
                return;
            }
            if (type == TextureType::ROUGHNESS) {
                m_ormPack[matName][1] = texture;
                return;
            }
            if (type == TextureType::METALLIC) {
                m_ormPack[matName][2] = texture;
                return;
            }

            if (imageFormatState == ImageFormatState::COMPRESS_ME) {
                tools::CompressTextureAndReadFromFile(texture.get());
            }

            am->SaveTextureCPU(texture);

            const auto& mat = am->GetOrCreateMaterialBase(matName);
            switch (type) {
                case TextureType::ALBEDO:   mat->m_Albedo   = texture->GetUUID(); break;
                case TextureType::NORMAL:   mat->m_Normal   = texture->GetUUID(); break;
                case TextureType::ORM:      mat->m_ORM      = texture->GetUUID(); break;
                case TextureType::HEIGHT:   mat->m_Height   = texture->GetUUID(); break;
                case TextureType::EMISSIVE: mat->m_Emissive = texture->GetUUID(); break;
                default: ;
            }
            am->SaveMaterialCPU(mat);
        };

        // Uncompressed State (Load from file)
        for (const auto& file : fs::IterateDirectory(ConcatStr(ASSETS_DIR, "textures/uncompressed/"))) {
            SaveTexture(file, ImageFormatState::UNCOMPRESSED);
        }

        // Compress_me State (Load from file)
        for (const auto& file : fs::IterateDirectory(ConcatStr(ASSETS_DIR, "textures/compress_me/"))) {
            SaveTexture(file, ImageFormatState::COMPRESS_ME);
        }

        // Process ORM textures
        for (const auto& [matName, pack] : m_ormPack) {
            const auto& ao  = pack[0] ? pack[0] : am->GetOrCreateDefaultTexture(TextureType::AMBIENT_OCCLUSION);
            const auto& rgh = pack[1] ? pack[1] : am->GetOrCreateDefaultTexture(TextureType::ROUGHNESS);
            const auto& mtl = pack[2] ? pack[2] : am->GetOrCreateDefaultTexture(TextureType::METALLIC);
            const auto& orm = tools::PackTexturesToRGBChannels(ao, rgh, mtl, matName);
            if (!orm || orm->GetImageFormatState() == ImageFormatState::DEFAULT) {
                continue;
            }
            am->GetOrCreateMaterialBase(matName)->m_ORM = orm->GetUUID();
            tools::CompressTextureAndReadFromFile(orm.get());
            am->SaveTextureCPU(orm);
        }
    }

    void AssetImporter::Update() {
        UpdateAssetDB();
    }

    bool AssetImporter::HasAssetWithName(const std::string &sourceName) const {
        return m_NameToUUID.contains(sourceName);
    }

    bool AssetImporter::HasAssetWithPath(const std::string &sourcePath) const {
        return m_PathToUUID.contains(fs::NormalizePath(sourcePath));
    }
}
