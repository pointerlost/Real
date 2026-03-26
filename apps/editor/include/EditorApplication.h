//
// Created by pointerlost on 2/19/26.
//
#pragma once
#include "Editor.h"
#include "EditorState.h"
#include "Core/IApplication.h"
#include "Core/IApplicationContext.h"
#include "../../../engine/include/Common/Utils.h"
#include "Input/CameraInput.h"

namespace Real {
    namespace ecs {
        class CameraSystem;
    }

    namespace core {
        class IRenderer;
    }
}

namespace Real::app::editor {

    struct EditorContext final : public core::IApplicationContext {
        void SetWindow(core::IWindow *win)         override { window = win;         }
        void SetAssetManager  (AssetManager*  am)  override { assetManager   = am;  }
        void SetAssetImporter (AssetImporter* ai)  override { assetImporter  = ai;  }
        void SetMeshManager   (MeshManager*   mm)  override { meshManager    = mm;  }
        void SetResourceLoader(ResourceLoader* rl) override { resourceLoader = rl;  }
        void SetSceneManager  (SceneManager*  sm)  override { sceneManager   = sm;  }
        void SetDebugRenderer (graphics::debug::DebugRenderer* dr) override { debugRenderer = dr; }
        void SetCameraSystem  (ecs::CameraSystem* cam) override { cameraSystem = cam; }

        core::IWindow*  window         = nullptr;
        AssetManager*   assetManager   = nullptr;
        AssetImporter*  assetImporter  = nullptr;
        MeshManager*    meshManager    = nullptr;
        ResourceLoader* resourceLoader = nullptr;
        SceneManager*   sceneManager   = nullptr;
        graphics::debug::DebugRenderer* debugRenderer = nullptr;
        ecs::CameraSystem* cameraSystem = nullptr;
    };

    class EditorApplication final : public core::IApplication {
    public:
        explicit EditorApplication(EditorContext ctx) noexcept;

        void Init() override;
        void Update(float dt) override;
        void Render() override;
        void Shutdown() override;
        core::IApplicationContext &GetContext() override { return m_Ctx; }

    private:
        EditorContext m_Ctx;

        Scope<UI::Editor>    m_Editor;
        Scope<EditorState>   m_State;
        Scope<CameraInput>   m_CameraInput;
    };
}
