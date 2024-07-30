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
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Log/Math/Pixel/LogPxExtent2D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxSize2D.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslGraphics/Bitmap/ReadOnlyRawBitmap.hpp>
#include <FslGraphics/Log/LogPixelFormat.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using TestBitmap_ReadOnlyRawBitmap = TestFixtureFslGraphics;
}


TEST(TestBitmap_ReadOnlyRawBitmap, Construct_Default)
{
  ReadOnlyRawBitmap rawBitmap;

  ASSERT_TRUE(rawBitmap.IsValid());
  EXPECT_EQ(ReadOnlyRawBitmapEmpty::GetEmptyAddress(), rawBitmap.Content());
  EXPECT_EQ(PxSize1D(), rawBitmap.Width());
  EXPECT_EQ(PxSize1D(), rawBitmap.Height());
  EXPECT_EQ(PxValueU(), rawBitmap.UnsignedWidth());
  EXPECT_EQ(PxValueU(), rawBitmap.UnsignedHeight());
  EXPECT_EQ(0, rawBitmap.RawWidth());
  EXPECT_EQ(0, rawBitmap.RawHeight());
  EXPECT_EQ(0u, rawBitmap.RawUnsignedWidth());
  EXPECT_EQ(0u, rawBitmap.RawUnsignedHeight());
  EXPECT_EQ(0u, rawBitmap.Stride());
  EXPECT_EQ(0u, rawBitmap.GetByteSize());
  EXPECT_EQ(PxExtent2D(), rawBitmap.GetExtent());
  EXPECT_EQ(PxSize2D(), rawBitmap.GetSize());
  EXPECT_EQ(BitmapOrigin::UpperLeft, rawBitmap.GetOrigin());
  EXPECT_EQ(PixelFormat::Undefined, rawBitmap.GetPixelFormat());
  EXPECT_EQ(PixelFormatLayout::Undefined, rawBitmap.GetPixelFormatLayout());
}


TEST(TestBitmap_ReadOnlyRawBitmap, Create_Span_Size_PixelFormat_Origin)
{
  constexpr auto SrcWidthPx = PxSize1D::Create(2);
  constexpr auto SrcHeightPx = PxSize1D::Create(3);
  constexpr PxSize2D SrcSizePx(SrcWidthPx, SrcHeightPx);
  const std::array<uint8_t, 4 * SrcWidthPx.RawUnsignedValue() * SrcHeightPx.RawUnsignedValue()> srcContent{
    0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72};
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8A8_UNORM;
  constexpr BitmapOrigin SrcBitmapOrigin = BitmapOrigin::LowerLeft;

  auto rawBitmap = ReadOnlyRawBitmap::Create(SpanUtil::AsReadOnlySpan(srcContent), SrcSizePx, SrcPixelFormat, SrcBitmapOrigin);

  constexpr PxExtent2D SrcExtent = TypeConverter::To<PxExtent2D>(SrcSizePx);

  ASSERT_TRUE(rawBitmap.IsValid());
  EXPECT_EQ(srcContent.data(), rawBitmap.Content());
  EXPECT_EQ(SrcSizePx.Width(), rawBitmap.Width());
  EXPECT_EQ(SrcSizePx.Height(), rawBitmap.Height());
  EXPECT_EQ(SrcExtent.Width, rawBitmap.UnsignedWidth());
  EXPECT_EQ(SrcExtent.Height, rawBitmap.UnsignedHeight());
  EXPECT_EQ(SrcSizePx.RawWidth(), rawBitmap.RawWidth());
  EXPECT_EQ(SrcSizePx.RawHeight(), rawBitmap.RawHeight());
  EXPECT_EQ(SrcExtent.Width.Value, rawBitmap.RawUnsignedWidth());
  EXPECT_EQ(SrcExtent.Height.Value, rawBitmap.RawUnsignedHeight());
  EXPECT_EQ(PixelFormatUtil::CalcMinimumStride(SrcWidthPx, SrcPixelFormat), rawBitmap.Stride());
  EXPECT_EQ(srcContent.size(), rawBitmap.GetByteSize());
  EXPECT_EQ(SrcExtent, rawBitmap.GetExtent());
  EXPECT_EQ(SrcSizePx, rawBitmap.GetSize());
  EXPECT_EQ(SrcBitmapOrigin, rawBitmap.GetOrigin());
  EXPECT_EQ(SrcPixelFormat, rawBitmap.GetPixelFormat());
  EXPECT_EQ(PixelFormatUtil::GetPixelFormatLayout(SrcPixelFormat), rawBitmap.GetPixelFormatLayout());
}


