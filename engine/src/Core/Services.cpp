//
// Created by pointerlost on 10/4/25.
//
#include "Core/Services.h"

namespace {
    Real::AssetManager *s_AssetManager;
    Real::MeshManager *s_MeshManager;
    Real::EditorState* s_EditorState;
    Real::AssetImporter* s_AssetImporter;
    Real::graphics::debug::DebugRenderer* s_DebugRenderer;
}

namespace Real::Services {
    void SetAssetManager(AssetManager *manager) {
        s_AssetManager = manager;
    }

    void SetMeshManager(MeshManager *manager) {
        s_MeshManager = manager;
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

    MeshManager* GetMeshManager() {
        return s_MeshManager;
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
