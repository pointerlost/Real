//
// Created by pointerlost on 12/15/25.
//
#include <Serialization/Binary.h>
#include <fstream>
#include "Common/RealTypes.h"
#include "Core/Services.h"
#include "Graphics/Mesh.h"
#include "Core/AssetManager.h"
#include "Core/file_manager.h"
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "Graphics/MeshManager.h"
#include "Graphics/Model.h"

namespace Real::serialization::binary {

    void WriteModel(const std::string &path, ModelBinaryHeader binaryHeader, const std::vector<UUID>& meshUUIDs) {
        std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!file) {
            Warn("[Write] Model binary file can't opening: " + path);
            return;
        }

        // Update the mesh count to be sure
        binaryHeader.m_MeshCount = static_cast<uint32_t>(meshUUIDs.size());

        // Write entire header once
        file.write(reinterpret_cast<const char*>(&binaryHeader), sizeof(binaryHeader));

        std::vector<uint64_t> raw_ids;
        raw_ids.reserve(meshUUIDs.size());
        for (const auto& uuid : meshUUIDs) {
            raw_ids.push_back(static_cast<uint64_t>(uuid));
        }
        // Bulk upload Mesh UUIDs
        file.write(reinterpret_cast<const char*>(raw_ids.data()), raw_ids.size() * sizeof(uint64_t));

        if (!file) {
            Warn("Failed to write data!");
            return;
        }

        file.close();
    }

    UUID LoadModel(const std::string &path) {
        const auto& am = Services::GetAssetManager();
        std::vector<Graphics::Vertex> vertices;
        std::vector<uint64_t> indices;

        std::ifstream file(path, std::ios::binary | std::ios::in);
        if (!file) {
            Warn("[Load] Model binary file can't opening: " + path);
            return{};
        }

        ModelBinaryHeader header;

        // Read ENTIRE header
        file.read(reinterpret_cast<char*>(&header), sizeof(header));

        // Validate REAL magic numbers
        if (header.m_Magic != MakeFourCC('R', 'E', 'A', 'L')) { // Little-endian
            Warn("Real Magic number mismatch!");
            return{};
        }

        const auto& fi = fs::CreateFileInfoFromPath(path);
        const auto& m  = CreateRef<Model>(header, fi);
        am->SaveModelCPU(m);

        if (header.m_MeshCount > 0) {
            std::vector<uint64_t> raw_ids(header.m_MeshCount);
            file.read(reinterpret_cast<char*>(raw_ids.data()), header.m_MeshCount * sizeof(uint64_t));

            m->m_MeshUUIDs.reserve(header.m_MeshCount);
            for (uint64_t raw_id : raw_ids) {
                m->m_MeshUUIDs.emplace_back(raw_id);
            }
        }

        if (!file) {
            Warn("Failed to read data!");
            return{};
        }

        // Close Binary file
        file.close();

        return m->m_UUID;
    }

    void WriteMesh(const std::string &path, const MeshBinaryHeader &binaryHeader,
        const std::vector<Graphics::Vertex>& vertices, const std::vector<uint64_t>& indices)
    {
        std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!file) {
            Warn("[Write] Mesh binary file can't opening: " + path);
            return;
        }

        file.write(reinterpret_cast<const char*>(&binaryHeader), sizeof(binaryHeader));

        if (!vertices.empty()) {
            file.write(reinterpret_cast<const char*>(vertices.data()), vertices.size() * sizeof(Graphics::Vertex));
        } else {
            Warn("[WriteMesh] Vertices are empty!");
        }

        if (!indices.empty()) {
            file.write(reinterpret_cast<const char*>(indices.data()), indices.size() * sizeof(uint64_t));
        } else {
            Warn("[WriteMesh] Indices are empty!");
        }

        if (!file) {
            Warn("Failed to write data!");
            return;
        }

        file.close();
    }

    UUID LoadMesh(const std::string &path) {
        std::ifstream file(path, std::ios::binary | std::ios::in);
        if (!file) {
            Warn("[Load] Mesh binary file can't opening: " + path);
            return{};
        }

        MeshBinaryHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(header));

        // Validate REAL magic numbers
        if (header.m_Magic != MakeFourCC('R', 'E', 'A', 'L')) { // Little-endian
            Warn("Real Magic number mismatch!");
            return{};
        }

        std::vector<Graphics::Vertex> vertices(header.m_VertexCount);
        if (header.m_VertexCount > 0) {
            file.read(reinterpret_cast<char*>(vertices.data()), header.m_VertexCount * sizeof(Graphics::Vertex));
        }

        std::vector<uint64_t> indices(header.m_IndexCount);
        if (header.m_IndexCount > 0) {
            file.read(reinterpret_cast<char*>(indices.data()), header.m_IndexCount * sizeof(uint64_t));
        }

        if (!file) {
            Warn("Failed to read data!");
            return{};
        }

        // Call the UUID constructor before sending UUIDs
        const auto meshUUID = UUID(header.m_MeshUUID);
        const auto matUUID  = UUID(header.m_MaterialUUID);
        Services::GetMeshManager()->CreateSingleMesh(vertices, indices, matUUID, meshUUID);

        file.close();
        return meshUUID;
    }

}
