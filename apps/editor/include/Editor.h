//
// Created by pointerlost on 10/17/25.
//
#pragma once
#include <imgui.h>
#include "IPanel.h"
#include "ImGuizmo/ImGuizmo.h"
#include "Common/Types.h"
#include "Core/RealConfig.h"
#include "Core/Utils.h"
#include "Math/Vec2.h"

namespace Real {
    class Scene;

    namespace UI {
        class InspectorPanel;
        class HierarchyPanel;
    }

    namespace core   { class IWindow; }
}

namespace Real::UI {

    class Editor {
    public:
        Editor();

        void Init(core::IWindow* window);
        void BeginFrame(float dt);
        void Update(float dt);
        void RenderUI();
        void EndFrame();

        void OnAttachScene(Scene *scene);
        void Shutdown();

    private:
        Scene* m_Scene = nullptr;
        core::IWindow* m_Window = nullptr;
        Vector<Scope<IPanel>> m_Panels;
        bool m_OpenPerformanceProfile = false;
        ImGuizmo::OPERATION m_GizmoType = ImGuizmo::TRANSLATE;
        // User intention
        bool m_EditEntityTransform = true;

        // Screen height can wrong for editor-time, because of main menu panel has some height
        // TODO: remove hardcoded stuff
        math::Vec2 m_SceneWindowSize = math::Vec2(SCREEN_WIDTH - (SCREEN_WIDTH / 5 + 31.0) * 2, SCREEN_HEIGHT);

    private:
        void Render();

        void RenderMenuBar();
        void RenderFileBar();
        void RenderEditBar();
        void RenderViewBar();
        void RenderDebugBar();
        void RenderHelpBar();

        void DrawPerformanceProfile();
        void UpdateInputUI();

        void InitFontStyle();
        void InitDarkTheme();

        void RenderSceneGizmos();
        void UpdateGizmoLogic();
        void DebugGizmos();
    };
}
