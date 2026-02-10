//
// Created by pointerlost on 12/6/25.
//
#pragma once
#include "Macros.h"
#include <vector>
#include <Core/UUID.h>

#include "Graphics/Debug/DebugTypes.h"
#include "Math/Mat4.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"

namespace Real {

    struct Vertex {
        math::Vec3 position;
        math::Vec3 normal;
        math::Vec2 UV;
    };

    struct TextureData {
        void* data = nullptr;
        int channelCount = 0;
        int width = 0;
        int height = 0;
        int dataSize = 0;
        int format = {};
        int internalFormat = {};
    };

    struct FileInfo {
        std::string name; // Full name
        std::string stem; // Name without extension
        std::string path; // Full path
        std::string ext;  // Extension
    };

#pragma pack(push, 1)
    struct ModelBinaryHeader {
        uint32_t magic = REAL_MAGIC; // Real Magic number
        uint32_t version = 1;
        uint32_t meshCount{};
        uint64_t uuid{};
    };

    struct MeshBinaryHeader {
        uint32_t magic = REAL_MAGIC;
        uint32_t version = 1;

        uint64_t uuid{};
        uint64_t materialUUID{};

        uint64_t vertexCount{};
        uint64_t indexCount{};
        uint64_t vertexOffset{};
        uint64_t indexOffset{};
    };
#pragma pack(pop)

    struct MeshLoadResult {
        MeshBinaryHeader header;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    struct MeshAsset {
        UUID meshUUID;
        // TODO: Need transform for per mesh!

        uint64_t vertexCount;
        uint64_t indexCount;
        uint64_t vertexOffset;
        uint64_t indexOffset;
    };

    struct RenderableData {
        const MeshAsset* mesh;
        UUID materialUUID{};
        // TODO: Need transform for per mesh!
    };

}
