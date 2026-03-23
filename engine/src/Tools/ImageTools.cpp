//
// Created by pointerlost on 10/30/25.
//
#include <Tools/ImageTools.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb_image_write.h>
#include <fstream>
#include "compressonator/include/cmp_compressonatorlib/compressonator.h"
#include "Core/Logger.h"
#include "Graphics/Material.h"
#include "Assets/AssetManager.h"
#include "Util/Util.h"
#include <Tools/DDS.h>
#include <algorithm>
#include "Assets/FileManager.h"
#include "Core/CMakeConfig.h"
#include "Core/Services.h"

namespace Real::tools {

    Ref<platform::opengl::OpenGLTexture> PackTexturesToRGBChannels(
        const Ref<platform::opengl::OpenGLTexture> &ao,
        const Ref<platform::opengl::OpenGLTexture> &rgh,
        const Ref<platform::opengl::OpenGLTexture> &mtl,
        const String& materialName)
    {
        if (!ao || !rgh || !mtl) {
            Warn("Texture nullptr! from: " + String(__FILE__));
            return {};
        }

        Ref<platform::opengl::OpenGLTexture> texture;
        if (ao->GetImageFormatState() != ImageFormatState::DEFAULT) {
            texture = ao;
        } else if (rgh->GetImageFormatState() != ImageFormatState::DEFAULT) {
            texture = rgh;
        } else if (mtl->GetImageFormatState() != ImageFormatState::DEFAULT) {
            texture = mtl;
        } else {
            return Services::GetAssetManager()->GetOrCreateDefaultTexture(TextureType::ORM);
        }

        Ref<platform::opengl::OpenGLTexture> mixedTexture = CreateRef<platform::opengl::OpenGLTexture>();
        const int width  = texture->GetLevelData(0).width;
        const int height = texture->GetLevelData(0).height;

        const auto tex1RawData = static_cast<u8 *>(ao->GetLevelData(0).data);
        const auto tex2RawData = static_cast<u8 *>(rgh->GetLevelData(0).data);
        const auto tex3RawData = static_cast<u8 *>(mtl->GetLevelData(0).data);

        // All textures have the same resolution
        constexpr int channelCount = 4;
        const int dataSize = width * height * channelCount * 1;

        // Create mixedTexture data
        graphics::TextureData mixedData;
        mixedData.data = new u8[dataSize]; // Still void ptr
        auto* mixedTexRawData = static_cast<u8*>(mixedData.data); // Cast to u8*

        for (size_t i = 0; i < width * height; i++) {
            constexpr int greyscaleChannelCount = 1;
            mixedTexRawData[i * channelCount + 0] = tex1RawData[i * greyscaleChannelCount];
            mixedTexRawData[i * channelCount + 1] = tex2RawData[i * greyscaleChannelCount];
            mixedTexRawData[i * channelCount + 2] = tex3RawData[i * greyscaleChannelCount];
            mixedTexRawData[i * channelCount + 3] = 255;
        }

        mixedTexture->SetType(TextureType::ORM);
        mixedData.channelCount   = channelCount;
        mixedData.dataSize       = dataSize;
        mixedData.width          = width;
        mixedData.height         = height;
        mixedData.format         = util::GetGLFormat(channelCount);
        mixedData.internalFormat = util::GetGLInternalFormat(channelCount);

        const auto& stateFolder = util::ImageFormatState_EnumToString(texture->GetImageFormatState());
        const auto ext = texture->GetImageFormatState() == ImageFormatState::DEFAULT ? ".png" : texture->GetExtension();

        const auto& newPath = String(ASSETS_DIR) + "textures/" + stateFolder + '/' + materialName + "_ORM" + ext;
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

    Ref<platform::opengl::OpenGLTexture> PackTexturesToRGBChannels(
        const std::array<Ref<platform::opengl::OpenGLTexture>, 3> &orm,
        const String &materialName)
    {
        return PackTexturesToRGBChannels(orm[0], orm[1], orm[2], materialName);
    }

    bool SaveTextureAsFile(platform::opengl::OpenGLTexture* texture, int jpgQuality) {
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
            stbi_write_png(destPath.c_str(), width, height, cc, texture->GetLevelData(0).data, stride_in_bytes);
        }
        else if (extension == ".jpg") {
            stbi_write_jpg(destPath.c_str(), width, height, cc, texture->GetLevelData(0).data, jpgQuality);
        }
        else if (extension == ".tga") {
            stbi_write_tga(destPath.c_str(), width, height, cc, texture->GetLevelData(0).data);
        } else {
            Warn("There is no extension for texture: " + texture->GetFileInfo().name);
            return false;
        }
        return true;
    }

