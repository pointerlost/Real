//
// Created by pointerlost on 10/4/25.
//
#include "Core/AssetManager.h"
#include <condition_variable>
#include "Core/Logger.h"
#include "Core/Utils.h"
#include <fstream>
#include <ranges>
#include <thread>
#include <Core/CmakeConfig.h>
#include "Graphics/Material.h"
#include "queue"
#include "Math/Math.h"
#include "Tools/ImageTools.h"
#include "Util/Util.h"
#include <stb/stb_image.h>

#include "Core/file_manager.h"
#include "Core/Services.h"
#include "Graphics/Model.h"
#include "Serialization/Binary.h"
#include "Serialization/Json.h"

namespace Real {

    AssetManager::AssetManager() {
        m_AssetDB = serialization::json::Load(ASSET_DB_PATH);
        s_LoadedAssetDB = true;

        if (!m_AssetDB.contains("textures") || !m_AssetDB["textures"].is_object())
            m_AssetDB["textures"] = nlohmann::json::object();

        if (!m_AssetDB.contains("materials") || !m_AssetDB["materials"].is_object())
            m_AssetDB["materials"] = nlohmann::json::object();
    }

    void AssetManager::SaveCPUTexture(const Ref<OpenGLTexture> &tex) {
        if (!m_Textures.contains(tex->GetUUID())) {
            m_Textures[tex->GetUUID()] = tex;
        }
    }

    void AssetManager::Update() {
        UpdateAssetDB();
    }

    void AssetManager::LoadNewAssetsToDataBase() {
        // If not already added, save the materials to the database
        for (const auto& mat : std::views::values(m_Materials)) {
            for (const auto& tex : GetMaterialTextures(mat.get())) {
                SaveTextureToAssetDB(GetTexture( tex->GetUUID(), tex->GetType() ).get());
                m_LoadedTexturesPath.insert(tex->GetPath());
            }
            SaveMaterialToAssetDB(mat);
        }
    }

    nlohmann::json& AssetManager::GetAssetDB() {
        if (!s_LoadedAssetDB) {
            m_AssetDB = serialization::json::Load(ASSET_DB_PATH);
            s_LoadedAssetDB = true;
        }
        return m_AssetDB;
    }

    void AssetManager::LoadShader(const std::string &vertexPath, const std::string &fragmentPath,
                                  const std::string& name)
    {
        const auto vertPath = PreprocessorForShaders(vertexPath);
        const auto fragPath = PreprocessorForShaders(fragmentPath);
        m_Shaders[name] = Shader{vertPath, fragPath, name};

        // Without preprocessors
        // const auto vert = File::ReadFromFile(vertexPath);
        // const auto frag = File::ReadFromFile(fragmentPath);
        // m_Shaders[name] = Shader{vert, frag, name};
    }

    std::string AssetManager::PreprocessorForShaders(const std::string &filePath) {
        using namespace std;

        string result;
        queue<string> glslContentQueue;

        if (!fs::File::Exists(filePath)) return {};

        ifstream stream(filePath, ios::in);
        if (!stream.is_open()) {
            Warn("File can't opening from: " + filePath);
            return{};
        }

        // Load main files like (vertex, fragment)
        std::string mainFileResult;
        std::string line;
        while (getline(stream, line)) {
            if (line.substr(0, 8) == "#include") {
                size_t pathFirstLine = 10;
                glslContentQueue.push(ConcatStr(SHADERS_DIR + line.substr(pathFirstLine, line.size() - pathFirstLine - 1)));
            } else if (line.substr(0, 1) != "#") {
                mainFileResult.append(ConcatStr(line + "\n"));
            } else if (line.substr(0, 7) != "#version") {
                result.append(ConcatStr(line + "\n"));
            }
        }

        // Merge all files containing '#include'
        while (!glslContentQueue.empty()) {
            line = "";
            std::string queuePath = glslContentQueue.front();
            glslContentQueue.pop();

            ifstream newStream(queuePath, ios::in);
            while (getline(newStream, line)) {
                if (line.substr(0, 8) == "#include") {
                    size_t pathFirstLine = 10;
                    glslContentQueue.push(ConcatStr(SHADERS_DIR + line.substr(pathFirstLine, line.size() - pathFirstLine - 1)));
                } else if (line.substr(0, 7) != "#version" || line.substr(0, 1) != "#") {
                    // Load into the beginning because main section should at the end
                    // result.append(ConcatStr(line + "\n"));
                    result += ConcatStr(line + "\n");
                }
                else if (line.substr(0, 5) == "#endif") {
                    break;
                }
            }
        }

        return ConcatStr(result + mainFileResult);
    }

