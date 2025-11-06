//
// Created by pointerlost on 10/30/25.
//
#include <Tools/ImageTools.h>
#include "compressonator/include/cmp_compressonatorlib/compressonator.h"
#include "Core/Logger.h"
#include "Graphics/Material.h"
#include <glad/include/glad/glad.h>
#include <GL/glext.h>

#include "Core/AssetManager.h"
#include "Core/Services.h"

namespace Real::tools {

    Ref<Texture> PackTexturesToChannels(const Ref<Texture> &tex1, const Ref<Texture> &tex2, const Ref<Texture> &tex3) {
        if (!tex1 || !tex2 || !tex3) {
            Warn("There is no texture! from: " + std::string(__FILE__));
            return {};
        }

        constexpr int channelCount = 3; // RGB
        Ref<Texture> mixedTexture = CreateRef<Texture>();
        // Copy all the data to the mixedTex and only change what is necessary
        if (tex1) {
            mixedTexture->GetData() = tex1->GetData();
            mixedTexture->SetFileInfo(tex1->GetFileInfo());
        }
        else if (tex2) {
            mixedTexture->GetData() = tex2->GetData();
            mixedTexture->SetFileInfo(tex2->GetFileInfo());
        }
        else if (tex3) {
            mixedTexture->GetData() = tex3->GetData();
            mixedTexture->SetFileInfo(tex3->GetFileInfo());
        }

        // Set new props to mixedTex
        mixedTexture->GetData().m_DataSize *= channelCount;
        mixedTexture->GetData().m_ChannelCount = channelCount;

        auto* roughnessRawData = new uint8_t[mixedTexture->GetData().m_DataSize];
        std::fill_n(roughnessRawData, mixedTexture->GetData().m_DataSize, 0.5);
        if (tex1) roughnessRawData = static_cast<uint8_t*>(tex1->GetData().m_Data);
        auto* metallicRawData = new uint8_t[mixedTexture->GetData().m_DataSize];
        std::fill_n(metallicRawData, mixedTexture->GetData().m_DataSize, 0.0);
        if (tex2) metallicRawData = static_cast<uint8_t*>(tex2->GetData().m_Data);
        auto* aoRawData = new uint8_t[mixedTexture->GetData().m_DataSize];
        std::fill_n(aoRawData, mixedTexture->GetData().m_DataSize, 1.0);
        if (tex3) aoRawData = static_cast<uint8_t*>(tex3->GetData().m_Data);

        // Create mixedTexture data
        const auto mixedTexRawData = new uint8_t[mixedTexture->GetData().m_DataSize];

        // All textures have the same resolution
        const int texWidth = mixedTexture->GetData().m_Width;
        const int texHeight = mixedTexture->GetData().m_Height;

        for (int y = 0; y < texHeight; y++) {
            for (int x = 0; x < texWidth; x++) {
                const int pixelIdx = (y * texWidth + x) * channelCount;
                mixedTexRawData[pixelIdx + 0] = roughnessRawData[pixelIdx + 0];
                mixedTexRawData[pixelIdx + 1] = metallicRawData[pixelIdx + 1];
                mixedTexRawData[pixelIdx + 2] = aoRawData[pixelIdx + 2];
            }
        }
        // Copy the local mixed text data into the original text data to free up memory
        memcpy(mixedTexture->GetData().m_Data, mixedTexRawData, mixedTexture->GetData().m_DataSize);
        delete[] mixedTexRawData;
        delete[] roughnessRawData;
        delete[] metallicRawData;
        delete[] aoRawData;

        const std::string name = mixedTexture->GetFileInfo().name;
        const std::string newName = name.substr(0, name.size() - 4) + "_RMA";
        Info(newName);
        mixedTexture->GetFileInfo().name = newName;

        return std::move(mixedTexture);
    }

    Ref<Texture> PackTexturesToChannels(const std::array<Ref<Texture>, 3>& textures) {
        return PackTexturesToChannels(textures[0], textures[1], textures[2]);
    }

    void CompressTextureToBCn(Ref<Texture>& texture, const std::string& destPath) {
        if (Services::GetAssetManager()->IsTextureCompressed(texture->GetName())) {
            Info("Texture already compressed! Texture name: " + texture->GetName());
            return;
        }

        auto& texData = texture->GetData();
        const int inputPixelCount   = texData.m_Width * texData.m_Height;
        const int inputChannelCount = texData.m_ChannelCount;

        int8_t* inputData = new int8_t[texData.m_DataSize];

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

            default:
                // Use it BC7 for now
                destFormat = CMP_FORMAT_BC7; // 8-bit
                texData.m_InternalFormat = GL_COMPRESSED_RGBA_BPTC_UNORM; // match it to BC7
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
