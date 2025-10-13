//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
#include <glad/glad.h>
#include "Mesh.h"


namespace Real {

    struct Buffer {
        void* ptr;
        GLuint buffer;
        GLsizeiptr size;
        GLsizeiptr capacity;

        GLbitfield flags = (GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT);
    };

    class MeshManager {
    public:
        void InitResourcesWithBufferData();
        void InitResourcesPersistentMapping();

        const std::unordered_map<std::string, Graphics::MeshInfo>& GetAllMeshes() { return m_MeshInfos; }
        [[nodiscard]] const Graphics::MeshInfo &GetMeshData(const std::string& name) const;
        [[nodiscard]] unsigned int GetUniversalVAO() const { return m_UniversalVAO; }
        void BindUniversalVAO() const { glBindVertexArray(m_UniversalVAO); }
        void UnbindCurrVAO() const { glBindVertexArray(0); }

        [[nodiscard]] size_t GetMeshCount() const { return m_AllVertices.size(); }

    private:
        std::unordered_map<std::string, Graphics::MeshInfo> m_MeshInfos;
        std::vector<Graphics::Vertex> m_AllVertices;
        std::vector<uint32_t> m_AllIndices;

        Buffer m_PBufferV;
        Buffer m_PBufferE;

        unsigned int m_UniversalVAO = 0, m_VBO = 0, m_EBO = 0;
    private:
        void CreateSingleMesh(std::vector<Graphics::Vertex> vertices, std::vector<uint32_t> indices,
            const std::string& name);
    };
}
