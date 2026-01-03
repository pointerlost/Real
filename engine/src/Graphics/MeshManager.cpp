//
// Created by pointerlost on 10/4/25.
//
#include "Graphics/MeshManager.h"
#include <glad/glad.h>
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "Graphics/MeshFactory.h"
#include <span>
#include "Core/AssetManager.h"
#include "Graphics/Material.h"

namespace Real {

    const MeshAsset* MeshData::GetMeshData(const UUID& uuid) const {
        const auto it = m_MeshAssets.find(uuid);
        if (it == m_MeshAssets.end()) {
            Warn("Mesh doesn't exist! UUID: " + std::to_string(uuid));
            return nullptr;
        }
        return &it->second;
    }

    const MeshAsset& MeshData::CreateSingleMesh(std::vector<Vertex> vertices,
        const std::vector<uint32_t>& indices, const UUID& meshUUID)
    {
        if (m_MeshAssets.contains(meshUUID))
            return m_MeshAssets[meshUUID]; // Skip if mesh already exists
        MeshAsset info{};
        info.m_MeshUUID     = meshUUID;

        info.m_VertexCount  = vertices.size();
        info.m_IndexCount   = indices.size();
        info.m_VertexOffset = m_AllVertices.size();
        info.m_IndexOffset  = m_AllIndices.size();

        m_AllVertices.insert(m_AllVertices.end(), vertices.begin(), vertices.end());

        for (const auto idx : indices) {
            m_AllIndices.push_back(idx + info.m_VertexOffset);
        }

        return m_MeshAssets[meshUUID] = info;
    }

    std::span<const Vertex> MeshData::ViewVertices(const UUID& uuid) const {
        if (!m_MeshAssets.contains(uuid)) return {};

        const auto& info = m_MeshAssets.at(uuid);
        return {
            m_AllVertices.data() + info.m_VertexOffset,
            info.m_VertexCount
        };
    }

    std::span<const uint32_t> MeshData::ViewIndices(const UUID& uuid) const {
        if (!m_MeshAssets.contains(uuid)) return {};

        const auto& info = m_MeshAssets.at(uuid);
        return {
            m_AllIndices.data() + info.m_IndexOffset,
            info.m_IndexCount
        };
    }

    const MeshAsset& MeshData::GetPrimitiveMeshData(const std::string &name) {
        if (m_PrimitiveTypesUUIDs.contains(name)) {
            Warn("There is no primitive type with this name: " + name);
            return m_MeshAssets[m_PrimitiveTypesUUIDs["triangle"]];
        }
        return m_MeshAssets[m_PrimitiveTypesUUIDs[name]];
    }

    const UUID& MeshData::GetPrimitiveUUID(const std::string &name) {
        return m_PrimitiveTypesUUIDs[name];
    }

    void MeshData::LoadPrimitiveTypes() {
        auto [triFirst, triSecond] = MeshFactory::CreateTriangle();
        m_PrimitiveTypesUUIDs["triangle"] = UUID();
        CreateSingleMesh(triFirst, triSecond, m_PrimitiveTypesUUIDs["triangle"]);

        m_PrimitiveTypesUUIDs["cube"] = UUID();
        auto [cubeFirst, cubeSecond] = MeshFactory::CreateCube();
        CreateSingleMesh(cubeFirst, cubeSecond, m_PrimitiveTypesUUIDs["cube"]);
    }

    void MeshData3D::AddMesh3DToMeshData(std::vector<Vertex> v, std::vector<uint32_t> i, const UUID& meshUUID)
    {
        CreateSingleMesh(std::move(v), std::move(i), meshUUID);
    }

    void MeshData::InitResources() {
        glCreateBuffers(1, &m_VBO);
        glNamedBufferData(m_VBO, m_AllVertices.size() * sizeof(Vertex), m_AllVertices.data(), GL_STATIC_DRAW);

        glCreateBuffers(1, &m_EBO);
        glNamedBufferData(m_EBO, m_AllIndices.size() * sizeof(uint32_t), m_AllIndices.data(), GL_STATIC_DRAW);

        // Create and bind global vao
        glCreateVertexArrays(1, &m_UniversalVAO);

        // Bind VBO to Current VAO
        glVertexArrayVertexBuffer(m_UniversalVAO, 0, m_VBO, 0, sizeof(Vertex));

        // Bind EBO to VAO
        glVertexArrayElementBuffer(m_UniversalVAO, m_EBO);

        // Position attribute
        glEnableVertexArrayAttrib(m_UniversalVAO, 0);
        glVertexArrayAttribFormat(m_UniversalVAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, m_Position));
        glVertexArrayAttribBinding(m_UniversalVAO, 0, 0);

        // Normal attribute
        glEnableVertexArrayAttrib(m_UniversalVAO, 1);
        glVertexArrayAttribFormat(m_UniversalVAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, m_Normal));
        glVertexArrayAttribBinding(m_UniversalVAO, 1, 0);

        // UV attribute
        glEnableVertexArrayAttrib(m_UniversalVAO, 2);
        glVertexArrayAttribFormat(m_UniversalVAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, m_UV));
        glVertexArrayAttribBinding(m_UniversalVAO, 2, 0);
    }

}
