//
// Created by pointerlost on 3/21/26.
//
#include "ApplicationBuilder.h"
#include "EditorApplication.h"
#include "../apps/game/include/GameApplication.h"
#include "Core/Logger.h"

namespace Real {

    Scope<core::IApplication> ApplicationBuilder::Build(ApplicationMode mode)
    {
        if (mode == ApplicationMode::Editor) {
            app::editor::EditorContext ctx;
            return CreateScope<app::editor::EditorApplication>(std::move(ctx));
        }
        // if (mode == ApplicationMode::Game) {
        //     realgame::app::GameContext ctx;
        //     return CreateScope<realgame::app::GameApplication>(std::move(ctx));
        // }

        Error("There is no ApplicationMode!");
        return nullptr;
    }
}
