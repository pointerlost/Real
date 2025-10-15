//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <vector>
#include <cstdint>

namespace Real::Graphics { struct Vertex; }

namespace Real {

    class MeshFactory {
    public:
        static std::pair<std::vector<Graphics::Vertex>, std::vector<uint32_t>> CreateTriangle();
        static std::pair<std::vector<Graphics::Vertex>, std::vector<uint32_t>> CreateCube();
    };
}
