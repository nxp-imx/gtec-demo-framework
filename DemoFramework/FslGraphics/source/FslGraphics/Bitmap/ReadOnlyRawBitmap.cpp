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

#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslGraphics/Bitmap/ReadOnlyRawBitmap.hpp>
#include <FslGraphics/Bitmap/ReadOnlyRawBitmapArea.hpp>
#include <stdexcept>

namespace Fsl
{
  ReadOnlyRawBitmap ReadOnlyRawBitmap::Create(ReadOnlySpan<uint8_t> span, const PxExtent2D extentPx, const PixelFormat pixelFormat,
                                              const BitmapOrigin origin)
  {
    const auto stride = PixelFormatUtil::CalcMinimumStride(extentPx.Width, pixelFormat);
    if ((extentPx.Height.Value * stride) > span.size())
    {
      throw std::invalid_argument("span is not large enough to contain a bitmap of the given dimensions");
    }
    return {span, TypeConverter::To<PxSize2D>(extentPx), pixelFormat, stride, origin};
  }


  ReadOnlyRawBitmap ReadOnlyRawBitmap::Create(ReadOnlySpan<uint8_t> span, const PxExtent2D extentPx, const PixelFormat pixelFormat,
                                              const uint32_t stride, const BitmapOrigin origin)
  {
    if (stride < PixelFormatUtil::CalcMinimumStride(extentPx.Width, pixelFormat))
    {
      throw std::invalid_argument("stride is smaller than the width allows");
    }
    if ((extentPx.Height.Value * stride) > span.size())
    {
      throw std::invalid_argument("span is not large enough to contain a bitmap of the given dimensions");
    }
    return {span, TypeConverter::To<PxSize2D>(extentPx), pixelFormat, stride, origin};
  }


  ReadOnlyRawBitmap ReadOnlyRawBitmap::Create(const ReadOnlyRawBitmapArea& rawBitmapArea, const PixelFormat pixelFormat, const BitmapOrigin origin)
  {
    return Create(ReadOnlySpan<uint8_t>(rawBitmapArea.ContentPtr, rawBitmapArea.ByteSize), rawBitmapArea.SizePx, pixelFormat, rawBitmapArea.Stride,
                  origin);
  }


  ReadOnlyRawBitmap ReadOnlyRawBitmap::UncheckedCreate(ReadOnlySpan<uint8_t> span, const PxExtent2D extentPx, const PixelFormat pixelFormat,
                                                       const BitmapOrigin origin) noexcept
  {
    return UncheckedCreate(span, TypeConverter::UncheckedTo<PxSize2D>(extentPx), pixelFormat, origin);
  }


  ReadOnlyRawBitmap ReadOnlyRawBitmap::UncheckedCreate(ReadOnlySpan<uint8_t> span, const PxExtent2D extentPx, const PixelFormat pixelFormat,
                                                       const uint32_t stride, const BitmapOrigin origin) noexcept
  {
    return UncheckedCreate(span, TypeConverter::UncheckedTo<PxSize2D>(extentPx), pixelFormat, stride, origin);
  }


  ReadOnlyRawBitmap ReadOnlyRawBitmap::UncheckedCreate(const void* const pContent, const uint32_t contentByteSize, const PxExtent2D extentPx,
                                                       const PixelFormat pixelFormat, const BitmapOrigin origin) noexcept
  {
    return UncheckedCreate(pContent, contentByteSize, TypeConverter::UncheckedTo<PxSize2D>(extentPx), pixelFormat, origin);
  }


  ReadOnlyRawBitmap ReadOnlyRawBitmap::UncheckedCreate(const void* const pContent, const uint32_t contentByteSize, const PxExtent2D extentPx,
                                                       const PixelFormat pixelFormat, const uint32_t stride, const BitmapOrigin origin) noexcept
  {
    return UncheckedCreate(pContent, contentByteSize, TypeConverter::UncheckedTo<PxSize2D>(extentPx), pixelFormat, stride, origin);
  }
}
