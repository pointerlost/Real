//
// Created by pointerlost on 2/9/26.
//
#pragma once
#include "Math/Mat4.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"

namespace Real::graphics::debug {

    enum class DebugShapes : u8 {
        Line,
        Box,
        Sphere,
        Cylinder,
        Capsule,
        Count,
    };

    constexpr DebugShapes AllDebugShapes[] = {
        DebugShapes::Line,
        DebugShapes::Box,
        DebugShapes::Cylinder,
        DebugShapes::Sphere
    };

    struct DebugMesh {
        u32 indexCount{};
        u32 indexOffset{};
    };

    struct DebugVertex {
        math::Vec3 position;
    };

    struct DebugInstance {
        math::Mat4 model;
        math::Vec4 color;
        DebugShapes shape = DebugShapes::Box; // CPU-only
    };
}
