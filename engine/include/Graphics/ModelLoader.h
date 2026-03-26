//
// Created by pointerlost on 12/4/25.
//
#pragma once
#include <filesystem>
#include <assimp/scene.h>
#include <string>
#include <unordered_map>
#include "Texture/Texture.h"
#include "Common/Types.h"
#include "Common/Utils.h"

namespace Real::graphics {
    struct Material;
    struct Model;
}

namespace Real {

    class ModelLoader {
    public:
        void LoadAll(const String& rootDir);
        Ref<graphics::Model> Load(
            const String& filePath,
            const String& name,
            ImageFormatState state = ImageFormatState::COMPRESS_ME
        );

    private:
        void ProcessNode(const aiNode* node, const aiScene* scene, const aiMatrix4x4& parentTransform);
        void ProcessMesh(const aiMesh* mesh, const aiScene* scene, const aiMatrix4x4& transform);
        Ref<graphics::Material> ProcessMaterial(const aiMaterial* mat, int materialIndex);
        void AddTextureToMaterial(
            const Ref<platform::opengl::OpenGLTexture>& tex,
            const Ref<graphics::Material>& material
        );
        void SaveModelTextureAsFile(const Ref<platform::opengl::OpenGLTexture>& tex);

        std::filesystem::path ChooseBest(const Vector<std::filesystem::path>& paths);

    private:
        bool m_IsFBX = false;
        Ref<graphics::Model> m_CurrentModel;
        String m_CurrentDirectory;
        ImageFormatState m_CurrImageFormatState = ImageFormatState::COMPRESS_ME;
        std::unordered_map<String, Vector<std::filesystem::path>> m_TextureIndex;
        std::unordered_map<String, UUID> m_CacheProcessedTextures;
    };
}
