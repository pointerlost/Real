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
        // TODO: when we add HDR Skyboxes match to BC6
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
                Warn("Returning UNDEFINED Texture type for: " + TextureTypeEnumToString(type));
                return ImageCompressedType::UNDEFINED;
        }
    }

    GLenum ConvertChannelCountToGLType(int channelCount, const std::string& name) {
        switch (channelCount) {
            case 1:
                return GL_R8;
            case 2:
                return GL_RG8;
            case 3:
                return GL_RGB8;
            case 4:
                return GL_RGBA8;

            default:
                Warn("There is no GLType for this channel count, tex name: " + name);
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
                Warn("This image compression type does not have a GLenum: " + CompressTypeToString(type));
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
        } else if (type == "RMA") {
            return TextureType::RMA;
        } else if (type == "RGH") {
            return TextureType::RGH;
        } else if (type == "MTL") {
            return TextureType::MTL;
        } else if (type == "AO") {
            return TextureType::AO;
        } else if (type == "HEIGHT") {
            return TextureType::HEIGHT;
        }
        Info("Type returning UNDEFINED for: " + type);
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
                Warn("Texture type returning UNDEFINED for: " + TextureTypeEnumToString(type));
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
                Warn("[GetCMPFormatWithCompressType] Unknown format!");
                return CMP_FORMAT_Unknown;
        }
    }

    // Bit Per Pixel
    uint TexFormat_uncompressed_GetBitPerTexel(TextureType type) {
        switch (type) {
            case TextureType::ALB:
            case TextureType::NRM:
            case TextureType::RMA:
                return 32;

            case TextureType::RGH:
            case TextureType::MTL:
            case TextureType::AO:
            case TextureType::HEIGHT:
                return 8;

            default: // TextureType::UNDEFINED
                return 1;
        }
    }

    uint TexFormat_uncompressed_GetBytePerTexel(TextureType type) {
        return TexFormat_uncompressed_GetBitPerTexel(type) / 8;
    }

    uint TexFormat_compressed_GetBytesPerBlock(ImageCompressedType type) {
        switch (type) {
            case ImageCompressedType::BC1:
                return 8;
            case ImageCompressedType::BC2:
                return 16;
            case ImageCompressedType::BC3:
                return 16;
            case ImageCompressedType::BC4:
                return 8;
            case ImageCompressedType::BC5:
                return 16;
            case ImageCompressedType::BC6:
                return 16;
            case ImageCompressedType::BC7:
                return 16;

            default:
                Warn("There is no ImageFormatState for this type!!! returning null val!");
                return 0;
        }
    }

}
