//
// Created by pointerlost on 10/6/25.
//
#pragma once
#include <vector>
#include "Common/RealEnum.h"
#include "Common/RealTypes.h"
#include "Core/Utils.h"
#include "Core/UUID.h"
#include "glad/glad.h"

namespace Real {

    struct OpenGLTexture {
        explicit OpenGLTexture(const TextureData &data, bool isSTBAllocated, TextureType type,
            ImageFormatState image_state = ImageFormatState::UNCOMPRESSED,
            FileInfo info = FileInfo(), UUID uuid = UUID()
        );
        explicit OpenGLTexture(const std::vector<TextureData>& data); // Compressed textures
        explicit OpenGLTexture(FileInfo fileinfo, bool isSTBAllocated, ImageFormatState imagestate = ImageFormatState::UNCOMPRESSED);
        explicit OpenGLTexture(bool isSTBAllocated = false, TextureType type = TextureType::UNDEFINED);
        OpenGLTexture(const OpenGLTexture&) = default;
        ~OpenGLTexture();

        void AddLevelData(const TextureData &data, int mipLevel);
        void SetLevelData(void* data, int mipLevel);
        void SetFileInfo(FileInfo info);
        void SetType(TextureType type);
        void SetIndex(int idx);
        void SetFormat(int format, int mipLevel);
        void SetInternalFormat(int format, int mipLevel);
        void SetImageFormatState(ImageFormatState format);
        void SetChannelCount(int count, int mipLevel);
        void SetResolution(const glm::ivec2& res, int mipLevel);
        void SetDataSize(int size, int mipLevel);
        void SetTextureParameters();
        void SetWrapMode(TextureWrapMode mode);
        void SetFilterMode(TextureFilterMode mode);
        void SetMipLevelsData(const std::vector<TextureData>& mipLevels);
        void SetUUID(uint64_t uuid);
        void SetUUID(const UUID& uuid);

        FileInfo& GetFileInfo() { return m_FileInfo; }
        [[nodiscard]] const std::string& GetName() const { return m_FileInfo.name; }
        [[nodiscard]] const std::string& GetStem() const { return m_FileInfo.stem; }
        [[nodiscard]] const std::string& GetPath() const { return m_FileInfo.path; }
        [[nodiscard]] const std::string& GetExtension() const { return m_FileInfo.ext; }
        [[nodiscard]] TextureType GetType() const { return m_Type; }
        [[nodiscard]] bool IsCompressed() const { return m_ImageFormatState == ImageFormatState::COMPRESSED; }
        [[nodiscard]] bool HasData(int mipLevel) const { return m_MipLevelsData[mipLevel].m_Data != nullptr; }
        [[nodiscard]] bool HasBindlessHandle() const { return m_BindlessHandleID != 0; }
        [[nodiscard]] std::pair<int, int> GetResolution(int mipLevel);
        [[nodiscard]] int GetIndex() const { return m_GPUIndex; }
        [[nodiscard]] bool HasBindlessID() const { return m_BindlessHandleID != 0; }
        [[nodiscard]] GLuint64 GetBindlessHandle() const { return m_BindlessHandleID; }
        [[nodiscard]] TextureData& GetLevelData(int mipLevel);
        [[nodiscard]] ImageFormatState GetImageFormatState() const { return m_ImageFormatState; }
        [[nodiscard]] int GetInternalFormat(int mipLevel) const { return m_MipLevelsData[mipLevel].m_InternalFormat; }
        [[nodiscard]] int GetFormat(int mipLevel) const { return m_MipLevelsData[mipLevel].m_Format; }
        [[nodiscard]] int GetMipMapCount() const { return m_MipLevelCount; }
        [[nodiscard]] int GetChannelCount(int mipLevel) const { return m_MipLevelsData[mipLevel].m_ChannelCount; }
        int& GetChannelCount(int mipLevel) { return m_MipLevelsData[mipLevel].m_ChannelCount; }
        [[nodiscard]] UUID GetUUID() const { return m_UUID; }

        [[maybe_unused]] TextureData LoadFromFile(const std::string& path);
        void Create();
        void CreateFromData(const TextureData &data, TextureType type);
        void CleanUpCPUData();

        void PrepareOptionsAndUploadToGPU();
        void Resize(const glm::ivec2& resolution, int mipLevel, bool srgbSpace = false);
        void CreateBindless();
        void MakeResident() const;
        void MakeNonResident() const;
        [[nodiscard]] bool IsCPUGenerated() const;

    private:
        GLuint m_Handle = 0;
        GLuint64 m_BindlessHandleID = 0;
        UUID m_UUID{};

        bool m_IsSTBAllocated = false;
        int m_GPUIndex = 0;
        int m_BlockSize = 0;
        int m_MipLevelCount = 0;
        std::vector<TextureData> m_MipLevelsData;

        ImageFormatState m_ImageFormatState = ImageFormatState::UNDEFINED;
        TextureType m_Type = TextureType::UNDEFINED;
        TextureWrapMode m_WrapMode = TextureWrapMode::REPEAT;
        TextureFilterMode m_FilterMode = TextureFilterMode::NEAREST;
        FileInfo m_FileInfo{};

    private:
        void CreateHandle();
        void UploadMipLevels();
        void CreateMipmapsFromDDS(const std::vector<TextureData> &levelsData);
        int CalculateMaxMipMapLevels(int width, int height);
        int CalculateMaxMipMapLevels(const glm::ivec2& res);
    };
}
