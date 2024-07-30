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

#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Math/TypeConverter_Double.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Bitmap/Converter/RawBitmapConverterFunctions.hpp>
#include <FslGraphics/Bitmap/RawBitmapEx.hpp>
#include <FslGraphics/Bitmap/ReadOnlyRawBitmap.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>

namespace Fsl::RawBitmapConverterFunctions
{
  namespace
  {
    constexpr bool IsSingleChannel8Bit(const PixelFormat pixelFormat) noexcept
    {
      switch (pixelFormat)
      {
      case PixelFormat::R8_UNORM:
      case PixelFormat::EX_LUMINANCE8_UNORM:
      case PixelFormat::EX_ALPHA8_UNORM:
        return true;
      default:
        return false;
      }
    }
  }


  void R8G8B8ToGrayscaleLuminanceNTSC(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap)
  {
    if (srcBitmap.GetPixelFormat() != PixelFormat::R8G8B8_UNORM)
    {
      throw UnsupportedPixelFormatException("R8G8B8ToGrayscaleLuminanceNTSC only supports R8G8B8_UNORM", srcBitmap.GetPixelFormat());
    }
    if (!IsSingleChannel8Bit(dstBitmap.GetPixelFormat()))
    {
      throw UnsupportedPixelFormatException("R8G8B8ToGrayscaleLuminanceNTSC only supports R8_UNORM or EX_LUMINANCE8_UNORM",
                                            srcBitmap.GetPixelFormat());
    }
    if (dstBitmap.GetOrigin() != srcBitmap.GetOrigin())
    {
      throw std::invalid_argument("R8G8B8ToGrayscaleLuminanceNTSC requires that the origin matches");
    }
    if (srcBitmap.GetExtent() != dstBitmap.GetExtent())
    {
      throw std::invalid_argument("R8G8B8ToGrayscaleLuminanceNTSC requires that the extent of the src and dst bitmap is the same");
    }
    assert(PixelFormatUtil::GetBytesPerPixel(srcBitmap.GetPixelFormat()) == 3);
    assert(PixelFormatUtil::GetBytesPerPixel(dstBitmap.GetPixelFormat()) == 1);

    const auto* pSrc = static_cast<const uint8_t*>(srcBitmap.Content());
    const uint8_t* const pSrcEnd = pSrc + srcBitmap.GetByteSize();
    auto* pDst = static_cast<uint8_t*>(dstBitmap.Content());
    uint8_t* const pDstEnd = pDst + dstBitmap.GetByteSize();
    const uint32_t srcStride = srcBitmap.Stride();
    const uint32_t dstStride = dstBitmap.Stride();

    // The buffers can only overlap if pSrc == pDst && srcStride >= dstStride, if that is not the case they can not overlap
    if ((pSrc != pDst || (pSrc == pDst && srcStride < dstStride)) && pSrc < pDstEnd && pSrcEnd > pDst)
    {
      throw UsageErrorException("R8G8B8ToGrayscaleLuminanceNTSC does not support overlapping buffers");
    }

    // Generic slow R8G8B8 to single channel luminance (NTSC) conversion.
    const uint32_t width = srcBitmap.RawUnsignedWidth();
    while (pDst < pDstEnd)
    {
      for (uint32_t x = 0; x < width; ++x)
      {
        const auto r = static_cast<double>(pSrc[(x * 3)]);
        const auto g = static_cast<double>(pSrc[(x * 3) + 1]);
        const auto b = static_cast<double>(pSrc[(x * 3) + 2]);

        const auto gray = TypeConverter::UncheckedChangeTo<int32_t>((0.299 * r) + (0.587 * g) + (0.114 * b));

        pDst[x] = UncheckedNumericCast<uint8_t>(MathHelper::Clamp(gray, 0, 255));
      }
      pSrc += srcStride;
      pDst += dstStride;
    }
  }
}
