//
// Created by pointerlost on 2/16/26.
//
#pragma once
#include "Core/Utils.h"

namespace Real::core {
    class EngineCore;
}

namespace Real::core {

    struct EngineConfig {
        bool editorMode = true;
        int windowWidth = 1280;
        int windowHeight = 720;
        std::string title = "Real";
    };

    class EngineBootstrap {
    public:
        static Scope<EngineCore> Build(const EngineConfig& cfg);
    };
}