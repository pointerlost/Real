//
// Created by pointerlost on 10/12/25.
//
#pragma once
#include <glm/ext.hpp>
#include <utility>
#include "GPUBuffers.h"
#include "Core/AssetManager.h"
#include "Core/Services.h"
#include "Core/Utils.h"
#include "Core/UUID.h"

namespace Real { struct OpenGLTexture; }

namespace Real {

    // TODO: Need material baking system to optimize run-time
    struct Material {
        UUID m_UUID = UUID(0);
        std::string m_Name{};

        UUID m_Albedo   = UUID(0);
        UUID m_Normal   = UUID(0);
        UUID m_ORM      = UUID(0);
        UUID m_Height   = UUID(0);
        UUID m_Emissive = UUID(0);

        Material() = default;
        Material(const Material&) = default;
        explicit Material(const UUID& uuid) : m_UUID(uuid) {}
        explicit Material(const UUID& uuid, std::string  name) : m_UUID(uuid), m_Name(std::move(name)) {}
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
