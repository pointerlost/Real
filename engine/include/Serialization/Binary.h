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
    void WriteModel(const String &path, ModelBinaryHeader binaryHeader,
        const Vector<UUID>& meshUUIDs, const Vector<UUID>& materialUUIDs
    );
    // The second argument is MeshUUIDs and the third argument is MaterialUUIDs
    [[maybe_unused]] std::tuple<ModelBinaryHeader, Vector<UUID>, Vector<UUID>> LoadModel(const String& path);

    /* ********************************************* MESH STATE ********************************************* */
    void WriteMesh(const String& path, const MeshBinaryHeader &binaryHeader,
        const Vector<Vertex>& vertices, const Vector<u32>& indices
    );
    [[maybe_unused]] MeshLoadResult LoadMesh(const String& path);
}
