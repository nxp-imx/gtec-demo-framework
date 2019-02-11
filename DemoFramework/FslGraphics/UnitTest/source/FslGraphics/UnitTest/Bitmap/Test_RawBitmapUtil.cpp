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

#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Log/LogPixelFormat.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslUnitTest/Test/TestArray.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using TestBitmap_RawBitmapUtil = TestFixtureFslGraphics;
}


TEST(TestBitmap_RawBitmapUtil, CalcAlignment)
{
  EXPECT_EQ(1, RawBitmapUtil::CalcAlignment(PixelFormat::R8_UNORM, 1u, 1u, 1u));
  EXPECT_EQ(2, RawBitmapUtil::CalcAlignment(PixelFormat::R8_UNORM, 2u, 2u, 1u));

  EXPECT_EQ(2, RawBitmapUtil::CalcAlignment(PixelFormat::R8G8_UNORM, 1u, 2u, 2u));
  EXPECT_EQ(4, RawBitmapUtil::CalcAlignment(PixelFormat::R8G8_UNORM, 2u, 4u, 2u));

  EXPECT_EQ(4, RawBitmapUtil::CalcAlignment(PixelFormat::R8G8B8A8_UNORM, 1u, 4u, 4u));
  EXPECT_EQ(8, RawBitmapUtil::CalcAlignment(PixelFormat::R8G8B8A8_UNORM, 100u, 400u, 4u));
}


TEST(TestBitmap_RawBitmapUtil, CalcAlignment_Bitmap)
{
  std::array<uint8_t, 400 * 2> tmp{};

  EXPECT_EQ(1, RawBitmapUtil::CalcAlignment(RawBitmap(tmp.data(), 1, 1, PixelFormat::R8_UNORM, BitmapOrigin::UpperLeft), 1u));
  EXPECT_EQ(1, RawBitmapUtil::CalcAlignment(RawBitmap(tmp.data(), 1, 2, PixelFormat::R8_UNORM, BitmapOrigin::UpperLeft), 1u));
  EXPECT_EQ(2, RawBitmapUtil::CalcAlignment(RawBitmap(tmp.data(), 2, 1, PixelFormat::R8_UNORM, BitmapOrigin::UpperLeft), 1u));

  EXPECT_EQ(2, RawBitmapUtil::CalcAlignment(RawBitmap(tmp.data(), 1, 1, PixelFormat::R8G8_UNORM, BitmapOrigin::UpperLeft), 2u));
  EXPECT_EQ(4, RawBitmapUtil::CalcAlignment(RawBitmap(tmp.data(), 2, 1, PixelFormat::R8G8_UNORM, BitmapOrigin::UpperLeft), 2u));

  EXPECT_EQ(4, RawBitmapUtil::CalcAlignment(RawBitmap(tmp.data(), 1, 1, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft), 4u));
  EXPECT_EQ(8, RawBitmapUtil::CalcAlignment(RawBitmap(tmp.data(), 2, 1, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft), 4u));
  EXPECT_EQ(8, RawBitmapUtil::CalcAlignment(RawBitmap(tmp.data(), 100, 2, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft), 4u));
}


