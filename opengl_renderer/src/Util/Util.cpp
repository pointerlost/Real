//
// Created by pointerlost on 10/30/25.
//
#include "Util/Util.h"

#include <GL/glext.h>

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

    GLenum CompressTypeToGLEnum(ImageCompressedType type) {
        switch (type) {
            case ImageCompressedType::BC1:
                return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            case ImageCompressedType::BC2:
                return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            case ImageCompressedType::BC3:
                return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            case ImageCompressedType::BC4:
                return GL_COMPRESSED_RED_RGTC1_EXT;
            case ImageCompressedType::BC5:
                return GL_COMPRESSED_RED_GREEN_RGTC2_EXT;
            case ImageCompressedType::BC6:
                return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
            case ImageCompressedType::BC7:
                return GL_COMPRESSED_RGBA_BPTC_UNORM;

            default:
                Warn("This image compression type does not have a GLenum!");
                return GL_INVALID_ENUM;
        }
    }

    std::string CompressTypeToString(ImageCompressedType type) {
        switch (type) {
            case ImageCompressedType::BC1:
                return "BC1";
            case ImageCompressedType::BC2:
                return "BC2";
            case ImageCompressedType::BC3:
                return "BC3";
            case ImageCompressedType::BC4:
                return "BC4";
            case ImageCompressedType::BC5:
                return "BC5";
            case ImageCompressedType::BC6:
                return "BC6";
            case ImageCompressedType::BC7:
                return "BC7";

            default:
                return "UNDEFINED";
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

    std::string TextureTypeEnumToString(TextureType type) {
        switch (type) {
            case TextureType::ALB:
                return "ALB";
            case TextureType::NRM:
                return "NRM";
            case TextureType::RGH:
                return "RGH";
            case TextureType::MTL:
                return "MTL";
            case TextureType::AO:
                return "AO";
            case TextureType::HEIGHT:
                return "HEIGHT";
            case TextureType::RMA:
                return "RMA";

            default:
                return "UNDEFINED";
        }
    }

    CMP_FORMAT GetCMPFormatWithCompressType(ImageCompressedType type) {
        switch (type) {
            case ImageCompressedType::BC1:
                return CMP_FORMAT_BC1; // 8-bit
            case ImageCompressedType::BC2:
                return CMP_FORMAT_BC2; // 8-bit
            case ImageCompressedType::BC3:
                return CMP_FORMAT_BC3; // 8-bit
            case ImageCompressedType::BC4:
                return CMP_FORMAT_BC4; // 8-bit
            case ImageCompressedType::BC5:
                return CMP_FORMAT_BC5; // 8-bit
            case ImageCompressedType::BC6:
                return CMP_FORMAT_BC6H; // This one using 16-bit option
            case ImageCompressedType::BC7:
                return CMP_FORMAT_BC7; // 8-bit

            default:
                return CMP_FORMAT_Unknown;
        }
    }
}
