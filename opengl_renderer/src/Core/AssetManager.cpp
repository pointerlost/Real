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
#include "Common/Scheduling/Threads.h"
#include "Math/Math.h"
#include "Tools/ImageTools.h"
#include "Util/Util.h"

#include <stb/stb_image.h>
#include <stb_image_resize2.h>

namespace Real {

    AssetManager::AssetManager() {
        auto create_default_tex = [this](TextureType type, int channelCount) {
            constexpr int constantFactor = 2;
            int resolution = 128;

            for (size_t i = 0; i <= 4; i++) {
                // For example name = default_ALB_256, default_RMA_2048, etc.
                const auto name = "default_" + util::TextureTypeEnumToString(type) + "_" + std::to_string(resolution);
                CreateDefaultTexture(name, type, glm::ivec2(constantFactor * resolution, constantFactor * resolution), channelCount);
                resolution *= 2;
            }
        };
        Thread t;
        t.Submit(create_default_tex, TextureType::ALB, 3);
        t.Submit(create_default_tex, TextureType::NRM, 3);
        t.Submit(create_default_tex, TextureType::RGH, 1);
        t.Submit(create_default_tex, TextureType::MTL, 1);
        t.Submit(create_default_tex, TextureType::AO, 1);
        t.Submit(create_default_tex, TextureType::HEIGHT, 1);
        t.JoinAll();
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

    Ref<Texture> AssetManager::CreateDefaultTexture(const std::string& name, TextureType type, const glm::ivec2 &resolution, int channelCount) {
        if (IsTextureExists(name))
            return m_Textures[name];

        Ref<Texture> defaultTex = CreateRef<Texture>();
        const auto imageSize = resolution.x * resolution.y * channelCount;
        auto* imageData = new uint8_t[imageSize];

        uint8_t channelColor[3] = {UINT8_MAX};
        switch (type) {
            case TextureType::ALB:
                channelColor[0] = 128;
                channelColor[1] = 128;
                channelColor[2] = 128;

            case TextureType::NRM:
                channelColor[0] = 128;
                channelColor[1] = 128;
                channelColor[2] = 255;

            case TextureType::RGH:
                channelColor[0] = 128;

            case TextureType::MTL:
            case TextureType::HEIGHT:
                channelColor[0] = 0;

            case TextureType::AO:
                channelColor[0] = 255;

            default:
                channelColor[0] = UINT8_MAX;
                channelColor[1] = UINT8_MAX;
                channelColor[2] = UINT8_MAX;
        }

        switch (channelCount) {
            case 1: // Grayscale
                for (size_t i = 0; i < imageSize; i++) {
                    imageData[i + 0] = channelColor[0];
                }
                break;
            case 2:
                for (size_t i = 0; i < imageSize; i++) {
                    imageData[i + 0] = channelColor[0];
                    imageData[i + 1] = channelColor[1];
                }
                break;
            case 3:
                for (size_t i = 0; i < imageSize; i++) {
                    imageData[i + 0] = channelColor[0];
                    imageData[i + 1] = channelColor[1];
                    imageData[i + 2] = channelColor[2];
                }
                break;

            default:
                Warn("Channel count mismatch! from: " + std::string(__FILE__));
        }
        auto& data = defaultTex->GetData();
        data.m_Data = new uint8_t[imageSize];

        data.m_ChannelCount = channelCount;
        data.m_Format = util::ConvertChannelCountToGLType(channelCount);
        data.m_InternalFormat = util::CompressTypeToGLEnum(util::PickTextureCompressionType(type));

        defaultTex->SetType(type);
        memcpy(defaultTex->GetData().m_Data, imageData, imageSize);
        delete[] imageData;

        m_Textures[name] = std::move(defaultTex);
        return m_Textures[name];
    }

    Ref<Texture> AssetManager::GetDefaultTexture(const std::string &name) {
        if (!IsTextureExists(name)) {
            Warn("Default Texture doesn't exists! returning random texture! (it can be nullptr too!!)");
            // TODO: Remove hardcoded default val
            return m_Textures["default_ALB_256"];
        }
        return m_Textures[name];
    }

    bool AssetManager::IsTextureCompressed(const std::string &name) const {
        return File::Exists(ConcatStr(ASSETS_DIR, "textures/compressed/", name, ".dds"));
    }

    Ref<Texture> AssetManager::LoadUncompressedTexture(const std::string& name, const std::string &filePath, TextureType type, const FileInfo& info) {
        if (IsTextureExists(name)) {
            Info("Texture already exists! : " + name);
            return m_Textures[name];
        }

        Ref<Texture> texture = CreateRef<Texture>(ImageFormatState::UNCOMPRESSED);
        auto& data = texture->GetData();

        // RGB
        data.m_Data = stbi_load(filePath.c_str(), &data.m_Width, &data.m_Height, &data.m_ChannelCount, 0);
        // Pick a channel flag for Texture resizing
        stbir_pixel_layout channelFlag;
        switch (data.m_ChannelCount) {
            case 1:
                channelFlag = STBIR_1CHANNEL;
                break;
            case 2:
                channelFlag = STBIR_2CHANNEL;
                break;
            case 4:
                channelFlag = STBIR_RGBA;
                break;
            default:
                channelFlag = STBIR_RGB;
        }

        const auto newWidth = util::FindClosestPowerOfTwo(data.m_Width);
        const auto newHeight = util::FindClosestPowerOfTwo(data.m_Height);
        const auto newResolution = Math::FindMax(newWidth, newHeight);

        // Resize Texture And Set resizing type srgb for ALBEDO coz color data needs gamma correction
        if (type == TextureType::ALB) {
            stbir_resize_uint8_srgb(static_cast<unsigned char*>(data.m_Data), data.m_Height, data.m_Height, 0,
                static_cast<unsigned char*>(data.m_Data), newResolution, newResolution, 0, channelFlag
            );
        } else {
            stbir_resize_uint8_linear(static_cast<unsigned char*>(data.m_Data), data.m_Height, data.m_Height, 0,
                static_cast<unsigned char*>(data.m_Data), newResolution, newResolution, 0, channelFlag
            );
        }

        texture->SetType(type);
        texture->SetFileInfo(info);

        // DataSize = TexPixelCount * ChannelCount * Byte-Per-Channel
        // Get byte per channel as 1 coz we are loading images as 8-bit format | TODO: Update with other bit types
        data.m_DataSize = (data.m_Width * data.m_Height) * data.m_ChannelCount * 1;

        data.m_Format = util::ConvertChannelCountToGLType(data.m_ChannelCount);
        data.m_ImageCompressType = util::PickTextureCompressionType(type);

        // We are deciding for m_InternalFormat in Compress-time
        m_Textures[name] = std::move(texture);
        return m_Textures[name];
    }

    Ref<Texture> AssetManager::LoadUncompressedTexture(const std::string &name, const FileInfo &info, TextureType type) {
        return LoadUncompressedTexture(name, info.path, type, info);
    }

    void AssetManager::LoadUncompressedTextures(const std::string &name, void *mixedData, TextureType type) {
        if (IsTextureExists(name)) {
            Info("Texture already exists! : " + name);
            return;
        }

        Ref<Texture> texture = CreateRef<Texture>(ImageFormatState::UNCOMPRESSED);
        auto& data = texture->GetData();
        data.m_Data = mixedData;

        texture->SetType(type); // Set as RMA etc.
        data.m_Format = util::ConvertChannelCountToGLType(data.m_ChannelCount);
        data.m_ImageCompressType = util::PickTextureCompressionType(type);

        // We are deciding for m_InternalFormat in Compress-time
        m_Textures[name] = std::move(texture);
    }

    void AssetManager::CreateCompressedTextureArray(const glm::ivec2 &resolution,
        const std::vector<Ref<Texture>> &textures)
    {
        static int textureArrayIndex = 0;
        // Create texture array
        GLuint texArray;
        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texArray);
        // Allocate the storage
        glTextureStorage3D(texArray, 1, textures[0]->GetData().m_InternalFormat, resolution.x, resolution.y, textures.size());

        for (const auto& tex : textures) {
            tex->SetTexArrayIndex(textureArrayIndex);

            auto& texData = tex->GetData();
            // Use GL compressed types for format
            // glCompressedTexSubImage3D(texArray, 0, 0, 0, tex->GetIndex(), texData.m_Width, texData.m_Height, 1, texData.m_InternalFormat, ,texData.m_Data);
            glTextureSubImage3D(texArray, 0, 0, 0, tex->GetIndex(), texData.m_Width, texData.m_Height, 1, texData.m_InternalFormat, GL_UNSIGNED_BYTE, texData.m_Data);
            stbi_image_free(texData.m_Data); // Clean up vRAM
            texData.m_Data = nullptr;
        }
        // Set parameters
        glTextureParameteri(texArray, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texArray, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(texArray, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(texArray, GL_TEXTURE_WRAP_T, GL_REPEAT);

        textureArrayIndex++;
    }

    void AssetManager::CreateTextureArray(const glm::ivec2 &resolution, const std::vector<Ref<Texture>>& textures) {
        static int textureArrayIndex = 0;
        // Create texture array
        GLuint texArray;
        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texArray);
        // Allocate the storage
        glTextureStorage3D(texArray, 1, textures[0]->GetData().m_InternalFormat, resolution.x, resolution.y, textures.size());

        for (const auto& tex : textures) {
            tex->SetTexArrayIndex(textureArrayIndex);

            auto& texData = tex->GetData();
            // Use GL compressed types for format
            glTextureSubImage3D(texArray, 0, 0, 0, tex->GetIndex(), texData.m_Width, texData.m_Height, 1, texData.m_InternalFormat, GL_UNSIGNED_BYTE, texData.m_Data);
            stbi_image_free(texData.m_Data); // Clean up vRAM
            texData.m_Data = nullptr;
        }
        // Set parameters
        glTextureParameteri(texArray, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texArray, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(texArray, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(texArray, GL_TEXTURE_WRAP_T, GL_REPEAT);

        textureArrayIndex++;
    }

    void AssetManager::LoadTextures() {
        // Pack same textures into one type like; (Wood-ALB, Wood-NRM, Wood-RMA) (pack.first = Wood, pack.second = ALB)
        std::unordered_map<std::string, std::unordered_map<std::string, Ref<Texture>>> packedSameTextures;
        // Load compress_me textures
        auto fileInfos = util::IterateDirectory(ConcatStr(ASSETS_DIR, "textures/compress_me/"));
        for (const auto& file : fileInfos) {
            // This file name represents the full name without the 'extension', .jpg, .png etc.
            auto& fileName = file.stem;
            const auto dashPos = fileName.find('_');

            auto texType = fileName.substr(dashPos + 1);
            auto texName = fileName.substr(0, dashPos);

            const TextureType type = util::StringToEnumTextureType(texType);

            LoadUncompressedTexture(fileName, file, type);
            // Pack same textures into group
            if (type == TextureType::RGH || type == TextureType::MTL || type == TextureType::AO)
                packedSameTextures[texName][fileName] = GetTexture(fileName);
        }

        // Pack textures into channels (only what is necessary) like roughness + metallic + ao = rma
        // TODO: this helper could be made more flexible to pack other maps, this is(RMA) enough for now
        for (const auto& [first, second] : packedSameTextures) {
            std::array<Ref<Texture>, 3> rma;
            size_t idx = 0;
            for (auto& tex : std::views::values(second)) {
                const auto texType = tex->GetType();
                if (texType == TextureType::RGH || texType == TextureType::MTL || texType == TextureType::AO) {
                    rma[idx++] = tex;
                } else {
                    auto name = "default_" + util::TextureTypeEnumToString(texType) + "_" + std::to_string(tex->GetData().m_Width);
                    rma[idx++] = GetDefaultTexture(name);
                }
            }

            // Pack into channels
            const auto& mixedTexture = tools::PackTexturesToChannels(first, rma);
            if (mixedTexture) {
                // We are using RMA as a TextureType for now, remove hardcoded if
                LoadUncompressedTextures(first, mixedTexture->GetData().m_Data, TextureType::RMA);
                m_Textures[mixedTexture->GetName()] = mixedTexture;

                // Delete the separate textures because they are now merged into one
                for (auto& tex : std::views::values(second)) {
                    if (tex) {
                        m_Textures.erase(tex->GetName());
                    }
                }
            }
        }

        // Compress all the Textures with multiple threads
        {
            const Scope<Thread> t = CreateScope<Thread>();
            for (auto& tex : std::views::values(m_Textures)) {
                t->Submit(tools::CompressTextureToBCn, std::ref(tex), std::string(ASSETS_DIR) + "textures/compressed/");
            }
            t->JoinAll();
        }

    }

    Ref<Texture> & AssetManager::GetTexture(const std::string &name) {
        if (!IsTextureExists(name)) Warn("Texture '" + name + "' can't find!");
        return m_Textures[name];
    }

    Ref<MaterialInstance> AssetManager::GetDefaultMat() {
        const auto instance = CreateRef<MaterialInstance>();
        m_Materials["default"] = instance;
        return m_Materials["default"];
    }

    Ref<MaterialInstance>& AssetManager::CreateMaterialInstance(const std::string& name) {
        if (m_Materials.contains(name))
            return m_Materials[name];
        const auto material = CreateRef<MaterialInstance>();
        m_Materials[name] = material;
        return m_Materials[name];
    }

    void AssetManager::BindTextureArray() const {
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
