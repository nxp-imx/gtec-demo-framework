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

#include <FslGraphics/Texture/RawTexture.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <cmath>


namespace Fsl
{
  RawTexture::RawTexture()
    : m_textureType(TextureType::Undefined)
    , m_pContent(nullptr)
    , m_contentByteSize(0)
    , m_pBlobs(nullptr)
    , m_blobCount(0)
    , m_extent()
    , m_pixelFormat(PixelFormat::Undefined)
    , m_textureInfo()
    , m_bitmapOrigin(BitmapOrigin::Undefined)
  {
  }


  RawTexture::RawTexture(const TextureType textureType, const void*const pContent, const std::size_t contentByteSize,
                         const BlobRecord*const pTextureBlobs, const std::size_t textureBlobCount,
                         const Extent3D& extent, const PixelFormat pixelFormat, const TextureInfo& textureInfo,
                         const BitmapOrigin& bitmapOrigin)
    : m_textureType(textureType)
    , m_pContent(pContent)
    , m_contentByteSize(contentByteSize)
    , m_pBlobs(pTextureBlobs)
    , m_blobCount(textureBlobCount)
    , m_extent(extent)
    , m_pixelFormat(pixelFormat)
    , m_textureInfo(textureInfo)
    , m_bitmapOrigin(bitmapOrigin)
  {
    if (m_textureType == TextureType::Undefined)
      throw std::invalid_argument("textureType can not be undefined");
    if (pContent == nullptr)
      throw std::invalid_argument("pContent can not be null");
    if (pTextureBlobs == nullptr)
      throw std::invalid_argument("pTextureBlobs can not be null");
    if (m_pixelFormat == PixelFormat::Undefined)
      throw std::invalid_argument("pixelFormat can not be undefined");
    if (! textureInfo.IsValid())
      throw std::invalid_argument("textureInfo must be valid");
    if (m_bitmapOrigin == BitmapOrigin::Undefined)
      throw std::invalid_argument("bitmap origin can not be undefined");
    if (m_blobCount != textureInfo.GetBlobCount())
      throw std::invalid_argument("the supplied number of bitmap blobs do not match the the amount requested by levels * faces * layers");

#ifndef NDEBUG
    {
      for (std::size_t i = 0; i < textureBlobCount; ++i)
      {
        // two checks bypasses any overflow issues
        if (m_pBlobs[i].Size > contentByteSize)
          throw std::invalid_argument("Content blob is outside the content buffer");
        if (m_pBlobs[i].Offset > (contentByteSize - m_pBlobs[i].Size))
          throw std::invalid_argument("Content blob is outside the content buffer");
      }
    }
#endif
  }

  Extent3D RawTexture::GetExtent(const std::size_t level) const
  {
    if (!IsValid())
      throw UsageErrorException("GetTextureBlob called on invalid object");
    if (level >= m_textureInfo.Levels)
      throw std::invalid_argument("argument out of bounds");

    return level == 0 ? m_extent : Extent3D(m_extent.Width >> level, m_extent.Height >> level, m_extent.Depth >> level);
  }


  BlobRecord RawTexture::GetTextureBlob(const uint32_t level, const uint32_t face, const uint32_t layer) const
  {
    if (!IsValid())
      throw UsageErrorException("GetTextureBlob called on invalid object");
    if (level >= m_textureInfo.Levels || face >= m_textureInfo.Faces || layer >= m_textureInfo.Layers)
      throw std::invalid_argument("argument out of bounds");

    const std::size_t index = m_textureInfo.GetBlockIndex(level, face, layer);

    assert(index <= m_blobCount);
    return m_pBlobs[index];
  }


  BlobRecord RawTexture::GetBlob(const std::size_t index) const
  {
    if (!IsValid())
      throw UsageErrorException("GetBlob called on invalid object");
    if (index > m_blobCount)
      throw std::invalid_argument("argument out of bounds");
    return m_pBlobs[index];
  }


}
