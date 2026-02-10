//
// Created by pointerlost on 2/1/26.
//
#pragma once
#include "Common/RealTypes.h"

namespace Real::graphics::debug {

    class DebugMeshFactory {
    public:
        static DebugMesh CreateLine(std::vector<DebugVertex>& vertices, std::vector<uint32_t>& indices);
        static DebugMesh CreateBoxWire(std::vector<DebugVertex>& vertices, std::vector<uint32_t>& indices);
        static DebugMesh CreateSphereWire(std::vector<DebugVertex>& vertices, std::vector<uint32_t>& indices,
            uint32_t slices
        );
    };
}
