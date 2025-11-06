//
// Created by pointerlost on 10/13/25.
//
#include "Graphics/Material.h"

#include "Core/AssetManager.h"
#include "Core/Services.h"

namespace Real {

    MaterialSSBO MaterialInstance::ConvertToGPUFormat() const {
        const auto& assetManager = Services::GetAssetManager();
        MaterialSSBO gpuData{};
        gpuData.m_BaseColor = m_BaseColor;
        gpuData.m_NormalRMA = m_NormalRMA;

        const auto& albedo = assetManager->GetTexture(m_Base->m_AlbedoMapName);
        const auto& normal = assetManager->GetTexture(m_Base->m_NormalMapName);
        const auto& rma    = assetManager->GetTexture(m_Base->m_rmaMapName);
        const auto& height = assetManager->GetTexture(m_Base->m_HeightMapName);

        // Upload texture lookup data to GPU, texIndex, texArrayIndex
        gpuData.albedoMapLookupData = { albedo->GetIndex(), albedo->GetArrayIndex() };
        gpuData.normalMapLookupData = { normal->GetIndex(), normal->GetArrayIndex() };
        gpuData.rmaMapLookupData    = { rma->GetIndex(),    rma->GetArrayIndex()    };
        gpuData.heightMapLookupData = { height->GetIndex(), height->GetArrayIndex() };
        return gpuData;
    }
}
