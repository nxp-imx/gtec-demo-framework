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

#include <FslGraphics/Texture/TextureUtil.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/Texture/TextureBlobBuilder.hpp>
#include <FslGraphics/Texture/RawTextureEx.hpp>
#include <FslGraphics/Texture/CubeMapFace.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include <FslGraphics/Bitmap/RawBitmap.hpp>
#include <FslGraphics/Bitmap/RawBitmapEx.hpp>
#include <FslGraphics/Bitmap/Converter/RawBitmapConverter.hpp>
#include <algorithm>
#include <cassert>
#include <utility>

namespace Fsl
{
  namespace
  {
    Texture CreateTargetTexture(const Texture& srcTexture, const PixelFormat desiredPixelFormat)
    {
      assert(!PixelFormatUtil::IsCompressed(srcTexture.GetPixelFormat()));
      TextureBlobBuilder builder(srcTexture.GetTextureType(), srcTexture.GetExtent(), desiredPixelFormat, srcTexture.GetTextureInfo(), srcTexture.GetBitmapOrigin());
      builder.SetDefaultBlobLayout();
      return Texture(std::move(builder));
    }


    bool TryFlipUncompressedInPlace(Texture& rTexture, const BitmapOrigin desiredOrigin)
    {
      assert(desiredOrigin != BitmapOrigin::Undefined && rTexture.GetBitmapOrigin() != desiredOrigin);

      const auto srcPixelFormat = rTexture.GetPixelFormat();
      assert(!PixelFormatUtil::IsCompressed(srcPixelFormat));

      const uint32_t layers = rTexture.GetLayers();
      const uint32_t faces = rTexture.GetFaces();
      const uint32_t levels = rTexture.GetLevels();

      // So we have a uncompressed texture at this point in time
      // So we just need to flip all the faces, mip levels and layers one by one using the RawBitmap conversion capabilities
      {
        RawTextureEx rawTexture;
        Texture::ScopedDirectAccess directAccessSrc(rTexture, rawTexture);

        assert(layers == rawTexture.GetLayers());
        assert(faces == rawTexture.GetFaces());
        assert(levels == rawTexture.GetLevels());

        const auto srcBytesPerPixel = PixelFormatUtil::GetBytesPerPixel(srcPixelFormat);
        uint8_t*const pContent = static_cast<uint8_t*>(rawTexture.GetContentWriteAccess());
        const auto srcOrigin = rawTexture.GetBitmapOrigin();

        for (uint32_t level = 0; level < levels; ++level)
        {
          const auto extent = rawTexture.GetExtent(level);
          const auto srcStride = PixelFormatLayoutUtil::CalcMinimumStride(extent.Width, srcBytesPerPixel);
          const auto fullTextureStride = srcStride * extent.Height;

          for (uint32_t layer = 0; layer < layers; ++layer)
          {
            for (uint32_t face = 0; face < faces; ++face)
            {
              const auto rawBlob = rawTexture.GetTextureBlob(level, face, layer);
              for (uint32_t z = 0; z < extent.Depth; ++z)
              {
                RawBitmapEx rawSrcBitmap(pContent + rawBlob.Offset + (z*fullTextureStride), extent.Width, extent.Height, srcPixelFormat, srcStride, srcOrigin);

                // TODO: use a more generic converter, this relies on the fact we know that there is only two origins
                try
                {
                  RawBitmapUtil::FlipHorizontal(rawSrcBitmap);
                  assert(desiredOrigin == rawSrcBitmap.GetOrigin());
                }
                catch (const std::exception& ex)
                {
                  FSLLOG_WARNING("Unexpected exception: " << ex.what());
                  return false;
                }
              }
            }
          }
        }
        rawTexture.SetBitmapOrigin(desiredOrigin);
      }

      // If this is a cube map we need to flip the top and bottom (posY and negY)
      if (rTexture.GetTextureType() == TextureType::TexCube || rTexture.GetTextureType() == TextureType::TexCubeArray)
      {
        for (uint32_t level = 0; level < levels; ++level)
        {
          for (uint32_t layer = 0; layer < layers; ++layer)
          {
            // We expect cube maps to be stored
            // 0=posX, 1=negX, 2=posY, 3=negY, 4=posZ, 5=negZ
            rTexture.SwapCompatibleBlobs(level, CubeMapFace::PosY, layer, CubeMapFace::NegY, layer);
          }
        }
      }

      return true;
    }


