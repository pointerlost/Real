//
// Created by pointerlost on 10/4/25.
//
#include "Core/Services.h"

#include "Core/Timer.h"

namespace {
    Real::AssetManager *s_AssetManager;
    Real::MeshManager *s_MeshManager;
    Real::Timer *s_EditorTimer;
    Real::EditorState* s_EditorState;
}

namespace Real::Services {
    void SetAssetManager(AssetManager *manager) {
        s_AssetManager = manager;
    }

    void SetMeshManager(MeshManager *manager) {
        s_MeshManager = manager;
    }

    void SetEditorTimer(Timer* timer) {
        s_EditorTimer = timer;
    }

    void SetEditorState(EditorState *state) {
        s_EditorState = state;
    }
}

namespace Real::Services {
    AssetManager *GetAssetManager() {
        return s_AssetManager;
    }

    MeshManager *GetMeshManager() {
        return s_MeshManager;
    }

    Timer *GetEditorTimer() {
        return s_EditorTimer;
    }

    EditorState *GetEditorState() {
        return s_EditorState;
    }
}