TEST(TestBitmap_RawBitmapUtil, CheckIsUsingMinimumStrideForAlignment)
{
  std::array<uint8_t, 400 * 2> tmp{};

  EXPECT_NO_THROW(
    RawBitmapUtil::CheckIsUsingMinimumStrideForAlignment(RawBitmap(tmp.data(), 1, 1, PixelFormat::R8_UNORM, BitmapOrigin::UpperLeft), 1u));
  EXPECT_NO_THROW(
    RawBitmapUtil::CheckIsUsingMinimumStrideForAlignment(RawBitmap(tmp.data(), 1, 1, PixelFormat::R8_UNORM, 8u, BitmapOrigin::UpperLeft), 8u));
  EXPECT_NO_THROW(
    RawBitmapUtil::CheckIsUsingMinimumStrideForAlignment(RawBitmap(tmp.data(), 1, 2, PixelFormat::R8_UNORM, BitmapOrigin::UpperLeft), 1u));
  EXPECT_NO_THROW(
    RawBitmapUtil::CheckIsUsingMinimumStrideForAlignment(RawBitmap(tmp.data(), 2, 1, PixelFormat::R8_UNORM, BitmapOrigin::UpperLeft), 2u));

  EXPECT_NO_THROW(
    RawBitmapUtil::CheckIsUsingMinimumStrideForAlignment(RawBitmap(tmp.data(), 1, 1, PixelFormat::R8G8_UNORM, BitmapOrigin::UpperLeft), 2u));
  EXPECT_NO_THROW(
    RawBitmapUtil::CheckIsUsingMinimumStrideForAlignment(RawBitmap(tmp.data(), 2, 1, PixelFormat::R8G8_UNORM, BitmapOrigin::UpperLeft), 4u));

  EXPECT_NO_THROW(
    RawBitmapUtil::CheckIsUsingMinimumStrideForAlignment(RawBitmap(tmp.data(), 1, 1, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft), 4u));
  EXPECT_NO_THROW(
    RawBitmapUtil::CheckIsUsingMinimumStrideForAlignment(RawBitmap(tmp.data(), 2, 1, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft), 8u));
  EXPECT_NO_THROW(
    RawBitmapUtil::CheckIsUsingMinimumStrideForAlignment(RawBitmap(tmp.data(), 100, 2, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft), 8u));
}


TEST(TestBitmap_RawBitmapUtil, CheckIsUsingMinimumStrideForAlignment_Unsupported)
{
  std::array<uint8_t, 100> tmp{};

  EXPECT_THROW(
    RawBitmapUtil::CheckIsUsingMinimumStrideForAlignment(RawBitmap(tmp.data(), 1, 1, PixelFormat::R8_UNORM, 10u, BitmapOrigin::UpperLeft), 10u),
    UnsupportedAlignmentException);
}


TEST(TestBitmap_RawBitmapUtil, CheckIsUsingMinimumStrideForAlignment_NotMin)
{
  std::array<uint8_t, 100> tmp{};

  EXPECT_THROW(
    RawBitmapUtil::CheckIsUsingMinimumStrideForAlignment(RawBitmap(tmp.data(), 1, 1, PixelFormat::R8_UNORM, 2u, BitmapOrigin::UpperLeft), 1u),
    UnsupportedStrideException);
}


TEST(TestBitmap_RawBitmapUtil, Swizzle_InPlace_24_012To210)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, X, B, B, B, X
  std::array<uint8_t, 2 * (3 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8};
  //----------------------------------------------A, A, A, X, B, B, B, X
  const std::array<uint8_t, 2 * (3 + 1)> expected{3, 2, 1, 4, 7, 6, 5, 8};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle(rawBitmap, 2u, 1u, 0u, 0u);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle_InPlace_24_012To120)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, X, B, B, B, X
  std::array<uint8_t, 2 * (3 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8};
  //----------------------------------------------A, A, A, X, B, B, B, X
  const std::array<uint8_t, 2 * (3 + 1)> expected{2, 3, 1, 4, 6, 7, 5, 8};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle(rawBitmap, 1u, 2u, 0u, 0u);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle_InPlace_24_012To201)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, X, B, B, B, X
  std::array<uint8_t, 2 * (3 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8};
  //----------------------------------------------A, A, A, X, B, B, B, X
  const std::array<uint8_t, 2 * (3 + 1)> expected{3, 1, 2, 4, 7, 5, 6, 8};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle(rawBitmap, 2u, 0u, 01u, 0u);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle_InPlace_32_0123To1230)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, A, X, B, B, B, B, X
  std::array<uint8_t, 2 * (4 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, A, A, A, X, B, B, B, B, X
  const std::array<uint8_t, 2 * (4 + 1)> expected{2, 3, 4, 1, 5, 7, 8, 9, 6, 10};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle(rawBitmap, 1u, 2u, 3u, 0u);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle_InPlace_32_0123To2103)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, A, X, B, B, B, B, X
  std::array<uint8_t, 2 * (4 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, A, A, A, X, B, B, B, B, X
  const std::array<uint8_t, 2 * (4 + 1)> expected{3, 2, 1, 4, 5, 8, 7, 6, 9, 10};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle(rawBitmap, 2u, 1u, 0u, 3u);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle24From012To210_InPLace)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, X, B, B, B, X
  std::array<uint8_t, 2 * (3 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8};
  //----------------------------------------------A, A, A, X, B, B, B, X
  const std::array<uint8_t, 2 * (3 + 1)> expected{3, 2, 1, 4, 7, 6, 5, 8};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle24From012To210(rawBitmap);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle24_InPlace_012To210)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, X, B, B, B, X
  std::array<uint8_t, 2 * (3 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8};
  //----------------------------------------------A, A, A, X, B, B, B, X
  const std::array<uint8_t, 2 * (3 + 1)> expected{3, 2, 1, 4, 7, 6, 5, 8};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle24(rawBitmap, 2u, 1u, 0u);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle24_InPlace_012To120)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, X, B, B, B, X
  std::array<uint8_t, 2 * (3 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8};
  //----------------------------------------------A, A, A, X, B, B, B, X
  const std::array<uint8_t, 2 * (3 + 1)> expected{2, 3, 1, 4, 6, 7, 5, 8};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle24(rawBitmap, 1u, 2u, 0u);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle32_InPlace)
{
}


