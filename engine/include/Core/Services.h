//
// Created by pointerlost on 10/4/25.
//
#pragma once

namespace Real {
    class MeshData;
    class AssetManager;
    class Timer;
    struct EditorState;
    class AssetImporter;
}

namespace Real::Services {
    void SetAssetManager(AssetManager* manager);
    void SetMeshManager(MeshData* manager);
    void SetEditorTimer(Timer* timer);
    void SetEditorState(EditorState* state);
    void SetAssetImporter(AssetImporter* importer);
}

namespace Real::Services {
    AssetManager *GetAssetManager();
    MeshData *GetMeshManager();
    Timer *GetEditorTimer();
    EditorState* GetEditorState();
    AssetImporter* GetAssetImporter();
}