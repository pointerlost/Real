//
// Created by pointerlost on 10/10/25.
//
#include "Graphics/Light.h"
#include "Graphics/Transformations.h"

namespace Real {

    Light::Light() {
    }

    void Light::Update(Transformations& transform) {
        m_Position = transform.GetPosition();
    }

    LightSSBO Light::ConvertToGPUFormat(Transformations& transform) {
        Update(transform);
        LightSSBO gpuData{};
        gpuData.diffuse  = glm::vec4(m_Diffuse, 1.0);
        gpuData.specular = glm::vec4(m_Specular, 1.0);
        gpuData.position = glm::vec4(m_Position, 1.0);
        return gpuData;
    }
}
