//
// Created by pointerlost on 10/30/25.
//
#include <GL/glext.h>
#include <Tools/ImageTools.h>
#include "compressonator/include/cmp_compressonatorlib/compressonator.h"
#include "compressonator/include/cmp_framework/common/hdr_encode.h"
#include "compressonator/include/cmp_framework/common/cmp_mips.h"
#include "compressonator/include/cmp_core/shaders/bc1_cmp.h"
#include "compressonator/include/cmp_core/shaders/bc7_cmpmsc.h"
#include "compressonator/include/cmp_framework/compute_base.h"
#include "compressonator/include/cmp_framework/common/half/tofloat.h"
#include "Core/Logger.h"
#include "Graphics/Material.h"

namespace Real::Tools {

    Ref<Texture> PackRMATexturesToChannels(const Ref<Texture> &roughness, const Ref<Texture> &metallic, const Ref<Texture> &ao) {
        constexpr int channelCount = 3; // RGB
        Ref<Texture> mixedTexture = CreateRef<Texture>();
        // Copy all the data to the mixedTex and only change what is necessary
        mixedTexture->m_Data = roughness->m_Data;

        // Set new props to mixedTex
        mixedTexture->m_Data.m_DataSize = roughness->m_Data.m_DataSize * channelCount;
        mixedTexture->m_Data.m_ChannelCount = channelCount;

        const auto roughnessData = roughness->m_Data;
        const auto metallicData = metallic->m_Data;
        const auto aoData = ao->m_Data;

        const auto roughnessRawData = static_cast<uint8_t *>(roughnessData.m_Data);
        const auto metallicRawData = static_cast<uint8_t *>(metallicData.m_Data);
        const auto aoRawData = static_cast<uint8_t *>(aoData.m_Data);

        // Create mixedTexture data
        const auto mixedTexRawData = new uint8_t[mixedTexture->m_Data.m_DataSize];

        // All textures have the same resolution
        const int texWidth = roughnessData.m_Width;
        const int texHeight = roughnessData.m_Height;

        for (int y = 0; y < texHeight; y++) {
            for (int x = 0; x < texWidth; x++) {
                const int pixelIdx = (y * texWidth + x) * channelCount;
                mixedTexRawData[pixelIdx + 0] = roughnessRawData[pixelIdx + 0];
                mixedTexRawData[pixelIdx + 1] = metallicRawData[pixelIdx + 1];
                mixedTexRawData[pixelIdx + 2] = aoRawData[pixelIdx + 2];
            }
        }
        mixedTexture->m_Data.m_Data = static_cast<void *>(mixedTexRawData);

        return std::move(mixedTexture);
    }

    void CompressTextureToBCn(Ref<Texture>& texture, const char *destPath) {
        if (texture->m_ImageFormatState == ImageFormatState::COMPRESSED) {
            Info("Texture already compressed!");
            return;
        }

        auto& texData = texture->m_Data;
        const int inputPixelCount   = texData.m_Width * texData.m_Height;
        const int inputChannelCount = texData.m_ChannelCount;

        const Scope<int8_t> inpData = CreateScope<int8_t>(texData.m_DataSize);
        int8_t* inputData = inpData.get();

        for (int i = 0; i < inputPixelCount; ++i)
        {
            inputData[i * inputChannelCount + 0] = 0xFF;
            inputData[i * inputChannelCount + 1] = 0xFF;
            inputData[i * inputChannelCount + 2] = 0x00;
            inputData[i * inputChannelCount + 3] = 0xFF;
        }

        // Init framework plugin and IO interfaces
        CMP_InitFramework();

        // TODO: We can specify format texture-based like (albedo = BC1, so on.)
        // TODO: there is a problem: We are loading texture as 8-bit, but using 16-bit compression options!!
        CMP_FORMAT destFormat;
        // TODO: Could be written a converter like CMPformatToGLEnum etc.
        switch (texData.m_ImageCompressType) {
            case ImageCompressedType::BC1:
                destFormat = CMP_FORMAT_BC1; // 8-bit
                texData.m_InternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; // match it to BC1
                break;
            case ImageCompressedType::BC6:
                destFormat = CMP_FORMAT_BC6H; // This one using 16-bit option
                texData.m_InternalFormat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT; // match it to BC6H
                break;
            case ImageCompressedType::BC7:
                destFormat = CMP_FORMAT_BC7; // 8-bit
                texData.m_InternalFormat = GL_COMPRESSED_RGBA_BPTC_UNORM; // match it to BC7
                break;
        }

        // Create the MipSet to hold the input data
        CMP_MipSet inputTexture = {};
        if (CMP_CreateMipSet(&inputTexture, texData.m_Width, texData.m_Height, 1, CF_8bit, TT_2D) != CMP_OK) {
            Error("[CMP_CreateMipSet] Failed to create MipSet");
            return;
        }

        // Copy external data into the created MipSet
        CMP_MipLevel* baseLevel = nullptr;
        CMP_GetMipLevel(&baseLevel, &inputTexture, 0, 0);

        memcpy(baseLevel->m_pbData, inputData, inputPixelCount * inputChannelCount);

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
        status = CMP_SaveTexture(destPath, &resultTexture);

        // Clean up buffers
        CMP_FreeMipSet(&inputTexture);
        CMP_FreeMipSet(&resultTexture);

        if (status != CMP_OK) {
            std::printf("Error %d: Saving processed file %s\n", status, destPath);
            return;
        }

        texture->m_ImageFormatState = ImageFormatState::COMPRESSED;

        Info(ConcatStr("Texture compression done for path: ", destPath));
    }
}
