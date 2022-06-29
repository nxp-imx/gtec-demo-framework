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

#include <FslBase/Log/Log3Core.hpp>
#include <FslGraphics/Bitmap/Converter/RawBitmapConverter.hpp>
#include <FslGraphics/Bitmap/RawBitmapEx.hpp>
#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
  }


  bool RawBitmapConverter::TryConvert(RawBitmapEx& rDstBitmap, const PixelFormat desiredPixelFormat)
  {
    if (!rDstBitmap.IsValid())
    {
      FSLLOG3_DEBUG_WARNING("TryConvert called with invalid bitmap");
      return false;
    }

    const PixelFormat srcPixelFormat = rDstBitmap.GetPixelFormat();

    if (srcPixelFormat == desiredPixelFormat)
    {
      return true;
    }

    if (rDstBitmap.Width() == 0 || rDstBitmap.Height() == 0)
    {
      rDstBitmap.SetPixelFormat(desiredPixelFormat);
      return true;
    }

    const PixelFormatLayout srcPixelFormatLayout = PixelFormatUtil::GetPixelFormatLayout(srcPixelFormat);
    const PixelFormatLayout desiredPixelFormatLayout = PixelFormatUtil::GetPixelFormatLayout(desiredPixelFormat);
    if (srcPixelFormatLayout == desiredPixelFormatLayout)
    {
      // The pixel format layout does not need to be changed, so just update the pixel format
      rDstBitmap.SetPixelFormat(desiredPixelFormat);
      return true;
    }

    // We only support a few hard-coded conversions
    if ((srcPixelFormatLayout == PixelFormatLayout::R8G8B8 && desiredPixelFormatLayout == PixelFormatLayout::B8G8R8) ||
        (srcPixelFormatLayout == PixelFormatLayout::B8G8R8 && desiredPixelFormatLayout == PixelFormatLayout::R8G8B8))
    {
      // 012    012
      // RGB -> BGR
      // BGR -> RGB
      RawBitmapUtil::Swizzle24From012To210(rDstBitmap);
      rDstBitmap.SetPixelFormat(desiredPixelFormat);
      return true;
    }
    if ((srcPixelFormatLayout == PixelFormatLayout::R8G8B8A8 && desiredPixelFormatLayout == PixelFormatLayout::B8G8R8A8) ||
        (srcPixelFormatLayout == PixelFormatLayout::B8G8R8A8 && desiredPixelFormatLayout == PixelFormatLayout::R8G8B8A8))
    {
      // 0123    0123
      // RGBA -> BGRA
      // BGRA -> RGBA
      RawBitmapUtil::Swizzle32(rDstBitmap, 2, 1, 0, 3);
      rDstBitmap.SetPixelFormat(desiredPixelFormat);
      return true;
    }
    return false;
  }


  bool RawBitmapConverter::TryConvert(RawBitmapEx& rDstBitmap, const RawBitmap& srcBitmap)
  {
    if (!rDstBitmap.IsValid() || !srcBitmap.IsValid())
    {
      FSLLOG3_DEBUG_WARNING("TryConvert called with invalid bitmap");
      return false;
    }
    // Dimensions must be the same and they must have a positive stride
    if (rDstBitmap.Width() != srcBitmap.Width() || rDstBitmap.Height() != srcBitmap.Height())
    {
      return false;
    }

    const PixelFormat srcPixelFormat = srcBitmap.GetPixelFormat();
    const PixelFormat dstPixelFormat = rDstBitmap.GetPixelFormat();
    if (srcPixelFormat == dstPixelFormat)
    {
      return true;
    }


    const auto* const pSrc = static_cast<const uint8_t*>(srcBitmap.Content());
    const uint8_t* const pSrcEnd = pSrc + srcBitmap.GetByteSize();
    auto* const pDst = static_cast<uint8_t*>(rDstBitmap.Content());
    uint8_t* const pDstEnd = pDst + rDstBitmap.GetByteSize();

    // The buffers can not overlap
    if (!(pSrc >= pDstEnd || pSrcEnd <= pDst))
    {
      FSLLOG3_DEBUG_WARNING("TryConvert called with overlapping buffers, this is not supported");
      return false;
    }

    const PixelFormatLayout srcPixelFormatLayout = PixelFormatUtil::GetPixelFormatLayout(srcPixelFormat);
    const PixelFormatLayout dstPixelFormatLayout = PixelFormatUtil::GetPixelFormatLayout(dstPixelFormat);
    if (srcPixelFormatLayout == dstPixelFormatLayout)
    {
      // The pixel format layout does not need to be changed, so just do a copy
      RawBitmapUtil::MemoryCopy(rDstBitmap, srcBitmap);
      return true;
    }


    // We only support a few hard-coded conversions
    if ((srcPixelFormatLayout == PixelFormatLayout::R8G8B8 && dstPixelFormatLayout == PixelFormatLayout::B8G8R8) ||
        (srcPixelFormatLayout == PixelFormatLayout::B8G8R8 && dstPixelFormatLayout == PixelFormatLayout::R8G8B8))
    {
      // 012    012
      // RGB -> BGR
      // BGR -> RGB
      RawBitmapUtil::Swizzle24From012To210(rDstBitmap, srcBitmap);
      return true;
    }
    if ((srcPixelFormatLayout == PixelFormatLayout::R8G8B8A8 && dstPixelFormatLayout == PixelFormatLayout::B8G8R8A8) ||
        (srcPixelFormatLayout == PixelFormatLayout::B8G8R8A8 && dstPixelFormatLayout == PixelFormatLayout::R8G8B8A8))
    {
      // 0123    0123
      // RGBA -> BGRA
      // BGRA -> RGBA
      RawBitmapUtil::Swizzle32(rDstBitmap, srcBitmap, 2, 1, 0, 3);
      return true;
    }
    if ((srcPixelFormatLayout == PixelFormatLayout::R8G8B8A8 && dstPixelFormatLayout == PixelFormatLayout::B8G8R8) ||
        (srcPixelFormatLayout == PixelFormatLayout::B8G8R8A8 && dstPixelFormatLayout == PixelFormatLayout::R8G8B8))
    {
      // 0123    012
      // RGBA -> BGR
      // BGRA -> RGB
      RawBitmapUtil::Swizzle32To24(rDstBitmap, srcBitmap, 2, 1, 0);
      return true;
    }
    if ((srcPixelFormatLayout == PixelFormatLayout::R8G8B8A8 && dstPixelFormatLayout == PixelFormatLayout::R8G8B8) ||
        (srcPixelFormatLayout == PixelFormatLayout::B8G8R8A8 && dstPixelFormatLayout == PixelFormatLayout::B8G8R8))
    {
      // 0123    012
      // RGBA -> RGB
      // BGRA -> BGR
      RawBitmapUtil::Swizzle32To24(rDstBitmap, srcBitmap, 0, 1, 2);
      return true;
    }
    if ((srcPixelFormatLayout == PixelFormatLayout::R8G8B8 && dstPixelFormatLayout == PixelFormatLayout::B8G8R8A8) ||
        (srcPixelFormatLayout == PixelFormatLayout::B8G8R8 && dstPixelFormatLayout == PixelFormatLayout::R8G8B8A8))
    {
      // 012    0123
      // RGB -> BGRA
      // BGR -> RGBA
      RawBitmapUtil::Swizzle24To32(rDstBitmap, srcBitmap, 2, 1, 0, 3, 0xFF);
      return true;
    }
    if ((srcPixelFormatLayout == PixelFormatLayout::R8G8B8 && dstPixelFormatLayout == PixelFormatLayout::R8G8B8A8) ||
        (srcPixelFormatLayout == PixelFormatLayout::B8G8R8 && dstPixelFormatLayout == PixelFormatLayout::B8G8R8A8))
    {
      // 012    0123
      // RGB -> RGBA
      // BGR -> BGRA
      RawBitmapUtil::Swizzle24To32(rDstBitmap, srcBitmap, 0, 1, 2, 3, 0xFF);
      return true;
    }
    if (dstPixelFormat == PixelFormat::EX_ALPHA8_UNORM || dstPixelFormat == PixelFormat::EX_LUMINANCE8_UNORM)
    {
      if (srcPixelFormatLayout == PixelFormatLayout::R8G8B8 || srcPixelFormatLayout == PixelFormatLayout::B8G8R8)
      {
        RawBitmapUtil::Average24To8(rDstBitmap, srcBitmap, 0, 1, 2);
        return true;
      }
      if (srcPixelFormatLayout == PixelFormatLayout::R8G8B8A8 || srcPixelFormatLayout == PixelFormatLayout::B8G8R8A8)
      {
        RawBitmapUtil::Average32To8(rDstBitmap, srcBitmap, 0, 1, 2);
        return true;
      }
    }
    else if ((srcPixelFormat == PixelFormat::EX_ALPHA8_UNORM || srcPixelFormat == PixelFormat::EX_LUMINANCE8_UNORM) &&
             (dstPixelFormatLayout == PixelFormatLayout::B8G8R8 || dstPixelFormatLayout == PixelFormatLayout::R8G8B8 ||
              dstPixelFormatLayout == PixelFormatLayout::B8G8R8A8 || dstPixelFormatLayout == PixelFormatLayout::R8G8B8A8))
    {
      RawBitmapUtil::Expand1ByteToNBytes(rDstBitmap, srcBitmap);
      return true;
    }
    return false;
  }
}
