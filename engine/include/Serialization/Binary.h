//
// Created by pointerlost on 12/15/25.
//
#pragma once
#include "Assets/AssetTypes.h"
#include "Common/Types.h"
#include "../Common/Utils.h"

namespace Real::graphics { struct Vertex; }

namespace Real::serialization::binary {

    /* ********************************************* MODEL STATE ********************************************* */
    void WriteModel(const String &path, assets::ModelBinaryHeader binaryHeader,
        const Vector<UUID>& meshUUIDs, const Vector<UUID>& materialUUIDs
    );
    // The second argument is MeshUUIDs and the third argument is MaterialUUIDs
    [[maybe_unused]] std::tuple<assets::ModelBinaryHeader, Vector<UUID>, Vector<UUID>> LoadModel(const String& path);

    /* ********************************************* MESH STATE ********************************************* */
    void WriteMesh(const String& path, const assets::MeshBinaryHeader &binaryHeader,
        const Vector<graphics::Vertex>& vertices, const Vector<u32>& indices
    );
    [[maybe_unused]] assets::MeshLoadResult LoadMesh(const String& path);
}
