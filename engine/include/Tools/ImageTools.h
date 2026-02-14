//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include "Common/RealTypes.h"
#include "Core/Utils.h"

namespace Real {
    struct TextureData;
    struct OpenGLTexture;
}

namespace Real::tools {
    Ref<OpenGLTexture> PackTexturesToRGBChannels(const Ref<OpenGLTexture>& ao, const Ref<OpenGLTexture>& rgh,
        const Ref<OpenGLTexture>& mtl, const String& materialName
    );
    [[maybe_unused]] Ref<OpenGLTexture> PackTexturesToRGBChannels(
        const std::array<Ref<OpenGLTexture>, 3> &orm, const String& materialName
    );
    [[maybe_unused]] bool SaveTextureAsFile(OpenGLTexture* texture, int jpgQuality = 90);
    bool CompressCPUGeneratedTexture(OpenGLTexture* texture, f32 fQuality = 0.9f);
    bool CompressTextureToBCn(OpenGLTexture* texture, f32 fQuality = 0.9f);
    void CompressTextureAndReadFromFile(OpenGLTexture* texture);
    Ref<OpenGLTexture> ReadCompressedDataFromDDSFile(const String& path);
    void ReadCompressedDataFromDDSFile(OpenGLTexture* texture);
}
