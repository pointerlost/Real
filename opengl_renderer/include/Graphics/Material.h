//
// Created by pointerlost on 10/12/25.
//
#pragma once
#include <unordered_map>
#include <glm/ext.hpp>
#include <utility>

#include "GPUBuffers.h"
#include "Texture.h"
#include "Core/Utils.h"

namespace Real {
    struct Texture;
}

namespace Real {

    enum class TextureType {
        Albedo,
        Normal,
        Roughness,
        Metallic,
        AmbientOcclusion,
        Height,
    };

    struct Material {
        Ref<Texture> albedoMap;
        Ref<Texture> normalMap;
        Ref<Texture> m_rmaMap;
        Ref<Texture> heightMap;
    };

    struct MaterialInstance {
        glm::vec4 m_BaseColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
        glm::vec4 m_NormalRMA = {};

        Ref<Material> m_Base = CreateRef<Material>();

        [[nodiscard]] MaterialSSBO ConvertToGPUFormat() const;
    };
}
