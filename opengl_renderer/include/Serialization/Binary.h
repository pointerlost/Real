//
// Created by pointerlost on 12/15/25.
//
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace Real {
    namespace Graphics {
        struct Vertex;
    }

    struct ModelBinaryHeader;
}

namespace Real::serialization::binary {

    /* ********************************************* MODEL STATE ********************************************* */
    [[nodiscard]] ModelBinaryHeader LoadModel(const std::string& path);
    void WriteModel(const std::string& path, ModelBinaryHeader binaryHeader,
        std::vector<Graphics::Vertex> vertices, std::vector<uint64_t> indices
    );

}
