//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include <string>
#include <vector>

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
    GLenum ConvertChannelCountToGLType(int channelCount);
    TextureType StringToEnumTextureType(const std::string& type);
}
