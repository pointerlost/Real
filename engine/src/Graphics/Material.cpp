//
// Created by pointerlost on 10/13/25.
//
#include "Graphics/Material.h"
#include "Core/Services.h"
#include "Assets/TextureManager.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Real::graphics {

    MaterialInstance::MaterialInstance(const Ref<Material> &assetMaterial)
        : m_UUID(UUID{}), m_Base(assetMaterial)
    {
    }

    void MaterialInstance::ConvertToGPUFormat(MaterialSSBO& outData) const {
        auto& tm = Services::GetTextureManager();

        auto resolve = [&](
            const std::optional<GLTextureResourceHandle>& override,
            const GLTextureResourceHandle& base
            ) -> GLTextureResourceHandle
        {
            return override.has_value() ? *override : base;
        };

        const auto& GetIndex = [&](const GLTextureResourceHandle& handle) {
            return tm.GetTexture(handle)->GetGPUIndex();
        };

        const auto albedoHandle   = resolve( albedoOverride,   m_Base->albedo   );
        const auto normalHandle   = resolve( normalOverride,   m_Base->normal   );
        const auto ormHandle      = resolve( ormOverride,      m_Base->orm      );
        const auto heightHandle   = resolve( heightOverride,   m_Base->height   );
        const auto emissiveHandle = resolve( emissiveOverride, m_Base->emissive );

        outData.m_BindlessAlbedoIdx   = static_cast<int>( GetIndex(albedoHandle)   );
        outData.m_BindlessNormalIdx   = static_cast<int>( GetIndex(normalHandle)   );
        outData.m_BindlessORMIdx      = static_cast<int>( GetIndex(ormHandle)      );
        outData.m_BindlessHeightIdx   = static_cast<int>( GetIndex(heightHandle)   );
        outData.m_BindlessEmissiveIdx = static_cast<int>( GetIndex(emissiveHandle) );

        // Override colors
        outData.m_BaseColorFactor = baseColorFactor;
        outData.m_ORMFactor       = ormFactor;
    }

}