    const Shader& AssetManager::GetShader(const std::string &name) {
        if (!m_Shaders.contains(name)) {
            Warn(ConcatStr("Shader Doesn't exists! Warn from the file: ", __FILE__));
        }
        return m_Shaders.at(name);
    }

    void AssetManager::LoadTextureArraysToGPU() const {
        // TextureArrayManager::PrepareAndBindTextureArrays();
    }

    Ref<OpenGLTexture>& AssetManager::GetOrCreateDefaultTexture(TextureType type) {
        if (m_DefaultTextures.contains(type))
            return m_DefaultTextures[type];

        const auto channelCount = util::TextureTypeToChannelCount(type);
        constexpr glm::ivec2 resolution{1, 1}; // If you want to compress default textures, pick 4x4 as resolution!!

        const Ref<OpenGLTexture> defaultTex = CreateRef<OpenGLTexture>();

        uint8_t channelColor[4] = {UINT8_MAX};
        // Pick default color for specific texture types to leave unharmed (materials,models etc.)
        switch (type) {
            case TextureType::ALBEDO:
                channelColor[0] = 128; channelColor[1] = 128;
                channelColor[2] = 128; channelColor[3] = 255; // Optional alpha
                break;

            case TextureType::NORMAL:
                channelColor[0] = 128; channelColor[1] = 128;
                channelColor[2] = 255; channelColor[3] = 255; // Optional alpha
                break;

            case TextureType::EMISSIVE:
                channelColor[0] = 0; channelColor[1] = 0;
                channelColor[2] = 0; channelColor[3] = 255;
                break;

            case TextureType::ROUGHNESS:
            case TextureType::AMBIENT_OCCLUSION:
                channelColor[0] = 255;
                break;

            case TextureType::METALLIC:
            case TextureType::HEIGHT:
                channelColor[0] = 0;
                break;

            default: channelColor[0] = UINT8_MAX; channelColor[1] = UINT8_MAX;
                     channelColor[2] = UINT8_MAX; channelColor[3] = UINT8_MAX;
        }

        const auto imageSize = resolution.x * resolution.y * channelCount;
        TextureData data;
        data.m_Data = new uint8_t[imageSize];
        auto* imageData = static_cast<uint8_t*>(data.m_Data);

        switch (channelCount) {
            case 1: // Grayscale
                for (size_t i = 0; i < imageSize; i+= channelCount) {
                    imageData[i + 0] = channelColor[0];
                }
                break;
            case 2:
                for (size_t i = 0; i < imageSize; i+= channelCount) {
                    imageData[i + 0] = channelColor[0];
                    imageData[i + 1] = channelColor[1];
                }
                break;
            case 3:
            case 4:
                for (size_t i = 0; i < imageSize; i+= channelCount) {
                    imageData[i + 0] = channelColor[0];
                    imageData[i + 1] = channelColor[1];
                    imageData[i + 2] = channelColor[2];
                    imageData[i + 3] = channelColor[3];
                }
                break;

            default:
                Warn("Channel count mismatch! from: " + std::string(__FILE__));
        }

        data.m_ChannelCount   = channelCount;
        data.m_DataSize       = imageSize;
        data.m_Width          = resolution.x;
        data.m_Height         = resolution.y;
        data.m_Format         = util::GetGLFormat(channelCount);
        data.m_InternalFormat = util::GetGLInternalFormat(channelCount);

        defaultTex->SetImageFormatState(ImageFormatState::DEFAULT);
        defaultTex->CreateFromData(data, type);
        m_Textures[defaultTex->GetUUID()] = defaultTex;
        return m_DefaultTextures[type] = defaultTex;
    }

    bool AssetManager::IsTextureCompressed(const std::string &stem) const {
        return fs::File::Exists(std::string(ASSETS_DIR) + "textures/compressed/" + stem + ".dds");
    }

    TextureData AssetManager::LoadTextureFromFile(const std::string &path, TextureType type) {
        if (!fs::File::Exists(path)) { Warn("There is no texture: " + path); }
        const int desiredChannels = type != TextureType::UNDEFINED ? util::TextureTypeToChannelCount(type) : 0;

        TextureData data;
        data.m_Data     = stbi_load(path.c_str(), &data.m_Width, &data.m_Height, &data.m_ChannelCount, desiredChannels);
        data.m_ChannelCount = desiredChannels != 0 ? desiredChannels : data.m_ChannelCount;
        // We are using bytesPerChannel = 1 because of using 8-bit textures
        data.m_DataSize = data.m_Width * data.m_Height * data.m_ChannelCount * 1;
        data.m_Format   = util::GetGLFormat(data.m_ChannelCount);
        data.m_InternalFormat = util::GetGLInternalFormat(data.m_ChannelCount);

        if (!data.m_Data) { Warn("[LoadTextureFromFile] stbi_load returning nullptr! Fix it"); }
        if (data.m_ChannelCount == 0) { Warn("[LoadTextureFromFile] has 0 channel count!!! Texture loading failed!"); }
        return data;
    }

