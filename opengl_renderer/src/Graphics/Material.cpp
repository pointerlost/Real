//
// Created by pointerlost on 10/13/25.
//
#include "Graphics/Material.h"
#include "Core/AssetManager.h"
#include "Core/Logger.h"
#include "Core/Services.h"

namespace Real {

    MaterialInstance::MaterialInstance(const UUID& uuid) {
        const auto& am = Services::GetAssetManager();
        m_Base = am->GetOrCreateMaterialBase(uuid);
    }

    MaterialSSBO MaterialInstance::ConvertToGPUFormat() const {
        const auto& am = Services::GetAssetManager();
        MaterialSSBO gpuData{};

        const auto& GetIndex = [&](const UUID& uuid, TextureType type) {
            return am->GetTexture(uuid, type)->GetIndex();
        };

        gpuData.m_BindlessAlbedoIdx   = { GetIndex(m_Base->m_Albedo,   TextureType::ALBEDO)   };
        gpuData.m_BindlessNormalIdx   = { GetIndex(m_Base->m_Normal,   TextureType::NORMAL)   };
        gpuData.m_BindlessORMIdx      = { GetIndex(m_Base->m_ORM,      TextureType::ORM)      };
        gpuData.m_BindlessHeightIdx   = { GetIndex(m_Base->m_Height,   TextureType::HEIGHT)   };
        gpuData.m_BindlessEmissiveIdx = { GetIndex(m_Base->m_Emissive, TextureType::EMISSIVE) };

        // Override colors
        gpuData.m_BaseColor = m_BaseColor;
        gpuData.m_NormalORM = m_NormalRMA;

        return gpuData;
    }

}
