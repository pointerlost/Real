//
// Created by pointerlost on 10/30/25.
//
#include <cstring>
#include <fstream>

#include <Tools/Image/ImageTools.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "Assets/TextureManager.h"
#include "compressonator/include/cmp_compressonatorlib/compressonator.h"
#include "Core/CMakeConfig.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/OpenGL/OpenGLUtils.h"
#include "Tools/Compression/CompressionUtils.h"
#include "Tools/Image/DDS.h"


namespace {
    using namespace Real;

    bool CompressTexture_Internal(
        platform::opengl::OpenGLTexture* texture,
        CMP_MipSet& MipSetIn,
        f32 fQuality)
    {
        const int channelCount = texture->GetMipLevel(0).channelCount;
        const int width        = texture->GetMipLevel(0).width;
        const int height       = texture->GetMipLevel(0).height;

        CMP_FORMAT srcFormat;
        switch (channelCount) {
            case 1:  srcFormat = CMP_FORMAT_R_8;       break;
            case 2:  srcFormat = CMP_FORMAT_RG_8;      break;
            case 3:  srcFormat = CMP_FORMAT_RGB_888;   break;
            case 4:  srcFormat = CMP_FORMAT_RGBA_8888; break;
            default: srcFormat = CMP_FORMAT_RGBA_8888;
        }

        MipSetIn.m_format = srcFormat;

        if (CMP_GenerateMIPLevels(&MipSetIn, 4) != CMP_OK) {
            Warn("[CompressTexture] CMP_GenerateMIPLevels failed - name: " + texture->GetDebugName());
            CMP_FreeMipSet(&MipSetIn);
            return false;
        }

        CMP_MipSet    MipSetCmp      = {};
        KernelOptions kernel_options = {};
        const CMP_FORMAT destFormat  = util::compression::GetCMPDestinationFormat(channelCount);

        MipSetCmp.m_format        = destFormat;
        kernel_options.encodeWith = CMP_HPC;
        kernel_options.srcformat  = srcFormat;
        kernel_options.format     = destFormat;
        kernel_options.fquality   = fQuality;
        kernel_options.width      = width;
        kernel_options.height     = height;

        CMP_ERROR cmp_status = CMP_ProcessTexture(&MipSetIn, &MipSetCmp, kernel_options, nullptr);
        if (cmp_status != CMP_OK) {
            Warn("[CompressTexture] CMP_ProcessTexture failed: " + util::compression::DebugCMPStatus(cmp_status));
            CMP_FreeMipSet(&MipSetIn);
            CMP_FreeMipSet(&MipSetCmp);
            return false;
        }

        Vector<graphics::TextureData> mipLevelsData;
        mipLevelsData.reserve(MipSetCmp.m_nMipLevels);
        for (int level = 0; level < MipSetCmp.m_nMipLevels; level++) {
            auto* currLevel = MipSetCmp.m_pMipLevelTable[level];
            graphics::TextureData levelData;
            levelData.data           = new u8[currLevel->m_dwLinearSize];
            levelData.dataSize       = static_cast<int>(currLevel->m_dwLinearSize);
            levelData.width          = currLevel->m_nWidth;
            levelData.height         = currLevel->m_nHeight;
            levelData.channelCount   = channelCount;
            levelData.format         = util::opengl::GetGLFormat(channelCount);
            levelData.internalFormat = util::opengl::GetCompressedInternalFormat(channelCount);
            memcpy(levelData.data, currLevel->m_pbData, currLevel->m_dwLinearSize);
            mipLevelsData.push_back(levelData);
        }

        const String outPath = String(ASSETS_DIR) + "textures/compressed/"
                             + texture->GetDebugName() + ".dds";
        cmp_status = CMP_SaveTexture(outPath.c_str(), &MipSetCmp);

        CMP_FreeMipSet(&MipSetIn);
        CMP_FreeMipSet(&MipSetCmp);

        if (cmp_status != CMP_OK) {
            Warn("[CompressTexture] CMP_SaveTexture failed: " + outPath);
            // Fix: free collected mip data before returning
            for (auto& lvl : mipLevelsData)
                delete[] static_cast<u8*>(lvl.data);
            return false;
        }

        texture->SetFileInfo(fs::FileInfoFromPath(outPath));
        texture->SetMipLevels(mipLevelsData);
        texture->SetImageFormatState(graphics::ImageFormatState::COMPRESSED);
        return true;
    }

