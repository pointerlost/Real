//
// Created by pointerlost on 3/28/26.
//
#pragma once
#include "Common/Types.h"
#include <limits>

namespace Real::core {

    struct SlotHandle {
        u32 index      = std::numeric_limits<u32>::max(); // Where in the array (int with sentinel invalid val)
        u32 generation = 0;                               // Which "version" of that slot

        [[nodiscard]] bool IsNull() const {
            return index == std::numeric_limits<u32>::max();
        }

        bool operator==(const SlotHandle& o) const { return index == o.index && generation == o.generation; }
        bool operator!=(const SlotHandle& o) const { return !(*this == o); }
    };
}

namespace std {
    template<>
    struct hash<Real::core::SlotHandle> {
        size_t operator()(const Real::core::SlotHandle& h) const noexcept {
            return std::hash<Real::u32>{}(h.index) ^ std::hash<Real::u32>{}(h.generation);
        }
    };
}