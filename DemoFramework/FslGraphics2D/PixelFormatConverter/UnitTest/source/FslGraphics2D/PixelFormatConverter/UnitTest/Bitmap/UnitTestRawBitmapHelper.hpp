#ifndef FSLGRAPHICS2D_PIXELFORMATCONVERTER_UNITTEST_BITMAP_FSLGRAPHICS2D_PIXELFORMATCONVERTER_UNITTEST_UNITTESTRAWBITMAPHELPER_HPP
#define FSLGRAPHICS2D_PIXELFORMATCONVERTER_UNITTEST_BITMAP_FSLGRAPHICS2D_PIXELFORMATCONVERTER_UNITTEST_UNITTESTRAWBITMAPHELPER_HPP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslGraphics/Bitmap/TightBitmap.hpp>
#include <FslGraphics/ColorChannelConverter.hpp>
#include <FslGraphics/ColorSpaceConversion.hpp>
#include <FslGraphics2D/PixelFormatConverter/Bitmap/RawBitmapConverterFunctions.hpp>
#include <half.hpp>
#include <array>
#include "gtest/gtest.h"

namespace Fsl::UnitTestRawBitmapHelper
{
  template <typename TSrcType, typename TDstType, typename TTransformFunc>
  inline void CheckIfTransformMatch(const ReadOnlySpan<TSrcType> untransformedSpan, const ReadOnlySpan<TDstType> transformedSpan,
                                    TTransformFunc fnTransform)
  {
    ASSERT_EQ(untransformedSpan.size(), transformedSpan.size());

    for (std::size_t i = 0; i < untransformedSpan.size(); ++i)
    {
      ASSERT_EQ(fnTransform(untransformedSpan[i]), transformedSpan[i]);
    }
  }


  template <typename TSrcType, typename TDstType, typename TTransformFunc0, typename TTransformFunc1>
  inline void CheckIfTransformMatch3And1(const ReadOnlySpan<TSrcType> untransformedSpan, const ReadOnlySpan<TDstType> transformedSpan,
                                         TTransformFunc0 fnTransform0, TTransformFunc1 fnTransform1)
  {
    ASSERT_EQ((untransformedSpan.size() % 4), 0u);
    ASSERT_EQ(untransformedSpan.size(), transformedSpan.size());

    for (std::size_t i = 0; i < untransformedSpan.size(); i += 4)
    {
      ASSERT_FLOAT_EQ(fnTransform0(untransformedSpan[i]), transformedSpan[i]);
      ASSERT_FLOAT_EQ(fnTransform0(untransformedSpan[i + 1]), transformedSpan[i + 1]);
      ASSERT_FLOAT_EQ(fnTransform0(untransformedSpan[i + 2]), transformedSpan[i + 2]);
      ASSERT_FLOAT_EQ(fnTransform1(untransformedSpan[i + 3]), transformedSpan[i + 3]);
    }
  }

  template <typename TSrcType, typename TTransformFunc>
  inline void CheckIfTransformMatch(const ReadOnlySpan<TSrcType> untransformedSpan, const ReadOnlySpan<float> transformedSpan,
                                    TTransformFunc fnTransform)
  {
    ASSERT_EQ(untransformedSpan.size(), transformedSpan.size());

    for (std::size_t i = 0; i < untransformedSpan.size(); ++i)
    {
      ASSERT_FLOAT_EQ(fnTransform(untransformedSpan[i]), transformedSpan[i]);
    }
  }

