//
// Created by pointerlost on 10/10/25.
//
#include "Graphics/Light.h"

#include "Graphics/GPUBuffers.h"
#include "Graphics/Transformations.h"
#include "Math/Math.h"

namespace Real {

    Light::Light(Mode type) : m_Type(type) {
    }

    void Light::Update(Transform& transform) {
    }

    void Light::ConvertToGPUFormat(const Transform& transform, LightSSBO& outData) {
        outData.pos_cutoff = math::Vec4(
            transform.GetWorldPosition(),
            math::cos(math::DegreesToRadians(m_CutOff))
        ); // Inner cone
        outData.dir_outer  = math::Vec4(
            transform.Forward(),
            math::cos(math::DegreesToRadians(m_OuterCutOff))
        ); // Outer cone

        outData.radiance   = math::Vec4(m_Radiance, 1.0); // w unused
        outData.constant   = m_Constant;
        outData.linear     = m_Linear;
        outData.quadratic  = m_Quadratic;
        outData.type       = static_cast<int>(m_Type);
    }
}
