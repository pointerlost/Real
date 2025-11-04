//
// Created by pointerlost on 10/30/25.
//
#include "Util/Util.h"

#include "Core/file_manager.h"
#include "Core/Logger.h"

namespace Real::Util {

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
                fileInfo.name = p.path().filename();
                fileInfo.ext = p.path().extension();
                fileInfo.stem = p.path().stem();
                fileInfo.path = p.path();
                files.emplace_back(fileInfo);
            }
        }

        return files;
    }

    bool CheckSubStrExistsInString(const std::string &subStr, const std::string &string) {
        return string.find(subStr) != std::string::npos;
    }

}
