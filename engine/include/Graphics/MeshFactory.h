//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include "Common/RealTypes.h"

namespace Real { struct Vertex; }

namespace Real {

    class MeshFactory {
    public:
        static std::pair<Vector<Vertex>, Vector<u32>> CreateTriangle();
        static std::pair<Vector<Vertex>, Vector<u32>> CreateCube();
    };
}
