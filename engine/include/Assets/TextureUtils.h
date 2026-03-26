//
// Created by pointerlost on 3/23/26.
//
#pragma once
#include <assimp/material.h>
#include "Graphics/Texture/Texture.h"

namespace Real::util::texture {
    int         TextureTypeToChannelCount(TextureType type);
    TextureType TextureType_StringToEnum(const String& type);
    TextureType AssimpTextureTypeToRealType(aiTextureType type);
    String      TextureType_EnumToString(TextureType type);
    String      GetDefaultTextureName(TextureType type, int width);
    uint        GetBitPerTexel(TextureType type);
    uint        GetBytePerTexel(TextureType type);

    graphics::TextureData ExtractChannel(const graphics::TextureData& data, int channelIndex);
    graphics::TextureData ExtractChannel(void* data, int width, int height, int channels, int channelIndex);
    graphics::TextureData ExtractChannels(const graphics::TextureData& data, const Vector<int>& channels);
    graphics::TextureData ExtractChannels(void* data, int width, int height, int channels, const Vector<int>& wanted);
}
