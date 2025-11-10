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

    struct TextureData {
        void* m_Data;
        int m_Handle;
        int m_Width;
        int m_Height;
        int m_DataSize;
        int m_ChannelCount;
        GLsizei m_ImageSize;
        GLenum m_Format;
        GLenum m_InternalFormat;
        ImageCompressedType m_ImageCompressType;
    };

    struct Texture {
    public:
        explicit Texture(ImageFormatState format = ImageFormatState::UNCOMPRESSED, bool isDefaultTex = false);
        explicit Texture(const Ref<TextureData>& data, TextureType type = TextureType::UNDEFINED, bool isDefaultTex = false);
        Texture(const Texture&) = default;

        void SetData(TextureData data);
        void SetFileInfo(FileInfo info);
        void SetTexIndex(int idx);
        void SetTexArrayIndex(int idx);
        void SetType(TextureType type);
        void SetImageFormat(ImageFormatState format);

        TextureData& GetData() { return m_Data; }
        FileInfo& GetFileInfo() { return m_FileInfo; }
        [[nodiscard]] const std::string& GetName() const { return m_FileInfo.stem; }
        [[nodiscard]] const std::string& GetFullName() const { return m_FileInfo.name; }
        [[nodiscard]] int GetIndex() const { return m_TexIndex; }
        [[nodiscard]] int GetArrayIndex() const { return m_TexIndex; }
        [[nodiscard]] TextureType GetType() const { return m_Type; }
        [[nodiscard]] std::string GetTypeAsString(TextureType type) const;
        [[nodiscard]] ImageFormatState GetImageFormat() const { return m_ImageFormatState; }
        [[nodiscard]] bool IsDefault() const { return m_IsDefault; }

    private:
        TextureData m_Data{};
        TextureType m_Type{};
        FileInfo m_FileInfo{};
        ImageFormatState m_ImageFormatState = ImageFormatState::UNCOMPRESSED;
        bool m_IsDefault = false;

        int m_TexIndex = -1;
        int m_ArrayIndex = -1;
    };
}
