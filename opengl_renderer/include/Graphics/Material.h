//
// Created by pointerlost on 10/12/25.
//
#pragma once
#include <glm/ext.hpp>
#include "GPUBuffers.h"
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

        Material() = default;
        Material(const Material&) = default;
        explicit Material(const UUID& uuid) : m_UUID(uuid) {}
        explicit Material(uint64_t uuid) : m_UUID(UUID(uuid)) {}
    };

    struct MaterialInstance {
        Ref<Material> m_Base = nullptr;

        explicit MaterialInstance(const UUID& uuid);
        MaterialInstance(const MaterialInstance&) = default;

        // Instance override colors
        glm::vec4 m_BaseColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
        glm::vec4 m_NormalRMA = {};
        // TODO: add other types like emissive, shininess etc.

        [[nodiscard]] MaterialSSBO ConvertToGPUFormat() const;
    };
}
