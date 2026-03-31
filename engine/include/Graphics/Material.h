//
// Created by pointerlost on 10/12/25.
//
#pragma once
#include <utility>
#include "GPUBuffers.h"
#include "Common/Utils.h"
#include "Core/UUID.h"
#include "Resource/ResourceHandle.h"

namespace Real::graphics {

    struct Material {
        UUID   id   {};
        String name {};

        GLTextureResourceHandle albedo;
        GLTextureResourceHandle normal;
        GLTextureResourceHandle orm;
        GLTextureResourceHandle height;
        GLTextureResourceHandle emissive;

        Material()                = default;
        Material(const Material&) = delete;
        explicit Material(const UUID& uuid) : id(uuid) {}
        explicit Material(const UUID& uuid, String n) : id(uuid), name(std::move(n)) {}
    };

    struct MaterialInstance {
        UUID                m_UUID {}; // Instance UUID, initialized with constructor
        Ref<const Material> m_Base = nullptr;

                 MaterialInstance(const MaterialInstance&) = default;
        explicit MaterialInstance(const Ref<Material>& assetMaterial);

        // TODO: Factors should add into AssetDB
        // Instance override colors
        math::Vec4 baseColorFactor = math::Vec4(1.0, 1.0, 1.0, 1.0);
        math::Vec4 ormFactor = {}; // last index padding

        // Instance override textures
        std::optional<GLTextureResourceHandle> albedoOverride;
        std::optional<GLTextureResourceHandle> normalOverride;
        std::optional<GLTextureResourceHandle> ormOverride;
        std::optional<GLTextureResourceHandle> heightOverride;
        std::optional<GLTextureResourceHandle> emissiveOverride;
        // TODO: add other types like emissive, shininess etc.

        void ConvertToGPUFormat(MaterialSSBO& outData) const;
    };
}
