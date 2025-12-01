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
        const Ref<OpenGLTexture> &tex2, const Ref<OpenGLTexture> &tex3
    );
    [[nodiscard]] Ref<OpenGLTexture> PackTexturesToRGBChannels(const std::array<Ref<OpenGLTexture>, 3> &textures);
    void LoadRMATextures(std::array<Ref<OpenGLTexture>, 3> rma, const std::string& materialName);
    void SaveTextureAsFile(OpenGLTexture* texture, const std::string &filename);
    void CompressTextureToBCn(OpenGLTexture* texture, const std::string& destPath);
    void CompressCPUGeneratedTexture(OpenGLTexture* texture, const std::string& destPath);
    void ReadCompressedDataFromDDSFile(OpenGLTexture* texture);
}
