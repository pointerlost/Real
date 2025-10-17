//
// Created by pointerlost on 10/13/25.
//
#pragma once
#include <glm/ext.hpp>

namespace Real {

    struct TransformSSBO {
        glm::mat4 modelMatrix;
        glm::mat4 normalMatrix;
    };

    struct LightSSBO {
        glm::vec4 position{0.0}; // w unused (padding)
        glm::vec4 direction{0.0};
        glm::vec4 diffuse{1.0};
        glm::vec4 specular{0.7};
        glm::vec4 constLinQuadratic{1.0, 0.09, 0.032, 0.0}; // constant,linear,quadratic | w unused (padding)
    };

    struct MaterialSSBO {
        glm::vec4 baseColor = glm::vec4(1.0, 0.2, 0.0, 1.0);
        glm::vec4 emissiveMetallic = glm::vec4(0.2f);
        float roughnessTexLayer[4]; // 0 = roughness, 1 = texture layer, other indices padding (16-byte alignment)
    };

    struct CameraUBO {
        glm::vec4 position = glm::vec4(0.0);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 viewProjection = glm::mat4(1.0f);
    };

}