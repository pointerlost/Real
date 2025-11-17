//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include <vector>
#include "Core/Utils.h"

namespace Real {
    enum class ImageCompressedType;
    struct TextureData;
    struct OpenGLTexture;
}

namespace Real::tools {
    [[nodiscard]] Ref<OpenGLTexture> PackTexturesToRGBChannels(const Ref<OpenGLTexture> &tex1, const Ref<OpenGLTexture> &tex2, const Ref<OpenGLTexture> &tex3);
    [[nodiscard]] Ref<OpenGLTexture> PackTexturesToRGBChannels(const std::array<Ref<OpenGLTexture>, 3> &textures);
    void CompressTextureToBCn(const Ref<OpenGLTexture>& texture, const std::string& destPath, ImageCompressedType compressionType);
    std::vector<TextureData> ReadCompressedDataFromDDSFile(OpenGLTexture* texture);
}
