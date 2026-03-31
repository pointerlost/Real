//
// Created by pointerlost on 3/23/26.
//
#include "Assets/MaterialManager.h"
#include "Assets/AssetImporter.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Graphics/Material.h"

namespace Real::assets {

    Ref<Material> MaterialManager::CreateBase(const String& name) {
        const auto uniqueName = GenerateUniqueName(name);
        const auto base       = CreateRef<Material>(UUID{}, uniqueName);

        m_Materials.emplace(base->id, base);
        m_MaterialNameToUUID.emplace(base->name, base->id);

        return m_Materials.at(base->id);
    }

    Ref<Material> MaterialManager::GetOrCreateBase(const String& name) {
        const String normalized = NormalizeName(name);

        if (m_MaterialNameToUUID.contains(normalized))
            return m_Materials[m_MaterialNameToUUID[normalized]];

        const String uniqueName = GenerateUniqueName(normalized);
        UUID uuid{};
        auto mat = CreateRef<Material>(uuid, uniqueName);

        m_Materials.emplace(uuid, mat);
        m_MaterialNameToUUID.emplace(uniqueName, uuid);

        return mat;
    }

    Ref<Material> MaterialManager::LoadBaseAsset(const UUID& uuid, const String& name) {
        if (m_Materials.contains(uuid))
            return m_Materials.at(uuid);

        auto mat = CreateRef<Material>(uuid, name);
        m_Materials.emplace(uuid, mat);
        m_MaterialNameToUUID.emplace(name, uuid);

        return mat;
    }

    Ref<Material> MaterialManager::GetBase(const UUID& uuid) const {
        const auto it = m_Materials.find(uuid);
        if (it == m_Materials.end()) {
            Warn("[GetBase] Material not found!");
            return nullptr;
        }
        return it->second;
    }

    Ref<Material> MaterialManager::GetBase(const String& name) const {
        const auto it = m_MaterialNameToUUID.find(name);
        if (it == m_MaterialNameToUUID.end()) {
            Warn("Material not found: " + name);
            return nullptr;
        }
        const auto mit = m_Materials.find(it->second);
        return (mit != m_Materials.end()) ? mit->second : nullptr;
    }

    UUID MaterialManager::CreateInstance(const UUID& assetUUID) {
        const auto base = GetBase(assetUUID);
        if (!base) {
            Warn("Material asset not found!");
            return UUID(0);
        }
        const auto instance = CreateRef<MaterialInstance>(base);
        instance->m_UUID = UUID{};
        m_MaterialInstances.emplace(instance->m_UUID, instance);
        return instance->m_UUID;
    }

    UUID MaterialManager::CreateInstance(const String& assetName) {
        const auto it = m_MaterialNameToUUID.find(assetName);
        if (it == m_MaterialNameToUUID.end() || it->second.IsNull()) {
            Warn("Material not found: " + assetName);
            return UUID(0);
        }
        return CreateInstance(it->second);
    }

    Ref<MaterialInstance> MaterialManager::GetInstance(const UUID& uuid) const {
        const auto it = m_MaterialInstances.find(uuid);
        if (it == m_MaterialInstances.end()) {
            Warn("Material instance not found: " + std::to_string(uuid));
            return nullptr;
        }
        return it->second;
    }

    UUID MaterialManager::GetBaseUUIDByName(const String& name) const {
        const auto it = m_MaterialNameToUUID.find(name);
        if (it == m_MaterialNameToUUID.end()) {
            Warn("No material asset with name: " + name);
            return UUID(0);
        }
        return it->second;
    }

    bool MaterialManager::BaseExists(const String& name) const {
        return m_MaterialNameToUUID.contains(name)
            && m_Materials.contains(m_MaterialNameToUUID.at(name));
    }

    void MaterialManager::RegisterBase(const Ref<Material>& material) {
        if (!m_Materials.contains(material->id))
            m_Materials.emplace(material->id, material);
    }

    void MaterialManager::Rename(const String& newName, const UUID& uuid) {
        auto& ai = Services::GetAssetImporter();
        auto& db = ai.GetAssetDB();

        db["materials"][std::to_string(uuid)]["name"] = newName;

        const auto& mat = m_Materials[uuid];
        m_MaterialNameToUUID.erase(mat->name);
        mat->name = newName;
        m_MaterialNameToUUID[newName] = uuid;

        ai.MarkDirtyAssetDB();
    }

    const std::unordered_map<UUID, Ref<Material>>& MaterialManager::GetAllBases() const {
        return m_Materials;
    }

    void MaterialManager::Update() {
    }

    String MaterialManager::GenerateUniqueName(const String& desired) const {
        String base = NormalizeName(desired);
        if (!m_MaterialNameToUUID.contains(base))
            return base;

        u32 index = 1;
        String candidate;
        do {
            candidate = base + "_" + std::to_string(index++);
        } while (m_MaterialNameToUUID.contains(candidate));

        return candidate;
    }

    String MaterialManager::NormalizeName(String name) const {
        Trim(name);
        if (name.empty()) name = "New_Material";
        return name;
    }

}
