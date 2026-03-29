//
// Created by pointerlost on 3/28/26.
//
#pragma once
#include "Common/Types.h"

namespace Real::core {

    struct SlotHandle {
        u32 index      = std::numeric_limits<u32>::max(); // Where in the array (int with sentinel invalid val)
        u32 generation = 0;                               // Which "version" of that slot

        [[nodiscard]] bool IsNull() const {
            return index == std::numeric_limits<u32>::max();
        }
    };
}