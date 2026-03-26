//
// Created by pointerlost on 2/25/26.
//
#pragma once
#include "Common/RealEnum.h"
#include "Graphics/RenderTypes.h"

namespace Real::graphics {

    struct GPUTextureHandle {
        TextureHandle  apiHandle      = TextureHandle{};
        BindlessHandle bindlessHandle = BindlessHandle{};
    };

    struct TextureMetadata {
        ImageFormatState  imageFormatState = ImageFormatState::UNDEFINED;
        TextureType       type             = TextureType::UNDEFINED;
        TextureWrapMode   wrapMode         = TextureWrapMode::REPEAT;
        TextureFilterMode filterMode       = TextureFilterMode::NEAREST;

        fs::FileInfo fileInfo{};
    };

    struct TextureAsset {
        TextureMetadata  metadata{};  // compression, format, wrap, etc.
        GPUTextureHandle gpu{};       // api handle + bindless handle
        bool             dirtyGPU = true;
    };
}
