//
// Created by pointerlost on 12/4/25.
//
#pragma once
#include <filesystem>
#include <assimp/scene.h>
#include <string>
#include <unordered_map>
#include "Graphics/RenderEnum.h"
#include "Common/Types.h"
#include "Common/Utils.h"
#include "Core/UUID.h"

namespace Real {
    namespace graphics {
        struct Material;
        struct Model;
    }

    namespace platform::opengl {
        class OpenGLTexture;
    }
}

namespace Real::assets {

    using namespace graphics;

    class ModelManager final {
    public:
        void LoadAll(const String& rootDir);
        Ref<Model> Load(
            const String& filePath,
            const String& name,
            ImageFormatState state = ImageFormatState::COMPRESS_ME
        );

        // Models
        void                     RegisterModel(const Ref<Model>& model);
        [[nodiscard]] Ref<Model> GetModel     (const String& name) const;
        [[nodiscard]] bool       IsModelExist (const String& name) const;

    private:
        void ProcessNode(const aiNode* node, const aiScene* scene, const aiMatrix4x4& parentTransform);
        void ProcessMesh(const aiMesh* mesh, const aiScene* scene, const aiMatrix4x4& transform);
        Ref<Material> ProcessMaterial(const aiMaterial* mat, int materialIndex);
        void AddTextureToMaterial(
            const Ref<platform::opengl::OpenGLTexture>& tex,
            const Ref<Material>& material
        );

        std::filesystem::path ChooseBest(const Vector<std::filesystem::path>& paths);

    private:
        bool       m_IsFBX = false;
        Ref<Model> m_CurrentModel     {};
        String     m_CurrentDirectory {};

        ImageFormatState m_CurrImageFormatState = ImageFormatState::COMPRESS_ME;

        std::unordered_map<String, Vector<std::filesystem::path>> m_TextureIndex {};
        std::unordered_map<String, UUID>                          m_CacheProcessedTextures {};

        std::unordered_map<UUID, Ref<Model>> m_Models;
        std::unordered_map<String, UUID>     m_ModelNameToUUID;
    };
}
