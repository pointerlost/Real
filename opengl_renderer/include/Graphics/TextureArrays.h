//
// Created by pointerlost on 11/1/25.
//
#pragma once
#include <unordered_map>
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

    enum class TextureResolution {
        _256,
        _512,
        _1024,
        _2048,
        _4096,
        UNDEFINED,
    };

    struct TextureArrayManager {
        static void AddTextureMap(TextureType texType, TextureResolution resolution, const Ref<Texture> &texMap);
        static const std::vector<Ref<Texture>>& GetTextureArray(TextureType arrayType, TextureResolution res);
        static void PrepareAndBindTextureArrays();
        static void BindTextureArrays();

    private:
        // [0][] = albedoMap, [1][] = normalMap, [2][] = rmaMap, [3][] = heightMap
        // [][0] = res256, [][1] = res512, [][2] = res1024, [][3] = res2048, [][4] = res4096
        static inline std::vector<Ref<Texture>> m_TextureArrays[4][5];
        static inline std::unordered_map<std::string, GLuint> m_TextureArrayHandles;
        inline static size_t m_TexArrayIndex = 0;

    private:
        static size_t TexArrayTypeToArrayIndex(TextureType arrayType);
        static void CreateCompressedTextureArray(const std::string& texMapName, const std::vector<Ref<Texture>>& textureArray);
        static size_t TexArrayResolutionToIndex(TextureResolution resolution);
    };
}
