//
// Created by pointerlost on 9/7/25.
//
#pragma once
#include <filesystem>
#include <vector>

namespace Real {
    struct FileInfo;
}

namespace Real::fs {

    class File {
    public:
        [[nodiscard]] static std::string ReadFromFile(const std::string& path);
        [[nodiscard]] static bool Exists(const std::string& path);
        [[maybe_unused]] static bool DeleteFile(const std::string& path);
    };

    [[nodiscard]] std::vector<FileInfo> IterateDirectory(const std::string& folderPath);
    FileInfo CreateFileInfoFromPath(const std::string& path);
}
