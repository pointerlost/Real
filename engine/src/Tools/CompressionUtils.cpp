//
// Created by pointerlost on 3/23/26.
//
#include "Tools/CompressionUtils.h"
#include "Common/RealEnum.h"
#include "Core/Logger.h"

namespace Real::util::compression {

    CMP_FORMAT GetCMPDestinationFormat(int channelCount) {
        switch (channelCount) {
            case 1: return CMP_FORMAT_BC4;
            case 2: return CMP_FORMAT_BC5;
            case 3:
            case 4: return CMP_FORMAT_BC7;

            default: {
                Warn("Undefined CMP_FORMAT for channelCount: " + std::to_string(channelCount));
                return CMP_FORMAT_BC1;
            }
        }
    }

    String DebugCMPStatus(CMP_ERROR error) {
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

            default: return "UNKNOWN ERROR";
        }
    }

    String ImageFormatState_EnumToString(ImageFormatState state) {
        switch (state) {
            case ImageFormatState::COMPRESS_ME:  return "compress_me";
            case ImageFormatState::COMPRESSED:   return "compressed";
            case ImageFormatState::UNCOMPRESSED: return "uncompressed";
            case ImageFormatState::UNDEFINED:    return "undefined";
            case ImageFormatState::DEFAULT:      return "default";

            default: Warn("No image format state enum!"); return "real_null";
        }
    }

    ImageFormatState ImageFormatState_StringToEnum(const String& state) {
        if (state == "compress_me")  return ImageFormatState::COMPRESS_ME;
        if (state == "compressed")   return ImageFormatState::COMPRESSED;
        if (state == "uncompressed") return ImageFormatState::UNCOMPRESSED;
        if (state == "undefined")    return ImageFormatState::UNDEFINED;
        if (state == "default")      return ImageFormatState::DEFAULT;

        return ImageFormatState::real_null;
    }

}
