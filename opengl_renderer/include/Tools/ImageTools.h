//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include <vector>
#include "Core/Utils.h"
#include "Graphics/Texture.h"

namespace Real {
    struct TextureData;
    struct OpenGLTexture;
}

namespace Real::tools {
    [[nodiscard]] Ref<OpenGLTexture> PackTexturesToRGBChannels(const Ref<OpenGLTexture> &tex1,
        const Ref<OpenGLTexture> &tex2, const Ref<OpenGLTexture> &tex3, const std::string& materialName
    );
    [[nodiscard]] Ref<OpenGLTexture> PackTexturesToRGBChannels(
        const std::array<Ref<OpenGLTexture>, 3> &textures, const std::string& materialName
    );
    Ref<OpenGLTexture> LoadRMATextures(std::array<Ref<OpenGLTexture>, 3>& rma, const std::string& materialName);
    [[nodiscard]] bool SaveTextureAsFile(OpenGLTexture* texture, const std::string&destPath, int jpgQuality = 90);
    void CompressTextureToBCn(OpenGLTexture* texture, const std::string& destPath);
    void CompressCPUGeneratedTexture(OpenGLTexture* texture, const std::string& destPath);
    void ReadCompressedDataFromDDSFile(OpenGLTexture* texture);
}