TEST(TestBitmap_RawBitmapUtil, Swizzle32To8_InPlace_0)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, A, X, B, B, B, B, X
  std::array<uint8_t, 2 * (4 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, X, B, X
  const std::array<uint8_t, 2 * (4 + 1)> expected{1, 2, 6, 7};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle32To8(rawBitmap, PixelFormat::R8_UNORM, 2u, 0);

  // As we don't have requirements about the '1 byte extra stride area' we don't check it
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 0u, 1u));
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 2u, 1u));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle32To8_InPlace_1)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, A, X, B, B, B, B, X
  std::array<uint8_t, 2 * (4 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, X, B, X
  const std::array<uint8_t, 2 * (4 + 1)> expected{2, 2, 7, 7};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle32To8(rawBitmap, PixelFormat::R8_UNORM, 2u, 1);

  // As we don't have requirements about the '1 byte extra stride area' we don't check it
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 0u, 1u));
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 2u, 1u));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle32To8_InPlace_2)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, A, X, B, B, B, B, X
  std::array<uint8_t, 2 * (4 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, X, B, X
  const std::array<uint8_t, 2 * (4 + 1)> expected{3, 2, 8, 7};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle32To8(rawBitmap, PixelFormat::R8_UNORM, 2u, 2);

  // As we don't have requirements about the '1 byte extra stride area' we don't check it
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 0u, 1u));
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 2u, 1u));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle32To8_InPlace_3)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, A, X, B, B, B, B, X
  std::array<uint8_t, 2 * (4 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, X, B, X
  const std::array<uint8_t, 2 * (4 + 1)> expected{4, 2, 9, 7};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle32To8(rawBitmap, PixelFormat::R8_UNORM, 2u, 3);

  // As we don't have requirements about the '1 byte extra stride area' we don't check it
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 0u, 1u));
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 2u, 1u));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle32To24_InPlace_012)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, A, X, B, B, B, B, X
  std::array<uint8_t, 2 * (4 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, A, A, X, B, B, B, X
  const std::array<uint8_t, 2 * (4 + 1)> expected{1, 2, 3, 0, 6, 7, 8, 0};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle32To24(rawBitmap, PixelFormat::R8G8B8_UNORM, 4u, 0, 1, 2);

  // As we don't have requirements about the '1 byte extra stride area' we don't check it
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 0u, 3u));
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 4u, 3u));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle32To24_InPlace_120)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, A, X, B, B, B, B, X
  std::array<uint8_t, 2 * (4 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, A, A, X, B, B, B
  const std::array<uint8_t, 2 * (4 + 1)> expected{2, 3, 1, 0, 7, 8, 6};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle32To24(rawBitmap, PixelFormat::R8G8B8_UNORM, 4u, 1, 2, 0);

  // As we don't have requirements about the '1 byte extra stride area' we don't check it
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 0u, 3u));
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 4u, 3u));
}


