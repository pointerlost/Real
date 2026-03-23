//
// Created by pointerlost on 10/4/25.
//
#pragma once

namespace Real {
    class MeshManager;
    class AssetManager;
    struct EditorState;
    class AssetImporter;

    namespace graphics::debug {
        class DebugRenderer;
    }
}

namespace Real::Services {
    void SetAssetManager(AssetManager* manager);
    void SetMeshManager(MeshManager* manager);
    void SetEditorState(EditorState* state);
    void SetAssetImporter(AssetImporter* importer);
    void SetDebugRenderer(graphics::debug::DebugRenderer* dr);
}

namespace Real::Services {
    AssetManager  *GetAssetManager();
    MeshManager   *GetMeshManager();
    EditorState   *GetEditorState();
    AssetImporter *GetAssetImporter();
    graphics::debug::DebugRenderer* GetDebugRenderer();
}