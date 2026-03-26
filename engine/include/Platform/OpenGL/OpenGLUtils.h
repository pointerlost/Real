//
// Created by pointerlost on 3/23/26.
//
#pragma once

namespace Real {
    namespace graphics {
        enum class ClearFlags : unsigned char;
    }

    enum class TextureFilterMode;
    enum class TextureWrapMode;
}

namespace Real::util::opengl {
    int  TextureWrapModeToGLEnum(TextureWrapMode mode);
    int  TextureFilterModeToGLEnum(TextureFilterMode filter);
    int  GetGLFormat(int channelCount, bool srgb = false);
    int  GetGLInternalFormat(int channelCount, bool srgb = false);
    int  GetCompressedInternalFormat(int channelCount);
    void DebugGLError();
}
