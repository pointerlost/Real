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
#include "Common/RealTypes.h"

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
        void LoadAll(const std::string& rootDir);
        Ref<Model> Load(const std::string& filePath, ImageFormatState state = ImageFormatState::COMPRESS_ME);

    private:
        void ProcessNode(aiNode* node, const aiScene* scene, const std::string& directory);
        MeshEntry ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);
        Ref<Material> ProcessMaterial(const aiMaterial* mat, int materialIndex, const std::string& directory);

        TextureType GetRealTypeFromAssimpTexType(aiTextureType type);
    private:
        Ref<Model> m_CurrentModel;
        std::string m_CurrentDirectory;
        ModelBinaryHeader m_CurrentBinaryFile;
        ImageFormatState m_CurrImageFormatState = ImageFormatState::COMPRESS_ME;
    };
}
