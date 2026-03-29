//
// Created by pointerlost on 3/28/26.
//
#pragma once
#include "Core/SlotMap.h"
#include "RHI/ITexture.h"


namespace Real {

    class ResourceManager {
    public:
        ResourceManager()                       = default;
        ResourceManager(const ResourceManager&) = delete;


    private:
        core::SlotMap<rhi::ITexture> m_TextureSlotMap;
    };
}
