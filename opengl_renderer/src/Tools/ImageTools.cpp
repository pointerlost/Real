//
// Created by pointerlost on 10/30/25.
//
#include <Tools/ImageTools.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb_image_write.h>
#include <Graphics/Texture.h>
#include <fstream>
#include "compressonator/include/cmp_compressonatorlib/compressonator.h"
#include "Core/Logger.h"
#include "Graphics/Material.h"
#include "Core/AssetManager.h"
#include "Math/Math.h"
#include "Util/Util.h"
#include <Tools/DDS.h>
#include <algorithm>
#include "Core/CmakeConfig.h"
#include "Core/file_manager.h"
#include "Core/Services.h"

namespace Real::tools {

    Ref<OpenGLTexture> PackTexturesToRGBChannels(const Ref<OpenGLTexture> &ao,
        const Ref<OpenGLTexture> &rgh, const Ref<OpenGLTexture> &mtl, const std::string& materialName)
    {
        if (!ao || !rgh || !mtl) {
            Warn("Texture nullptr! from: " + std::string(__FILE__));
            return {};
        }

        Ref<OpenGLTexture> texture;
        if (ao->GetImageFormatState() != ImageFormatState::DEFAULT) {
            texture = ao;
        } else if (rgh->GetImageFormatState() != ImageFormatState::DEFAULT) {
            texture = rgh;
        } else if (mtl->GetImageFormatState() != ImageFormatState::DEFAULT) {
            texture = mtl;
        } else {
            return Services::GetAssetManager()->GetOrCreateDefaultTexture(TextureType::ORM);
        }

        Ref<OpenGLTexture> mixedTexture = CreateRef<OpenGLTexture>();
        const int width  = texture->GetLevelData(0).m_Width;
        const int height = texture->GetLevelData(0).m_Height;

        const auto tex1RawData = static_cast<uint8_t *>(ao->GetLevelData(0).m_Data);
        const auto tex2RawData = static_cast<uint8_t *>(rgh->GetLevelData(0).m_Data);
        const auto tex3RawData = static_cast<uint8_t *>(mtl->GetLevelData(0).m_Data);

        // All textures have the same resolution
        constexpr int channelCount = 4;
        const int dataSize = width * height * channelCount * 1;

        // Create mixedTexture data
        TextureData mixedData;
        mixedData.m_Data = new uint8_t[dataSize]; // Still void ptr
        auto* mixedTexRawData = static_cast<uint8_t*>(mixedData.m_Data); // Cast to uint8_t*

        constexpr int greyscaleChannelCount = 1;
        for (size_t i = 0; i < width * height; i++) {
            mixedTexRawData[i * channelCount + 0] = tex1RawData[i * greyscaleChannelCount];
            mixedTexRawData[i * channelCount + 1] = tex2RawData[i * greyscaleChannelCount];
            mixedTexRawData[i * channelCount + 2] = tex3RawData[i * greyscaleChannelCount];
            mixedTexRawData[i * channelCount + 3] = 255;
        }

        mixedTexture->SetType(TextureType::ORM);
        mixedData.m_ChannelCount = channelCount;
        mixedData.m_DataSize = dataSize;
        mixedData.m_Width    = width;
        mixedData.m_Height   = height;
        mixedData.m_Format   = util::GetGLFormat(channelCount);
        mixedData.m_InternalFormat = util::GetGLInternalFormat(channelCount);

        const auto& stateFolder = util::ImageFormatState_EnumToString(texture->GetImageFormatState());
        const auto ext = texture->GetImageFormatState() == ImageFormatState::DEFAULT ? ".png" : texture->GetExtension();

        const auto& newPath = std::string(ASSETS_DIR) + "textures/" + stateFolder + '/' + materialName + "_ORM" + ext;
        mixedTexture->SetFileInfo(fs::CreateFileInfoFromPath(newPath));

        mixedTexture->SetImageFormatState(texture->GetImageFormatState());
        mixedTexture->CreateFromData(mixedData, TextureType::ORM);

        // Clear seperated textures
        const auto& am = Services::GetAssetManager();
        fs::File::Delete(ao->GetPath());
        am->DeleteCPUTexture(ao->GetUUID());
        fs::File::Delete(rgh->GetPath());
        am->DeleteCPUTexture(rgh->GetUUID());
        fs::File::Delete(mtl->GetPath());
        am->DeleteCPUTexture(mtl->GetUUID());

        if (!SaveTextureAsFile(mixedTexture.get())) {
            Warn("ORM packed texture can't saved!");
        }

        return mixedTexture;
    }