    void AssetManager::LoadAssetsFromDataBase() {
        LoadDefaultTextures();
        LoadTexturesFromAssetDB();
        LoadMaterialsFromAssetDB();
        LoadMeshesFromAssetDB();
        LoadModelsFromAssetDB();

        std::unordered_map<std::string, std::array<Ref<OpenGLTexture>, 3>> m_ormPack;

        const auto SaveTexture = [this, &m_ormPack](const FileInfo& file, ImageFormatState image_state) {
            if (m_LoadedTexturesPath.contains(file.path)) return; // If already loaded skip it

            auto& stem = file.stem;
            const auto dashPos = stem.find('_');
            const auto matName = stem.substr(0, dashPos);
            const TextureType type = util::TextureType_StringToEnum(stem.substr(dashPos + 1));

            const auto texData = LoadTextureFromFile(file.path, type);
            const auto texture = CreateRef<OpenGLTexture>(texData, true, type, image_state, file);

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

            if (image_state == ImageFormatState::COMPRESS_ME) {
                tools::CompressTextureAndReadFromFile(texture.get());
            }
            m_Textures[texture->GetUUID()] = texture;

            const auto& mat = GetOrCreateMaterialBase(matName);
            switch (type) {
                case TextureType::ALBEDO:   mat->m_Albedo   = texture->GetUUID(); break;
                case TextureType::NORMAL:   mat->m_Normal   = texture->GetUUID(); break;
                case TextureType::ORM:      mat->m_ORM      = texture->GetUUID(); break;
                case TextureType::HEIGHT:   mat->m_Height   = texture->GetUUID(); break;
                case TextureType::EMISSIVE: mat->m_Emissive = texture->GetUUID(); break;
                default: ;
            }
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
            const auto& ao  = pack[0] ? pack[0] : GetOrCreateDefaultTexture(TextureType::AMBIENT_OCCLUSION);
            const auto& rgh = pack[1] ? pack[1] : GetOrCreateDefaultTexture(TextureType::ROUGHNESS);
            const auto& mtl = pack[2] ? pack[2] : GetOrCreateDefaultTexture(TextureType::METALLIC);
            const auto& orm = tools::PackTexturesToRGBChannels(ao, rgh, mtl, matName);
            if (!orm || orm->GetImageFormatState() == ImageFormatState::DEFAULT) {
                continue;
            }
            m_LoadedTexturesPath.insert(orm->GetPath());
            GetOrCreateMaterialBase(matName)->m_ORM = orm->GetUUID();
            tools::CompressTextureAndReadFromFile(orm.get());
        }

        // Bulk upload
        UpdateAssetDB();
    }

    void AssetManager::DeleteCPUTexture(const UUID &uuid) {
        if (m_Textures.contains(uuid)) {
            m_Textures.erase(uuid);
        }
    }

    const Ref<OpenGLTexture>& AssetManager::GetTexture(const UUID &uuid, TextureType type) {
        if (!m_Textures.contains(uuid)) {
            const auto& tex = GetOrCreateDefaultTexture(type);
            m_Textures[tex->GetUUID()] = tex;
            return tex;
        }
        return m_Textures[uuid];
    }

    Ref<Material>& AssetManager::GetOrCreateMaterialBase(const UUID& uuid) {
        auto [it, inserted] = m_Materials.try_emplace(uuid);
        if (inserted) {
            it->second = CreateRef<Material>(uuid);
        }
        return it->second;
    }

    Ref<Material>& AssetManager::GetOrCreateMaterialBase(const std::string& name) {
        if (m_MaterialNameToUUID.contains(name)) {
            if (m_Materials.contains(m_MaterialNameToUUID[name]))
                return m_Materials[m_MaterialNameToUUID[name]];
        }
        return m_Materials[m_MaterialNameToUUID[name]] = CreateRef<Material>();
    }

    Ref<MaterialInstance>& AssetManager::GetOrCreateMaterialInstance(const UUID& uuid) {
        if (m_MaterialInstances.contains(uuid))
            return m_MaterialInstances[uuid];
        return m_MaterialInstances[uuid] = CreateRef<MaterialInstance>(uuid);
    }

