//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <span>
#include <unordered_map>
#include <vector>
#include "Common/RealTypes.h"
#include "Core/UUID.h"
#include "glad/glad.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"

namespace Real { struct OpenGLTexture; }

namespace Real {

    struct Vertex {
        math::Vec3 position;
        math::Vec3 normal;
        math::Vec2 UV;
    };

    // TODO: MeshManager vs MeshData wrong naming fix it this shit
    class MeshManager {
    public:
        void InitResources();

        const MeshAsset& CreateSingleMesh(Vector<Vertex> vertices,
            const Vector<u32>& indices, const UUID& meshUUID
        );

        std::span<const Vertex> ViewVertices(const UUID& uuid) const;
        std::span<const u32> ViewIndices(const UUID& uuid) const;

        const std::unordered_map<UUID, MeshAsset>& GetAllMeshes() { return m_MeshAssets; }
        [[nodiscard]] const MeshAsset* GetMeshData(const UUID& uuid) const;
        [[maybe_unused]] const MeshAsset &GetPrimitiveMeshData(const String& name);
        [[maybe_unused]] const UUID& GetPrimitiveUUID(const String& name);
        [[nodiscard]] GLuint GetUniversalVAO() const { return m_UniversalVAO; }
        void BindUniversalVAO() const;
        void UnbindCurrVAO() const;

        [[nodiscard]] size_t GetVerticesCount() const { return m_AllVertices.size(); }
        [[nodiscard]] size_t GetIndicesCount()  const { return m_AllIndices.size(); }
        void LoadPrimitiveTypes();

    private:
        std::unordered_map<UUID, MeshAsset> m_MeshAssets;
        std::unordered_map<String, UUID> m_PrimitiveTypesUUIDs;
        Vector<Vertex> m_AllVertices;
        Vector<u32> m_AllIndices;

        unsigned int m_UniversalVAO = 0, m_VBO = 0, m_EBO = 0;
    };

    class MeshData3D final : public MeshManager {
        void AddMesh3DToMeshData(Vector<Vertex> v, const Vector<u32>& i, const UUID& meshUUID);

        [[nodiscard]] u32 GetIndexCount(const UUID& uuid) const {
            return GetMeshData(uuid)->indexCount;
        }

        [[nodiscard]] u32 GetIndexOffSet(const UUID& uuid) const {
            return GetMeshData(uuid)->indexOffset;
        }
    };
}
