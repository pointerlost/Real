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

    void WriteModel(const std::string &path, ModelBinaryHeader binaryHeader,
        std::vector<Graphics::Vertex> vertices, std::vector<uint64_t> indices)
    {
        std::ofstream file(path, std::ios::binary);
        if (!file) {
            Warn("Can't created model binary file: " + path);
            return;
        }

        // Write magic numbers
        file.write(reinterpret_cast<char*>(binaryHeader.m_Magic), sizeof(uint32_t));

        // Write version
        file.write(reinterpret_cast<char*>(&binaryHeader.m_Version), sizeof(uint32_t));

        // Write UUID
        file.write(reinterpret_cast<char*>(&binaryHeader.m_UUID), sizeof(uint64_t));

        // Write Per Mesh data
        size_t count = binaryHeader.m_MeshData.size();
        file.write(reinterpret_cast<char *>(&count), sizeof(count));
        file.write(reinterpret_cast<const char *>(binaryHeader.m_MeshData.data()), count * sizeof(MeshEntry));

        // Write Mesh Info
        file.write(reinterpret_cast<const char *>(&binaryHeader.m_MeshInfo), sizeof(binaryHeader.m_MeshInfo));

        const auto& vc = binaryHeader.m_MeshInfo.m_VertexCount;
        const auto& ic = binaryHeader.m_MeshInfo.m_IndexCount;

        file.write(reinterpret_cast<char*>(vertices.data()), vc * sizeof(Graphics::Vertex));
        file.write(reinterpret_cast<char*>(indices.data()),  ic * sizeof(uint64_t));

        file.close();
    }

    ModelBinaryHeader LoadModel(const std::string &path) {
        const auto& am = Services::GetAssetManager();
        const auto& mm = Services::GetMeshManager();
        ModelBinaryHeader m_binary;
        std::vector<Graphics::Vertex> vertices;
        std::vector<uint64_t> indices;

        std::fstream file(path, std::ios::binary);
        if (!file) {
            Warn("There is no file, path: " + path);
            return{};
        }

        // Read REAL magic numbers
        file.read(reinterpret_cast<char*>(&m_binary.m_Magic), 4);
        if (m_binary.m_Magic != MakeFourCC('R', 'E', 'A', 'L')) { // Little-endian
            Warn("Real magic number mismatch!");
            return{};
        }

        // Read Version
        file.read(reinterpret_cast<char*>(&m_binary.m_Version), sizeof(uint32_t));

        // Read UUID
        file.read(reinterpret_cast<char*>(&m_binary.m_UUID), sizeof(uint64_t));

        // Read mesh count
        file.read((char*)&m_binary.m_MeshCount, sizeof(uint64_t));
        m_binary.m_MeshData.resize(m_binary.m_MeshCount);

        // Read per mesh data
        file.read(reinterpret_cast<char*>(m_binary.m_MeshData.data()), m_binary.m_MeshCount * sizeof(MeshEntry));

        // Read Mesh Info
        file.read(reinterpret_cast<char*>(&m_binary.m_MeshInfo), sizeof(Graphics::MeshInfo));

        const auto& vc = m_binary.m_MeshInfo.m_VertexCount;
        const auto& ic = m_binary.m_MeshInfo.m_IndexCount;

        vertices.resize(vc);
        indices.resize(ic);

        // Read vertices and indices
        file.read(reinterpret_cast<char*>(vertices.data()), vc * sizeof(Graphics::Vertex));
        file.read(reinterpret_cast<char*>(indices.data()),  ic * sizeof(uint64_t));

        file.close();

        mm->LoadMeshFromFile(vertices, indices, m_binary.m_MeshInfo);
        const auto& fi = fs::CreateFileInfoFromPath(path);
        const auto& m  = CreateRef<Model>(m_binary, fi);
        am->SaveModelCPU(m);

        return m_binary;
    }
}
