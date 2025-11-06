//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include "Core/Utils.h"

namespace Real { struct Texture; }

namespace Real::tools {
    [[nodiscard]] Ref<Texture> PackTexturesToChannels(const std::string& name, const Ref<Texture> &tex1, const Ref<Texture> &tex2, const Ref<Texture> &tex3);
    [[nodiscard]] Ref<Texture> PackTexturesToChannels(const std::string& name, const std::array<Ref<Texture>, 3>& textures);
    void CompressTextureToBCn(Ref<Texture>& texture, const std::string& destPath);
}
