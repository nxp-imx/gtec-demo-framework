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

#include <FslGraphics/TextureAtlas/TestAtlasTextureGenerator.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslGraphics/ContainerTypeConvert.hpp>
#include <FslGraphics/TextureAtlas/NamedAtlasTexture.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/Log/FmtPixelFormat.hpp>
#include <FslGraphics/TextureAtlas/ITextureAtlas.hpp>

namespace Fsl
{
  namespace TestAtlasTextureGenerator
  {
    namespace
    {
      void FillWithTestPattern(Bitmap& rBitmap, const PxRectangleU& dstRect)
      {
        constexpr const uint32_t colorRed = 0xFFFF0000;
        constexpr const uint32_t colorBlue = 0xFF0000FF;

        const uint32_t xStart = dstRect.X;
        const uint32_t xEnd = xStart + dstRect.Width;
        const uint32_t yStart = dstRect.Y;
        const uint32_t yEnd = yStart + dstRect.Height;

        uint32_t yMagic = 0;
        for (uint32_t y = yStart; y < yEnd; ++y)
        {
          uint32_t startCol = yMagic & 1;
          for (uint32_t x = xStart; x < xEnd; ++x)
          {
            rBitmap.SetNativePixel(x, y, (startCol & 1) != 0u ? colorRed : colorBlue);
            ++startCol;
          }
          yMagic = ~yMagic;
        }
      }
    }

    void PatchWithTestPattern(Bitmap& rBitmap)
    {
      const auto extent = rBitmap.GetExtent();
      if (rBitmap.GetPixelFormat() != PixelFormat::R8G8B8A8_UNORM)
      {
        FSLLOG3_WARNING("Changing pixel format from {} to PixelFormat::R8G8B8A8_UNORM", rBitmap.GetPixelFormat());
        const auto origin = rBitmap.GetOrigin();
        rBitmap.Reset(extent, PixelFormat::R8G8B8A8_UNORM, origin);
      }
      rBitmap.Clear();

      FillWithTestPattern(rBitmap, PxRectangleU(0, 0, rBitmap.GetExtent().Width, rBitmap.GetExtent().Height));
    }


    void PatchWithTestPattern(Bitmap& rBitmap, const ITextureAtlas& sourceAtlas)
    {
      const auto extent = rBitmap.GetExtent();
      if (rBitmap.GetPixelFormat() != PixelFormat::R8G8B8A8_UNORM)
      {
        FSLLOG3_WARNING("Changing pixel format from {} to PixelFormat::R8G8B8A8_UNORM", rBitmap.GetPixelFormat());
        const auto origin = rBitmap.GetOrigin();
        rBitmap.Reset(extent, PixelFormat::R8G8B8A8_UNORM, origin);
      }

      rBitmap.Clear();

      const auto count = sourceAtlas.Count();
      for (uint32_t i = 0; i < count; ++i)
      {
        const NamedAtlasTexture& rEntry = sourceAtlas.GetEntry(i);
        if (rEntry.TextureInfo.TrimmedRectPx.Right() > rBitmap.Width() || rEntry.TextureInfo.TrimmedRectPx.Bottom() > rBitmap.Height())
        {
          throw NotSupportedException("rBitmap could not contain the source atlas entries");
        }
        FillWithTestPattern(rBitmap, rEntry.TextureInfo.TrimmedRectPx);
      }
    }

    void PatchWithTestPattern(Texture& rTexture, const ITextureAtlas& sourceAtlas)
    {
      Bitmap tmpBitmap = ContainerTypeConvert::Convert(std::move(rTexture));
      PatchWithTestPattern(tmpBitmap, sourceAtlas);
      rTexture = ContainerTypeConvert::Convert(std::move(tmpBitmap));
    }

    void PatchWithTestPattern(Texture& rTexture)
    {
      Bitmap tmpBitmap = ContainerTypeConvert::Convert(std::move(rTexture));
      PatchWithTestPattern(tmpBitmap);
      rTexture = ContainerTypeConvert::Convert(std::move(tmpBitmap));
    }

    Bitmap CreateTestPatternBitmap(const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin,
                                   const ITextureAtlas& sourceAtlas)
    {
      Bitmap newBitmap(extent, pixelFormat, origin);
      PatchWithTestPattern(newBitmap, sourceAtlas);
      return newBitmap;
    }

    Bitmap CreateTestPatternBitmap(const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin)
    {
      Bitmap newBitmap(extent, pixelFormat, origin);
      PatchWithTestPattern(newBitmap);
      return newBitmap;
    }

    Texture CreateTestPatternTexture(const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin,
                                     const ITextureAtlas& sourceAtlas)
    {
      return ContainerTypeConvert::Convert(CreateTestPatternBitmap(extent, pixelFormat, origin, sourceAtlas));
    }

    Texture CreateTestPatternTexture(const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin)
    {
      return ContainerTypeConvert::Convert(CreateTestPatternBitmap(extent, pixelFormat, origin));
    }
  }
}
