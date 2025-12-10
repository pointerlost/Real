//
// Created by pointerlost on 12/6/25.
//
#pragma once
#include <iostream>
#include <cstdint>

namespace Real {

    struct ModelBinaryHeader {
        uint32_t m_Magic = REAL_MAGIC; // Real magic number
        uint32_t m_Version = 1;
        uint32_t m_MeshCount = 0;
        std::vector<MeshEntry> m_MeshData;
        Graphics::MeshInfo m_MeshInfo{};
    };
}