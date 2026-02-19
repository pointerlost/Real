//
// Created by pointerlost on 2/16/26.
//
#pragma once
#include "Core/AssetImporter.h"
#include "Core/AssetManager.h"
#include "Core/Window/GLFWPlatform.h"
#include "Platform/opengl/OpenGLBackend.h"
#include "Core/Window/GLFWwindow.h"
#include "Platform/opengl/OpenGLRenderer.h"
#include "Scene/Scene.h"
#include "Scene/Systems/SystemsManager.h"
#include "Timer/Timer.h"
#include "../../../../apps/editor/include/Editor.h"
#include "../../../../apps/editor/include/EditorState.h"
#include "Graphics/ModelLoader.h"
#include "Graphics/Debug/DebugRenderer.h"
#include "Input/CameraInput.h"
#include "Physics/IPhysicsBackend.h"
#include "Resource/ResourceLoader.h"

namespace Real {
    class MeshManager;
}

namespace Real::core {
    struct EngineConfig;
    class IEditor;
    class SystemManager;
    class IWindow;
    class IRenderer;
    class ITimer;
    class IGPUUploader;
}

namespace Real::core {

    class EngineCore {
    public:
        explicit EngineCore(const EngineConfig& cfg);

        void Start();    // start loop (bootstrap should have done Init on subsystems)
        void RunLoop();  // orchestrates per-frame calls (poll, update, upload, render, swap)
        void Stop();     // request stop

    private:
        Scope<platform::GLFWPlatform> m_GLFWPlatform;
        Scope<IGraphicsBackend> m_GraphicBackend;
        Scope<IWindow> m_Window;
        Scope<IRenderer> m_Renderer;
        Scope<Scene> m_Scene;
        Scope<SystemManager> m_Systems;
        Scope<physics::IPhysicsBackend> m_PhysicsBackend;
        Scope<ITimer> m_Timer;
        /*IGPUUploader& m_Uploader;*/
        Scope<IEditor> m_Editor;
        Scope<EditorState> m_EditorState;
        Scope<ModelLoader> m_ModelLoader;
        Scope<AssetManager> m_AssetManager;
        Scope<MeshManager> m_MeshManager;
        Scope<graphics::debug::DebugRenderer> m_DebugRenderer;
        Scope<CameraInput> m_CameraInput;
        Scope<ResourceLoader> m_ResourceLoader;
        Scope<AssetImporter> m_AssetImporter;

        bool m_ShouldStop{false};
    };
}