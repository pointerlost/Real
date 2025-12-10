//
// Created by pointerlost on 12/4/25.
//
#pragma once

namespace Real {

    enum class BufferType {
        SSBO,
        UBO,
    };

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
}