    bool CompressTextureToBCn(platform::opengl::OpenGLTexture* texture, f32 fQuality) {
        if (!texture) {
            Warn("[CompressTextureToBCn] Texture nullptr!");
            return false;
        }
        if (Services::GetAssetManager()->IsTextureCompressed(texture->GetStem())) {
            const auto compressed_dir = String(ASSETS_DIR) + "textures/compressed/";
            const String fullName = compressed_dir + texture->GetFileInfo().stem + ".dds";
            texture->SetFileInfo(fs::CreateFileInfoFromPath(fullName));
            texture->SetImageFormatState(ImageFormatState::COMPRESSED);
            return true;
        }

        const auto [width, height] = texture->GetResolution(0);
        const auto channelCount    = texture->GetLevelData(0).channelCount;

        // Init framework plugin and IO interfaces
        CMP_InitFramework();
        CMP_ERROR     cmp_status = CMP_OK;
        CMP_MipSet    MipSetIn  = {};
        CMP_MipSet    MipSetCmp = {};
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

        Vector<graphics::TextureData> mipLevelsData;
        mipLevelsData.reserve(MipSetCmp.m_nMipLevels);
        for (size_t level = 0; level < MipSetCmp.m_nMipLevels; level++) {
            auto currLevel = MipSetCmp.m_pMipLevelTable[level];
            graphics::TextureData levelData;
            levelData.data = new u8[currLevel->m_dwLinearSize];
            memcpy(levelData.data, currLevel->m_pbData, currLevel->m_dwLinearSize);
            levelData.dataSize       = currLevel->m_dwLinearSize;
            levelData.width          = currLevel->m_nWidth;
            levelData.height         = currLevel->m_nHeight;
            levelData.channelCount   = channelCount;
            levelData.format         = util::GetGLFormat(channelCount);
            levelData.internalFormat = util::GetCompressedInternalFormat(channelCount);

            mipLevelsData.push_back(levelData);
        }

        // Save the result to compressed folder
        const auto compressed_dir = String(ASSETS_DIR) + "textures/compressed/";
        const String fullName = compressed_dir + texture->GetFileInfo().stem + ".dds";
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

    void CompressTextureAndReadFromFile(platform::opengl::OpenGLTexture *texture) {
        if (texture->IsCPUGenerated()) {
            if (CompressCPUGeneratedTexture(texture)) {
                ReadCompressedDataFromDDSFile(texture);
            }
        } else {
            if (CompressTextureToBCn(texture)) {
                ReadCompressedDataFromDDSFile(texture);
            }
        }

        // TODO: REFACTORING NEEDED, BAD RESPONSIBILITY SEPARATION ( CREATION STATE )
    }

    bool CompressCPUGeneratedTexture(platform::opengl::OpenGLTexture *texture, f32 fQuality) {
        if (!texture) {
            Warn("[CompressTextureToBCn] Texture nullptr!");
            return false;
        }
        if (Services::GetAssetManager()->IsTextureCompressed(texture->GetStem())) {
            const auto compressed_dir = String(ASSETS_DIR) + "textures/compressed/";
            const String fullName = compressed_dir + texture->GetFileInfo().stem + ".dds";
            texture->SetFileInfo(fs::CreateFileInfoFromPath(fullName));
            texture->SetImageFormatState(ImageFormatState::COMPRESSED);
            return true;
        }

        const auto [width, height] = texture->GetResolution(0);
        const auto channelCount = texture->GetLevelData(0).channelCount;

        // Init framework plugin and IO interfaces
        CMP_InitFramework();
        CMP_MipSet MipSetIn   = {};
        CMP_MipSet MipSetCmp  = {};
        CMP_ERROR  cmp_status = {};
        KernelOptions kernel_options = {};

        auto& texFirstMipLevelData          = texture->GetLevelData(0);
        texFirstMipLevelData.format         = util::GetGLFormat(channelCount);
        texFirstMipLevelData.internalFormat = util::GetCompressedInternalFormat(channelCount);

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
        mipLevel->m_dwLinearSize = texFirstMipLevelData.dataSize;
        mipLevel->m_nWidth  = width;
        mipLevel->m_nHeight = height;

        mipLevel->m_pbData = static_cast<CMP_BYTE*>(malloc(mipLevel->m_dwLinearSize));
        memcpy(mipLevel->m_pbData, texFirstMipLevelData.data, mipLevel->m_dwLinearSize);

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

        Vector<graphics::TextureData> mipLevelsData;
        for (size_t level = 0; level < MipSetCmp.m_nMipLevels; level++) {
            auto currLevel = MipSetCmp.m_pMipLevelTable[level];
            graphics::TextureData levelData;
            levelData.data = new u8[currLevel->m_dwLinearSize];
            memcpy(levelData.data, currLevel->m_pbData, currLevel->m_dwLinearSize);
            levelData.dataSize       = currLevel->m_dwLinearSize;
            levelData.width          = currLevel->m_nWidth;
            levelData.height         = currLevel->m_nHeight;
            levelData.channelCount   = channelCount; // optional, may differ for compressed formats
            levelData.format         = util::GetGLFormat(channelCount);
            levelData.internalFormat = util::GetCompressedInternalFormat(channelCount);
            mipLevelsData.push_back(levelData);
        }

        // Save the result to compressed folder
        const auto compressed_dir = String(ASSETS_DIR) + "textures/compressed/";
        const String fullPath = compressed_dir + texture->GetFileInfo().stem + ".dds";
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

    Ref<platform::opengl::OpenGLTexture> ReadCompressedDataFromDDSFile(const String& path) {
        const auto& am = Services::GetAssetManager();
        Vector<graphics::TextureData> mipLevelsData;

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
        u32 magicNumber;
        file.read(reinterpret_cast<char*>(&magicNumber), sizeof(magicNumber));
        if (!file) {
            Warn("Failed to read magic number for DDS: " + path);
            return am->GetOrCreateDefaultTexture(TextureType::ALBEDO);
        }
        if (magicNumber != 0x20534444) { // 0x20534444 = DDS Magic number
            Warn("This file is not a DDS file!! path: " + path);
            return am->GetOrCreateDefaultTexture(TextureType::ALBEDO);
        }

        DDSHeader header = {};
        if (!file.read(reinterpret_cast<char*>(&header), sizeof(DDSHeader))) {
            Warn("Failed to read DDSHeader: " + path);
            return am->GetOrCreateDefaultTexture(TextureType::ALBEDO);
        }
        if (header.dwSize != 124) {
            Warn("Shit happened for magic 124!");
            return am->GetOrCreateDefaultTexture(TextureType::ALBEDO);
        }

        DDSHeaderDX10 dx10Header = {};
        if (header.ddspf_dwFourCC == CMP_MAKEFOURCC('D', 'X', '1', '0')) {
            if (!file.read(reinterpret_cast<char*>(&dx10Header), sizeof(DDSHeaderDX10))) {
                Warn("Failed to read DDSHeaderDX10: " + path);
                return am->GetOrCreateDefaultTexture(TextureType::ALBEDO);
            }
        }

        auto [internalFormat, format, blockSize, channelCount] = GetDDSFormatInfo(header, &dx10Header);

        u32 mipWidth  = header.dwWidth;
        u32 mipHeight = header.dwHeight;
        for (size_t level = 0; level < header.dwMipMapCount; level++) {
            u32 blocksWide = (mipWidth + 3)  / 4;
            u32 blocksHigh = (mipHeight + 3) / 4;
            u32 dataSize   = blocksWide * blocksHigh * blockSize;

            graphics::TextureData data = {};
            data.width          = (int)mipWidth;
            data.height         = (int)mipHeight;
            data.dataSize       = (int)dataSize;
            data.format         = format;
            data.internalFormat = internalFormat;
            data.channelCount   = channelCount;

            if (format == 0 || internalFormat == 0) {
                Warn("Format or InternalFormat is UNDEFINED for: " + path);
                continue;
            }

            // Allocate and read
            data.data = new u8[dataSize];
            file.read(static_cast<char*>(data.data), dataSize);

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

        return CreateRef<platform::opengl::OpenGLTexture>(mipLevelsData, fs::CreateFileInfoFromPath(path));
    }

    void ReadCompressedDataFromDDSFile(platform::opengl::OpenGLTexture *texture) {
        const auto& path = texture->GetPath();
        Vector<graphics::TextureData> mipLevelsData;

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
        u32 magicNumber;
        file.read(reinterpret_cast<char*>(&magicNumber), sizeof(magicNumber));
        if (!file) {
            Warn("Failed to read magic number for DDS: " + path);
            return;
        }
        if (magicNumber != 0x20534444) { // 0x20534444 = DDS Magic number
            Warn("This file is not a DDS file!! path: " + path);
            return;
        }

        DDSHeader header = {};
        if (!file.read(reinterpret_cast<char*>(&header), sizeof(DDSHeader))) {
            Warn("Failed to read DDSHeader: " + path);
            return;
        }
        if (header.dwSize != 124) {
            Warn("Shit happened for magic 124!");
            return;
        }

        DDSHeaderDX10 dx10Header = {};
        if (header.ddspf_dwFourCC == CMP_MAKEFOURCC('D', 'X', '1', '0')) {
            if (!file.read(reinterpret_cast<char*>(&dx10Header), sizeof(DDSHeaderDX10))) {
                Warn("Failed to read DDSHeaderDX10: " + path);
                return;
            }
        }

        auto [internalFormat, format, blockSize, channelCount] = GetDDSFormatInfo(header, &dx10Header);

        u32 mipWidth  = header.dwWidth;
        u32 mipHeight = header.dwHeight;
        for (size_t level = 0; level < header.dwMipMapCount; level++) {
            u32 blocksWide = (mipWidth + 3)  / 4;
            u32 blocksHigh = (mipHeight + 3) / 4;
            u32 dataSize   = blocksWide * blocksHigh * blockSize;

            graphics::TextureData data = {};
            data.width          = static_cast<int>(mipWidth);
            data.height         = static_cast<int>(mipHeight);
            data.dataSize       = static_cast<int>(dataSize);
            data.format         = format;
            data.internalFormat = internalFormat;
            data.channelCount   = channelCount;

            if (format == 0 || internalFormat == 0) {
                Warn("Format or InternalFormat is UNDEFINED for: " + path);
                continue;
            }

            // Allocate and read
            data.data = new u8[dataSize];
            file.read(static_cast<char*>(data.data), dataSize);

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

        texture->PrepareOptionsAndUploadToGPU();
        texture->SetIndex(Services::GetAssetManager()->GetNextBindlessIndex());
    }
}
