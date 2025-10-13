//
// Created by pointerlost on 10/13/25.
//
#pragma once
#include <glm/ext.hpp>

namespace Real {

    struct TransformSSBO {
        glm::mat4 modelMatrix;
        // TODO: Add normal matrix!!
    };

    struct LightSSBO {
        glm::vec4 diffuse  = glm::vec4(1.0f);
        glm::vec4 specular = glm::vec4(0.7f);
    };

    struct MaterialSSBO {
        glm::vec4 baseColor = glm::vec4(1.0f);
        glm::vec4 emissiveMetallic = glm::vec4(0.2f);
        float roughness[4]; // 0 = roughness, other indices padding (16-byte alignment)
    };

    struct CameraUBO {
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 viewProjection = glm::mat4(1.0f);
    };

}