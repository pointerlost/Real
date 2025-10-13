//
// Created by pointerlost on 10/13/25.
//
#include "Graphics/Material.h"

namespace Real {

    MaterialSSBO MaterialInstance::ConvertToGPUFormat() const {
        MaterialSSBO gpuData{};
        gpuData.baseColor = m_Base->BaseColor;
        gpuData.emissiveMetallic = glm::vec4(m_Base->emissive, m_Base->Metallic);
        gpuData.roughness[0] = m_Base->Roughness;
        return gpuData;
    }
}
