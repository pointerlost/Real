//
// Created by pointerlost on 3/28/26.
//
#pragma once
#include "Core/SlotMap.h"

namespace Real::assets {

    struct IResourceManager {
        virtual ~IResourceManager() = default;

        virtual void* Get(core::SlotHandle handle)     = 0;
        virtual void  Release(core::SlotHandle handle) = 0;
    };
}

