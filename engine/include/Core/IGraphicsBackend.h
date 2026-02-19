//
// Created by pointerlost on 2/19/26.
//
#pragma once

namespace Real {
    struct RendererConfig;
}

namespace Real::core {

    class IGraphicsBackend {
    public:
        virtual ~IGraphicsBackend() = default;

        virtual void Initialize(void* nativeWindow, const RendererConfig& cfg) = 0;
        virtual void Shutdown() = 0;
    };
}
