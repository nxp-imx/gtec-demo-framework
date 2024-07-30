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
#include <FslBase/Log/Math/Pixel/FmtPxExtent2D.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslGraphics/Bitmap/Converter/BitmapConverterFunctions.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using TestBitmapConverter_BitmapConverterFunctions = TestFixtureFslGraphics;
}


TEST(TestBitmapConverter_BitmapConverterFunctions, R8G8B8ToGrayscaleLuminanceNTSC_Empty)
{
  {
    const PixelFormat dstPixelFormat = PixelFormat::R8_UNORM;
    const Bitmap srcBitmap(PxSize2D(), PixelFormat::R8G8B8_UNORM);
    auto result = BitmapConverterFunctions::R8G8B8ToGrayscaleLuminanceNTSC(srcBitmap, dstPixelFormat);

    EXPECT_EQ(result.GetExtent(), srcBitmap.GetExtent());
    EXPECT_EQ(result.GetOrigin(), srcBitmap.GetOrigin());
    EXPECT_EQ(result.GetPixelFormat(), dstPixelFormat);
  }
  {
    const PixelFormat dstPixelFormat = PixelFormat::EX_LUMINANCE8_UNORM;
    const Bitmap srcBitmap(PxSize2D(), PixelFormat::R8G8B8_UNORM);
    auto result = BitmapConverterFunctions::R8G8B8ToGrayscaleLuminanceNTSC(srcBitmap, dstPixelFormat);

    EXPECT_EQ(result.GetExtent(), srcBitmap.GetExtent());
    EXPECT_EQ(result.GetOrigin(), srcBitmap.GetOrigin());
    EXPECT_EQ(result.GetPixelFormat(), dstPixelFormat);
  }
  {
    const PixelFormat dstPixelFormat = PixelFormat::EX_ALPHA8_UNORM;
    const Bitmap srcBitmap(PxSize2D(), PixelFormat::R8G8B8_UNORM);
    auto result = BitmapConverterFunctions::R8G8B8ToGrayscaleLuminanceNTSC(srcBitmap, dstPixelFormat);

    EXPECT_EQ(result.GetExtent(), srcBitmap.GetExtent());
    EXPECT_EQ(result.GetOrigin(), srcBitmap.GetOrigin());
    EXPECT_EQ(result.GetPixelFormat(), dstPixelFormat);
  }
}

TEST(TestBitmapConverter_BitmapConverterFunctions, R8G8B8ToGrayscaleLuminanceNTSC_SrcUnsupportedPixelFormat)
{
  constexpr uint16_t SrcWidth = 2;
  constexpr uint16_t SrcHeight = 3;
  const PixelFormat dstPixelFormat = PixelFormat::R8_UNORM;
  const std::array<uint8_t, 4 * SrcWidth * SrcHeight> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x01, 0x11, 0x21, 0x31,
                                                                 0x41, 0x51, 0x61, 0x71, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72};

  const Bitmap srcBitmap(SpanUtil::AsReadOnlySpan(srcContent), PxSize2D::Create(SrcWidth, SrcHeight), PixelFormat::R8G8B8A8_UNORM);

  EXPECT_THROW(BitmapConverterFunctions::R8G8B8ToGrayscaleLuminanceNTSC(srcBitmap, dstPixelFormat), UnsupportedPixelFormatException);
}

TEST(TestBitmapConverter_BitmapConverterFunctions, R8G8B8ToGrayscaleLuminanceNTSC_DstUnsupportedPixelFormat)
{
  constexpr uint16_t SrcWidth = 2;
  constexpr uint16_t SrcHeight = 3;
  const PixelFormat dstPixelFormat = PixelFormat::R8G8_UNORM;
  const std::array<uint8_t, 3 * SrcWidth * SrcHeight> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                 0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  const Bitmap srcBitmap(SpanUtil::AsReadOnlySpan(srcContent), PxSize2D::Create(SrcWidth, SrcHeight), PixelFormat::R8G8B8_UNORM);

  EXPECT_THROW(BitmapConverterFunctions::R8G8B8ToGrayscaleLuminanceNTSC(srcBitmap, dstPixelFormat), UnsupportedPixelFormatException);
}


TEST(TestBitmapConverter_BitmapConverterFunctions, R8G8B8ToGrayscaleLuminanceNTSC)
{
  constexpr uint16_t SrcWidth = 2;
  constexpr uint16_t SrcHeight = 3;
  const PixelFormat dstPixelFormat = PixelFormat::R8_UNORM;
  const std::array<uint8_t, 3 * SrcWidth * SrcHeight> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                 0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  const Bitmap srcBitmap(SpanUtil::AsReadOnlySpan(srcContent), PxSize2D::Create(SrcWidth, SrcHeight), PixelFormat::R8G8B8_UNORM);

  auto result = BitmapConverterFunctions::R8G8B8ToGrayscaleLuminanceNTSC(srcBitmap, dstPixelFormat);

  EXPECT_EQ(result.GetExtent(), srcBitmap.GetExtent());
  EXPECT_EQ(result.GetOrigin(), srcBitmap.GetOrigin());
  EXPECT_EQ(result.GetPixelFormat(), dstPixelFormat);
}


TEST(TestBitmapConverter_BitmapConverterFunctions, R8G8B8ToGrayscaleLuminanceNTSC_Stride)
{
  constexpr uint16_t SrcWidth = 2;
  constexpr uint16_t SrcStride = (SrcWidth * 3) + 1;
  constexpr uint16_t SrcHeight = 3;
  const PixelFormat dstPixelFormat = PixelFormat::R8_UNORM;
  const std::array<uint8_t, SrcStride * SrcHeight> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0xFF, 0x01, 0x11, 0x21, 0x31,
                                                              0x41, 0x51, 0xFF, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0xFF};

  const Bitmap srcBitmap(SpanUtil::AsReadOnlySpan(srcContent), PxSize2D::Create(SrcWidth, SrcHeight), PixelFormat::R8G8B8_UNORM, SrcStride);

  auto result = BitmapConverterFunctions::R8G8B8ToGrayscaleLuminanceNTSC(srcBitmap, dstPixelFormat);

  EXPECT_EQ(result.GetExtent(), srcBitmap.GetExtent());
  EXPECT_EQ(result.GetOrigin(), srcBitmap.GetOrigin());
  EXPECT_EQ(result.GetPixelFormat(), dstPixelFormat);
}
