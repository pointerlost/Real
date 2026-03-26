//
// Created by pointerlost on 2/21/26.
//
#pragma once
#include "Math/Vec2.h"
#include "Math/Vec3.h"

namespace Real::graphics {

    // Flags
    enum class ClearFlags : u8 {
        None    = 0,
        Color   = 1 << 0,
        Depth   = 1 << 1,
        Stencil = 1 << 2
    };

    inline ClearFlags operator|(ClearFlags a, ClearFlags b) {
        return static_cast<ClearFlags>(static_cast<u8>(a) | static_cast<u8>(b));
    }
    inline bool HasFlag(ClearFlags value, ClearFlags flag) {
        return (static_cast<u8>(value) & static_cast<u8>(flag)) != 0;
    }

    // Geometry
    struct Vertex {
        math::Vec3 position;
        math::Vec3 normal;
        math::Vec2 UV;
    };

    // Color
    struct Color {
        f32 r = 0.f, g = 0.f, b = 0.f, a = 1.f;
    };

    struct TextureData {
        void* data           = nullptr;
        int   width          = 0;
        int   height         = 0;
        int   channelCount   = 0;
        int   dataSize       = 0;
        int   format         = 0;
        int   internalFormat = 0;
    };

    // Typed handles - prevent accidental mixing
    template<typename Tag>
    struct Handle {
        u32 value = 0;

        explicit Handle(u32 val) : value(val) {}
                 Handle()              = default;
                 Handle(const Handle&) = default;

        [[nodiscard]] bool IsValid() const { return value != 0; }

        Handle& operator=(const Handle& other) {
            value = other.value;
            return *this;
        }

        bool operator==(const Handle& other) const { return value == other.value; }
        bool operator!=(const Handle& other) const { return value != other.value; }
        // Comparison with number like: (handle != 0)
        bool operator==(u32 raw) const { return value == raw; }
        bool operator!=(u32 raw) const { return value != raw; }
    };

    using TextureHandle  = Handle<struct TextureTag>;
    using ShaderHandle   = Handle<struct ShaderTag>;
    using BufferHandle   = Handle<struct BufferTag>;
    using BindingPoint   = Handle<struct BindingTag>;

    using BindlessHandle = u64; // stays flat, GL-specific
    constexpr u64 InvalidBindlessHandle = 0;

    // What stage of the pipeline - technical GPU concept
    enum class ShaderStage {
        Vertex,
        Fragment,
        Program, // linked program, not a stage - used only for error checking
    };

    // Shader types
    enum class ShaderType {
        Main,
        Debug,
    };

    enum class PrimitiveType {
        Triangle,
        Cube,
        Sphere,
    };

    struct FrameConfig {
        Color      clearColor = { 0, 0, 0, 1 };
        ClearFlags clearFlags = ClearFlags::Color | ClearFlags::Depth;
    };

}