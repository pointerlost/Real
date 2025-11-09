//
// Created by pointerlost on 11/1/25.
//
#include "Graphics/TextureArrays.h"
#include "Graphics/Texture.h"

namespace Real {

    void TextureArrayManager::AddTextureMap(TextureType arrayType, TextureResolution resolution, Ref<Texture> texMap) {
        m_TextureArrays[TexArrayTypeToIndex(arrayType)][TexArrayResolutionToIndex(resolution)].push_back(std::move(texMap));
    }

    const std::vector<Ref<Texture>>& TextureArrayManager::GetTextureArray(TextureType arrayType, TextureResolution res) {
        return m_TextureArrays[TexArrayTypeToIndex(arrayType)][TexArrayResolutionToIndex(res)];
    }

    size_t TextureArrayManager::TexArrayTypeToIndex(TextureType arrayType) {
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

    size_t TextureArrayManager::TexArrayResolutionToIndex(TextureResolution res) {
        switch (res) {
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
