//
// Created by pointerlost on 3/23/26.
//
#pragma once
#include <unordered_map>
#include "Common/Types.h"
#include "Graphics/RenderTypes.h"
#include "Resource/ResourceHandle.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Core/IResourceManager.h"

namespace Real::graphics { struct Material; }

namespace Real::assets {

    using namespace graphics;

    // TODO: using gl specific types is not good idea for cross-api, clean it from gl
    class TextureManager final : public IResourceManager {
    public:
        TextureManager();

        void* Get(core::SlotHandle handle)     override;
        void  Release(core::SlotHandle handle) override;

        // Registration
        GLTextureResourceHandle Register(const GLTextureReference& texture);
        void                    DeleteCPU(core::SlotHandle slot);

        GLTextureResourceHandle FindByUUID(const UUID& uuid);

        // Lookup
        [[nodiscard]] GLTextureReference         GetTexture(const GLTextureResourceHandle& handle);
        [[nodiscard]] GLTextureReference         GetTexture(const UUID& id);
        [[nodiscard]] bool                       IsCompressed(const String& stem) const;
        [[maybe_unused]] GLTextureReference&     GetOrCreateDefault(TextureType type);
        [[nodiscard]] Vector<GLTextureReference> GetMaterialTextures(const Material* mat);

        // GPU
        [[nodiscard]] Vector<BindlessHandle> FlushPendingUploads(); // Should we call updateframe? to
        [[nodiscard]] size_t                 GetNextBindlessIndex() const;

    private:
        core::SlotMap<GLTextureReference>          m_Textures{};
        std::unordered_map<UUID, core::SlotHandle> m_UUIDToSlot{};
        Vector<GLTextureReference>                 m_PendingUpload{};
        Vector<BindlessHandle>                     m_BindlessHandles{};

        std::unordered_map<TextureType, GLTextureReference> m_DefaultTextures{};

        void InitDefaults();
    };
}
