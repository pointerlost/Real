//
// Created by pointerlost on 12/4/25.
//
#pragma once
#include <utility>
#include <vector>
#include <Core/UUID.h>
#include "Assets/FileManager.h"
#include "Common/Types.h"

namespace Real {

    struct Model {
        UUID m_UUID{};
        Vector<UUID> m_MeshUUIDs{};
        Vector<UUID> m_MaterialAssetUUIDs{};
        fs::FileInfo m_FileInfo{};
        String m_Name = "NULL"; // Engine asset name

        void AddMesh(const UUID& uuid) {
            m_MeshUUIDs.push_back(uuid);
        }

        Model() = default;
        Model(const Model&) = default;
        Model(const UUID& modelUUID, fs::FileInfo info) : m_UUID(modelUUID), m_FileInfo(std::move(info)) {}
    };
}