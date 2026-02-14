//
// Created by pointerlost on 9/7/25.
//
#pragma once
#include <iostream>
#include <Common/RealTypes.h>

namespace Real::fs {

    class File {
    public:
        [[nodiscard]] static String ReadFromFile(const String& path);
        [[nodiscard]] static bool Exists(const String& path);
        [[maybe_unused]] static bool Delete(const String& path);
    };

    [[nodiscard]] Vector<FileInfo> IterateDirectory(const String& folderPath);
    FileInfo CreateFileInfoFromPath(const String& rawPath);
    String NormalizePath(const String& path);
}
