/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslBase/Math/MathHelper_CalcOptimalSize.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/RawBitmapEx.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Font/EmbeddedFont8x8.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <array>
#include <cassert>
#include <cmath>

namespace Fsl
{
  namespace
  {
    // The embedded monochrome font is based on the 8x8 bitmapped font by John Hall
    // License: This font is free to use for any purpose.
    // http://overcode.yak.net/12
    //
    constexpr std::array<uint8_t, 760> g_monochromeFont = {
      8,  8,  8,  8,  8,  0,  8,  0,  20, 20, 0,  0,  0,  0,  0,  0,   20, 20, 127, 20, 127, 20, 20,  0,  8,   30, 40, 28,  10, 60, 8,  0,   0,   50,
      52, 8,  22, 38, 0,  0,  24, 40, 16, 40, 70, 68, 58, 0,  8,  8,   0,  0,  0,   0,  0,   0,  4,   8,  16,  16, 16, 8,   4,  0,  16, 8,   4,   4,
      4,  8,  16, 0,  8,  73, 42, 28, 42, 73, 8,  0,  8,  8,  8,  127, 8,  8,  8,   0,  0,   0,  0,   0,  12,  12, 4,  8,   0,  0,  0,  127, 0,   0,
      0,  0,  0,  0,  0,  0,  0,  12, 12, 0,  1,  2,  4,  8,  16, 32,  64, 0,  28,  34, 34,  42, 34,  34, 28,  0,  8,  24,  8,  8,  8,  8,   28,  0,
      28, 34, 2,  4,  8,  16, 62, 0,  28, 34, 2,  12, 2,  34, 28, 0,   12, 20, 36,  62, 4,   4,  14,  0,  62,  32, 32, 60,  2,  34, 28, 0,   28,  34,
      32, 60, 34, 34, 28, 0,  62, 2,  4,  8,  16, 16, 16, 0,  28, 34,  34, 28, 34,  34, 28,  0,  28,  34, 34,  30, 2,  34,  28, 0,  0,  12,  12,  0,
      12, 12, 0,  0,  0,  12, 12, 0,  12, 12, 4,  8,  4,  8,  16, 32,  16, 8,  4,   0,  0,   0,  127, 0,  127, 0,  0,  0,   32, 16, 8,  4,   8,   16,
      32, 0,  28, 34, 2,  4,  8,  0,  8,  0,  28, 34, 46, 42, 46, 32,  28, 0,  28,  34, 34,  62, 34,  34, 34,  0,  60, 34,  34, 60, 34, 34,  60,  0,
      28, 34, 32, 32, 32, 34, 28, 0,  60, 34, 34, 34, 34, 34, 60, 0,   62, 32, 32,  60, 32,  32, 62,  0,  62,  32, 32, 62,  32, 32, 32, 0,   28,  34,
      32, 46, 34, 34, 28, 0,  34, 34, 34, 62, 34, 34, 34, 0,  28, 8,   8,  8,  8,   8,  28,  0,  14,  4,  4,   4,  36, 36,  24, 0,  34, 34,  36,  56,
      36, 34, 34, 0,  16, 16, 16, 16, 16, 16, 30, 0,  65, 99, 85, 73,  65, 65, 65,  0,  34,  50, 42,  42, 38,  34, 34, 0,   28, 34, 34, 34,  34,  34,
      28, 0,  28, 18, 18, 28, 16, 16, 16, 0,  28, 34, 34, 34, 34, 34,  28, 6,  60,  34, 34,  60, 40,  36, 34,  0,  28, 34,  32, 28, 2,  34,  28,  0,
      62, 8,  8,  8,  8,  8,  8,  0,  34, 34, 34, 34, 34, 34, 28, 0,   34, 34, 34,  20, 20,  8,  8,   0,  65,  65, 65, 42,  42, 20, 20, 0,   34,  34,
      20, 8,  20, 34, 34, 0,  34, 34, 20, 8,  8,  8,  8,  0,  62, 2,   4,  8,  16,  32, 62,  0,  28,  16, 16,  16, 16, 16,  28, 0,  64, 32,  16,  8,
      4,  2,  1,  0,  28, 4,  4,  4,  4,  4,  28, 0,  8,  20, 34, 0,   0,  0,  0,   0,  0,   0,  0,   0,  0,   0,  0,  127, 16, 8,  0,  0,   0,   0,
      0,  0,  0,  28, 2,  30, 34, 34, 29, 0,  16, 16, 28, 18, 18, 18,  44, 0,  0,   0,  28,  32, 32,  32, 28,  0,  2,  2,   14, 18, 18, 18,  13,  0,
      0,  0,  28, 34, 62, 32, 28, 0,  12, 18, 16, 56, 16, 16, 16, 0,   0,  0,  29,  34, 34,  30, 2,   28, 32,  32, 44, 50,  34, 34, 34, 0,   0,   8,
      0,  8,  8,  8,  8,  0,  0,  8,  0,  8,  8,  8,  8,  48, 32, 32,  36, 40, 48,  40, 36,  0,  24,  8,  8,   8,  8,  8,   8,  0,  0,  0,   182, 73,
      73, 65, 65, 0,  0,  0,  44, 18, 18, 18, 18, 0,  0,  0,  28, 34,  34, 34, 28,  0,  0,   0,  44,  18, 18,  28, 16, 16,  0,  0,  26, 36,  36,  28,
      4,  4,  0,  0,  44, 48, 32, 32, 32, 0,  0,  0,  28, 32, 24, 4,   56, 0,  0,   8,  28,  8,  8,   8,  8,   0,  0,  0,   36, 36, 36, 36,  26,  0,
      0,  0,  34, 34, 34, 20, 8,  0,  0,  0,  65, 65, 73, 85, 34, 0,   0,  0,  34,  20, 8,   20, 34,  0,  0,   0,  18, 18,  18, 14, 2,  28,  0,   0,
      60, 4,  8,  16, 60, 0,  12, 16, 16, 32, 16, 16, 12, 0,  8,  8,   8,  8,  8,   8,  8,   0,  48,  8,  8,   4,  8,  8,   48, 0,  0,  0,   48,  73,
      6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
  }

