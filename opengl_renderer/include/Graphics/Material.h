//
// Created by pointerlost on 10/12/25.
//
#pragma once
#include <glm/ext.hpp>
#include "GPUBuffers.h"
#include "Texture.h"
#include "Core/Services.h"
#include "Core/Utils.h"

namespace Real { struct OpenGLTexture; }

namespace Real {

    struct Material {
        Ref<OpenGLTexture> m_Albedo;
        Ref<OpenGLTexture> m_Normal;
        Ref<OpenGLTexture> m_RMA;
        Ref<OpenGLTexture> m_Height;
    };

    struct MaterialInstance {
        Ref<Material> m_Base = nullptr;

        explicit MaterialInstance(const std::string& name,
            const std::array<std::string, 4>& extensions = {".jpg", ".jpg", ".jpg", ".jpg"}
        );
        MaterialInstance(const MaterialInstance&) = default;

        // Instance override colors
        glm::vec4 m_BaseColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
        glm::vec4 m_NormalRMA = {};

        [[nodiscard]] MaterialSSBO ConvertToGPUFormat() const;
    };
}
