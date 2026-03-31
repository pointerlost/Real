//
// Created by pointerlost on 2/21/26.
//
#pragma once
#include "RenderEnum.h"
#include "Common/Utils.h"
#include "Core/FileManager.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Resource/ResourceHandle.h"

namespace Real::platform::opengl { class OpenGLTexture; }
namespace Real::graphics { struct Material; }

namespace Real::graphics {

    // Geometry
    struct Vertex {
        math::Vec3 position;
        math::Vec3 normal;
        math::Vec2 UV;
    };

    // Color
    struct Color {
        f32 r = 0.f, g = 0.f, b = 0.f, a = 1.f;
    };


    // Typed handles - prevent accidental mixing
    template<typename Tag>
    struct Handle {
        u32 value = 0;

        explicit Handle(u32 val) : value(val) {}
                 Handle()              = default;
                 Handle(const Handle&) = default;

        [[nodiscard]] bool IsValid() const { return value != 0; }

        Handle& operator=(const Handle& other) = default;

        bool operator==(const Handle& other) const { return value == other.value; }
        bool operator!=(const Handle& other) const { return value != other.value; }
        // Comparison with number like: (handle != 0)
        bool operator==(u32 raw) const { return value == raw; }
        bool operator!=(u32 raw) const { return value != raw; }
    };

    using TextureHandle  = Handle<struct TextureTag>;
    using ShaderHandle   = Handle<struct ShaderTag>;
    using BufferHandle   = Handle<struct BufferTag>;
    using BindingPoint   = Handle<struct BindingTag>;

    using GLTextureResourceHandle  = core::ResourceHandle<platform::opengl::OpenGLTexture>;
    using GLMaterialResourceHandle = core::ResourceHandle<Material>;
    using GLTextureReference       = Ref<platform::opengl::OpenGLTexture>;
    using GLTexture                = platform::opengl::OpenGLTexture;

    using     BindlessHandle = u64; // stays flat, GL-specific
    constexpr BindlessHandle InvalidBindlessHandle = 0;

    using     GPUIndex = u32;
    constexpr GPUIndex InvalidGPUIndex = 0;

    struct FrameConfig {
        Color      clearColor = { 0, 0, 0, 1 };
        ClearFlags clearFlags = ClearFlags::Color | ClearFlags::Depth;
    };

    struct TextureData {
        void* data           = nullptr;
        int   width          = 0;
        int   height         = 0;
        int   channelCount   = 0;
        int   dataSize       = 0;
        int   format         = 0;
        int   internalFormat = 0;
    };

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
