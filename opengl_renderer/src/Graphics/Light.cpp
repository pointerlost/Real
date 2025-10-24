//
// Created by pointerlost on 10/10/25.
//
#include "Graphics/Light.h"

#include "Core/Services.h"
#include "Core/Timer.h"
#include "Graphics/Transformations.h"
#include "Input/Input.h"
#include "Input/Keycodes.h"

namespace Real {

    Light::Light(LightType type) : m_Type(type) {
    }

    void Light::Update(Transformations& transform) {
    }

    LightSSBO Light::ConvertToGPUFormat(Transformations& transform) {
        LightSSBO gpuData{};                                     // Convert angles to cosine
        gpuData.pos_cutoff = glm::vec4(transform.GetTranslate(),  glm::cos(glm::radians(m_CutOff))); // Inner cone
        gpuData.dir_outer  = glm::vec4(transform.GetWorldDirection(), glm::cos(glm::radians(m_OuterCutOff))); // Outer cone
        gpuData.diffuse   = glm::vec4(m_Diffuse, 1.0);
        gpuData.specular  = glm::vec4(m_Specular, 1.0);
        gpuData.constant  = m_Constant;
        gpuData.linear    = m_Linear;
        gpuData.quadratic = m_Quadratic;
        gpuData.type = static_cast<int>(m_Type);
        return gpuData;
    }
}
