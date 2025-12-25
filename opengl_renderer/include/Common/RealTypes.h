//
// Created by pointerlost on 12/6/25.
//
#pragma once
#include <glm/ext.hpp>
#include "Macros.h"
#include <vector>
#include <Core/UUID.h>

namespace Real {

    struct Vertex {
        glm::vec3 m_Position;
        glm::vec3 m_Normal;
        glm::vec2 m_UV;
    };

    struct TextureData {
        void* m_Data = nullptr;
        int m_ChannelCount = 0;
        int m_Width = 0;
        int m_Height = 0;
        int m_DataSize = 0;
        int m_Format = {};
        int m_InternalFormat = {};
    };

    struct FileInfo {
        std::string name; // Full name
        std::string stem; // Name without extension
        std::string path; // Full path
        std::string ext;  // Extension
    };

#pragma pack(push, 1)
    struct ModelBinaryHeader {
        uint32_t m_Magic = REAL_MAGIC; // Real Magic number
        uint32_t m_Version = 1;
        uint32_t m_MeshCount{};
        uint64_t m_UUID{};
    };

    struct MeshBinaryHeader {
        uint32_t m_Magic = REAL_MAGIC;
        uint32_t m_Version = 1;

        uint64_t m_UUID{};
        uint64_t m_MaterialUUID{};

        uint64_t m_VertexCount{};
        uint64_t m_IndexCount{};
        uint64_t m_VertexOffset{};
        uint64_t m_IndexOffset{};
    };
#pragma pack(pop)

    struct MeshLoadResult {
        MeshBinaryHeader header;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    struct MeshAsset {
        UUID m_MeshUUID;
        // TODO: Need transform for per mesh!

        uint64_t m_VertexCount;
        uint64_t m_IndexCount;
        uint64_t m_VertexOffset;
        uint64_t m_IndexOffset;
    };

    struct RenderableData {
        const MeshAsset* m_Mesh;
        UUID m_MaterialUUID{};
        // TODO: Need transform for per mesh!
    };

}
