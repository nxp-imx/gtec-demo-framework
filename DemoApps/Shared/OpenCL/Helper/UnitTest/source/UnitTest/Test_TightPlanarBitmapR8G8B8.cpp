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
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
// #include <FslGraphics/Log/Bitmap/LogBitmapOrigin.hpp>
#include <FslGraphics/Bitmap/TightBitmap.hpp>
#include <Shared/OpenCL/Helper/TightPlanarBitmapR8G8B8.hpp>
#include <array>
#include <vector>

using namespace Fsl;

namespace
{
  using Test_TightPlanarBitmapR8G8B8 = TestFixtureFslGraphics;

  void CompareContent(const ReadOnlySpan<uint8_t> expected, const uint32_t offset, const uint32_t stride, const ReadOnlySpan<uint8_t> result)
  {
    ASSERT_EQ((expected.size() % stride), 0u);
    ASSERT_LT(offset, stride);
    ASSERT_EQ(result.size(), (expected.size() / stride));

    for (std::size_t i = 0; i < result.size(); ++i)
    {
      EXPECT_EQ(result[i], expected[(i * stride) + offset]);
    }
  }

  void CompareContentRawBitmap(const ReadOnlySpan<uint8_t> expected, const uint32_t offset, const uint32_t stride,
                               const ReadOnlyRawBitmap& resultRawBitmap)
  {
    ASSERT_EQ((expected.size() % stride), 0u);
    ASSERT_LT(offset, stride);
    ASSERT_EQ(resultRawBitmap.GetByteSize(), (expected.size() / stride));

    const auto* pResultContent = static_cast<const uint8_t*>(resultRawBitmap.Content());
    const uint32_t resultStride = resultRawBitmap.Stride();
    uint32_t expectedIndex = offset;
    for (std::size_t y = 0; y < resultRawBitmap.RawUnsignedHeight(); ++y)
    {
      for (std::size_t x = 0; x < resultRawBitmap.RawUnsignedWidth(); ++x)
      {
        auto expectedByte = expected[expectedIndex];
        auto resultByte = pResultContent[x + (y * resultStride)];
        EXPECT_EQ(resultByte, expectedByte);
        expectedIndex += stride;
      }
    }
  }

  void CompareContentRawBitmapEx(const ReadOnlySpan<uint8_t> expected, const uint32_t offset, const uint32_t stride,
                                 const RawBitmapEx& resultRawBitmap)
  {
    ASSERT_EQ((expected.size() % stride), 0u);
    ASSERT_LT(offset, stride);
    ASSERT_EQ(resultRawBitmap.GetByteSize(), (expected.size() / stride));

    const auto* pResultContent = static_cast<const uint8_t*>(resultRawBitmap.Content());
    const uint32_t resultStride = resultRawBitmap.Stride();
    uint32_t expectedIndex = offset;
    for (std::size_t y = 0; y < resultRawBitmap.RawUnsignedHeight(); ++y)
    {
      for (std::size_t x = 0; x < resultRawBitmap.RawUnsignedWidth(); ++x)
      {
        auto expectedByte = expected[expectedIndex];
        auto resultByte = pResultContent[x + (y * resultStride)];
        EXPECT_EQ(resultByte, expectedByte);
        expectedIndex += stride;
      }
    }
  }


  void CompareSpanToRawBitmap(const ReadOnlySpan<uint8_t> expected, const ReadOnlyRawBitmap& result, const uint32_t srcStride)
  {
    const auto* pResultContent = static_cast<const uint8_t*>(result.Content());
    const uint32_t resultStride = result.Stride();
    for (std::size_t y = 0; y < result.RawUnsignedHeight(); ++y)
    {
      for (std::size_t x = 0; x < srcStride; ++x)
      {
        auto expectedByte = expected[x + (y * srcStride)];
        auto resultByte = pResultContent[x + (y * resultStride)];
        EXPECT_EQ(resultByte, expectedByte);
      }
    }
  }
}

TEST(Test_TightPlanarBitmapR8G8B8, ConstructEmpty)
{
  TightPlanarBitmapR8G8B8 bitmap;

  EXPECT_EQ(bitmap.AsSpanR().size(), 0u);
  EXPECT_EQ(bitmap.AsSpanG().size(), 0u);
  EXPECT_EQ(bitmap.AsSpanB().size(), 0u);
  EXPECT_EQ(bitmap.GetBytesPerPlanePixel(), 1u);
}


