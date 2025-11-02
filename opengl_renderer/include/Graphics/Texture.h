//
// Created by pointerlost on 10/6/25.
//
#pragma once
#include <glad/glad.h>
#include <string>

#include "Core/Utils.h"

namespace Real {

    enum class ImageFormatState {
        UNCOMPRESSED,
        COMPRESSED
    };

    enum class ImageCompressedType {
        BC1,
        BC6,
        BC7,
    };

    struct TextureData {
        void* m_Data;
        int m_Handle;
        int m_Width;
        int m_Height;
        int m_DataSize;
        int m_ChannelCount;
        int m_Format;
        int m_InternalFormat;
        ImageCompressedType m_ImageCompressType;
    };

    struct Texture {
        explicit Texture(ImageFormatState format = ImageFormatState::UNCOMPRESSED) { m_ImageFormatState = format; }
        explicit Texture(const Ref<TextureData>& data) : m_Data(*data) {}
        Texture(const Texture&) = default;
        TextureData m_Data{};
        ImageFormatState m_ImageFormatState = ImageFormatState::UNCOMPRESSED;

        int m_TexIndex = -1;
        int m_ArrayIndex = -1;
    };
}
