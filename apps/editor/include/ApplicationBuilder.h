//
// Created by pointerlost on 3/21/26.
//
#pragma once
#include "Core/Utils.h"

namespace Real::core {
    class IApplication;
}

namespace Real {

    enum class ApplicationMode {
        Editor,
        Game
    };

    class ApplicationBuilder {
    public:
        static Scope<core::IApplication> Build(ApplicationMode mode);
    };
}
