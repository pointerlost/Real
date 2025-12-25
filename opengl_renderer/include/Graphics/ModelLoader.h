//
// Created by pointerlost on 12/4/25.
//
#pragma once
#include <filesystem>
#include <assimp/scene.h>
#include <string>
#include "MeshManager.h"
#include "Texture.h"
#include "Common/RealTypes.h"

namespace Real {
    struct Model;
    struct Material;
}

namespace Real {

    class ModelLoader {
    public:
        void LoadAll(const std::string& rootDir);
        Ref<Model> Load(const std::string& filePath, const std::string& name, ImageFormatState state = ImageFormatState::COMPRESS_ME);

    private:
        void ProcessNode(const aiNode* node, const aiScene* scene);
        MeshBinaryHeader ProcessMesh(const aiMesh* mesh, const aiScene* scene);
        Ref<Material> ProcessMaterial(const aiMaterial* mat, int materialIndex);
        std::filesystem::path ChooseBest(const std::vector<std::filesystem::path>& paths);
        TextureType GetRealTypeFromAssimpTexType(aiTextureType type);

    private:
        bool m_IsFBX = false;
        Ref<Model> m_CurrentModel;
        std::string m_CurrentDirectory;
        ImageFormatState m_CurrImageFormatState = ImageFormatState::COMPRESS_ME;
        std::unordered_map<std::string, std::vector<std::filesystem::path>> m_TextureIndex;
    };
}
