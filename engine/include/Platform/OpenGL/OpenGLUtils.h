//
// Created by pointerlost on 3/23/26.
//
#pragma once


namespace Real::graphics {
    enum class TextureFilterMode;
    enum class TextureWrapMode;
    enum class ClearFlags : unsigned char;
}



namespace Real::util::opengl {
    int  TextureWrapModeToGLEnum(graphics::TextureWrapMode mode);
    int  TextureFilterModeToGLEnum(graphics::TextureFilterMode filter);
    int  GetGLFormat(int channelCount, bool srgb = false);
    int  GetGLInternalFormat(int channelCount, bool srgb = false);
    int  GetCompressedInternalFormat(int channelCount);
    void DebugGLError();
}
