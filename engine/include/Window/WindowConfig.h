//
// Created by pointerlost on 2/17/26.
//
#pragma once
#include <string>

namespace Real {
    enum struct WindowType;
}

namespace Real::core {

    struct WindowConfig {
        int width = 1520;
        int height = 840;
        std::string title = "Real";
        WindowType type;
    };

    enum class WindowBackend {
        GLFW,   // screaming snake for acronyms
        SDL,
    };

}
