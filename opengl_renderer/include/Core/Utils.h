//
// Created by pointerlost on 10/3/25.
//
#pragma once
#include <iostream>
#include <format>
#include <memory>

namespace Real {

    template <typename T>
    using Scope = std::unique_ptr<T>;
    template <typename T, typename ...Args>
    constexpr Scope<T> CreateScope(Args&& ... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    using Ref = std::shared_ptr<T>;
    template <typename T, typename ...Args>
    constexpr Ref<T> CreateRef(Args&& ... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    /* Concatenate everything into a string */
    template <typename... Args>
    std::string ConcatStr(Args... args) {
        return (static_cast<std::string>(args) + ...);
    }

    template <typename... Args>
    void ToStringAndRead(Args... args) {
        std::cout << (std::to_string(args) + ...) << "\n";
    }
}