//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include <string>
#include <vector>
#include "compressonator/include/cmp_compressonatorlib/compressonator.h"
#include "glad/glad.h"

#define glCheckError() glCheckError_(__FILE__, __LINE__)

namespace Real {
    enum class TextureResolution;
}

namespace Real {
    enum class TextureFilterMode;
}

namespace Real {
    enum class TextureWrapMode;
}

namespace Real {
    enum class TextureType;
    enum class ImageCompressedType;
}

namespace Real { struct FileInfo; }

namespace Real::util {
    [[nodiscard]] std::vector<FileInfo> IterateDirectory(const std::string& folderPath);
    bool IsSubString(const std::string& subStr, const std::string& string);

    [[nodiscard]] int FindClosestPowerOfTwo(int num);
    ImageCompressedType PickTextureCompressionType(TextureType type);
    GLenum ConvertChannelCountToGLType(int channelCount, const std::string& name = std::string());
    GLenum ImageCompressTypeToGLEnum(ImageCompressedType type);
    std::string CompressTypeToString(ImageCompressedType type);
    TextureType StringToEnum_TextureType(const std::string& type);
    std::string EnumToString_TextureType(TextureType type);
    CMP_FORMAT GetCMPFormat(ImageCompressedType type);
    std::string GetDefaultTextureName(TextureType type, int width);
    uint TexFormat_uncompressed_GetBitPerTexel(TextureType type);
    uint TexFormat_uncompressed_GetBytePerTexel(TextureType type);
    uint TexFormat_compressed_GetBytesPerBlock(ImageCompressedType type);
    GLenum TextureWrapModeToGLEnum(TextureWrapMode mode);
    GLenum TextureFilterModeToGLEnum(TextureFilterMode filter);
    GLenum glCheckError_(const char *file, int line);

}