    Ref<OpenGLTexture> PackTexturesToRGBChannels(const std::array<Ref<OpenGLTexture>, 3> &orm,
        const std::string &materialName)
    {
        return PackTexturesToRGBChannels(orm[0], orm[1], orm[2], materialName);
    }

    bool SaveTextureAsFile(OpenGLTexture* texture, int jpgQuality) {
        const auto& [width, height] = texture->GetResolution(0);
        const auto cc = texture->GetChannelCount(0);
        const auto stride_in_bytes = cc * width;

        if (stride_in_bytes == 0 || cc == 0 || width == 0 || height == 0) {
            Warn("There is a problem for this texture: " + texture->GetName());
            return false;
        }

        const auto extension = texture->GetFileInfo().ext;
        const auto& destPath = texture->GetPath();

        if (extension == ".png") {
            stbi_write_png(destPath.c_str(), width, height, cc, texture->GetLevelData(0).m_Data, stride_in_bytes);
        }
        else if (extension == ".jpg") {
            stbi_write_jpg(destPath.c_str(), width, height, cc, texture->GetLevelData(0).m_Data, jpgQuality);
        }
        else if (extension == ".tga") {
            stbi_write_tga(destPath.c_str(), width, height, cc, texture->GetLevelData(0).m_Data);
        } else {
            Warn("There is no extension for texture: " + texture->GetFileInfo().name);
            return false;
        }
        return true;
    }

    bool CompressTextureToBCn(OpenGLTexture* texture, float fQuality) {
        if (!texture) {
            Warn("[CompressTextureToBCn] Texture nullptr!");
            return false;
        }
        if (Services::GetAssetManager()->IsTextureCompressed(texture->GetStem())) {
            const auto compressed_dir = std::string(ASSETS_DIR) + "textures/compressed/";
            const std::string fullName = compressed_dir + texture->GetFileInfo().stem + ".dds";
            texture->SetFileInfo(fs::CreateFileInfoFromPath(fullName));
            texture->SetImageFormatState(ImageFormatState::COMPRESSED);
            return true;
        }

        const auto [width, height] = texture->GetResolution(0);
        const auto channelCount    = texture->GetLevelData(0).m_ChannelCount;

        // Init framework plugin and IO interfaces
        CMP_InitFramework();
        CMP_ERROR cmp_status = CMP_OK;
        CMP_MipSet MipSetIn  = {};
        CMP_MipSet MipSetCmp = {};
        KernelOptions kernel_options = {};

        CMP_FORMAT srcFormat;
        switch (channelCount) {
            case 1:  srcFormat = CMP_FORMAT_R_8;       break;
            case 2:  srcFormat = CMP_FORMAT_RG_8;      break;
            case 3:  srcFormat = CMP_FORMAT_RGB_888;   break;
            case 4:  srcFormat = CMP_FORMAT_RGBA_8888; break;
            default: srcFormat = CMP_FORMAT_RGBA_8888;
        }

        MipSetIn.m_format = srcFormat;
        CMP_LoadTexture(texture->GetFileInfo().path.c_str(), &MipSetIn);

        // Pick 4 as min level of mipmap coz compressed textures getting minimum 4x4
        if (CMP_GenerateMIPLevels(&MipSetIn, 4) != CMP_OK) {
            Warn("CMP_GenerateMIPLevels failed!");
            return false;
        }

        // TODO: Treat it as 8-bit everything except the 16-bit We don't have 16 bit option yet!
        CMP_FORMAT destFormat = util::GetCMPDestinationFormat(channelCount);
        MipSetCmp.m_format = destFormat;

        kernel_options.encodeWith = CMP_HPC; // CMP_CPU | CMP_GPU_OCL
        kernel_options.srcformat  = srcFormat;
        kernel_options.format     = destFormat;
        kernel_options.fquality   = fQuality;
        kernel_options.width      = width;
        kernel_options.height     = height;

        cmp_status = CMP_ProcessTexture(&MipSetIn, &MipSetCmp, kernel_options, nullptr);

        if (cmp_status != CMP_OK) {
            Warn(util::DebugCMPStatus(cmp_status));
            Warn("Damn, cmp_status is failed!");
            return false;
        }

        std::vector<TextureData> mipLevelsData;
        mipLevelsData.reserve(MipSetCmp.m_nMipLevels);
        for (size_t level = 0; level < MipSetCmp.m_nMipLevels; level++) {
            auto currLevel = MipSetCmp.m_pMipLevelTable[level];
            TextureData levelData;
            levelData.m_Data = new uint8_t[currLevel->m_dwLinearSize];
            memcpy(levelData.m_Data, currLevel->m_pbData, currLevel->m_dwLinearSize);
            levelData.m_DataSize     = currLevel->m_dwLinearSize;
            levelData.m_Width        = currLevel->m_nWidth;
            levelData.m_Height       = currLevel->m_nHeight;
            levelData.m_ChannelCount = channelCount;
            levelData.m_Format       = util::GetGLFormat(channelCount);
            levelData.m_InternalFormat = util::GetCompressedInternalFormat(channelCount);

            mipLevelsData.push_back(levelData);
        }

        // Save the result to compressed folder
        const auto compressed_dir = std::string(ASSETS_DIR) + "textures/compressed/";
        const std::string fullName = compressed_dir + texture->GetFileInfo().stem + ".dds";
        cmp_status = CMP_SaveTexture(fullName.c_str(), &MipSetCmp);

        // Clean up buffers
        CMP_FreeMipSet(&MipSetIn);
        CMP_FreeMipSet(&MipSetCmp);

        texture->SetFileInfo(fs::CreateFileInfoFromPath(fullName));
        texture->SetMipLevelsData(mipLevelsData);
        texture->SetImageFormatState(ImageFormatState::COMPRESSED);

        if (cmp_status != CMP_OK) {
            std::printf("Error %d: Saving processed file %s\n", cmp_status, compressed_dir.c_str());
            return false;
        }
        return true;
    }

