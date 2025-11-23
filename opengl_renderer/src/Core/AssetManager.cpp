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
        /*
         * We are using default textures to mix other textures like;
         * roughness + defaultMetallic + AO = RMA, in this case we don't have to compress default textures
         * until we will use as a texture in UI or something else, in this case we can compress them.
        */
        auto create_default_tex = [this](TextureType type, int channelCount) {
            int resolution = 256;

            for (size_t i = 0; i <= 4; i++) {
                // For example name = default_ALB_256, default_RMA_2048, etc.
                const auto name = "default_" + util::EnumToString_TextureType(type) + "_" + std::to_string(resolution);
                GetOrCreateDefaultTexture(name, type, glm::ivec2(resolution, resolution), channelCount);
                resolution *= 2;
            }
        };

        create_default_tex(TextureType::ALB, 3);
        create_default_tex(TextureType::NRM, 3);
        create_default_tex(TextureType::RGH, 1);
        create_default_tex(TextureType::MTL, 1);
        create_default_tex(TextureType::AO, 1);
        create_default_tex(TextureType::HEIGHT, 1);
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

        Ref<OpenGLTexture> defaultTex = CreateRef<OpenGLTexture>();
        const auto imageSize = resolution.x * resolution.y * channelCount;
        auto* imageData = new uint8_t[imageSize];

        uint8_t channelColor[3] = {UINT8_MAX};
        switch (type) {
            case TextureType::ALB:
                channelColor[0] = 128;
                channelColor[1] = 128;
                channelColor[2] = 128;
                break;

            case TextureType::NRM:
                channelColor[0] = 128;
                channelColor[1] = 128;
                channelColor[2] = 255;
                break;

            case TextureType::RGH:
                channelColor[0] = 128;
                break;

            case TextureType::MTL:
            case TextureType::HEIGHT:
                channelColor[0] = 0;
                break;

            case TextureType::AO:
                channelColor[0] = 255;
                break;

            default:
                channelColor[0] = UINT8_MAX;
                channelColor[1] = UINT8_MAX;
                channelColor[2] = UINT8_MAX;
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
                for (size_t i = 0; i < imageSize; i+= channelCount) {
                    imageData[i + 0] = channelColor[0];
                    imageData[i + 1] = channelColor[1];
                    imageData[i + 2] = channelColor[2];
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
        data.m_Width  = resolution.x;
        data.m_Height = resolution.y;
        data.m_Format = util::ConvertChannelCountToGLFormat(channelCount);
        data.m_InternalFormat = util::ConvertChannelCountToGLFormat(channelCount);

        defaultTex->CreateFromData(data, type);
        return m_DefaultTextures[name] = defaultTex;
    }

    bool AssetManager::IsTextureCompressed(const std::string &stem) const {
        const std::string ddsFilePath = std::string(ASSETS_DIR) +  "textures/compressed/" + stem + ".dds";
        return File::Exists(ddsFilePath);
    }

    Ref<OpenGLTexture> AssetManager::LoadTextureOnlyCPUData(const std::string& name, TextureType type, const FileInfo& info) {
        if (IsTextureExists(name)) return m_Textures[name];

        const Ref<OpenGLTexture> texture = CreateRef<OpenGLTexture>();
        texture->SetType(type);
        texture->SetFileInfo(info);
        texture->SetIndex(m_Textures.size());
        texture->Create();

        return m_Textures[name] = texture;
    }

    void AssetManager::LoadPackedTexturesCPUData(const std::string &name, const Ref<OpenGLTexture>& mixedTextures) {
        if (IsTextureExists(name)) return;

        FileInfo info;
        info.name = name;
        info.stem = name.substr(0, name.size() - 4); // Without extension
        info.path = std::string(ASSETS_DIR) + "textures/compress_me/" + name;
        info.ext  = name.substr(name.size() - 4);

        mixedTextures->SetFileInfo(info);

        mixedTextures->CreateFromData(mixedTextures->GetLevelData(0), mixedTextures->GetType());
        m_Textures[name] = mixedTextures;
    }

    void AssetManager::PrepareTexturesToUpload() {
        // Load compress_me textures
        std::unordered_map<std::string, std::array<Ref<OpenGLTexture>, 3>> packedTextures = {};
        for (const auto& file : util::IterateDirectory(ConcatStr(ASSETS_DIR, "textures/compress_me/"))) {
            // This fileName represents the full name without the 'extension', .jpg, .png etc.
            auto& fileStem = file.stem;
            const auto dashPos = fileStem.find('_');

            auto texType = fileStem.substr(dashPos + 1);
            auto texName = fileStem.substr(0, dashPos);

            const TextureType type = util::StringToEnum_TextureType(texType);
            auto texture = LoadTextureOnlyCPUData(file.name, type, file);

            // Don't save the Roughness, metallic, ambient occlusion as DDS files, coz not necessary
            if (type != TextureType::RGH && type != TextureType::MTL && type != TextureType::AO) {
                const auto compressionPath = std::string(ASSETS_DIR) + "textures/compressed/";
                if (!IsTextureCompressed(fileStem)) {
                    tools::CompressTextureToBCn(texture.get(), compressionPath);
                }
                // Read from DDS file after creation
                tools::ReadCompressedDataFromDDSFile(texture.get());
            } else {
                switch (type) {
                    case TextureType::RGH: packedTextures[texName][0] = texture; break;
                    case TextureType::MTL: packedTextures[texName][1] = texture; break;
                    case TextureType::AO:  packedTextures[texName][2] = texture; break;
                    default: ;
                }
            }
        }

        for (auto& [texName, rma] : packedTextures) {
            // Store these values to create missing cases
            std::pair resolution = {1024, 1024};
            int channelCount = 0;
            std::string ext;

            // Find the resolution of any existing texture to specify the missing cases!
            for (size_t i = 0; i < 3; i++) {
                if (rma[i]) {
                    ext = rma[i]->GetFileInfo().ext;
                    resolution = rma[i]->GetResolution(0);
                    channelCount = rma[i]->GetChannelCount(0);
                    break;
                }
            }

            for (size_t i = 0; i < 3; i++) {
                if (!rma[i]) {
                    TextureType missingType = {};
                    switch (i) {
                        case 0: missingType = TextureType::RGH; break;
                        case 1: missingType = TextureType::MTL; break;
                        case 2: missingType = TextureType::AO;  break;
                        default: ;
                    }
                    rma[i] = GetOrCreateDefaultTexture(texName, missingType,
                        glm::ivec2(resolution.first, resolution.second), channelCount
                    );
                }
            }

            auto packedTextureName = texName + "_RMA" + ext;
            auto packedTex = tools::PackTexturesToRGBChannels(rma, true, 255);
            LoadPackedTexturesCPUData(packedTextureName, packedTex);

            const auto compressionPath = std::string(ASSETS_DIR) + "textures/compressed/";
            tools::CompressCPUGeneratedTexture(packedTex.get(), compressionPath);
            tools::ReadCompressedDataFromDDSFile(packedTex.get());
        }
    }

    Ref<OpenGLTexture>& AssetManager::GetTexture(const std::string &name) {
        if (!IsTextureExists(name))
            Warn("Texture '" + name + "' can't find! returning nullptr!!");
        return m_Textures[name];
    }

    Ref<MaterialInstance>& AssetManager::CreateMaterialInstance(const std::string& name, const std::array<std::string, 4> &fileFormats) {
        if (m_Materials.contains(name))
            return m_Materials[name];

        const auto material = CreateRef<MaterialInstance>();
        material->m_Base->m_AlbedoMap = GetTexture(name + "_ALB" + fileFormats[0]);
        material->m_Base->m_NormalMap = GetTexture(name + "_NRM" + fileFormats[1]);
        material->m_Base->m_rmaMap    = GetTexture(name + "_RMA" + fileFormats[2]);
        material->m_Base->m_HeightMap = GetTexture(name + "_HEIGHT" + fileFormats[3]);

        m_Materials[name] = material;
        return m_Materials[name];
    }

    std::vector<GLuint64> AssetManager::UploadTexturesToGPU() const {
        std::vector<GLuint64> bindlessIDs;
        for (const auto& tex : std::views::values(m_Textures)) {
            tex->PrepareOptionsAndUploadToGPU();
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
}
