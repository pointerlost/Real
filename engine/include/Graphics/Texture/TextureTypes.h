//
// Created by pointerlost on 2/25/26.
//
#pragma once
#include "Common/RealEnum.h"
#include "Graphics/RenderTypes.h"

namespace Real {

    struct GPUTextureHandle {
        graphics::TextureHandle  apiHandle      = graphics::TextureHandle{};
        graphics::BindlessHandle bindlessHandle = graphics::BindlessHandle{};
    };

    struct TextureMetadata {
        ImageFormatState imageFormatState = ImageFormatState::UNDEFINED;
        TextureType type                  = TextureType::UNDEFINED;
        TextureWrapMode wrapMode          = TextureWrapMode::REPEAT;
        TextureFilterMode filterMode      = TextureFilterMode::NEAREST;

        FileInfo fileInfo{};
    };

    struct TextureAsset {
        TextureMetadata metadata;  // compression, format, wrap, etc.
        GPUTextureHandle gpu;      // api handle + bindless handle
        bool dirtyGPU = true;
    };
}
