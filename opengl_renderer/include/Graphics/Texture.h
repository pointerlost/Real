//
// Created by pointerlost on 10/6/25.
//
#pragma once
#include <glad/glad.h>
#include <string>

namespace Real {

    struct Texture {
        Texture() = default;
        Texture(const Texture&) = default;

        [[nodiscard]] GLuint GetID() const { return m_ID; }

        bool Create(const std::string& filePath, const std::string& name);
        void CheckOrMakeResident();
        void CheckOrMakeNonResident();

    private:
        GLuint m_ID = 0;
        uint64_t m_BindlessHandle = 0;
        bool m_IsResident = false;
        int m_Width = 0, m_Height = 0;
        std::string m_Name;

    private:
        void GenerateMipMaps();
    };
}