TEST(TestBitmap_RawBitmapUtil, Average24To8_InPlace_000)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, X, B, B, B, X
  std::array<uint8_t, 2 * (3 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8};
  //----------------------------------------------A, X, B
  const std::array<uint8_t, 2 * (3 + 1)> expected{1, 0, 5};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Average24To8(rawBitmap, PixelFormat::R8_UNORM, 2u, 0, 0, 0);

  // As we don't have requirements about the '1 byte extra stride area' we don't check it
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 0u, 1u));
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 2u, 1u));
}


TEST(TestBitmap_RawBitmapUtil, Average24To8_InPlace_111)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, X, B, B, B, X
  std::array<uint8_t, 2 * (3 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8};
  //----------------------------------------------A, X, B
  const std::array<uint8_t, 2 * (3 + 1)> expected{2, 0, 6};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Average24To8(rawBitmap, PixelFormat::R8_UNORM, 2u, 1, 1, 1);

  // As we don't have requirements about the '1 byte extra stride area' we don't check it
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 0u, 1u));
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 2u, 1u));
}


TEST(TestBitmap_RawBitmapUtil, Average24To8_InPlace_222)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, X, B, B, B, X
  std::array<uint8_t, 2 * (3 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8};
  //----------------------------------------------A, X, B
  const std::array<uint8_t, 2 * (3 + 1)> expected{3, 0, 7};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Average24To8(rawBitmap, PixelFormat::R8_UNORM, 2u, 2, 2, 2);

  // As we don't have requirements about the '1 byte extra stride area' we don't check it
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 0u, 1u));
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 2u, 1u));
}


TEST(TestBitmap_RawBitmapUtil, Average24To8_InPlace_012)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, X, B, B, B, X
  std::array<uint8_t, 2 * (3 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8};
  //----------------------------------------------A, X, B
  const std::array<uint8_t, 2 * (3 + 1)> expected{2, 0, 6};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Average24To8(rawBitmap, PixelFormat::R8_UNORM, 2u, 0, 1, 2);

  // As we don't have requirements about the '1 byte extra stride area' we don't check it
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 0u, 1u));
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 2u, 1u));
}


TEST(TestBitmap_RawBitmapUtil, Average32To8_InPlace_000)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, A, X, B, B, B, B, X
  std::array<uint8_t, 2 * (4 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, X, B
  const std::array<uint8_t, 2 * (4 + 1)> expected{1, 0, 6};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Average32To8(rawBitmap, PixelFormat::R8_UNORM, 2u, 0, 0, 0);

  // As we don't have requirements about the '1 byte extra stride area' we don't check it
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 0u, 1u));
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 2u, 1u));
}


TEST(TestBitmap_RawBitmapUtil, Average32To8_InPlace_111)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, A, X, B, B, B, B, X
  std::array<uint8_t, 2 * (4 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, X, B
  const std::array<uint8_t, 2 * (4 + 1)> expected{2, 0, 7};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Average32To8(rawBitmap, PixelFormat::R8_UNORM, 2u, 1, 1, 1);

  // As we don't have requirements about the '1 byte extra stride area' we don't check it
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 0u, 1u));
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 2u, 1u));
}


TEST(TestBitmap_RawBitmapUtil, Average32To8_InPlace_222)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, A, X, B, B, B, B, X
  std::array<uint8_t, 2 * (4 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, X, B
  const std::array<uint8_t, 2 * (4 + 1)> expected{3, 0, 8};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Average32To8(rawBitmap, PixelFormat::R8_UNORM, 2u, 2, 2, 2);

  // As we don't have requirements about the '1 byte extra stride area' we don't check it
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 0u, 1u));
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 2u, 1u));
}


TEST(TestBitmap_RawBitmapUtil, Average32To8_InPlace_333)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, A, X, B, B, B, B, X
  std::array<uint8_t, 2 * (4 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, X, B
  const std::array<uint8_t, 2 * (4 + 1)> expected{4, 0, 9};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Average32To8(rawBitmap, PixelFormat::R8_UNORM, 2u, 3, 3, 3);

  // As we don't have requirements about the '1 byte extra stride area' we don't check it
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 0u, 1u));
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 2u, 1u));
}


TEST(TestBitmap_RawBitmapUtil, Average32To8_InPlace_012)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, A, X, B, B, B, B, X
  std::array<uint8_t, 2 * (4 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, X, B
  const std::array<uint8_t, 2 * (4 + 1)> expected{2, 0, 7};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Average32To8(rawBitmap, PixelFormat::R8_UNORM, 2u, 0, 1, 2);

  // As we don't have requirements about the '1 byte extra stride area' we don't check it
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 0u, 1u));
  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data, 2u, 1u));
}


