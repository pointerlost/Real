//
// Created by pointerlost on 10/30/25.
//
#include <Tools/ImageTools.h>
#include <fstream>
#include "compressonator/include/cmp_compressonatorlib/compressonator.h"
#include "Core/Logger.h"
#include "Graphics/Material.h"
#include "Core/AssetManager.h"
#include "Math/Math.h"
#include "Util/Util.h"
#include <Tools/DDS.h>
#include "Graphics/Texture.h"
#include <algorithm>
#include "Core/CmakeConfig.h"
#include "Core/Services.h"

namespace Real::tools {

    Ref<OpenGLTexture> PackTexturesToRGBChannels(const Ref<OpenGLTexture> &tex1, const Ref<OpenGLTexture> &tex2, const Ref<OpenGLTexture> &tex3, bool optionalAlpha, uint alpha) {
        if (!tex1 || !tex2 || !tex3) {
            Warn("Texture nullptr! from: " + std::string(__FILE__));
            return {};
        }

        Ref<OpenGLTexture> mixedTexture = CreateRef<OpenGLTexture>();

        const auto tex1LevelData = tex1->GetLevelData(0);
        const auto tex2LevelData = tex2->GetLevelData(0);
        const auto tex3LevelData = tex3->GetLevelData(0);
        // If textures has different resolutions, resize them
        int width = 0,height = 0;
        if (tex1LevelData.m_Width != tex2LevelData.m_Width || tex2LevelData.m_Width != tex3LevelData.m_Width
            || tex1LevelData.m_Height != tex2LevelData.m_Height || tex2LevelData.m_Height != tex3LevelData.m_Height)
        {
            width  = std::max({tex1LevelData.m_Width, tex2LevelData.m_Width, tex3LevelData.m_Width});
            height = std::max({tex1LevelData.m_Height, tex2LevelData.m_Height, tex3LevelData.m_Height});

            width = util::FindClosestPowerOfTwo(std::min(width, height));
            if (width < 256) width = 256;
            else if (width > 4096) width = 4096;
            height = width;

            tex1->Resize({width, height}, 0);
            tex2->Resize({width, height}, 0);
            tex3->Resize({width, height}, 0);
        } else {
            width  = tex1LevelData.m_Width;
            height = tex1LevelData.m_Height;
        }

        const auto tex1RawData = static_cast<uint8_t *>(tex1->GetLevelData(0).m_Data);
        const auto tex2RawData = static_cast<uint8_t *>(tex2->GetLevelData(0).m_Data);
        const auto tex3RawData = static_cast<uint8_t *>(tex3->GetLevelData(0).m_Data);

        // All textures have the same resolution
        const int channelCount = optionalAlpha ? 4 : 3;
        const int dataSize = width * height * channelCount * 1;

        // Create mixedTexture data
        TextureData mixedData;
        mixedData.m_Data = new uint8_t[dataSize]; // Still void ptr
        uint8_t* mixedTexRawData = static_cast<uint8_t*>(mixedData.m_Data); // Cast to uint8_t*

        constexpr int greyscaleChannelCount = 1;
        for (size_t i = 0; i < width * height; i++) {
            mixedTexRawData[i * channelCount + 0] = tex1RawData[i * greyscaleChannelCount];
            mixedTexRawData[i * channelCount + 1] = tex2RawData[i * greyscaleChannelCount];
            mixedTexRawData[i * channelCount + 2] = tex3RawData[i * greyscaleChannelCount];
            if (optionalAlpha) {
                mixedTexRawData[i * channelCount + 3] = alpha;
            }
        }

        mixedTexture->SetType(TextureType::RMA);
        mixedData.m_ChannelCount = channelCount;
        mixedData.m_DataSize = dataSize;
        mixedData.m_Width    = width;
        mixedData.m_Height   = height;
        mixedData.m_Format = util::ConvertChannelCountToGLFormat(channelCount);
        mixedData.m_InternalFormat = util::ConvertChannelCountToGLFormat(channelCount);
        mixedTexture->CreateFromData(mixedData, TextureType::RMA);
        return mixedTexture;
    }

    Ref<OpenGLTexture> PackTexturesToRGBChannels(const std::array<Ref<OpenGLTexture>, 3> &textures, bool optionalAlpha, uint alpha) {
        return PackTexturesToRGBChannels(textures[0], textures[1], textures[2], optionalAlpha, alpha);
    }

