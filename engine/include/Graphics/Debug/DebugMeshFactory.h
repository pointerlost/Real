//
// Created by pointerlost on 2/1/26.
//
#pragma once
#include "DebugTypes.h"
#include "Common/Types.h"

namespace Real::graphics::debug {

    class DebugMeshFactory {
    public:
        static DebugMesh CreateLine(Vector<DebugVertex>& vertices, Vector<u32>& indices);
        static DebugMesh CreateBoxWire(Vector<DebugVertex>& vertices, Vector<u32>& indices);
        static DebugMesh CreateSphereWire(Vector<DebugVertex>& vertices, Vector<u32>& indices,
            u32 slices
        );
    };
}
