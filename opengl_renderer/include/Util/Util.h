//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include <string>
#include <vector>
#include "compressonator/include/cmp_compressonatorlib/compressonator.h"
#include "glad/glad.h"
#include <assimp/material.h>
#include <nlohmann/json_fwd.hpp>
#include "Graphics/Texture.h"

namespace Real {
    struct TextureData;
}

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
    FileInfo CreateFileInfoFromPath(const std::string& path);

    [[nodiscard]] int FindClosestPowerOfTwo(int num);
    int ConvertChannelCountToGLFormat(int channelCount, const std::string& name = std::string(), bool srgb = false);
    int ConvertChannelCountToGLInternalFormat(int channelCount, bool srgb = false);
    int GetCompressedInternalFormat(int channelCount);
    std::string ImageFormatState_EnumToString(ImageFormatState state);
    ImageFormatState ImageFormatState_StringToEnum(std::string state);
    std::string FormatToString(int format);
    std::string InternalFormatToString(int format);
    CMP_FORMAT GetCMPDestinationFormat(int channelCount);
    std::string DebugCMPStatus(CMP_ERROR error);
    void DebugGLError();
    TextureType TextureType_StringToEnum(const std::string& type);
    std::string TextureType_EnumToString(TextureType type);
    std::string GetDefaultTextureName(TextureType type, int width);
    uint TexFormat_uncompressed_GetBitPerTexel(TextureType type);
    uint TexFormat_uncompressed_GetBytePerTexel(TextureType type);
    GLenum TextureWrapModeToGLEnum(TextureWrapMode mode);
    GLenum TextureFilterModeToGLEnum(TextureFilterMode filter);

    TextureType AssimpTextureTypeToRealType(aiTextureType type);

    TextureData ExtractChannel(TextureData data, int channelIndex);
    TextureData ExtractChannel(void* data, int width, int height, int channels, int channelIndex);
    TextureData ExtractChannels(TextureData data, const std::vector<int>& wantedChannels);
    TextureData ExtractChannels(void* data, int width, int height, int channels, const std::vector<int>& wantedChannels);

    nlohmann::json LoadJSON(const std::string& path);
}
