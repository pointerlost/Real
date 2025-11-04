//
// Created by pointerlost on 10/4/25.
//
// #define STB_IMAGE_IMPLEMENTATION
// #include <stb/stb_image.h>
#include "Core/AssetManager.h"
#include "Core/file_manager.h"
#include "Core/Logger.h"
#include "Core/Utils.h"
#include <fstream>
#include <ranges>
#include <Core/CmakeConfig.h>
#include "Graphics/Material.h"
#include "stb/stb_image.h"
#include "queue"
#include "Tools/ImageTools.h"
#include "Util/Util.h"

namespace Real {

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

    bool AssetManager::IsTextureCompressed(const std::string &name) const {
        return File::Exists(ConcatStr(ASSETS_DIR, "textures/compressed/", name, ".dds"));
    }

    void AssetManager::LoadUncompressedTexture(const std::string& name, const std::string &filePath, TextureType type, const FileInfo& info) {
        if (IsTextureExists(name)) {
            Info("Texture already exists! : " + name);
            return;
        }

        Ref<Texture> texture = CreateRef<Texture>(ImageFormatState::UNCOMPRESSED);
        auto& data = texture->GetData();

        data.m_Data = stbi_load(filePath.c_str(), &data.m_Width, &data.m_Height, &data.m_ChannelCount, 0);

        // DataSize = TexPixelCount * ChannelCount * Byte-Per-Channel
        // Get byte per channel as 1 coz we are loading images as 8-bit format
        data.m_DataSize = (data.m_Width * data.m_Height) * data.m_ChannelCount * 1;

        if (data.m_ChannelCount == 3) {
            data.m_Format = GL_RGB8;
            data.m_ImageCompressType = ImageCompressedType::BC1;
        }
        else if (data.m_ChannelCount == 3) {
            data.m_Format = GL_RGB8;
            data.m_ImageCompressType = ImageCompressedType::BC6;
        }
        else if (data.m_ChannelCount == 4) {
            data.m_Format = GL_RGBA8;
            data.m_ImageCompressType = ImageCompressedType::BC7;
        }

        texture->SetType(type);
        texture->SetFileInfo(info);

        // We are deciding for m_InternalFormat in Compress-time
        m_Textures[name] = std::move(texture);
    }

    void AssetManager::LoadUncompressedTexture(const std::string &name, const std::string &filePath, TextureType type) {
        const FileInfo info; // Send null info
        LoadUncompressedTexture(name, filePath, type, info);
    }

    void AssetManager::LoadUncompressedTexture(const std::string &name, const FileInfo &info, TextureType type) {
        LoadUncompressedTexture(name, info.path, type, info);
    }

    void AssetManager::LoadUncompressedTextures(const std::string &name, void *mixedData, TextureType type) {
        if (IsTextureExists(name)) {
            Info("Texture already exists! : " + name);
            return;
        }

        Ref<Texture> texture = CreateRef<Texture>(ImageFormatState::UNCOMPRESSED);
        texture->SetType(type);
        auto& data = texture->GetData();

        data.m_Data = mixedData;

        // TODO: There is should more ImageCompressedType like BC3, BC4 etc.
        if (data.m_ChannelCount == 3) {
            data.m_Format = GL_RGB8;
            data.m_ImageCompressType = ImageCompressedType::BC1;
        }
        else if (data.m_ChannelCount == 3) {
            data.m_Format = GL_RGB8;
            data.m_ImageCompressType = ImageCompressedType::BC6;
        }
        else if (data.m_ChannelCount == 4) {
            data.m_Format = GL_RGBA8;
            data.m_ImageCompressType = ImageCompressedType::BC7;
        }

        // We are deciding for m_InternalFormat in Compress-time
        m_Textures[name] = std::move(texture);
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
        std::unordered_map<std::string, std::unordered_map<std::string, Ref<Texture>>> packedSameTextures;
        // Load compress_me textures
        auto fileInfos = Util::IterateDirectory(ConcatStr(ASSETS_DIR, "textures/compress_me/"));
        for (const auto& file : fileInfos) {
            auto name = file.name;
            const auto dashPos = name.find('-');

            TextureType type = TextureType::UNDEFINED;
            auto texType = name.substr(dashPos + 1, name.size() - 1);
            auto texName = name.substr(0, dashPos - 1);

            if (texType == "ALB") {
                type = TextureType::ALB;
            } else if (texType == "NRM") {
                type = TextureType::NRM;
            } else if (texType == "RGH") {
                type = TextureType::RGH;
            } else if (texType == "MTL") {
                type = TextureType::MTL;
            } else if (texType == "AO") {
                type = TextureType::AO;
            } else if (texType == "HEIGHT") {
                type = TextureType::HEIGHT;
            }

            LoadUncompressedTexture(name, file, type);
            // Pack same textures into group
            packedSameTextures[texName][name] = GetTexture(name);
        }

        // Pack textures into channels (only what is necessary) like roughness + metallic + ao = rma
        // TODO: this helper could be made more flexible to pack other maps, this is enough for now
        for (const auto&[first, second] : packedSameTextures) {
            std::array<Ref<Texture>, 3> rma;
            size_t idx = 0;
            for (auto& tex : std::views::values(second)) {
                const auto texType = tex->GetType();
                if (texType == TextureType::RGH || texType == TextureType::MTL || texType == TextureType::AO)
                    rma[idx++] = tex;
                // If one of these tex's is missing, we will send nullptr, so check inside PackTexturesToChannels()
            }

            // Pack into channels
            const auto& mixedTexture = Tools::PackTexturesToChannels(rma);
            if (mixedTexture) {
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

        // Compress all the Textures
        for (auto& tex : std::views::values(m_Textures)) {
            Tools::CompressTextureToBCn(tex, std::string(ASSETS_DIR) + "textures/compressed/");
        }
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
