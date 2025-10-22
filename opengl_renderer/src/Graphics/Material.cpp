//
// Created by pointerlost on 10/13/25.
//
#include "Graphics/Material.h"

#include "Core/Logger.h"

namespace Real {

    MaterialSSBO MaterialInstance::ConvertToGPUFormat() const {
        MaterialSSBO gpuData{};
        gpuData.baseColor = m_Base->BaseColor;
        gpuData.emissiveMetallic = glm::vec4(m_Base->Emissive, m_Base->Metallic);
        gpuData.roughnessTexLayer[0] = m_Base->Roughness;
        gpuData.roughnessTexLayer[1] = static_cast<float>(m_Base->GetTextureIndex(TextureType::BaseColor));
        gpuData.roughnessTexLayer[2] = static_cast<float>(m_Base->GetTextureIndex(TextureType::Specular));
        return gpuData;
    }
}
