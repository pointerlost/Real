//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include "Core/Utils.h"
#include "Graphics/Texture.h"

namespace Real {
    struct TextureData;
    struct OpenGLTexture;
}

namespace Real::tools {
    Ref<OpenGLTexture> PackTexturesToRGBChannels(const Ref<OpenGLTexture>& ao, const Ref<OpenGLTexture>& rgh,
        const Ref<OpenGLTexture>& mtl, const std::string& materialName
    );
    [[maybe_unused]] Ref<OpenGLTexture> PackTexturesToRGBChannels(
        const std::array<Ref<OpenGLTexture>, 3> &orm, const std::string& materialName
    );
    Ref<OpenGLTexture> PrepareAndPackRMATextures(std::array<Ref<OpenGLTexture>, 3>& orm, const std::string& materialName);
    [[maybe_unused]] bool SaveTextureAsFile(OpenGLTexture* texture, const std::string&destPath, int jpgQuality = 90);
    void CompressCPUGeneratedTexture(OpenGLTexture* texture, const std::string& destPath);
    void CompressTextureToBCn(OpenGLTexture* texture);
    void CompressTextureAndReadFromFile(OpenGLTexture* texture);
    void ReadCompressedDataFromDDSFile(OpenGLTexture* texture);
}