  uint8_t EmbeddedFont8x8::MinCharacter()
  {
    return 33;
  }

  uint8_t EmbeddedFont8x8::MaxCharacter()
  {
    return 127;
  }

  uint8_t EmbeddedFont8x8::CharacterCount()
  {
    return (MaxCharacter() - MinCharacter()) + 1;
  }

  PxSize2D EmbeddedFont8x8::CharacterSize()
  {
    return PxSize2D::Create(8, 8);
  }

  void EmbeddedFont8x8::ExtractCharacter(const uint8_t theCharacter, uint8_t* pDst, const int32_t dstLength)
  {
    if (theCharacter < MinCharacter() || theCharacter > MaxCharacter())
    {
      throw std::invalid_argument("unsupported character");
    }
    if (pDst == nullptr)
    {
      throw std::invalid_argument("pDst can not be null");
    }
    if (dstLength < 8)
    {
      throw std::invalid_argument("dstLength has to be >= 8");
    }

    const int srcIndex = (theCharacter - MinCharacter()) * 8;
    pDst[0] = g_monochromeFont[srcIndex];
    pDst[1] = g_monochromeFont[srcIndex + 1];
    pDst[2] = g_monochromeFont[srcIndex + 2];
    pDst[3] = g_monochromeFont[srcIndex + 3];
    pDst[4] = g_monochromeFont[srcIndex + 4];
    pDst[5] = g_monochromeFont[srcIndex + 5];
    pDst[6] = g_monochromeFont[srcIndex + 6];
    pDst[7] = g_monochromeFont[srcIndex + 7];
  }

  void EmbeddedFont8x8::ExtractCharacter(const uint8_t theCharacter, Bitmap& rDstBitmap, const PxPoint2 dstPos)
  {
    RawBitmapEx rawBitmap;
    Bitmap::ScopedDirectAccess directAccess(rDstBitmap, rawBitmap);
    ExtractCharacter(theCharacter, rawBitmap, dstPos);
  }