TEST(Test_TightPlanarBitmapR8G8B8, ConstructFromReadOnlySpan_Size)
{
  constexpr PxSize2D Size = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Size.Width(), BytesPerPixel);
  constexpr uint32_t ExpectedPlaneStride = Stride / 3;
  constexpr uint32_t ExpectedBytesPerPlane = ExpectedPlaneStride * Size.RawUnsignedHeight();

  std::array<uint8_t, 3 * Size.RawWidth() * Size.RawHeight()> srcContent{0x00, 0x40, 0x80, 0x10, 0x50, 0x90, 0x01, 0x41, 0x81,
                                                                         0x11, 0x51, 0x91, 0x02, 0x42, 0x82, 0x12, 0x52, 0x92};

  const TightPlanarBitmapR8G8B8 result(SpanUtil::AsReadOnlySpan(srcContent), Size, SrcPixelFormat, Origin);

  EXPECT_EQ(result.GetExtent(), TypeConverter::To<PxExtent2D>(Size));
  EXPECT_EQ(result.GetSize(), Size);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.PlaneStride(), ExpectedPlaneStride);
  ASSERT_EQ(result.AsSpanR().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanG().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanB().size(), ExpectedBytesPerPlane);

  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 0, 3, result.AsSpanR());
  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 1, 3, result.AsSpanG());
  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 2, 3, result.AsSpanB());
}


TEST(Test_TightPlanarBitmapR8G8B8, ConstructFromReadOnlySpan_Extent)
{
  constexpr PxExtent2D Extent = PxExtent2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Extent.Width, BytesPerPixel);
  constexpr uint32_t ExpectedPlaneStride = Stride / 3;
  constexpr uint32_t ExpectedBytesPerPlane = ExpectedPlaneStride * Extent.Height.Value;

  std::array<uint8_t, 3 * Extent.Width.Value * Extent.Height.Value> srcContent{0x00, 0x40, 0x80, 0x10, 0x50, 0x90, 0x01, 0x41, 0x81,
                                                                               0x11, 0x51, 0x91, 0x02, 0x42, 0x82, 0x12, 0x52, 0x92};

  const TightPlanarBitmapR8G8B8 result(SpanUtil::AsReadOnlySpan(srcContent), Extent, SrcPixelFormat, Origin);

  EXPECT_EQ(result.GetExtent(), Extent);
  EXPECT_EQ(result.GetSize(), TypeConverter::To<PxSize2D>(Extent));
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.PlaneStride(), ExpectedPlaneStride);
  ASSERT_EQ(result.AsSpanR().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanG().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanB().size(), ExpectedBytesPerPlane);

  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 0, 3, result.AsSpanR());
  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 1, 3, result.AsSpanG());
  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 2, 3, result.AsSpanB());
}


TEST(Test_TightPlanarBitmapR8G8B8, ConstructFromRawBitmap)
{
  constexpr PxExtent2D Extent = PxExtent2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Extent.Width, BytesPerPixel);
  constexpr uint32_t ExpectedPlaneStride = Stride / 3;
  constexpr uint32_t ExpectedBytesPerPlane = ExpectedPlaneStride * Extent.Height.Value;

  std::array<uint8_t, 3 * Extent.Width.Value * Extent.Height.Value> srcContent{0x00, 0x40, 0x80, 0x10, 0x50, 0x90, 0x01, 0x41, 0x81,
                                                                               0x11, 0x51, 0x91, 0x02, 0x42, 0x82, 0x12, 0x52, 0x92};

  const ReadOnlyRawBitmap srcRawBitmap = ReadOnlyRawBitmap::Create(SpanUtil::AsReadOnlySpan(srcContent), Extent, SrcPixelFormat, Origin);
  const TightPlanarBitmapR8G8B8 result(srcRawBitmap);

  EXPECT_EQ(result.GetExtent(), Extent);
  EXPECT_EQ(result.GetSize(), TypeConverter::To<PxSize2D>(Extent));
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.PlaneStride(), ExpectedPlaneStride);
  ASSERT_EQ(result.AsSpanR().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanG().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanB().size(), ExpectedBytesPerPlane);

  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 0, 3, result.AsSpanR());
  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 1, 3, result.AsSpanG());
  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 2, 3, result.AsSpanB());
}


