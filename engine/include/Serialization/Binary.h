//
// Created by pointerlost on 12/15/25.
//
#pragma once
#include <string>
#include <vector>
#include "Common/RealTypes.h"
#include "Core/Utils.h"

namespace Real::serialization::binary {

    /* ********************************************* MODEL STATE ********************************************* */
    void WriteModel(const std::string &path, ModelBinaryHeader binaryHeader,
        const std::vector<UUID>& meshUUIDs, const std::vector<UUID>& materialUUIDs
    );
    // The second argument is MeshUUIDs and the third argument is MaterialUUIDs
    [[maybe_unused]] std::tuple<ModelBinaryHeader, std::vector<UUID>, std::vector<UUID>> LoadModel(const std::string& path);

    /* ********************************************* MESH STATE ********************************************* */
    void WriteMesh(const std::string& path, const MeshBinaryHeader &binaryHeader,
        const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices
    );
    [[maybe_unused]] MeshLoadResult LoadMesh(const std::string& path);
}
