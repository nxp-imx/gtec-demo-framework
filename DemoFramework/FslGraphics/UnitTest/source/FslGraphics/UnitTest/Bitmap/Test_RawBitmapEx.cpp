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
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslGraphics/Bitmap/RawBitmapEx.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using Test_RawBitmapEx = TestFixtureFslGraphics;
}


TEST(Test_RawBitmapEx, Create_Empty)
{
  constexpr PxExtent2D Extent = PxExtent2D::Zero();
  constexpr PixelFormat PixelFormat = PixelFormat::R8G8B8A8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::UpperLeft;
  std::vector<uint8_t> empty;
  const auto result = RawBitmapEx::Create(SpanUtil::AsSpan(empty), Extent, PixelFormat, Origin);

  EXPECT_EQ(result.GetExtent(), Extent);
  EXPECT_EQ(result.GetPixelFormat(), PixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.GetByteSize(), 0u);
}

TEST(Test_RawBitmapEx, R8G8B8ToGrayscaleLuminanceNTSC)
{
  constexpr PxExtent2D Extent = PxExtent2D::Create(2, 3);
  constexpr PixelFormat PixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::UpperLeft;
  std::array<uint8_t, 3 * Extent.Width.Value * Extent.Height.Value> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                               0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  const auto result = RawBitmapEx::Create(SpanUtil::AsSpan(srcContent), Extent, PixelFormat, Origin);

  EXPECT_EQ(result.GetExtent(), Extent);
  EXPECT_EQ(result.GetPixelFormat(), PixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.GetByteSize(), srcContent.size());
  EXPECT_EQ(result.Stride(), PixelFormatUtil::CalcMinimumStride(Extent.Width, PixelFormatUtil::GetBytesPerPixel(PixelFormat)));
}


TEST(Test_RawBitmapEx, R8G8B8ToGrayscaleLuminanceNTSC_Stride)
{
  constexpr PxExtent2D Extent = PxExtent2D::Create(2, 3);
  constexpr PixelFormat PixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr uint16_t SrcStride = (Extent.Width.Value * 3) + 1;
  constexpr BitmapOrigin Origin = BitmapOrigin::UpperLeft;
  std::array<uint8_t, SrcStride * Extent.Height.Value> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0xFF, 0x01, 0x11, 0x21, 0x31,
                                                                  0x41, 0x51, 0xFF, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0xFF};

  const auto result = RawBitmapEx::Create(SpanUtil::AsSpan(srcContent), Extent, PixelFormat, SrcStride, Origin);

  EXPECT_EQ(result.GetExtent(), Extent);
  EXPECT_EQ(result.GetPixelFormat(), PixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.GetByteSize(), srcContent.size());
  EXPECT_EQ(result.Stride(), SrcStride);
}


// TEST(Test_RawBitmapEx, R8G8B8ToGrayscaleLuminanceNTSC_ContentSpanSizeTooLarge)
//{
//   constexpr PxExtent2D Extent = PxExtent2D::Create(2, 3);
//   constexpr PixelFormat PixelFormat = PixelFormat::R8G8B8_UNORM;
//   constexpr BitmapOrigin Origin = BitmapOrigin::UpperLeft;
//   std::array<uint8_t, (3 * Extent.Width.Value * Extent.Height.Value) + 1> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21, 0x31,
//                                                                                      0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0xFF};
//
//   EXPECT_THROW(RawBitmapEx::Create(SpanUtil::AsSpan(srcContent), Extent, PixelFormat, Origin), std::invalid_argument);
// }


TEST(Test_RawBitmapEx, R8G8B8ToGrayscaleLuminanceNTSC_ContentSpanSizeTooSmall)
{
  constexpr PxExtent2D Extent = PxExtent2D::Create(2, 3);
  constexpr PixelFormat PixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::UpperLeft;
  std::array<uint8_t, (3 * Extent.Width.Value * Extent.Height.Value) - 1> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                                     0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42};

  EXPECT_THROW(RawBitmapEx::Create(SpanUtil::AsSpan(srcContent), Extent, PixelFormat, Origin), std::invalid_argument);
}


// TEST(Test_RawBitmapEx, R8G8B8ToGrayscaleLuminanceNTSC_StrideTooSmall)
//{
//   constexpr PxExtent2D Extent = PxExtent2D::Create(2, 3);
//   constexpr PixelFormat PixelFormat = PixelFormat::R8G8B8_UNORM;
//   constexpr uint16_t SrcStride = (Extent.Width.Value * 3) + 1;
//   constexpr BitmapOrigin Origin = BitmapOrigin::UpperLeft;
//   std::array<uint8_t, SrcStride * Extent.Height.Value> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0xFF, 0x01, 0x11, 0x21, 0x31,
//                                                                   0x41, 0x51, 0xFF, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0xFF};
//
//   EXPECT_THROW(RawBitmapEx::Create(SpanUtil::AsSpan(srcContent), Extent, PixelFormat, SrcStride - 1, Origin), std::invalid_argument);
// }


TEST(Test_RawBitmapEx, R8G8B8ToGrayscaleLuminanceNTSC_StrideTooLarge)
{
  constexpr PxExtent2D Extent = PxExtent2D::Create(2, 3);
  constexpr PixelFormat PixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr uint16_t SrcStride = (Extent.Width.Value * 3) + 1;
  constexpr BitmapOrigin Origin = BitmapOrigin::UpperLeft;
  std::array<uint8_t, SrcStride * Extent.Height.Value> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0xFF, 0x01, 0x11, 0x21, 0x31,
                                                                  0x41, 0x51, 0xFF, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0xFF};

  EXPECT_THROW(RawBitmapEx::Create(SpanUtil::AsSpan(srcContent), Extent, PixelFormat, SrcStride + 1, Origin), std::invalid_argument);
}
