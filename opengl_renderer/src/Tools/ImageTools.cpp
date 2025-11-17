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

namespace Real::tools {

    Ref<OpenGLTexture> PackTexturesToRGBChannels(const Ref<OpenGLTexture> &tex1, const Ref<OpenGLTexture> &tex2, const Ref<OpenGLTexture> &tex3) {
        if (!tex1 && !tex2 && !tex3) {
            Warn("There is no texture! from: " + std::string(__FILE__));
            return {};
        }

        Ref<OpenGLTexture> mixedTexture = CreateRef<OpenGLTexture>(ImageFormatState::UNCOMPRESSED);
        auto& data = mixedTexture->GetLevelData(0);

        const auto tex1RawData = static_cast<uint8_t *>(tex1->GetLevelData(0).m_Data);
        const auto tex2RawData = static_cast<uint8_t *>(tex2->GetLevelData(0).m_Data);
        const auto tex3RawData = static_cast<uint8_t *>(tex3->GetLevelData(0).m_Data);

        const auto tex1LevelData = tex1->GetLevelData(0);
        const auto tex2LevelData = tex2->GetLevelData(0);
        const auto tex3LevelData = tex3->GetLevelData(0);
        // If textures has different resolutions, resize them
        int width = 0,height = 0;
        if (!(tex1LevelData.m_Width == tex2LevelData.m_Width && tex2LevelData.m_Width == tex3LevelData.m_Width)
            && !(tex1LevelData.m_Height == tex2LevelData.m_Height && tex2LevelData.m_Height == tex3LevelData.m_Height))
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

        // All textures have the same resolution
        constexpr int channelCount = 3;
        const int dataSize = width * height * channelCount * 1;

        // Create mixedTexture data
        data.m_Data = new uint8_t[dataSize];
        uint8_t* mixedTexRawData = static_cast<uint8_t*>(data.m_Data);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                const int p = y * width + x;
                mixedTexRawData[p * 3 + 0] = tex1RawData[p * channelCount];
                mixedTexRawData[p * 3 + 1] = tex2RawData[p * channelCount];
                mixedTexRawData[p * 3 + 2] = tex3RawData[p * channelCount];
            }
        }

        constexpr int mipLevel = 0;
        mixedTexture->SetChannelCount(3, mipLevel);
        mixedTexture->SetDataSize(dataSize, mipLevel);
        mixedTexture->SetFormat(util::ConvertChannelCountToGLType(3, mixedTexture->GetName()), mipLevel);
        mixedTexture->SetInternalFormat(util::ImageCompressTypeToGLEnum(mixedTexture->GetCompressionType()), mipLevel);
        mixedTexture->SetCompressionType(util::PickTextureCompressionType(util::StringToEnum_TextureType("RMA")));
        mixedTexture->SetDataSize(dataSize, mipLevel);

        if (!mixedTexture) {
            Warn("[PackTexturesToRGBChannels] Mixed Texture is nullptr!");
            return {};
        }

        return mixedTexture;
    }

    Ref<OpenGLTexture> PackTexturesToRGBChannels(const std::array<Ref<OpenGLTexture>, 3> &textures) {
        return PackTexturesToRGBChannels(textures[0], textures[1], textures[2]);
    }

    void CompressTextureToBCn(const Ref<OpenGLTexture>& texture, const std::string& destPath, ImageCompressedType compressionType) {
        if (!texture) {
            Warn("[CompressTextureToBCn] Texture nullptr!");
            return;
        }
        if (texture->IsCompressed()) return;

        const auto [width, height] = texture->GetResolution(0);
        const auto mipmapCount = texture->GetMipMapCount();

        // Init framework plugin and IO interfaces
        CMP_InitFramework();

        // Compress the source image
        CMP_ERROR cmp_status = CMP_OK;

        // TODO: Treat it as 8-bit everything except the 16-bit We don't have 16 bit option yet!
        CMP_FORMAT destFormat;
        destFormat = util::GetCMPFormat(compressionType);

        // TODO: Treat it as 8-bit everything except the 16-bit We don't have 16 bit option yet!
        // Setup a results buffer for the processed data
        CMP_MipSet MipSetIn = {};
        MipSetIn.m_format  = destFormat;
        MipSetIn.m_nWidth  = width;
        MipSetIn.m_nHeight = height;
        MipSetIn.m_ChannelFormat = CF_8bit; // TODO: Fix me! if you want to use 16-bit options
        // Specify the number of mipmaps to create mipmapCount times
        MipSetIn.m_nMipLevels = mipmapCount;

        CMP_GenerateMIPLevels(&MipSetIn, mipmapCount);

        KernelOptions kernel_options = {};
        kernel_options.format = destFormat;
        kernel_options.fquality = 1.0f;
        kernel_options.width  = MipSetIn.m_nWidth;
        kernel_options.height = MipSetIn.m_nHeight;

        CMP_MipSet MipSetCmp = {};
        MipSetCmp.m_format = destFormat;

        for (size_t level = 0; level < mipmapCount; level++) {
            auto& texData = texture->GetLevelData((int)level);
            texData.m_InternalFormat = util::ImageCompressTypeToGLEnum(compressionType);
            CMP_MipLevel* mipLevel = {};
            CMP_GetMipLevel(&mipLevel, &MipSetIn, (int)level, 0);
            memcpy(mipLevel->m_pbData, texData.m_Data, texData.m_DataSize);
        }

        cmp_status = CMP_ProcessTexture(&MipSetIn, &MipSetCmp, kernel_options, nullptr);

        // Save the result to a file
        if (texture->GetFileInfo().stem.empty()) {
            Warn(util::EnumToString_TextureType(texture->GetType()));
        }
        std::string fullName = destPath + texture->GetFileInfo().stem + ".dds";
        cmp_status = CMP_SaveTexture(fullName.c_str(), &MipSetCmp);

        // Clean up buffers
        CMP_FreeMipSet(&MipSetIn);
        CMP_FreeMipSet(&MipSetCmp);

        if (cmp_status != CMP_OK) {
            std::printf("Error %d: Saving processed file %s\n", cmp_status, destPath.c_str());
            return;
        }

        texture->SetImageFormat(ImageFormatState::COMPRESSED);

        Info(ConcatStr("Texture compression done for path: ", fullName));
    }

    std::vector<TextureData> ReadCompressedDataFromDDSFile(OpenGLTexture* texture) {
        std::vector<TextureData> mipLevelsData;

        std::string filepath = texture->GetFileInfo().path;
        if (!File::Exists(filepath)) {
            Warn("There is no filepath:" + filepath);
            return{};
        }

        std::ifstream file(filepath, std::ios::binary | std::ios::ate);

        // Check that the file is a valid DDS file, DirectX::DDS_MAGIC = "DDS "
        uint32_t magicNumber;
        file.read((char*)&magicNumber, sizeof(magicNumber));
        if (magicNumber != 0x20534444) { // 0x20534444 = DDS Magic number
            Warn("This file is not a DDS file!! path: " + filepath);
        }

        DDSHeader header = {};
        if (!file.read((char*)&header, sizeof(DDSHeader))) {
            Warn("Failed to read DDSHeader: " + filepath);
            return {};
        }

        DDSHeaderDX10 dx10Header = {};
        if (header.ddspf_dwFourCC == CMP_MAKEFOURCC('D', 'X', '1', '0')) {
            if (file.read((char*)&dx10Header, sizeof(DDSHeaderDX10))) {
                Warn("Failed to read DDSHeaderDX10: " + filepath);
                return {};
            }
        }

        DDSFormatInfo formatInfo = GetDDSFormatInfo(header, &dx10Header);

        uint32_t mipWidth = header.dwWidth;
        uint32_t mipHeight = header.dwHeight;
        for (size_t level = 0; level < header.dwMipMapCount; level++) {
            uint32_t blocksWide = (mipWidth + 3) / 4;
            uint32_t blocksHigh = (mipHeight + 3) / 4;
            uint32_t dataSize = blocksWide * blocksHigh * formatInfo.blockSize;

            TextureData data = {};
            data.m_Width    = (int)mipWidth;
            data.m_Height   = (int)mipHeight;
            data.m_DataSize = (int)dataSize;
            data.m_Format   = formatInfo.format;
            data.m_InternalFormat = formatInfo.internalFormat;
            data.m_ChannelCount   = formatInfo.channelCount;
            // Allocate and read
            data.m_Data = new uint8_t[dataSize];
            file.read((char*)data.m_Data, dataSize);

            // Update dimensions for next mipmap level
            mipWidth  = std::max(1u, mipWidth / 2);
            mipHeight = std::max(1u, mipHeight / 2);

            mipLevelsData.push_back(data);
        }

        file.close();

        return mipLevelsData;
    }

}
