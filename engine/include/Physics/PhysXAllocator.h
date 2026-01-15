//
// Created by pointerlost on 1/15/26.
//
#pragma once
#include "foundation/PxAllocatorCallback.h"

namespace Real::physics {

    class PhysXAllocator final : public physx::PxAllocatorCallback {
        void* allocate(size_t size, const char* typeName, const char* filename, int line) override;
        void deallocate(void* ptr) override;
    };
}
