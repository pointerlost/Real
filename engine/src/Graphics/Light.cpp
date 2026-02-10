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

    void Light::Update(Transform& transform) {
    }

    LightSSBO Light::ConvertToGPUFormat(const Transform& transform) {
        LightSSBO gpuData{};                                // Convert angles to cosine
        gpuData.pos_cutoff = math::Vec4(transform.position,  math::cos(math::DegreesToRadians(m_CutOff)));      // Inner cone
        gpuData.dir_outer  = math::Vec4(transform.Forward(), math::cos(math::DegreesToRadians(m_OuterCutOff))); // Outer cone
        gpuData.radiance   = math::Vec4(m_Radiance, 1.0); // w unused
        gpuData.constant   = m_Constant;
        gpuData.linear     = m_Linear;
        gpuData.quadratic  = m_Quadratic;
        gpuData.type       = static_cast<int>(m_Type);
        return gpuData;
    }
}
