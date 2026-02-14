//
// Created by pointerlost on 2/1/26.
//
#include <Graphics/Debug/DebugMeshFactory.h>
#include "Math/Math.h"

namespace Real::graphics::debug {

    DebugMesh DebugMeshFactory::CreateLine(Vector<DebugVertex>& vertices, Vector<u32>& indices) {
        const u32 vertexOffset = vertices.size();

        // Two vertices for a single line
        vertices.push_back({{0, 0, 0}});
        vertices.push_back({{1, 0, 0}});

        const u32 indexOffset = indices.size();
        indices.push_back(vertexOffset + 0);
        indices.push_back(vertexOffset + 1);

        return { 2, indexOffset };
    }

    DebugMesh DebugMeshFactory::CreateBoxWire(Vector<DebugVertex>& vertices, Vector<u32>& indices) {
        const u32 vertexOffset = vertices.size();

        const math::Vec3 p[8] = {
            {-0.5f,-0.5f,-0.5f}, {0.5f,-0.5f,-0.5f},
            {0.5f, 0.5f,-0.5f},  {-0.5f, 0.5f,-0.5f},
            {-0.5f,-0.5f, 0.5f}, {0.5f,-0.5f, 0.5f},
            {0.5f, 0.5f, 0.5f},  {-0.5f, 0.5f, 0.5f}
        };

        for (int i = 0; i < 8; ++i)
            vertices.push_back({p[i]});

        const u32 indexOffset = indices.size(); // start of new indices
        const u32 edgeIndices[24] = {
            0,1, 1,2, 2,3, 3,0,
            4,5, 5,6, 6,7, 7,4,
            0,4, 1,5, 2,6, 3,7
        };

        for (int i = 0; i < 24; ++i)
            indices.push_back(edgeIndices[i] + vertexOffset);

        return { 24, indexOffset }; // indexCount, indexOffset
    }

    DebugMesh DebugMeshFactory::CreateSphereWire(
        Vector<DebugVertex> &vertices, Vector<u32>& indices, u32 slices)
    {
        const u32 vertexOffset = vertices.size();
        const u32 indexOffset = indices.size();

        // XY circle
        for (u32 i = 0; i < slices; ++i) {
            f32 a0 = (i / f32(slices)) * 2.0f * math::PI;
            f32 a1 = ((i+1) / f32(slices)) * 2.0f * math::PI;
            vertices.push_back({{math::cos(a0), math::sin(a0), 0}});
            vertices.push_back({{math::cos(a1), math::sin(a1), 0}});
            indices.push_back(vertexOffset + i*2);
            indices.push_back(vertexOffset + i*2 + 1);
        }

        // XZ circle
        u32 xzOffset = vertices.size();
        for (u32 i = 0; i < slices; ++i) {
            f32 a0 = (i / f32(slices)) * 2.0f * math::PI;
            f32 a1 = ((i+1) / f32(slices)) * 2.0f * math::PI;
            vertices.push_back({{math::cos(a0), 0, math::sin(a0)}});
            vertices.push_back({{math::cos(a1), 0, math::sin(a1)}});
            indices.push_back(xzOffset + i*2);
            indices.push_back(xzOffset + i*2 + 1);
        }

        // YZ circle
        u32 yzOffset = vertices.size();
        for (u32 i = 0; i < slices; ++i) {
            f32 a0 = (i / f32(slices)) * 2.0f * math::PI;
            f32 a1 = ((i+1) / f32(slices)) * 2.0f * math::PI;
            vertices.push_back({{0, math::cos(a0), math::sin(a0)}});
            vertices.push_back({{0, math::cos(a1), math::sin(a1)}});
            indices.push_back(yzOffset + i*2);
            indices.push_back(yzOffset + i*2 + 1);
        }

        return { static_cast<u32>(indices.size() - indexOffset), indexOffset };
    }
}
