//
// Created by pointerlost on 3/23/26.
//
#pragma once
#include "Common/Types.h"
#include "Core/UUID.h"
#include "Graphics/Texture/TextureTypes.h"

namespace Real::rhi {

    class ITexture {
    public:
        virtual ~ITexture() = default;

        [[nodiscard]] virtual UUID         GetUUID() const = 0;
        [[nodiscard]] virtual u32          GetWidth() const = 0;
        [[nodiscard]] virtual u32          GetHeight() const = 0;
        [[nodiscard]] virtual TextureType  GetType() const = 0;
        virtual void CreateFromData(const graphics::TextureData& data, TextureType type) = 0;
    };

}
