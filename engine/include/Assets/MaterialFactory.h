//
// Created by pointerlost on 3/23/26.
//
#pragma once
#include <unordered_map>
#include "Core/Utils.h"
#include "Core/UUID.h"

namespace Real {
    struct MaterialInstance;
    struct Material;
}

namespace Real {

    class MaterialFactory {
    public:
        // Creation
        [[nodiscard]] Ref<Material> CreateBase(const String& name);
        [[nodiscard]] Ref<Material> GetOrCreateBase(const String& name);
        [[nodiscard]] Ref<Material> LoadBaseAsset(const UUID& uuid, const String& name);
        [[nodiscard]] UUID          CreateInstance(const UUID& assetUUID);
        [[nodiscard]] UUID          CreateInstance(const String& assetName);

        // Lookup
        [[nodiscard]] Ref<Material>         GetBase(const UUID& uuid) const;
        [[nodiscard]] Ref<Material>         GetBase(const String& name) const;
        [[nodiscard]] Ref<MaterialInstance> GetInstance(const UUID& uuid) const;
        [[nodiscard]] UUID                  GetBaseUUIDByName(const String& name) const;
        [[nodiscard]] bool                  BaseExists(const String& name) const;
        [[nodiscard]] const std::unordered_map<UUID, Ref<Material>>& GetAllBases() const;

        // Mutation
        void RegisterBase(const Ref<Material>& material);
        void Rename(const String& newName, const UUID& uuid);

        void Update();

    private:
        std::unordered_map<UUID, Ref<Material>>         m_Materials;
        std::unordered_map<String, UUID>                m_MaterialNameToUUID;
        std::unordered_map<UUID, Ref<MaterialInstance>> m_MaterialInstances;

    private:
        [[nodiscard]] String GenerateUniqueName(const String& desired) const;
        [[nodiscard]] String NormalizeName(String name) const;
    };

}
