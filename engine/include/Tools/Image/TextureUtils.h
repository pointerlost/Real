//
// Created by pointerlost on 3/23/26.
//
#pragma once
#include "Graphics/RenderTypes.h"

namespace Real::graphics {
    enum class TextureType;
}

namespace Real::util::texture {
    using namespace graphics;
    int         TextureTypeToChannelCount(TextureType type);
    TextureType TextureType_StringToEnum(const String& type);
    TextureType AssimpTextureTypeToRealType(uint type);
    String      TextureType_EnumToString(TextureType type);
    String      GetDefaultTextureName(TextureType type, int width);
    uint        GetBitPerTexel(TextureType type);
    uint        GetBytePerTexel(TextureType type);

    TextureData ExtractChannel(const TextureData& data, int channelIndex);
    TextureData ExtractChannel(void* data, int width, int height, int channels, int channelIndex);
    TextureData ExtractChannels(const TextureData& data, const Vector<int>& channels);
    TextureData ExtractChannels(void* data, int width, int height, int channels, const Vector<int>& wanted);
}
