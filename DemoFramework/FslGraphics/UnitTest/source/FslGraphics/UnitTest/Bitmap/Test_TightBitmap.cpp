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
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/TightBitmap.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>
#include <vector>

using namespace Fsl;

namespace
{
  using TestBitmap_TightBitmap = TestFixtureFslGraphics;

  void CompareSpans(const ReadOnlySpan<uint8_t> expected, const ReadOnlySpan<uint8_t> result)
  {
    ASSERT_EQ(expected.size(), result.size());
    for (std::size_t i = 0; i < result.size(); ++i)
    {
      EXPECT_EQ(result[i], expected[i]);
    }
  }


  void CompareSpanToRawBitmap(const ReadOnlySpan<uint8_t> expected, const ReadOnlyRawBitmap& result)
  {
    ASSERT_EQ(expected.size(), result.RawUnsignedWidth() * result.RawUnsignedHeight() * PixelFormatUtil::GetBytesPerPixel(result.GetPixelFormat()));

    const uint32_t minimumStride = PixelFormatUtil::CalcMinimumStride(result.Width(), result.GetPixelFormat());
    const auto* pResultContent = static_cast<const uint8_t*>(result.Content());
    const uint32_t resultStride = result.Stride();
    std::size_t srcIndex = 0;
    for (std::size_t y = 0; y < result.RawUnsignedHeight(); ++y)
    {
      for (std::size_t x = 0; x < minimumStride; ++x)
      {
        auto expectedByte = expected[srcIndex];
        auto resultByte = pResultContent[x + (y * resultStride)];
        EXPECT_EQ(resultByte, expectedByte);
        ++srcIndex;
      }
    }
  }

  void CompareSpanToRawBitmapEx(const ReadOnlySpan<uint8_t> expected, const RawBitmapEx& result)
  {
    ASSERT_EQ(expected.size(),
              (result.GetExtent().Width * result.GetExtent().Height).Value * PixelFormatUtil::GetBytesPerPixel(result.GetPixelFormat()));

    const uint32_t minimumStride = PixelFormatUtil::CalcMinimumStride(result.Width(), result.GetPixelFormat());
    const auto* pResultContent = static_cast<const uint8_t*>(result.Content());
    const uint32_t resultStride = result.Stride();
    std::size_t srcIndex = 0;
    for (std::size_t y = 0; y < result.RawUnsignedHeight(); ++y)
    {
      for (std::size_t x = 0; x < minimumStride; ++x)
      {
        auto expectedByte = expected[srcIndex];
        auto resultByte = pResultContent[x + (y * resultStride)];
        EXPECT_EQ(resultByte, expectedByte);
        ++srcIndex;
      }
    }
  }
}

TEST(TestBitmap_TightBitmap, ConstructEmpty)
{
  TightBitmap bitmap;

  EXPECT_EQ(bitmap.AsSpan().size(), 0u);
}


TEST(TestBitmap_TightBitmap, ConstructFromReadOnlySpan_Size)
{
  constexpr PxSize2D Size = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  std::array<uint8_t, 3 * Size.RawWidth() * Size.RawHeight()> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                         0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  const TightBitmap result(SpanUtil::AsReadOnlySpan(srcContent), Size, SrcPixelFormat, Origin);

  EXPECT_EQ(result.GetExtent(), TypeConverter::To<PxExtent2D>(Size));
  EXPECT_EQ(result.GetSize(), Size);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.GetBytesPerPixel(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat));
  EXPECT_EQ(result.Stride(), PixelFormatUtil::CalcMinimumStride(Size.Width(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat)));
  ASSERT_EQ(result.AsSpan().size(), srcContent.size());

  CompareSpans(result.AsSpan(), SpanUtil::AsReadOnlySpan(srcContent));
}


TEST(TestBitmap_TightBitmap, ConstructFromReadOnlySpan_Extend)
{
  constexpr PxExtent2D Extent = PxExtent2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  std::array<uint8_t, 3 * Extent.Width.Value * Extent.Height.Value> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                               0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  const TightBitmap result(SpanUtil::AsReadOnlySpan(srcContent), Extent, SrcPixelFormat, Origin);

  EXPECT_EQ(result.GetExtent(), Extent);
  EXPECT_EQ(result.GetSize(), TypeConverter::To<PxSize2D>(Extent));
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.GetBytesPerPixel(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat));
  EXPECT_EQ(result.Stride(), PixelFormatUtil::CalcMinimumStride(Extent.Width, PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat)));
  ASSERT_EQ(result.AsSpan().size(), srcContent.size());

  CompareSpans(result.AsSpan(), SpanUtil::AsReadOnlySpan(srcContent));
}