  void EmbeddedFont8x8::ExtractCharacter(const uint8_t theCharacter, RawBitmapEx& rDstBitmap, const PxPoint2 dstPos)
  {
    if (theCharacter < MinCharacter() || theCharacter > MaxCharacter())
    {
      throw std::invalid_argument("unsupported character");
    }
    if (!rDstBitmap.IsValid())
    {
      throw std::invalid_argument("dst bitmap was invalid");
    }

    if (dstPos.X.Value < 0 || dstPos.Y.Value < 0)
    {
      throw std::invalid_argument("dst coordinates can not be negative");
    }

    const auto dstPosX = static_cast<uint32_t>(dstPos.X.Value);
    const auto dstPosY = static_cast<uint32_t>(dstPos.Y.Value);

    const PxSize2D characterSize = CharacterSize();
    if ((dstPosX + characterSize.RawWidth()) > rDstBitmap.Width() || (dstPosY + characterSize.RawHeight()) > rDstBitmap.Height())
    {
      throw std::invalid_argument("dst coordinates would be outside the bitmap");
    }


    const auto dstStride = NumericCast<int32_t>(rDstBitmap.Stride());
    uint8_t* pDst = static_cast<uint8_t*>(rDstBitmap.Content()) + (dstStride * dstPosY);
    const uint8_t* const pDstEnd = pDst + (dstStride * characterSize.RawHeight());
    const int srcIndex = (theCharacter - MinCharacter()) * 8;
    const uint8_t* pSrc = g_monochromeFont.data() + srcIndex;

    switch (PixelFormatUtil::GetPixelFormatLayout(rDstBitmap.GetPixelFormat()))
    {
    case PixelFormatLayout::R8:
      pDst += dstPosX;
      while (pDst < pDstEnd)
      {
        pDst[0] = ((*pSrc) & 0x80) != 0 ? 255 : 0;
        pDst[1] = ((*pSrc) & 0x40) != 0 ? 255 : 0;
        pDst[2] = ((*pSrc) & 0x20) != 0 ? 255 : 0;
        pDst[3] = ((*pSrc) & 0x10) != 0 ? 255 : 0;
        pDst[4] = ((*pSrc) & 0x08) != 0 ? 255 : 0;
        pDst[5] = ((*pSrc) & 0x04) != 0 ? 255 : 0;
        pDst[6] = ((*pSrc) & 0x02) != 0 ? 255 : 0;
        pDst[7] = ((*pSrc) & 0x01) != 0 ? 255 : 0;
        ++pSrc;
        pDst += dstStride;
      }
      break;
    case PixelFormatLayout::R8G8B8:
    case PixelFormatLayout::B8G8R8:
      pDst += dstPosX * 3;
      while (pDst < pDstEnd)
      {
        pDst[0] = ((*pSrc) & 0x80) != 0 ? 255 : 0;
        pDst[1] = ((*pSrc) & 0x80) != 0 ? 255 : 0;
        pDst[2] = ((*pSrc) & 0x80) != 0 ? 255 : 0;
        pDst[3] = ((*pSrc) & 0x40) != 0 ? 255 : 0;
        pDst[4] = ((*pSrc) & 0x40) != 0 ? 255 : 0;
        pDst[5] = ((*pSrc) & 0x40) != 0 ? 255 : 0;
        pDst[6] = ((*pSrc) & 0x20) != 0 ? 255 : 0;
        pDst[7] = ((*pSrc) & 0x20) != 0 ? 255 : 0;
        pDst[8] = ((*pSrc) & 0x20) != 0 ? 255 : 0;
        pDst[9] = ((*pSrc) & 0x10) != 0 ? 255 : 0;
        pDst[10] = ((*pSrc) & 0x10) != 0 ? 255 : 0;
        pDst[11] = ((*pSrc) & 0x10) != 0 ? 255 : 0;
        pDst[12] = ((*pSrc) & 0x08) != 0 ? 255 : 0;
        pDst[13] = ((*pSrc) & 0x08) != 0 ? 255 : 0;
        pDst[14] = ((*pSrc) & 0x08) != 0 ? 255 : 0;
        pDst[15] = ((*pSrc) & 0x04) != 0 ? 255 : 0;
        pDst[16] = ((*pSrc) & 0x04) != 0 ? 255 : 0;
        pDst[17] = ((*pSrc) & 0x04) != 0 ? 255 : 0;
        pDst[18] = ((*pSrc) & 0x02) != 0 ? 255 : 0;
        pDst[19] = ((*pSrc) & 0x02) != 0 ? 255 : 0;
        pDst[20] = ((*pSrc) & 0x02) != 0 ? 255 : 0;
        pDst[21] = ((*pSrc) & 0x01) != 0 ? 255 : 0;
        pDst[22] = ((*pSrc) & 0x01) != 0 ? 255 : 0;
        pDst[23] = ((*pSrc) & 0x01) != 0 ? 255 : 0;
        ++pSrc;
        pDst += dstStride;
      }
      break;
    case PixelFormatLayout::R8G8B8A8:
    case PixelFormatLayout::B8G8R8A8:
      pDst += dstPosX * 4;
      while (pDst < pDstEnd)
      {
        pDst[0] = ((*pSrc) & 0x80) != 0 ? 255 : 0;
        pDst[1] = ((*pSrc) & 0x80) != 0 ? 255 : 0;
        pDst[2] = ((*pSrc) & 0x80) != 0 ? 255 : 0;
        pDst[3] = ((*pSrc) & 0x80) != 0 ? 255 : 0;

        pDst[4] = ((*pSrc) & 0x40) != 0 ? 255 : 0;
        pDst[5] = ((*pSrc) & 0x40) != 0 ? 255 : 0;
        pDst[6] = ((*pSrc) & 0x40) != 0 ? 255 : 0;
        pDst[7] = ((*pSrc) & 0x40) != 0 ? 255 : 0;

        pDst[8] = ((*pSrc) & 0x20) != 0 ? 255 : 0;
        pDst[9] = ((*pSrc) & 0x20) != 0 ? 255 : 0;
        pDst[10] = ((*pSrc) & 0x20) != 0 ? 255 : 0;
        pDst[11] = ((*pSrc) & 0x20) != 0 ? 255 : 0;

        pDst[12] = ((*pSrc) & 0x10) != 0 ? 255 : 0;
        pDst[13] = ((*pSrc) & 0x10) != 0 ? 255 : 0;
        pDst[14] = ((*pSrc) & 0x10) != 0 ? 255 : 0;
        pDst[15] = ((*pSrc) & 0x10) != 0 ? 255 : 0;

        pDst[16] = ((*pSrc) & 0x08) != 0 ? 255 : 0;
        pDst[17] = ((*pSrc) & 0x08) != 0 ? 255 : 0;
        pDst[18] = ((*pSrc) & 0x08) != 0 ? 255 : 0;
        pDst[19] = ((*pSrc) & 0x08) != 0 ? 255 : 0;

        pDst[20] = ((*pSrc) & 0x04) != 0 ? 255 : 0;
        pDst[21] = ((*pSrc) & 0x04) != 0 ? 255 : 0;
        pDst[22] = ((*pSrc) & 0x04) != 0 ? 255 : 0;
        pDst[23] = ((*pSrc) & 0x04) != 0 ? 255 : 0;

        pDst[24] = ((*pSrc) & 0x02) != 0 ? 255 : 0;
        pDst[25] = ((*pSrc) & 0x02) != 0 ? 255 : 0;
        pDst[26] = ((*pSrc) & 0x02) != 0 ? 255 : 0;
        pDst[27] = ((*pSrc) & 0x02) != 0 ? 255 : 0;

        pDst[28] = ((*pSrc) & 0x01) != 0 ? 255 : 0;
        pDst[29] = ((*pSrc) & 0x01) != 0 ? 255 : 0;
        pDst[30] = ((*pSrc) & 0x01) != 0 ? 255 : 0;
        pDst[31] = ((*pSrc) & 0x01) != 0 ? 255 : 0;
        ++pSrc;
        pDst += dstStride;
      }
      break;
    default:
      throw UnsupportedPixelFormatException("PixelFormat was not B8G8R8_UINT, R8G8B8_UINT or Alpha8 as expected", rDstBitmap.GetPixelFormat());
    }
  }


