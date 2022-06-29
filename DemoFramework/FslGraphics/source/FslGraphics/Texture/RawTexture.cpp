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

#include <FslBase/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Texture/RawTexture.hpp>
#include <algorithm>
#include <cmath>


namespace Fsl
{
  namespace
  {
    constexpr PxExtent3D DoGetExtent(const PxExtent3D& maxExtent, const std::size_t level)
    {
      return level == 0 ? maxExtent
                        : PxExtent3D(std::max(maxExtent.Width >> level, static_cast<uint32_t>(1u)),
                                     std::max(maxExtent.Height >> level, static_cast<uint32_t>(1u)),
                                     std::max(maxExtent.Depth >> level, static_cast<uint32_t>(1u)));
    }
  }


  RawTexture::RawTexture(const void* const pContent, const std::size_t contentByteSize, const PxExtent2D& extent, const PixelFormat pixelFormat,
                         const BitmapOrigin& bitmapOrigin)
    : RawTexture(TextureType::Tex2D, pContent, contentByteSize, nullptr, 0u, PxExtent3D(extent, 1u), pixelFormat, TextureInfo(1u, 1u, 1u),
                 bitmapOrigin)
  {
  }


  RawTexture::RawTexture(const TextureType textureType, const void* const pContent, const std::size_t contentByteSize,
                         const BlobRecord* const pTextureBlobs, const std::size_t textureBlobCount, const PxExtent3D& extent,
                         const PixelFormat pixelFormat, const TextureInfo& textureInfo, const BitmapOrigin& bitmapOrigin)
    : m_textureType(textureType)
    , m_pContent(pContent)
    , m_contentByteSize(contentByteSize)
    , m_blobEverything(0, contentByteSize)
    , m_pBlobs(pTextureBlobs)
    , m_blobCount(textureBlobCount)
    , m_extent(extent)
    , m_pixelFormat(pixelFormat)
    , m_textureInfo(textureInfo)
    , m_bitmapOrigin(bitmapOrigin)
  {
    if (m_textureType == TextureType::Undefined)
    {
      throw std::invalid_argument("textureType can not be undefined");
    }
    if (pContent == nullptr)
    {
      throw std::invalid_argument("pContent can not be null");
    }
    if (m_pBlobs == nullptr && textureBlobCount == 0u && textureType == TextureType::Tex2D)
    {
      // Override the user supplied settings and allow a null blob for Tex2D
      m_pBlobs = &m_blobEverything;
      m_blobCount = 1u;
    }
    if (m_pBlobs == nullptr)
    {
      throw std::invalid_argument("pTextureBlobs can not be null if textureBlobCount > 0");
    }
    if (m_pixelFormat == PixelFormat::Undefined)
    {
      throw std::invalid_argument("pixelFormat can not be undefined");
    }
    if (!textureInfo.IsValid())
    {
      throw std::invalid_argument("textureInfo must be valid");
    }
    if (m_bitmapOrigin == BitmapOrigin::Undefined)
    {
      throw std::invalid_argument("bitmap origin can not be undefined");
    }
    if (m_blobCount != textureInfo.GetBlobCount())
    {
      throw std::invalid_argument("the supplied number of bitmap blobs do not match the the amount requested by levels * faces * layers");
    }

#ifndef NDEBUG
    {
      for (std::size_t i = 0; i < m_blobCount; ++i)
      {
        // two checks bypasses any overflow issues
        if (m_pBlobs[i].Size > contentByteSize)
        {
          throw std::invalid_argument("Content blob is outside the content buffer");
        }
        if (m_pBlobs[i].Offset > (contentByteSize - m_pBlobs[i].Size))
        {
          throw std::invalid_argument("Content blob is outside the content buffer");
        }
      }
    }
#endif
  }


  std::size_t RawTexture::GetStride(const std::size_t level) const
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

  PxExtent3D RawTexture::GetExtent(const std::size_t level) const
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


  PxExtent2D RawTexture::GetExtent2D(const std::size_t level) const
  {
    auto res = GetExtent(level);
    if (res.Depth != 1u)
    {
      throw UsageErrorException("GetExtent2D called on a non 2d texture");
    }
    return {res.Width, res.Height};
  }


  BlobRecord RawTexture::GetTextureBlob(const uint32_t level, const uint32_t face, const uint32_t layer) const
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

    assert(index <= m_blobCount);
    return m_pBlobs[index];
  }


  BlobRecord RawTexture::GetBlob(const std::size_t index) const
  {
    if (!IsValid())
    {
      throw UsageErrorException("GetBlob called on invalid object");
    }
    if (index > m_blobCount)
    {
      throw std::invalid_argument("argument out of bounds");
    }
    return m_pBlobs[index];
  }
}
