//
// Created by pointerlost on 3/29/26.
//
#pragma once
#include "Common/Types.h"

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

    enum class TextureType {
        ALBEDO,
        NORMAL,
        AMBIENT_OCCLUSION,
        ROUGHNESS,
        METALLIC,
        ORM,
        HEIGHT,
        EMISSIVE,
        ALPHA,
        ALBEDO_ROUGHNESS,
        METALLIC_ROUGHNESS,
        UNDEFINED,
    };

    enum class ImageFormatState {
        UNCOMPRESSED,
        COMPRESSED,
        COMPRESS_ME,
        DEFAULT,
        UNDEFINED,
        real_null,
    };

    enum class TextureFilterMode {
        NEAREST,
        LINEAR,
    };

    enum class TextureWrapMode {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER
    };

    enum class BufferType {
        SSBO,
        UBO,
    };

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

}