  template <typename TSrcType, typename TTransformFunc0, typename TTransformFunc1>
  inline void CheckIfTransformMatch3And1(const ReadOnlySpan<TSrcType> untransformedSpan, const ReadOnlySpan<float> transformedSpan,
                                         TTransformFunc0 fnTransform0, TTransformFunc1 fnTransform1)
  {
    ASSERT_EQ((untransformedSpan.size() % 4), 0);
    ASSERT_EQ(untransformedSpan.size(), transformedSpan.size());

    for (std::size_t i = 0; i < untransformedSpan.size(); i += 4)
    {
      ASSERT_FLOAT_EQ(fnTransform0(untransformedSpan[i]), transformedSpan[i]);
      ASSERT_FLOAT_EQ(fnTransform0(untransformedSpan[i + 1]), transformedSpan[i + 1]);
      ASSERT_FLOAT_EQ(fnTransform0(untransformedSpan[i + 2]), transformedSpan[i + 2]);
      ASSERT_FLOAT_EQ(fnTransform1(untransformedSpan[i + 3]), transformedSpan[i + 3]);
    }
  }


  template <typename TSrcType, typename TDstType, typename TTransformFunc>
  inline void CheckIfTransform3To4Match(const ReadOnlySpan<TSrcType> untransformedSpan, const ReadOnlySpan<TDstType> transformedSpan,
                                        const uint32_t pixelWidth, TTransformFunc fnTransform, const TDstType addedChannelValue)
  {
    if (untransformedSpan.empty())
    {
      ASSERT_TRUE(transformedSpan.empty());
      return;
    }
    const uint32_t minSrcStride = pixelWidth * 3u;
    const uint32_t minDstStride = pixelWidth * 4u;

    ASSERT_EQ(untransformedSpan.size() % minSrcStride, 0u);
    ASSERT_EQ(transformedSpan.size() % minDstStride, 0u);
    // The height is expected to be the same for each buffer
    ASSERT_EQ((untransformedSpan.size() / minSrcStride), (transformedSpan.size() / minDstStride));

    std::size_t srcOffsetY = 0u;
    std::size_t dstOffsetY = 0u;
    while (srcOffsetY < untransformedSpan.size())
    {
      for (std::size_t srcOffsetX = 0u, dstOffsetX = 0u; srcOffsetX < minSrcStride; srcOffsetX += 3u, dstOffsetX += 4u)
      {
        ASSERT_EQ(fnTransform(untransformedSpan[srcOffsetY + srcOffsetX]), transformedSpan[dstOffsetY + dstOffsetX]);
        ASSERT_EQ(fnTransform(untransformedSpan[srcOffsetY + srcOffsetX + 1u]), transformedSpan[dstOffsetY + dstOffsetX + 1u]);
        ASSERT_EQ(fnTransform(untransformedSpan[srcOffsetY + srcOffsetX + 2u]), transformedSpan[dstOffsetY + dstOffsetX + 2u]);
        ASSERT_EQ(addedChannelValue, transformedSpan[dstOffsetY + dstOffsetX + 3u]);
      }
      srcOffsetY += minSrcStride;
      dstOffsetY += minDstStride;
    }
  }

  template <typename TSrcType, typename TTransformFunc>
  inline void CheckIfTransform3To4Match(const ReadOnlySpan<TSrcType> untransformedSpan, const ReadOnlySpan<float> transformedSpan,
                                        const uint32_t pixelWidth, TTransformFunc fnTransform, const float addedChannelValue)
  {
    if (untransformedSpan.empty())
    {
      ASSERT_TRUE(transformedSpan.empty());
      return;
    }
    const uint32_t minSrcStride = pixelWidth * 3u;
    const uint32_t minDstStride = pixelWidth * 4u;

    ASSERT_EQ(untransformedSpan.size() % minSrcStride, 0u);
    ASSERT_EQ(transformedSpan.size() % minDstStride, 0u);
    // The height is expected to be the same for each buffer
    ASSERT_EQ((untransformedSpan.size() / minSrcStride), (transformedSpan.size() / minDstStride));

    std::size_t srcOffsetY = 0u;
    std::size_t dstOffsetY = 0u;
    while (srcOffsetY < untransformedSpan.size())
    {
      for (std::size_t srcOffsetX = 0u, dstOffsetX = 0u; srcOffsetX < minSrcStride; srcOffsetX += 3u, dstOffsetX += 4u)
      {
        const float untransformedC0 = fnTransform(untransformedSpan[srcOffsetY + srcOffsetX]);
        const float untransformedC1 = fnTransform(untransformedSpan[srcOffsetY + srcOffsetX + 1u]);
        const float untransformedC2 = fnTransform(untransformedSpan[srcOffsetY + srcOffsetX + 2u]);
        const float transformedC0 = transformedSpan[dstOffsetY + dstOffsetX];
        const float transformedC1 = transformedSpan[dstOffsetY + dstOffsetX + 1u];
        const float transformedC2 = transformedSpan[dstOffsetY + dstOffsetX + 2u];
        const float transformedC3 = transformedSpan[dstOffsetY + dstOffsetX + 3u];
        ASSERT_FLOAT_EQ(untransformedC0, transformedC0);
        ASSERT_FLOAT_EQ(untransformedC1, transformedC1);
        ASSERT_FLOAT_EQ(untransformedC2, transformedC2);
        ASSERT_FLOAT_EQ(addedChannelValue, transformedC3);
      }
      srcOffsetY += minSrcStride;
      dstOffsetY += minDstStride;
    }
  }


