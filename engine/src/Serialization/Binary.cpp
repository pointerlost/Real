//
// Created by pointerlost on 12/15/25.
//
#include <Serialization/Binary.h>
#include <fstream>
#include "Common/RealTypes.h"
#include "Core/AssetManager.h"
#include "Core/Logger.h"
#include "Core/Utils.h"

namespace Real::serialization::binary {

    void WriteModel(const std::string &path, ModelBinaryHeader binaryHeader,
        const std::vector<UUID>& meshUUIDs, const std::vector<UUID>& materialUUIDs)
    {
        std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!file) {
            Warn("[Write] Model binary file can't opening: " + path);
            return;
        }

        // Update the mesh count to ensure the size is correct
        binaryHeader.m_MeshCount = static_cast<uint32_t>(meshUUIDs.size());

        // Write entire header once
        file.write(reinterpret_cast<const char*>(&binaryHeader), sizeof(binaryHeader));

        std::vector<uint64_t> raw_meshUUIDs;
        raw_meshUUIDs.reserve(meshUUIDs.size());
        for (const auto& uuid : meshUUIDs) {
            raw_meshUUIDs.push_back(static_cast<uint64_t>(uuid));
        }
        // Bulk upload Mesh UUIDs
        file.write(reinterpret_cast<const char*>(raw_meshUUIDs.data()), raw_meshUUIDs.size() * sizeof(uint64_t));

        std::vector<uint64_t> raw_matUUIDs;
        raw_matUUIDs.reserve(materialUUIDs.size());
        for (const auto& uuid : materialUUIDs) {
            raw_matUUIDs.push_back(static_cast<uint64_t>(uuid));
        }
        // Bulk upload Mesh UUIDs
        file.write(reinterpret_cast<const char*>(raw_matUUIDs.data()), raw_matUUIDs.size() * sizeof(uint64_t));

        if (!file) {
            Warn("[WriteModel] Failed to write data!");
            return;
        }

        file.close();
    }

    std::tuple<ModelBinaryHeader, std::vector<UUID>, std::vector<UUID>> LoadModel(const std::string &path)
    {
        std::vector<Vertex> vertices;
        std::vector<uint64_t> indices;

        std::ifstream file(path, std::ios::binary | std::ios::in);
        if (!file) {
            Warn("[Load] Model binary file can't opening: " + path);
            return{};
        }

        ModelBinaryHeader header;

        // Read entire header
        file.read(reinterpret_cast<char*>(&header), sizeof(header));

        // Validate REAL magic numbers
        if (header.m_Magic != MakeFourCC('R', 'E', 'A', 'L')) { // Little-endian
            Warn("Real Magic number mismatch!");
            return{};
        }

        std::vector<uint64_t> raw_MeshUUIDs(header.m_MeshCount);
        std::vector<uint64_t> raw_MatUUIDs(header.m_MeshCount);

        std::vector<UUID> meshUUIDs;
        meshUUIDs.reserve(header.m_MeshCount);

        // per-mesh material so reserve with mesh count
        std::vector<UUID> materialUUIDs;
        materialUUIDs.reserve(header.m_MeshCount);

        if (header.m_MeshCount > 0) {
            file.read(reinterpret_cast<char*>(raw_MeshUUIDs.data()), header.m_MeshCount * sizeof(uint64_t));
            file.read(reinterpret_cast<char*>(raw_MatUUIDs.data()),  header.m_MeshCount * sizeof(uint64_t));
        }

        for (uint64_t raw_id : raw_MeshUUIDs) {
            meshUUIDs.emplace_back(raw_id);
        }

        for (uint64_t raw_id : raw_MatUUIDs) {
            materialUUIDs.emplace_back(raw_id);
        }

        if (!file) {
            Warn("[LoadModel] Failed to read data!");
            return{};
        }
        if (raw_MeshUUIDs.empty()) {
            Warn("There is no mesh inside model path: " + path);
        }

        return std::make_tuple(header, meshUUIDs, materialUUIDs);
    }

    void WriteMesh(const std::string &path, const MeshBinaryHeader &binaryHeader,
        const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    {
        std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!file) {
            Warn("[Write] Mesh binary file can't opening: " + path);
            return;
        }

        file.write(reinterpret_cast<const char*>(&binaryHeader), sizeof(binaryHeader));

        if (!vertices.empty()) {
            file.write(reinterpret_cast<const char*>(vertices.data()), vertices.size() * sizeof(Vertex));
        } else {
            Warn("[WriteMesh] Vertices are empty!");
        }

        if (!indices.empty()) {
            file.write(reinterpret_cast<const char*>(indices.data()), indices.size() * sizeof(uint32_t));
        } else {
            Warn("[WriteMesh] Indices are empty!");
        }

        if (!file) {
            Warn("[WriteMesh] Failed to write data!");
        }
    }

    MeshLoadResult LoadMesh(const std::string &path) {
        std::ifstream file(path, std::ios::binary | std::ios::in);
        if (!file) {
            Warn("[Load] Mesh binary file can't opening: " + path);
            return {};
        }

        MeshLoadResult result{};
        file.read(reinterpret_cast<char*>(&result.header), sizeof(MeshBinaryHeader));

        // Validate REAL magic numbers
        if (result.header.m_Magic != MakeFourCC('R', 'E', 'A', 'L')) {
            Warn("Real Magic number mismatch!");
            return {};
        }

        if (result.header.m_VertexCount > 0) {
            result.vertices.resize(result.header.m_VertexCount);
            file.read(reinterpret_cast<char*>(result.vertices.data()),
                        result.header.m_VertexCount * sizeof(Vertex)
            );
        }

        if (result.header.m_IndexCount > 0) {
            result.indices.resize(result.header.m_IndexCount);
            file.read(reinterpret_cast<char*>(result.indices.data()),
                        result.header.m_IndexCount * sizeof(uint32_t)
            );
        }

        if (!file) {
            Warn("[LoadMesh] Failed to read data!");
            return {};
        }

        return result;
    }

}
