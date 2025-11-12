//
// Created by pointerlost on 10/6/25.
//
#pragma once
#include <string>
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

    struct OpenGLTextureData {
        void* m_Data;
        int m_Width;
        int m_Height;
        int m_DataSize;
        int m_ChannelCount;
        GLuint m_Handle;
        GLsizei m_ImageSize;
        GLenum m_Format;
        GLenum m_InternalFormat;
        GLenum m_GLCompressedType;
        ImageCompressedType m_ImageCompressType;
    };

    struct Texture {
    public:
        explicit Texture(ImageFormatState format = ImageFormatState::UNCOMPRESSED, bool isDefaultTex = false);
        explicit Texture(const Ref<OpenGLTextureData>& data, TextureType type = TextureType::UNDEFINED, bool isDefaultTex = false);
        Texture(const Texture&) = default;

        void SetData(OpenGLTextureData data);
        void SetFileInfo(FileInfo info);
        void SetType(TextureType type);
        void SetIndex(int idx);
        void SetImageFormat(ImageFormatState format);

        OpenGLTextureData& GetData() { return m_Data; }
        FileInfo& GetFileInfo() { return m_FileInfo; }
        [[nodiscard]] const std::string& GetName() const { return m_FileInfo.name; }
        [[nodiscard]] const std::string& GetStem() const { return m_FileInfo.stem; }
        [[nodiscard]] const std::string& GetFullName() const { return m_FileInfo.name; }
        [[nodiscard]] TextureType GetType() const { return m_Type; }
        [[nodiscard]] std::string GetTypeAsString(TextureType type) const;
        [[nodiscard]] ImageFormatState GetImageFormat() const { return m_ImageFormatState; }
        [[nodiscard]] bool IsDefault() const { return m_IsDefault; }
        [[nodiscard]] bool IsCompressed() const { return m_ImageFormatState == ImageFormatState::COMPRESSED; }
        [[nodiscard]] bool HasData() const { return m_Data.m_Data != nullptr; }
        [[nodiscard]] bool HasBindlessHandle() const { return m_glBindlessHandle != 0; }
        [[nodiscard]] std::pair<int, int> GetResolution() const { return std::make_pair(m_Data.m_Width, m_Data.m_Height); }
        [[nodiscard]] int GetIndex() const { return m_Index; }
        [[nodiscard]] GLuint GetBindlessHandle() const { return m_glBindlessHandle; }
        [[nodiscard]] GLuint& GetBindlessHandle() { return m_glBindlessHandle; }

        void Create();

    private:
        OpenGLTextureData m_Data{};
        GLuint m_glBindlessHandle = 0;
        TextureType m_Type{};
        FileInfo m_FileInfo{};
        ImageFormatState m_ImageFormatState = ImageFormatState::UNCOMPRESSED;
        bool m_IsDefault = false;

        int m_Index = 0;

    private:
        void CreateBindlessAndMakeResident();
    };
}
