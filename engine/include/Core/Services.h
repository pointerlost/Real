//
// Created by pointerlost on 10/4/25.
//
#pragma once

namespace Real {
    class MeshManager;
    class AssetManager;
    class RealTimeTimer;
    struct EditorState;
    class AssetImporter;

    namespace graphics::debug {
        class DebugRenderer;
    }
}

namespace Real::Services {
    void SetAssetManager(AssetManager* manager);
    void SetMeshManager(MeshManager* manager);
    void SetEditorTimer(RealTimeTimer* timer);
    void SetEditorState(EditorState* state);
    void SetAssetImporter(AssetImporter* importer);
    void SetDebugRenderer(graphics::debug::DebugRenderer* dr);
}

namespace Real::Services {
    AssetManager *GetAssetManager();
    MeshManager *GetMeshManager();
    RealTimeTimer *GetEditorTimer();
    EditorState* GetEditorState();
    AssetImporter* GetAssetImporter();
    graphics::debug::DebugRenderer* GetDebugRenderer();
}