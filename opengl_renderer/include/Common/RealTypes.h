//
// Created by pointerlost on 12/6/25.
//
#pragma once
#include <iostream>
#include <cstdint>
#include <Graphics/Mesh.h>
#include "Macros.h"

namespace Real { struct MeshEntry; }

namespace Real {

    struct TextureData {
        void* m_Data = nullptr;
        int m_ChannelCount = 0;
        int m_Width = 0;
        int m_Height = 0;
        uint m_DataSize = 0;
        int m_Format = {};
        int m_InternalFormat = {};

        TextureData() = default;
        ~TextureData() { delete[] static_cast<uint8_t*>(m_Data); }

        // Copy constructor
        TextureData(const TextureData& other) {
            m_ChannelCount = other.m_ChannelCount;
            m_Width    = other.m_Width;
            m_Height   = other.m_Height;
            m_DataSize = other.m_DataSize;
            m_Format   = other.m_Format;
            m_InternalFormat = other.m_InternalFormat;

            if (other.m_Data && other.m_DataSize > 0) {
                m_Data = new uint8_t[m_DataSize];
                memcpy(m_Data, other.m_Data, m_DataSize);
            }
        }

        // Copy assignment
        TextureData& operator=(const TextureData& other) {
            if (this == &other) return *this;

            // free existing
            delete[] static_cast<uint8_t*>(m_Data);

            m_ChannelCount = other.m_ChannelCount;
            m_Width    = other.m_Width;
            m_Height   = other.m_Height;
            m_DataSize = other.m_DataSize;
            m_Format   = other.m_Format;
            m_InternalFormat = other.m_InternalFormat;

            if (other.m_Data && other.m_DataSize > 0) {
                m_Data = new uint8_t[m_DataSize];
                memcpy(m_Data, other.m_Data, m_DataSize);
            } else {
                m_Data = nullptr;
            }

            return *this;
        }
    };

    struct FileInfo {
        std::string name; // Full name
        std::string path; // Full path
        std::string stem; // Name without extension
        std::string ext;  // Extension
    };

    struct ModelBinaryHeader {
        uint32_t m_Magic = REAL_MAGIC; // Real magic number
        uint32_t m_Version = 1;
        uint32_t m_MeshCount = 0;
        uint64_t m_UUID = UUID();
        std::vector<MeshEntry> m_MeshData;
        Graphics::MeshInfo m_MeshInfo{};
    };
}
