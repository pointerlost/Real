//
// Created by pointerlost on 10/9/25.
//
#include "Graphics/Transformations.h"

namespace Real {

    void Transformations::Update() {
        if (!m_ModelMatrixDirty) return;

        const auto translate = glm::translate(glm::mat4(1.0f), m_Translate);
        const auto rotate = glm::mat4_cast(m_Rotate);
        const auto scale = glm::scale(glm::mat4(1.0f), m_Scale);
        // Warning...
        // if we do world-space movement so order should be R * T * S
        m_ModelMatrix = translate * rotate * scale;

        m_ModelMatrixDirty = false;
    }

    TransformSSBO Transformations::ConvertToGPUFormat() {
        Update();
        TransformSSBO gpuData{};
        gpuData.modelMatrix = m_ModelMatrix;
        gpuData.normalMatrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_ModelMatrix))));
        return gpuData;
    }
}
