//
// Created by pointerlost on 10/30/25.
//
#pragma once
#include <string>
#include <vector>

namespace Real { struct FileInfo; }

namespace Real::Util {
    [[nodiscard]] std::vector<FileInfo> IterateDirectory(const std::string& folderPath);
    bool CheckSubStrExistsInString(const std::string& subStr, const std::string& string);
}
