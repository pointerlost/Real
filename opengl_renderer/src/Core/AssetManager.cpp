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

#include "Core/AssetImporter.h"
#include "Core/file_manager.h"
#include "Core/Services.h"
#include "Graphics/Model.h"
#include "Serialization/Binary.h"
#include "Serialization/Json.h"

namespace Real {

    AssetManager::AssetManager() {
        LoadDefaultTextures();
    }

    void AssetManager::SaveTextureCPU(const Ref<OpenGLTexture> &tex) {
        if (!m_Textures.contains(tex->GetUUID())) {
            m_Textures.emplace(tex->GetUUID(), tex);
        }
    }

    void AssetManager::Update() {
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

    std::string AssetManager::GenerateUniqueMaterialName(const std::string &desiredName) {
        std::string base = NormalizeMaterialName(desiredName);

        // Fast path
        if (!m_MaterialNameToUUID.contains(base))
            return base;

        uint32_t index = 1;
        std::string candidate;

        do {
            candidate = base + "_" + std::to_string(index++);
        } while (m_MaterialNameToUUID.contains(candidate));

        return candidate;
    }

    std::string AssetManager::NormalizeMaterialName(std::string name) {
        Trim(name);
        if (name.empty()) name = "New_Material";
        return name;
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

    void AssetManager::SaveModelCPU(const Ref<Model> &model) {
        if (!m_Models.contains(model->m_UUID)) {
            m_Models.emplace(model->m_UUID, model);
        }
    }

    void AssetManager::SaveMaterialCPU(const Ref<Material> &material) {
        if (!m_Materials.contains(material->m_UUID)) {
            m_Materials.emplace(material->m_UUID, material);
        }
    }

    const std::unordered_map<UUID, Ref<Material>>& AssetManager::GetBaseMaterials() {
        return m_Materials;
    }

    void AssetManager::RenameMaterial(const std::string &newName, const UUID &uuid) {
        const std::string uuidStr = std::to_string(uuid);

        const auto& ai = Services::GetAssetImporter();
        auto& db = ai->GetAssetDB();

        // Update JSON
        db["materials"][uuidStr]["name"] = newName;

        // TODO: Should i change the texture names as well???

        // Update run-time caches
        const auto& mat = m_Materials[uuid];
        m_MaterialNameToUUID.erase(mat->m_Name); // old name
        mat->m_Name = newName;
        m_MaterialNameToUUID[newName] = uuid;

        ai->MarkDirtyAssetDB();
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

        auto tryAddTexture = [this](const UUID textureId, std::vector<Ref<OpenGLTexture>>& outTextures) {
            if (textureId == 0) return; // Skip invalid UUID
            const auto it = m_Textures.find(textureId);
            if (it != m_Textures.end() && it->second && it->second->GetImageFormatState() != ImageFormatState::DEFAULT) {
                outTextures.push_back(it->second);
            }
        };

        tryAddTexture(mat->m_Albedo, textures);
        tryAddTexture(mat->m_Normal, textures);
        tryAddTexture(mat->m_ORM, textures);
        tryAddTexture(mat->m_Height, textures);
        tryAddTexture(mat->m_Emissive, textures);

        return textures;
    }

    Ref<Material> AssetManager::GetMaterialBase(const std::string& name) {
        const auto it = m_MaterialNameToUUID.find(name);
        if (it == m_MaterialNameToUUID.end()) {
            Warn("Material not found: " + name);
            return nullptr;
        }
        const auto mit = m_Materials.find(it->second);
        return (mit != m_Materials.end()) ? mit->second : nullptr;
    }

    Ref<Material> AssetManager::LoadMaterialBase(const UUID &uuid, const std::string &name) {
        if (m_Materials.contains(uuid))
            return m_Materials.at(uuid);

        auto mat = CreateRef<Material>(uuid);
        mat->m_Name = name;

        m_Materials.emplace(uuid, mat);
        m_MaterialNameToUUID.emplace(name, uuid);

        return mat;
    }

    Ref<Material> AssetManager::CreateMaterialBase(const std::string& name) {
        const std::string uniqueName = GenerateUniqueMaterialName(name);
        UUID uuid{};

        auto mat = CreateRef<Material>(uuid);
        mat->m_Name = uniqueName;

        m_Materials.emplace(uuid, mat);
        m_MaterialNameToUUID.emplace(uniqueName, uuid);

        return mat;
    }
}
