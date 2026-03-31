//
// Created by pointerlost on 10/4/25.
//
#include "Core/Services.h"
#include "Core/Engine/EngineCore.h"

namespace {
    Real::core::AssetSystems *s_AssetSystems;
    Real::EditorState        *s_EditorState;
    Real::graphics::debug::DebugRenderer *s_DebugRenderer;
}

namespace Real::Services {
    void SetAssetSystems(core::AssetSystems *as) {
        s_AssetSystems = as;
    }

    void SetEditorState(EditorState* state) {
        s_EditorState = state;
    }

    void SetDebugRenderer(graphics::debug::DebugRenderer *dr) {
        s_DebugRenderer = dr;
    }
}

namespace Real::Services {
    core::AssetSystems* GetAssetSystems() {
        return s_AssetSystems;
    }

    EditorState* GetEditorState() {
        return s_EditorState;
    }

    graphics::debug::DebugRenderer* GetDebugRenderer() {
        return s_DebugRenderer;
    }

    assets::AssetImporter& GetAssetImporter() {
        return *s_AssetSystems->assetImporter;
    }

    assets::AssetManager& GetAssetManager() {
        return *s_AssetSystems->assetManager;
    }

    assets::MaterialManager& GetMaterialManager() {
        return *s_AssetSystems->materialManager;
    }

    assets::MeshManager& GetMeshManager() {
        return *s_AssetSystems->meshManager;
    }

    assets::ResourceManager& GetResourceLoader() {
        return *s_AssetSystems->resourceManager;
    }

    assets::ShaderManager& GetShaderManager() {
        return *s_AssetSystems->shaderManager;
    }

    assets::TextureManager& GetTextureManager() {
        return *s_AssetSystems->textureManager;
    }

    assets::ModelManager& GetModelManager() {
        return *s_AssetSystems->modelManager;
    }
}
