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
            return CreateScope<app::editor::EditorApplication>(app::editor::EditorContext{});
        }
        // if (mode == ApplicationMode::Game) {
        //     realgame::app::GameContext ctx;
        //     return CreateScope<realgame::app::GameApplication>(std::move(ctx));
        // }

        Error("There is no ApplicationMode!");
        return nullptr;
    }
}