    bool TryDoConvertUncompressedInPlace(Texture& rTexture, const PixelFormat desiredPixelFormat)
    {
      const auto srcPixelFormat = rTexture.GetPixelFormat();
      assert(!PixelFormatUtil::IsCompressed(srcPixelFormat));

      // So we have a uncompressed texture at this point in time
      // So we just need to convert all the faces, mip levels and layers one by one using the RawBitmap conversion capabilities
      {
        RawTextureEx rawTexture;
        Texture::ScopedDirectAccess directAccessSrc(rTexture, rawTexture);

        const uint32_t srcBytesPerPixel = PixelFormatUtil::GetBytesPerPixel(srcPixelFormat);

        uint8_t*const pContent = static_cast<uint8_t*>(rawTexture.GetContentWriteAccess());

        const auto srcOrigin = rawTexture.GetBitmapOrigin();
        const uint32_t layers = rawTexture.GetLayers();
        const uint32_t faces = rawTexture.GetFaces();
        const uint32_t levels = rawTexture.GetLevels();

        for (uint32_t level = 0; level < levels; ++level)
        {
          const auto extent = rawTexture.GetExtent(level);
          const uint32_t srcStride = PixelFormatLayoutUtil::CalcMinimumStride(extent.Width, srcBytesPerPixel);
          const auto fullTextureStride = srcStride * extent.Height;

          for (uint32_t layer = 0; layer < layers; ++layer)
          {
            for (uint32_t face = 0; face < faces; ++face)
            {
              const auto rawBlob = rawTexture.GetTextureBlob(level, face, layer);
              for (uint32_t z = 0; z < extent.Depth; ++z)
              {
                RawBitmapEx rawSrcBitmap(pContent + rawBlob.Offset + (z*fullTextureStride), extent.Width, extent.Height, srcPixelFormat, srcStride, srcOrigin);

                if (!RawBitmapConverter::TryConvert(rawSrcBitmap, desiredPixelFormat))
                  return false;
              }
            }
          }
        }
      }
      rTexture.SetCompatiblePixelFormat(desiredPixelFormat, true);
      return true;
    }


    bool TryDoConvertUncompressedUsingTemporaryTexture(Texture& rTexture, const PixelFormat desiredPixelFormat)
    {
      const auto srcPixelFormat = rTexture.GetPixelFormat();
      assert(!PixelFormatUtil::IsCompressed(srcPixelFormat));

      // TODO: it would be much better to query the converter about if it supports the conversion
      // before all of this

      auto targetTexture = CreateTargetTexture(rTexture, desiredPixelFormat);

      // So we have a uncompressed texture at this point in time
      // So we just need to convert all the faces, mip levels and layers one by one using the RawBitmap conversion capabilities
      {
        RawTexture rawSrcTexture;
        RawTextureEx rawDstTexture;
        Texture::ScopedDirectAccess directAccessSrc(rTexture, rawSrcTexture);
        Texture::ScopedDirectAccess directAccessDst(targetTexture, rawDstTexture);

        assert(rawSrcTexture.GetBitmapOrigin() == rawDstTexture.GetBitmapOrigin());
        assert(rawSrcTexture.GetTextureInfo() == rawDstTexture.GetTextureInfo());

        const uint32_t srcBytesPerPixel = PixelFormatUtil::GetBytesPerPixel(srcPixelFormat);
        const uint32_t dstBytesPerPixel = PixelFormatUtil::GetBytesPerPixel(desiredPixelFormat);

        const uint8_t*const pSrcContent = static_cast<const uint8_t*>(rawSrcTexture.GetContent());
        uint8_t*const pDstContent = static_cast<uint8_t*>(rawDstTexture.GetContentWriteAccess());

        const auto srcOrigin = rawSrcTexture.GetBitmapOrigin();
        const uint32_t layers = rawSrcTexture.GetLayers();
        const uint32_t faces = rawSrcTexture.GetFaces();
        const uint32_t levels = rawSrcTexture.GetLevels();

        for (uint32_t level = 0; level < levels; ++level)
        {
          const auto srcExtent = rawSrcTexture.GetExtent(level);
          const uint32_t srcStride = PixelFormatLayoutUtil::CalcMinimumStride(srcExtent.Width, srcBytesPerPixel);
          const uint32_t dstStride = PixelFormatLayoutUtil::CalcMinimumStride(srcExtent.Width, dstBytesPerPixel);
          const uint32_t srcFullTextureStride = srcStride * srcExtent.Height;
          const uint32_t dstFullTextureStride = dstStride * srcExtent.Height;

          for (uint32_t layer = 0; layer < layers; ++layer)
          {
            for (uint32_t face = 0; face < faces; ++face)
            {
              const auto rawSrcBlob = rawSrcTexture.GetTextureBlob(level, face, layer);
              const auto rawDstBlob = rawDstTexture.GetTextureBlob(level, face, layer);
              for (uint32_t z = 0; z < srcExtent.Depth; ++z)
              {
                RawBitmap rawSrcBitmap(pSrcContent + rawSrcBlob.Offset + (z*srcFullTextureStride), srcExtent.Width, srcExtent.Height, srcPixelFormat, srcStride, srcOrigin);
                RawBitmapEx rawDstBitmap(pDstContent + rawDstBlob.Offset + (z*dstFullTextureStride), srcExtent.Width, srcExtent.Height, desiredPixelFormat, dstStride, srcOrigin);

                // Try the raw bitmap converter
                if (!RawBitmapConverter::TryConvert(rawDstBitmap, rawSrcBitmap))
                  return false;
              }
            }
          }
        }
      }
      rTexture = std::move(targetTexture);
      return true;
    }


