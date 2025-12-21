//
// Created by pointerlost on 10/4/25.
//
#include "Graphics/MeshManager.h"
#include <glad/glad.h>
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "Graphics/MeshFactory.h"
#include <span>

namespace Real {

    const Graphics::MeshInfo &MeshData::GetMeshData(const UUID& uuid) const {
        if (!m_MeshInfos.contains(uuid)) {
            Warn(ConcatStr("Mesh doesn't exists! error from file: ", __FILE__));
        }
        return m_MeshInfos.at(uuid);
    }

    UUID MeshData::CreateSingleMesh(std::vector<Graphics::Vertex> vertices, std::vector<uint64_t> indices,
        const UUID& matUUID, const UUID& meshUUID)
    {
        if (m_MeshInfos.contains(meshUUID)) return meshUUID; // Skip if mesh already exists
        Graphics::MeshInfo info{};
        info.m_MeshUUID     = meshUUID;
        info.m_MaterialUUID = matUUID;

        info.m_VertexCount  = vertices.size();
        info.m_IndexCount   = indices.size();
        info.m_VertexOffset = m_AllVertices.size();
        info.m_IndexOffset  = m_AllIndices.size();

        m_AllVertices.insert(m_AllVertices.end(), vertices.begin(), vertices.end());

        for (const auto idx : indices) {
            m_AllIndices.push_back(idx + info.m_VertexOffset);
        }

        m_MeshInfos[meshUUID] = info;
        return meshUUID;
    }

    std::span<const Graphics::Vertex> MeshData::ViewVertices(const UUID& uuid) const {
        if (!m_MeshInfos.contains(uuid)) return {};

        const auto& info = m_MeshInfos.at(uuid);
        return {
            m_AllVertices.data() + info.m_VertexOffset,
            info.m_VertexCount
        };
    }

    std::span<const uint64_t> MeshData::ViewIndices(const UUID& uuid) const {
        if (!m_MeshInfos.contains(uuid)) return {};

        const auto& info = m_MeshInfos.at(uuid);
        return {
            m_AllIndices.data() + info.m_IndexOffset,
            info.m_IndexCount
        };
    }

    const Graphics::MeshInfo& MeshData::GetPrimitiveMeshData(const std::string &name) {
        if (m_PrimitiveTypesUUIDs.contains(name)) {
            Warn("There is no primitive type with this name: " + name);
            return m_MeshInfos[m_PrimitiveTypesUUIDs["triangle"]];
        }
        return m_MeshInfos[m_PrimitiveTypesUUIDs[name]];
    }

    const UUID& MeshData::GetPrimitiveUUID(const std::string &name) {
        return m_PrimitiveTypesUUIDs[name];
    }

    void MeshData3D::AddMesh3DToMeshData(std::vector<Graphics::Vertex> v, std::vector<uint64_t> i, const UUID& uuid) {
        CreateSingleMesh(std::move(v), std::move(i), uuid);
    }

    void MeshData::InitResources() {
        // TODO: Load primitive types from another location like ResourceLoader or something..
        auto [triFirst, triSecond] = MeshFactory::CreateTriangle();
        m_PrimitiveTypesUUIDs["triangle"] = UUID();
        CreateSingleMesh(triFirst, triSecond, m_PrimitiveTypesUUIDs["triangle"]);

        m_PrimitiveTypesUUIDs["cube"] = UUID();
        auto [cubeFirst, cubeSecond] = MeshFactory::CreateCube();
        CreateSingleMesh(cubeFirst, cubeSecond, m_PrimitiveTypesUUIDs["cube"]);

        glCreateBuffers(1, &m_VBO);
        glNamedBufferData(m_VBO, m_AllVertices.size() * sizeof(Graphics::Vertex), m_AllVertices.data(), GL_STATIC_DRAW);

        glCreateBuffers(1, &m_EBO);
        glNamedBufferData(m_EBO, m_AllIndices.size() * sizeof(uint64_t), m_AllIndices.data(), GL_STATIC_DRAW);

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
    }

}
