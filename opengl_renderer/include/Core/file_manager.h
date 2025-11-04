//
// Created by pointerlost on 9/7/25.
//
#pragma once
#include <filesystem>

namespace Real {

    struct FileInfo {
        std::string name; // Full name
        std::string path; // Full path
        std::string stem; // Name without extension
        std::string ext;  // Extension
    };

    class File {
    public:
        [[nodiscard]] static std::string ReadFromFile(const std::string& path);
        [[nodiscard]] static bool Exists(const std::string& path);
    };
}
