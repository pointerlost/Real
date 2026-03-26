//
// Created by pointerlost on 10/12/25.
//
#pragma once
#include <utility>
#include "GPUBuffers.h"
#include "Common/Utils.h"
#include "Core/UUID.h"

namespace Real::graphics {

    // TODO: Need material baking system to optimize run-time
    struct Material {
        UUID m_UUID = UUID(0);
        String m_Name{};

        UUID m_Albedo   = UUID(0);
        UUID m_Normal   = UUID(0);
        UUID m_ORM      = UUID(0);
        UUID m_Height   = UUID(0);
        UUID m_Emissive = UUID(0);

        Material() = default;
        Material(const Material&) = default;
        explicit Material(const UUID& uuid) : m_UUID(uuid) {}
        explicit Material(const UUID& uuid, String  name) : m_UUID(uuid), m_Name(std::move(name)) {}
    };

    struct MaterialInstance {
        UUID m_UUID{}; // Instance UUID, initialized with constructor
        Ref<const Material> m_Base = nullptr;

        explicit MaterialInstance(const Ref<Material>& assetMaterial);
        MaterialInstance(const MaterialInstance&) = default;

        // TODO: Factors should add into AssetDB
        // Instance override colors
        math::Vec4 m_BaseColorFactor = math::Vec4(1.0, 1.0, 1.0, 1.0);
        math::Vec4 m_ORMFactor = {}; // last index padding

        // Instance override textures
        std::optional<UUID> m_AlbedoOverride;
        std::optional<UUID> m_NormalOverride;
        std::optional<UUID> m_ORMOverride;
        std::optional<UUID> m_HeightOverride;
        std::optional<UUID> m_EmissiveOverride;
        // TODO: add other types like emissive, shininess etc.

        void ConvertToGPUFormat(MaterialSSBO& outData) const;
    };
}
