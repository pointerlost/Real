//
// Created by pointerlost on 10/4/25.
//
#pragma once

namespace Real::assets {
    class ModelManager;
}

namespace Real::assets {
    class ResourceManager;
}

namespace Real {
    struct EditorState;
    namespace core { struct AssetSystems; }

    namespace graphics::debug {
        class DebugRenderer;
    }
    namespace assets {
        class ShaderManager;
        class MaterialManager;
        class TextureManager;
        class AssetImporter;
        class MeshManager;
        class AssetManager;
    }
}

namespace Real::Services {
    void SetAssetSystems (core::AssetSystems* as);
    void SetEditorState  (EditorState* state);
    void SetDebugRenderer(graphics::debug::DebugRenderer* dr);
}

namespace Real::Services {
    core::AssetSystems             *GetAssetSystems();
    EditorState                    *GetEditorState();
    graphics::debug::DebugRenderer *GetDebugRenderer();

    [[nodiscard]] assets::AssetImporter   &GetAssetImporter();
    [[nodiscard]] assets::AssetManager    &GetAssetManager();
    [[nodiscard]] assets::MaterialManager &GetMaterialManager();
    [[nodiscard]] assets::MeshManager     &GetMeshManager();
    [[nodiscard]] assets::ResourceManager &GetResourceLoader();
    [[nodiscard]] assets::ShaderManager   &GetShaderManager();
    [[nodiscard]] assets::TextureManager  &GetTextureManager();
    [[nodiscard]] assets::ModelManager    &GetModelManager();
}