TEST(Test_TightPlanarBitmapR8G8B8, ConstructFromRawBitmapEx)
{
  constexpr PxExtent2D Extent = PxExtent2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Extent.Width, BytesPerPixel);
  constexpr uint32_t ExpectedPlaneStride = Stride / 3;
  constexpr uint32_t ExpectedBytesPerPlane = ExpectedPlaneStride * Extent.Height.Value;

  std::array<uint8_t, 3 * Extent.Width.Value * Extent.Height.Value> srcContent{0x00, 0x40, 0x80, 0x10, 0x50, 0x90, 0x01, 0x41, 0x81,
                                                                               0x11, 0x51, 0x91, 0x02, 0x42, 0x82, 0x12, 0x52, 0x92};

  const RawBitmapEx srcRawBitmap = RawBitmapEx::Create(SpanUtil::AsSpan(srcContent), Extent, SrcPixelFormat, Origin);
  const TightPlanarBitmapR8G8B8 result(srcRawBitmap);

  EXPECT_EQ(result.GetExtent(), Extent);
  EXPECT_EQ(result.GetSize(), TypeConverter::To<PxSize2D>(Extent));
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.PlaneStride(), ExpectedPlaneStride);
  ASSERT_EQ(result.AsSpanR().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanG().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanB().size(), ExpectedBytesPerPlane);

  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 0, 3, result.AsSpanR());
  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 1, 3, result.AsSpanG());
  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 2, 3, result.AsSpanB());
}


TEST(Test_TightPlanarBitmapR8G8B8, ConstructFromBitmap)
{
  constexpr PxExtent2D Extent = PxExtent2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Extent.Width, BytesPerPixel);
  constexpr uint32_t ExpectedPlaneStride = Stride / 3;
  constexpr uint32_t ExpectedBytesPerPlane = ExpectedPlaneStride * Extent.Height.Value;

  std::array<uint8_t, 3 * Extent.Width.Value * Extent.Height.Value> srcContent{0x00, 0x40, 0x80, 0x10, 0x50, 0x90, 0x01, 0x41, 0x81,
                                                                               0x11, 0x51, 0x91, 0x02, 0x42, 0x82, 0x12, 0x52, 0x92};

  const Bitmap srcBitmap(SpanUtil::AsSpan(srcContent), TypeConverter::To<PxSize2D>(Extent), SrcPixelFormat, Origin);
  const TightPlanarBitmapR8G8B8 result(srcBitmap);

  EXPECT_EQ(result.GetExtent(), Extent);
  EXPECT_EQ(result.GetSize(), TypeConverter::To<PxSize2D>(Extent));
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.PlaneStride(), ExpectedPlaneStride);
  ASSERT_EQ(result.AsSpanR().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanG().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanB().size(), ExpectedBytesPerPlane);

  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 0, 3, result.AsSpanR());
  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 1, 3, result.AsSpanG());
  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 2, 3, result.AsSpanB());
}

TEST(Test_TightPlanarBitmapR8G8B8, ConstructFromBitmapMemory)
{
  constexpr PxExtent2D Extent = PxExtent2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Extent.Width, BytesPerPixel);
  constexpr uint32_t ExpectedPlaneStride = Stride / 3;
  constexpr uint32_t ExpectedBytesPerPlane = ExpectedPlaneStride * Extent.Height.Value;

  std::array<uint8_t, 3 * Extent.Width.Value * Extent.Height.Value> srcContent{0x00, 0x40, 0x80, 0x10, 0x50, 0x90, 0x01, 0x41, 0x81,
                                                                               0x11, 0x51, 0x91, 0x02, 0x42, 0x82, 0x12, 0x52, 0x92};

  const TightPlanarBitmapR8G8B8 result(Bitmap(SpanUtil::AsSpan(srcContent), TypeConverter::To<PxSize2D>(Extent), SrcPixelFormat, Origin).Release());

  EXPECT_EQ(result.GetExtent(), Extent);
  EXPECT_EQ(result.GetSize(), TypeConverter::To<PxSize2D>(Extent));
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.PlaneStride(), ExpectedPlaneStride);
  ASSERT_EQ(result.AsSpanR().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanG().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanB().size(), ExpectedBytesPerPlane);

  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 0, 3, result.AsSpanR());
  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 1, 3, result.AsSpanG());
  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 2, 3, result.AsSpanB());
}