    void CompressTextureAndReadFromFile(OpenGLTexture *texture) {
        if (texture->IsCPUGenerated()) {
            if (CompressCPUGeneratedTexture(texture)) {
                ReadCompressedDataFromDDSFile(texture);
            }
        } else {
            if (CompressTextureToBCn(texture)) {
                ReadCompressedDataFromDDSFile(texture);
            }
        }
    }

    bool CompressCPUGeneratedTexture(OpenGLTexture *texture, float fQuality) {
        if (!texture) {
            Warn("[CompressTextureToBCn] Texture nullptr!");
            return false;
        }
        if (Services::GetAssetManager()->IsTextureCompressed(texture->GetStem())) {
            const auto compressed_dir = std::string(ASSETS_DIR) + "textures/compressed/";
            const std::string fullName = compressed_dir + texture->GetFileInfo().stem + ".dds";
            texture->SetFileInfo(fs::CreateFileInfoFromPath(fullName));
            texture->SetImageFormatState(ImageFormatState::COMPRESSED);
            return true;
        }

        const auto [width, height] = texture->GetResolution(0);
        const auto channelCount = texture->GetLevelData(0).m_ChannelCount;

        // Init framework plugin and IO interfaces
        CMP_InitFramework();
        CMP_MipSet MipSetIn   = {};
        CMP_MipSet MipSetCmp  = {};
        CMP_ERROR  cmp_status = {};
        KernelOptions kernel_options = {};

        auto& texFirstMipLevelData = texture->GetLevelData(0);
        texFirstMipLevelData.m_Format = util::GetGLFormat(channelCount);
        texFirstMipLevelData.m_InternalFormat = util::GetCompressedInternalFormat(channelCount);

        CMP_FORMAT srcFormat;
        switch (channelCount) {
            case 1:  srcFormat = CMP_FORMAT_R_8;       break;
            case 2:  srcFormat = CMP_FORMAT_RG_8;      break;
            case 3:  srcFormat = CMP_FORMAT_RGB_888;   break;
            case 4:  srcFormat = CMP_FORMAT_RGBA_8888; break;
            default: srcFormat = CMP_FORMAT_RGBA_8888;
        }

        MipSetIn.m_format = srcFormat;
        if (CMP_CreateMipSet(&MipSetIn, width, height, 1, CF_8bit, TT_2D) != CMP_OK) {
            Warn(util::DebugCMPStatus(cmp_status));
            Warn("Mipmap creation failed!");
            return false;
        }

        CMP_MipLevel* mipLevel = MipSetIn.m_pMipLevelTable[0];
        mipLevel->m_dwLinearSize = texFirstMipLevelData.m_DataSize;
        mipLevel->m_nWidth  = width;
        mipLevel->m_nHeight = height;

        mipLevel->m_pbData = (CMP_BYTE*)malloc(mipLevel->m_dwLinearSize);
        memcpy(mipLevel->m_pbData, texFirstMipLevelData.m_Data, mipLevel->m_dwLinearSize);

        if (CMP_GenerateMIPLevels(&MipSetIn, 4) != CMP_OK) {
            Warn(util::DebugCMPStatus(cmp_status));
            Warn("CMP_GenerateMIPLevels failed!");
            return false;
        }

        CMP_FORMAT destFormat = util::GetCMPDestinationFormat(channelCount);

        kernel_options.encodeWith = CMP_HPC;
        kernel_options.format     = destFormat;
        kernel_options.fquality   = fQuality;
        kernel_options.width      = width;
        kernel_options.height     = height;

        cmp_status = CMP_ProcessTexture(&MipSetIn, &MipSetCmp, kernel_options, nullptr);

        if (cmp_status != CMP_OK) {
            Warn(util::DebugCMPStatus(cmp_status));
            Warn("Damn, cmp_status is failed! Error type: " + util::DebugCMPStatus(cmp_status));
            return false;
        }

        std::vector<TextureData> mipLevelsData;
        for (size_t level = 0; level < MipSetCmp.m_nMipLevels; level++) {
            auto currLevel = MipSetCmp.m_pMipLevelTable[level];
            TextureData levelData;
            levelData.m_Data = new uint8_t[currLevel->m_dwLinearSize];
            memcpy(levelData.m_Data, currLevel->m_pbData, currLevel->m_dwLinearSize);
            levelData.m_DataSize     = currLevel->m_dwLinearSize;
            levelData.m_Width        = currLevel->m_nWidth;
            levelData.m_Height       = currLevel->m_nHeight;
            levelData.m_ChannelCount = channelCount; // optional, may differ for compressed formats
            levelData.m_Format       = util::GetGLFormat(channelCount);
            levelData.m_InternalFormat = util::GetCompressedInternalFormat(channelCount);
            mipLevelsData.push_back(levelData);
        }

        // Save the result to compressed folder
        const auto compressed_dir = std::string(ASSETS_DIR) + "textures/compressed/";
        const std::string fullPath = compressed_dir + texture->GetFileInfo().stem + ".dds";
        cmp_status = CMP_SaveTexture(fullPath.c_str(), &MipSetCmp);

        texture->SetFileInfo(fs::CreateFileInfoFromPath(fullPath));
        texture->SetMipLevelsData(mipLevelsData);
        texture->SetImageFormatState(ImageFormatState::COMPRESSED);

        // Clean up buffers
        CMP_FreeMipSet(&MipSetIn);
        CMP_FreeMipSet(&MipSetCmp);

        if (cmp_status != CMP_OK) {
            std::printf("Error %d: Saving processed file %s\n", cmp_status, compressed_dir.c_str());
            return false;
        }
        return true;
    }

