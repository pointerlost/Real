//
// Created by pointerlost on 3/23/26.
//
#pragma once
#include "Common/Types.h"
#include "Core/UUID.h"

namespace Real::graphics { struct Vertex; }

namespace Real::assets {

    constexpr u32 REAL_MAGIC = 0x4C414552;

#pragma pack(push, 1)
    struct ModelBinaryHeader {
        u32 magic     = REAL_MAGIC;
        u32 version   = 1;
        u32 meshCount{};
        u64 uuid{};
    };

    struct MeshBinaryHeader {
        u32 magic        = REAL_MAGIC;
        u32 version      = 1;
        u64 uuid{};
        u64 materialUUID{};
        u64 vertexCount{};
        u64 indexCount{};
        u64 vertexOffset{};
        u64 indexOffset{};
    };
#pragma pack(pop)

    struct MeshAsset {
        UUID uuid;
        u64  vertexCount{};
        u64  indexCount{};
        u64  vertexOffset{};
        u64  indexOffset{};
    };

    struct MeshLoadResult {
        MeshBinaryHeader         header;
        Vector<graphics::Vertex> vertices;
        Vector<u32>              indices;
    };

}
