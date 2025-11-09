//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include <string>
#include <vector>
#include "compressonator/include/cmp_compressonatorlib/compressonator.h"
#include "glad/glad.h"

namespace Real {
    enum class TextureType;
    enum class ImageCompressedType;
}

namespace Real { struct FileInfo; }

namespace Real::util {
    [[nodiscard]] std::vector<FileInfo> IterateDirectory(const std::string& folderPath);
    bool CheckSubStrExistsInString(const std::string& subStr, const std::string& string);

    [[nodiscard]] int FindClosestPowerOfTwo(int num);
    ImageCompressedType PickTextureCompressionType(TextureType type);
    GLenum ConvertChannelCountToGLType(int channelCount, const std::string& name = std::string());
    GLenum CompressTypeToGLEnum(ImageCompressedType type);
    std::string CompressTypeToString(ImageCompressedType type);
    TextureType StringToEnumTextureType(const std::string& type);
    std::string TextureTypeEnumToString(TextureType type);
    CMP_FORMAT GetCMPFormatWithCompressType(ImageCompressedType type);
    uint TexFormat_uncompressed_GetBitPerTexel(TextureType type);
    uint TexFormat_uncompressed_GetBytePerTexel(TextureType type);
    uint TexFormat_compressed_GetBytesPerBlock(ImageCompressedType type);

}