    Ref<OpenGLTexture> ReadCompressedDataFromDDSFile(const std::string& path) {
        const auto& am = Services::GetAssetManager();
        std::vector<TextureData> mipLevelsData;

        if (!fs::File::Exists(path)) {
            Warn("There is no DDS file with this name: " + path);
            return am->GetOrCreateDefaultTexture(TextureType::ALBEDO);
        }

        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            Warn("Can't open DDS file: " + path);
            return am->GetOrCreateDefaultTexture(TextureType::ALBEDO);
        }

        // Check that the file is a valid DDS file, DirectX::DDS_MAGIC = "DDS "
        uint32_t magicNumber;
        file.read((char*)&magicNumber, sizeof(magicNumber));
        if (!file) {
            Warn("Failed to read magic number for DDS: " + path);
            return am->GetOrCreateDefaultTexture(TextureType::ALBEDO);
        }
        if (magicNumber != 0x20534444) { // 0x20534444 = DDS Magic number
            Warn("This file is not a DDS file!! path: " + path);
            return am->GetOrCreateDefaultTexture(TextureType::ALBEDO);
        }

        DDSHeader header = {};
        if (!file.read((char*)&header, sizeof(DDSHeader))) {
            Warn("Failed to read DDSHeader: " + path);
            return am->GetOrCreateDefaultTexture(TextureType::ALBEDO);
        }
        if (header.dwSize != 124) {
            Warn("Shit happened for magic 124!");
            return am->GetOrCreateDefaultTexture(TextureType::ALBEDO);
        }

        DDSHeaderDX10 dx10Header = {};
        if (header.ddspf_dwFourCC == CMP_MAKEFOURCC('D', 'X', '1', '0')) {
            if (!file.read((char*)&dx10Header, sizeof(DDSHeaderDX10))) {
                Warn("Failed to read DDSHeaderDX10: " + path);
                return am->GetOrCreateDefaultTexture(TextureType::ALBEDO);
            }
        }

