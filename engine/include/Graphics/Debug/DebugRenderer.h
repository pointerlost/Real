//
// Created by pointerlost on 2/1/26.
//
#pragma once
#include <vector>
#include "DebugTypes.h"
#include "Graphics/Buffer.h"
#include "Graphics/RenderCommand.h"

namespace Real {
    class Shader;
    class Camera;
}

namespace Real::graphics::debug {

    class DebugRenderer {
    public:
        void Init();
        void BeginFrame();
        void Update();
        void Render();
        void EndFrame();

        void DrawLine(const math::Vec3& a, const math::Vec3& b, const math::Vec4& color);
        void DrawBox(const math::Mat4& model, const math::Vec4& color);
        void DrawSphere(const math::Mat4& model, const math::Vec4& color);
        void DrawCylinder(const math::Mat4& model, const math::Vec4& color);
        void DrawCapsule(const math::Vec3& a, const math::Vec3& b, f32 radius, const math::Vec4& color);

    private:
        Vector<DebugVertex> m_StaticVertices;
        Vector<DebugInstance> m_Instances;
        Vector<DrawElementsIndirectCommand> m_DebugIndirectCommands;

        Vector<DebugVertex> m_DebugVertices;
        Vector<u32> m_DebugIndices;

        DebugMesh m_LineMesh;
        DebugMesh m_BoxMesh;
        DebugMesh m_SphereMesh;
        DebugMesh m_CylinderMesh;

        BufferHandle m_VAO = 0;
        BufferHandle m_VBO = 0;
        BufferHandle m_EBO = 0;

        int m_BaseInstance = 0;
        opengl::Buffer m_DebugInstanceBuffer;
        opengl::Buffer m_DebugIndirectBuffer;
        Shader* m_DebugShader = nullptr;

    private:
        void PrepareDrawCommands();
        DebugMesh PickTypeBasedDebugMesh(DebugShapes shapeType);
    };

}
