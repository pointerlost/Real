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
        gpuData.roughnessShininess[0] = m_Base->Roughness;
        gpuData.roughnessShininess[1] = m_Base->Shininess;
        gpuData.textureLayers[0] = static_cast<float>(m_Base->GetTextureIndex(TextureType::BaseColor));
        gpuData.textureLayers[1] = static_cast<float>(m_Base->GetTextureIndex(TextureType::Specular));
        return gpuData;
    }
}
