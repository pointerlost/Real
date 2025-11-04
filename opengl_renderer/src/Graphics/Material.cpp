//
// Created by pointerlost on 10/13/25.
//
#include "Graphics/Material.h"

namespace Real {

    MaterialSSBO MaterialInstance::ConvertToGPUFormat() const {
        MaterialSSBO gpuData{};
        gpuData.m_BaseColor = m_BaseColor;
        gpuData.m_NormalRMA = m_NormalRMA;

        gpuData.albedoMapIdx = m_Base->m_AlbedoMap->GetIndex();
        gpuData.normalMapIdx = m_Base->m_NormalMap->GetIndex();
        gpuData.rmaMapIdx = m_Base->m_rmaMap->GetIndex();
        gpuData.heightMapIdx = m_Base->m_HeightMap->GetIndex();
        return gpuData;
    }
}