TEST(TestBitmap_TightBitmap, ConstructFromVector_Size)
{
  constexpr PxSize2D SizePx = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  std::vector<uint8_t> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  const TightBitmap result(SpanUtil::AsReadOnlySpan(srcContent), SizePx, SrcPixelFormat, Origin);

  EXPECT_EQ(result.GetExtent(), TypeConverter::To<PxExtent2D>(SizePx));
  EXPECT_EQ(result.GetSize(), SizePx);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.GetBytesPerPixel(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat));
  EXPECT_EQ(result.Stride(), PixelFormatUtil::CalcMinimumStride(SizePx.Width(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat)));
  ASSERT_EQ(result.AsSpan().size(), srcContent.size());

  CompareSpans(result.AsSpan(), SpanUtil::AsReadOnlySpan(srcContent));
}


TEST(TestBitmap_TightBitmap, ConstructFromVector_Extent)
{
  constexpr PxExtent2D Extent = PxExtent2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  std::vector<uint8_t> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  const TightBitmap result(SpanUtil::AsReadOnlySpan(srcContent), Extent, SrcPixelFormat, Origin);

  EXPECT_EQ(result.GetExtent(), Extent);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.GetBytesPerPixel(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat));
  EXPECT_EQ(result.Stride(), PixelFormatUtil::CalcMinimumStride(Extent.Width, PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat)));
  ASSERT_EQ(result.AsSpan().size(), srcContent.size());

  CompareSpans(result.AsSpan(), SpanUtil::AsReadOnlySpan(srcContent));
}


TEST(TestBitmap_TightBitmap, ConstructFromRawBitmap)
{
  constexpr PxExtent2D ExtentPx = PxExtent2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  std::array<uint8_t, 3 * ExtentPx.Width.Value * ExtentPx.Height.Value> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                                   0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  const ReadOnlyRawBitmap srcRawBitmap = ReadOnlyRawBitmap::Create(SpanUtil::AsReadOnlySpan(srcContent), ExtentPx, SrcPixelFormat, Origin);
  const TightBitmap result(srcRawBitmap);

  EXPECT_EQ(result.GetExtent(), ExtentPx);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.GetBytesPerPixel(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat));
  EXPECT_EQ(result.Stride(), PixelFormatUtil::CalcMinimumStride(ExtentPx.Width, PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat)));
  ASSERT_EQ(result.AsSpan().size(), srcContent.size());

  CompareSpans(result.AsSpan(), SpanUtil::AsReadOnlySpan(srcContent));
}


TEST(TestBitmap_TightBitmap, ConstructFromRawBitmapEx)
{
  constexpr PxExtent2D ExtentPx = PxExtent2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  std::array<uint8_t, 3 * ExtentPx.Width.Value * ExtentPx.Height.Value> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                                   0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  RawBitmapEx srcRawBitmap = RawBitmapEx::Create(SpanUtil::AsSpan(srcContent), ExtentPx, SrcPixelFormat, Origin);
  const TightBitmap result(srcRawBitmap);

  EXPECT_EQ(result.GetExtent(), ExtentPx);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.GetBytesPerPixel(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat));
  EXPECT_EQ(result.Stride(), PixelFormatUtil::CalcMinimumStride(ExtentPx.Width, PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat)));
  ASSERT_EQ(result.AsSpan().size(), srcContent.size());

  CompareSpans(result.AsSpan(), SpanUtil::AsReadOnlySpan(srcContent));
}


