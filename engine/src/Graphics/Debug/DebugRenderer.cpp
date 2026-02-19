//
// Created by pointerlost on 2/1/26.
//
#include <Graphics/Debug/DebugRenderer.h>
#include "Graphics/Debug/DebugMeshFactory.h"
#include <glad/include/glad/glad.h>
#include "Core/AssetManager.h"
#include "Core/Services.h"
#include "Graphics/Camera.h"
#include "Graphics/Shader.h"
#include "Graphics/Debug/DebugTypes.h"

namespace {
    // make it power of two
    constexpr int MAX_DEBUG_INSTANCE_COUNT = 16384;
    constexpr int DEBUG_INDIRECT_COMMANDS_BINDING_POINT = 0;
    constexpr int DEBUG_INSTANCES_BINDING_POINT = 1;
}

namespace Real::graphics::debug {

    void DebugRenderer::Init() {
        // Build static geometry
        m_LineMesh   = DebugMeshFactory::CreateLine(m_DebugVertices, m_DebugIndices);
        m_BoxMesh    = DebugMeshFactory::CreateBoxWire(m_DebugVertices, m_DebugIndices);
        m_SphereMesh = DebugMeshFactory::CreateSphereWire(m_DebugVertices, m_DebugIndices, 32);

        glCreateBuffers(1, &m_VBO);
        glNamedBufferData(m_VBO, m_DebugVertices.size() * sizeof(DebugVertex), m_DebugVertices.data(), GL_STATIC_DRAW);

        glCreateBuffers(1, &m_EBO);
        glNamedBufferData(m_EBO, m_DebugIndices.size() * sizeof(u32), m_DebugIndices.data(), GL_STATIC_DRAW);

        glCreateVertexArrays(1, &m_VAO);

        glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, sizeof(DebugVertex));

        // Bind EBO to VAO
        glVertexArrayElementBuffer(m_VAO, m_EBO);

        glEnableVertexArrayAttrib(m_VAO, 0);
        glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(m_VAO, 0, 0);

        // Instance Buffer(SSBO)
        m_DebugInstanceBuffer.Create(
            Vector<DebugInstance>{},
            sizeof(DebugInstance) * MAX_DEBUG_INSTANCE_COUNT,
            BufferType::SSBO
        );

        m_DebugIndirectBuffer.Create(
            Vector<DrawElementsIndirectCommand>{},
            sizeof(DrawElementsIndirectCommand) * MAX_DEBUG_INSTANCE_COUNT,
            BufferType::SSBO
        );

