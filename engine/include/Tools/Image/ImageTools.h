//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include "Common/Types.h"
#include "Common/Utils.h"

namespace Real::graphics         { struct TextureData;   }
namespace Real::platform::opengl { struct OpenGLTexture; }

namespace Real::tools {

    // Compress texture to BCn DDS, update CPU mip data and state
    // TextureManager FlushPendingUploads handles Uploading to GPU
    bool CompressTexture(platform::opengl::OpenGLTexture* texture, f32 fQuality = 0.9f);

    bool ReadDDS(const String& path, Vector<graphics::TextureData>& outLevels);

    // Save raw CPU texture data to disk (.png, .jpg, etc.)
    bool SaveTextureAsFile(platform::opengl::OpenGLTexture* texture, int jpgQuality = 90);

    // Pack AO + Roughness + Metallic into a single RGBA texture.
    // Compresses result and Returns CPU-ready texture for texture manager to register and upload.
    Ref<platform::opengl::OpenGLTexture> PackORM(
        const Ref<platform::opengl::OpenGLTexture>& ao,
        const Ref<platform::opengl::OpenGLTexture>& rgh,
        const Ref<platform::opengl::OpenGLTexture>& mtl,
        const String& materialName
    );

    Ref<platform::opengl::OpenGLTexture> PackORM(
        const std::array<Ref<platform::opengl::OpenGLTexture>, 3>& orm,
        const String& materialName
    );
}
