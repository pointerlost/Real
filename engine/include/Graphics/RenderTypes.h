//
// Created by pointerlost on 2/21/26.
//
#pragma once
#include "Common/RealTypes.h"

namespace Real::graphics {

    enum class ClearFlags : u8 {
        None    = 0,
        Color   = 1 << 0,
        Depth   = 1 << 1,
        Stencil = 1 << 2
    };

    struct Color {
        f32 r = 0.f;
        f32 g = 0.f;
        f32 b = 0.f;
        f32 a = 1.f;
    };

    inline ClearFlags operator|(ClearFlags a, ClearFlags b) {
        return static_cast<ClearFlags>(static_cast<u8>(a) | static_cast<u8>(b));
    }

    inline bool HasFlag(ClearFlags value, ClearFlags flag) {
        return (static_cast<u8>(value) & static_cast<u8>(flag)) != 0;
    }

    using TextureHandle  = uint32_t;
    using ShaderHandle   = uint32_t;
    using BufferHandle   = uint32_t;
    using BindingPoint   = uint32_t;
    using BindlessHandle = uint64_t;
}
