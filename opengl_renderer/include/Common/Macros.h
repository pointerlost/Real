//
// Created by pointerlost on 11/5/25.
//
#pragma once
#include <cstdint>
#include "Core/CmakeConfig.h"

constexpr auto ASSETS_SOURCE_DIR  = ASSETS_DIR "sources/";
constexpr auto ASSETS_RUNTIME_DIR = ASSETS_DIR "runtime/";

namespace Real {

    constexpr uint32_t REAL_MAGIC = 0x4C414552; // Little endian

    constexpr uint32_t MakeFourCC(char a, char b, char c, char d) noexcept {
        return  static_cast<uint32_t>(a)
             | (static_cast<uint32_t>(b) << 8)
             | (static_cast<uint32_t>(c) << 16)
             | (static_cast<uint32_t>(d) << 24);
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