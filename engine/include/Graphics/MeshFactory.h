//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include "Common/Types.h"

namespace Real::graphics { struct Vertex; }

namespace Real {

    class MeshFactory {
    public:
        static std::pair<Vector<graphics::Vertex>, Vector<u32>> CreateTriangle();
        static std::pair<Vector<graphics::Vertex>, Vector<u32>> CreateCube();
    };
}
