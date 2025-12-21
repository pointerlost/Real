//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <iostream>
#include <span>
#include <unordered_map>
#include <vector>
#include <glad/glad.h>
#include "Mesh.h"
#include "Core/UUID.h"

namespace Real { struct OpenGLTexture; }

namespace Real {

    // TODO: MeshManager vs MeshData wrong naming fix it this shit
    class MeshData {
    public:
        void InitResources();

        UUID CreateSingleMesh(std::vector<Graphics::Vertex> vertices, std::vector<uint64_t> indices,
            const UUID& matUUID, const UUID& meshUUID = UUID()
        );

        std::span<const Graphics::Vertex> ViewVertices(const UUID& uuid) const;
        std::span<const uint64_t> ViewIndices(const UUID& uuid) const;

        const std::unordered_map<UUID, Graphics::MeshInfo>& GetAllMeshes() { return m_MeshInfos; }
        [[nodiscard]] const Graphics::MeshInfo &GetMeshData(const UUID& uuid) const;
        [[maybe_unused]] const Graphics::MeshInfo &GetPrimitiveMeshData(const std::string& name);
        [[maybe_unused]] const UUID& GetPrimitiveUUID(const std::string& name);
        [[nodiscard]] GLuint GetUniversalVAO() const { return m_UniversalVAO; }
        void BindUniversalVAO() const { glBindVertexArray(m_UniversalVAO); }
        void UnbindCurrVAO() const { glBindVertexArray(0); }

        [[nodiscard]] size_t GetVerticesCount() const { return m_AllVertices.size(); }
        [[nodiscard]] size_t GetIndicesCount()  const { return m_AllIndices.size(); }

    private:
        std::unordered_map<UUID, Graphics::MeshInfo> m_MeshInfos;
        std::unordered_map<std::string, UUID> m_PrimitiveTypesUUIDs;
        std::vector<Graphics::Vertex> m_AllVertices;
        std::vector<uint64_t> m_AllIndices;

        unsigned int m_UniversalVAO = 0, m_VBO = 0, m_EBO = 0;
    };

    class MeshData3D final : public MeshData {
        void AddMesh3DToMeshData(std::vector<Graphics::Vertex> v, std::vector<uint64_t> i, const UUID& uuid);

        [[nodiscard]] uint32_t GetIndexCount(const UUID& uuid) const {
            return GetMeshData(uuid).m_IndexCount;
        }

        [[nodiscard]] uint32_t GetIndexOffSet(const UUID& uuid) const {
            return GetMeshData(uuid).m_IndexOffset;
        }
    };
}