  inline constexpr ReadOnlySpan<uint16_t> ReinterpretSpanToUInt16(ReadOnlySpan<uint8_t> span)
  {
    if ((span.size() % 2) != 0u)
    {
      throw UsageErrorException("Span does not appear to be compatible");
    }
    return ReadOnlySpan<uint16_t>(static_cast<const uint16_t*>(static_cast<const void*>(span.data())), span.size() / 2u);
  }

  inline constexpr ReadOnlySpan<float> ReinterpretSpanToFloat(ReadOnlySpan<uint8_t> span)
  {
    if ((span.size() % 4) != 0u)
    {
      throw UsageErrorException("Span does not appear to be compatible");
    }
    return ReadOnlySpan<float>(static_cast<const float*>(static_cast<const void*>(span.data())), span.size() / 4u);
  }

  inline uint16_t ConvertLinearFloatToLinearFp16(const float value)
  {
    return half_float::detail::float2half<std::float_round_style::round_to_nearest>(value);
  }

  inline float ConvertLinearFP16ToLinearFloat(const uint16_t value)
  {
    return half_float::detail::half2float<float>(value);
  }


  inline float ConvertLinearUInt8ToLinearFloat(const uint8_t linearValue)
  {
    return ColorSpaceConversion::ConvertLinearUInt8ToLinearFloat(linearValue);
  }

  inline uint8_t ConvertLinearUInt16ToLinearUInt8(const uint16_t linearValue)
  {
    return ColorSpaceConversion::ConvertLinearUInt16ToLinearUInt8(linearValue);
  }

  inline uint8_t ConvertLinearFloatToLinearUInt8(const float linearValue)
  {
    return ColorSpaceConversion::ConvertLinearFloatToLinearUInt8(linearValue);
  }

  inline uint16_t ConvertLinearUInt8ToLinearUInt16(const uint8_t linearValue)
  {
    return ColorSpaceConversion::ConvertLinearUInt8ToLinearUInt16(linearValue);
  }

  inline uint16_t ConvertLinearUInt8ToLinearFp16(const uint8_t linearValue)
  {
    return ConvertLinearFloatToLinearFp16(ColorSpaceConversion::ConvertLinearUInt8ToLinearFloat(linearValue));
  }

  inline uint16_t ConvertSrg8ToLinearUInt16(const uint8_t value)
  {
    return ColorSpaceConversion::ConvertSRGBToLinearUInt16(value);
  }

  inline uint16_t ConvertSrgb8ToLinearFp16(const uint8_t value)
  {
    return ConvertLinearFloatToLinearFp16(ColorSpaceConversion::ConvertSRGBToLinearFloat(value));
  }

  inline float ConvertSRGB8ToLinearFloat(const uint8_t value)
  {
    return ColorSpaceConversion::ConvertSRGBToLinearFloat(value);
  }


