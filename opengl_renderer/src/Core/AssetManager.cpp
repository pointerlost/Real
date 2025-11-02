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
#include <Core/CmakeConfig.h>
#include "Graphics/Material.h"
#include "stb/stb_image.h"
#include "queue"

#include <compressonator/include/cmp_core/source/cmp_core.h>
#include <compressonator/include/cmp_compressonatorlib/compressonator.h>
#include <compressonator/include/cmp_framework/common/cmp_mips.h>
#include <compressonator/include/cmp_framework/compute_base.h>
#include <GL/glext.h>

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

    void AssetManager::LoadUncompressedTexture(const std::string& name, const std::string &filePath) {
        if (IsTextureExists(name)) return;

        Ref<Texture> texture = CreateRef<Texture>(ImageFormatState::UNCOMPRESSED);
        auto& data = texture->m_Data;

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

        // We are deciding for m_InternalFormat in Compress-time
        m_Textures[name] = std::move(texture);
    }

    void AssetManager::LoadUncompressedTextures(const std::string &name, void *mixedData) {
        if (IsTextureExists(name)) return;

        Ref<Texture> texture = CreateRef<Texture>(ImageFormatState::UNCOMPRESSED);
        auto& data = texture->m_Data;

        data.m_Data = mixedData;

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
        // Create texture array
        GLuint texArray;
        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texArray);
        // Allocate the storage
        glTextureStorage3D(texArray, 1, textures[0]->m_Data.m_InternalFormat, resolution.x, resolution.y, textures.size());

        for (const auto& tex : textures) {
            auto& texData = tex->m_Data;
            // use GL compressed types for format
            glTextureSubImage3D(texArray, 0, 0, 0, tex->m_Index, texData.m_Width, texData.m_Height, 1, texData.m_InternalFormat, GL_UNSIGNED_BYTE, texData.m_Data);
            stbi_image_free(texData.m_Data); // Clean up vRAM
            texData.m_Data = nullptr;
        }
        // Set parameters
        glTextureParameteri(texArray, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texArray, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(texArray, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(texArray, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    Ref<Texture> AssetManager::LoadTexture(const std::string &filePath, const std::string& name) {
        if (IsTextureExists(name)) return m_Textures[name];
        auto texture = CreateRef<Texture>();

        if (!filePath.empty())
            texture->Load(filePath);

        texture->m_Index = static_cast<int>(m_TextureArrays.size());

        m_TextureArrays.push_back(texture);
        m_Textures[name] = texture;
        return texture;
    }

    void AssetManager::LoadTextures() {
        // TODO: Read from file like materials.json
        // Load all textures
        LoadTexture("assets/textures/container.jpg", "container");
        LoadTexture("assets/textures/container2_specular.png", "container2_specular");
        LoadTexture("assets/textures/container2.png", "container2");
        LoadTexture("assets/textures/woodFloor.png", "floor_wood");

        // Bind texture array (binding point = 6)
        glBindTextureUnit(6, m_GPUTextureArray);
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
        glBindTextureUnit(6, m_GPUTextureArray);
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
