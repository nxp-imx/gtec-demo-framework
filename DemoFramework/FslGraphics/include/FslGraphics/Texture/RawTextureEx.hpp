#ifndef FSLGRAPHICS_TEXTURE_RAWTEXTUREEX_HPP
#define FSLGRAPHICS_TEXTURE_RAWTEXTUREEX_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *
 *    * Neither the name of the NXP. nor the names of
 *      its contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************************************************************************************/

#include <FslBase/Span/Span.hpp>
#include <FslGraphics/Texture/ReadOnlyRawTexture.hpp>

namespace Fsl
{
  class RawTextureEx final
  {
    TextureType m_textureType{TextureType::Undefined};
    void* m_pContent{nullptr};
    std::size_t m_contentByteSize{0};
    //! The raw blobs, we expect this to be a multidimensional array
    ReadOnlySpan<BlobRecord> m_blobSpan;
    PxExtent3D m_extent;
    PixelFormat m_pixelFormat{PixelFormat::Undefined};
    TextureInfo m_textureInfo;
    BitmapOrigin m_bitmapOrigin{BitmapOrigin::Undefined};

    //! @param pTextureBlobs in the content. The pointer to the raw texture blobs. See RawTextureBlobHelper::GetBlockIndex for how its accessed.
    //! @param blobSpan we expect that blobSpan.size() is be equal to faces*levels*layers
    constexpr RawTextureEx(const TextureType textureType, Span<uint8_t> contentSpan, const ReadOnlySpan<BlobRecord> blobSpan,
                           const PxExtent3D extentPx, const PixelFormat pixelFormat, const TextureInfo& textureInfo,
                           const BitmapOrigin bitmapOrigin) noexcept
      : m_textureType(textureType)
      , m_pContent(contentSpan.data() != nullptr ? contentSpan.data() : RawTextureEmpty::GetEmptyAddress())
      , m_contentByteSize(contentSpan.size())
      , m_blobSpan(blobSpan)
      , m_extent(extentPx)
      , m_pixelFormat(pixelFormat)
      , m_textureInfo(textureInfo)
      , m_bitmapOrigin(bitmapOrigin)
    {
      assert(m_textureType != TextureType::Undefined);
      assert(m_pixelFormat != PixelFormat::Undefined);
      assert(textureInfo.IsValid());
      assert(m_bitmapOrigin != BitmapOrigin::Undefined);
      assert(IsBlobSpanSizeValid(m_blobSpan.size(), m_textureInfo));
      assert(AreBlobsValid(m_contentByteSize, m_blobSpan));
    }

  public:
    constexpr RawTextureEx() noexcept = default;


    constexpr bool IsValid() const noexcept
    {
      return m_textureInfo.IsValid();
    }

    //! @brief Get the bitmap origin of the texture data
    constexpr BitmapOrigin GetBitmapOrigin() const noexcept
    {
      return m_bitmapOrigin;
    }


    constexpr const void* GetContent() const noexcept
    {
      return m_pContent;
    }


    constexpr void* GetContent() noexcept
    {
      return m_pContent;
    }


    constexpr std::size_t GetByteSize() const noexcept
    {
      return m_contentByteSize;
    }

    //! @brief Get the number of faces (one for normal textures, six for cube maps)
    constexpr uint32_t GetFaces() const noexcept
    {
      return m_textureInfo.Faces;
    }

    //! @brief Get the number of array layers (for texture arrays, will be zero for normal textures)
    constexpr uint32_t GetLayers() const noexcept
    {
      return m_textureInfo.Layers;
    }

    //! @brief The number of mip map levels
    constexpr uint32_t GetLevels() const noexcept
    {
      return m_textureInfo.Levels;
    }

    constexpr TextureInfo GetTextureInfo() const noexcept
    {
      return m_textureInfo;
    }

    //! Get the pixel format of the raw bitmap
    constexpr PixelFormat GetPixelFormat() const noexcept
    {
      return m_pixelFormat;
    }

    //! Get the pixel format of the raw bitmap
    constexpr TextureType GetTextureType() const noexcept
    {
      return m_textureType;
    }

    constexpr std::size_t GetBlobCount() const noexcept
    {
      return m_blobSpan.size();
    }

