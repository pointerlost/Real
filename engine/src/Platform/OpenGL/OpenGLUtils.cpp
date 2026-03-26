//
// Created by pointerlost on 3/23/26.
//
#include "Platform/OpenGL/OpenGLUtils.h"
#include "glad/glad.h"
#include <GL/glext.h>
#include "Common/RealEnum.h"
#include "Common/Types.h"
#include "Core/Logger.h"

namespace Real::util::opengl {

    int GetGLFormat(int channelCount, bool srgb) {
        switch (channelCount) {
            case 1: return GL_R;
            case 2: return GL_RG;
            case 3: return srgb ? GL_SRGB : GL_RGB;
            case 4: return srgb ? GL_SRGB_ALPHA : GL_RGBA;

            default: {
                Warn("There is no GLType for this channel count: " + std::to_string(channelCount));
                return GL_INVALID_ENUM;
            }
        }
    }

    int GetGLInternalFormat(int channelCount, bool srgb) {
        switch (channelCount) {
            case 1: return GL_R8;
            case 2: return GL_RG8;
            case 3: return srgb ? GL_SRGB8 : GL_RGB8;
            case 4: return srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;

            default: {
                Warn("There is no Internal format for this channel count!");
                return GL_INVALID_ENUM;
            }
        }
    }

    int GetCompressedInternalFormat(int channelCount) {
        switch (channelCount) {
            case 3:
            case 4: return GL_COMPRESSED_RGBA_BPTC_UNORM;
            case 1: return GL_COMPRESSED_RED_RGTC1_EXT;

            default: {
                Warn("[GetCompressedInternalFormat] Missing channel count, returning undefined");
                return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            }
        }
    }

    int TextureWrapModeToGLEnum(TextureWrapMode mode) {
        switch (mode) {
            case TextureWrapMode::REPEAT:          return GL_REPEAT;
            case TextureWrapMode::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
            case TextureWrapMode::CLAMP_TO_EDGE:   return GL_CLAMP_TO_EDGE;
            case TextureWrapMode::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;

            default: {
                Warn("There is no wrapping mode for this type!");
                return GL_REPEAT;
            }
        }
    }

    int TextureFilterModeToGLEnum(TextureFilterMode filter) {
        switch (filter) {
            case TextureFilterMode::LINEAR:  return GL_LINEAR;
            case TextureFilterMode::NEAREST: return GL_NEAREST;

            default: return GL_REPEAT;
        }
    }

    String FormatToString(int format) {
        switch (format) {
            case GL_R:    return "GL_R";
            case GL_RG:   return "GL_RG";
            case GL_RGB:  return "GL_RGB";
            case GL_RGBA: return "GL_RGBA";

            default:      return "GL_INVALID_FORMAT";
        }
    }

    String InternalFormatToString(int format) {
        switch (format) {
            case GL_COMPRESSED_RGBA_BPTC_UNORM: return "GL_COMPRESSED_RGBA_BPTC_UNORM";
            case GL_COMPRESSED_RED_RGTC1_EXT:   return "GL_COMPRESSED_RED_RGTC1_EXT";

            default: return std::to_string(format);
        }
    }

    void DebugGLError() {
        switch (const GLenum err = glGetError(); err) {
            case GL_NO_ERROR:                      Info("No GL error");                      break;
            case GL_INVALID_ENUM:                  Warn("GL_INVALID_ENUM");                  break;
            case GL_INVALID_VALUE:                 Warn("GL_INVALID_VALUE");                 break;
            case GL_INVALID_OPERATION:             Warn("GL_INVALID_OPERATION");             break;
            case GL_INVALID_INDEX:                 Warn("GL_INVALID_INDEX");                 break;
            case GL_STACK_OVERFLOW:                Warn("GL_STACK_OVERFLOW");                break;
            case GL_STACK_UNDERFLOW:               Warn("GL_STACK_UNDERFLOW");               break;
            case GL_OUT_OF_MEMORY:                 Warn("GL_OUT_OF_MEMORY");                 break;
            case GL_CONTEXT_LOST:                  Warn("GL_CONTEXT_LOST");                  break;
            case GL_TABLE_TOO_LARGE:               Warn("GL_TABLE_TOO_LARGE");               break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: Warn("GL_INVALID_FRAMEBUFFER_OPERATION"); break;
            default: ;
        }
    }

}