TEST(Test_TightPlanarBitmapR8G8B8, ConstructFromTightBitmap)
{
  constexpr PxExtent2D Extent = PxExtent2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Extent.Width, BytesPerPixel);
  constexpr uint32_t ExpectedPlaneStride = Stride / 3;
  constexpr uint32_t ExpectedBytesPerPlane = ExpectedPlaneStride * Extent.Height.Value;

  std::array<uint8_t, 3 * Extent.Width.Value * Extent.Height.Value> srcContent{0x00, 0x40, 0x80, 0x10, 0x50, 0x90, 0x01, 0x41, 0x81,
                                                                               0x11, 0x51, 0x91, 0x02, 0x42, 0x82, 0x12, 0x52, 0x92};

  const TightBitmap srcBitmap(SpanUtil::AsSpan(srcContent), Extent, SrcPixelFormat, Origin);
  const TightPlanarBitmapR8G8B8 result(srcBitmap);

  EXPECT_EQ(result.GetExtent(), Extent);
  EXPECT_EQ(result.GetSize(), TypeConverter::To<PxSize2D>(Extent));
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.PlaneStride(), ExpectedPlaneStride);
  ASSERT_EQ(result.AsSpanR().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanG().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanB().size(), ExpectedBytesPerPlane);

  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 0, 3, result.AsSpanR());
  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 1, 3, result.AsSpanG());
  CompareContent(SpanUtil::AsReadOnlySpan(srcContent), 2, 3, result.AsSpanB());
}

TEST(Test_TightPlanarBitmapR8G8B8, Construct_Size)
{
  constexpr PxSize2D Size = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Size.Width(), BytesPerPixel);
  constexpr uint32_t ExpectedPlaneStride = Stride / 3;
  constexpr uint32_t ExpectedBytesPerPlane = ExpectedPlaneStride * Size.RawUnsignedHeight();


  const TightPlanarBitmapR8G8B8 result(Size, SrcPixelFormat, Origin);

  EXPECT_EQ(result.GetExtent(), TypeConverter::To<PxExtent2D>(Size));
  EXPECT_EQ(result.GetSize(), Size);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.PlaneStride(), ExpectedPlaneStride);
  ASSERT_EQ(result.AsSpanR().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanG().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanB().size(), ExpectedBytesPerPlane);
}


TEST(Test_TightPlanarBitmapR8G8B8, Construct_Extent)
{
  constexpr PxExtent2D Extent = PxExtent2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Extent.Width, BytesPerPixel);
  constexpr uint32_t ExpectedPlaneStride = Stride / 3;
  constexpr uint32_t ExpectedBytesPerPlane = ExpectedPlaneStride * Extent.Height.Value;


  const TightPlanarBitmapR8G8B8 result(Extent, SrcPixelFormat, Origin);

  EXPECT_EQ(result.GetExtent(), Extent);
  EXPECT_EQ(result.GetSize(), TypeConverter::To<PxSize2D>(Extent));
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.PlaneStride(), ExpectedPlaneStride);
  ASSERT_EQ(result.AsSpanR().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanG().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanB().size(), ExpectedBytesPerPlane);
}


TEST(Test_TightPlanarBitmapR8G8B8, ToBitmap)
{
  constexpr PxSize2D Size = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Size.Width(), BytesPerPixel);
  // constexpr uint32_t ExpectedPlaneStride = Stride / 3;
  // constexpr uint32_t ExpectedBytesPerPlane = ExpectedPlaneStride * Size.RawUnsignedHeight();

  std::array<uint8_t, 3 * Size.RawWidth() * Size.RawHeight()> srcContent{0x00, 0x40, 0x80, 0x10, 0x50, 0x90, 0x01, 0x41, 0x81,
                                                                         0x11, 0x51, 0x91, 0x02, 0x42, 0x82, 0x12, 0x52, 0x92};

  const TightPlanarBitmapR8G8B8 tmp(SpanUtil::AsReadOnlySpan(srcContent), Size, SrcPixelFormat, Origin);
  const Bitmap result(tmp.ToBitmapMemory());

  EXPECT_EQ(result.GetExtent(), TypeConverter::To<PxExtent2D>(Size));
  EXPECT_EQ(result.GetSize(), Size);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.Stride(), Stride);
  ASSERT_EQ(result.GetByteSize(), srcContent.size());
  {
    const Bitmap::ScopedDirectReadAccess directAccess(result);
    CompareSpanToRawBitmap(SpanUtil::AsReadOnlySpan(srcContent), directAccess.AsRawBitmap(), Stride);
  }
}

