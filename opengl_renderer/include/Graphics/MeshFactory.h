//
// Created by pointerlost on 10/4/25.
//
#pragma once
#include <iostream>
#include <vector>


namespace Real::Graphics {
    struct Vertex;
}

namespace Real {

    class MeshFactory {
    public:
        static std::pair<std::vector<Graphics::Vertex>, std::vector<unsigned int>> CreateTriangle();
        static std::pair<std::vector<Graphics::Vertex>, std::vector<unsigned int>> CreateCube();
    };
}
