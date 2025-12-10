//
// Created by pointerlost on 10/13/25.
//
#include "Graphics/Material.h"
#include "Core/AssetManager.h"
#include "Core/Logger.h"
#include "Core/Services.h"

namespace Real {
    MaterialInstance::MaterialInstance(const std::string &name) {
        const auto& am = Services::GetAssetManager();
        m_Base = am->GetOrCreateMaterialBase(name);
    }

    MaterialSSBO MaterialInstance::ConvertToGPUFormat() const {
        MaterialSSBO gpuData{};

        gpuData.m_BindlessAlbedoIdx = { m_Base->m_Albedo->GetIndex() };
        gpuData.m_BindlessNormalIdx = { m_Base->m_Normal->GetIndex() };
        gpuData.m_BindlessRMAIdx    = { m_Base->m_ORM->GetIndex()    };
        gpuData.m_BindlessHeightIdx = { m_Base->m_Height->GetIndex() };

        // Override colors
        gpuData.m_BaseColor = m_BaseColor;
        gpuData.m_NormalRMA = m_NormalRMA;

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
