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
    /* ***************************************** TEXTURE STATE  ************************************** */
    std::string ImageFormatState_EnumToString(ImageFormatState state);
    std::string TextureType_EnumToString(TextureType type);
    std::string GetDefaultTextureName(TextureType type, int width);
    std::string FormatToString(int format);
    std::string InternalFormatToString(int format);
    ImageFormatState ImageFormatState_StringToEnum(std::string state);
    CMP_FORMAT GetCMPDestinationFormat(int channelCount);
    TextureType TextureType_StringToEnum(const std::string& type);
    uint TexFormat_uncompressed_GetBitPerTexel(TextureType type);
    uint TexFormat_uncompressed_GetBytePerTexel(TextureType type);
    GLenum TextureWrapModeToGLEnum(TextureWrapMode mode);
    GLenum TextureFilterModeToGLEnum(TextureFilterMode filter);
    TextureType AssimpTextureTypeToRealType(aiTextureType type);
    int TextureTypeToChannelCount(TextureType type);
    int GetGLFormat(int channelCount, bool srgb = false);
    int GetCompressedInternalFormat(int channelCount);
    int GetGLInternalFormat(int channelCount, bool srgb = false);
    TextureData ExtractChannel(const TextureData& data, int channelIndex);
    TextureData ExtractChannel(void* data, int width, int height, int channels, int channelIndex);
    TextureData ExtractChannels(const TextureData& data, const std::vector<int>& wantedChannels);
    TextureData ExtractChannels(void* data, int width, int height, int channels, const std::vector<int>& wantedChannels);

    /* ***************************************** APIs DEBUG STATE  ************************************** */
    void DebugGLError();
    std::string DebugCMPStatus(CMP_ERROR error);

    /* ***************************************** ENGINE STATE  ************************************** */
    bool TryParseUUID(const std::string& strUUID, UUID& uuid);

    /* ***************************************** MIXED STATE  ************************************** */
    bool IsSubString(const std::string& subStr, const std::string& string);
}
