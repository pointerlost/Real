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
        Ref<OpenGLTexture> nonDefaultTexFromORM;
        if (ao->GetImageFormatState() != ImageFormatState::DEFAULT) {
            nonDefaultTexFromORM = ao;
        } else if (rgh->GetImageFormatState() != ImageFormatState::DEFAULT) {
            nonDefaultTexFromORM = rgh;
        } else if (mtl->GetImageFormatState() != ImageFormatState::DEFAULT) {
            nonDefaultTexFromORM = mtl;
        } else {
            nonDefaultTexFromORM = ao;
        }

        Ref<OpenGLTexture> mixedTexture = CreateRef<OpenGLTexture>();
        const int width  = nonDefaultTexFromORM->GetLevelData(0).m_Width;
        const int height = nonDefaultTexFromORM->GetLevelData(0).m_Height;

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
        mixedData.m_Format   = util::ConvertChannelCountToGLFormat(channelCount);
        mixedData.m_InternalFormat = util::ConvertChannelCountToGLInternalFormat(channelCount);

        FileInfo info;
        info.name = materialName + "_ORM" + nonDefaultTexFromORM->GetFileInfo().ext;
        info.stem = info.name.substr(0, info.name.size() - 4); // Without extension
        info.path = std::string(ASSETS_DIR) + "textures/compressed/" + info.name;
        info.ext  = info.name.substr(info.name.size() - 4);

        mixedTexture->SetImageFormatState(nonDefaultTexFromORM->GetImageFormatState());
        mixedTexture->SetFileInfo(info);
        mixedTexture->CreateFromData(mixedData, TextureType::ORM);

        // Clear seperated textures
        const auto& am = Services::GetAssetManager();
        fs::File::DeleteFile(ao->GetPath());
        am->DeleteCPUTexture(ao->GetUUID());
        fs::File::DeleteFile(rgh->GetPath());
        am->DeleteCPUTexture(rgh->GetUUID());
        fs::File::DeleteFile(mtl->GetPath());
        am->DeleteCPUTexture(mtl->GetUUID());

        if (!SaveTextureAsFile(mixedTexture.get(), info.path)) {
            Warn("ORM packed texture can't saved!");
        }

        return mixedTexture;
    }

    Ref<OpenGLTexture> PackTexturesToRGBChannels(const std::array<Ref<OpenGLTexture>, 3> &orm,
        const std::string &materialName)
    {
        return PackTexturesToRGBChannels(orm[0], orm[1], orm[2], materialName);
    }

    Ref<OpenGLTexture> PrepareAndPackRMATextures(std::array<Ref<OpenGLTexture>, 3>& orm, const std::string &materialName) {
        const auto& am = Services::GetAssetManager();
        std::pair<int, int> max_res{};
        FileInfo info;
        auto imagestate = ImageFormatState::UNDEFINED;
        // Detection for missing cases
        for (const auto& tex : orm) {
            if (!tex) continue;
            auto res = tex->GetResolution(0);
            max_res = (res.first * res.second > max_res.first * max_res.second) ? res : max_res;
            info = tex->GetFileInfo();
            imagestate = tex->GetImageFormatState();
        }

        while (max_res.first == 0 || max_res.second == 0) {
            const auto& ao  = am->GetTexture(orm[0]->GetUUID(), TextureType::AMBIENT_OCCLUSION);
            const auto& rgh = am->GetTexture(orm[1]->GetUUID(), TextureType::ROUGHNESS);
            const auto& mtl = am->GetTexture(orm[2]->GetUUID(), TextureType::METALLIC);
            if (ao) {
                max_res = max_res > ao->GetResolution(0) ? ao->GetResolution(0) : max_res;
            }
            else if (rgh) {
                max_res = max_res > rgh->GetResolution(0) ? rgh->GetResolution(0) : max_res;
            }
            else if (mtl) {
                max_res = max_res > mtl->GetResolution(0) ? mtl->GetResolution(0) : max_res;
            } else {
                max_res = {1, 1};
            }
        }

        for (size_t i = 0; i < 3; i++) {
            if (orm[i]) continue;
            std::string missingName;
            auto type = TextureType::UNDEFINED;
            switch (i) {
                case 0: missingName = "default_RGH"; type = TextureType::ROUGHNESS; break;
                case 1: missingName = "default_MTL"; type = TextureType::METALLIC;  break;
                case 2: missingName = "default_AO";  type = TextureType::AMBIENT_OCCLUSION; break;
                default: ;
            }

            missingName += std::to_string(max_res.first) + '_' + std::to_string(max_res.second);
            orm[i] = am->GetOrCreateDefaultTexture(TextureType::ORM);
        }

        for (const auto &tex : orm) {
            tex->SetFileInfo(info);
            tex->SetImageFormatState(imagestate);

            const auto [fst, snd] = tex->GetResolution(0);
            if (fst != max_res.first || snd != max_res.second) {
                tex->Resize(glm::ivec2(max_res.first, max_res.second), 0);
            }
        }

        return PackTexturesToRGBChannels(orm, materialName);
    }

    bool SaveTextureAsFile(OpenGLTexture* texture, const std::string&destPath, int jpgQuality) {
        const auto& [width, height] = texture->GetResolution(0);
        const auto cc = texture->GetChannelCount(0);
        const auto stride_in_bytes = cc * width;

        const auto extension = texture->GetFileInfo().ext;

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

    void CompressTextureToBCn(OpenGLTexture* texture) {
        if (!texture) {
            Warn("[CompressTextureToBCn] Texture nullptr!");
            return;
        }
        if (Services::GetAssetManager()->IsTextureCompressed(texture->GetStem())) {
            ReadCompressedDataFromDDSFile(texture);
            return;
        }

        const auto [width, height]  = texture->GetResolution(0);
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

        MipSetIn.m_format = srcFormat;
        CMP_LoadTexture(texture->GetFileInfo().path.c_str(), &MipSetIn);

        // Pick 4 as min level of mipmap coz compressed textures getting minimum 4x4
        if (CMP_GenerateMIPLevels(&MipSetIn, 4) != CMP_OK) {
            Warn("CMP_GenerateMIPLevels failed!");
            return;
        }

        // TODO: Treat it as 8-bit everything except the 16-bit We don't have 16 bit option yet!
        CMP_FORMAT destFormat = util::GetCMPDestinationFormat(channelCount);
        MipSetCmp.m_format = destFormat;

        kernel_options.encodeWith = CMP_HPC; // CMP_CPU | CMP_GPU_OCL
        kernel_options.srcformat  = srcFormat;
        kernel_options.format     = destFormat;
        kernel_options.fquality   = 0.9f;
        kernel_options.width      = width;
        kernel_options.height     = height;

        cmp_status = CMP_ProcessTexture(&MipSetIn, &MipSetCmp, kernel_options, nullptr);

        if (cmp_status != CMP_OK) {
            Warn("Damn, cmp_status is failed!");
            return;
        }

        // Save the result to compressed folder
        const auto compressed_dir = std::string(ASSETS_DIR) + "textures/compressed/";
        const std::string fullName = compressed_dir + texture->GetFileInfo().stem + ".dds";
        cmp_status = CMP_SaveTexture(fullName.c_str(), &MipSetCmp);

        // Clean up buffers
        CMP_FreeMipSet(&MipSetIn);
        CMP_FreeMipSet(&MipSetCmp);

        texture->SetImageFormatState(ImageFormatState::COMPRESSED);

        if (cmp_status != CMP_OK) {
            std::printf("Error %d: Saving processed file %s\n", cmp_status, compressed_dir.c_str());
        }
    }

    void CompressTextureAndReadFromFile(OpenGLTexture *texture) {
        CompressTextureToBCn(texture);
        ReadCompressedDataFromDDSFile(texture);
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

        MipSetIn.m_format = srcFormat;
        if (CMP_CreateMipSet(&MipSetIn, width, height, 1, CF_8bit, TT_2D) != CMP_OK) {
            Warn("Mipmap creation failed!");
            return;
        }

        CMP_MipLevel* mipLevel = MipSetIn.m_pMipLevelTable[0];
        mipLevel->m_dwLinearSize = texFirstMipLevelData.m_DataSize;
        mipLevel->m_nWidth  = width;
        mipLevel->m_nHeight = height;

        mipLevel->m_pbData = (CMP_BYTE*)malloc(mipLevel->m_dwLinearSize);
        memcpy(mipLevel->m_pbData, texFirstMipLevelData.m_Data, mipLevel->m_dwLinearSize);

        if (CMP_GenerateMIPLevels(&MipSetIn, 4) != CMP_OK) {
            Warn("CMP_GenerateMIPLevels failed!");
            return;
        }

        std::vector<TextureData> mipLevelsData;
        for (size_t level = 0; level < MipSetIn.m_nMipLevels; level++) {
            auto currLevel = MipSetIn.m_pMipLevelTable[level];
            TextureData levelData;
            levelData.m_Data = new uint8_t[currLevel->m_dwLinearSize];
            memcpy(levelData.m_Data, currLevel->m_pbData, currLevel->m_dwLinearSize);
            levelData.m_DataSize = currLevel->m_dwLinearSize;
            levelData.m_Width    = currLevel->m_nWidth;
            levelData.m_Height   = currLevel->m_nHeight;
            levelData.m_ChannelCount   = channelCount;
            levelData.m_Format         = util::ConvertChannelCountToGLFormat(channelCount);
            levelData.m_InternalFormat = util::GetCompressedInternalFormat(channelCount);
            mipLevelsData.push_back(levelData);
        }

        CMP_FORMAT destFormat = util::GetCMPDestinationFormat(channelCount);

        kernel_options.encodeWith = CMP_HPC;
        kernel_options.format     = destFormat;
        kernel_options.fquality   = 0.9f;
        kernel_options.width      = width;
        kernel_options.height     = height;

        cmp_status = CMP_ProcessTexture(&MipSetIn, &MipSetCmp, kernel_options, nullptr);

        if (cmp_status != CMP_OK) {
            Warn("Damn, cmp_status is failed! Error type: " + util::DebugCMPStatus(cmp_status));
            return;
        }

        // Save the result to compressed folder
        std::string fullName = destPath + texture->GetFileInfo().stem + ".dds";
        cmp_status = CMP_SaveTexture(fullName.c_str(), &MipSetCmp);

        texture->SetMipLevelsData(mipLevelsData);
        texture->SetImageFormatState(ImageFormatState::COMPRESSED);

        // Clean up buffers
        CMP_FreeMipSet(&MipSetIn);
        CMP_FreeMipSet(&MipSetCmp);

        if (cmp_status != CMP_OK) {
            std::printf("Error %d: Saving processed file %s\n", cmp_status, destPath.c_str());
            return;
        }
    }

    void ReadCompressedDataFromDDSFile(OpenGLTexture* texture) {
        std::vector<TextureData> mipLevelsData;

        const auto ddsPath = std::string(ASSETS_DIR) + "textures/compressed/" + texture->GetStem() + ".dds";
        if (!fs::File::Exists(ddsPath)) {
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
                Warn("Format or InternalFormat is UNDEFINED for: " + texture->GetName());
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
            return;
        }

        texture->SetMipLevelsData(mipLevelsData);
    }

}
