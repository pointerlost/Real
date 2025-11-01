//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include "Core/Utils.h"

namespace Real {
    struct Texture;
}

namespace Real::Tools {
    Ref<Texture> PackRMATexturesToChannels(const Ref<Texture> &roughness, const Ref<Texture> &metallic, const Ref<Texture> &ao);
    void CompressTextureToBCn(Ref<Texture>& texture, const char* destPath);
}
