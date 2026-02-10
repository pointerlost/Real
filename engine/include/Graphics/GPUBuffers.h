//
// Created by pointerlost on 10/13/25.
//
#pragma once
#include "Math/Mat4.h"
#include "Math/Vec4.h"

namespace Real {

    struct TransformSSBO {
        math::Mat4 modelMatrix;
        math::Mat4 normalMatrix;
    };

    struct LightSSBO {
        math::Vec4 pos_cutoff{0.0}; // vec3 = position, float = cutoff
        math::Vec4 dir_outer{0.0}; // vec3 direction, float = outerCutoff
        math::Vec4 radiance{0.0};
        int type = 0;
        // Attenuation parameters
        float constant = 1.0;
        float linear = 0.09;
        float quadratic = 0.002;
    };

    struct MaterialSSBO {
        math::Vec4 m_BaseColorFactor = math::Vec4(1.0, 1.0, 1.0, 1.0);
        // 0 = ambient occlusion, 1 = roughness, 2 = metallic, 3 = padding
        math::Vec4 m_ORMFactor = math::Vec4{1.0, 1.0, 1.0, 1.0};

        // lookup table provides us texIndex and texArrayIndex
        int m_BindlessAlbedoIdx;
        int m_BindlessNormalIdx;
        int m_BindlessORMIdx;
        int m_BindlessHeightIdx;
        int m_BindlessEmissiveIdx;
        int pad1_, pad2_, pad3_;
    };

    struct FrameUBO {
        math::Vec4 position       = math::Vec4(0.0f);
        math::Mat4 view           = math::Mat4(1.0f);
        math::Mat4 projection     = math::Mat4(1.0f);
        math::Mat4 viewProjection = math::Mat4(1.0f);
    };

    struct GlobalUBO {
        math::Vec4 GlobalAmbient{0.1};
        int lightCount[4]{}; // 0 = light count, other indices padding
    };

}
