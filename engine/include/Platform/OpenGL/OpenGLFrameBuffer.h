//
// Created by pointerlost on 10/29/25.
//
#pragma once
#include "Common/Types.h"
#include "Graphics/RenderTypes.h"

namespace Real::math { struct iVec2; }

namespace Real::opengl {

    // TODO: Create Color Buffer

    // TODO: Create Depth Buffer

    // TODO: Create Stencil Buffer

    class OpenGLFrameBuffer {
    public:
        OpenGLFrameBuffer() = default;
        OpenGLFrameBuffer(String name, int width, int height);
        OpenGLFrameBuffer(String name, const math::iVec2& resolution);
        OpenGLFrameBuffer(const OpenGLFrameBuffer&) = default;
        ~OpenGLFrameBuffer();

        void Create();
        void Bind() const;
        void Unbind();
        void CheckIfStatusCompleted();
        void SetTarget(graphics::BufferHandle target);
        void BindAttachment(const graphics::BufferHandle &attachment) const;

        [[nodiscard]] graphics::BufferHandle        GetHandle() const { return m_FBO;    }
        [[nodiscard]] const graphics::BufferHandle& GetTarget() const { return m_Target; }

        [[nodiscard]] math::iVec2 GetViewportSize() const;
        [[nodiscard]] const String& GetName() const { return m_Name; }

    private:
        void CleanUp() const;

    private:
        graphics::BufferHandle  m_FBO{};
        graphics::BufferHandle  m_Target{};
        graphics::TextureHandle m_Texture{};
        int    m_Width  = 800;
        int    m_Height = 600;
        String m_Name   = {};
    };
}
