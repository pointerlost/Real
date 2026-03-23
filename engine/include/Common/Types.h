//
// Created by pointerlost on 12/6/25.
//
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace Real {
    using i8  = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using u8  = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using f32 = float;
    using f64 = double;

    template<typename T>
    using Vector = std::vector<T>;
    using String = std::string;
}
