//
// Created by pointerlost on 10/4/25.
//
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
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


namespace Real {

    AssetManager::AssetManager() {
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

        if (!File::Exists(filePath)) {
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

    Ref<OpenGLTexture> AssetManager::GetOrCreateDefaultTexture(const std::string& name, TextureType type, const glm::ivec2 &resolution, int channelCount) {
        if (m_DefaultTextures.contains(name))
            return m_DefaultTextures[name];

        const Ref<OpenGLTexture> defaultTex = CreateRef<OpenGLTexture>();
        const auto imageSize = resolution.x * resolution.y * channelCount;
        auto* imageData = new uint8_t[imageSize];

        uint8_t channelColor[4] = {UINT8_MAX};
        // Pick default color for specific texture types to leave unharmed
        switch (type) {
            case TextureType::ALBEDO:
                channelColor[0] = 128; channelColor[1] = 128;
                channelColor[2] = 128; channelColor[3] = 255; // Optional alpha
                break;

            case TextureType::NORMAL:
                channelColor[0] = 128; channelColor[1] = 128;
                channelColor[2] = 255; channelColor[3] = 255; // Optional alpha
                break;

            case TextureType::ROUGHNESS: channelColor[0] = 128; break;
            case TextureType::METALLIC:
            case TextureType::HEIGHT:
                channelColor[0] = 0; break;

            case TextureType::AMBIENT_OCCLUSION: channelColor[0] = 255; break;

            default: channelColor[0] = UINT8_MAX; channelColor[1] = UINT8_MAX;
                     channelColor[2] = UINT8_MAX; channelColor[3] = UINT8_MAX;
        }

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
        TextureData data;

        data.m_Data = new uint8_t[imageSize];
        memcpy(data.m_Data, imageData, imageSize);
        delete[] imageData;

        data.m_ChannelCount = channelCount;
        data.m_DataSize = imageSize;
        data.m_Width    = resolution.x;
        data.m_Height   = resolution.y;
        data.m_Format   = util::ConvertChannelCountToGLFormat(channelCount);
        data.m_InternalFormat = util::ConvertChannelCountToGLInternalFormat(channelCount);

        defaultTex->SetImageFormatState(ImageFormatState::DEFAULT);
        defaultTex->CreateFromData(data, type);
        return m_DefaultTextures[name] = defaultTex;
    }

    bool AssetManager::IsTextureCompressed(const std::string &stem) const {
        return File::Exists(std::string(ASSETS_DIR) +  "textures/compressed/" + stem + ".dds");
    }

    Ref<OpenGLTexture> AssetManager::LoadTextureOnlyCPUData(const FileInfo& file, TextureType type, ImageFormatState imageState) {
        if (m_TexturesByName.contains(file.name)) return m_TexturesByName[file.name];

        const Ref<OpenGLTexture> texture = CreateRef<OpenGLTexture>();
        texture->SetType(type);
        texture->SetImageFormatState(imageState);
        texture->SetFileInfo(file);
        texture->SetIndex(m_TexturesByName.size());
        texture->Create();

        return m_TexturesByName[file.name] = texture;
    }

    Ref<OpenGLTexture> AssetManager::LoadTextureOnlyCPUData(const std::string &path, TextureType type,
        ImageFormatState imageState)
    {
        if (m_TexturesByPath.contains(path)) return m_TexturesByPath[path];

        const Ref<OpenGLTexture> texture = CreateRef<OpenGLTexture>();
        texture->SetType(type);
        texture->SetImageFormatState(imageState);
        texture->SetFileInfo(std::move(util::CreateFileInfoFromPath(path)));
        texture->SetIndex(m_TexturesByName.size());
        texture->Create();

        return m_TexturesByPath[path] = texture;
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

        const auto SaveTexture = [this](const FileInfo& file, ImageFormatState imageState) {
            auto& stem = file.stem;
            const auto dashPos = stem.find('_');

            const auto matName = stem.substr(0, dashPos);
            const TextureType type = util::TextureType_StringToEnum(stem.substr(dashPos + 1));

            const auto texData = LoadTextureFromFile(file.path);
            switch (type) {
            }
            const auto tex = LoadTextureOnlyCPUData(file, type, imageState);

            const auto& mat = GetOrCreateMaterialBase(uuid);
            switch (type) {
                case TextureType::ALBEDO:            mat->m_Albedo    = tex; break;
                case TextureType::NORMAL:            mat->m_Normal    = tex; break;
                case TextureType::AMBIENT_OCCLUSION: mat->m_AO        = tex; break;
                case TextureType::METALLIC:          mat->m_Metallic  = tex; break;
                case TextureType::ROUGHNESS:         mat->m_Roughness = tex; break;
                case TextureType::ORM:               mat->m_ORM       = tex; break;
                case TextureType::HEIGHT:            mat->m_Height    = tex; break;
                case TextureType::EMISSIVE:          mat->m_Emissive  = tex; break;
                default: ;
            }
        };

        // Uncompressed State
        for (const auto& file : util::IterateDirectory(ConcatStr(ASSETS_DIR, "textures/uncompressed/"))) {
            SaveTexture(file, ImageFormatState::UNCOMPRESSED);
        }

        // Compress_me State
        for (const auto& file : util::IterateDirectory(ConcatStr(ASSETS_DIR, "textures/compress_me/"))) {
            SaveTexture(file, ImageFormatState::COMPRESS_ME);
        }

        // Compress textures and read from DDS
        for (const auto& tex : std::views::values(m_TexturesByName)) {
            if (tex->GetImageFormatState() != ImageFormatState::COMPRESS_ME) continue;
            tools::CompressTextureToBCn(tex.get(), std::string(ASSETS_DIR) + "textures/compressed/");
        }
    }

    Ref<OpenGLTexture> & AssetManager::GetTexture(const UUID &uuid) {
        if (m_Textures.contains(uuid)) {
            Warn("There is no texture with this UUID! returning nullptr!!!");
        }
        return m_Textures[uuid];
    }

    Ref<Material>& AssetManager::GetOrCreateMaterialBase(const UUID& uuid) {
        if (m_Materials.contains(uuid))
            return m_Materials[uuid];
        return m_Materials[uuid] = CreateRef<Material>();
    }

    Ref<MaterialInstance>& AssetManager::GetOrCreateMaterialInstance(const UUID& uuid) {
        if (m_MaterialInstances.contains(uuid))
            return m_MaterialInstances[uuid];
        // Should I save this UUID to asset database?
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

    nlohmann::json AssetManager::LoadMaterialsFromAssetDB() {
        using nlohmann::json;
        json j = util::LoadJSON(std::string(ASSETS_DIR) + "asset_database/asset_database.json");

        const auto mat = CreateRef<Material>();

        mat->m_ID = UUID(j.value("uuid", 0ull));
        mat->m_Name = j.value("name", "Unnamed");

        auto loadTexUUID = [&](const std::string& key) -> UUID {
            const uint64_t id = j["textures"].value(key, 0ull);
            if (id == 0ull)
                Warn("There is no UUID for this material: " + mat->m_Name);
            return {id};
        };

        mat->m_Albedo   = loadTexUUID("albedo");
        mat->m_Normal   = loadTexUUID("normal");
        mat->m_ORM      = loadTexUUID("orm");
        mat->m_Height   = loadTexUUID("height");
        mat->m_Emissive = loadTexUUID("emissive");

        return j;
    }

    nlohmann::json AssetManager::SaveMaterialToAssetDB(const Ref<Material>& mat) {
        using nlohmann::json;
        json j = util::LoadJSON(std::string(ASSETS_DIR) + "asset_database/asset_database.json");

        // nlohmann doesn't know custom type, so cast to uint64_t
        j["uuid"] = static_cast<uint64_t>(mat->m_ID);
        // Save name for debugging purposes, don't use like a key
        j["name"] = mat->m_Name;

        j["textures"] = {
            { "albedo",    static_cast<uint64_t>(mat->m_Albedo)   },
            { "normal",    static_cast<uint64_t>(mat->m_Normal)   },
            { "orm",       static_cast<uint64_t>(mat->m_ORM)      },
            { "height",    static_cast<uint64_t>(mat->m_Height)   },
            { "emissive",  static_cast<uint64_t>(mat->m_Emissive) },
        };

        return j;
    }

    nlohmann::json AssetManager::SaveTextureToAssetDB(const OpenGLTexture *texture) {
        using nlohmann::json;
        json j = util::LoadJSON(std::string(ASSETS_DIR) + "asset_database/asset_database.json");

        // nlohmann doesn't know custom type, so cast to uint64_t
        j["uuid"] = static_cast<uint64_t>(texture->GetUUID());
        // Save file_info for ui or debugging purposes
        j["file_info"] = {
            { "name",      texture->GetName(), },
            { "stem",      texture->GetStem(), },
            { "path",      texture->GetPath(), },
            { "extension", texture->GetExtension(), }
        };

        j["type"] = util::TextureType_EnumToString(texture->GetType());
        j["image_format_state"] = util::ImageFormatState_EnumToString(texture->GetImageFormatState());
    }
}
