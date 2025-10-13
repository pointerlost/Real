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

    typedef struct {
        uint transformIndex;
        uint materialIndex;
        uint indexCount;
        uint indexOffset;
    } EntityMetadata;
}