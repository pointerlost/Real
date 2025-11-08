//
// Created by pointerlost on 10/12/25.
//
#include "Graphics/Texture.h"
#include <utility>

namespace Real {

    Texture::Texture(ImageFormatState format, bool isDefaultTex)
        : m_Type(TextureType::UNDEFINED), m_ImageFormatState(format), m_IsDefault(isDefaultTex)
    {
    }

    Texture::Texture(const Ref<TextureData> &data, TextureType type, bool isDefaultTex)
        : m_Data(data.get()), m_Type(type), m_IsDefault(isDefaultTex)
    {
    }

    void Texture::SetData(TextureData data) {
        m_Data = data;
    }

    void Texture::SetFileInfo(FileInfo info) {
        m_FileInfo = std::move(info);
    }

    void Texture::SetTexIndex(int idx) {
        m_TexIndex = idx;
    }

    void Texture::SetTexArrayIndex(int idx) {
        m_ArrayIndex = idx;
    }

    void Texture::SetType(TextureType type) {
        m_Type = type;
    }

    void Texture::SetImageFormat(ImageFormatState format) {
        m_ImageFormatState = format;
    }

    std::string Texture::GetTypeAsString(TextureType type) const {
        switch (type) {
            case TextureType::ALB:
                return "ALB";
            case TextureType::NRM:
                return "NRM";
            case TextureType::RGH:
                return "RGH";
            case TextureType::MTL:
                return "MTL";
            case TextureType::AO:
                return "AO";
            case TextureType::RMA:
                return "RMA";
            case TextureType::HEIGHT:
                return "HEIGHT";

            default:
                return "UNDEFINED";
        }
    }

}
