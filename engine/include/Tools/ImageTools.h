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
    [[maybe_unused]] bool SaveTextureAsFile(OpenGLTexture* texture, int jpgQuality = 90);
    bool CompressCPUGeneratedTexture(OpenGLTexture* texture, float fQuality = 0.9f);
    bool CompressTextureToBCn(OpenGLTexture* texture, float fQuality = 0.9f);
    void CompressTextureAndReadFromFile(OpenGLTexture* texture);
    Ref<OpenGLTexture> ReadCompressedDataFromDDSFile(const std::string& path);
    void ReadCompressedDataFromDDSFile(OpenGLTexture* texture);
}
