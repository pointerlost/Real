//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include "Common/Types.h"
#include "Core/Utils.h"


namespace Real::platform::opengl { struct OpenGLTexture; }

namespace Real::tools {
    Ref<platform::opengl::OpenGLTexture> PackTexturesToRGBChannels(
        const Ref<platform::opengl::OpenGLTexture>& ao,  const Ref<platform::opengl::OpenGLTexture>& rgh,
        const Ref<platform::opengl::OpenGLTexture>& mtl, const String& materialName
    );
    [[maybe_unused]] Ref<platform::opengl::OpenGLTexture> PackTexturesToRGBChannels(
        const std::array<Ref<platform::opengl::OpenGLTexture>, 3> &orm, const String& materialName
    );
    [[maybe_unused]] bool SaveTextureAsFile(platform::opengl::OpenGLTexture* texture, int jpgQuality = 90);
    bool CompressCPUGeneratedTexture(platform::opengl::OpenGLTexture* texture, f32 fQuality = 0.9f);
    bool CompressTextureToBCn(platform::opengl::OpenGLTexture* texture, f32 fQuality = 0.9f);
    void CompressTextureAndReadFromFile(platform::opengl::OpenGLTexture* texture);
    Ref<platform::opengl::OpenGLTexture> ReadCompressedDataFromDDSFile(const String& path);
    void ReadCompressedDataFromDDSFile(platform::opengl::OpenGLTexture* texture);
}
