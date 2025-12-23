//
// Created by pointerlost on 12/15/25.
//
#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "Common/RealTypes.h"
#include "Core/Utils.h"

namespace Real {
    struct Model;
    struct ModelBinaryHeader;
    struct MeshBinaryHeader;
    struct UUID;

    namespace Graphics {
        struct Vertex;
    }
}

namespace Real::serialization::binary {

    /* ********************************************* MODEL STATE ********************************************* */
    void WriteModel(const std::string &path, ModelBinaryHeader binaryHeader, const std::vector<UUID>& meshUUIDs);
    [[maybe_unused]] std::pair<ModelBinaryHeader, std::vector<UUID>> LoadModel(const std::string& path);

    /* ********************************************* MESH STATE ********************************************* */
    void WriteMesh(const std::string& path, const MeshBinaryHeader &binaryHeader,
        const std::vector<Graphics::Vertex>& vertices, const std::vector<uint64_t>& indices
    );
    [[maybe_unused]] MeshLoadResult LoadMesh(const std::string& path);
}