    void CompressTextureToBCn(OpenGLTexture* texture, const std::string& destPath) {
        if (!texture) {
            Warn("[CompressTextureToBCn] Texture nullptr!");
            return;
        }
        if (Services::GetAssetManager()->IsTextureCompressed(texture->GetStem())) return;

        const auto [width, height] = texture->GetResolution(0);
        const auto channelCount = texture->GetLevelData(0).m_ChannelCount;

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

        CMP_LoadTexture(texture->GetFileInfo().path.c_str(), &MipSetIn);
        MipSetIn.m_format = srcFormat;

        CMP_INT mipLevelCount = (CMP_INT)std::log2(std::max(width, height)) + 1;
        CMP_INT nMinSize = CMP_CalcMinMipSize(height, width, mipLevelCount);
        if (CMP_GenerateMIPLevels(&MipSetIn, nMinSize) != CMP_OK) {
            Warn("CMP_GenerateMIPLevels failed!");
            return;
        }

        // TODO: Treat it as 8-bit everything except the 16-bit We don't have 16 bit option yet!
        CMP_FORMAT destFormat = util::GetCMPDestinationFormat(channelCount);

        MipSetCmp.m_format = destFormat;

        kernel_options.encodeWith = CMP_HPC; // CMP_CPU | CMP_GPU_OCL
        kernel_options.srcformat = srcFormat;
        kernel_options.format    = destFormat;
        kernel_options.fquality  = 0.9f;
        kernel_options.width     = width;
        kernel_options.height    = height;

        cmp_status = CMP_ProcessTexture(&MipSetIn, &MipSetCmp, kernel_options, nullptr);

        if (cmp_status != CMP_OK) {
            Warn("Damn, cmp_status is failed!");
            return;
        }

        // Save the result to compressed folder
        std::string fullName = destPath + texture->GetFileInfo().stem + ".dds";
        cmp_status = CMP_SaveTexture(fullName.c_str(), &MipSetCmp);

        // Clean up buffers
        CMP_FreeMipSet(&MipSetIn);
        CMP_FreeMipSet(&MipSetCmp);

        texture->SetImageFormatState(ImageFormatState::COMPRESSED);

        if (cmp_status != CMP_OK) {
            std::printf("Error %d: Saving processed file %s\n", cmp_status, destPath.c_str());
            return;
        }
    }

    void CompressCPUGeneratedTexture(OpenGLTexture *texture, const std::string &destPath) {
        if (!texture) {
            Warn("[CompressTextureToBCn] Texture nullptr!");
            return;
        }
        if (Services::GetAssetManager()->IsTextureCompressed(texture->GetStem())) return;

        const auto [width, height] = texture->GetResolution(0);
        const auto channelCount = texture->GetLevelData(0).m_ChannelCount;

        // Init framework plugin and IO interfaces
        CMP_InitFramework();
        CMP_MipSet MipSetIn   = {};
        CMP_MipSet MipSetCmp  = {};
        CMP_ERROR  cmp_status = {};
        KernelOptions kernel_options = {};

        auto& texFirstMipLevelData = texture->GetLevelData(0);
        texFirstMipLevelData.m_Format = util::ConvertChannelCountToGLFormat(channelCount);
        texFirstMipLevelData.m_InternalFormat = util::GetCompressedInternalFormat(channelCount);

        CMP_FORMAT srcFormat;
        switch (channelCount) {
            case 1:  srcFormat = CMP_FORMAT_R_8;       break;
            case 2:  srcFormat = CMP_FORMAT_RG_8;      break;
            case 3:  srcFormat = CMP_FORMAT_RGB_888;   break;
            case 4:  srcFormat = CMP_FORMAT_RGBA_8888; break;
            default: srcFormat = CMP_FORMAT_RGBA_8888;
        }

        if (CMP_CreateMipSet(&MipSetIn, width, height, 1, CF_8bit, TT_2D) != CMP_OK) {
            Warn("Mipmap creation failed!");
            return;
        }
        MipSetIn.m_format = srcFormat;

        CMP_MipLevel* mipLevel = MipSetIn.m_pMipLevelTable[0];
        mipLevel->m_dwLinearSize = texFirstMipLevelData.m_DataSize;
        mipLevel->m_nWidth  = width;
        mipLevel->m_nHeight = height;

        mipLevel->m_pbData = (CMP_BYTE*)malloc(mipLevel->m_dwLinearSize);
        memcpy(mipLevel->m_pbData, texFirstMipLevelData.m_Data, mipLevel->m_dwLinearSize);

        const CMP_INT mipLevelCount = (CMP_INT)std::log2(std::max(width, height)) + 1;
        const CMP_INT minSize = CMP_CalcMinMipSize(height, width, mipLevelCount);
        if (CMP_GenerateMIPLevels(&MipSetIn, minSize) != CMP_OK) {
            Warn("CMP_GenerateMIPLevels failed!");
            return;
        }

        std::vector<TextureData> mipLevelsData;
        for (size_t level = 0; level < MipSetIn.m_nMipLevels; level++) {
            TextureData levelData;
            levelData.m_Data     = MipSetIn.m_pMipLevelTable[level]->m_pbData;
            levelData.m_DataSize = MipSetIn.m_pMipLevelTable[level]->m_dwLinearSize;
            levelData.m_Width    = MipSetIn.m_pMipLevelTable[level]->m_nWidth;
            levelData.m_Height   = MipSetIn.m_pMipLevelTable[level]->m_nHeight;
            levelData.m_ChannelCount = channelCount;
            levelData.m_Format   = util::ConvertChannelCountToGLFormat(channelCount);
            levelData.m_InternalFormat = util::GetCompressedInternalFormat(channelCount);
            mipLevelsData.push_back(levelData);
        }

        CMP_FORMAT destFormat = util::GetCMPDestinationFormat(channelCount);

        kernel_options.encodeWith = CMP_HPC; // Doesn't support GPU guess so
        kernel_options.format    = destFormat;
        kernel_options.fquality  = 0.9f;
        kernel_options.width     = width;
        kernel_options.height    = height;

        cmp_status = CMP_ProcessTexture(&MipSetIn, &MipSetCmp, kernel_options, nullptr);

        if (cmp_status != CMP_OK) {
            Warn("Damn, cmp_status is failed! Error type: " + util::DebugCMPStatus(cmp_status));
            return;
        }

        // Save the result to compressed folder
        std::string fullName = destPath + texture->GetFileInfo().stem + ".dds";
        cmp_status = CMP_SaveTexture(fullName.c_str(), &MipSetCmp);

        // Clean up buffers
        CMP_FreeMipSet(&MipSetIn);
        CMP_FreeMipSet(&MipSetCmp);

        texture->SetImageFormatState(ImageFormatState::COMPRESSED);
        texture->SetMipLevelsData(mipLevelsData);

        if (cmp_status != CMP_OK) {
            std::printf("Error %d: Saving processed file %s\n", cmp_status, destPath.c_str());
            return;
        }
    }

