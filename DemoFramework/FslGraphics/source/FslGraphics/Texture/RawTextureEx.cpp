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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/MathHelper_MinMax.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Texture/RawTextureEx.hpp>
#include <algorithm>
#include <cmath>


namespace Fsl
{
  namespace
  {
    constexpr PxExtent3D DoGetExtent(const PxExtent3D& maxExtent, const std::size_t level) noexcept
    {
      return level == 0 ? maxExtent
                        : PxExtent3D::Create(MathHelper::Max(maxExtent.Width.Value >> level, static_cast<uint32_t>(1u)),
                                             MathHelper::Max(maxExtent.Height.Value >> level, static_cast<uint32_t>(1u)),
                                             MathHelper::Max(maxExtent.Depth.Value >> level, static_cast<uint32_t>(1u)));
    }
  }


  std::size_t RawTextureEx::GetStride(const std::size_t level) const
  {
    if (!IsValid())
    {
      throw UsageErrorException("GetTextureBlob called on invalid object");
    }
    if (level >= m_textureInfo.Levels)
    {
      throw std::invalid_argument("argument out of bounds");
    }

    const auto extent = DoGetExtent(m_extent, level);
    return PixelFormatUtil::CalcMinimumStride(extent.Width, m_pixelFormat);
  }

  PxExtent3D RawTextureEx::GetExtent(const std::size_t level) const
  {
    if (!IsValid())
    {
      throw UsageErrorException("GetTextureBlob called on invalid object");
    }
    if (level >= m_textureInfo.Levels)
    {
      throw std::invalid_argument("argument out of bounds");
    }

    return DoGetExtent(m_extent, level);
  }


  PxExtent2D RawTextureEx::GetExtent2D(const std::size_t level) const
  {
    auto res = GetExtent(level);
    if (res.Depth.Value != 1u)
    {
      throw UsageErrorException("GetExtent2D called on a non 2d texture");
    }
    return {res.Width, res.Height};
  }


  PxSize2D RawTextureEx::GetSize2D(const std::size_t level) const
  {
    return TypeConverter::To<PxSize2D>(GetExtent2D(level));
  }


  BlobRecord RawTextureEx::GetTextureBlob(const uint32_t level, const uint32_t face, const uint32_t layer) const
  {
    if (!IsValid())
    {
      throw UsageErrorException("GetTextureBlob called on invalid object");
    }
    if (level >= m_textureInfo.Levels || face >= m_textureInfo.Faces || layer >= m_textureInfo.Layers)
    {
      throw std::invalid_argument("argument out of bounds");
    }

    const std::size_t index = m_textureInfo.GetBlockIndex(level, face, layer);
    if (!m_blobSpan.empty())
    {
      return m_blobSpan[index];
    }
    assert(index == 0u);
    return {0u, m_contentByteSize};
  }


  BlobRecord RawTextureEx::GetBlob(const std::size_t index) const
  {
    if (!IsValid())
    {
      throw UsageErrorException("GetBlob called on invalid object");
    }
    if (!m_blobSpan.empty())
    {
      return m_blobSpan.at(index);
    }
    if (index != 0u)
    {
      throw std::out_of_range("pos out of range");
    }
    return {0u, m_contentByteSize};
  }


  RawTextureEx RawTextureEx::Create(Span<uint8_t> contentSpan, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin bitmapOrigin)
  {
    return Create(contentSpan, TypeConverter::To<PxExtent2D>(sizePx), pixelFormat, bitmapOrigin);
  }


  RawTextureEx RawTextureEx::Create(Span<uint8_t> contentSpan, const PxExtent2D extentPx, const PixelFormat pixelFormat,
                                    const BitmapOrigin bitmapOrigin)
  {
    return Create(TextureType::Tex2D, contentSpan, {}, PxExtent3D(extentPx, PxValueU(1u)), pixelFormat, TextureInfo(1u, 1u, 1u), bitmapOrigin);
  }


  RawTextureEx RawTextureEx::Create(const TextureType textureType, Span<uint8_t> contentSpan, const ReadOnlySpan<BlobRecord> blobSpan,
                                    const PxExtent3D extentPx, const PixelFormat pixelFormat, const TextureInfo& textureInfo,
                                    const BitmapOrigin bitmapOrigin)
  {
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
      throw std::invalid_argument("textureInfo must be valid");
    }
    if (bitmapOrigin == BitmapOrigin::Undefined)
    {
      throw std::invalid_argument("bitmap origin can not be undefined");
    }
    if (!IsBlobSpanSizeValid(blobSpan.size(), textureInfo))
    {
      throw std::invalid_argument("the supplied number of blobs do not match the the amount requested by levels * faces * layers");
    }
    if (!AreBlobsValid(contentSpan.size(), blobSpan))
    {
      throw std::invalid_argument("Content blob is outside the content buffer");
    }
    return {textureType, contentSpan, blobSpan, extentPx, pixelFormat, textureInfo, bitmapOrigin};
  }
}
