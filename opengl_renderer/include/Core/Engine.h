//
// Created by pointerlost on 10/3/25.
//
#pragma once
#include <Core/Window.h>
#include <memory>
#include "AssetManager.h"
#include "Timer.h"
#include "Utils.h"
#include "Graphics/MeshManager.h"
#include "Graphics/Renderer.h"
#include "Input/CameraInput.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"

namespace Real {

    class Engine {
    public:
        Engine() = default;
        ~Engine();

        void InitResources();
        void Running();

    private:
        Scope<Graphics::Window> m_Window;
        Scope<MeshManager> m_MeshManager;
        Scope<AssetManager> m_AssetManager;
        Scope<opengl::Renderer> m_Renderer;
        Scope<Scene> m_Scene;

        Entity editorCamera;
        Scope<CameraInput> m_CameraInput;

        Scope<Timer> m_EditorTimer;
        // Scope<Timer> m_GameTimer;
    private:
        void ShutDown();

        void StartPhase();
        void UpdatePhase();
        void RenderPhase();
        void EndPhase(GLFWwindow* window);

        void InitServices();
        void InitAsset();
        void InitMesh();

        // TODO: Snapshot editor to game-time
    };
}
