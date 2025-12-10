//
// Created by pointerlost on 11/1/25.
//
#include "Graphics/TextureArrays.h"

#include <ranges>

#include "Graphics/Texture.h"
#include <stb/stb_image.h>

#include "Common/Scheduling/Threads.h"
#include "Core/Logger.h"
#include "Util/Util.h"

namespace Real {

    void TextureArrayManager::AddTextureMap(TextureType texType, TextureResolution resolution, const Ref<OpenGLTexture> &texMap) {
        if (!texMap) {
            Warn("Texture can't added to TextureArray (nullptr): " + util::TextureType_EnumToString(texType));
            return;
        }
        // texMap->SetTexArrayIndex(TexArrayTypeToArrayIndex(texType));
        // texMap->SetTexIndex(m_TextureArrays[TexArrayTypeToArrayIndex(texType)][TexArrayResolutionToIndex(resolution)].size());
        m_TextureArrays[TexArrayTypeToArrayIndex(texType)][TexArrayResolutionToIndex(resolution)].push_back(texMap);
    }

    const std::vector<Ref<OpenGLTexture>>& TextureArrayManager::GetTextureArray(TextureType arrayType, TextureResolution res) {
        return m_TextureArrays[TexArrayTypeToArrayIndex(arrayType)][TexArrayResolutionToIndex(res)];
    }

    void TextureArrayManager::PrepareAndBindTextureArrays() {
        auto create_compressed_tex_array = [](const std::string& arrayName, TextureType type) {
            int resolution = 256;
            const size_t arrayTypeIndex = TexArrayTypeToArrayIndex(type);
            for (size_t i = 0; i <= 4; i++) {
                auto fullName = arrayName + "_" + std::to_string(resolution);
                CreateCompressedTextureArray(fullName, m_TextureArrays[arrayTypeIndex][i]);
                resolution *= 2;
            }
        };
        // Texture arrays | Albedo x 5 | Normal x 5 | RMA x 5 | Height x 5
        // We are using 5 different resolution for per type (256,512,1024,2048,4096)
        create_compressed_tex_array("albedoArray", TextureType::ALB);
        create_compressed_tex_array("normalArray", TextureType::NRM);
        create_compressed_tex_array("rmaArray", TextureType::RMA);
        create_compressed_tex_array("heightArray", TextureType::HEIGHT);

        BindTextureArrays();

        Info("Texture arrays created and bound!");
    }

    void TextureArrayManager::BindTextureArrays() {
        size_t i = 0;
        for (const auto& handle : std::ranges::views::values(m_TextureArrayHandles)) {
            glActiveTexture(GL_TEXTURE0 + i++);
            glBindTexture(GL_TEXTURE_2D_ARRAY, handle);
        }
    }

    size_t TextureArrayManager::TexArrayTypeToArrayIndex(TextureType arrayType) {
        switch (arrayType) {
            case TextureType::ALB:
                return 0;
            case TextureType::NRM:
                return 1;
            case TextureType::RMA:
                return 2;
            case TextureType::HEIGHT:
                return 3;

            default:
                return 0;
        }
    }

    void TextureArrayManager::CreateCompressedTextureArray(const std::string& texMapName, const std::vector<Ref<OpenGLTexture>>& textureArray) {
        // if (m_TextureArrayHandles.contains(texMapName)) {
        //     Warn("TextureArray already loaded before with name: " + texMapName);
        //     return;
        // }
        // // Create texture array
        // GLuint texArray;
        // glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texArray);
        //
        // // Allocate the storage based firstTexData
        // const auto& firstTexData = textureArray[0]->GetData();
        // // Check if empty, because OpenGL does not support zero-sized arrays
        // if (!textureArray.empty()) {
        //     glTextureStorage3D(texArray, 1, firstTexData.m_GLCompressedType, firstTexData.m_Width, firstTexData.m_Height, textureArray.size());
        //
        //     for (const auto& tex : textureArray) {
        //         if (tex->HasData()) {
        //             auto& texData = tex->GetData();
        //             glCompressedTexSubImage3D(texArray, 0, 0, 0, tex->GetIndex(), texData.m_Width, texData.m_Height, 1,
        //                 texData.m_GLCompressedType, texData.m_ImageSize, texData.m_Data
        //             );
        //             stbi_image_free(texData.m_Data); // Clean up vRAM
        //             texData.m_Data = nullptr;
        //         }
        //     }
        //     // Set parameters
        //     glTextureParameteri(texArray, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //     glTextureParameteri(texArray, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //     glTextureParameteri(texArray, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //     glTextureParameteri(texArray, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // }
        //
        // m_TextureArrayHandles[texMapName] = texArray;
    }

    size_t TextureArrayManager::TexArrayResolutionToIndex(TextureResolution resolution) {
        switch (resolution) {
            case TextureResolution::_256:
                return 0;
            case TextureResolution::_512:
                return 1;
            case TextureResolution::_1024:
                return 2;
            case TextureResolution::_2048:
                return 3;
            case TextureResolution::_4096:
                return 4;

            default:
                return 0;
        }
    }
}
