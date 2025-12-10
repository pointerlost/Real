//
// Created by pointerlost on 10/12/25.
//
#pragma once
#include <glm/ext.hpp>
#include "GPUBuffers.h"
#include "Texture.h"
#include "Core/Utils.h"
#include "Core/UUID.h"

namespace Real { struct OpenGLTexture; }

namespace Real {

    struct Material {
        UUID m_ID;
        std::string m_Name;

        UUID m_Albedo;
        UUID m_Normal;
        UUID m_ORM;
        UUID m_Height;
        UUID m_Emissive;

        UUID m_AO;        // This texture not using in cpu or gpu anymore, just have to package into m_ORM
        UUID m_Roughness; // This texture not using in cpu or gpu anymore, just have to package into m_ORM
        UUID m_Metallic;  // This texture not using in cpu or gpu anymore, just have to package into m_ORM
    };

    struct MaterialInstance {
        Ref<Material> m_Base = nullptr;

        explicit MaterialInstance(const std::string& name);
        MaterialInstance(const MaterialInstance&) = default;

        // Instance override colors
        glm::vec4 m_BaseColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
        glm::vec4 m_NormalRMA = {};
        // TODO: add other types like emissive, shininess etc.

        [[nodiscard]] MaterialSSBO ConvertToGPUFormat() const;
    };
}
