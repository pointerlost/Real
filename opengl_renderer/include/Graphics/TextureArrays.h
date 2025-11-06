//
// Created by pointerlost on 11/1/25.
//
#pragma once
#include <unordered_set>
#include <vector>
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
    };

    struct TextureArrayManager {
        void AddAlbedoMap(TextureResolution resolution, const Ref<Texture>& tex);
        void AddNormalMap(TextureResolution resolution, const Ref<Texture>& tex);
        void AddRMATexturesMap(TextureResolution resolution, const Ref<Texture>& tex);
        void AddHeightMap(TextureResolution resolution, const Ref<Texture>& tex);

    private:
        std::array<std::vector<Ref<Texture>>, 20> m_TextureArrays;
        // BC7 compression
        std::vector<Ref<Texture>> m_AlbedoMapTexArray_256;
        std::vector<Ref<Texture>> m_AlbedoMapTexArray_512;
        std::vector<Ref<Texture>> m_AlbedoMapTexArray_1024;
        std::vector<Ref<Texture>> m_AlbedoMapTexArray_2048;
        std::vector<Ref<Texture>> m_AlbedoMapTexArray_4096;

        // BC5 compression
        std::vector<Ref<Texture>> m_NormalMapTexArray_256;
        std::vector<Ref<Texture>> m_NormalMapTexArray_512;
        std::vector<Ref<Texture>> m_NormalMapTexArray_1024;
        std::vector<Ref<Texture>> m_NormalMapTexArray_2048;
        std::vector<Ref<Texture>> m_NormalMapTexArray_4096;

        // BC4 compression | Roughness, Metallic, AO maps
        std::vector<Ref<Texture>> m_rmaMapTexArray_256;
        std::vector<Ref<Texture>> m_rmaMapTexArray_512;
        std::vector<Ref<Texture>> m_rmaMapTexArray_1024;
        std::vector<Ref<Texture>> m_rmaMapTexArray_2048;
        std::vector<Ref<Texture>> m_rmaMapTexArray_4096;

        // BC4 compression
        std::vector<Ref<Texture>> m_HeightMapTexArray_256;
        std::vector<Ref<Texture>> m_HeightMapTexArray_512;
        std::vector<Ref<Texture>> m_HeightMapTexArray_1024;
        std::vector<Ref<Texture>> m_HeightMapTexArray_2048;
        std::vector<Ref<Texture>> m_HeightMapTexArray_4096;

        std::unordered_set<int> m_CheckTextureExists;

        inline static int m_TextureArrayIndex = 0;

    private:
        const std::vector<Ref<Texture>>& GetTextureArray(TextureResolution resolution, TextureType type);
        void GetTexArrayWithResolution(TextureResolution res, TextureType type);

        bool IsTextureExists(const Ref<Texture>& texture) const;
    };
}
