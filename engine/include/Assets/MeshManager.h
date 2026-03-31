//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <span>
#include <unordered_map>
#include "AssetTypes.h"
#include "Graphics/RenderTypes.h"
#include "Core/IResourceManager.h"

namespace Real::graphics {
    enum class PrimitiveType;
}

namespace Real::assets {
    using namespace graphics;

    class MeshManager {
    public:
        void InitResources();

        const MeshAsset& CreateSingleMesh(Vector<Vertex> vertices,
            const Vector<u32>& indices, const UUID& meshUUID
        );

        std::span<const Vertex> ViewVertices(const UUID& uuid) const;
        std::span<const u32> ViewIndices(const UUID& uuid) const;

        const std::unordered_map<UUID, MeshAsset>& GetAllMeshes() { return m_MeshAssets; }
        [[nodiscard]]    const MeshAsset *GetMeshData         (const UUID& uuid) const;
        [[maybe_unused]] const MeshAsset &GetPrimitiveMeshData(const PrimitiveType& type);
        [[maybe_unused]] const UUID      &GetPrimitiveUUID    (const PrimitiveType& type);

        [[nodiscard]] u32 GetUniversalVAO () const { return m_UniversalVAO.value; }
        void              BindUniversalVAO() const;
        void              UnbindCurrVAO   () const;

        [[nodiscard]] size_t GetVerticesCount() const;
        [[nodiscard]] size_t GetIndicesCount()  const;

    private:
        void LoadPrimitiveTypes();

    private:
        std::unordered_map<UUID, MeshAsset>               m_MeshAssets;
        std::unordered_map<PrimitiveType, UUID> m_PrimitiveTypesUUIDs;

        Vector<Vertex> m_AllVertices;
        Vector<u32>              m_AllIndices;

        BufferHandle m_UniversalVAO{}, m_VBO{}, m_EBO{};
    };

    class MeshData3D final : public MeshManager {
        [[nodiscard]] u32 GetIndexCount (const UUID& uuid) const;
        [[nodiscard]] u32 GetIndexOffSet(const UUID& uuid) const;

        void AddMesh3DToMeshData(Vector<Vertex> v, const Vector<u32>& i, const UUID& meshUUID);
    };
}
