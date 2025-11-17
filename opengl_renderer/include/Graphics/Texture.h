//
// Created by pointerlost on 10/6/25.
//
#pragma once
#include <string>
#include <vector>
#include <glm/vec2.hpp>

#include "Core/file_manager.h"
#include "Core/Utils.h"
#include "glad/glad.h"

namespace Real {

    enum class TextureType {
        ALB,
        NRM,
        RGH,
        MTL,
        AO,
        RMA,
        HEIGHT,
        UNDEFINED,
    };

    enum class ImageFormatState {
        UNCOMPRESSED,
        COMPRESSED,
        UNDEFINED,
    };

    enum class ImageCompressedType {
        BC1,
        BC2,
        BC3,
        BC4,
        BC5,
        BC6,
        BC7,
        UNDEFINED,
    };

    enum class TextureFilterMode {
        NEAREST,
        LINEAR,
    };

    enum class TextureWrapMode {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER
    };

    struct TextureData {
        void* m_Data = nullptr;
        int m_ChannelCount = 0;
        int m_Width = 0;
        int m_Height = 0;
        int m_DataSize = 0;
        GLenum m_Format = {};
        GLenum m_InternalFormat = {};
    };

    struct OpenGLTexture {
    public:
        explicit OpenGLTexture(ImageFormatState format = ImageFormatState::UNCOMPRESSED, bool isDefaultTex = false);
        explicit OpenGLTexture(TextureType type = TextureType::UNDEFINED, bool isDefaultTex = false);
        OpenGLTexture(const OpenGLTexture&) = default;

        void AddLevelData(TextureData data);
        void SetLevelData(void* data, int mipLevel);
        void SetFileInfo(FileInfo info);
        void SetType(TextureType type);
        void SetIndex(int idx);
        void SetFormat(GLenum format, int mipLevel);
        void SetInternalFormat(GLenum format, int mipLevel);
        void SetImageFormat(ImageFormatState format);
        void SetChannelCount(int count, int mipLevel);
        void SetResolution(const glm::ivec2& res, int mipLevel);
        void SetDataSize(int size, int mipLevel);
        void SetTextureParameters(GLenum target = GL_TEXTURE_2D);
        void SetWrapMode(TextureWrapMode mode);
        void SetFilterMode(TextureFilterMode mode);
        void SetCompressionType(ImageCompressedType type);

        FileInfo& GetFileInfo() { return m_FileInfo; }
        [[nodiscard]] const std::string& GetName() const { return m_FileInfo.name; }
        [[nodiscard]] const std::string& GetStem() const { return m_FileInfo.stem; }
        [[nodiscard]] TextureType GetType() const { return m_Type; }
        [[nodiscard]] bool IsDefault() const { return m_IsDefault; }
        [[nodiscard]] bool IsCompressed() const { return m_ImageFormatState == ImageFormatState::COMPRESSED; }
        [[nodiscard]] bool HasData(int mipLevel) const { return m_MipLevelDatas[mipLevel].m_Data != nullptr; }
        [[nodiscard]] bool HasBindlessHandle() const { return m_BindlessHandleID != 0; }
        [[nodiscard]] std::pair<int, int> GetResolution(int mipLevel) const;
        [[nodiscard]] int GetIndex() const { return m_Index; }
        [[nodiscard]] GLuint GetBindlessHandle() const { return m_BindlessHandleID; }
        [[nodiscard]] GLuint& GetBindlessHandle() { return m_BindlessHandleID; }
        [[nodiscard]] TextureData& GetLevelData(int mipLevel);
        [[nodiscard]] ImageFormatState GetImageFormat() const { return m_ImageFormatState; }
        [[nodiscard]] GLenum GetInternalFormat(int mipLevel) const { return m_MipLevelDatas[mipLevel].m_InternalFormat; }
        [[nodiscard]] ImageCompressedType GetCompressionType() const { return m_ImageCompressType; }
        [[nodiscard]] int GetMipMapCount() const { return m_MipLevelCount; }
        [[nodiscard]] int GetChannelCount(int mipLevel) const { return m_MipLevelDatas[mipLevel].m_ChannelCount; }
        int& GetChannelCount(int mipLevel) { return m_MipLevelDatas[mipLevel].m_ChannelCount; }

        TextureData LoadFromFile(const std::string& path);
        void Create();
        void Resize(const glm::ivec2& resolution, int mipLevel, bool srgbSpace = false);
        void MakeResident(GLuint id) const;
        void MakeNonResident(GLuint id) const;

    private:
        GLuint m_Handle = 0;
        GLuint m_BindlessHandleID = 0;

        // TODO: Remove isDefault variable when you have an idea about this
        bool m_IsDefault = false;
        int m_Index = 0;
        int m_BlockSize = 0;
        int m_MipLevelCount = 0;
        std::vector<TextureData> m_MipLevelDatas;

        ImageCompressedType m_ImageCompressType = ImageCompressedType::UNDEFINED;
        ImageFormatState m_ImageFormatState = ImageFormatState::UNDEFINED;
        TextureType m_Type = TextureType::UNDEFINED;
        TextureWrapMode m_WrapMode = TextureWrapMode::REPEAT;
        TextureFilterMode m_FilterMode = TextureFilterMode::NEAREST;
        FileInfo m_FileInfo{};

    private:
        void CreateBindlessAndMakeResident();
        void UploadMipLevels(ImageFormatState state);
        void CreateMipMaps(int width, int height);
        int CalculateMipMapLevels(int width, int height);
        int CalculateMipMapLevels(const glm::ivec2& res);
    };
}
