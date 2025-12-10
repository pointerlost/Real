//
// Created by pointerlost on 12/4/25.
//
#pragma once
#include <assimp/scene.h>
#include <string>
#include <vector>
#include "MeshManager.h"
#include "Model.h"
#include "Texture.h"

namespace Real {
    struct ModelBinaryHeader;
}

namespace Real {
    struct Material;
    struct Model;
}

namespace Real {

    class ModelLoader {
    public:
        void LoadAll(const std::string& dir_path);
        Ref<Model> Load(const std::string& filePath, ImageFormatState state = ImageFormatState::COMPRESS_ME);

    private:
        void ProcessNode(aiNode* node, const aiScene* scene, const std::string& directory);
        MeshEntry ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);
        Ref<Material> ProcessMaterial(const aiMaterial* mat, const std::string& directory);

        TextureType GetRealTypeFromAssimpTexType(aiTextureType type);

        ModelBinaryHeader LoadFromBinaryFile(const std::string& path);
        [[nodiscard]] bool WriteToBinaryFile(const std::string& path, ModelBinaryHeader binaryHeader,
            std::vector<Graphics::Vertex> vertices, std::vector<uint64_t> indices
        ) const;

    private:
        Ref<Model> m_CurrentModel;
        std::string m_CurrentDirectory;
        ImageFormatState m_CurrImageFormatState = ImageFormatState::COMPRESS_ME;
    };
}
