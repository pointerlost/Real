//
// Created by pointerlost on 10/4/25.
//
#include "Core/Services.h"

namespace {
    Real::AssetManager *s_AssetManager;
    Real::MeshData *s_MeshManager;
    Real::Timer *s_EditorTimer;
    Real::EditorState* s_EditorState;
    Real::AssetImporter* s_AssetImporter;
}

namespace Real::Services {
    void SetAssetManager(AssetManager *manager) {
        s_AssetManager = manager;
    }

    void SetMeshManager(MeshData *manager) {
        s_MeshManager = manager;
    }

    void SetEditorTimer(Timer* timer) {
        s_EditorTimer = timer;
    }

    void SetEditorState(EditorState *state) {
        s_EditorState = state;
    }

    void SetAssetImporter(AssetImporter *importer) {
        s_AssetImporter = importer;
    }
}

namespace Real::Services {
    AssetManager* GetAssetManager() {
        return s_AssetManager;
    }

    MeshData* GetMeshManager() {
        return s_MeshManager;
    }

    Timer* GetEditorTimer() {
        return s_EditorTimer;
    }

    EditorState* GetEditorState() {
        return s_EditorState;
    }

    AssetImporter* GetAssetImporter() {
        return s_AssetImporter;
    }
}