    /*
     *

    void ReloadCompressedIntoTexture(platform::opengl::OpenGLTexture* texture) {
        Vector<graphics::TextureData> levels;
        if (!ReadDDS(texture->GetFileInfo().path, levels)) {
            Warn("[ReloadCompressed] Failed to read back DDS - name: " + texture->GetDebugName());
            return;
        }
        texture->SetMipLevels(levels);
        texture->SetImageFormatState(graphics::ImageFormatState::COMPRESSED);
        // Manager calls UploadToGPU + SetGPUIndex when it flushes pending uploads
    }

    */
}

namespace Real::tools {

    bool CompressTexture(platform::opengl::OpenGLTexture* texture, f32 fQuality) {
        if (!texture) {
            Warn("[CompressTexture] Texture is nullptr!");
            return false;
        }

        if (Services::GetTextureManager().IsCompressed(texture->GetDebugName())) {
            const String fullPath = String(ASSETS_DIR) + "textures/compressed/"
                                  + texture->GetDebugName() + ".dds";
            texture->SetFileInfo(fs::FileInfoFromPath(fullPath));
            texture->SetImageFormatState(graphics::ImageFormatState::COMPRESSED);
            return true;
        }

        CMP_InitFramework();
        CMP_MipSet MipSetIn = {};

        if (texture->IsGenerated() || texture->IsPacked()) {
            const auto& d = texture->GetMipLevel(0);
            if (CMP_CreateMipSet(&MipSetIn, d.width, d.height, 1, CF_8bit, TT_2D) != CMP_OK) {
                Warn("[CompressTexture] CMP_CreateMipSet failed - name: " + texture->GetDebugName());
                return false;
            }
            CMP_MipLevel* mipLevel   = MipSetIn.m_pMipLevelTable[0];
            mipLevel->m_dwLinearSize = d.dataSize;
            mipLevel->m_nWidth       = d.width;
            mipLevel->m_nHeight      = d.height;
            mipLevel->m_pbData       = static_cast<CMP_BYTE*>(malloc(d.dataSize));
            memcpy(mipLevel->m_pbData, d.data, d.dataSize);
        } else {
            if (CMP_LoadTexture(texture->GetFileInfo().path.c_str(), &MipSetIn) != CMP_OK) {
                Warn("[CompressTexture] CMP_LoadTexture failed - path: " + texture->GetFileInfo().path);
                return false;
            }
        }

        return CompressTexture_Internal(texture, MipSetIn, fQuality);
    }

    bool ReadDDS(const String& path, Vector<graphics::TextureData>& outLevels) {
        if (!fs::File::Exists(path)) {
            Warn("[ReadDDS] File not found: " + path);
            return false;
        }

        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            Warn("[ReadDDS] Cannot open: " + path);
            return false;
        }

        u32 magic = 0;
        file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        if (magic != 0x20534444) {
            Warn("[ReadDDS] Not a DDS file: " + path);
            return false;
        }

        DDSHeader header = {};
        file.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (header.dwSize != 124) {
            Warn("[ReadDDS] Invalid DDS header: " + path);
            return false;
        }

        DDSHeaderDX10 dx10Header = {};
        if (header.ddspf_dwFourCC == CMP_MAKEFOURCC('D', 'X', '1', '0'))
            file.read(reinterpret_cast<char*>(&dx10Header), sizeof(dx10Header));

        auto [internalFormat, format, blockSize, channelCount] = GetDDSFormatInfo(header, &dx10Header);

        u32 mipW = header.dwWidth;
        u32 mipH = header.dwHeight;
        for (u32 level = 0; level < header.dwMipMapCount; level++) {
            const u32 blocksWide = (mipW + 3) / 4;
            const u32 blocksHigh = (mipH + 3) / 4;
            const u32 dataSize   = blocksWide * blocksHigh * blockSize;

            if (format == 0 || internalFormat == 0) {
                Warn("[ReadDDS] Unknown format at level " + std::to_string(level) + ": " + path);
                mipW = std::max(4u, mipW >> 1);
                mipH = std::max(4u, mipH >> 1);
                continue;
            }

            graphics::TextureData data;
            data.width          = static_cast<int>(mipW);
            data.height         = static_cast<int>(mipH);
            data.dataSize       = static_cast<int>(dataSize);
            data.format         = format;
            data.internalFormat = internalFormat;
            data.channelCount   = channelCount;
            data.data           = new u8[dataSize];
            file.read(static_cast<char*>(data.data), dataSize);
            outLevels.push_back(data);

            mipW = std::max(4u, mipW >> 1);
            mipH = std::max(4u, mipH >> 1);
        }

