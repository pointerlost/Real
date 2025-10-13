//
// Created by pointerlost on 10/10/25.
//
#include "Graphics/Light.h"
#include "Graphics/Transformations.h"

namespace Real {

    Light::Light() {
    }

    void Light::Update(Transformations& transform) {
    }

    LightSSBO Light::ConvertToGPUFormat() const {
        LightSSBO gpuData{};
        gpuData.diffuse  = glm::vec4(m_Diffuse, 1.0f);
        gpuData.specular = glm::vec4(m_Specular, 1.0f);
        return gpuData;
    }
}
