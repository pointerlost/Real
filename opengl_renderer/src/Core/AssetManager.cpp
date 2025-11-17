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
                CreateDefaultTexture(name, type, glm::ivec2(resolution, resolution), channelCount);
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

    Ref<OpenGLTexture> AssetManager::CreateDefaultTexture(const std::string& name, TextureType type, const glm::ivec2 &resolution, int channelCount) {
        if (IsTextureExists(name))
            return m_Textures[name];

        const Ref<OpenGLTexture> defaultTex = CreateRef<OpenGLTexture>(ImageFormatState::UNCOMPRESSED, true);
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
        data.m_Width    = resolution.x;
        data.m_Height   = resolution.y;
        data.m_DataSize = imageSize;
        // defaultTex->SetLevelData(data.m_Data, 0);
        defaultTex->SetChannelCount(channelCount, 0);
        defaultTex->SetFormat(util::ConvertChannelCountToGLType(channelCount, util::EnumToString_TextureType(type)), 0);
        defaultTex->SetInternalFormat(util::ImageCompressTypeToGLEnum(util::PickTextureCompressionType(type)), 0);
        defaultTex->SetCompressionType(util::PickTextureCompressionType(type));
        defaultTex->SetType(type);

        return m_Textures[name] = defaultTex;
    }

    Ref<OpenGLTexture> AssetManager::GetDefaultTexture(const std::string &name) {
        if (!IsTextureExists(name)) {
            Warn("Default Texture doesn't exists! returning random texture! :" + name);
            // TODO: Remove hardcoded default val
            return m_Textures["default_ALB_256"];
        }
        return m_Textures[name];
    }

    bool AssetManager::IsTextureCompressed(const std::string &stem) const {
        const std::string ddsFilePath = std::string(ASSETS_DIR) +  "textures/compressed/" + stem + ".dds";
        return File::Exists(ddsFilePath);
    }

    Ref<OpenGLTexture> AssetManager::LoadTextureCPUData(const std::string& name, TextureType type, const FileInfo& info) {
        if (IsTextureExists(name)) {
            Info("Texture already exists! name:" + name);
            return m_Textures[name];
        }

        const Ref<OpenGLTexture> texture = CreateRef<OpenGLTexture>(
            IsTextureCompressed(info.stem) ? ImageFormatState::COMPRESSED : ImageFormatState::UNCOMPRESSED
        );
        texture->SetType(type);
        texture->SetFileInfo(info);
        texture->SetInternalFormat(util::ImageCompressTypeToGLEnum(util::PickTextureCompressionType(type)), 0);
        texture->SetCompressionType(util::PickTextureCompressionType(type));

        // texture->Create(); // Channel count can change with this function, so use SetFormat after this line
        texture->SetIndex(m_Textures.size());
        texture->SetFormat(util::ConvertChannelCountToGLType(texture->GetChannelCount(0), util::EnumToString_TextureType(type)), 0);
        return m_Textures[name] = texture;
    }

    Ref<OpenGLTexture> AssetManager::LoadPackedTexturesCPUData(const std::string &name, const Ref<OpenGLTexture>& mixedTextures) {
        if (IsTextureExists(name)) {
            Info("Texture already exists! : " + name);
            return m_Textures[name];
        }

        FileInfo info;
        info.name = name;
        info.stem = name.substr(0, name.size() - 4); // Without extension
        info.path = std::string(ASSETS_DIR) + "textures/compress_me/" + name;
        info.ext  = name.substr(name.size() - 4);

        mixedTextures->SetFileInfo(info);
        mixedTextures->SetImageFormat(
            IsTextureCompressed(info.stem) ? ImageFormatState::COMPRESSED : ImageFormatState::UNCOMPRESSED
        );

        LoadTextureCPUData(name, mixedTextures->GetType(), mixedTextures->GetFileInfo());
        return m_Textures[name] = mixedTextures;
    }

    void AssetManager::LoadTexturesFromFile() {
        // Pack same textures into one type like; (Wood _ALB, Wood _NRM, Wood _RMA)
        std::unordered_map<std::string, std::unordered_map<std::string, Ref<OpenGLTexture>>> packedSameTextures;
        // Load compress_me textures
        auto fileInfos = util::IterateDirectory(ConcatStr(ASSETS_DIR, "textures/compress_me/"));
        for (const auto& file : fileInfos) {
            // This fileName represents the full name without the 'extension', .jpg, .png etc.
            auto& fileStem = file.stem;
            const auto dashPos = fileStem.find('_');

            auto texType = fileStem.substr(dashPos + 1);
            auto texName = fileStem.substr(0, dashPos);

            const TextureType type = util::StringToEnum_TextureType(texType);
            const auto& texture = LoadTextureCPUData(file.name, type, file);

            // Pack same textures into group
            if (type == TextureType::RGH || type == TextureType::MTL || type == TextureType::AO) {
                packedSameTextures[texName][fileStem] = texture;
            }
        }

        // Pack textures into channels (only what is necessary) like roughness + metallic + ao = rma
        // TODO: this helper could be made more flexible to pack other maps
        for (const auto& [first, second] : packedSameTextures) {
            std::array<Ref<OpenGLTexture>, 3> rma;
            size_t idx = 0;
            std::array processTypes = { TextureType::RGH, TextureType::MTL, TextureType::AO};
            int defaultTexWidth = 0;
            std::string rmaName;
            for (auto& tex : std::views::values(second)) {
                if (tex) {
                    rmaName = first + "_RMA" + tex->GetFileInfo().ext;
                    rma[idx++] = tex;
                    defaultTexWidth = defaultTexWidth <= 0 ? tex->GetResolution(0).first : defaultTexWidth;
                }
            }
            // Get default texture for missing textures
            for (size_t i = 0; i < 3; i++) {
                if (!rma[i]) {
                    auto name = util::GetDefaultTextureName(processTypes[i], defaultTexWidth);
                    rma[i] = GetDefaultTexture(name);
                }
            }

            LoadPackedTexturesCPUData(rmaName, tools::PackTexturesToRGBChannels(rma));
        }

        // Compress all the textures with threads
        // const Scope<Thread> t = CreateScope<Thread>();
        for (auto& tex : std::views::values(m_Textures)) {
            if (!tex) { Warn("texture nullptr!"); continue; }
            if (tex->IsDefault()) continue;
            std::string destPath = std::string(ASSETS_DIR) + "textures/compressed/";
            tools::CompressTextureToBCn(tex, destPath, tex->GetCompressionType());
            tex->Create();
            // t->Submit(tools::CompressTextureToBCn, std::ref(tex), std::string(ASSETS_DIR) + "textures/compressed/");
        }
        // t->JoinAll();
        // TODO: threads cause a crash when loading opengl state with create texture!?
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
        material->m_Base->m_rmaMap = GetTexture(name + "_RMA" + fileFormats[2]);
        material->m_Base->m_HeightMap = GetTexture(name + "_HEIGHT" + fileFormats[3]);

        m_Materials[name] = material;
        return m_Materials[name];
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
