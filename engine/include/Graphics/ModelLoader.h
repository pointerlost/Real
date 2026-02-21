//
// Created by pointerlost on 12/4/25.
//
#pragma once
#include <filesystem>
#include <assimp/scene.h>
#include <string>
#include <unordered_map>
#include "Texture.h"
#include "Common/RealTypes.h"
#include "Core/Utils.h"

namespace Real {
    struct Model;
    struct Material;
}

namespace Real {

    class ModelLoader {
    public:
        void LoadAll(const String& rootDir);
        Ref<Model> Load(const String& filePath, const String& name, ImageFormatState state = ImageFormatState::COMPRESS_ME);

    private:
        void ProcessNode(const aiNode* node, const aiScene* scene, const aiMatrix4x4& parentTransform);
        void ProcessMesh(const aiMesh* mesh, const aiScene* scene, const aiMatrix4x4& transform);
        Ref<Material> ProcessMaterial(const aiMaterial* mat, int materialIndex);
        void AddTextureToMaterial(const Ref<OpenGLTexture>& tex, const Ref<Material>& material);
        void SaveModelTextureAsFile(const Ref<OpenGLTexture>& tex);

        std::filesystem::path ChooseBest(const Vector<std::filesystem::path>& paths);

    private:
        bool m_IsFBX = false;
        Ref<Model> m_CurrentModel;
        String m_CurrentDirectory;
        ImageFormatState m_CurrImageFormatState = ImageFormatState::COMPRESS_ME;
        std::unordered_map<String, Vector<std::filesystem::path>> m_TextureIndex;
        std::unordered_map<String, UUID> m_CacheProcessedTextures;
    };
}
