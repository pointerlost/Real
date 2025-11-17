//
// Created by pointerlost on 10/12/25.
//
#pragma once
#include <glm/ext.hpp>
#include "GPUBuffers.h"
#include "Texture.h"
#include "Core/Utils.h"

namespace Real {
    struct OpenGLTexture;
}

namespace Real {

    struct Material {
        Ref<OpenGLTexture> m_AlbedoMap;
        Ref<OpenGLTexture> m_NormalMap;
        Ref<OpenGLTexture> m_rmaMap;
        Ref<OpenGLTexture> m_HeightMap;
    };

    struct MaterialInstance {
        glm::vec4 m_BaseColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
        glm::vec4 m_NormalRMA = {};

        Ref<Material> m_Base = CreateRef<Material>();

        [[nodiscard]] MaterialSSBO ConvertToGPUFormat() const;
    };
}
