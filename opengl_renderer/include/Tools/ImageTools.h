//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include <vector>
#include "Core/Utils.h"

namespace Real {
    struct TextureData;
    struct OpenGLTexture;
}

namespace Real::tools {
    [[nodiscard]] Ref<OpenGLTexture> PackTexturesToRGBChannels(const Ref<OpenGLTexture> &tex1,
        const Ref<OpenGLTexture> &tex2, const Ref<OpenGLTexture> &tex3, bool optionalAlpha = false, uint alpha = 255
    );
    [[nodiscard]] Ref<OpenGLTexture> PackTexturesToRGBChannels(const std::array<Ref<OpenGLTexture>, 3> &textures,
        bool optionalAlpha = false, uint alpha = 255
    );
    void CompressTextureToBCn(OpenGLTexture* texture, const std::string& destPath);
    void CompressCPUGeneratedTexture(OpenGLTexture* texture, const std::string& destPath);
    void ReadCompressedDataFromDDSFile(OpenGLTexture* texture);
}
