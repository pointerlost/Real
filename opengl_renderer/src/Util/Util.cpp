//
// Created by pointerlost on 10/30/25.
//
#include "Util/Util.h"

#include "Core/file_manager.h"
#include "Core/Logger.h"
#include "Graphics/Texture.h"

namespace Real::util {

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

    int FindClosestPowerOfTwo(int num) {
        int x = 1;
        int y = 0;
        while (x < num) {
            y = x;
            x <<= 1;
        }
        return abs(y - num) > abs(x - num) ? x : y;
    }

    ImageCompressedType PickTextureCompressionType(TextureType type) {
        switch (type) {
            case TextureType::ALB:
            case TextureType::RMA:
                return ImageCompressedType::BC7;

            case TextureType::NRM:
                return ImageCompressedType::BC5;

            case TextureType::HEIGHT:
            case TextureType::RGH:
            case TextureType::MTL:
            case TextureType::AO:
                return ImageCompressedType::BC4;

            default:
                return ImageCompressedType::UNDEFINED;
        }
    }

    GLenum ConvertChannelCountToGLType(int channelCount) {
        switch (channelCount) {
            case 1:
                return GL_R8;
            case 2:
                return GL_RG8;
            case 4:
                return GL_RGBA8;

            default:
                return GL_RGB8;
        }
    }

    TextureType StringToEnumTextureType(const std::string &type) {
        if (type == "ALB") {
            return TextureType::ALB;
        } else if (type == "NRM") {
            return TextureType::NRM;
        } else if (type == "RGH") {
            return TextureType::RGH;
        } else if (type == "MTL") {
            return TextureType::MTL;
        } else if (type == "AO") {
            return TextureType::AO;
        } else if (type == "HEIGHT") {
            return TextureType::HEIGHT;
        }
        return TextureType::UNDEFINED;
    }
}
