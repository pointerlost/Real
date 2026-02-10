//
// Created by pointerlost on 10/13/25.
//
#pragma once
#include <cstdint>

namespace Real {

    typedef struct {
        uint count;
        uint instanceCount;
        uint firstIndex;
        uint baseVertex;
        uint baseInstance;
    } DrawElementsIndirectCommand;
    static_assert(sizeof(DrawElementsIndirectCommand) == 20);

    typedef struct {
        int transformIndex;
        int materialIndex;
        int indexCount;
        int indexOffset;
    } EntityMetadata;
    static_assert(sizeof(EntityMetadata) == 16);
}