TEST(TestBitmap_ReadOnlyRawBitmap, Create_Span_Extent_PixelFormat_Origin)
{
  constexpr auto SrcWidthPx = PxValueU::Create(2);
  constexpr auto SrcHeightPx = PxValueU::Create(3);
  constexpr PxExtent2D SrcExtentPx(SrcWidthPx, SrcHeightPx);
  const std::array<uint8_t, 4 * SrcWidthPx.Value * SrcHeightPx.Value> srcContent{
    0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72};
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8A8_UNORM;
  constexpr BitmapOrigin SrcBitmapOrigin = BitmapOrigin::LowerLeft;

  auto rawBitmap = ReadOnlyRawBitmap::Create(SpanUtil::AsReadOnlySpan(srcContent), SrcExtentPx, SrcPixelFormat, SrcBitmapOrigin);

  constexpr PxSize2D SrcSize = TypeConverter::To<PxSize2D>(SrcExtentPx);

  ASSERT_TRUE(rawBitmap.IsValid());
  EXPECT_EQ(srcContent.data(), rawBitmap.Content());
  EXPECT_EQ(SrcSize.Width(), rawBitmap.Width());
  EXPECT_EQ(SrcSize.Height(), rawBitmap.Height());
  EXPECT_EQ(SrcExtentPx.Width, rawBitmap.UnsignedWidth());
  EXPECT_EQ(SrcExtentPx.Height, rawBitmap.UnsignedHeight());
  EXPECT_EQ(SrcSize.RawWidth(), rawBitmap.RawWidth());
  EXPECT_EQ(SrcSize.RawHeight(), rawBitmap.RawHeight());
  EXPECT_EQ(SrcExtentPx.Width.Value, rawBitmap.RawUnsignedWidth());
  EXPECT_EQ(SrcExtentPx.Height.Value, rawBitmap.RawUnsignedHeight());
  EXPECT_EQ(PixelFormatUtil::CalcMinimumStride(SrcWidthPx, SrcPixelFormat), rawBitmap.Stride());
  EXPECT_EQ(srcContent.size(), rawBitmap.GetByteSize());
  EXPECT_EQ(SrcExtentPx, rawBitmap.GetExtent());
  EXPECT_EQ(SrcSize, rawBitmap.GetSize());
  EXPECT_EQ(SrcBitmapOrigin, rawBitmap.GetOrigin());
  EXPECT_EQ(SrcPixelFormat, rawBitmap.GetPixelFormat());
  EXPECT_EQ(PixelFormatUtil::GetPixelFormatLayout(SrcPixelFormat), rawBitmap.GetPixelFormatLayout());
}