TEST(Test_TightPlanarBitmapR8G8B8, AsRawBitmap)
{
  constexpr PxSize2D Size = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Size.Width(), BytesPerPixel);
  constexpr uint32_t ExpectedPlaneStride = Stride / 3;
  constexpr uint32_t ExpectedBytesPerPlane = ExpectedPlaneStride * TypeConverter::To<PxValueU>(Size.Height()).Value;

  std::array<uint8_t, 3 * Size.RawWidth() * Size.RawHeight()> srcContent{0x00, 0x40, 0x80, 0x10, 0x50, 0x90, 0x01, 0x41, 0x81,
                                                                         0x11, 0x51, 0x91, 0x02, 0x42, 0x82, 0x12, 0x52, 0x92};

  const TightPlanarBitmapR8G8B8 result(SpanUtil::AsReadOnlySpan(srcContent), Size, SrcPixelFormat, Origin);

  EXPECT_EQ(result.GetExtent(), TypeConverter::To<PxExtent2D>(Size));
  EXPECT_EQ(result.GetSize(), Size);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.PlaneStride(), ExpectedPlaneStride);
  ASSERT_EQ(result.AsSpanR().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanG().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanB().size(), ExpectedBytesPerPlane);

  CompareContentRawBitmap(SpanUtil::AsReadOnlySpan(srcContent), 0, 3, result.AsRawBitmapR());
  CompareContentRawBitmap(SpanUtil::AsReadOnlySpan(srcContent), 1, 3, result.AsRawBitmapG());
  CompareContentRawBitmap(SpanUtil::AsReadOnlySpan(srcContent), 2, 3, result.AsRawBitmapB());
}

TEST(Test_TightPlanarBitmapR8G8B8, AsRawBitmapEx)
{
  constexpr PxSize2D Size = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  constexpr uint32_t BytesPerPixel = PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat);
  constexpr uint32_t Stride = PixelFormatUtil::CalcMinimumStride(Size.Width(), BytesPerPixel);
  constexpr uint32_t ExpectedPlaneStride = Stride / 3;
  constexpr uint32_t ExpectedBytesPerPlane = ExpectedPlaneStride * Size.RawUnsignedHeight();

  std::array<uint8_t, 3 * Size.RawWidth() * Size.RawHeight()> srcContent{0x00, 0x40, 0x80, 0x10, 0x50, 0x90, 0x01, 0x41, 0x81,
                                                                         0x11, 0x51, 0x91, 0x02, 0x42, 0x82, 0x12, 0x52, 0x92};

  TightPlanarBitmapR8G8B8 result(SpanUtil::AsReadOnlySpan(srcContent), Size, SrcPixelFormat, Origin);

  EXPECT_EQ(result.GetExtent(), TypeConverter::To<PxExtent2D>(Size));
  EXPECT_EQ(result.GetSize(), Size);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.PlaneStride(), ExpectedPlaneStride);
  ASSERT_EQ(result.AsSpanR().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanG().size(), ExpectedBytesPerPlane);
  ASSERT_EQ(result.AsSpanB().size(), ExpectedBytesPerPlane);

  CompareContentRawBitmapEx(SpanUtil::AsReadOnlySpan(srcContent), 0, 3, result.AsRawBitmapR());
  CompareContentRawBitmapEx(SpanUtil::AsReadOnlySpan(srcContent), 1, 3, result.AsRawBitmapG());
  CompareContentRawBitmapEx(SpanUtil::AsReadOnlySpan(srcContent), 2, 3, result.AsRawBitmapB());
}
