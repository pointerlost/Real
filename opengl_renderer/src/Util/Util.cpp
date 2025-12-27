//
// Created by pointerlost on 10/30/25.
//
#include "Util/Util.h"

#include <fstream>
#include <GL/glext.h>
#include <nlohmann/json.hpp>
#include "Core/file_manager.h"
#include "Core/Logger.h"
#include "Graphics/Texture.h"

namespace Real::util {

    bool IsSubString(const std::string &subStr, const std::string &string) {
        return string.find(subStr) != std::string::npos;
    }

    int TextureTypeToChannelCount(TextureType type) {
        switch (type) {
            case TextureType::ALBEDO:
            case TextureType::NORMAL:
            case TextureType::ORM:
            case TextureType::EMISSIVE:
            case TextureType::ALBEDO_ROUGHNESS:
            case TextureType::METALLIC_ROUGHNESS:
                return 4;

            case TextureType::AMBIENT_OCCLUSION:
            case TextureType::ROUGHNESS:
            case TextureType::METALLIC:
            case TextureType::HEIGHT:
            case TextureType::ALPHA:
                return 1;

            default: Warn("There is no channel count for this texture type: " + TextureType_EnumToString(type)); return 1;
        }
    }

    int GetGLFormat(int channelCount, bool srgb) {
        switch (channelCount) {
            case 1: return GL_R;
            case 2: return GL_RG;
            case 3: return srgb ? GL_SRGB : GL_RGB;
            case 4: return srgb ? GL_SRGB_ALPHA : GL_RGBA;

            default:
                Warn("There is no GLType for this channel count: " + std::to_string(channelCount));
                return GL_INVALID_ENUM;
        }
    }