  void EmbeddedFont8x8::CreateFontBitmap(Bitmap& rDstBitmap, const PixelFormat dstPixelFormat, const PxSize2D padding,
                                         const RectangleSizeRestrictionFlag restrictionFlags)
  {
    CreateFontBitmap(rDstBitmap, dstPixelFormat, MinCharacter(), CharacterCount(), padding, restrictionFlags);
  }


  void EmbeddedFont8x8::CreateFontBitmap(Bitmap& rDstBitmap, const PixelFormat dstPixelFormat, const uint8_t startCharacter, const uint8_t length,
                                         const PxSize2D padding, const RectangleSizeRestrictionFlag restrictionFlags)
  {
    if (padding.RawWidth() < 0 || padding.RawHeight() < 0)
    {
      throw std::invalid_argument("padding can not be negative");
    }
    if (startCharacter < MinCharacter() || (startCharacter + length) > (MaxCharacter() + 1))
    {
      throw std::invalid_argument("character range is out of bounds");
    }
    if (length <= 0)
    {
      rDstBitmap.Reset(PxExtent2D(), dstPixelFormat);
      return;
    }

    const PxSize2D characterSize = CharacterSize();
    const PxSize1D charWidth = characterSize.Width() + padding.Width();
    const PxSize1D charHeight = characterSize.Height() + padding.Height();
    const PxSize2D bitmapSize = MathHelper::CalcOptimalSize(PxSize2D(charWidth, charHeight), PxSize1D::Create(length), restrictionFlags);
    rDstBitmap.Reset(TypeConverter::UncheckedTo<PxExtent2D>(bitmapSize), dstPixelFormat);

    const int32_t charsX = (bitmapSize.Width() / charWidth).RawValue();
    const int32_t numFullLines = length / charsX;
    const int32_t charsLeft = length % charsX;

    uint8_t character = startCharacter;
    auto dstY = PxSize1D::Create(0);

    RawBitmapEx rawBitmap;
    Bitmap::ScopedDirectAccess directAccess(rDstBitmap, rawBitmap);

    // Copy the full lines
    for (int j = 0; j < numFullLines; ++j)
    {
      auto dstX = PxSize1D::Create(0);
      for (int i = 0; i < charsX; ++i)
      {
        ExtractCharacter(character, rawBitmap, PxPoint2(dstX, dstY));
        dstX += charWidth;
        ++character;
      }
      dstY += charHeight;
    }
    // copy the leftover chars
    {
      auto dstX = PxSize1D::Create(0);
      for (int i = 0; i < charsLeft; ++i)
      {
        ExtractCharacter(character, rawBitmap, PxPoint2(dstX, dstY));
        dstX += charWidth;
        ++character;
      }
    }
  }
}
