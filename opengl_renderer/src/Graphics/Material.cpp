//
// Created by pointerlost on 10/13/25.
//
#include "Graphics/Material.h"

#include "Core/Logger.h"

namespace Real {

    MaterialSSBO MaterialInstance::ConvertToGPUFormat() const {
        MaterialSSBO gpuData{};
        gpuData.m_BaseColor = m_BaseColor;
        gpuData.m_NormalRMA = m_NormalRMA;

        gpuData.albedoMapIdx = m_Base->albedoMap->m_Index;
        gpuData.metallicMapIdx = m_Base->metallicMap->m_Index;
        gpuData.roughnessMapIdx = m_Base->roughnessMap->m_Index;
        gpuData.normalMapIdx = m_Base->normalMap->m_Index;
        gpuData.aoMapIdx = m_Base->aoMap->m_Index;
        gpuData.heightMapIdx = m_Base->heightMap->m_Index;
        return gpuData;
    }
}
