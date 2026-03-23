//
// Created by pointerlost on 11/5/25.
//
#pragma once
#include "Types.h"
#include "Core/CMakeConfig.h"

constexpr auto ASSETS_SOURCE_DIR  = ASSETS_DIR "sources/";
constexpr auto ASSETS_RUNTIME_DIR = ASSETS_DIR "runtime/";

namespace Real {

    constexpr u32 MakeFourCC(char a, char b, char c, char d) noexcept {
        return  static_cast<u32>(a)
             | (static_cast<u32>(b) << 8)
             | (static_cast<u32>(c) << 16)
             | (static_cast<u32>(d) << 24);
    }
    //
    // template <typename T>
    // constexpr bool is_lValue(T&) {
    //     return true;
    // }
    //
    // template <typename T>
    // constexpr bool is_lValue(T&&) {
    //     return false;
    // }
}