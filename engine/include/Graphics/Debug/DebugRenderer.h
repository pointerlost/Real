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

namespace Real::math {
    struct Mat4;
    struct Vec3;
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
        void DrawCapsule(const math::Vec3& a, const math::Vec3& b, float radius, const math::Vec4& color);

    private:
        std::vector<DebugVertex> m_StaticVertices;
        std::vector<DebugInstance> m_Instances;
        std::vector<DrawElementsIndirectCommand> m_DebugIndirectCommands;

        std::vector<DebugVertex> m_DebugVertices;
        std::vector<uint32_t> m_DebugIndices;

        DebugMesh m_LineMesh;
        DebugMesh m_BoxMesh;
        DebugMesh m_SphereMesh;
        DebugMesh m_CylinderMesh;

        GLuint m_VAO = 0;
        GLuint m_VBO = 0;
        GLuint m_EBO = 0;
        int m_BaseInstance = 0;
        opengl::Buffer m_DebugInstanceBuffer;
        opengl::Buffer m_DebugIndirectBuffer;
        Shader* m_DebugShader = nullptr;

    private:
        void PrepareDrawCommands();
        DebugMesh PickTypeBasedDebugMesh(DebugShapes shapeType);
    };

}
