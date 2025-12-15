//
// Created by pointerlost on 10/3/25.
//
#pragma once
#include <iostream>

namespace Real {

    template <typename T>
    constexpr void Info(const T& message) {
        std::cout << "INFO: " << message << std::endl;
    }

    template <typename T>
    constexpr void Warn(const T& message) {
        std::cout << "WARNING: " << message << std::endl;
    }

    template <typename T>
    constexpr void Error(const T& message) {
        std::cerr << "ERROR: " << message << std::endl;
        throw std::runtime_error(message);
    }

    template <typename... Args>
    constexpr void WarnDebugExtraInfo(Args&&... args) {
        // TODO: fill
    }
}