//
// Created by pointerlost on 3/28/26.
//
#pragma once
#include "Assets/FileManager.h"
#include "Graphics/RenderTypes.h"
#include "RHI/ITexture.h"

namespace Real::platform::opengl {
    using namespace graphics;
    using IFS = ImageFormatState;

    class OpenGLTexture final : public rhi::ITexture {
    public:
        enum class TextureOrigin { File, Packed, Generated };

    public:
        OpenGLTexture()                     = default;
        OpenGLTexture(const OpenGLTexture&) = delete;

        // rhi::ITexture interface
        [[nodiscard]] UUID        GetUUID()   const override { return m_ID;   }
        [[nodiscard]] u32         GetWidth()  const override { return m_MipLevels.empty() ? 0 : m_MipLevels[0].width;  }
        [[nodiscard]] u32         GetHeight() const override { return m_MipLevels.empty() ? 0 : m_MipLevels[0].height; }
        [[nodiscard]] TextureType GetType()   const override { return m_Type; }

        void LoadFromFile(const String& path)                           override;
        void CreateFromData(const TextureData& data, TextureType type)  override;

        // Upload
        void UploadToGPU();
        void MakeResident()    const;
        void MakeNonResident() const;

        // Setters
        void SetID(const UUID& id)                       { m_ID = id;             }
        void SetType(TextureType type)                   { m_Type        = type;  }
        void SetWrapMode(TextureWrapMode mode)           { m_WrapMode    = mode;  }
        void SetFilterMode(TextureFilterMode mode)       { m_FilterMode  = mode;  }
        void SetImageFormatState(IFS state) { m_ImageFormatState  = state; }
        void SetDebugName(const String& name)            { m_DebugName   = name;  }
        void SetOrigin(TextureOrigin origin)             { m_Origin      = origin;}
        void SetGPUIndex(GPUIndex idx)                   { m_GPUIndex    = idx;   }
        void SetFileInfo(const fs::FileInfo& info)       { m_FileInfo = info;     }
        void SetMipLevels(const Vector<TextureData>& levels);  // for pre-baked DDS

        // Getters
        [[nodiscard]] IFS             GetImageFormatState() const { return m_ImageFormatState;            }
        [[nodiscard]] bool            IsDefault()    const { return m_ImageFormatState == IFS::DEFAULT;   }
        [[nodiscard]] bool            IsCompressed() const { return m_ImageFormatState == IFS::COMPRESSED;}
        [[nodiscard]] bool            IsUploaded()   const { return m_IsUploadedToGPU;    }
        [[nodiscard]] bool            IsFromFile()   const { return m_Origin == TextureOrigin::File;      }
        [[nodiscard]] bool            IsPacked()     const { return m_Origin == TextureOrigin::Packed;    }
        [[nodiscard]] bool            IsGenerated()  const { return m_Origin == TextureOrigin::Generated; }
        [[nodiscard]] GPUIndex        GetGPUIndex()  const { return m_GPUIndex;           }
        [[nodiscard]] fs::FileInfo    GetFileInfo()  const { return m_FileInfo;           }
        [[nodiscard]] BindlessHandle  GetBindless()  const { return m_Bindless;           }
        [[nodiscard]] TextureHandle   GetHandle()    const { return m_Handle;             }
        [[nodiscard]] int             GetMipCount()  const { return static_cast<int>(m_MipLevels.size()); }
        [[nodiscard]] TextureData&    GetMipLevel(int level);

        [[nodiscard]] const String& GetDebugName() const {
            if (m_Origin == TextureOrigin::File) return m_FileInfo.name;
            return m_DebugName;
        }

        [[nodiscard]] bool IsReadyForUpload() const {
            return !m_IsUploadedToGPU
                && !m_MipLevels.empty()
                && m_MipLevels[0].data != nullptr
                && m_ImageFormatState != IFS::UNDEFINED;
        }

    private:
        void CreateHandle();
        void UploadMipLevels() const;
        void CleanUpCPUData();
        void CreateBindless();
        void SetTextureParameters();
        void CreateMipmapsFromDDS(const Vector<TextureData>& levels);

        [[nodiscard]] int CalculateMaxMipLevels(int width, int height) const;

    private:
        Vector<TextureData> m_MipLevels {};

        UUID           m_ID      {};
        TextureHandle  m_Handle  {};
        BindlessHandle m_Bindless{};
        GPUIndex       m_GPUIndex = InvalidGPUIndex;

        TextureType       m_Type      {};
        IFS               m_ImageFormatState{};
        TextureWrapMode   m_WrapMode  {};
        TextureFilterMode m_FilterMode{};
        TextureOrigin     m_Origin    = TextureOrigin::Generated;

        fs::FileInfo m_FileInfo  {};
        String       m_DebugName {};

        bool m_IsSTBAllocated  = false;
        bool m_IsUploadedToGPU = false;
    };
}
