/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Texture/TextureBlobBuilder.hpp>
#include <FslGraphics/Texture/TextureTypeUtil.hpp>
#include <FslGraphics/Texture/TextureUtil.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <utility>

namespace Fsl
{
  // move assignment operator
  TextureBlobBuilder& TextureBlobBuilder::operator=(TextureBlobBuilder&& other) noexcept
  {
    if (this != &other)
    {
      // Free existing resources then transfer the content of other to this one and fill other with default values
      ResetNoThrow();

      // Claim ownership here
      m_textureType = other.m_textureType;
      m_extent = other.m_extent;
      m_pixelFormat = other.m_pixelFormat;
      m_textureInfo = other.m_textureInfo;
      m_bitmapOrigin = other.m_bitmapOrigin;
      m_blobs = std::move(other.m_blobs);
      m_totalTexels = other.m_totalTexels;
      m_contentSize = other.m_contentSize;

      // Remove the data from other
      other.m_textureType = TextureType::Undefined;
      other.m_extent = PxExtent3D();
      other.m_pixelFormat = PixelFormat::Undefined;
      other.m_textureInfo = TextureInfo();
      other.m_bitmapOrigin = BitmapOrigin::Undefined;
      other.m_totalTexels = 0;
      other.m_contentSize = 0;
    }
    return *this;
  }


  // Transfer ownership from other to this
  TextureBlobBuilder::TextureBlobBuilder(TextureBlobBuilder&& other) noexcept
    : m_textureType(other.m_textureType)
    , m_extent(other.m_extent)
    , m_pixelFormat(other.m_pixelFormat)
    , m_textureInfo(other.m_textureInfo)
    , m_bitmapOrigin(other.m_bitmapOrigin)
    , m_blobs(std::move(other.m_blobs))
    , m_totalTexels(other.m_totalTexels)
    , m_contentSize(other.m_contentSize)
  {
    // Remove the data from other
    other.m_textureType = TextureType::Undefined;
    other.m_extent = PxExtent3D();
    other.m_pixelFormat = PixelFormat::Undefined;
    other.m_textureInfo = TextureInfo();
    other.m_bitmapOrigin = BitmapOrigin::Undefined;
    other.m_totalTexels = 0;
    other.m_contentSize = 0;
  }


  TextureBlobBuilder::TextureBlobBuilder() = default;


  TextureBlobBuilder::TextureBlobBuilder(const TextureType textureType, const PxExtent3D& extent, const PixelFormat pixelFormat,
                                         const TextureInfo& textureInfo, const BitmapOrigin bitmapOrigin, const bool useDefaultBlobLayout)
    : TextureBlobBuilder()
  {
    Reset(textureType, extent, pixelFormat, textureInfo, bitmapOrigin, useDefaultBlobLayout);
  }


  TextureBlobBuilder::TextureBlobBuilder(const TextureType textureType, const PxExtent3D& extent, const PixelFormat pixelFormat,
                                         const TextureInfo& textureInfo, const BitmapOrigin bitmapOrigin, const std::size_t contentSize)
  {
    Reset(textureType, extent, pixelFormat, textureInfo, bitmapOrigin, contentSize);
  }


  BlobRecord TextureBlobBuilder::GetBlob(const uint32_t level, const uint32_t face, const uint32_t layer) const
  {
    if (!IsValid())
    {
      throw UsageErrorException("GetBlob called on invalid object");
    }
    if (level >= m_textureInfo.Levels || face >= m_textureInfo.Faces || layer >= m_textureInfo.Layers)
    {
      throw std::invalid_argument("argument out of bounds");
    }

    const std::size_t index = m_textureInfo.GetBlockIndex(level, face, layer);
    assert(index <= m_blobs.size());
    return m_blobs[index];
  }


  void TextureBlobBuilder::SetBlob(const BlobRecord& record, const uint32_t level, const uint32_t face, const uint32_t layer)
  {
    if (!IsValid())
    {
      throw UsageErrorException("GetBlob called on invalid object");
    }

    if (level >= m_textureInfo.Levels || face >= m_textureInfo.Faces || layer >= m_textureInfo.Layers)
    {
      throw std::invalid_argument("argument out of bounds");
    }
    if (record.Size > m_contentSize)
    {
      throw std::invalid_argument("blob content is out of bounds");
    }
    if (record.Offset > (m_contentSize - record.Size))
    {
      throw std::invalid_argument("blob content is out of bounds");
    }

    const std::size_t index = m_textureInfo.GetBlockIndex(level, face, layer);
    assert(index <= m_blobs.size());
    m_blobs[index] = record;
  }


  void TextureBlobBuilder::SetDefaultBlobLayout()
  {
    if (!IsValid())
    {
      throw UsageErrorException("GetBlob called on invalid object");
    }

    if (PixelFormatUtil::IsCompressed(m_pixelFormat))
    {
      throw UsageErrorException("SetDefaultBlobLayout does not work on compressed formats");
    }


    const auto bytesPerPixel = PixelFormatUtil::GetBytesPerPixel(m_pixelFormat);

    std::size_t offset = 0;
    // uint32_t texel = 0;
    for (uint32_t level = 0; level < m_textureInfo.Levels; ++level)
    {
      const PxExtent3D currentExtent = GetExtent(level);
      const std::size_t stride = PixelFormatUtil::CalcMinimumStride(currentExtent.Width, bytesPerPixel);
      const std::size_t blobSize = (stride * currentExtent.Height.Value) * currentExtent.Depth.Value;
      // uint32_t texel2 = (currentExtent.Width * currentExtent.Height * currentExtent.Depth).Value;
      for (uint32_t layer = 0; layer < m_textureInfo.Layers; ++layer)
      {
        for (uint32_t face = 0; face < m_textureInfo.Faces; ++face)
        {
          assert(blobSize <= m_contentSize);
          assert(offset <= (m_contentSize - blobSize));
          SetBlob(BlobRecord(offset, blobSize), level, face, layer);
          offset += blobSize;
          // texel += texel2;
        }
      }
    }
  }


