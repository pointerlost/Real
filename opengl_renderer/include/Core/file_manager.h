//
// Created by pointerlost on 9/7/25.
//
#pragma once
#include <filesystem>

namespace Real {

    struct FileInfo {
        std::string name;
        std::string path;
        std::string stem;
        std::string extension;
    };

    class File {
    public:
        [[nodiscard]] static std::string ReadFromFile(const std::string& path);
        [[nodiscard]] static bool Exists(const std::string& path);
    };
}
