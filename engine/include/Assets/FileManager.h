//
// Created by pointerlost on 9/7/25.
//
#pragma once
#include <Common/Types.h>

namespace Real::fs {

    struct FileInfo {
        String name; // Full name
        String stem; // Without extension
        String path; // Full path
        String ext;  // Extension only
    };

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
