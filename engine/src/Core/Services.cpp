//
// Created by pointerlost on 10/4/25.
//
#include "Core/Services.h"

namespace {
    Real::AssetManager *s_AssetManager;
    Real::MeshData *s_MeshManager;
    Real::RealTimeTimer *s_EditorTimer;
    Real::EditorState* s_EditorState;
    Real::AssetImporter* s_AssetImporter;
    Real::graphics::debug::DebugRenderer* s_DebugRenderer;
}

namespace Real::Services {
    void SetAssetManager(AssetManager *manager) {
        s_AssetManager = manager;
    }

    void SetMeshManager(MeshData *manager) {
        s_MeshManager = manager;
    }

    void SetEditorTimer(RealTimeTimer* timer) {
        s_EditorTimer = timer;
    }

    void SetEditorState(EditorState *state) {
        s_EditorState = state;
    }

    void SetAssetImporter(AssetImporter *importer) {
        s_AssetImporter = importer;
    }

    void SetDebugRenderer(graphics::debug::DebugRenderer *dr) {
        s_DebugRenderer = dr;
    }
}

namespace Real::Services {
    AssetManager* GetAssetManager() {
        return s_AssetManager;
    }

    MeshData* GetMeshManager() {
        return s_MeshManager;
    }

    RealTimeTimer* GetEditorTimer() {
        return s_EditorTimer;
    }

    EditorState* GetEditorState() {
        return s_EditorState;
    }

    AssetImporter* GetAssetImporter() {
        return s_AssetImporter;
    }

    graphics::debug::DebugRenderer* GetDebugRenderer() {
        return s_DebugRenderer;
    }
}
