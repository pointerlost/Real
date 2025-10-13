//
// Created by pointerlost on 10/3/25.
//
#pragma once
#include <iostream>
#include <format>

namespace Real {

    template <typename T>
    constexpr void Info(const T& message) {
        std::cout << "INFO: " << message << "\n";
    }

    // inline void InfoFmt(const std::string& message) {
    //     printf(std::format(__FILE__ , message).c_str());
    // }

    template <typename T>
    constexpr void Warn(const T& message) {
        std::cout << "WARNING: " << message << "\n";
    }

    template <typename T>
    constexpr void Error(const T& message) {
        std::cerr << "ERROR: " << message << "\n";
        throw std::runtime_error(message);
    }
}