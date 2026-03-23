//
// Created by pointerlost on 9/7/25.
//
#include "../../include/Assets/FileManager.h"
#include <filesystem>
#include <algorithm>
#include <fstream>
#include "Core/Logger.h"
#include "Core/Utils.h"

namespace Real::fs {

    String File::ReadFromFile(const String &path) {
        if (!Exists(path)) {
            Warn("Path doesn't exists: " + path);
            return {};
        }

        String content;
        std::ifstream stream(path, std::ios::in);
        if (!stream.is_open()) {
            Warn("File can't opening!");
            return {};
        }

        String line;
        while (getline(stream, line)) {
            content.append(line + "\n");
        }

        stream.close();
        return content;
    }

    bool File::Exists(const String &path) {
        return std::filesystem::exists(path);
    }

    bool File::Delete(const String &path) {
        if (Exists(path)) {
            if (std::filesystem::remove(path)) {
                return true;
            }
            Info("File can't deleted!");
            return false;
        }
        Info("There is no file path to be remove!");
        return false;
    }

    Vector<FileInfo> IterateDirectory(const String &folderPath) {
        Vector<FileInfo> files;
        namespace fs = std::filesystem;

        if (!File::Exists(folderPath)) {
            Warn("Folder doesn't exists: " + folderPath);
            return {};
        }

        for (auto &p : fs::recursive_directory_iterator(folderPath)) {
            if (p.path().has_extension()) {
                FileInfo fileInfo{};
                fileInfo.name = p.path().filename().string();
                fileInfo.ext  = p.path().extension().string();
                fileInfo.stem = p.path().stem().string();
                fileInfo.path = p.path().string();
                files.emplace_back(fileInfo);
            }
        }

        return files;
    }

    FileInfo CreateFileInfoFromPath(const String &rawPath) {
        const String path = NormalizePath(rawPath);
        const std::filesystem::path p(path);

        FileInfo info;
        info.path = path; // canonical, normalized
        info.name = p.filename().string();
        info.stem = p.stem().string();
        info.ext  = p.extension().string();

        return info;
    }

    String NormalizePath(const String &path) {
        const std::filesystem::path p = std::filesystem::weakly_canonical(path); // resolves '.', '..' etc.

        String result = p.generic_string(); // Use always '/' for platform consistency

#ifdef _WIN32
        std::ranges::transform(result, result.begin(), ::tolower);
#endif
        return result;
    }
}