    int GetGLInternalFormat(int channelCount, bool srgb) {
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
                Warn("[GetCompressedInternalFormat] Missing channel count Returning UNDEFINED Internal format");
                return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        }
    }

    std::string ImageFormatState_EnumToString(ImageFormatState state) {
        switch (state) {
            case ImageFormatState::COMPRESS_ME:  return "compress_me";
            case ImageFormatState::COMPRESSED:   return "compressed";
            case ImageFormatState::UNCOMPRESSED: return "uncompressed";
            case ImageFormatState::UNDEFINED:    return "undefined";
            case ImageFormatState::DEFAULT:      return "default";
            default: Warn("There is no image format state enum!!"); return "real_null";
        }
    }

    ImageFormatState ImageFormatState_StringToEnum(std::string state) {
        if (state == "compress_me")  return ImageFormatState::COMPRESS_ME;
        if (state == "compressed")   return ImageFormatState::COMPRESSED;
        if (state == "uncompressed") return ImageFormatState::UNCOMPRESSED;
        if (state == "undefined")    return ImageFormatState::UNDEFINED;
        if (state == "default")      return ImageFormatState::DEFAULT;
        return ImageFormatState::real_null;
    }

    std::string FormatToString(int format) {
        switch (format) {
            case GL_R:    return "GL_R";
            case GL_RG:   return "GL_RG";
            case GL_RGB:  return "GL_RGB";
            case GL_RGBA: return "GL_RGBA";

            default:
                return "GL_INVALID_FORMAT";
        }
    }

    std::string InternalFormatToString(int format) {
        switch (format) {
            case GL_COMPRESSED_RGBA_BPTC_UNORM: return "GL_COMPRESSED_RGBA_BPTC_UNORM";
            case GL_COMPRESSED_RED_RGTC1_EXT:   return "GL_COMPRESSED_RED_RGTC1_EXT";
            default: return std::to_string(format);
        }
    }

    CMP_FORMAT GetCMPDestinationFormat(int channelCount) {
        switch (channelCount) {
            case 1: return CMP_FORMAT_BC4;
            case 2: return CMP_FORMAT_BC5;

            case 3:
            case 4:
                return CMP_FORMAT_BC7;

            default: Warn("UNDEFINED CMP_FORMAT, for channelCount: " + std::to_string(channelCount));
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

    TextureType TextureType_StringToEnum(const std::string &type) {
        if (type == "albedo" || type == "ALB")        return TextureType::ALBEDO;
        if (type == "normal" || type == "NRM")        return TextureType::NORMAL;
        if (type == "orm" || type == "ORM")           return TextureType::ORM;
        if (type == "height" || type == "HEIGHT")     return TextureType::HEIGHT;
        if (type == "emissive" || type == "EMISSIVE") return TextureType::EMISSIVE;
        if (type == "ao" || type == "AO")             return TextureType::AMBIENT_OCCLUSION;
        if (type == "roughness" || type == "RGH")     return TextureType::ROUGHNESS;
        if (type == "metallic" || type == "MTL")      return TextureType::METALLIC;
        if (type == "albedo_roughness" || type == "ALB_RGH")   return TextureType::ALBEDO_ROUGHNESS;
        if (type == "alpha" || type == "ALPHA")                return TextureType::ALPHA;
        if (type == "metallic_roughness" || type == "MTL_RGH") return TextureType::METALLIC_ROUGHNESS;

        Info("[TextureType_StringToEnum] Type returning UNDEFINED for: " + type);
        return TextureType::UNDEFINED;
    }

    // This method returns the type as a suffix, not a full string
    std::string TextureType_EnumToString(TextureType type) {
        switch (type) {
            case TextureType::ALBEDO:             return "ALB";
            case TextureType::NORMAL:             return "NRM";
            case TextureType::ORM:                return "ORM";
            case TextureType::HEIGHT:             return "HEIGHT";
            case TextureType::EMISSIVE:           return "EMISSIVE";
            case TextureType::AMBIENT_OCCLUSION:  return "AO";
            case TextureType::ROUGHNESS:          return "RGH";
            case TextureType::METALLIC:           return "MTL";
            case TextureType::ALBEDO_ROUGHNESS:   return "ALB_RGH";
            case TextureType::ALPHA:              return "ALPHA";
            case TextureType::METALLIC_ROUGHNESS: return "MTL_RGH";

            default:
                Warn("[TextureType_EnumToString] Texture type returning UNDEFINED for: " + TextureType_EnumToString(type));
                return "UNDEFINED";
        }
    }

    std::string GetDefaultTextureName(TextureType type, int width) {
        return std::string("default_" + TextureType_EnumToString(type) + "_" + std::to_string(width));
    }

    // Bit Per Pixel
    uint TexFormat_uncompressed_GetBitPerTexel(TextureType type) {
        switch (type) {
            case TextureType::ALBEDO:
            case TextureType::ALBEDO_ROUGHNESS:
            case TextureType::NORMAL:
            case TextureType::ORM:
                return 32;

            case TextureType::ROUGHNESS:
            case TextureType::METALLIC:
            case TextureType::AMBIENT_OCCLUSION:
            case TextureType::HEIGHT:
            case TextureType::EMISSIVE:
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

    TextureType AssimpTextureTypeToRealType(const aiTextureType type) {
        switch (type) {
            case aiTextureType_BASE_COLOR:
            case aiTextureType_DIFFUSE:
                return TextureType::ALBEDO;

            case aiTextureType_NORMAL_CAMERA:
            case aiTextureType_NORMALS:
                return TextureType::NORMAL;

            case aiTextureType_DISPLACEMENT:
            case aiTextureType_HEIGHT:
                return TextureType::HEIGHT;

            case aiTextureType_AMBIENT_OCCLUSION:       return TextureType::AMBIENT_OCCLUSION;
            case aiTextureType_METALNESS:               return TextureType::METALLIC;
            case aiTextureType_EMISSIVE:                return TextureType::EMISSIVE;
            case aiTextureType_DIFFUSE_ROUGHNESS:       return TextureType::ALBEDO_ROUGHNESS;
            case aiTextureType_GLTF_METALLIC_ROUGHNESS: return TextureType::METALLIC_ROUGHNESS;

            default: return TextureType::UNDEFINED;
        }
    }

    bool TryParseUUID(const std::string& strUUID, UUID &uuid) {
        try {
            uuid = UUID(std::stoul(strUUID));
            return true;
        } catch (...) {
            return false;
        }
    }

    TextureData ExtractChannel(const TextureData& data, int channelIndex) {
        if (channelIndex < 0 || channelIndex >= data.m_ChannelCount) {
            Warn("[Util::ExtractChannel] Invalid channel index! Fix it!");
            return{};
        }
        TextureData d;
        d.m_Width          = data.m_Width;
        d.m_Height         = data.m_Height;
        d.m_ChannelCount   = 1; // We are extracting '1' channel
        d.m_DataSize       = data.m_Width * data.m_Height * 1;
        d.m_Format         = GetGLFormat(d.m_ChannelCount);
        d.m_InternalFormat = GetGLInternalFormat(d.m_ChannelCount);

        d.m_Data = new uint8_t[d.m_DataSize];

        const auto* src = static_cast<const uint8_t*>(data.m_Data);
        auto* dst = static_cast<uint8_t*>(d.m_Data);

        for (int i = 0; i < data.m_Width * data.m_Height; i++)
            dst[i] = src[i * data.m_ChannelCount + channelIndex];

        return d;
    }

    TextureData ExtractChannels(const TextureData& data, const std::vector<int> &wantedChannels) {
        const int outC = wantedChannels.size();

        TextureData d;
        d.m_Width          = data.m_Width;
        d.m_Height         = data.m_Height;
        d.m_ChannelCount   = outC;
        d.m_DataSize       = data.m_Width * data.m_Height * outC;
        d.m_Format         = GetGLFormat(d.m_ChannelCount);
        d.m_InternalFormat = GetGLInternalFormat(d.m_ChannelCount);

        d.m_Data = new uint8_t[d.m_DataSize];

        const auto* src = static_cast<const uint8_t*>(data.m_Data);
        auto* dst = static_cast<uint8_t*>(d.m_Data);

        for (int i = 0; i < data.m_Width * data.m_Height; i++)
            for (int j = 0; j < outC; j++)
                dst[i * outC + j] = src[i * data.m_ChannelCount + wantedChannels[j]];

        return d;
    }
}
