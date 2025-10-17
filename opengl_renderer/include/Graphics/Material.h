//
// Created by pointerlost on 10/12/25.
//
#pragma once
#include <unordered_map>
#include <glm/ext.hpp>

#include "GPUBuffers.h"
#include "Texture.h"
#include "Core/Utils.h"

namespace Real {
    struct Texture;
}

namespace Real {

    enum class TextureType {
        BaseColor,
        Normal,
        Roughness,
        Displacement,
        AmbientOcclusion,
    };

    struct Material {
    public:
        glm::vec4 BaseColor = glm::vec4(1.0, 1.0, 0.0, 1.0);
        float Metallic = 0.0f;
        float Roughness = 1.0f;
        glm::vec3 emissive = glm::vec3(0.0f);
        int GetTextureIndex(TextureType type) { return textures[type]->m_Index; }

        std::unordered_map<TextureType, Ref<Texture>> textures;
    };

    struct MaterialInstance {
    public:
        Ref<Material> m_Base = CreateRef<Material>();

        [[nodiscard]] MaterialSSBO ConvertToGPUFormat() const;
    };
}
