//
// Created by pointerlost on 10/30/25.
//
#include <Tools/ImageTools.h>
#include "compressonator/include/cmp_compressonatorlib/compressonator.h"
#include "Core/Logger.h"
#include "Graphics/Material.h"
#include "Core/AssetManager.h"
#include "Core/Services.h"
#include "Math/Math.h"
#include "Util/Util.h"

namespace Real::tools {

    Ref<Texture> PackTexturesToRGBChannels(const Ref<Texture> &tex1, const Ref<Texture> &tex2, const Ref<Texture> &tex3) {
        if (!tex1 && !tex2 && !tex3) {
            Warn("There is no texture! from: " + std::string(__FILE__));
            return {};
        }
        Ref<Texture> mixedTexture = CreateRef<Texture>();
        // mixedTexture->SetData(tex1->GetData());

        const auto tex1Data = static_cast<uint8_t *>(tex1->GetData().m_Data);
        const auto tex2Data = static_cast<uint8_t *>(tex2->GetData().m_Data);
        const auto tex3Data = static_cast<uint8_t *>(tex3->GetData().m_Data);

        // All textures have the same resolution
        const int texWidth = mixedTexture->GetData().m_Width;
        const int texHeight = mixedTexture->GetData().m_Height;
        constexpr int channelCount = 3;
        const int dataSize = texWidth * texHeight * channelCount * 1;

        // Create mixedTexture data
        mixedTexture->GetData().m_Data = new uint8_t[dataSize];
        uint8_t* mixedTexRawData = static_cast<uint8_t*>(mixedTexture->GetData().m_Data);

        for (int y = 0; y < texHeight; y++) {
            for (int x = 0; x < texWidth; x++) {
                const int p = y * texWidth + x;

                const int src1 = p * tex1->GetData().m_ChannelCount;
                const int src2 = p * tex2->GetData().m_ChannelCount;
                const int src3 = p * tex3->GetData().m_ChannelCount;

                const int dst = p * 3;

                mixedTexRawData[dst + 0] = tex1Data[src1];
                mixedTexRawData[dst + 1] = tex2Data[src2];
                mixedTexRawData[dst + 2] = tex3Data[src3];
            }
        }
        auto& mixedData = mixedTexture->GetData();
        // Copy the local mixed text data into the original text data to free up memory

        mixedData.m_ChannelCount = 3;
        mixedData.m_DataSize = dataSize;
        mixedData.m_Format = util::ConvertChannelCountToGLType(3, mixedTexture->GetName());
        mixedData.m_ImageCompressType = util::PickTextureCompressionType(util::StringToEnumTextureType("RMA"));
        mixedData.m_InternalFormat = util::CompressTypeToGLEnum(mixedTexture->GetData().m_ImageCompressType);

        return mixedTexture;
    }

    Ref<Texture> PackTexturesRGBToChannels(const std::array<Ref<Texture>, 3> &textures) {
        return PackTexturesToRGBChannels(textures[0], textures[1], textures[2]);
    }

