//
// Created by pointerlost on 10/4/25.
//
#include "Graphics/MeshManager.h"
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "Graphics/MeshFactory.h"
#include <span>
#include "Assets/AssetManager.h"
#include "Assets/AssetTypes.h"
#include "Graphics/Material.h"

namespace Real::assets {

    const MeshAsset* MeshManager::GetMeshData(const UUID& uuid) const {
        const auto it = m_MeshAssets.find(uuid);
        if (it == m_MeshAssets.end()) {
            Warn("Mesh doesn't exist! UUID: " + std::to_string(uuid));
            return nullptr;
        }
        return &it->second;
    }

    const MeshAsset& MeshManager::CreateSingleMesh(Vector<graphics::Vertex> vertices,
        const Vector<u32>& indices, const UUID& meshUUID)
    {
        if (m_MeshAssets.contains(meshUUID))
            return m_MeshAssets[meshUUID]; // Skip if mesh already exists
        MeshAsset info{};
        info.uuid = meshUUID;

        info.vertexCount  = vertices.size();
        info.indexCount   = indices.size();
        info.vertexOffset = m_AllVertices.size();
        info.indexOffset  = m_AllIndices.size();

        m_AllVertices.insert(m_AllVertices.end(), vertices.begin(), vertices.end());

        for (const auto idx : indices) {
            m_AllIndices.push_back(idx + info.vertexOffset);
        }

        return m_MeshAssets[meshUUID] = info;
    }

    std::span<const graphics::Vertex> MeshManager::ViewVertices(const UUID& uuid) const {
        if (!m_MeshAssets.contains(uuid)) return {};

        const auto& info = m_MeshAssets.at(uuid);
        return {
            m_AllVertices.data() + info.vertexOffset,
            info.vertexCount
        };
    }

    std::span<const u32> MeshManager::ViewIndices(const UUID& uuid) const {
        if (!m_MeshAssets.contains(uuid)) return {};

        const auto& info = m_MeshAssets.at(uuid);
        return {
            m_AllIndices.data() + info.indexOffset,
            info.indexCount
        };
    }

    const MeshAsset& MeshManager::GetPrimitiveMeshData(const String &name) {
        if (m_PrimitiveTypesUUIDs.contains(name)) {
            Warn("There is no primitive type with this name: " + name);
            return m_MeshAssets[m_PrimitiveTypesUUIDs["triangle"]];
        }
        return m_MeshAssets[m_PrimitiveTypesUUIDs[name]];
    }

    const UUID& MeshManager::GetPrimitiveUUID(const String &name) {
        return m_PrimitiveTypesUUIDs[name];
    }

    void MeshManager::BindUniversalVAO() const {
        glBindVertexArray(m_UniversalVAO);
    }

    void MeshManager::UnbindCurrVAO() const {
        glBindVertexArray(0);
    }

    void MeshManager::LoadPrimitiveTypes() {
        auto [triFirst, triSecond] = MeshFactory::CreateTriangle();
        m_PrimitiveTypesUUIDs["triangle"] = UUID();
        CreateSingleMesh(triFirst, triSecond, m_PrimitiveTypesUUIDs["triangle"]);

        m_PrimitiveTypesUUIDs["cube"] = UUID();
        auto [cubeFirst, cubeSecond] = MeshFactory::CreateCube();
        CreateSingleMesh(cubeFirst, cubeSecond, m_PrimitiveTypesUUIDs["cube"]);
    }

    u32 MeshData3D::GetIndexCount(const UUID &uuid) const {
        return GetMeshData(uuid)->indexCount;
    }

    u32 MeshData3D::GetIndexOffSet(const UUID &uuid) const {
        return GetMeshData(uuid)->indexOffset;
    }

    void MeshData3D::AddMesh3DToMeshData(Vector<graphics::Vertex> v, const Vector<u32>& i, const UUID& meshUUID)
    {
        CreateSingleMesh(std::move(v), i, meshUUID);
    }

    void MeshManager::InitResources() {
        LoadPrimitiveTypes();

        glCreateBuffers(1, &m_VBO);
        glNamedBufferData(m_VBO, m_AllVertices.size() * sizeof(Vertex), m_AllVertices.data(), GL_STATIC_DRAW);

        glCreateBuffers(1, &m_EBO);
        glNamedBufferData(m_EBO, m_AllIndices.size() * sizeof(u32), m_AllIndices.data(), GL_STATIC_DRAW);

        // Create and bind global vao
        glCreateVertexArrays(1, &m_UniversalVAO);

        // Bind VBO to Current VAO
        glVertexArrayVertexBuffer(m_UniversalVAO, 0, m_VBO, 0, sizeof(Vertex));

        // Bind EBO to VAO
        glVertexArrayElementBuffer(m_UniversalVAO, m_EBO);

        // Position attribute
        glEnableVertexArrayAttrib(m_UniversalVAO, 0);
        glVertexArrayAttribFormat(m_UniversalVAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
        glVertexArrayAttribBinding(m_UniversalVAO, 0, 0);

        // Normal attribute
        glEnableVertexArrayAttrib(m_UniversalVAO, 1);
        glVertexArrayAttribFormat(m_UniversalVAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
        glVertexArrayAttribBinding(m_UniversalVAO, 1, 0);

        // UV attribute
        glEnableVertexArrayAttrib(m_UniversalVAO, 2);
        glVertexArrayAttribFormat(m_UniversalVAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, UV));
        glVertexArrayAttribBinding(m_UniversalVAO, 2, 0);
    }

    size_t MeshManager::GetVerticesCount() const {
        return m_AllVertices.size();
    }

    size_t MeshManager::GetIndicesCount() const {
        return m_AllIndices.size();
    }
}