  void TextureBlobBuilder::Reset()
  {
    if (IsValid())
    {
      ResetNoThrow();
    }
  }


  void TextureBlobBuilder::Reset(const TextureType textureType, const PxExtent3D& extent, const PixelFormat pixelFormat,
                                 const TextureInfo& textureInfo, const BitmapOrigin bitmapOrigin, const bool useDefaultBlobLayout)
  {
    DoReset(textureType, extent, pixelFormat, textureInfo, bitmapOrigin, false, 0);
    if (useDefaultBlobLayout)
    {
      SetDefaultBlobLayout();
    }
  }


  void TextureBlobBuilder::Reset(const TextureType textureType, const PxExtent3D& extent, const PixelFormat pixelFormat,
                                 const TextureInfo& textureInfo, const BitmapOrigin bitmapOrigin, const std::size_t contentSize)
  {
    DoReset(textureType, extent, pixelFormat, textureInfo, bitmapOrigin, true, contentSize);
  }


  PxExtent3D TextureBlobBuilder::GetExtent(const uint32_t level) const
  {
    if (!IsValid())
    {
      throw UsageErrorException("GetExtent called on invalid object");
    }
    if (level >= m_textureInfo.Levels)
    {
      throw std::invalid_argument("level is out of bounds");
    }

    return TextureUtil::GetExtentForLevel(m_extent, level);
  }


  PixelFormatLayout TextureBlobBuilder::GetPixelFormatLayout() const
  {
    return PixelFormatUtil::GetPixelFormatLayout(m_pixelFormat);
  }

  BlobRecord TextureBlobBuilder::GetBlobByIndex(const std::size_t index) const
  {
    if (!IsValid())
    {
      throw UsageErrorException("GetExtent called on invalid object");
    }
    if (index >= m_blobs.size())
    {
      throw std::invalid_argument("index is out of bounds");
    }

    return m_blobs[index];
  }


  void TextureBlobBuilder::DoReset(const TextureType textureType, const PxExtent3D& extent, const PixelFormat pixelFormat,
                                   const TextureInfo& textureInfo, const BitmapOrigin bitmapOrigin, const bool hasContentSize,
                                   const std::size_t contentSize)
  {
    if (IsValid())
    {
      ResetNoThrow();
    }

    if (textureType == TextureType::Undefined)
    {
      throw std::invalid_argument("textureType can not be undefined");
    }
    if (pixelFormat == PixelFormat::Undefined)
    {
      throw std::invalid_argument("pixelFormat can not be undefined");
    }
    if (!textureInfo.IsValid())
    {
      throw std::invalid_argument("texture info must be valid");
    }
    if (textureInfo.Faces != TextureTypeUtil::GetFaceCount(textureType))
    {
      throw std::invalid_argument("Unexpected amount of faces");
    }
    if (bitmapOrigin == BitmapOrigin::Undefined)
    {
      throw std::invalid_argument("bitmapOrigin can not be undefined");
    }

    //! Just a security check to validate our assumptions (all of this ought to have been caught by a exception)
    assert(textureInfo.Levels > 0);
    assert(textureInfo.Faces > 0);
    assert(textureInfo.Layers > 0);

    const uint32_t totalTexels = TextureUtil::CalcTotalTexels(extent, textureInfo);
    auto usedContentSize = contentSize;
    if (PixelFormatUtil::IsCompressed(pixelFormat))
    {
      if (!hasContentSize || usedContentSize == 0)
      {
        throw std::invalid_argument("For compressed texture we expect a contentSize > 0");
      }
    }
    else
    {
      const std::size_t bytesPerPixel = PixelFormatUtil::GetBytesPerPixel(pixelFormat);
      usedContentSize = totalTexels * bytesPerPixel;
      if (hasContentSize && contentSize != usedContentSize)
      {
        throw std::invalid_argument("The supplied content size did not match our expectations");
      }
    }


    m_textureType = textureType;
    m_extent = extent;
    m_pixelFormat = pixelFormat;
    m_textureInfo = textureInfo;
    m_bitmapOrigin = bitmapOrigin;
    m_blobs.resize(textureInfo.GetBlobCount());
    m_totalTexels = totalTexels;
    m_contentSize = usedContentSize;
  }


  void TextureBlobBuilder::ResetNoThrow() noexcept
  {
    m_textureType = TextureType::Undefined;
    m_extent = PxExtent3D();
    m_pixelFormat = PixelFormat::Undefined;
    m_textureInfo = TextureInfo();
    m_bitmapOrigin = BitmapOrigin::Undefined;
    m_blobs.clear();
    m_totalTexels = 0;
    m_contentSize = 0;
  }
}
