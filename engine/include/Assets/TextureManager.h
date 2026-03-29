//
// Created by pointerlost on 3/23/26.
//
#pragma once
#include <unordered_map>
#include <unordered_set>
#include "Common/Types.h"
#include "Common/Utils.h"
#include "Core/UUID.h"
#include "Graphics/RenderTypes.h"

namespace Real::graphics         { struct Material;      }
namespace Real::platform::opengl { struct OpenGLTexture; }

namespace Real::assets {
    using namespace graphics;

    // TODO: using gl specific types is not good idea for cross-api, clean it from gl
    class TextureManager {
    public:
        TextureManager();

        // Registration
        void Register(const UUID& uuid, Ref<platform::opengl::OpenGLTexture> texture);
        void DeleteCPU(const UUID& uuid);

        // Lookup
        [[nodiscard]]    const Ref<platform::opengl::OpenGLTexture>&  GetTexture(const UUID& uuid, TextureType type);
        [[maybe_unused]] Ref<platform::opengl::OpenGLTexture>&        GetOrCreateDefault(TextureType type);
        [[nodiscard]]    bool                                         IsCompressed(const String& stem) const;
        [[nodiscard]]    Vector<Ref<platform::opengl::OpenGLTexture>> GetMaterialTextures(const Material* mat) const;

        // GPU - call once per frame or after batch load
        [[nodiscard]] Vector<BindlessHandle> FlushPendingUploads();
        [[nodiscard]] size_t                 GetNextBindlessIndex() const;

        void Update();

    private:
        std::unordered_map<UUID, Ref<platform::opengl::OpenGLTexture>>        m_Textures;
        std::unordered_map<TextureType, Ref<platform::opengl::OpenGLTexture>> m_DefaultTextures;

        Vector<Ref<platform::opengl::OpenGLTexture>> m_PendingUpload;
        Vector<BindlessHandle>                       m_BindlessHandles;

    private:
        void InitDefaults();
    };
}
