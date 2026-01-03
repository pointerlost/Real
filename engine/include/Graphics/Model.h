//
// Created by pointerlost on 12/4/25.
//
#pragma once
#include <utility>
#include <vector>
#include <Core/UUID.h>
#include "Common/RealTypes.h"

namespace Real {

    struct Model {
        UUID m_UUID{};
        std::vector<UUID> m_MeshUUIDs{};
        std::vector<UUID> m_MaterialAssetUUIDs{};
        FileInfo m_FileInfo{};
        std::string m_Name = "NULL"; // Engine asset name

        void AddMesh(const UUID& uuid) {
            m_MeshUUIDs.push_back(uuid);
        }

        Model() = default;
        Model(const Model&) = default;
        Model(const UUID& modelUUID, FileInfo info) : m_UUID(modelUUID), m_FileInfo(std::move(info)) {}
    };
}