TEST(TestBitmap_ReadOnlyRawBitmap, Create_Span_Size_PixelFormat_Stride_Origin)
{
  constexpr auto SrcWidthPx = PxSize1D::Create(2);
  constexpr auto SrcHeightPx = PxSize1D::Create(3);
  constexpr PxSize2D SrcSizePx(SrcWidthPx, SrcHeightPx);
  const std::array<uint8_t, 4 * SrcWidthPx.RawUnsignedValue() * SrcHeightPx.RawUnsignedValue()> srcContent{
    0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72};
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8A8_UNORM;
  constexpr BitmapOrigin SrcBitmapOrigin = BitmapOrigin::LowerLeft;

  constexpr uint32_t SrcStride = PixelFormatUtil::CalcMinimumStride(SrcWidthPx, SrcPixelFormat);

  auto rawBitmap = ReadOnlyRawBitmap::Create(SpanUtil::AsReadOnlySpan(srcContent), SrcSizePx, SrcPixelFormat, SrcStride, SrcBitmapOrigin);

  constexpr PxExtent2D SrcExtent = TypeConverter::To<PxExtent2D>(SrcSizePx);

  ASSERT_TRUE(rawBitmap.IsValid());

  EXPECT_EQ(srcContent.data(), rawBitmap.Content());
  EXPECT_EQ(SrcSizePx.Width(), rawBitmap.Width());
  EXPECT_EQ(SrcSizePx.Height(), rawBitmap.Height());
  EXPECT_EQ(SrcExtent.Width, rawBitmap.UnsignedWidth());
  EXPECT_EQ(SrcExtent.Height, rawBitmap.UnsignedHeight());
  EXPECT_EQ(SrcSizePx.RawWidth(), rawBitmap.RawWidth());
  EXPECT_EQ(SrcSizePx.RawHeight(), rawBitmap.RawHeight());
  EXPECT_EQ(SrcExtent.Width.Value, rawBitmap.RawUnsignedWidth());
  EXPECT_EQ(SrcExtent.Height.Value, rawBitmap.RawUnsignedHeight());
  EXPECT_EQ(PixelFormatUtil::CalcMinimumStride(SrcWidthPx, SrcPixelFormat), rawBitmap.Stride());
  EXPECT_EQ(srcContent.size(), rawBitmap.GetByteSize());
  EXPECT_EQ(SrcExtent, rawBitmap.GetExtent());
  EXPECT_EQ(SrcSizePx, rawBitmap.GetSize());
  EXPECT_EQ(SrcBitmapOrigin, rawBitmap.GetOrigin());
  EXPECT_EQ(SrcPixelFormat, rawBitmap.GetPixelFormat());
  EXPECT_EQ(PixelFormatUtil::GetPixelFormatLayout(SrcPixelFormat), rawBitmap.GetPixelFormatLayout());
}


TEST(TestBitmap_ReadOnlyRawBitmap, Create_Span_Extent_PixelFormat_Stride_Origin)
{
  constexpr auto SrcWidthPx = PxValueU::Create(2);
  constexpr auto SrcHeightPx = PxValueU::Create(3);
  constexpr PxExtent2D SrcExtent(SrcWidthPx, SrcHeightPx);
  const std::array<uint8_t, 4 * SrcWidthPx.Value * SrcHeightPx.Value> srcContent{
    0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72};

  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8A8_UNORM;
  constexpr BitmapOrigin SrcBitmapOrigin = BitmapOrigin::LowerLeft;

  constexpr uint32_t SrcStride = PixelFormatUtil::CalcMinimumStride(SrcWidthPx, SrcPixelFormat);

  auto rawBitmap = ReadOnlyRawBitmap::Create(SpanUtil::AsReadOnlySpan(srcContent), SrcExtent, SrcPixelFormat, SrcStride, SrcBitmapOrigin);

  constexpr PxSize2D SrcSizePx = TypeConverter::To<PxSize2D>(SrcExtent);

  ASSERT_TRUE(rawBitmap.IsValid());

  EXPECT_EQ(srcContent.data(), rawBitmap.Content());
  EXPECT_EQ(SrcSizePx.Width(), rawBitmap.Width());
  EXPECT_EQ(SrcSizePx.Height(), rawBitmap.Height());
  EXPECT_EQ(SrcExtent.Width, rawBitmap.UnsignedWidth());
  EXPECT_EQ(SrcExtent.Height, rawBitmap.UnsignedHeight());
  EXPECT_EQ(SrcSizePx.RawWidth(), rawBitmap.RawWidth());
  EXPECT_EQ(SrcSizePx.RawHeight(), rawBitmap.RawHeight());
  EXPECT_EQ(SrcExtent.Width.Value, rawBitmap.RawUnsignedWidth());
  EXPECT_EQ(SrcExtent.Height.Value, rawBitmap.RawUnsignedHeight());
  EXPECT_EQ(PixelFormatUtil::CalcMinimumStride(SrcWidthPx, SrcPixelFormat), rawBitmap.Stride());
  EXPECT_EQ(srcContent.size(), rawBitmap.GetByteSize());
  EXPECT_EQ(SrcExtent, rawBitmap.GetExtent());
  EXPECT_EQ(SrcSizePx, rawBitmap.GetSize());
  EXPECT_EQ(SrcBitmapOrigin, rawBitmap.GetOrigin());
  EXPECT_EQ(SrcPixelFormat, rawBitmap.GetPixelFormat());
  EXPECT_EQ(PixelFormatUtil::GetPixelFormatLayout(SrcPixelFormat), rawBitmap.GetPixelFormatLayout());
}