        return !outLevels.empty();
    }

    bool SaveTextureAsFile(platform::opengl::OpenGLTexture* texture, int jpgQuality) {
        if (!texture) {
            Warn("[SaveTextureAsFile] Texture is nullptr!");
            return false;
        }

        const auto& d      = texture->GetMipLevel(0);
        const auto& info   = texture->GetFileInfo();
        const int   stride = d.channelCount * d.width;

        if (stride == 0 || d.channelCount == 0 || d.width == 0 || d.height == 0) {
            Warn("[SaveTextureAsFile] Invalid dimensions - name: " + texture->GetDebugName());
            return false;
        }

        if (info.ext == ".png")
            stbi_write_png(info.path.c_str(), d.width, d.height, d.channelCount, d.data, stride);
        else if (info.ext == ".jpg")
            stbi_write_jpg(info.path.c_str(), d.width, d.height, d.channelCount, d.data, jpgQuality);
        else if (info.ext == ".tga")
            stbi_write_tga(info.path.c_str(), d.width, d.height, d.channelCount, d.data);
        else {
            Warn("[SaveTextureAsFile] Unsupported extension '" + info.ext + "' - name: " + texture->GetDebugName());
            return false;
        }
        return true;
    }

    Ref<platform::opengl::OpenGLTexture> PackORM(
        const Ref<platform::opengl::OpenGLTexture>& ao,
        const Ref<platform::opengl::OpenGLTexture>& rgh,
        const Ref<platform::opengl::OpenGLTexture>& mtl,
        const String& materialName)
    {
        if (!ao || !rgh || !mtl) {
            Warn("[PackORM] Null texture slot!");
            return Services::GetTextureManager().GetOrCreateDefault(graphics::TextureType::ORM);
        }

        if (ao->IsDefault() && rgh->IsDefault() && mtl->IsDefault())
            return Services::GetTextureManager().GetOrCreateDefault(graphics::TextureType::ORM);

        const auto* base = !ao->IsDefault()  ? ao.get()
                         : !rgh->IsDefault() ? rgh.get()
                                             : mtl.get();

        const int width        = static_cast<int>(base->GetWidth());
        const int height       = static_cast<int>(base->GetHeight());
        constexpr int channels = 4;
        const int dataSize     = width * height * channels;

        auto* rawOut = new u8[dataSize];

        const auto* aoData  = ao->IsDefault()  ? nullptr : static_cast<u8*>(ao->GetMipLevel(0).data);
        const auto* rghData = rgh->IsDefault() ? nullptr : static_cast<u8*>(rgh->GetMipLevel(0).data);
        const auto* mtlData = mtl->IsDefault() ? nullptr : static_cast<u8*>(mtl->GetMipLevel(0).data);

        for (int i = 0; i < width * height; i++) {
            rawOut[i * channels + 0] = aoData  ? aoData[i]  : 255;
            rawOut[i * channels + 1] = rghData ? rghData[i] : 255;
            rawOut[i * channels + 2] = mtlData ? mtlData[i] : 0;
            rawOut[i * channels + 3] = 255;
        }

        graphics::TextureData packedData;
        packedData.data         = rawOut;
        packedData.width        = width;
        packedData.height       = height;
        packedData.channelCount = channels;
        packedData.dataSize     = dataSize;

        auto packed = CreateRef<platform::opengl::OpenGLTexture>();
        packed->SetOrigin(platform::opengl::OpenGLTexture::TextureOrigin::Packed);
        packed->SetImageFormatState(graphics::ImageFormatState::UNCOMPRESSED);
        packed->SetDebugName(materialName + "_ORM");
        packed->CreateFromData(packedData, graphics::TextureType::ORM);

        const String rawPath = String(ASSETS_DIR) + "textures/uncompressed/"
                             + materialName + "_ORM.png";
        packed->SetFileInfo(fs::FileInfoFromPath(rawPath));
        if (!SaveTextureAsFile(packed.get()))
            Warn("[PackORM] Failed to save raw ORM: " + rawPath);

        if (!CompressTexture(packed.get()))
            Warn("[PackORM] Compression failed - texture will upload as uncompressed");

        auto& tm = Services::GetTextureManager();
        for (auto* slot : {ao.get(), rgh.get(), mtl.get()}) {
            if (!slot->IsDefault()) {
                fs::File::Delete(slot->GetFileInfo().path);
                tm.DeleteCPU(slot->GetUUID());
            }
        }

        return packed;
    }

    Ref<platform::opengl::OpenGLTexture> PackORM(
        const std::array<Ref<platform::opengl::OpenGLTexture>, 3>& orm,
        const String& materialName)
    {
        return PackORM(orm[0], orm[1], orm[2], materialName);
    }
}