        auto [internalFormat, format, blockSize, channelCount] = GetDDSFormatInfo(header, &dx10Header);

        uint32_t mipWidth  = header.dwWidth;
        uint32_t mipHeight = header.dwHeight;
        for (size_t level = 0; level < header.dwMipMapCount; level++) {
            uint32_t blocksWide = (mipWidth + 3)  / 4;
            uint32_t blocksHigh = (mipHeight + 3) / 4;
            uint32_t dataSize   = blocksWide * blocksHigh * blockSize;

            TextureData data = {};
            data.m_Width          = (int)mipWidth;
            data.m_Height         = (int)mipHeight;
            data.m_DataSize       = (int)dataSize;
            data.m_Format         = format;
            data.m_InternalFormat = internalFormat;
            data.m_ChannelCount   = channelCount;

            if (format == 0 || internalFormat == 0) {
                Warn("Format or InternalFormat is UNDEFINED for: " + path);
                continue;
            }

            // Allocate and read
            data.m_Data = new uint8_t[dataSize];
            file.read((char*)data.m_Data, dataSize);

            // Update dimensions for next mipmap level
            mipWidth  = std::max(4u, mipWidth  >> 1);
            mipHeight = std::max(4u, mipHeight >> 1);

            mipLevelsData.push_back(data);
        }

        file.close();

        if (mipLevelsData.empty()) {
            Warn("Mip levels data is empty!!!");
            return am->GetOrCreateDefaultTexture(TextureType::ALBEDO);
        }

        return CreateRef<OpenGLTexture>(mipLevelsData);
    }

    void ReadCompressedDataFromDDSFile(OpenGLTexture *texture) {
        const auto& path = texture->GetPath();
        std::vector<TextureData> mipLevelsData;

        if (!fs::File::Exists(path)) {
            Warn("There is no DDS file with this name: " + path);
            return;
        }

        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            Warn("Can't open DDS file: " + path);
            return;
        }

        // Check that the file is a valid DDS file, DirectX::DDS_MAGIC = "DDS "
        uint32_t magicNumber;
        file.read((char*)&magicNumber, sizeof(magicNumber));
        if (!file) {
            Warn("Failed to read magic number for DDS: " + path);
            return;
        }
        if (magicNumber != 0x20534444) { // 0x20534444 = DDS Magic number
            Warn("This file is not a DDS file!! path: " + path);
            return;
        }

        DDSHeader header = {};
        if (!file.read((char*)&header, sizeof(DDSHeader))) {
            Warn("Failed to read DDSHeader: " + path);
            return;
        }
        if (header.dwSize != 124) {
            Warn("Shit happened for magic 124!");
            return;
        }

        DDSHeaderDX10 dx10Header = {};
        if (header.ddspf_dwFourCC == CMP_MAKEFOURCC('D', 'X', '1', '0')) {
            if (!file.read((char*)&dx10Header, sizeof(DDSHeaderDX10))) {
                Warn("Failed to read DDSHeaderDX10: " + path);
                return;
            }
        }

        auto [internalFormat, format, blockSize, channelCount] = GetDDSFormatInfo(header, &dx10Header);

        uint32_t mipWidth  = header.dwWidth;
        uint32_t mipHeight = header.dwHeight;
        for (size_t level = 0; level < header.dwMipMapCount; level++) {
            uint32_t blocksWide = (mipWidth + 3)  / 4;
            uint32_t blocksHigh = (mipHeight + 3) / 4;
            uint32_t dataSize   = blocksWide * blocksHigh * blockSize;

            TextureData data = {};
            data.m_Width          = (int)mipWidth;
            data.m_Height         = (int)mipHeight;
            data.m_DataSize       = (int)dataSize;
            data.m_Format         = format;
            data.m_InternalFormat = internalFormat;
            data.m_ChannelCount   = channelCount;

            if (format == 0 || internalFormat == 0) {
                Warn("Format or InternalFormat is UNDEFINED for: " + path);
                continue;
            }

            // Allocate and read
            data.m_Data = new uint8_t[dataSize];
            file.read((char*)data.m_Data, dataSize);

            // Update dimensions for next mipmap level
            mipWidth  = std::max(4u, mipWidth  >> 1);
            mipHeight = std::max(4u, mipHeight >> 1);

            mipLevelsData.push_back(data);
        }

        file.close();

        if (mipLevelsData.empty()) {
            Warn("Mip levels data is empty!!!");
        }

        texture->SetMipLevelsData(mipLevelsData);
    }
}
