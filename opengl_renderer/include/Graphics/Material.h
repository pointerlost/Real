//
// Created by pointerlost on 10/12/25.
//
#pragma once
#include <glm/ext.hpp>
#include "GPUBuffers.h"
#include "Core/AssetManager.h"
#include "Core/Services.h"
#include "Core/Utils.h"
#include "Core/UUID.h"

namespace Real { struct OpenGLTexture; }

namespace Real {

    struct Material {
        UUID m_UUID = UUID(0);
        std::string m_Name{};

        UUID m_Albedo   = UUID(0);
        UUID m_Normal   = UUID(0);
        UUID m_ORM      = UUID(0);
        UUID m_Height   = UUID(0);
        UUID m_Emissive = UUID(0);
    };

    struct MaterialInstance {
        UUID m_UUID{}; // Instance UUID, initialized with constructor
        Ref<const Material> m_Base = nullptr;

        explicit MaterialInstance(const Ref<Material>& assetMaterial);
        MaterialInstance(const MaterialInstance&) = default;

        // Instance override colors
        glm::vec4 m_BaseColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
        glm::vec4 m_NormalRMA = {};

        // Instance override textures
        std::optional<UUID> m_AlbedoOverride;
        std::optional<UUID> m_NormalOverride;
        std::optional<UUID> m_ORMOverride;
        std::optional<UUID> m_HeightOverride;
        std::optional<UUID> m_EmissiveOverride;
        // TODO: add other types like emissive, shininess etc.

        [[nodiscard]] MaterialSSBO ConvertToGPUFormat() const;
    };
}