    void CompressTextureToBCn(Ref<Texture>& texture, const std::string& destPath) {
        if (!texture) {
            Warn("[CompressTextureToBCn] Texture nullptr!");
            return;
        }

        auto& texData = texture->GetData();
        const int inputPixelCount   = texData.m_Width * texData.m_Height;
        const int inputChannelCount = texData.m_ChannelCount;

        uint8_t* inputData = new uint8_t[texData.m_DataSize];

        // Info(util::TextureTypeEnumToString(texture->GetType())  + ": " + std::to_string(texData.m_DataSize));
        for (int i = 0; i < inputPixelCount; ++i)
        {
            inputData[i * inputChannelCount + 0] = 0xFF;
            inputData[i * inputChannelCount + 1] = 0xFF;
            inputData[i * inputChannelCount + 2] = 0x00;
            inputData[i * inputChannelCount + 3] = 0xFF;
        }

        // Init framework plugin and IO interfaces
        CMP_InitFramework();

        // TODO: Treat it as 8-bit everything except the 16-bit We don't have 16 bit option yet!
        CMP_FORMAT destFormat;
        destFormat = util::GetCMPFormatWithCompressType(texData.m_ImageCompressType);

        // Create the MipSet to hold the input data
        CMP_MipSet inputTexture = {};
        // TODO: Treat it as 8-bit everything except the 16-bit We don't have 16 bit option yet!
        if (CMP_CreateMipSet(&inputTexture, texData.m_Width, texData.m_Height, 1, CF_8bit, TT_2D) != CMP_OK) {
            Error("[CMP_CreateMipSet] Failed to create MipSet");
            return;
        }

        // Copy external data into the created MipSet
        CMP_MipLevel* baseLevel = nullptr;
        CMP_GetMipLevel(&baseLevel, &inputTexture, 0, 0);

        memcpy(baseLevel->m_pbData, inputData, inputPixelCount * inputChannelCount);
        // Free memory after copy
        delete[] inputData;

        // Setup a results buffer for the processed data
        CMP_MipSet resultTexture = {};
        resultTexture.m_format = destFormat;

        // Create a destination buffer based on destination format and MipSet source input
        CMP_CreateCompressMipSet(&resultTexture, &inputTexture);

        // Check we have an image buffer
        if (resultTexture.pData == nullptr) {
            Error("Destination buffer was not allocated");
            return;
        }

        // Compress the source image
        CMP_ERROR status = CMP_OK;

        void* bcnEncoder = nullptr;
        CMP_EncoderSetting encodeSettings = {};
        encodeSettings.format = destFormat;
        encodeSettings.quality = 0.5f;
        encodeSettings.width = inputTexture.m_nWidth;
        encodeSettings.height = inputTexture.m_nHeight;

        if (CMP_CreateBlockEncoder(&bcnEncoder, encodeSettings) != 0) {
            CMP_FreeMipSet(&inputTexture);
            CMP_FreeMipSet(&resultTexture);
            Error("Creation failure for BCn block encoder");
            return;
        }

        // Compress the source image in blocks of 4x4 Texels
        CMP_UINT numBlocksX = inputTexture.m_nWidth / 4;
        CMP_UINT numBlocksY = inputTexture.m_nHeight / 4;
        CMP_UINT srcStride = inputTexture.m_nWidth / 4;
        CMP_UINT dstStride = resultTexture.m_nWidth / 8;

        texture->GetData().m_ImageSize = numBlocksX * numBlocksY * util::TexFormat_compressed_GetBytesPerBlock(texture->GetData().m_ImageCompressType);

        for (CMP_UINT y = 0; y < numBlocksY; ++y)
        {
            for (CMP_UINT x = 0; x < numBlocksX; ++x)
            {
                std::printf("Processing block %3d %3d \r", x, y);

                if (CMP_CompressBlockXY(&bcnEncoder, x, y, (void*)inputTexture.pData, srcStride, (void*)resultTexture.pData, dstStride) != 0) {
                    std::printf("Error for processing block (%d, %d)", x, y);
                    CMP_FreeMipSet(&inputTexture);
                    CMP_FreeMipSet(&resultTexture);
                    return;
                }
            }
        }

        CMP_DestroyBlockEncoder(&bcnEncoder);

        // Save the result to a file
        if (texture->GetFileInfo().stem.empty()) {
            Info(util::TextureTypeEnumToString(texture->GetType()));
        }
        std::string fullName = destPath + texture->GetFileInfo().stem + ".dds";
        status = CMP_SaveTexture(fullName.c_str(), &resultTexture);

        // Clean up buffers
        CMP_FreeMipSet(&inputTexture);
        CMP_FreeMipSet(&resultTexture);

        if (status != CMP_OK) {
            std::printf("Error %d: Saving processed file %s\n", status, destPath.c_str());
            return;
        }

        texture->SetImageFormat(ImageFormatState::COMPRESSED);

        Info(ConcatStr("Texture compression done for path: ", fullName));
    }
}
