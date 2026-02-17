//
// Created by pointerlost on 2/16/26.
//
#pragma once

namespace Real {
}

namespace Real::core {
    class SystemManager;
    class IWindow;
    class IRenderer;
    class Scene;
    class ISystem;
    class ITimer;
    class IGPUUploader;
    class IEditorModule;
}

namespace Real::core {

    class EngineCore {
    public:
        EngineCore(IWindow& window,
                   IRenderer& renderer,
                   Scene& scene,
                   SystemManager& systems,
                   ITimer& timer
                   /* IGPUUploader& uploader,*/
                   /*IEditorModule* editor = nullptr*/
        ) noexcept;

        void Start();    // start loop (bootstrap should have done Init on subsystems)
        void RunLoop();  // orchestrates per-frame calls (poll, update, upload, render, swap)
        void Stop();     // request stop

    private:
        IWindow& m_Window;
        IRenderer& m_Renderer;
        Scene& m_Scene;
        SystemManager& m_Systems;
        ITimer& m_Timer;
        /*IGPUUploader& m_Uploader;*/
        /*IEditorModule* m_Editor;*/
        bool m_ShouldStop{false};
    };
}