    std::vector<GLuint64> AssetManager::UploadTexturesToGPU() const {
        std::vector<GLuint64> bindlessIDs;
        for (const auto& tex : std::views::values(m_Textures)) {
            tex->PrepareOptionsAndUploadToGPU();
            if (!tex->HasBindlessHandle()) {
                Warn("There is no bindless handle for this texture!");
                continue;
            }
            tex->SetIndex(bindlessIDs.size());
            bindlessIDs.push_back(tex->GetBindlessHandle());

            // Clean the texture data after uploading it to the GPU
            tex->CleanUpCPUData();
        }
        return bindlessIDs;
    }

    void AssetManager::AddFontStyle(const std::string &fontName, ImFont *font) {
        if (!font) {
            Warn("font is nullptr! name: " + fontName + "from: " + __FILE__);
            return;
        }
        if (m_Fonts.contains(fontName)) {
            Warn("font already exists with this name: " + fontName + " use a new name!");
            return;
        }
        m_Fonts[fontName] = font;
    }

    ImFont* AssetManager::GetFontStyle(const std::string &fontName) {
        if (m_Fonts.contains(fontName)) {
            return m_Fonts[fontName];
        }
        Warn(ConcatStr("Font doesn't exists! from: ", __FILE__, "\n name: ", fontName));
        return nullptr;
    }

    bool AssetManager::IsModelExist(const std::string &name) {
        if (m_ModelNameToUUID.contains(name) && m_Models.contains(m_ModelNameToUUID[name]))
            return true;
        return false;
    }

    bool AssetManager::IsMaterialExist(const std::string &name) {
        if (m_MaterialNameToUUID.contains(name) && m_Materials.contains(m_MaterialNameToUUID[name]))
            return true;
        return false;
    }

    void AssetManager::SaveModelToAssetDB(const Ref<Model>& model) {
        if (m_Models.contains(model->m_UUID)) return;

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

        MarkDirtyAssetDB();
        SaveModelCPU(model);
    }

    void AssetManager::SaveModelCPU(const Ref<Model> &model) {
        if (m_Models.contains(model->m_UUID)) return;
        m_Models[model->m_UUID] = model;
    }