TEST(TestBitmap_TightBitmap, ConstructFromBitmapMemory)
{
  constexpr PxSize2D SizePx = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  std::array<uint8_t, 3 * SizePx.RawUnsignedWidth() * SizePx.RawUnsignedHeight()> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                                             0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  const TightBitmap result(Bitmap(SpanUtil::AsReadOnlySpan(srcContent), SizePx, SrcPixelFormat, Origin).Release());

  EXPECT_EQ(result.GetSize(), SizePx);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.GetBytesPerPixel(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat));
  EXPECT_EQ(result.Stride(), PixelFormatUtil::CalcMinimumStride(SizePx.Width(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat)));
  ASSERT_EQ(result.AsSpan().size(), srcContent.size());

  CompareSpans(result.AsSpan(), SpanUtil::AsReadOnlySpan(srcContent));
}


TEST(TestBitmap_TightBitmap, ConstructFromBitmapMemoryWithPadding)
{
  constexpr PxExtent2D ExtentPx = PxExtent2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t MinStride = 3 * ExtentPx.Width.Value;
  constexpr uint32_t Padding = 2;
  constexpr uint32_t Stride = MinStride + Padding;
  std::array<uint8_t, Stride * ExtentPx.Height.Value> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0xFF, 0xFE, 0x01, 0x11, 0x21, 0x31,
                                                                 0x41, 0x51, 0xFD, 0xFC, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0xFB, 0xFA};

  const TightBitmap result(
    BitmapMemory::Create(SpanUtil::AsReadOnlySpan(srcContent), TypeConverter::To<PxSize2D>(ExtentPx), SrcPixelFormat, Origin, Stride));

  EXPECT_EQ(result.GetExtent(), ExtentPx);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.GetBytesPerPixel(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat));
  EXPECT_EQ(result.Stride(), PixelFormatUtil::CalcMinimumStride(ExtentPx.Width, PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat)));
  ASSERT_EQ(result.AsSpan().size(), MinStride * ExtentPx.Height.Value);

  {
    const auto resultSpan = result.AsSpan();
    std::size_t srcIndex = 0;
    for (std::size_t y = 0; y < ExtentPx.Width.Value; ++y)
    {
      for (std::size_t x = 0; x < MinStride; ++x)
      {
        auto expectedByte = srcContent[srcIndex];
        auto resultByte = resultSpan[x + (y * MinStride)];
        EXPECT_EQ(resultByte, expectedByte);
        ++srcIndex;
      }
      srcIndex += Padding;
    }
  }
}


TEST(TestBitmap_TightBitmap, AsRawBitmap)
{
  constexpr PxSize2D Size = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  // constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  // constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Size.Width(), BytesPerPixel);
  std::array<uint8_t, 3 * Size.RawWidth() * Size.RawHeight()> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                         0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  const TightBitmap bitmap(SpanUtil::AsReadOnlySpan(srcContent), Size, SrcPixelFormat, Origin);
  const ReadOnlyRawBitmap result = bitmap.AsRawBitmap();

  EXPECT_EQ(result.GetExtent(), TypeConverter::To<PxExtent2D>(Size));
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.Stride(), PixelFormatUtil::CalcMinimumStride(Size.Width(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat)));

  CompareSpanToRawBitmap(SpanUtil::AsReadOnlySpan(srcContent), result);
}


TEST(TestBitmap_TightBitmap, AsRawBitmapEx)
{
  constexpr PxSize2D Size = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  // constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  // constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Size.Width(), BytesPerPixel);
  std::array<uint8_t, 3 * Size.RawWidth() * Size.RawHeight()> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                         0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  TightBitmap bitmap(SpanUtil::AsReadOnlySpan(srcContent), Size, SrcPixelFormat, Origin);
  RawBitmapEx result = bitmap.AsRawBitmap();

  EXPECT_EQ(result.GetExtent(), TypeConverter::To<PxExtent2D>(Size));
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.Stride(), PixelFormatUtil::CalcMinimumStride(Size.Width(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat)));

  CompareSpanToRawBitmapEx(SpanUtil::AsReadOnlySpan(srcContent), result);
}

TEST(TestBitmap_TightBitmap, Reset)
{
  constexpr PxSize2D Size = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  // constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  // constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Size.Width(), BytesPerPixel);
  std::array<uint8_t, 3 * Size.RawWidth() * Size.RawHeight()> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                         0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  TightBitmap result(SpanUtil::AsReadOnlySpan(srcContent), Size, SrcPixelFormat, Origin);
  EXPECT_EQ(result.GetSize(), Size);
  result.Reset();

  EXPECT_EQ(result.GetExtent(), PxExtent2D());
  EXPECT_EQ(result.GetSize(), PxSize2D());
  EXPECT_EQ(result.GetPixelFormat(), PixelFormat::Undefined);
  EXPECT_EQ(result.GetOrigin(), BitmapOrigin::UpperLeft);
  EXPECT_EQ(result.GetBytesPerPixel(), 0u);
  EXPECT_EQ(result.Stride(), 0u);
  ASSERT_EQ(result.AsSpan().size(), 0u);
}

