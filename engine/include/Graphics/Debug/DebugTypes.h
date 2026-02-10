//
// Created by pointerlost on 2/9/26.
//
#pragma once
#include <cstdint>

#include "Math/Mat4.h"
#include "Math/Vec3.h"

namespace Real::graphics::debug {

    enum class DebugShapes : uint8_t {
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
        uint32_t indexCount{};
        uint32_t indexOffset{};
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
