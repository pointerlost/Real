//
// Created by pointerlost on 12/4/25.
//
#pragma once
#include <utility>
#include <vector>
#include <Core/UUID.h>
#include "Common/RealTypes.h"

namespace Real {
    struct ModelBinaryHeader;
}

namespace Real { struct MaterialInstance; }

namespace Real {

    struct Model {
        UUID m_UUID{};
        std::vector<UUID> m_MeshUUIDs{};
        FileInfo m_FileInfo{};
        std::string m_Name = "NULL"; // Engine asset name

        void AddMesh(const UUID& uuid) {
            m_MeshUUIDs.push_back(uuid);
        }

        Model() = default;
        Model(const Model&) = default;
        Model(const UUID& uuid, FileInfo info) : m_UUID(uuid), m_FileInfo(std::move(info)) {}
    };
}