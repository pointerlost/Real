//
// Created by pointerlost on 2/17/26.
//
#pragma once

namespace Real { class Scene; }

namespace Real::core {

    class IEditor {
    public:
        virtual ~IEditor() = default;

        // Called once after construction; set up ImGui, panels, fonts
        virtual void Init() = 0;

        // Called at start of each frame: prepare ImGui/new-frame and editor input handling
        virtual void BeginFrame(float dt) = 0;

        // Editor logic updates (selection, gizmos, command buffer)
        virtual void Update(float dt) = 0;

        // Render UI: ImGui::Render() and submit draw lists (should not call SwapBuffers)
        virtual void RenderUI() = 0;

        // Called after rendering and swap if you need end-of-frame cleanup; optional
        virtual void EndFrame() = 0;

        // Called when scene attached/removed
        virtual void OnAttachScene(Scene* scene) = 0;
        virtual void Shutdown() = 0;
    };
}