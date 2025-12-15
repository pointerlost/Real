//
// Created by pointerlost on 12/4/25.
//
#pragma once
#include <utility>
#include <vector>
#include "Core/file_manager.h"
#include <Core/UUID.h>

#include "Common/RealTypes.h"

namespace Real {
    struct ModelBinaryHeader;
}

namespace Real { struct MaterialInstance; }

namespace Real {

    struct MeshEntry {
        UUID m_SubMeshID;
        UUID m_MaterialID;
    };

    struct Model {
        std::vector<MeshEntry> m_Meshes;
        FileInfo m_FileInfo;
        UUID m_UUID;
        std::string m_Name; // Engine asset name

        void AddMesh(const UUID& uuid, const UUID& materialID) {
            m_Meshes.push_back({uuid, materialID});
        }

        [[nodiscard]] size_t GetMeshCount() const { return m_Meshes.size(); }
        [[nodiscard]] bool IsValid() const { return !m_Meshes.empty(); }

        Model() = default;
        Model(const Model&) = default;
        explicit Model(const ModelBinaryHeader& binary, FileInfo info)
            : m_Meshes(binary.m_MeshData), m_FileInfo(std::move(info)), m_UUID(binary.m_UUID) {}
    };
}