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

    void AssetManager::Update() {
        UpdateAssetDB();
    }

    nlohmann::json AssetManager::GetAssetDB() {
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

        if (!fs::File::Exists(filePath)) {
            Warn("Path doesn't exists: " + filePath);
            return{};
        }

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

    Ref<OpenGLTexture> AssetManager::GetOrCreateDefaultTexture(TextureType type) {
        if (m_DefaultTextures.contains(type))
            return m_DefaultTextures[type];

        const auto channelCount = util::TextureTypeToChannelCount(type);
        constexpr glm::ivec2 resolution{1, 1}; // If I want to compress my default textures, pick 4x4 as resolution!!

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
        data.m_Format         = util::ConvertChannelCountToGLFormat(channelCount);
        data.m_InternalFormat = util::ConvertChannelCountToGLInternalFormat(channelCount);

        defaultTex->SetImageFormatState(ImageFormatState::DEFAULT);
        defaultTex->CreateFromData(data, type);
        return m_DefaultTextures[type] = defaultTex;
    }

    bool AssetManager::IsTextureCompressed(const std::string &stem) const {
        return fs::File::Exists(std::string(ASSETS_DIR) +  "textures/compressed/" + stem + ".dds");
    }

    Ref<OpenGLTexture> AssetManager::LoadTextureOnlyCPUData(const FileInfo& file, TextureType type, ImageFormatState imageState, const UUID& uuid) {
        if (m_Textures.contains(uuid)) return m_Textures[uuid];

        const Ref<OpenGLTexture> texture = CreateRef<OpenGLTexture>();
        texture->SetType(type);
        texture->SetImageFormatState(imageState);
        texture->SetFileInfo(file);
        texture->SetIndex(m_Textures.size());
        texture->Create();

        return m_Textures[uuid] = texture;
    }

    Ref<OpenGLTexture> AssetManager::LoadTextureOnlyCPUData(const std::string &path, TextureType type,
        ImageFormatState imageState, const UUID& uuid)
    {
        if (m_Textures.contains(uuid)) return m_Textures[uuid];

        const Ref<OpenGLTexture> texture = CreateRef<OpenGLTexture>();
        texture->SetType(type);
        texture->SetImageFormatState(imageState);
        texture->SetFileInfo(std::move(fs::CreateFileInfoFromPath(path)));
        texture->SetIndex(m_Textures.size());
        texture->Create();

        return m_Textures[uuid] = texture;
    }

    TextureData AssetManager::LoadTextureFromFile(const std::string &path) {
        TextureData data;
        unsigned char* rawData = stbi_load(path.c_str(), &data.m_Width, &data.m_Height, &data.m_ChannelCount, 0);
        const auto size = data.m_Width * data.m_Height * data.m_ChannelCount;
        data.m_Data = new uint8_t[size];
        memcpy(data.m_Data, rawData, size);
        stbi_image_free(rawData);
        return data;
    }

    void AssetManager::LoadAssets() {
        LoadTexturesFromAssetDB();
        LoadMaterialsFromAssetDB();

        const auto SaveTexture = [this](const FileInfo& file, ImageFormatState image_state) {
            auto& stem = file.stem;
            const auto dashPos = stem.find('_');
            if (m_LoadedTexturesPath.contains(file.path)) return;

            const auto matName = stem.substr(0, dashPos);
            const TextureType type = util::TextureType_StringToEnum(stem.substr(dashPos + 1));

            const auto texData = LoadTextureFromFile(file.path);
            const auto texture = CreateRef<OpenGLTexture>(texData, type, image_state, file);
            if (image_state == ImageFormatState::COMPRESS_ME) {
                tools::CompressTextureAndReadFromFile(texture.get());
            }
            m_Textures[texture->GetUUID()] = texture;
            SaveTextureToAssetDB(texture.get());

            const auto& mat = GetOrCreateMaterialBase(matName);
            switch (type) {
                case TextureType::ALBEDO:            mat->m_Albedo    = texture->GetUUID(); break;
                case TextureType::NORMAL:            mat->m_Normal    = texture->GetUUID(); break;
                case TextureType::AMBIENT_OCCLUSION: mat->m_AO        = texture->GetUUID(); break;
                case TextureType::ROUGHNESS:         mat->m_Roughness = texture->GetUUID(); break;
                case TextureType::METALLIC:          mat->m_Metallic  = texture->GetUUID(); break;
                case TextureType::ORM:               mat->m_ORM       = texture->GetUUID(); break;
                case TextureType::HEIGHT:            mat->m_Height    = texture->GetUUID(); break;
                case TextureType::EMISSIVE:          mat->m_Emissive  = texture->GetUUID(); break;
                default: ;
            }
        };

        // Uncompressed State
        for (const auto& file : fs::IterateDirectory(ConcatStr(ASSETS_DIR, "textures/uncompressed/"))) {
            SaveTexture(file, ImageFormatState::UNCOMPRESSED);
        }

        // Compress_me State
        for (const auto& file : fs::IterateDirectory(ConcatStr(ASSETS_DIR, "textures/compress_me/"))) {
            SaveTexture(file, ImageFormatState::COMPRESS_ME);
        }

        // If not already added, save the materials to the database
        for (const auto& mat : std::views::values(m_Materials)) {
            const auto& ao  = GetTexture(mat->m_AO,        TextureType::AMBIENT_OCCLUSION);
            const auto& rgh = GetTexture(mat->m_Roughness, TextureType::ROUGHNESS);
            const auto& mtl = GetTexture(mat->m_Metallic,  TextureType::METALLIC);
            const auto& orm = tools::PackTexturesToRGBChannels(ao, rgh, mtl, mat->m_Name);
            tools::CompressTextureAndReadFromFile(orm.get());
            SaveTextureToAssetDB(orm.get());

            mat->m_ORM = orm->GetUUID();
            SaveMaterialToAssetDB(mat);
        }
    }

    void AssetManager::DeleteCPUTexture(const UUID &uuid) {
        if (m_Textures.contains(uuid)) {
            m_Textures.erase(uuid);
        }
    }

    Ref<OpenGLTexture> AssetManager::GetTexture(const UUID &uuid, TextureType type) {
        if (!m_Textures.contains(uuid)) {
            Warn("There is no texture with this UUID! returning nullptr!!!");
            return GetOrCreateDefaultTexture(type);
        }
        return m_Textures[uuid];
    }

    Ref<Material>& AssetManager::GetOrCreateMaterialBase(const UUID& uuid) {
        if (m_Materials.contains(uuid)) {
            return m_Materials[uuid];
        }
        return m_Materials[uuid] = CreateRef<Material>();
    }

    Ref<Material> AssetManager::GetOrCreateMaterialBase(const std::string& name) {
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
        }
        return bindlessIDs;
    }

    void AssetManager::MakeTexturesResident() const {
        for (const auto& tex : std::views::values(m_Textures)) {
            if (!tex->HasBindlessHandle()) {
                tex->CreateBindless();
                Warn("There is no bindless handle for this texture!");
            }
            tex->MakeResident();
        }
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

        m_Models[model->m_UUID] = model;
    }

    void AssetManager::SaveModelCPU(const Ref<Model> &model) {
        if (m_Models.contains(model->m_UUID)) return;
        m_Models[model->m_UUID] = model;
    }

    void AssetManager::LoadModelsFromAssetDB() {
        for (const auto& [uuidStr, modeldata] : m_AssetDB["models"].items()) {
            UUID uuid;
            if (!util::TryParseUUID(uuidStr, uuid)) {
                Warn("Invalıd UUID in material DB");
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
        }
    }

    void AssetManager::LoadMaterialsFromAssetDB() {
        for (const auto& [uuidStr, matData] : m_AssetDB["materials"].items()) {
            UUID uuid;
            if (!util::TryParseUUID(uuidStr, uuid)) {
                Warn("Invalıd UUID in material DB");
                continue;
            }
            const std::string name = matData.value("name", "Material");
            if (m_MaterialNameToUUID.contains(name)) continue; // if material already added skip it

            const Ref<Material> mat = CreateRef<Material>(uuid);
            mat->m_Name = name;

            if (matData.contains("textures")) {
                const nlohmann::json& t = matData["textures"];

                mat->m_Albedo   = UUID(t.value("albedo",   0ULL));
                mat->m_Normal   = UUID(t.value("normal",   0ULL));
                mat->m_ORM      = UUID(t.value("orm",      0ULL));
                mat->m_Height   = UUID(t.value("height",   0ULL));
                mat->m_Emissive = UUID(t.value("emissive", 0ULL));
            }

            m_Materials[uuid] = mat; // your material registry
            m_MaterialNameToUUID[name] = uuid; // Cache material
        }
    }

    void AssetManager::SaveMaterialToAssetDB(const Ref<Material>& mat) {
        const std::string uuidStr = std::to_string(mat->m_ID);

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
        for (const auto& [uuidStr, texdata] : m_AssetDB["textures"].items()) {
            FileInfo fi;
            fi.name = texdata.value("name", "null");
            fi.stem = texdata.value("stem", "null");
            fi.path = texdata.value("path", "null");
            fi.ext  = texdata.value("extension", "null");

            if (m_LoadedTexturesPath.contains(fi.path)) continue; // If texture already added skip it

            const auto td   = LoadTextureFromFile(fi.path);
            const auto type = util::TextureType_StringToEnum(texdata["type"]);
            const auto ifs  = util::ImageFormatState_StringToEnum(texdata["image_format_state"]);
            const auto uuid = UUID(std::stoull(uuidStr));

            const auto texture = CreateRef<OpenGLTexture>(td, type, ifs, fi, uuid);

            m_Textures[uuid] = texture;
            m_LoadedTexturesPath.insert(fi.path); // Cache texture
        }
    }

    void AssetManager::RenameMaterial(const std::string &newName, const UUID &uuid) {
        const std::string uuidStr = std::to_string(uuid);

        if (!m_AssetDB["materials"].contains(uuidStr))
            return;

        // Update JSON
        m_AssetDB["materials"][uuidStr]["name"] = newName;

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
}
