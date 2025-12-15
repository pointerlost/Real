//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <iostream>
#include <glm/ext.hpp>
#include <Core/UUID.h>

namespace Real::Graphics {

    struct Vertex {
        glm::vec3 m_Position;
        glm::vec3 m_Normal;
        glm::vec2 m_UV;
    };

    struct MeshInfo {
        uint64_t m_VertexCount;
        uint64_t m_IndexCount;
        uint64_t m_VertexOffset;
        uint64_t m_IndexOffset;
        UUID m_MaterialUUID;
    };
}