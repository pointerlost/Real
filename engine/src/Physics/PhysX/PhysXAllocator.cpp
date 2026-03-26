//
// Created by pointerlost on 1/15/26.
//
#include "Physics/PhysX/PhysXAllocator.h"

namespace Real::physics {
    void* PhysXAllocator::allocate(size_t size, const char *typeName, const char *filename, int line) {
        return ::operator new(size);
    }

    void PhysXAllocator::deallocate(void *ptr) {
        ::operator delete(ptr);
    }
}
