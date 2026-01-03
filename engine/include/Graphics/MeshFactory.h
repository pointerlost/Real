//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <vector>
#include <cstdint>

namespace Real { struct Vertex; }

namespace Real {

    class MeshFactory {
    public:
        static std::pair<std::vector<Vertex>, std::vector<uint32_t>> CreateTriangle();
        static std::pair<std::vector<Vertex>, std::vector<uint32_t>> CreateCube();
    };
}
