//
// Created by pointerlost on 9/7/25.
//
#include "Core/file_manager.h"
#include <fstream>
#include "Core/Logger.h"
#include <Common/RealTypes.h>

namespace Real::fs {

    std::string File::ReadFromFile(const std::string &path) {
        if (!Exists(path)) {
            Warn("Path doesn't exists: " + path);
            return {};
        }

        std::string content;
        std::ifstream stream(path, std::ios::in);
        if (!stream.is_open()) {
            Warn("File can't opening!");
            return {};
        }

        std::string line;
        while (getline(stream, line)) {
            content.append(line + "\n");
        }

        stream.close();
        return content;
    }

    bool File::Exists(const std::string &path) {
        return std::filesystem::exists(path);
    }

    bool File::DeleteFile(const std::string &path) {
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

    std::vector<FileInfo> IterateDirectory(const std::string &folderPath) {
        std::vector<FileInfo> files;
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

    FileInfo CreateFileInfoFromPath(const std::string &path) {
        FileInfo info;
        const size_t slashPos = path.find_last_of("/\\");

        if (slashPos == std::string::npos)
            Warn("There is no '/' in the path, So can't created FileInfo!!! Fix it");

        info.path = path;
        info.name = path.substr(slashPos + 1);

        const size_t dotPos = info.name.find_last_of('.');
        if (dotPos == std::string::npos)
            Warn("There is no '.' in the path, So can't created FileInfo!!! Fix it");

        info.ext  = info.name.substr(dotPos);
        info.stem = info.name.substr(0, dotPos);

        return info;
    }
}
