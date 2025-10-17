//
// Created by pointerlost on 10/6/25.
//
#pragma once
#include <glad/glad.h>
#include <string>

namespace Real {

    struct Texture {
    public:
        Texture() = default;
        Texture(const Texture&) = default;
        void Load(const std::string& filePath);

    public:
        unsigned char* m_Data;
        GLuint m_ID = 0;
        GLsizei m_Width = 2, m_Height = 2;
        int m_Index = -1; // index will use to find texture in TextureArray
    };
}
