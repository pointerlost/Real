//
// Created by pointerlost on 10/13/25.
//
#include "Graphics/Material.h"
#include "../../include/Assets/AssetManager.h"
#include "Core/Services.h"
#include "../../include/Graphics/Texture/Texture.h"

namespace Real {

    MaterialInstance::MaterialInstance(const Ref<Material> &assetMaterial)
        : m_UUID(UUID{}), m_Base(assetMaterial)
    {
    }

    void MaterialInstance::ConvertToGPUFormat(MaterialSSBO& outData) const {
        const auto& am = Services::GetAssetManager();

        const auto& GetIndex = [&](const UUID& uuid, TextureType type) {
            return am->GetTexture(uuid, type)->GetIndex();
        };

        const UUID albedoUUID   = m_AlbedoOverride.value_or(m_Base->m_Albedo);
        const UUID normalUUID   = m_NormalOverride.value_or(m_Base->m_Normal);
        const UUID ormUUID      = m_ORMOverride.value_or(m_Base->m_ORM);
        const UUID heightUUID   = m_HeightOverride.value_or(m_Base->m_Height);
        const UUID emissiveUUID = m_EmissiveOverride.value_or(m_Base->m_Emissive);

        outData.m_BindlessAlbedoIdx   = static_cast<int>(GetIndex(albedoUUID,   TextureType::ALBEDO));
        outData.m_BindlessNormalIdx   = static_cast<int>(GetIndex(normalUUID,   TextureType::NORMAL));
        outData.m_BindlessORMIdx      = static_cast<int>(GetIndex(ormUUID,      TextureType::ORM));
        outData.m_BindlessHeightIdx   = static_cast<int>(GetIndex(heightUUID,   TextureType::HEIGHT));
        outData.m_BindlessEmissiveIdx = static_cast<int>(GetIndex(emissiveUUID, TextureType::EMISSIVE));

        // Override colors
        outData.m_BaseColorFactor = m_BaseColorFactor;
        outData.m_ORMFactor = m_ORMFactor;
    }

}