    //! @brief Get a raw texture of the given blob of the texture
    BlobRecord GetTextureBlob(const uint32_t level = 0, const uint32_t face = 0, const uint32_t layer = 0) const;

    //! The stride at the given level (not valid for compressed textures)
    std::size_t GetStride(const std::size_t level = 0) const;

    //! The width and height of the texture in pixels
    PxExtent3D GetExtent(const std::size_t level = 0) const;

    //! The width and height of the texture in pixels
    PxExtent2D GetExtent2D(const std::size_t level = 0) const;

    //! The width and height of the texture in pixels
    PxSize2D GetSize2D(const std::size_t level = 0) const;

    BlobRecord GetBlob(const std::size_t index) const;

    constexpr void SetBitmapOrigin(const BitmapOrigin bitmapOrigin)
    {
      m_bitmapOrigin = bitmapOrigin;
    }

    //! @brief Simplified constructor to handle a very basic 2D texture without mipmaps.
    //! @param pTextureBlobs in the content. The pointer to the raw texture blobs. See RawTextureBlobHelper::GetBlockIndex for how its accessed.
    static RawTextureEx Create(Span<uint8_t> contentSpan, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin bitmapOrigin);

    //! @brief Simplified constructor to handle a very basic 2D texture without mipmaps.
    //! @param pTextureBlobs in the content. The pointer to the raw texture blobs. See RawTextureBlobHelper::GetBlockIndex for how its accessed.
    static RawTextureEx Create(Span<uint8_t> contentSpan, const PxExtent2D extentPx, const PixelFormat pixelFormat, const BitmapOrigin bitmapOrigin);

    //! @param pTextureBlobs in the content. The pointer to the raw texture blobs. See RawTextureBlobHelper::GetBlockIndex for how its accessed.
    //! @param blobSpan we expect that blobSpan.size() is be equal to faces*levels*layers
    static RawTextureEx Create(const TextureType textureType, Span<uint8_t> contentSpan, const ReadOnlySpan<BlobRecord> blobSpan,
                               const PxExtent3D extentPx, const PixelFormat pixelFormat, const TextureInfo& textureInfo,
                               const BitmapOrigin bitmapOrigin);


    //! @param pTextureBlobs in the content. The pointer to the raw texture blobs. See RawTextureBlobHelper::GetBlockIndex for how its accessed.
    //! @param blobSpan we expect that blobSpan.size() is be equal to faces*levels*layers
    static constexpr RawTextureEx UncheckedCreate(const TextureType textureType, Span<uint8_t> contentSpan, const ReadOnlySpan<BlobRecord> blobSpan,
                                                  const PxExtent3D extentPx, const PixelFormat pixelFormat, const TextureInfo& textureInfo,
                                                  const BitmapOrigin bitmapOrigin) noexcept
    {
      return {textureType, contentSpan, blobSpan, extentPx, pixelFormat, textureInfo, bitmapOrigin};
    }


    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr operator ReadOnlyRawTexture() const noexcept
    {
      return ReadOnlyRawTexture::UncheckedCreate(m_textureType, ReadOnlySpan<uint8_t>(static_cast<const uint8_t*>(m_pContent), m_contentByteSize),
                                                 m_blobSpan, m_extent, m_pixelFormat, m_textureInfo, m_bitmapOrigin);
    }


  private:
    static constexpr bool IsBlobSpanSizeValid(const std::size_t blobSpanSize, const TextureInfo& textureInfo) noexcept
    {
      return (blobSpanSize == textureInfo.GetBlobCount()) ||
             (blobSpanSize == 0u && textureInfo.Levels == 1u && textureInfo.Faces == 1u && textureInfo.Layers == 1u);
    }

    static constexpr bool AreBlobsValid(const std::size_t contentByteSize, const ReadOnlySpan<BlobRecord> blobSpan) noexcept
    {
      for (const auto& blob : blobSpan)
      {
        // two checks bypasses any overflow issues
        if (blob.Size > contentByteSize)
        {
          return false;
        }
        if (blob.Offset > (contentByteSize - blob.Size))
        {
          return false;
        }
      }
      return true;
    }
  };
}

#endif
