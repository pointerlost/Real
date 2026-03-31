//
// Created by pointerlost on 10/13/25.
//
#pragma once
#include <cstdint>

namespace Real {

    struct DrawElementsIndirectCommand {
        uint count;
        uint instanceCount;
        uint firstIndex;
        uint baseVertex;
        uint baseInstance;
    };
    static_assert(sizeof(DrawElementsIndirectCommand) == 20);

    struct EntityMetadata {
        int transformIndex;
        int materialIndex;
        int indexCount;
        int indexOffset;
    };
    static_assert(sizeof(EntityMetadata) == 16);
}