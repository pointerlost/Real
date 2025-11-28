//
// Created by pointerlost on 10/30/25.
//
#include "Util/Util.h"

#include <GL/glext.h>

#include "Core/file_manager.h"
#include "Core/Logger.h"
#include "Graphics/Texture.h"

namespace Real::util {

    std::vector<FileInfo> IterateDirectory(const std::string &folderPath) {
        std::vector<FileInfo> files;
        namespace fs = std::filesystem;

        if (!File::Exists(folderPath)) {
            Warn("Folder doesn't exists: " + folderPath);
            return {};
        }

        for (auto &p : fs::recursive_directory_iterator(folderPath)) {
            if (p.path().has_extension()) {
                FileInfo fileInfo{};
                fileInfo.name = p.path().filename();
                fileInfo.ext  = p.path().extension();
                fileInfo.stem = p.path().stem();
                fileInfo.path = p.path();
                files.emplace_back(fileInfo);
            }
        }

        return files;
    }

    bool IsSubString(const std::string &subStr, const std::string &string) {
        return string.find(subStr) != std::string::npos;
    }

    int FindClosestPowerOfTwo(int num) {
        int x = 1, y = 0;
        while (x < num) {
            y = x;
            x <<= 1;
        }
        return abs(y - num) > abs(x - num) ? x : y;
    }

    int ConvertChannelCountToGLFormat(int channelCount, const std::string& name, bool srgb) {
        switch (channelCount) {
            case 1: return GL_R;
            case 2: return GL_RG;
            case 3: return srgb ? GL_SRGB : GL_RGB;
            case 4: return srgb ? GL_SRGB_ALPHA : GL_RGBA;

            default:
                Warn("There is no GLType for this channel count, tex name: " + name);
                return GL_INVALID_ENUM;
        }
    }

    int ConvertChannelCountToGLInternalFormat(int channelCount, bool srgb) {
        switch (channelCount) {
            case 1: return GL_R8;
            case 2: return GL_RG8;
            case 3: return srgb ? GL_SRGB8 : GL_RGB8;
            case 4: return srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;

            default:
                Warn("There is no Internal format for this channel count!");
                return GL_INVALID_ENUM;
        }
    }

    int GetCompressedInternalFormat(int channelCount) {
        // TODO: when we add HDR Skyboxes match to BC6
        switch (channelCount) {
            case 4:
            case 3: return GL_COMPRESSED_RGBA_BPTC_UNORM;
            case 1: return GL_COMPRESSED_RED_RGTC1_EXT;
            default:
                Warn("Missing channel count Returning UNDEFINED Internal format");
                return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        }
    }

    std::string InternalFormatToString(int format) {
        switch (format) {
            case GL_COMPRESSED_RGBA_BPTC_UNORM: return "GL_COMPRESSED_RGBA_BPTC_UNORM";
            case GL_COMPRESSED_RED_RGTC1_EXT: return "GL_COMPRESSED_RED_RGTC1_EXT";
            default: return std::to_string(format);
        }
    }

    CMP_FORMAT GetCMPDestinationFormat(int channelCount) {
        switch (channelCount) {
            case 1: return CMP_FORMAT_BC4;
            case 2: return CMP_FORMAT_BC5;
            case 3: case 4: return CMP_FORMAT_BC7;
            default:
                Warn("UNDEFINED CMP_FORMAT, for channelCount: " + std::to_string(channelCount));
                return CMP_FORMAT_BC1;
        }
    }

    std::string DebugCMPStatus(CMP_ERROR error) {
        switch (error) {
            case CMP_ERR_CMP_DESTINATION:              return "CMP_ERR_CMP_DESTINATION";
            case CMP_ERR_FAILED_HOST_SETUP:            return "CMP_ERR_FAILED_HOST_SETUP";
            case CMP_ERR_GAMMA_OUTOFRANGE:             return "CMP_ERR_GAMMA_OUTOFRANGE";
            case CMP_ERR_GENERIC:                      return "CMP_ERR_GENERIC";
            case CMP_ERR_GPU_DOESNOT_SUPPORT_CMP_EXT:  return "CMP_ERR_GPU_DOESNOT_SUPPORT_CMP_EXT";
            case CMP_ERR_GPU_DOESNOT_SUPPORT_COMPUTE:  return "CMP_ERR_GPU_DOESNOT_SUPPORT_COMPUTE";
            case CMP_ERR_INVALID_DEST_TEXTURE:         return "CMP_ERR_INVALID_DEST_TEXTURE";
            case CMP_ERR_INVALID_SOURCE_TEXTURE:       return "CMP_ERR_INVALID_SOURCE_TEXTURE";
            case CMP_ERR_MEM_ALLOC_FOR_MIPSET:         return "CMP_ERR_MEM_ALLOC_FOR_MIPSET";
            case CMP_ERR_NOPERFSTATS:                  return "CMP_ERR_NOPERFSTATS";
            case CMP_ERR_PLUGIN_FILE_NOT_FOUND:        return "CMP_ERR_PLUGIN_FILE_NOT_FOUND";
            case CMP_ERR_NOSHADER_CODE_DEFINED:        return "CMP_ERR_NOSHADER_CODE_DEFINED";
            case CMP_ERR_PLUGIN_SHAREDIO_NOT_SET:      return "CMP_ERR_PLUGIN_SHAREDIO_NOT_SET";
            case CMP_ERR_SIZE_MISMATCH:                return "CMP_ERR_SIZE_MISMATCH";
            case CMP_ERR_UNABLE_TO_CREATE_ENCODER:     return "CMP_ERR_UNABLE_TO_CREATE_ENCODER";
            case CMP_ERR_UNABLE_TO_INIT_CODEC:         return "CMP_ERR_UNABLE_TO_INIT_CODEC";
            case CMP_ERR_UNABLE_TO_INIT_COMPUTELIB:    return "CMP_ERR_UNABLE_TO_INIT_COMPUTELIB";
            case CMP_ERR_UNABLE_TO_INIT_D3DX:          return "CMP_ERR_UNABLE_TO_INIT_D3DX";
            case CMP_ERR_UNABLE_TO_INIT_DECOMPRESSLIB: return "CMP_ERR_UNABLE_TO_INIT_DECOMPRESSLIB";
            case CMP_ERR_UNABLE_TO_LOAD_ENCODER:       return "CMP_ERR_UNABLE_TO_LOAD_ENCODER";
            case CMP_ERR_UNABLE_TO_LOAD_FILE:          return "CMP_ERR_UNABLE_TO_LOAD_FILE";
            case CMP_ERR_UNKNOWN_DESTINATION_FORMAT:   return "CMP_ERR_UNKNOWN_DESTINATION_FORMAT";
            case CMP_ERR_UNSUPPORTED_DEST_FORMAT:      return "CMP_ERR_UNSUPPORTED_DEST_FORMAT";
            case CMP_ERR_UNSUPPORTED_GPU_ASTC_DECODE:  return "CMP_ERR_UNSUPPORTED_GPU_ASTC_DECODE";
            case CMP_ERR_UNSUPPORTED_GPU_BASIS_DECODE: return "CMP_ERR_UNSUPPORTED_GPU_BASIS_DECODE";
            case CMP_ERR_UNSUPPORTED_SOURCE_FORMAT:    return "CMP_ERR_UNSUPPORTED_SOURCE_FORMAT";

            default: return "UNKNOWN ERROR!";
        }
    }

