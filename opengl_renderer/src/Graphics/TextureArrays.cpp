//
// Created by pointerlost on 11/1/25.
//
#include "Graphics/TextureArrays.h"

#include "Graphics/Material.h"
#include "Graphics/Texture.h"

namespace Real {

    void TextureArrayManager::AddAlbedoMap(TextureResolution resolution, const Ref<Texture>& tex) {
        if (IsTextureExists(tex)) return;

        switch (resolution) {
            case TextureResolution::_256:
                tex->SetTexIndex(static_cast<int>(m_AlbedoMapTexArray_256.size()));
                m_AlbedoMapTexArray_256.push_back(tex);
                break;
            case TextureResolution::_512:
                tex->SetTexIndex(static_cast<int>(m_AlbedoMapTexArray_512.size()));
                m_AlbedoMapTexArray_512.push_back(tex);
                break;
            case TextureResolution::_1024:
                tex->SetTexIndex(static_cast<int>(m_AlbedoMapTexArray_1024.size()));
                m_AlbedoMapTexArray_1024.push_back(tex);
                break;
            case TextureResolution::_2048:
                tex->SetTexIndex(static_cast<int>(m_AlbedoMapTexArray_2048.size()));
                m_AlbedoMapTexArray_2048.push_back(tex);
                break;
            case TextureResolution::_4096:
                tex->SetTexIndex(static_cast<int>(m_AlbedoMapTexArray_4096.size()));
                m_AlbedoMapTexArray_4096.push_back(tex);
                break;
        }
        m_CheckTextureExists.insert(tex->GetData().m_Handle);
    }

    void TextureArrayManager::AddNormalMap(TextureResolution resolution, const Ref<Texture> &tex) {
        if (IsTextureExists(tex)) return;

        switch (resolution) {
            case TextureResolution::_256:
                tex->SetTexIndex(static_cast<int>(m_NormalMapTexArray_256.size()));
                m_NormalMapTexArray_256.push_back(tex);
                break;
            case TextureResolution::_512:
                tex->SetTexIndex(static_cast<int>(m_NormalMapTexArray_512.size()));
                m_NormalMapTexArray_512.push_back(tex);
                break;
            case TextureResolution::_1024:
                tex->SetTexIndex(static_cast<int>(m_NormalMapTexArray_1024.size()));
                m_NormalMapTexArray_1024.push_back(tex);
                break;
            case TextureResolution::_2048:
                tex->SetTexIndex(static_cast<int>(m_NormalMapTexArray_2048.size()));
                m_NormalMapTexArray_2048.push_back(tex);
                break;
            case TextureResolution::_4096:
                tex->SetTexIndex(static_cast<int>(m_NormalMapTexArray_4096.size()));
                m_NormalMapTexArray_4096.push_back(tex);
                break;
        }
        m_CheckTextureExists.insert(tex->GetData().m_Handle);
    }

    void TextureArrayManager::AddRMATexturesMap(TextureResolution resolution, const Ref<Texture> &tex) {
        if (IsTextureExists(tex)) return;

        switch (resolution) {
            case TextureResolution::_256:
                tex->SetTexIndex(static_cast<int>(m_rmaMapTexArray_256.size()));
                m_NormalMapTexArray_256.push_back(tex);
                break;
            case TextureResolution::_512:
                tex->SetTexIndex(static_cast<int>(m_NormalMapTexArray_512.size()));
                m_NormalMapTexArray_512.push_back(tex);
                break;
            case TextureResolution::_1024:
                tex->SetTexIndex(static_cast<int>(m_NormalMapTexArray_1024.size()));
                m_NormalMapTexArray_1024.push_back(tex);
                break;
            case TextureResolution::_2048:
                tex->SetTexIndex(static_cast<int>(m_NormalMapTexArray_2048.size()));
                m_NormalMapTexArray_2048.push_back(tex);
                break;
            case TextureResolution::_4096:
                tex->SetTexIndex(static_cast<int>(m_NormalMapTexArray_4096.size()));
                m_NormalMapTexArray_4096.push_back(tex);
                break;
        }
        m_CheckTextureExists.insert(tex->GetData().m_Handle);
    }

