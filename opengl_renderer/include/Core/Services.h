//
// Created by pointerlost on 10/4/25.
//
#pragma once

namespace Real {
    class MeshManager;
    class AssetManager;
    class Timer;
}

namespace Real::Services {
    void SetAssetManager(AssetManager* manager);
    void SetMeshManager(MeshManager* manager);
    void SetEditorTimer(Timer* timer);
}

namespace Real::Services {
    AssetManager *GetAssetManager();
    MeshManager *GetMeshManager();
    Timer *GetEditorTimer();
}