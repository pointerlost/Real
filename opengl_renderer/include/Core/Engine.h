//
// Created by pointerlost on 10/3/25.
//
#pragma once
#include <Core/Window.h>
#include <memory>
#include "AssetManager.h"
#include "Timer.h"
#include "Utils.h"
#include "Editor/EditorPanel.h"
#include "Editor/EditorState.h"
#include "Editor/HierarchyPanel.h"
#include "Editor/InspectorPanel.h"
#include "Graphics/MeshManager.h"
#include "Graphics/ModelLoader.h"
#include "Graphics/Renderer.h"
#include "Input/CameraInput.h"
#include "Resource/ResourceLoader.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Scene/Systems.h"

namespace Real {

    class Engine {
    public:
        Engine() = default;
        ~Engine();

        void InitResources();
        void InitGameResources(); // This is not permanent, just use it debugging purpose
        void Running();

    private:
        Scope<Timer> m_EditorTimer;
        Scope<EditorState> m_EditorState;
        Scope<Graphics::Window> m_Window;
        Scope<ModelLoader> m_ModelLoader;
        Scope<MeshData> m_MeshManager;
        Scope<AssetManager> m_AssetManager;
        Scope<opengl::Renderer> m_Renderer;
        Scope<Scene> m_Scene;
        Scope<CameraInput> m_CameraInput;
        Scope<UI::EditorPanel> m_EditorPanel;
        Scope<UI::HierarchyPanel> m_HierarchyPanel;
        Scope<UI::InspectorPanel> m_InspectorPanel;
        Scope<Systems> m_Systems;
        Scope<ResourceLoader> m_ResourceLoader;

        // Scope<Timer> m_GameTimer;
    private:
        void ShutDown();

        void StartPhase() const;
        void UpdatePhase() const;
        void RenderPhase() const;
        void EndPhase(GLFWwindow* window);

        void InitWindow();
        void InitServices() const;
        void InitSystems();
        void InitEditorState();
        void InitEditorScene();
        void InitEditorRenderer();
        void InitEditorUIState();
        void InitEditorCamera();
        void InitResourceLoader();
        void InitAssetManager();
        void InitMeshManager();
        void SetOpenGLStateFunctions();

        // TODO: Snapshot editor to game-time
    };
}