    void TextureArrayManager::AddHeightMap(TextureResolution resolution, const Ref<Texture> &tex) {
        if (IsTextureExists(tex)) return;

        switch (resolution) {
            case TextureResolution::_256:
                tex->SetTexIndex(static_cast<int>(m_HeightMapTexArray_256.size()));
                m_HeightMapTexArray_256.push_back(tex);
                break;
            case TextureResolution::_512:
                tex->SetTexIndex(static_cast<int>(m_HeightMapTexArray_512.size()));
                m_HeightMapTexArray_512.push_back(tex);
                break;
            case TextureResolution::_1024:
                tex->SetTexIndex(static_cast<int>(m_HeightMapTexArray_1024.size()));
                m_HeightMapTexArray_1024.push_back(tex);
                break;
            case TextureResolution::_2048:
                tex->SetTexIndex(static_cast<int>(m_HeightMapTexArray_2048.size()));
                m_HeightMapTexArray_2048.push_back(tex);
                break;
            case TextureResolution::_4096:
                tex->SetTexIndex(static_cast<int>(m_HeightMapTexArray_4096.size()));
                m_HeightMapTexArray_4096.push_back(tex);
                break;
        }
        m_CheckTextureExists.insert(tex->GetData().m_Handle);
    }

    const std::vector<Ref<Texture>> & TextureArrayManager::GetTextureArray(TextureResolution resolution, TextureType type) {
        if (type == TextureType::ALB) {
            switch (resolution) {
                case TextureResolution::_256:
                    return m_AlbedoMapTexArray_256;
                case TextureResolution::_512:
                    return m_AlbedoMapTexArray_512;
                case TextureResolution::_1024:
                    return m_AlbedoMapTexArray_1024;
                case TextureResolution::_2048:
                    return m_AlbedoMapTexArray_2048;
                case TextureResolution::_4096:
                    return m_AlbedoMapTexArray_4096;
            }
        }
        else if (type == TextureType::NRM) {
            switch (resolution) {
                case TextureResolution::_256:
                    return m_NormalMapTexArray_256;
                case TextureResolution::_512:
                    return m_NormalMapTexArray_512;
                case TextureResolution::_1024:
                    return m_NormalMapTexArray_1024;
                case TextureResolution::_2048:
                    return m_NormalMapTexArray_2048;
                case TextureResolution::_4096:
                    return m_NormalMapTexArray_4096;
            }
        }
        else if (type == TextureType::RMA) {
            switch (resolution) {
                case TextureResolution::_256:
                    return m_rmaMapTexArray_256;
                case TextureResolution::_512:
                    return m_rmaMapTexArray_512;
                case TextureResolution::_1024:
                    return m_rmaMapTexArray_1024;
                case TextureResolution::_2048:
                    return m_rmaMapTexArray_2048;
                case TextureResolution::_4096:
                    return m_rmaMapTexArray_4096;
            }
        }
        else if (type == TextureType::HEIGHT) {
            switch (resolution) {
                case TextureResolution::_256:
                    return m_HeightMapTexArray_256;
                case TextureResolution::_512:
                    return m_HeightMapTexArray_512;
                case TextureResolution::_1024:
                    return m_HeightMapTexArray_1024;
                case TextureResolution::_2048:
                    return m_HeightMapTexArray_2048;
                case TextureResolution::_4096:
                    return m_HeightMapTexArray_4096;
            }
        }
        return {};
    }

    bool TextureArrayManager::IsTextureExists(const Ref<Texture> &texture) const {
        const auto handle = texture->GetData().m_Handle;
        return m_CheckTextureExists.contains(handle) && handle != 0;
    }
}
