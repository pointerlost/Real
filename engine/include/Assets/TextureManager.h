//
// Created by pointerlost on 3/23/26.
//
#pragma once
#include <unordered_map>
#include <unordered_set>

#include "Common/RealEnum.h"
#include "Common/Types.h"
#include "Core/Utils.h"
#include "Core/UUID.h"
#include "Graphics/RenderTypes.h"


namespace Real {
    struct Material;
    enum class TextureType;
    struct OpenGLTexture;
}

namespace Real {

    class TextureManager {
    public:
        TextureManager();

        // Registration
        void Register(const Ref<OpenGLTexture>& tex);
        void DeleteCPU(const UUID& uuid);

        // Loading
        graphics::TextureData LoadFromFile(const String& path, TextureType type = TextureType::UNDEFINED);

        // Lookup
        [[nodiscard]] const Ref<OpenGLTexture>&  GetTexture(const UUID& uuid, TextureType type);
        [[nodiscard]] Ref<OpenGLTexture>&        GetOrCreateDefault(TextureType type);
        [[nodiscard]] bool                       IsCompressed(const String& stem) const;
        [[nodiscard]] Vector<Ref<OpenGLTexture>> GetMaterialTextures(const Material* mat) const;

        // GPU
        void                                           LoadArraysToGPU() const;
        [[nodiscard]] Vector<graphics::BindlessHandle> UploadToGPU();
        [[nodiscard]] size_t                           GetNextBindlessIndex() const;

        void Update();

    private:
        std::unordered_map<UUID, Ref<OpenGLTexture>>        m_Textures;
        std::unordered_set<graphics::BindlessHandle>        m_BindlessHandles;
        std::unordered_map<TextureType, Ref<OpenGLTexture>> m_DefaultTextures;

    private:
        void LoadDefaultTextures();
    };
}
