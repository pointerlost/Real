//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <span>
#include <unordered_map>
#include <vector>
#include <glad/glad.h>
#include "Common/RealTypes.h"
#include "Core/UUID.h"

namespace Real { struct OpenGLTexture; }

namespace Real {

    // TODO: MeshManager vs MeshData wrong naming fix it this shit
    class MeshData {
    public:
        void InitResources();

        const MeshAsset& CreateSingleMesh(std::vector<Vertex> vertices,
            std::vector<uint32_t> indices, const UUID& matUUID
        );

        std::span<const Vertex> ViewVertices(const UUID& uuid) const;
        std::span<const uint32_t> ViewIndices(const UUID& uuid) const;

        const std::unordered_map<UUID, MeshAsset>& GetAllMeshes() { return m_MeshAssets; }
        [[nodiscard]] const MeshAsset* GetMeshData(const UUID& uuid) const;
        [[maybe_unused]] const MeshAsset &GetPrimitiveMeshData(const std::string& name);
        [[maybe_unused]] const UUID& GetPrimitiveUUID(const std::string& name);
        [[nodiscard]] GLuint GetUniversalVAO() const { return m_UniversalVAO; }
        void BindUniversalVAO() const { glBindVertexArray(m_UniversalVAO); }
        void UnbindCurrVAO() const { glBindVertexArray(0); }

        [[nodiscard]] size_t GetVerticesCount() const { return m_AllVertices.size(); }
        [[nodiscard]] size_t GetIndicesCount()  const { return m_AllIndices.size(); }
        void LoadPrimitiveTypes();

    private:
        std::unordered_map<UUID, MeshAsset> m_MeshAssets;
        std::unordered_map<std::string, UUID> m_PrimitiveTypesUUIDs;
        std::vector<Vertex> m_AllVertices;
        std::vector<uint32_t> m_AllIndices;

        unsigned int m_UniversalVAO = 0, m_VBO = 0, m_EBO = 0;
    };

    class MeshData3D final : public MeshData {
        void AddMesh3DToMeshData(std::vector<Vertex> v, std::vector<uint32_t> i, const UUID& meshUUID);

        [[nodiscard]] uint32_t GetIndexCount(const UUID& uuid) const {
            return GetMeshData(uuid)->m_IndexCount;
        }

        [[nodiscard]] uint32_t GetIndexOffSet(const UUID& uuid) const {
            return GetMeshData(uuid)->m_IndexOffset;
        }
    };
}
