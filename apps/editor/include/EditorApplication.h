//
// Created by pointerlost on 2/19/26.
//
#pragma once
#include "Editor.h"
#include "EditorState.h"
#include "UIResources.h"
#include "Core/IApplication.h"
#include "Core/IApplicationContext.h"
#include "Common/Utils.h"
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
        void SetWindow        (core::IWindow *win)                 override { window = win;         }
        void SetAssetManager  (assets::AssetManager*  am)          override { assetManager   = am;  }
        void SetAssetImporter (assets::AssetImporter* ai)          override { assetImporter  = ai;  }
        void SetMeshManager   (assets::MeshManager*   mm)          override { meshManager    = mm;  }
        void SetResourceLoader(assets::ResourceManager* rl)        override { resourceLoader = rl;  }
        void SetSceneManager  (SceneManager*  sm)                  override { sceneManager   = sm;  }
        void SetDebugRenderer (graphics::debug::DebugRenderer* dr) override { debugRenderer = dr;   }
        void SetCameraSystem  (ecs::CameraSystem* cam)             override { cameraSystem = cam;   }

        core::IWindow*                  window         = nullptr;
        assets::AssetManager*           assetManager   = nullptr;
        assets::AssetImporter*          assetImporter  = nullptr;
        assets::MeshManager*            meshManager    = nullptr;
        assets::ResourceManager*        resourceLoader = nullptr;
        SceneManager*                   sceneManager   = nullptr;
        graphics::debug::DebugRenderer* debugRenderer  = nullptr;
        ecs::CameraSystem*              cameraSystem   = nullptr;
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

        Scope<UI::Editor>                m_Editor;
        Scope<Real::editor::UIResources> m_UIResources;
        Scope<EditorState>               m_State;
        Scope<CameraInput>               m_CameraInput; // Probably it shouldn't be here?
    };
}
