//
// Created by pointerlost on 10/8/25.
//
#pragma once
#include "Scene/Entity.h"

namespace Real {

    struct EditorState {
        Entity* selectedEntity{};
        Entity* camera{};
        bool Running = true;
        bool FpsMode = true;
    };
}
