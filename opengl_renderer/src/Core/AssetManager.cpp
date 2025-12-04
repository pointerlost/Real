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
#include <set>
#include <stack>
#include <thread>
#include <Core/CmakeConfig.h>
#include "Graphics/Material.h"
#include "queue"
#include "Common/Scheduling/Threads.h"
#include "Math/Math.h"
#include "Tools/ImageTools.h"
#include "Util/Util.h"

#include <stb/stb_image.h>
#include <stb_image_resize2.h>
#include <unordered_set>

#include "Core/Config.h"
#include "Graphics/TextureArrays.h"

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
            case TextureType::ALB: channelColor[0] = 128; channelColor[1] = 128;
                                   channelColor[2] = 128; channelColor[3] = 255; // Optional alpha
                break;

            case TextureType::NRM: channelColor[0] = 128; channelColor[1] = 128;
                                   channelColor[2] = 255; channelColor[3] = 255; // Optional alpha
                break;

            case TextureType::RGH:    channelColor[0] = 128; break;
            case TextureType::MTL:
            case TextureType::HEIGHT: channelColor[0] = 0;   break;
            case TextureType::AO:     channelColor[0] = 255; break;

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
        if (IsTextureExists(file.name)) return m_Textures[file.name];

        const Ref<OpenGLTexture> texture = CreateRef<OpenGLTexture>();
        texture->SetType(type);
        texture->SetImageFormatState(imageState);
        texture->SetFileInfo(file);
        texture->SetIndex(m_Textures.size());
        texture->Create();

        return m_Textures[file.name] = texture;
    }

    void AssetManager::LoadTextures() {
        // Pack first rma textures into nrChannels, then load to folder as RMA, then compress it if needed
        std::unordered_map<std::string, std::array<Ref<OpenGLTexture>, 3>> packed_rma;

        const auto SaveTexture = [this, &packed_rma](const FileInfo& file, ImageFormatState imageState) {
            auto& stem = file.stem;
            const auto dashPos = stem.find('_');

            const auto matName = stem.substr(0, dashPos);
            const TextureType type = util::StringToEnum_TextureType(stem.substr(dashPos + 1));

            const auto tex = LoadTextureOnlyCPUData(file, type, imageState);

            const auto& mat = CreateMaterialBase(matName);
            switch (type) {
                case TextureType::ALB:    mat->m_Albedo = tex; break;
                case TextureType::NRM:    mat->m_Normal = tex; break;
                case TextureType::RMA:    mat->m_RMA    = tex; break;
                case TextureType::HEIGHT: mat->m_Height = tex; break;
                case TextureType::RGH:    packed_rma[matName][0] = tex; break;
                case TextureType::MTL:    packed_rma[matName][1] = tex; break;
                case TextureType::AO:     packed_rma[matName][2] = tex; break;
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

        // Save RMA files and delete each one
        for (auto& [materialName, rma_array] : packed_rma) {
            const auto tex = tools::LoadRMATextures(rma_array, materialName);
            const auto iState = tex->GetImageFormatState();
            std::string filepath = std::string(ASSETS_DIR) + "textures/";

            if (iState == ImageFormatState::COMPRESS_ME) {
                filepath += "compress_me/" + materialName + "_RMA" + tex->GetFileInfo().ext;
                if (!tools::SaveTextureAsFile(tex.get(), filepath)) {
                    Warn("[COMPRESS_ME] RMA file can't save!");
                }
            }
            else if (iState == ImageFormatState::UNCOMPRESSED) {
                filepath += "uncompressed/" + materialName + "_RMA" + tex->GetFileInfo().ext;
                if (!tools::SaveTextureAsFile(tex.get(), filepath)) {
                    Warn("[UNCOMPRESSED] RMA file can't save!");
                }
            } else {
                Warn("Image format state is UNDEFINED!");
            }
        }

        // Compress textures and read from DDS
        for (const auto& tex : std::views::values(m_Textures)) {
            if (tex->GetImageFormatState() != ImageFormatState::COMPRESS_ME) continue;
            tools::CompressTextureToBCn(tex.get(), std::string(ASSETS_DIR) + "textures/compressed/");
        }
    }

    void AssetManager::DeleteCPUTexture(const std::string& name) {
        m_Textures.erase(name);
    }

    Ref<OpenGLTexture>& AssetManager::GetTexture(const std::string &name) {
        if (!IsTextureExists(name))
            Warn("Texture '" + name + "' can't find! returning nullptr!!");
        return m_Textures[name];
    }

    Ref<Material>& AssetManager::CreateMaterialBase(const std::string &name) {
        if (m_Materials.contains(name))
            return m_Materials[name];

        return m_Materials[name] = CreateRef<Material>();
    }

    Ref<Material> & AssetManager::GetMaterialBase(const std::string &name) {
        if (!m_Materials.contains(name)) {
            Warn("There is no Base material for this name: " + name);
            return m_Materials[name] = CreateRef<Material>();
        }
        return m_Materials[name];
    }

    Ref<MaterialInstance>& AssetManager::GetOrCreateMaterialInstance(const std::string& name) {
        if (m_MaterialInstances.contains(name))
            return m_MaterialInstances[name];

        return m_MaterialInstances[name] = CreateRef<MaterialInstance>(name);
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
}
