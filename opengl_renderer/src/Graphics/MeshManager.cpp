//
// Created by pointerlost on 10/4/25.
//
#include "Graphics/MeshManager.h"
#include <glad/glad.h>
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "Graphics/MeshFactory.h"

namespace Real {

    const Graphics::MeshInfo &MeshManager::GetMeshData(const std::string &name) const {
        if (!m_MeshInfos.contains(name)) {
            Warn(ConcatStr("Mesh doesn't exists! error from file: ", __FILE__));
        }
        return m_MeshInfos.at(name);
    }

    void MeshManager::CreateSingleMesh(std::vector<Graphics::Vertex> vertices, std::vector<uint32_t> indices,
            const std::string& name)
    {
        Graphics::MeshInfo info{};
        info.m_VertexCount = vertices.size();
        info.m_IndexCount  = indices.size();
        info.m_VertexOffset = m_AllVertices.size();
        info.m_IndexOffset = m_AllIndices.size();

        m_AllVertices.insert(m_AllVertices.end(), vertices.begin(), vertices.end());

        for (const auto idx : indices) {
            m_AllIndices.push_back(idx + info.m_VertexOffset);
        }

        m_MeshInfos[name] = info;
    }

    void MeshManager::InitResourcesWithBufferData() {
        auto [triFirst, triSecond] = MeshFactory::CreateTriangle();
        CreateSingleMesh(triFirst, triSecond, "triangle");
        auto [cubeFirst, cubeSecond] = MeshFactory::CreateCube();
        CreateSingleMesh(cubeFirst, cubeSecond, "cube");

        glCreateBuffers(1, &m_VBO);
        glNamedBufferData(m_VBO, m_AllVertices.size() * sizeof(Graphics::Vertex), m_AllVertices.data(), GL_STATIC_DRAW);

        glCreateBuffers(1, &m_EBO);
        glNamedBufferData(m_EBO, m_AllIndices.size() * sizeof(uint32_t), m_AllIndices.data(), GL_STATIC_DRAW);

        // Create and bind global vao
        glCreateVertexArrays(1, &m_UniversalVAO);

        // Bind VBO to Current VAO
        glVertexArrayVertexBuffer(m_UniversalVAO, 0, m_VBO, 0, sizeof(Graphics::Vertex));

        // Bind EBO to VAO
        glVertexArrayElementBuffer(m_UniversalVAO, m_EBO);

        // Position attribute
        glEnableVertexArrayAttrib(m_UniversalVAO, 0);
        glVertexArrayAttribFormat(m_UniversalVAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Graphics::Vertex, m_Position));
        glVertexArrayAttribBinding(m_UniversalVAO, 0, 0);

        // Normal attribute
        glEnableVertexArrayAttrib(m_UniversalVAO, 1);
        glVertexArrayAttribFormat(m_UniversalVAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Graphics::Vertex, m_Normal));
        glVertexArrayAttribBinding(m_UniversalVAO, 1, 0);

        // UV attribute
        glEnableVertexArrayAttrib(m_UniversalVAO, 2);
        glVertexArrayAttribFormat(m_UniversalVAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Graphics::Vertex, m_UV));
        glVertexArrayAttribBinding(m_UniversalVAO, 2, 0);

        Info(GetMeshCount());
    }

    void MeshManager::InitResourcesPersistentMapping() {
        auto [fstTri, secTri] = MeshFactory::CreateTriangle();
        CreateSingleMesh(fstTri, secTri, "triangle");
        auto [fstCube, secCube] = MeshFactory::CreateCube();
        CreateSingleMesh(fstCube, secCube, "cube");

        // Create universal VAO
        glCreateVertexArrays(1, &m_UniversalVAO);

        // Create persistent map buffer for VBO
        glCreateBuffers(1, &m_PBufferV.buffer);
        glNamedBufferStorage(m_PBufferV.buffer, m_AllVertices.size() * sizeof(Graphics::Vertex), m_AllVertices.data(), m_PBufferV.flags);
        m_PBufferV.ptr = glMapNamedBufferRange(m_PBufferV.buffer, 0, m_AllVertices.size() * sizeof(Graphics::Vertex), m_PBufferV.flags);
        memcpy(m_PBufferV.ptr, m_AllVertices.data(), m_AllVertices.size() * sizeof(Graphics::Vertex));

        // Bind VAO to (VBO) current persistent map buffer
        glVertexArrayVertexBuffer(m_UniversalVAO, 0, m_PBufferV.buffer, 0, sizeof(Graphics::Vertex));

        // Same thing for EBO
        glCreateBuffers(1, &m_PBufferE.buffer);
        glNamedBufferStorage(m_PBufferE.buffer, m_AllIndices.size() * sizeof(uint32_t), m_AllIndices.data(), m_PBufferE.flags);
        m_PBufferE.ptr = glMapNamedBufferRange(m_PBufferE.buffer, 0, m_AllIndices.size() * sizeof(uint32_t), m_PBufferE.flags);
        memcpy(m_PBufferE.ptr, m_AllIndices.data(), m_AllIndices.size() * sizeof(uint32_t));

        // Bind VAO to (EBO) current persistent map buffer
        glVertexArrayElementBuffer(m_UniversalVAO, m_PBufferE.buffer);

        // Load Attributes to current opengl state
        // Position Attribute
        glEnableVertexArrayAttrib(m_UniversalVAO, 0);
        glVertexArrayAttribFormat(m_UniversalVAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Graphics::Vertex, m_Position));
        glVertexArrayAttribBinding(m_UniversalVAO, 0, 0);

        // Normal Attribute
        glEnableVertexArrayAttrib(m_UniversalVAO, 1);
        glVertexArrayAttribFormat(m_UniversalVAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Graphics::Vertex, m_Normal));
        glVertexArrayAttribBinding(m_UniversalVAO, 1, 0);

        // UV Attribute
        glEnableVertexArrayAttrib(m_UniversalVAO, 2);
        glVertexArrayAttribFormat(m_UniversalVAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Graphics::Vertex, m_UV));
        glVertexArrayAttribBinding(m_UniversalVAO, 2, 0);

        Info(GetMeshCount());
    }
}