TEST(TestBitmap_RawBitmapUtil, ClearPadding)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, A, X, B, B, B, B, X
  std::array<uint8_t, 2 * (4 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, A, A, A, X, B, B, B, B, X
  const std::array<uint8_t, 2 * (4 + 1)> expected{1, 2, 3, 4, 0, 6, 7, 8, 9, 0};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::ClearPadding(rawBitmap);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data));
}


TEST(TestBitmap_RawBitmapUtil, FlipHorizontal)
{
  // 1X2 with one byte skipped in stride
  //------------------------------------A, A, A, A, X, B, B, B, B, X
  std::array<uint8_t, 2 * (4 + 1)> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------B, B, B, B, X, A, A, A, A, X
  const std::array<uint8_t, 2 * (4 + 1)> expected{6, 7, 8, 9, 5, 1, 2, 3, 4, 10};
  RawBitmapEx rawBitmap(data.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::FlipHorizontal(rawBitmap);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, data));
}


TEST(TestBitmap_RawBitmapUtil, MemoryCopy_32_SameStride)
{
  // 1X2 with one byte skipped in stride
  //-----------------------------------------A, A, A, A, X, B, B, B, B, X
  const std::array<uint8_t, 2 * (4 + 1)> src{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  const RawBitmap srcRawBitmap(src.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  std::array<uint8_t, 2 * (4 + 1)> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::MemoryCopy(dstRawBitmap, srcRawBitmap);

  //----------------------------------------------A, A, A, A, X, B, B, B, B, X
  const std::array<uint8_t, 2 * (4 + 1)> expected{1, 2, 3, 4, 0, 6, 7, 8, 9, 0};

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}


TEST(TestBitmap_RawBitmapUtil, MemoryCopy_32_DifferentStride)
{
  // 1X2 with one byte skipped in stride
  //-----------------------------------------A, A, A, A, X, B, B, B, B, X
  const std::array<uint8_t, 2 * (4 + 1)> src{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  const RawBitmap srcRawBitmap(src.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  std::array<uint8_t, 2 * 4> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 4u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::MemoryCopy(dstRawBitmap, srcRawBitmap);

  //----------------------------------------A, A, A, A, B, B, B, B
  const std::array<uint8_t, 2 * 4> expected{1, 2, 3, 4, 6, 7, 8, 9};

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle24From012To210_SrcToDst_SameStride)
{
  // 1X2 with one byte skipped in stride
  //-----------------------------------------A, A, A, X, B, B, B, X
  const std::array<uint8_t, 2 * (3 + 1)> src{1, 2, 3, 4, 5, 6, 7, 8};
  //----------------------------------------------A, A, A, X, B, B, B, X
  const std::array<uint8_t, 2 * (3 + 1)> expected{3, 2, 1, 0, 7, 6, 5, 0};
  RawBitmap rawBitmap(src.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  std::array<uint8_t, 2 * (3 + 1)> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle24From012To210(dstRawBitmap, rawBitmap);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle24From012To210_SrcToDst_DifferentStride)
{
  // 1X2 with one byte skipped in stride
  //-----------------------------------------A, A, A, X, B, B, B, X
  const std::array<uint8_t, 2 * (3 + 1)> src{1, 2, 3, 4, 5, 6, 7, 8};
  //----------------------------------------A, A, A, B, B, B
  const std::array<uint8_t, 2 * 3> expected{3, 2, 1, 7, 6, 5};
  RawBitmap rawBitmap(src.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  std::array<uint8_t, 2 * 3> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 3u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle24From012To210(dstRawBitmap, rawBitmap);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle24_SrcToDst)
{
}


TEST(TestBitmap_RawBitmapUtil, Swizzle32_SrcToDst)
{
}


TEST(TestBitmap_RawBitmapUtil, Swizzle32To24_SrcToDst_012)
{
  // 1X2 with one byte skipped in stride
  //-----------------------------------------A, A, A, A, X, B, B, B, B, X
  const std::array<uint8_t, 2 * (4 + 1)> src{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, A, A, X, B, B, B, X
  const std::array<uint8_t, 2 * (4 + 1)> expected{1, 2, 3, 0, 6, 7, 8, 0};
  const RawBitmap srcRawBitmap(src.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  std::array<uint8_t, 2 * (4 + 1)> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle32To24(dstRawBitmap, srcRawBitmap, 0, 1, 2);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle32To24_SrcToDst_120)
{
  // 1X2 with one byte skipped in stride
  //-----------------------------------------A, A, A, A, X, B, B, B, B, X
  const std::array<uint8_t, 2 * (4 + 1)> src{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  //----------------------------------------------A, A, A, X, B, B, B, X
  const std::array<uint8_t, 2 * (4 + 1)> expected{2, 3, 1, 0, 7, 8, 6, 0};
  const RawBitmap srcRawBitmap(src.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  std::array<uint8_t, 2 * (4 + 1)> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle32To24(dstRawBitmap, srcRawBitmap, 1, 2, 0);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle24To32_SrcToDst_012)
{
  // 1X2 with one byte skipped in stride
  //-----------------------------------------A, A, A, X, B, B, B, X
  const std::array<uint8_t, 2 * (3 + 1)> src{1, 2, 3, 4, 5, 6, 7, 8};
  //----------------------------------------------A, A, A, A,  X, B, B, B, B,  X
  const std::array<uint8_t, 2 * (4 + 1)> expected{1, 2, 3, 10, 0, 5, 6, 7, 10, 0};
  const RawBitmap srcRawBitmap(src.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  std::array<uint8_t, 2 * (4 + 1)> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle24To32(dstRawBitmap, srcRawBitmap, 0, 1, 2, 3, 10u);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}


TEST(TestBitmap_RawBitmapUtil, Swizzle24To32_SrcToDst_1230)
{
  // 1X2 with one byte skipped in stride
  //-----------------------------------------A, A, A, X, B, B, B, X
  const std::array<uint8_t, 2 * (3 + 1)> src{1, 2, 3, 4, 5, 6, 7, 8};
  //----------------------------------------------A, A, A, A,  X, B, B, B, B,  X
  const std::array<uint8_t, 2 * (4 + 1)> expected{10, 1, 2, 3, 0, 10, 5, 6, 7, 0};
  const RawBitmap srcRawBitmap(src.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  std::array<uint8_t, 2 * (4 + 1)> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Swizzle24To32(dstRawBitmap, srcRawBitmap, 1, 2, 3, 0, 10u);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}


TEST(TestBitmap_RawBitmapUtil, Average24To8_SrcToDst_000)
{
  // 1X2 with one byte skipped in stride
  //-----------------------------------------A, A, A, X, B, B, B, X
  const std::array<uint8_t, 2 * (3 + 1)> src{1, 2, 3, 4, 5, 6, 7, 8};
  //----------------------------------------------A, X, B
  const std::array<uint8_t, 2 * (3 + 1)> expected{1, 0, 5};
  const RawBitmap srcRawBitmap(src.data(), 1, 2, PixelFormat::R8G8B8_UNORM, 4u, BitmapOrigin::UpperLeft);

  std::array<uint8_t, 2 * (3 + 1)> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 1, 2, PixelFormat::R8_UNORM, 2u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Average24To8(dstRawBitmap, srcRawBitmap, 0, 0, 0);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}


TEST(TestBitmap_RawBitmapUtil, Expand1ByteToNBytes_SrcToDst)
{
  // 1X2 with one byte skipped in stride
  //-----------------------------------------A, X, B, X
  const std::array<uint8_t, 2 * (1 + 1)> src{1, 2, 3, 4};
  //----------------------------------------------A, A, A, A, X, B, B, B, B, X
  const std::array<uint8_t, 2 * (4 + 1)> expected{1, 1, 1, 1, 0, 3, 3, 3, 3, 0};
  const RawBitmap srcRawBitmap(src.data(), 1, 2, PixelFormat::R8_UNORM, 2u, BitmapOrigin::UpperLeft);

  std::array<uint8_t, 2 * (4 + 1)> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 5u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::Expand1ByteToNBytes(dstRawBitmap, srcRawBitmap);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}