    bool TryDoConvert(Texture& rTexture, const PixelFormat desiredPixelFormat)
    {
      const auto srcPixelFormat = rTexture.GetPixelFormat();
      if (desiredPixelFormat == PixelFormat::Undefined || desiredPixelFormat == srcPixelFormat)
      {
        //! Quick exit if since this is a no-op
        return true;
      }

      const auto currentPixelFormatLayout = PixelFormatUtil::GetPixelFormatLayout(srcPixelFormat);
      const auto desiredPixelFormatLayout = PixelFormatUtil::GetPixelFormatLayout(desiredPixelFormat);
      if (currentPixelFormatLayout == desiredPixelFormatLayout)
      {
        // The pixel format layout is 100% compatible, so we only need to change the format
        rTexture.SetCompatiblePixelFormat(desiredPixelFormat);
        return true;
      }

      // The easy things has been checked and handled, so lets check for compression
      if (PixelFormatUtil::IsCompressed(srcPixelFormat))
      {
        if (currentPixelFormatLayout != desiredPixelFormatLayout)
          return false;

        // There was no need to do any compression if desiredPixelFormat was PixelFormat::Undefined
        return desiredPixelFormat == PixelFormat::Undefined;
      }

      if (TryDoConvertUncompressedInPlace(rTexture, desiredPixelFormat))
        return true;

      // Last chance, try to see if we can get it done using a temporary texture as a intermediary
      return TryDoConvertUncompressedUsingTemporaryTexture(rTexture, desiredPixelFormat);

    }
  }


  bool TextureUtil::TryConvert(Texture& rTexture, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin)
  {
    // Convert the pixel format first
    if (!TryDoConvert(rTexture, desiredPixelFormat))
      return false;

    // Check if we need to modify the origin
    if (desiredOrigin != BitmapOrigin::Undefined && rTexture.GetBitmapOrigin() != desiredOrigin)
    {
      if (PixelFormatUtil::IsCompressed(rTexture.GetPixelFormat()))
      {
        // Can't modify compressed texture
        return false;
      }

      if (!TryFlipUncompressedInPlace(rTexture, desiredOrigin))
        return false;
    }
    return true;
  }


  void TextureUtil::Convert(Texture& rTexture, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin)
  {
    // We check a few things here to be able to throw a more describing exception
    if (PixelFormatUtil::IsCompressed(rTexture.GetPixelFormat()))
    {
      const auto currentPixelFormatLayout = PixelFormatUtil::GetPixelFormatLayout(rTexture.GetPixelFormat());
      const auto desiredPixelFormatLayout = PixelFormatUtil::GetPixelFormatLayout(desiredPixelFormat);

      if (desiredPixelFormat != PixelFormat::Undefined && currentPixelFormatLayout != desiredPixelFormatLayout)
        throw NotSupportedException("Can not change the pixel format of compressed textures");
      if (desiredOrigin != BitmapOrigin::Undefined && rTexture.GetBitmapOrigin() != desiredOrigin)
        throw NotSupportedException("Can not change the origin of compressed textures");
    }

    if (!TryConvert(rTexture, desiredPixelFormat, desiredOrigin))
      throw UnsupportedPixelFormatConversionException("Conversion not supported", rTexture.GetPixelFormat(), desiredPixelFormat);
  }


  Extent3D TextureUtil::GetExtentForLevel(const Extent3D& extent, const uint32_t level)
  {
    if (level <= 0)
      return extent;
    return Extent3D(std::max(extent.Width >> level, 1u), std::max(extent.Height >> level, 1u), std::max(extent.Depth >> level, 1u));
  }


  uint32_t TextureUtil::CalcTotalTexels(const Extent3D& extent, const TextureInfo& textureInfo)
  {
    uint32_t totalTexels = 0;
    for (uint32_t level = 0; level < textureInfo.Levels; ++level)
    {
      auto currentExtend = GetExtentForLevel(extent, level);
      totalTexels += (currentExtend.Width * currentExtend.Height * currentExtend.Depth);
    }
    totalTexels *= textureInfo.Layers * textureInfo.Faces;
    return totalTexels;
  }
}
