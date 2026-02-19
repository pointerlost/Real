//
// Created by pointerlost on 10/3/25.
//
#pragma once

namespace Real {
    class Scene;

    class Engine {
    public:
        Engine() = default;
        ~Engine();

        void InitResources();
        void InitGameResources(); // This is not permanent, just use it debugging purpose
        void Running();

    private:

        // Scope<Timer> m_GameTimer;
    private:
        void ShutDown();

        void StartPhase() const;
        void UpdatePhase() const;
        void RenderPhase() const;
        void EndPhase();

        void InitWindow();
        void InitServices() const;
        void InitSystems();
        void InitAssetImporter();
        void InitEditorState();
        void InitEditorScene();
        void InitEditorRenderer();
        void InitEditorUIState();
        void InitEditorCamera();
        void InitDebugRenderer();
        void InitResourceLoader();
        void InitAssetManager();
        void InitMeshManager();
        void SetOpenGLStateFunctions();
        void AttachSceneToSystems();
        void SetActiveScene(Scene* scene);

        // TODO: Snapshot editor to game-time
    };
}