TEST(TestBitmap_TightBitmap, Release)
{
  constexpr PxSize2D Size = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Size.Width(), BytesPerPixel);
  std::array<uint8_t, 3 * Size.RawWidth() * Size.RawHeight()> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                         0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  Bitmap result(TightBitmap(SpanUtil::AsReadOnlySpan(srcContent), Size, SrcPixelFormat, Origin).Release());

  EXPECT_EQ(result.GetExtent(), TypeConverter::To<PxExtent2D>(Size));
  EXPECT_EQ(result.GetSize(), Size);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.Stride(), Stride);
  ASSERT_EQ(result.GetByteSize(), srcContent.size());
  {
    const Bitmap::ScopedDirectReadAccess directAccess(result);
    CompareSpanToRawBitmap(SpanUtil::AsReadOnlySpan(srcContent), directAccess.AsRawBitmap());
  }
}


TEST(TestBitmap_TightBitmap, ReleaseAsVector)
{
  constexpr PxSize2D SizePx = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  // constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  // constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(SizePx.Width(), BytesPerPixel);
  std::array<uint8_t, 3 * SizePx.RawWidth() * SizePx.RawHeight()> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                             0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  const std::vector<uint8_t> result(TightBitmap(SpanUtil::AsReadOnlySpan(srcContent), SizePx, SrcPixelFormat, Origin).ReleaseAsVector());

  CompareSpans(SpanUtil::AsReadOnlySpan(result), SpanUtil::AsReadOnlySpan(srcContent));
}


TEST(TestBitmap_TightBitmap, ToBitmapMemory)
{
  constexpr PxSize2D SizePx = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(SizePx.Width(), BytesPerPixel);
  std::array<uint8_t, 3 * SizePx.RawWidth() * SizePx.RawHeight()> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                             0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  const TightBitmap tmp(SpanUtil::AsReadOnlySpan(srcContent), SizePx, SrcPixelFormat, Origin);
  const Bitmap result(tmp.ToBitmapMemory());


  EXPECT_EQ(result.GetExtent(), TypeConverter::To<PxExtent2D>(SizePx));
  EXPECT_EQ(result.GetSize(), SizePx);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.Stride(), Stride);
  ASSERT_EQ(result.GetByteSize(), srcContent.size());
  {
    const Bitmap::ScopedDirectReadAccess directAccess(result);
    CompareSpanToRawBitmap(SpanUtil::AsReadOnlySpan(srcContent), directAccess.AsRawBitmap());
  }
}


TEST(TestBitmap_TightBitmap, Construct_Size)
{
  constexpr PxSize2D SizePx = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(SizePx.Width(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat));

  const TightBitmap result(SizePx, SrcPixelFormat, Origin);

  EXPECT_EQ(result.GetExtent(), TypeConverter::To<PxExtent2D>(SizePx));
  EXPECT_EQ(result.GetSize(), SizePx);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.GetBytesPerPixel(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat));
  EXPECT_EQ(result.Stride(), Stride);
  ASSERT_EQ(result.AsSpan().size(), Stride * TypeConverter::To<PxValueU>(SizePx.Height()).Value);
}


TEST(TestBitmap_TightBitmap, Construct_Extend)
{
  constexpr PxExtent2D ExtentPx = PxExtent2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(ExtentPx.Width, PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat));

  const TightBitmap result(ExtentPx, SrcPixelFormat, Origin);

  EXPECT_EQ(result.GetExtent(), ExtentPx);
  EXPECT_EQ(result.GetSize(), TypeConverter::To<PxSize2D>(ExtentPx));
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.GetBytesPerPixel(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat));
  EXPECT_EQ(result.Stride(), Stride);
  ASSERT_EQ(result.AsSpan().size(), Stride * ExtentPx.Height.Value);
}