    void DebugGLError() {
        switch (const GLenum err = glGetError(); err) {
            case GL_NO_ERROR:          Info("There is no GL error!"); break;
            case GL_INVALID_ENUM:      Warn("GL_INVALID_ENUM");       break;
            case GL_INVALID_VALUE:     Warn("GL_INVALID_VALUE");      break;
            case GL_INVALID_OPERATION: Warn("GL_INVALID_OPERATION");  break;
            case GL_INVALID_INDEX:     Warn("GL_INVALID_INDEX");      break;
            case GL_STACK_OVERFLOW:    Warn("GL_STACK_OVERFLOW");     break;
            case GL_STACK_UNDERFLOW:   Warn("GL_STACK_UNDERFLOW");    break;
            case GL_OUT_OF_MEMORY:     Warn("GL_OUT_OF_MEMORY");      break;
            case GL_CONTEXT_LOST:      Warn("GL_CONTEXT_LOST");       break;
            case GL_TABLE_TOO_LARGE:   Warn("GL_TABLE_TOO_LARGE");    break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: Warn("GL_INVALID_FRAMEBUFFER_OPERATION"); break;

            default: ;
        }
    }

    TextureType StringToEnum_TextureType(const std::string &type) {
        if (type == "ALB")    return TextureType::ALB;
        if (type == "NRM")    return TextureType::NRM;
        if (type == "RMA")    return TextureType::RMA;
        if (type == "RGH")    return TextureType::RGH;
        if (type == "MTL")    return TextureType::MTL;
        if (type == "AO")     return TextureType::AO;
        if (type == "HEIGHT") return TextureType::HEIGHT;

        Info("Type returning UNDEFINED for: " + type);
        return TextureType::UNDEFINED;
    }

    std::string EnumToString_TextureType(TextureType type) {
        switch (type) {
            case TextureType::ALB:    return "ALB";
            case TextureType::NRM:    return "NRM";
            case TextureType::RGH:    return "RGH";
            case TextureType::MTL:    return "MTL";
            case TextureType::AO:     return "AO";
            case TextureType::HEIGHT: return "HEIGHT";
            case TextureType::RMA:    return "RMA";

            default:
                Warn("Texture type returning UNDEFINED for: " + EnumToString_TextureType(type));
                return "UNDEFINED";
        }
    }

    std::string GetDefaultTextureName(TextureType type, int width) {
        return std::string("default_" + EnumToString_TextureType(type) + "_" + std::to_string(width));
    }

    // Bit Per Pixel
    uint TexFormat_uncompressed_GetBitPerTexel(TextureType type) {
        switch (type) {
            case TextureType::ALB:
            case TextureType::NRM:
            case TextureType::RMA:
                return 32;

            case TextureType::RGH:
            case TextureType::MTL:
            case TextureType::AO:
            case TextureType::HEIGHT:
                return 8;

            default: // TextureType::UNDEFINED
                return 1;
        }
    }

    uint TexFormat_uncompressed_GetBytePerTexel(TextureType type) {
        return TexFormat_uncompressed_GetBitPerTexel(type) / 8;
    }

    GLenum TextureWrapModeToGLEnum(TextureWrapMode mode) {
        switch (mode) {
            case TextureWrapMode::REPEAT:          return GL_REPEAT;
            case TextureWrapMode::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
            case TextureWrapMode::CLAMP_TO_EDGE:   return GL_CLAMP_TO_EDGE;
            case TextureWrapMode::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;

            default:
                Warn("There is no wrapping mode for this type!");
                return GL_REPEAT;
        }
    }

    GLenum TextureFilterModeToGLEnum(TextureFilterMode filter) {
        switch (filter) {
            case TextureFilterMode::LINEAR:  return GL_LINEAR;
            case TextureFilterMode::NEAREST: return GL_NEAREST;
            default: return GL_REPEAT;
        }
    }

}
