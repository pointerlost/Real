//
// Created by pointerlost on 11/1/25.
//
#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Texture.h"
#include "Core/Utils.h"

namespace Real {
    enum class TextureType;
}

namespace Real {
    struct Texture;
}

namespace Real {

    enum class TextureResolution : uint {
        _256 = 256,
        _512 = 512,
        _1024 = 1024,
        _2048 = 2048,
        _4096 = 4096,
        UNDEFINED = 0,
    };

    struct TextureArrayManager {
        static void AddTextureMap(TextureType arrayType, TextureResolution resolution, Ref<Texture> texMap);

    private:
        // [0][] = albedoMap, [1][] = normalMap, [2][] = rmaMap, [3][] = heightMap
        // [][0] = res256, [][1] = res512, [][2] = res1024, [][3] = res2048, [][4] = res4096
        static std::vector<Ref<Texture>> m_TextureArrays[4][5];

    private:
        static const std::vector<Ref<Texture>>& GetTextureArray(TextureType arrayType, TextureResolution res);
        static size_t TexArrayTypeToIndex(TextureType arrayType);
        static size_t TexArrayResolutionToIndex(TextureResolution res);
    };
}
