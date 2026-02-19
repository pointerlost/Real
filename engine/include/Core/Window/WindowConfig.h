//
// Created by pointerlost on 2/17/26.
//
#pragma once
#include <string>

namespace Real::core {

    struct WindowConfig {
        int width = 1520;
        int height = 840;
        std::string title = "Real";
        WindowType type;
    };

}