    void AssetManager::LoadModelsFromAssetDB() {
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

            const auto& b_path = modeldata["binary"];
            const auto& b_header = serialization::binary::LoadModel(b_path);

            const auto& model = CreateRef<Model>(b_header, info);
            model->m_Name = modeldata["name"];

            m_Models[uuid] = model;
            m_ModelNameToUUID[model->m_Name] = uuid;
        }
    }

    void AssetManager::LoadMaterialsFromAssetDB() {
        for (const auto& [uuidStr, mat_data] : m_AssetDB["materials"].items()) {
            UUID uuid;
            if (!util::TryParseUUID(uuidStr, uuid)) {
                Warn("Invalid UUID in Material DB");
                continue;
            }
            const std::string name = mat_data.value("name", "Material");
            if (m_MaterialNameToUUID.contains(name)) continue; // if material already added skip it

            const Ref<Material> mat = CreateRef<Material>(uuid);
            mat->m_Name = name;

            if (mat_data.contains("textures")) {
                const nlohmann::json& t = mat_data["textures"];

                mat->m_Albedo   = UUID(t.value("albedo",   0ULL));
                mat->m_Normal   = UUID(t.value("normal",   0ULL));
                mat->m_ORM      = UUID(t.value("orm",      0ULL));
                mat->m_Height   = UUID(t.value("height",   0ULL));
                mat->m_Emissive = UUID(t.value("emissive", 0ULL));
            }

            m_Materials[uuid] = mat; // Material registry
            m_MaterialNameToUUID[name] = uuid; // Cache material
        }
    }

    void AssetManager::SaveMaterialToAssetDB(const Ref<Material>& mat) {
        const std::string uuidStr = std::to_string(mat->m_UUID);
        if (m_AssetDB["materials"].contains(uuidStr)) return; // If it already exists

        nlohmann::json& material = m_AssetDB["materials"][uuidStr];

        material["name"] = mat->m_Name;

        material["textures"] = {
            { "albedo",   static_cast<uint64_t>(mat->m_Albedo)   },
            { "normal",   static_cast<uint64_t>(mat->m_Normal)   },
            { "orm",      static_cast<uint64_t>(mat->m_ORM)      },
            { "height",   static_cast<uint64_t>(mat->m_Height)   },
            { "emissive", static_cast<uint64_t>(mat->m_Emissive) }
        };
        MarkDirtyAssetDB();
    }

    void AssetManager::SaveTextureToAssetDB(const OpenGLTexture *texture) {
        const std::string uuidStr = std::to_string(texture->GetUUID());
        if (m_AssetDB["textures"].contains(uuidStr)) return; // If it already exists

        nlohmann::json& tex = m_AssetDB["textures"][uuidStr];

        tex["name"]      = texture->GetName();
        tex["stem"]      = texture->GetStem();
        tex["path"]      = texture->GetPath();
        tex["extension"] = texture->GetExtension();
        tex["type"]      = util::TextureType_EnumToString(texture->GetType());
        tex["image_format_state"] = util::ImageFormatState_EnumToString(texture->GetImageFormatState());

        MarkDirtyAssetDB();
    }

    void AssetManager::LoadTexturesFromAssetDB() {
        for (const auto& [uuidStr, tex_data] : m_AssetDB["textures"].items()) {
            FileInfo fi;
            fi.name = tex_data.value("name", "null");
            fi.stem = tex_data.value("stem", "null");
            fi.path = tex_data.value("path", "null");
            fi.ext  = tex_data.value("extension", "null");

            UUID uuid;
            if (!util::TryParseUUID(uuidStr, uuid)) {
                Warn("Invalid UUID in Material DB");
                continue;
            }
            const auto type = util::TextureType_StringToEnum(tex_data["type"]);
            const auto td   = LoadTextureFromFile(fi.path, type);
            const auto ifs  = util::ImageFormatState_StringToEnum(tex_data["image_format_state"]);

            const auto texture = CreateRef<OpenGLTexture>(td, true, type, ifs, fi, uuid);

            m_Textures[uuid] = texture;
            m_LoadedTexturesPath.insert(fi.path); // Cache texture
        }
    }

    void AssetManager::SaveMeshToAssetDB(const MeshBinaryHeader &header, const std::string& name) {
        const std::string uuidStr = std::to_string(header.m_MeshUUID);
        if (m_AssetDB["meshes"].contains(uuidStr)) return;

        nlohmann::json& m = m_AssetDB["meshes"][uuidStr];

        // Binary file path
        m["binary"] = std::string(ASSETS_RUNTIME_DIR) + "models/" + name + ".mesh";
        m["name"]   = name; // Engine asset name

        MarkDirtyAssetDB();
    }

    void AssetManager::LoadMeshesFromAssetDB() {
        for (const auto& [uuidStr, mesh_data] : m_AssetDB["meshes"].items()) {
            const auto& path = mesh_data["binary"];
            // Save meshes to mesh manager
            serialization::binary::LoadMesh(path);
        }
    }

    void AssetManager::RenameMaterial(const std::string &newName, const UUID &uuid) {
        const std::string uuidStr = std::to_string(uuid);

        if (!m_AssetDB["materials"].contains(uuidStr))
            return;

        // Update JSON
        m_AssetDB["materials"][uuidStr]["name"] = newName;

        // TODO: Should i change the texture names as well???

        // Update run-time caches
        const auto& mat = m_Materials[uuid];
        m_MaterialNameToUUID.erase(mat->m_Name); // old name
        mat->m_Name = newName;
        m_MaterialNameToUUID[newName] = uuid;

        MarkDirtyAssetDB();
    }

    void AssetManager::MarkDirtyAssetDB() {
        m_AssetDBDirty = true;
    }

    void AssetManager::UpdateAssetDB() {
        if (!m_AssetDBDirty) return;
        serialization::json::Save(ASSET_DB_PATH, m_AssetDB);
        m_AssetDBDirty = false;
    }

    void AssetManager::LoadDefaultTextures() {
        auto LoadDefaultTex = [this](TextureType type) {
            if (!m_DefaultTextures.contains(type)) {
                GetOrCreateDefaultTexture(type);
            }
        };

        for (int i = 0; i <= (int)TextureType::EMISSIVE; i++) {
            LoadDefaultTex(static_cast<TextureType>(i));
        }
    }

    std::vector<Ref<OpenGLTexture>> AssetManager::GetMaterialTextures(const Material *mat) {
        std::vector<Ref<OpenGLTexture>> textures;

        auto tryAddTexture = [&](const UUID textureId) {
            if (const auto it = m_Textures.find(textureId); it != m_Textures.end()) {
                if (it->second->GetImageFormatState() == ImageFormatState::DEFAULT) return;
                textures.push_back(it->second);
            }
        };

        tryAddTexture(mat->m_Albedo);
        tryAddTexture(mat->m_Normal);
        tryAddTexture(mat->m_ORM);
        tryAddTexture(mat->m_Height);
        tryAddTexture(mat->m_Emissive);

        return textures;
    }
}
