//
// Created by pointerlost on 12/4/25.
//
#pragma once
#include <vector>
#include "Core/file_manager.h"
#include <Core/UUID.h>

namespace Real { struct MaterialInstance; }

namespace Real {

    struct MeshEntry {
        UUID m_SubMeshID;
        UUID m_MaterialID;
    };

    struct Model {
        std::vector<MeshEntry> m_Meshes;
        FileInfo m_FileInfo;

        void AddMesh(const UUID& uuid, const UUID& materialID) {
            m_Meshes.push_back({uuid, materialID});
        }

        [[nodiscard]] size_t GetMeshCount() const { return m_Meshes.size(); }
        [[nodiscard]] bool IsValid() const { return !m_Meshes.empty(); }
    };
}