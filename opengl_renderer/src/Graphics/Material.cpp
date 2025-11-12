//
// Created by pointerlost on 10/13/25.
//
#include "Graphics/Material.h"

#include "Core/AssetManager.h"
#include "Core/Logger.h"
#include "Core/Services.h"

namespace Real {

    MaterialSSBO MaterialInstance::ConvertToGPUFormat() const {
        const auto& assetManager = Services::GetAssetManager();
        MaterialSSBO gpuData{};
        gpuData.m_BaseColor = m_BaseColor;
        gpuData.m_NormalRMA = m_NormalRMA;

        Ref<Texture> albedo = nullptr;
        Ref<Texture> normal = nullptr;
        Ref<Texture> rma    = nullptr;
        Ref<Texture> height = nullptr;

        if (m_Base->m_AlbedoMap)
            albedo = assetManager->GetTexture(m_Base->m_AlbedoMap->GetName());
        if (m_Base->m_NormalMap)
            normal = assetManager->GetTexture(m_Base->m_NormalMap->GetName());
        if (m_Base->m_rmaMap)
            rma    = assetManager->GetTexture(m_Base->m_rmaMap->GetName());
        if (m_Base->m_HeightMap)
            height = assetManager->GetTexture(m_Base->m_HeightMap->GetName());

        if (albedo)
            gpuData.m_BindlessAlbedoIdx = { albedo->GetIndex() };
        if (normal)
            gpuData.m_BindlessNormalIdx = { normal->GetIndex() };
        if (rma)
            gpuData.m_BindlessRMAIdx    = { rma->GetIndex()    };
        if (height)
            gpuData.m_BindlessHeightIdx = { height->GetIndex() };

        return gpuData;
    }

    /* TEXTURE ARRAY
        if (m_Base->m_AlbedoMap)
            albedo = assetManager->GetTexture(m_Base->m_AlbedoMap->GetName());
        if (m_Base->m_NormalMap)
            normal = assetManager->GetTexture(m_Base->m_NormalMap->GetName());
        if (m_Base->m_rmaMap)
            rma    = assetManager->GetTexture(m_Base->m_rmaMap->GetName());
        if (m_Base->m_HeightMap)
            height = assetManager->GetTexture(m_Base->m_HeightMap->GetName());

        // Upload texture lookup data to GPU (texIndex, texArrayIndex)
        if (albedo)
            gpuData.albedoMapLookupData = { albedo->GetArrayIndex(), albedo->GetIndex() };
        if (normal)
            gpuData.normalMapLookupData = { normal->GetArrayIndex(), normal->GetIndex() };
        if (rma)
            gpuData.rmaMapLookupData    = { rma->GetArrayIndex(),    rma->GetIndex()    };
        if (height)
            gpuData.heightMapLookupData = { height->GetArrayIndex(), height->GetIndex() };
     */
}
