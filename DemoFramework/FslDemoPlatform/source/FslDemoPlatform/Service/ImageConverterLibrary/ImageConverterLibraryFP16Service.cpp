#ifdef FSL_FEATURE_HALF
/****************************************************************************************************************************************************
* Copyright 2018 NXP
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

#include <FslDemoPlatform/Service/ImageConverterLibrary/ImageConverterLibraryFP16Service.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <half.hpp>

namespace Fsl
{
  namespace
  {
    uint32_t ConvertR32G32B32ToR16G16B16(RawBitmapEx& rawBitmapEx)
    {
      static_assert(sizeof(float) == 4, "We expect a float to be four bytes");
      const uint32_t srcStride = (rawBitmapEx.Stride() / 4);
      const float* pSrc = static_cast<const float*>(rawBitmapEx.Content());
      const float*const pSrcEnd = pSrc + (srcStride * rawBitmapEx.Height());
      const uint32_t srcWidth = rawBitmapEx.Width() * 3;

      uint16_t* pDst = static_cast<uint16_t*>(rawBitmapEx.Content());
      const uint32_t realDstStride = PixelFormatUtil::CalcMinimumStride(rawBitmapEx.Width(), 2 * 3);
      const uint32_t dstStride = realDstStride / 2;

      while (pSrc < pSrcEnd)
      {
        for (std::uint32_t x = 0; x < srcWidth; x += 3)
        {
          const auto v0 = half_float::detail::float2half<(std::float_round_style)(HALF_ROUND_STYLE)>(pSrc[x + 0]);
          const auto v1 = half_float::detail::float2half<(std::float_round_style)(HALF_ROUND_STYLE)>(pSrc[x + 1]);
          const auto v2 = half_float::detail::float2half<(std::float_round_style)(HALF_ROUND_STYLE)>(pSrc[x + 2]);
          pDst[x + 0] = static_cast<uint16_t>(v0 & 0xFFFF);
          pDst[x + 1] = static_cast<uint16_t>(v1 & 0xFFFF);
          pDst[x + 2] = static_cast<uint16_t>(v2 & 0xFFFF);
        }
        pSrc += srcStride;
        pDst += dstStride;
      }
      return realDstStride;
    }

    uint32_t ConvertR32G32B32ToR16G16B16A16(RawBitmapEx& rawBitmapEx)
    {
      static_assert(sizeof(float) == 4, "We expect a float to be four bytes");
      const uint32_t srcStride = (rawBitmapEx.Stride() / 4);
      const float* pSrc = static_cast<const float*>(rawBitmapEx.Content());
      const float*const pSrcEnd = pSrc + (srcStride * rawBitmapEx.Height());
      const uint32_t srcWidth = rawBitmapEx.Width();

      uint16_t* pDst = static_cast<uint16_t*>(rawBitmapEx.Content());
      const uint32_t realDstStride = PixelFormatUtil::CalcMinimumStride(rawBitmapEx.Width(), 2 * 4);
      const uint32_t dstStride = realDstStride / 2;

      while (pSrc < pSrcEnd)
      {
        for (std::uint32_t x = 0; x < srcWidth; ++x)
        {
          const auto v0 = half_float::detail::float2half<(std::float_round_style)(HALF_ROUND_STYLE)>(pSrc[(x * 3) + 0]);
          const auto v1 = half_float::detail::float2half<(std::float_round_style)(HALF_ROUND_STYLE)>(pSrc[(x * 3) + 1]);
          const auto v2 = half_float::detail::float2half<(std::float_round_style)(HALF_ROUND_STYLE)>(pSrc[(x * 3) + 2]);
          pDst[(x * 4) + 0] = static_cast<uint16_t>(v0 & 0xFFFF);
          pDst[(x * 4) + 1] = static_cast<uint16_t>(v1 & 0xFFFF);
          pDst[(x * 4) + 2] = static_cast<uint16_t>(v2 & 0xFFFF);
          pDst[(x * 4) + 3] = 0xFFFF;
        }
        pSrc += srcStride;
        pDst += dstStride;
      }
      return realDstStride;
    }


    bool TryConvertRGB32ToRGB16(Bitmap& rDstBitmap, const Bitmap& srcBitmap, const PixelFormat desiredPixelFormat)
    {
      // We only support in place conversion for now
      if (&rDstBitmap != &srcBitmap)
        return false;

      if (desiredPixelFormat == PixelFormat::Undefined || rDstBitmap.GetPixelFormat() == desiredPixelFormat)
        return true;

      if (srcBitmap.GetPixelFormat() != PixelFormat::R32G32B32_SFLOAT || desiredPixelFormat != PixelFormat::R16G16B16_SFLOAT)
        return false;

      uint32_t dstStride;
      {
        RawBitmapEx rawBitmapEx;
        Bitmap::ScopedDirectAccess directAccess(rDstBitmap, rawBitmapEx);

        if ((rawBitmapEx.Stride() % 4) != 0)
          return false;

        dstStride = ConvertR32G32B32ToR16G16B16(rawBitmapEx);
      }

      // Resize the bitmap as needed
      rDstBitmap.Reset(rDstBitmap.GetExtent(), desiredPixelFormat, dstStride, rDstBitmap.GetOrigin(), BitmapClearMethod::DontModify);
      return true;
    }

    bool TryConvertRGB32ToRGBA16(Bitmap& rDstBitmap, const Bitmap& srcBitmap, const PixelFormat desiredPixelFormat)
    {
      // We only support in place conversion for now
      if (&rDstBitmap != &srcBitmap)
        return false;

      if (desiredPixelFormat == PixelFormat::Undefined || rDstBitmap.GetPixelFormat() == desiredPixelFormat)
        return true;

      if (srcBitmap.GetPixelFormat() != PixelFormat::R32G32B32_SFLOAT || desiredPixelFormat != PixelFormat::R16G16B16A16_SFLOAT)
        return false;

      uint32_t dstStride;
      {
        RawBitmapEx rawBitmapEx;
        Bitmap::ScopedDirectAccess directAccess(rDstBitmap, rawBitmapEx);

        if ((rawBitmapEx.Stride() % 4) != 0)
          return false;

        dstStride = ConvertR32G32B32ToR16G16B16A16(rawBitmapEx);
      }

      // Resize the bitmap as needed
      rDstBitmap.Reset(rDstBitmap.GetExtent(), desiredPixelFormat, dstStride, rDstBitmap.GetOrigin(), BitmapClearMethod::DontModify);
      return true;
    }

  }


  ImageConverterLibraryFP16Service::ImageConverterLibraryFP16Service(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
  {
  }


  ImageConverterLibraryFP16Service::~ImageConverterLibraryFP16Service()
  {
  }


  bool ImageConverterLibraryFP16Service::TryConvert(Bitmap& rDstBitmap, const Bitmap& srcBitmap, const PixelFormat desiredPixelFormat,
                                                    const BitmapOrigin desiredOriginHint)
  {
    if (TryConvertRGB32ToRGB16(rDstBitmap, srcBitmap, desiredPixelFormat))
      return true;
    else if (TryConvertRGB32ToRGBA16(rDstBitmap, srcBitmap, desiredPixelFormat))
      return true;
    return false;
  }


  bool ImageConverterLibraryFP16Service::TryConvert(Texture& rDstTexture, const Texture& srcTexture, const PixelFormat desiredPixelFormat,
                                                    const BitmapOrigin desiredOriginHint)
  {
    return false;
  }
}
#endif
