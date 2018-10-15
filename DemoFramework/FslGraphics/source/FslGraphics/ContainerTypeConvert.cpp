/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslGraphics/ContainerTypeConvert.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Texture/TextureBlobBuilder.hpp>
#include <FslBase/Exceptions.hpp>
#include <limits>

namespace Fsl
{
  namespace ContainerTypeConvert
  {
    Texture Convert(Bitmap&& bitmap)
    {
      if (!bitmap.IsValid())
      {
        return Texture();
      }

      // Extract all relevant information about the bitmap
      RawBitmap rawBitmap;
      {
        Bitmap::ScopedDirectAccess access(bitmap, rawBitmap);
      }
      // Beware the rawBitmap pointers are not valid anymore

      std::vector<uint8_t> contentVector;
      bitmap.ReleaseInto(contentVector);
      // Bitmap has now been reset, so dont use it

      return Texture(std::move(contentVector), rawBitmap.GetExtent(), rawBitmap.GetPixelFormat(), rawBitmap.GetOrigin());
    }


    Bitmap Convert(Texture&& texture)
    {
      if (!texture.IsValid())
      {
        return Bitmap();
      }
      if (texture.GetLevels() != 1 || texture.GetFaces() != 1 || texture.GetLayers() != 1)
      {
        throw std::invalid_argument("the texture did not contain one image");
      }
      if (texture.GetTextureType() != TextureType::Tex1D && texture.GetTextureType() != TextureType::Tex2D)
      {
        throw std::invalid_argument("the texture did not contain one image");
      }

      const auto pixelFormat = texture.GetPixelFormat();
      if (PixelFormatUtil::IsCompressed(pixelFormat))
      {
        throw std::invalid_argument("the texture can not be compressed");
      }

      const auto stride = texture.GetStride();
      if (stride > std::numeric_limits<uint32_t>::max())
      {
        throw std::invalid_argument("the texture stride is not supported");
      }

      const auto origin = texture.GetBitmapOrigin();
      const auto extent = texture.GetExtent();

      std::vector<uint8_t> contentVector;
      texture.ReleaseInto(contentVector);
      // Texture has now been reset, so dont use it

      return Bitmap(std::move(contentVector), Extent2D(extent.Width, extent.Height), pixelFormat, static_cast<uint32_t>(stride), origin);
    }
  }
}
