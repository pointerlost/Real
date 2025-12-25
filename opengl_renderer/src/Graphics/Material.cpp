//
// Created by pointerlost on 10/13/25.
//
#include "Graphics/Material.h"
#include "Core/AssetManager.h"
#include "Core/Services.h"

namespace Real {

    MaterialInstance::MaterialInstance(const Ref<Material> &assetMaterial)
        : m_UUID(UUID{}), m_Base(assetMaterial)
    {
    }

    MaterialSSBO MaterialInstance::ConvertToGPUFormat() const {
        const auto& am = Services::GetAssetManager();
        MaterialSSBO gpuData{};

        const auto& GetIndex = [&](const UUID& uuid, TextureType type) {
            return am->GetTexture(uuid, type)->GetIndex();
        };

        const UUID albedoUUID   = m_AlbedoOverride.value_or(m_Base->m_Albedo);
        const UUID normalUUID   = m_NormalOverride.value_or(m_Base->m_Normal);
        const UUID ormUUID      = m_ORMOverride.value_or(m_Base->m_ORM);
        const UUID heightUUID   = m_HeightOverride.value_or(m_Base->m_Height);
        const UUID emissiveUUID = m_EmissiveOverride.value_or(m_Base->m_Emissive);

        gpuData.m_BindlessAlbedoIdx   = GetIndex(albedoUUID,   TextureType::ALBEDO);
        gpuData.m_BindlessNormalIdx   = GetIndex(normalUUID,   TextureType::NORMAL);
        gpuData.m_BindlessORMIdx      = GetIndex(ormUUID,      TextureType::ORM);
        gpuData.m_BindlessHeightIdx   = GetIndex(heightUUID,   TextureType::HEIGHT);
        gpuData.m_BindlessEmissiveIdx = GetIndex(emissiveUUID, TextureType::EMISSIVE);

        // Override colors
        gpuData.m_BaseColor = m_BaseColor;
        gpuData.m_NormalORM = m_NormalRMA;

        return gpuData;
    }

}
