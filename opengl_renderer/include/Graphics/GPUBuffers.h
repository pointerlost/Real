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
        glm::vec4 pos_cutoff{0.0}; // vec3 = position, float = cutoff
        glm::vec4 dir_outer{0.0}; // vec3 direction, float = outerCutoff
        glm::vec4 diffuse{1.0};
        glm::vec4 specular{0.7};
        int type = 0;
        // Attenuation parameters
        float constant = 1.0;
        float linear = 0.09;
        float quadratic = 0.002;
    };

    struct MaterialSSBO {
        glm::vec4 baseColor = glm::vec4(1.0, 0.2, 0.0, 1.0);
        glm::vec4 emissiveMetallic = glm::vec4(0.2f);
        float textureLayers[4]; // 0 = tex diffuse layer, 1 = tex specular layer, other indices padding (16-byte alignment)
        float roughnessShininess[4]; // 0 = roughness, 1 = shininess
    };

    struct CameraUBO {
        glm::vec4 position = glm::vec4(0.0);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 viewProjection = glm::mat4(1.0f);
    };

    struct GlobalUBO {
        glm::vec4 GlobalAmbient{0.1};
        int lightCount[4]; // 0 = light count, other indices padding
    };

}