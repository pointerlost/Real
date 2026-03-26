//
// Created by pointerlost on 12/15/25.
//
#include <Serialization/Binary.h>
#include <fstream>

#include "Common/Macros.h"
#include "Common/Types.h"
#include "../../include/Assets/AssetManager.h"
#include "Core/Logger.h"
#include "../../include/Common/Utils.h"
#include "../../include/Assets/MeshManager.h"
#include "Graphics/RenderTypes.h"

namespace Real::serialization::binary {

    void WriteModel(
        const String &path,
        assets::ModelBinaryHeader binaryHeader,
        const Vector<UUID>& meshUUIDs,
        const Vector<UUID>& materialUUIDs)
    {
        std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!file) {
            Warn("[Write] Model binary file can't opening: " + path);
            return;
        }

        // Update the mesh count to ensure the size is correct
        binaryHeader.meshCount = static_cast<u32>(meshUUIDs.size());

        // Write entire header once
        file.write(reinterpret_cast<const char*>(&binaryHeader), sizeof(binaryHeader));

        Vector<u64> raw_meshUUIDs;
        raw_meshUUIDs.reserve(meshUUIDs.size());
        for (const auto& uuid : meshUUIDs) {
            raw_meshUUIDs.push_back(static_cast<u64>(uuid));
        }
        // Bulk upload Mesh UUIDs
        file.write(reinterpret_cast<const char*>(raw_meshUUIDs.data()), raw_meshUUIDs.size() * sizeof(u64));

        Vector<u64> raw_matUUIDs;
        raw_matUUIDs.reserve(materialUUIDs.size());
        for (const auto& uuid : materialUUIDs) {
            raw_matUUIDs.push_back(static_cast<u64>(uuid));
        }
        // Bulk upload Mesh UUIDs
        file.write(reinterpret_cast<const char*>(raw_matUUIDs.data()), raw_matUUIDs.size() * sizeof(u64));

        if (!file) {
            Warn("[WriteModel] Failed to write data!");
            return;
        }

        file.close();
    }

    std::tuple<assets::ModelBinaryHeader, Vector<UUID>, Vector<UUID>> LoadModel(const String &path)
    {
        Vector<graphics::Vertex> vertices;
        Vector<u64> indices;

        std::ifstream file(path, std::ios::binary | std::ios::in);
        if (!file) {
            Warn("[Load] Model binary file can't opening: " + path);
            return{};
        }

        assets::ModelBinaryHeader header;

        // Read entire header
        file.read(reinterpret_cast<char*>(&header), sizeof(header));

        // Validate REAL magic numbers
        if (header.magic != MakeFourCC('R', 'E', 'A', 'L')) { // Little-endian
            Warn("Real Magic number mismatch!");
            return{};
        }

        Vector<u64> raw_MeshUUIDs(header.meshCount);
        Vector<u64> raw_MatUUIDs(header.meshCount);

        Vector<UUID> meshUUIDs;
        meshUUIDs.reserve(header.meshCount);

        // per-mesh material so reserve with mesh count
        Vector<UUID> materialUUIDs;
        materialUUIDs.reserve(header.meshCount);

        if (header.meshCount > 0) {
            file.read(reinterpret_cast<char*>(raw_MeshUUIDs.data()), header.meshCount * sizeof(u64));
            file.read(reinterpret_cast<char*>(raw_MatUUIDs.data()),  header.meshCount * sizeof(u64));
        }

        for (u64 raw_id : raw_MeshUUIDs) {
            meshUUIDs.emplace_back(raw_id);
        }

        for (u64 raw_id : raw_MatUUIDs) {
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

    void WriteMesh(
        const String &path,
        const assets::MeshBinaryHeader &binaryHeader,
        const Vector<graphics::Vertex>& vertices,
        const Vector<u32>& indices)
    {
        std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!file) {
            Warn("[Write] Mesh binary file can't opening: " + path);
            return;
        }

        file.write(reinterpret_cast<const char*>(&binaryHeader), sizeof(binaryHeader));

        if (!vertices.empty()) {
            file.write(reinterpret_cast<const char*>(vertices.data()), vertices.size() * sizeof(graphics::Vertex));
        } else {
            Warn("[WriteMesh] Vertices are empty!");
        }

        if (!indices.empty()) {
            file.write(reinterpret_cast<const char*>(indices.data()), indices.size() * sizeof(u32));
        } else {
            Warn("[WriteMesh] Indices are empty!");
        }

        if (!file) {
            Warn("[WriteMesh] Failed to write data!");
        }
    }

    assets::MeshLoadResult LoadMesh(const String &path)
    {
        std::ifstream file(path, std::ios::binary | std::ios::in);
        if (!file) {
            Warn("[Load] Mesh binary file can't opening: " + path);
            return {};
        }

        assets::MeshLoadResult result{};
        file.read(reinterpret_cast<char*>(&result.header), sizeof(assets::MeshBinaryHeader));

        // Validate REAL magic numbers
        if (result.header.magic != MakeFourCC('R', 'E', 'A', 'L')) {
            Warn("Real Magic number mismatch!");
            return {};
        }

        if (result.header.vertexCount > 0) {
            result.vertices.resize(result.header.vertexCount);
            file.read(reinterpret_cast<char*>(
                result.vertices.data()),
                result.header.vertexCount * sizeof(graphics::Vertex)
            );
        }

        if (result.header.indexCount > 0) {
            result.indices.resize(result.header.indexCount);
            file.read(reinterpret_cast<char*>(result.indices.data()), result.header.indexCount * sizeof(u32));
        }

        if (!file) {
            Warn("[LoadMesh] Failed to read data!");
            return {};
        }

        return result;
    }

}
