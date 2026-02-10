//
// Created by pointerlost on 2/1/26.
//
#include <Graphics/Debug/DebugMeshFactory.h>
#include "Math/Math.h"

namespace Real::graphics::debug {

    DebugMesh DebugMeshFactory::CreateLine(std::vector<DebugVertex>& vertices, std::vector<uint32_t>& indices) {
        const uint32_t vertexOffset = vertices.size();

        // Two vertices for a single line
        vertices.push_back({{0, 0, 0}});
        vertices.push_back({{1, 0, 0}});

        const uint32_t indexOffset = indices.size();
        indices.push_back(vertexOffset + 0);
        indices.push_back(vertexOffset + 1);

        return { 2, indexOffset };
    }

    DebugMesh DebugMeshFactory::CreateBoxWire(std::vector<DebugVertex>& vertices, std::vector<uint32_t>& indices) {
        const uint32_t vertexOffset = vertices.size();

        const math::Vec3 p[8] = {
            {-0.5f,-0.5f,-0.5f}, {0.5f,-0.5f,-0.5f},
            {0.5f, 0.5f,-0.5f},  {-0.5f, 0.5f,-0.5f},
            {-0.5f,-0.5f, 0.5f}, {0.5f,-0.5f, 0.5f},
            {0.5f, 0.5f, 0.5f},  {-0.5f, 0.5f, 0.5f}
        };

        for (int i = 0; i < 8; ++i)
            vertices.push_back({p[i]});

        const uint32_t indexOffset = indices.size(); // start of new indices
        const uint32_t edgeIndices[24] = {
            0,1, 1,2, 2,3, 3,0,
            4,5, 5,6, 6,7, 7,4,
            0,4, 1,5, 2,6, 3,7
        };

        for (int i = 0; i < 24; ++i)
            indices.push_back(edgeIndices[i] + vertexOffset);

        return { 24, indexOffset }; // indexCount, indexOffset
    }

    DebugMesh DebugMeshFactory::CreateSphereWire(
        std::vector<DebugVertex> &vertices, std::vector<uint32_t>& indices, uint32_t slices)
    {
        const uint32_t vertexOffset = vertices.size();
        const uint32_t indexOffset = indices.size();

        // XY circle
        for (uint32_t i = 0; i < slices; ++i) {
            float a0 = (i / float(slices)) * 2.0f * math::PI;
            float a1 = ((i+1) / float(slices)) * 2.0f * math::PI;
            vertices.push_back({{math::cos(a0), math::sin(a0), 0}});
            vertices.push_back({{math::cos(a1), math::sin(a1), 0}});
            indices.push_back(vertexOffset + i*2);
            indices.push_back(vertexOffset + i*2 + 1);
        }

        // XZ circle
        uint32_t xzOffset = vertices.size();
        for (uint32_t i = 0; i < slices; ++i) {
            float a0 = (i / float(slices)) * 2.0f * math::PI;
            float a1 = ((i+1) / float(slices)) * 2.0f * math::PI;
            vertices.push_back({{math::cos(a0), 0, math::sin(a0)}});
            vertices.push_back({{math::cos(a1), 0, math::sin(a1)}});
            indices.push_back(xzOffset + i*2);
            indices.push_back(xzOffset + i*2 + 1);
        }

        // YZ circle
        uint32_t yzOffset = vertices.size();
        for (uint32_t i = 0; i < slices; ++i) {
            float a0 = (i / float(slices)) * 2.0f * math::PI;
            float a1 = ((i+1) / float(slices)) * 2.0f * math::PI;
            vertices.push_back({{0, math::cos(a0), math::sin(a0)}});
            vertices.push_back({{0, math::cos(a1), math::sin(a1)}});
            indices.push_back(yzOffset + i*2);
            indices.push_back(yzOffset + i*2 + 1);
        }

        return { static_cast<uint32_t>(indices.size() - indexOffset), indexOffset };
    }
}
