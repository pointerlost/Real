//
// Created by pointerlost on 10/12/25.
//
#pragma once
#include <glm/ext.hpp>
#include "GPUBuffers.h"
#include "Texture.h"
#include "Core/Utils.h"

namespace Real {
    struct Texture;
}

namespace Real {

    struct Material {
        std::string m_AlbedoMapName;
        std::string m_NormalMapName;
        std::string m_rmaMapName;
        std::string m_HeightMapName;
        // Ref<Texture> m_AlbedoMap;
        // Ref<Texture> m_NormalMap;
        // Ref<Texture> m_rmaMap;
        // Ref<Texture> m_HeightMap;
    };

    struct MaterialInstance {
        glm::vec4 m_BaseColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
        glm::vec4 m_NormalRMA = {};

        Ref<Material> m_Base = CreateRef<Material>();

        [[nodiscard]] MaterialSSBO ConvertToGPUFormat() const;
    };
}
