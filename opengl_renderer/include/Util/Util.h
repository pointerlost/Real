//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include <string>
#include <vector>
#include "compressonator/include/cmp_compressonatorlib/compressonator.h"
#include "glad/glad.h"
#include "Graphics/Texture.h"

namespace Real {
    enum class TextureFilterMode;
    enum class TextureResolution;
    enum class TextureType;
    enum class TextureWrapMode;
}

namespace Real { struct FileInfo; }

namespace Real::util {
    [[nodiscard]] std::vector<FileInfo> IterateDirectory(const std::string& folderPath);
    bool IsSubString(const std::string& subStr, const std::string& string);

    [[nodiscard]] int FindClosestPowerOfTwo(int num);
    int ConvertChannelCountToGLFormat(int channelCount, const std::string& name = std::string(), bool srgb = false);
    int ConvertChannelCountToGLInternalFormat(int channelCount, bool srgb = false);
    int GetCompressedInternalFormat(int channelCount);
    std::string FormatToString(int format);
    std::string InternalFormatToString(int format);
    CMP_FORMAT GetCMPDestinationFormat(int channelCount);
    std::string DebugCMPStatus(CMP_ERROR error);
    void DebugGLError();
    TextureType StringToEnum_TextureType(const std::string& type);
    std::string EnumToString_TextureType(TextureType type);
    std::string GetDefaultTextureName(TextureType type, int width);
    uint TexFormat_uncompressed_GetBitPerTexel(TextureType type);
    uint TexFormat_uncompressed_GetBytePerTexel(TextureType type);
    GLenum TextureWrapModeToGLEnum(TextureWrapMode mode);
    GLenum TextureFilterModeToGLEnum(TextureFilterMode filter);
}
