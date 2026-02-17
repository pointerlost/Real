//
// Created by pointerlost on 2/16/26.
//
#pragma once

namespace Real::core {

    class IWindow {
    public:
        virtual ~IWindow() = default;
        virtual void Init(const char* title, int width, int height) = 0;
        virtual void PollEvents() = 0;
        virtual void SwapBuffers() = 0;
        virtual bool ShouldClose() const = 0;
        virtual void Close() = 0;

        virtual void Resize(int width, int height) = 0;
        // returning an untyped pointer(void*) so cast it before using them!
        [[nodiscard]] virtual void* GetNativeHandle() const = 0;
    };
}