  inline uint8_t ConvertLinearUInt16ToSrg8(const uint16_t value)
  {
    return ColorSpaceConversion::ConvertLinearToSRGBUInt8(value);
  }

  inline uint8_t ConvertLinearFP16ToSrg8(const uint16_t value)
  {
    return ColorSpaceConversion::ConvertLinearToSRGBUInt8(ConvertLinearFP16ToLinearFloat(value));
  }


  inline uint8_t ConvertLinearFP16ToLinearUInt8(const uint16_t value)
  {
    return ColorSpaceConversion::ConvertLinearFloatToLinearUInt8(ConvertLinearFP16ToLinearFloat(value));
  }


  inline uint8_t ConvertLinearFloatToSrg8(const float value)
  {
    return ColorSpaceConversion::ConvertLinearToSRGBUInt8(value);
  }

  inline float ConvertLinearUInt16ToLinearFloat(const uint16_t value)
  {
    return ColorChannelConverter::RawU16ToRawF32(value);
  }

  inline uint16_t ConvertLinearFloatToLinearUInt16(const float value)
  {
    return ColorChannelConverter::RawF32ToRawU16(value);
  }


  inline TightBitmap CreateR8G8B8Srgb()
  {
    constexpr uint16_t SrcWidth = 2;
    constexpr uint16_t SrcHeight = 3;
    const std::array<uint8_t, 3 * SrcWidth * SrcHeight> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                   0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

    return {SpanUtil::AsReadOnlySpan(srcContent), PxSize2D::Create(SrcWidth, SrcHeight), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft};
  }

  inline TightBitmap CreateR8G8B8A8Srgb()
  {
    constexpr uint16_t SrcWidth = 2;
    constexpr uint16_t SrcHeight = 3;
    const std::array<uint8_t, 4 * SrcWidth * SrcHeight> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x01, 0x11, 0x21, 0x31,
                                                                   0x41, 0x51, 0x61, 0x71, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72};

    return {SpanUtil::AsReadOnlySpan(srcContent), PxExtent2D::Create(SrcWidth, SrcHeight), PixelFormat::R8G8B8A8_SRGB, BitmapOrigin::UpperLeft};
  }


  inline TightBitmap CreateR16G16B16UNorm()
  {
    auto srcBitmap = CreateR8G8B8Srgb();
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_UNORM, BitmapOrigin::UpperLeft);
    FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8SrgbToR16G16B16UNorm(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    return dstBitmap;
  }

  inline TightBitmap CreateR16G16B16A16UNorm()
  {
    auto srcBitmap = CreateR8G8B8A8Srgb();
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_UNORM, BitmapOrigin::UpperLeft);
    FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8A8SrgbToR16G16B16A16UNorm(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    return dstBitmap;
  }

  inline TightBitmap CreateR16G16B16Float()
  {
    auto srcBitmap = CreateR8G8B8Srgb();
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_SFLOAT, BitmapOrigin::UpperLeft);
    FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8SrgbToR16G16B16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    return dstBitmap;
  }

  inline TightBitmap CreateR16G16B16A16Float()
  {
    auto srcBitmap = CreateR8G8B8A8Srgb();
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_SFLOAT, BitmapOrigin::UpperLeft);
    FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8A8SrgbToR16G16B16A16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    return dstBitmap;
  }

  inline TightBitmap CreateR32G32B32Float()
  {
    auto srcBitmap = CreateR8G8B8Srgb();
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32_SFLOAT, BitmapOrigin::UpperLeft);
    FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8SrgbToR32G32B32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    return dstBitmap;
  }

  inline TightBitmap CreateR32G32B32A32Float()
  {
    auto srcBitmap = CreateR8G8B8A8Srgb();
    TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32A32_SFLOAT, BitmapOrigin::UpperLeft);
    FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8A8SrgbToR32G32B32A32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());
    return dstBitmap;
  }
}

#endif
