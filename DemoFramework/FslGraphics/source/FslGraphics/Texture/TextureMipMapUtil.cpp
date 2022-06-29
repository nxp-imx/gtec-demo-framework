/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include <FslGraphics/Log/Texture/FmtTextureType.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Texture/RawTextureHelper.hpp>
#include <FslGraphics/Texture/TextureBlobBuilder.hpp>
#include <FslGraphics/Texture/TextureMipMapUtil.hpp>

namespace Fsl
{
  namespace TextureMipMapUtil
  {
    uint32_t CountMipMapLevels(uint32_t size)
    {
      if (!MathHelper::IsPowerOfTwo(size))
      {
        throw std::invalid_argument("size is not a power of two");
      }

      uint32_t levels = 0;
      while (size > 0)
      {
        size >>= 1;
        ++levels;
      }
      return levels;
    }

    Texture GenerateMipMaps(const Bitmap& src, const TextureMipMapFilter filter)
    {
      RawBitmap srcBitmap;
      Bitmap::ScopedDirectAccess access(src, srcBitmap);
      return GenerateMipMaps(srcBitmap, filter);
    }

    Texture GenerateMipMaps(const Texture& src, const TextureMipMapFilter filter)
    {
      RawTexture srcTexture;
      Texture::ScopedDirectAccess access(src, srcTexture);
      return GenerateMipMaps(srcTexture, filter);
    }

    Texture GenerateMipMaps(const RawBitmap& src, const TextureMipMapFilter filter)
    {
      RawTexture srcTexture = RawTextureHelper::ToRawTexture(src);
      return GenerateMipMaps(srcTexture, filter);
    }

    Texture GenerateMipMaps(const RawTexture& src, const TextureMipMapFilter filter)
    {
      if (!src.IsValid())
      {
        throw std::invalid_argument("src must be valid");
      }
      const PixelFormat pixelFormat = src.GetPixelFormat();
      const BitmapOrigin origin = src.GetBitmapOrigin();
      if (PixelFormatUtil::IsCompressed(pixelFormat))
      {
        throw std::invalid_argument("src pixel format can not be compressed");
      }
      PxExtent2D extent = src.GetExtent2D();
      if (extent.Width != extent.Height || !MathHelper::IsPowerOfTwo(extent.Width))
      {
        throw NotSupportedException("We expect a square pow2 texture");
      }
      if (src.GetLayers() != 1u || src.GetLevels() != 1u)
      {
        throw NotSupportedException("texture Layers and Faces must be 1");
      }

      switch (src.GetTextureType())
      {
      case TextureType::Tex1D:
      case TextureType::Tex2D:
      case TextureType::TexCube:
        break;
      default:
        throw NotSupportedException(fmt::format("unsupported texture type: {}", src.GetTextureType()));
      }

      const uint32_t mipLevels = CountMipMapLevels(extent.Width);
      const TextureInfo textureInfo(mipLevels, src.GetFaces(), src.GetLayers());
      Texture result(TextureBlobBuilder(src.GetTextureType(), src.GetExtent(), pixelFormat, textureInfo, origin, true));
      {
        RawTextureEx rawDstTexture;
        Texture::ScopedDirectAccess dstAccess(result, rawDstTexture);

        auto* const pDstStart = reinterpret_cast<uint8_t*>(rawDstTexture.GetContentWriteAccess());
        {    // Copy the original 'faces' directly
          const auto* const pSrcStart = reinterpret_cast<const uint8_t*>(src.GetContent());
          for (uint32_t faceIndex = 0; faceIndex < textureInfo.Faces; ++faceIndex)
          {
            BlobRecord srcBlobRecord = src.GetTextureBlob(0, faceIndex, 0);
            BlobRecord dstBlobRecord = rawDstTexture.GetTextureBlob(0, faceIndex, 0);
            if (srcBlobRecord.Size != dstBlobRecord.Size)
            {
              throw std::logic_error("internal error, the blob sizes did not match");
            }
            const uint8_t* const pSrc = (pSrcStart + srcBlobRecord.Offset);
            uint8_t* pDst = (pDstStart + dstBlobRecord.Offset);
            std::memcpy(pDst, pSrc, dstBlobRecord.Size);
          }
        }
        if (textureInfo.Layers > 0u)
        {    // Generate the mip maps
          const uint32_t finalSrcLevel = textureInfo.Levels - 1;
          uint32_t width = extent.Width;
          uint32_t height = extent.Height;
          for (uint32_t levelIndex = 0; levelIndex < finalSrcLevel; ++levelIndex)
          {
            for (uint32_t faceIndex = 0; faceIndex < textureInfo.Faces; ++faceIndex)
            {
              BlobRecord srcBlobRecord = rawDstTexture.GetTextureBlob(levelIndex, faceIndex, 0);
              BlobRecord dstBlobRecord = rawDstTexture.GetTextureBlob(levelIndex + 1, faceIndex, 0);
              // Since we copied the original data to dest and are reusing the previous mipmaps pDstStart is the base
              RawBitmap srcBitmap(pDstStart + srcBlobRecord.Offset, width, height, pixelFormat, origin);
              RawBitmapEx dstBitmap(pDstStart + dstBlobRecord.Offset, width / 2, height / 2, pixelFormat, origin);

              switch (filter)
              {
              case TextureMipMapFilter::Nearest:
                RawBitmapUtil::DownscaleNearest(dstBitmap, srcBitmap);
                break;
              case TextureMipMapFilter::Box:
                RawBitmapUtil::DownscaleBoxFilter(dstBitmap, srcBitmap);
                break;
              default:
                throw NotSupportedException("Unsupported filter");
              }
            }
            width /= 2;
            height /= 2;
          }
        }
      }
      return result;
    }
  };
}