    void ReadCompressedDataFromDDSFile(OpenGLTexture* texture) {
        std::vector<TextureData> mipLevelsData;

        const auto ddsPath = std::string(ASSETS_DIR) + "textures/compressed/" + texture->GetStem() + ".dds";
        if (!File::Exists(ddsPath)) {
            Warn("There is no DDS file with this name: " + ddsPath);
            return;
        }

        std::ifstream file(ddsPath, std::ios::binary);
        if (!file.is_open()) {
            Warn("Can't open DDS file: " + ddsPath);
            return;
        }

        // Check that the file is a valid DDS file, DirectX::DDS_MAGIC = "DDS "
        uint32_t magicNumber;
        file.read((char*)&magicNumber, sizeof(magicNumber));
        if (!file) {
            Warn("Failed to read magic number for DDS: " + ddsPath);
            return;
        }
        if (magicNumber != 0x20534444) { // 0x20534444 = DDS Magic number
            Warn("This file is not a DDS file!! path: " + ddsPath);
        }

        DDSHeader header = {};
        if (!file.read((char*)&header, sizeof(DDSHeader))) {
            Warn("Failed to read DDSHeader: " + ddsPath);
            return;
        }
        if (header.dwSize != 124) {
            Warn("Shit happened for magic 124!");
        }

        DDSHeaderDX10 dx10Header = {};
        if (header.ddspf_dwFourCC == CMP_MAKEFOURCC('D', 'X', '1', '0')) {
            if (!file.read((char*)&dx10Header, sizeof(DDSHeaderDX10))) {
                Warn("Failed to read DDSHeaderDX10: " + ddsPath);
                return;
            }
        }

        auto [internalFormat, format, blockSize, channelCount] = GetDDSFormatInfo(header, &dx10Header);

        uint32_t mipWidth = header.dwWidth;
        uint32_t mipHeight = header.dwHeight;
        for (size_t level = 0; level < header.dwMipMapCount; level++) {
            uint32_t blocksWide = (mipWidth + 3) / 4;
            uint32_t blocksHigh = (mipHeight + 3) / 4;
            uint32_t dataSize = blocksWide * blocksHigh * blockSize;

            TextureData data = {};
            data.m_Width          = (int)mipWidth;
            data.m_Height         = (int)mipHeight;
            data.m_DataSize       = (int)dataSize;
            data.m_Format         = format;
            data.m_InternalFormat = internalFormat;
            data.m_ChannelCount   = channelCount;

            if (format == 0 || internalFormat == 0) {
                Warn("Format or InternalFormat is UNDEFINED for: " + texture->GetName());
                continue;
            }

            // Allocate and read
            data.m_Data = new uint8_t[dataSize];
            file.read((char*)data.m_Data, dataSize);

            // Update dimensions for next mipmap level
            mipWidth  = std::max(1u, mipWidth  >> 1);
            mipHeight = std::max(1u, mipHeight >> 1);

            mipLevelsData.push_back(data);
        }

        file.close();

        if (mipLevelsData.empty()) {
            Warn("Mip levels data is empty!!!");
            return;
        }

        texture->SetMipLevelsData(mipLevelsData);
        Info("Loaded dds data for: " + ddsPath);
    }

}
