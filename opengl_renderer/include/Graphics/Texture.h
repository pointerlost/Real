//
// Created by pointerlost on 10/6/25.
//
#pragma once
#include <cstring>
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include "Common/RealEnum.h"
#include "Core/file_manager.h"
#include "Core/Utils.h"
#include "Core/UUID.h"
#include "glad/glad.h"

namespace Real {

    struct TextureData {
        void* m_Data = nullptr;
        int m_ChannelCount = 0;
        int m_Width = 0;
        int m_Height = 0;
        uint m_DataSize = 0;
        int m_Format = {};
        int m_InternalFormat = {};

        TextureData() = default;
        ~TextureData() { delete[] static_cast<uint8_t*>(m_Data); }

        // Copy constructor
        TextureData(const TextureData& other) {
            m_ChannelCount = other.m_ChannelCount;
            m_Width    = other.m_Width;
            m_Height   = other.m_Height;
            m_DataSize = other.m_DataSize;
            m_Format   = other.m_Format;
            m_InternalFormat = other.m_InternalFormat;

            if (other.m_Data && other.m_DataSize > 0) {
                m_Data = new uint8_t[m_DataSize];
                memcpy(m_Data, other.m_Data, m_DataSize);
            }
        }

        // Copy assignment
        TextureData& operator=(const TextureData& other) {
            if (this == &other) return *this;

            // free existing
            delete[] static_cast<uint8_t*>(m_Data);

            m_ChannelCount = other.m_ChannelCount;
            m_Width    = other.m_Width;
            m_Height   = other.m_Height;
            m_DataSize = other.m_DataSize;
            m_Format   = other.m_Format;
            m_InternalFormat = other.m_InternalFormat;

            if (other.m_Data && other.m_DataSize > 0) {
                m_Data = new uint8_t[m_DataSize];
                memcpy(m_Data, other.m_Data, m_DataSize);
            } else {
                m_Data = nullptr;
            }

            return *this;
        }
    };

    struct OpenGLTexture {
        explicit OpenGLTexture(const TextureData &data, FileInfo info = FileInfo(), UUID uuid = UUID());
        explicit OpenGLTexture(FileInfo fileinfo, ImageFormatState imagestate = ImageFormatState::UNCOMPRESSED);
        explicit OpenGLTexture(TextureType type = TextureType::UNDEFINED);
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
        [[nodiscard]] int GetIndex() const { return m_Index; }
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

        void PrepareOptionsAndUploadToGPU();
        void Resize(const glm::ivec2& resolution, int mipLevel, bool srgbSpace = false);
        void CreateBindless();
        void MakeResident() const;
        void MakeNonResident() const;

    private:
        GLuint m_Handle = 0;
        GLuint64 m_BindlessHandleID = 0;
        UUID m_UUID;

        int m_Index = 0;
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
