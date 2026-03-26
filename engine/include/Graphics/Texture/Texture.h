//
// Created by pointerlost on 10/6/25.
//
#pragma once
#include "Assets/FileManager.h"
#include "Graphics/RenderTypes.h"
#include "Common/RealEnum.h"
#include "Core/UUID.h"

namespace Real { struct TextureAsset; }
namespace Real::math { struct iVec2; }

namespace Real::platform::opengl {

    // TODO: NEED AN INTERFACE FOR GOOD SEPARATION BETWEEN APIs (VKTexture,OpenGLTexture etc.)
    struct OpenGLTexture {
        explicit OpenGLTexture(const graphics::TextureData &data, bool isSTBAllocated, TextureType type,
            ImageFormatState image_state = ImageFormatState::UNCOMPRESSED,
            fs::FileInfo info = fs::FileInfo(), UUID uuid = UUID{}
        );
        explicit OpenGLTexture(const TextureAsset& asset);
        explicit OpenGLTexture(const Vector<graphics::TextureData>& data, fs::FileInfo info); // Compressed textures
        explicit OpenGLTexture(fs::FileInfo fileinfo, bool isSTBAllocated, ImageFormatState imagestate = ImageFormatState::UNCOMPRESSED);
        // Don't create handle for default textures!!!!
        explicit OpenGLTexture(bool isSTBAllocated = false, TextureType type = TextureType::UNDEFINED);

        OpenGLTexture(const OpenGLTexture&) = default;
        ~OpenGLTexture();

        void AddLevelData(const graphics::TextureData &data, int mipLevel);
        void SetLevelData(void* data, int mipLevel);
        void SetFileInfo(fs::FileInfo info);
        void SetType(TextureType type);
        void SetIndex(u32 idx);
        void SetFormat(int format, int mipLevel);
        void SetInternalFormat(int format, int mipLevel);
        void SetImageFormatState(ImageFormatState format);
        void SetChannelCount(int count, int mipLevel);
        void SetResolution(const math::iVec2& res, int mipLevel);
        void SetDataSize(int size, int mipLevel);
        void SetTextureParameters();
        void SetWrapMode(TextureWrapMode mode);
        void SetFilterMode(TextureFilterMode mode);
        void SetMipLevelsData(const Vector<graphics::TextureData>& mipLevels);
        void SetUUID(u64 uuid);
        void SetUUID(const UUID& uuid);

        fs::FileInfo& GetFileInfo() { return m_FileInfo; }
        [[nodiscard]] const String& GetName() const { return m_FileInfo.name; }
        [[nodiscard]] const String& GetStem() const { return m_FileInfo.stem; }
        [[nodiscard]] const String& GetPath() const { return m_FileInfo.path; }
        [[nodiscard]] const String& GetExtension() const { return m_FileInfo.ext; }
        [[nodiscard]] TextureType GetType() const { return m_Type; }
        [[nodiscard]] bool IsCompressed() const { return m_ImageFormatState == ImageFormatState::COMPRESSED; }
        [[nodiscard]] bool HasData(int mipLevel) const { return m_MipLevelsData[mipLevel].data != nullptr; }
        [[nodiscard]] bool HasBindlessHandle() const { return m_BindlessHandleID != 0; }
        [[nodiscard]] std::pair<int, int> GetResolution(int mipLevel);
        [[nodiscard]] u32 GetIndex() const { return m_GPUIndex; }
        [[nodiscard]] bool HasBindlessID() const { return m_BindlessHandleID != 0; }
        [[nodiscard]] graphics::BindlessHandle GetBindlessHandle() const { return m_BindlessHandleID; }
        [[nodiscard]] graphics::TextureData& GetLevelData(int mipLevel);
        [[nodiscard]] ImageFormatState GetImageFormatState() const { return m_ImageFormatState; }
        [[nodiscard]] int GetInternalFormat(int mipLevel) const { return m_MipLevelsData[mipLevel].internalFormat; }
        [[nodiscard]] int GetFormat(int mipLevel) const { return m_MipLevelsData[mipLevel].format; }
        [[nodiscard]] int GetMipMapCount() const { return m_MipLevelCount; }
        [[nodiscard]] int GetChannelCount(int mipLevel) const { return m_MipLevelsData[mipLevel].channelCount; }
        int& GetChannelCount(int mipLevel) { return m_MipLevelsData[mipLevel].channelCount; }
        [[nodiscard]] UUID GetUUID() const { return m_UUID; }
        [[nodiscard]] graphics::TextureHandle GetHandle() const { return m_Handle; }

        [[maybe_unused]] graphics::TextureData LoadFromFile(const String& path);
        void Create();
        void CreateFromData(const graphics::TextureData &data, TextureType type);
        void CleanUpCPUData();

        void PrepareOptionsAndUploadToGPU();
        void Resize(const math::iVec2& resolution, int mipLevel, bool srgbSpace = false);
        void CreateBindless();
        void MakeResident() const;
        void MakeNonResident() const;
        [[nodiscard]] bool IsCPUGenerated() const;
        [[nodiscard]] bool IsHandleExist() const;

    private:
        graphics::TextureHandle m_Handle{};
        graphics::BindlessHandle m_BindlessHandleID = graphics::InvalidBindlessHandle;
        UUID m_UUID{};

        bool m_IsSTBAllocated  = false;
        bool m_IsUploadedToGPU = false;
        int m_BlockSize = 0;
        int m_MipLevelCount = 0;
        u32 m_GPUIndex = 0;
        Vector<graphics::TextureData> m_MipLevelsData;

        ImageFormatState m_ImageFormatState = ImageFormatState::UNDEFINED;
        TextureType m_Type = TextureType::UNDEFINED;
        TextureWrapMode m_WrapMode = TextureWrapMode::REPEAT;
        TextureFilterMode m_FilterMode = TextureFilterMode::NEAREST;
        fs::FileInfo m_FileInfo{};

    private:
        void CreateHandle();
        void UploadMipLevels();
        void CreateMipmapsFromDDS(const Vector<graphics::TextureData> &levelsData);
        int CalculateMaxMipMapLevels(int width, int height);
        int CalculateMaxMipMapLevels(const math::iVec2& res);
    };
}
