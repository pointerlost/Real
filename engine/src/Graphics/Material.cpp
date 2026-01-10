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

        const auto& GetIndex = [&](const UUID& uuid, const TextureType type) {
            return am->GetTexture(uuid, type)->GetIndex();
        };

        const UUID albedoUUID   = m_AlbedoOverride.value_or(m_Base->m_Albedo);
        const UUID normalUUID   = m_NormalOverride.value_or(m_Base->m_Normal);
        const UUID ormUUID      = m_ORMOverride.value_or(m_Base->m_ORM);
        const UUID heightUUID   = m_HeightOverride.value_or(m_Base->m_Height);
        const UUID emissiveUUID = m_EmissiveOverride.value_or(m_Base->m_Emissive);

        gpuData.m_BindlessAlbedoIdx   = static_cast<uint>(GetIndex(albedoUUID,   TextureType::ALBEDO));
        gpuData.m_BindlessNormalIdx   = static_cast<uint>(GetIndex(normalUUID,   TextureType::NORMAL));
        gpuData.m_BindlessORMIdx      = static_cast<uint>(GetIndex(ormUUID,      TextureType::ORM));
        gpuData.m_BindlessHeightIdx   = static_cast<uint>(GetIndex(heightUUID,   TextureType::HEIGHT));
        gpuData.m_BindlessEmissiveIdx = static_cast<uint>(GetIndex(emissiveUUID, TextureType::EMISSIVE));

        // Override colors
        gpuData.m_BaseColorFactor = m_BaseColorFactor;
        gpuData.m_ORMFactor = m_ORMFactor;

        return gpuData;
    }

}
