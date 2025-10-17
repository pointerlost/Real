//
// Created by pointerlost on 10/4/25.
//
#pragma once

namespace Real {
    class MeshManager;
    class AssetManager;
    class Timer;
    struct EditorState;
}

namespace Real::Services {
    void SetAssetManager(AssetManager* manager);
    void SetMeshManager(MeshManager* manager);
    void SetEditorTimer(Timer* timer);
    void SetEditorState(EditorState* state);
}

namespace Real::Services {
    AssetManager *GetAssetManager();
    MeshManager *GetMeshManager();
    Timer *GetEditorTimer();
    EditorState* GetEditorState();
}