        // Get debug shader
        m_DebugShader = &Services::GetAssetManager()->GetShader("debug");
    }

    void DebugRenderer::BeginFrame() {
        m_Instances.clear();
        m_DebugIndirectCommands.clear();
        m_BaseInstance = 0;
    }

    void DebugRenderer::Update() {
        PrepareDrawCommands();

        if (!m_Instances.empty()) {
            m_DebugInstanceBuffer.UploadToGPU(
                m_Instances,
                m_Instances.size() * sizeof(DebugInstance),
                BufferType::SSBO
            );
        }
        if (!m_DebugIndirectCommands.empty()) {
            m_DebugIndirectBuffer.UploadToGPU(
                m_DebugIndirectCommands,
                m_DebugIndirectCommands.size() * sizeof(DrawElementsIndirectCommand),
                BufferType::SSBO
            );
        }
    }

    void DebugRenderer::EndFrame() {
    }

    void DebugRenderer::Render() {
        if (m_DebugIndirectCommands.empty())
            return;

        m_DebugShader->Bind();
        m_DebugIndirectBuffer.Bind(GL_SHADER_STORAGE_BUFFER, BufferType::SSBO, DEBUG_INDIRECT_COMMANDS_BINDING_POINT);
        m_DebugInstanceBuffer.Bind(GL_SHADER_STORAGE_BUFFER, BufferType::SSBO, DEBUG_INSTANCES_BINDING_POINT);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_DebugIndirectBuffer.GetHandle());
        glMultiDrawElementsIndirect(
            GL_LINES,        // primitive type
            GL_UNSIGNED_INT, // index type
            nullptr,
            static_cast<GLsizei>(m_DebugIndirectCommands.size()), // draw count
            0
        );
    }

    void DebugRenderer::DrawLine(const math::Vec3 &a, const math::Vec3 &b, const math::Vec4 &color) {
        const math::Vec3 dir = b - a;
        f32 len = math::Vec3::Length(dir);

        const math::Mat4 model =
            math::Mat4::Translate(a) *
            math::Mat4::FromDirection(dir) *
            math::Mat4::Scale({len, 1, 1});

        m_Instances.push_back({
            model,
            color,
            DebugShapes::Line
        });
    }

    void DebugRenderer::DrawBox(const math::Mat4 &model, const math::Vec4 &color) {
        m_Instances.push_back({
            model,
            color,
            DebugShapes::Box
        });
    }

    void DebugRenderer::DrawSphere(const math::Mat4 &model, const math::Vec4 &color) {
        m_Instances.push_back({
            model,
            color,
            DebugShapes::Sphere
        });
    }

    void DebugRenderer::DrawCylinder(const math::Mat4 &model, const math::Vec4 &color) {
        m_Instances.push_back({
            model,
            color,
            DebugShapes::Cylinder
        });
    }

    // a = First endpoint (center of one hemispherical end)
    // b = Second endpoint (center of the other hemispherical end)
    void DebugRenderer::DrawCapsule(const math::Vec3 &a, const math::Vec3 &b, f32 radius, const math::Vec4 &color) {
        // Calculates direction and length between endpoints
        const math::Vec3 dir = b - a; // Vector from a to b
        const f32 len = math::Vec3::Length(dir); // Distance between endpoints

        if (len <= 0.0001f)
            return;

        // Draws the cylindrical middle part
        { // Places cylinder at midpoint between a and b, oriented along the axis
            const math::Vec3 axis = dir / len;
            const math::Mat4 model =
                math::Mat4::Translate(a + axis * (len * 0.5f)) *
                math::Mat4::FromDirection(axis) *
                math::Mat4::Scale({ radius, len, radius });

            DrawCylinder(model, color);
        }

        // Sphere at A
        {
            // Draws hemispherical end at point a
            const math::Mat4 model = math::Mat4::Translate(a) * math::Mat4::Scale({ radius, radius, radius });
            DrawSphere(model, color);
        }
        // Sphere at B
        {
            // Draws hemispherical end at point b
            const math::Mat4 model = math::Mat4::Translate(b) * math::Mat4::Scale({ radius, radius, radius });
            DrawSphere(model, color);
        }
    }

    DebugMesh DebugRenderer::PickTypeBasedDebugMesh(DebugShapes shapeType) {
        switch (shapeType) {
            case DebugShapes::Line:     return m_LineMesh;
            case DebugShapes::Box:      return m_BoxMesh;
            case DebugShapes::Cylinder: return m_CylinderMesh;
            case DebugShapes::Sphere:   return m_SphereMesh;
            default: return m_BoxMesh;
        }
    }

    void DebugRenderer::PrepareDrawCommands() {
        for (const auto shape : AllDebugShapes) {
            // Count how many instances of this shape exist
            int count = 0;
            for (const auto& inst : m_Instances) {
                if (inst.shape == shape)  // store shape per instance
                    ++count;
            }

            if (count == 0) continue;

            const auto debugMesh = PickTypeBasedDebugMesh(shape);

            DrawElementsIndirectCommand cmd{};
            cmd.count         = debugMesh.indexCount;
            cmd.instanceCount = count;
            cmd.firstIndex    = debugMesh.indexOffset;
            cmd.baseVertex    = 0;
            cmd.baseInstance  = m_BaseInstance;

            m_BaseInstance += count;  // next batch

            m_DebugIndirectCommands.push_back(cmd);
        }
    }
}
