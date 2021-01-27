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

TEST(TestBitmap_RawBitmapUtil, DownscaleNearest_Scale_16x1)
{
  // 1X2 with four bytes skipped in stride
  const std::array<uint8_t, 16 * 4> src{
    // A,    A,    A,    A,    B,    B,    B,    B,    C,    C,    C,    C,    D,    D,    D,    D
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
    0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  };

  //----------------------------------------------A, A, A, A, X, X, X, X, B, B,  B,  B,  X, X, X, X
  const std::array<uint8_t, 8 * 4> expected{
    0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b, 0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1a, 0x1b,
    0x20, 0x21, 0x22, 0x23, 0x28, 0x29, 0x2a, 0x2b, 0x30, 0x31, 0x32, 0x33, 0x38, 0x39, 0x3a, 0x3b,
  };

  const RawBitmap srcRawBitmap(src.data(), 16, 1, PixelFormat::R8G8B8A8_UNORM, 16 * 4, BitmapOrigin::UpperLeft);

  std::array<uint8_t, 8 * 4> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 8, 1, PixelFormat::R8G8B8A8_UNORM, 8 * 4, BitmapOrigin::UpperLeft);

  RawBitmapUtil::DownscaleNearest(dstRawBitmap, srcRawBitmap);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}

TEST(TestBitmap_RawBitmapUtil, DownscaleNearest32_NoScale_1x2)
{
  // 1X2 with four bytes skipped in stride
  //-----------------------------------------A, A, A, A, X, X, X, X, B, B,  B,  B,  X,  X,  X,  X
  const std::array<uint8_t, 2 * (4 + 4)> src{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

  //----------------------------------------------A, A, A, A, X, X, X, X, B, B,  B,  B,  X, X, X, X
  const std::array<uint8_t, 2 * (4 + 4)> expected{1, 2, 3, 4, 0, 0, 0, 0, 9, 10, 11, 12, 0, 0, 0, 0};
  const RawBitmap srcRawBitmap(src.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 8u, BitmapOrigin::UpperLeft);

  std::array<uint8_t, 2 * (4 + 4)> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 8u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmap);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}

TEST(TestBitmap_RawBitmapUtil, DownscaleNearest32_NoScale_4x4)
{
  // 1X2 with four bytes skipped in stride
  const std::array<uint8_t, 16 * 4> src{
    // A,    A,    A,    A,    B,    B,    B,    B,    C,    C,    C,    C,    D,    D,    D,    D
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
    0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  };

  //----------------------------------------------A, A, A, A, X, X, X, X, B, B,  B,  B,  X, X, X, X
  const std::array<uint8_t, 16 * 4> expected{
    // A,    A,    A,    A,    B,    B,    B,    B,    C,    C,    C,    C,    D,    D,    D,    D
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
    0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  };

  const RawBitmap srcRawBitmap(src.data(), 4, 4, PixelFormat::R8G8B8A8_UNORM, 16u, BitmapOrigin::UpperLeft);

  std::array<uint8_t, 16 * 4> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 4, 4, PixelFormat::R8G8B8A8_UNORM, 16u, BitmapOrigin::UpperLeft);

  RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmap);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}


TEST(TestBitmap_RawBitmapUtil, DownscaleNearest32_Scale_16x1)
{
  // 1X2 with four bytes skipped in stride
  const std::array<uint8_t, 16 * 4> src{
    // A,    A,    A,    A,    B,    B,    B,    B,    C,    C,    C,    C,    D,    D,    D,    D
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
    0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  };

  //----------------------------------------------A, A, A, A, X, X, X, X, B, B,  B,  B,  X, X, X, X
  const std::array<uint8_t, 8 * 4> expected{
    0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b, 0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1a, 0x1b,
    0x20, 0x21, 0x22, 0x23, 0x28, 0x29, 0x2a, 0x2b, 0x30, 0x31, 0x32, 0x33, 0x38, 0x39, 0x3a, 0x3b,
  };

  const RawBitmap srcRawBitmap(src.data(), 16, 1, PixelFormat::R8G8B8A8_UNORM, 16 * 4, BitmapOrigin::UpperLeft);

  std::array<uint8_t, 8 * 4> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 8, 1, PixelFormat::R8G8B8A8_UNORM, 8 * 4, BitmapOrigin::UpperLeft);

  RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmap);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}


TEST(TestBitmap_RawBitmapUtil, DownscaleNearest32_Scale_1x16)
{
  // 1X2 with four bytes skipped in stride
  const std::array<uint8_t, 16 * 4> src{
    // A,    A,    A,    A,    B,    B,    B,    B,    C,    C,    C,    C,    D,    D,    D,    D
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
    0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  };

  //----------------------------------------------A, A, A, A, X, X, X, X, B, B,  B,  B,  X, X, X, X
  const std::array<uint8_t, 8 * 4> expected{
    0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b, 0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1a, 0x1b,
    0x20, 0x21, 0x22, 0x23, 0x28, 0x29, 0x2a, 0x2b, 0x30, 0x31, 0x32, 0x33, 0x38, 0x39, 0x3a, 0x3b,
  };

  const RawBitmap srcRawBitmap(src.data(), 1, 16, PixelFormat::R8G8B8A8_UNORM, 1 * 4, BitmapOrigin::UpperLeft);

  std::array<uint8_t, 8 * 4> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 1, 8, PixelFormat::R8G8B8A8_UNORM, 1 * 4, BitmapOrigin::UpperLeft);

  RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmap);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}


TEST(TestBitmap_RawBitmapUtil, DownscaleNearest32_Scale_4x4)
{
  // 1X2 with four bytes skipped in stride
  const std::array<uint8_t, 16 * 4> src{
    // A,    A,    A,    A,    B,    B,    B,    B,    C,    C,    C,    C,    D,    D,    D,    D
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
    0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  };

  //----------------------------------------------A, A, A, A, X, X, X, X, B, B,  B,  B,  X, X, X, X
  const std::array<uint8_t, (2 * 2) * 4> expected{
    // A,    A,    A,    A,    B,    B,    B,    B,    C,    C,    C,    C,    D,    D,    D,    D
    0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b, 0x20, 0x21, 0x22, 0x23, 0x28, 0x29, 0x2a, 0x2b,
  };

  const RawBitmap srcRawBitmap(src.data(), 4, 4, PixelFormat::R8G8B8A8_UNORM, 4 * 4, BitmapOrigin::UpperLeft);

  std::array<uint8_t, (2 * 2) * 4> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 2, 2, PixelFormat::R8G8B8A8_UNORM, 2 * 4, BitmapOrigin::UpperLeft);

  RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmap);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}


TEST(TestBitmap_RawBitmapUtil, DownscaleNearest32_Scale_4x4ToZero)
{
  const std::array<uint8_t, 16 * 4> src{
    // A,    A,    A,    A,    B,    B,    B,    B,    C,    C,    C,    C,    D,    D,    D,    D
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
    0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  };

  //----------------------------------------------A, A, A, A, X, X, X, X, B, B,  B,  B,  X, X, X, X
  const std::array<uint8_t, 1> expected{};

  const RawBitmap srcRawBitmap(src.data(), 4, 4, PixelFormat::R8G8B8A8_UNORM, 4 * 4, BitmapOrigin::UpperLeft);

  // We use size 1 arrays here to ensure that we dont get a null ptr from data()
  std::array<uint8_t, 1> dst{};
  RawBitmapEx dstRawBitmap(dst.data(), 0, 0, PixelFormat::R8G8B8A8_UNORM, 0, BitmapOrigin::UpperLeft);

  RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmap);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
}

TEST(TestBitmap_RawBitmapUtil, DownscaleNearest32_Unsupported)
{
  // 1X2 with four bytes skipped in stride
  const std::array<uint8_t, 1 * 4> src{};

  const RawBitmap srcRawBitmap(src.data(), 1, 1, PixelFormat::R8G8B8A8_UNORM, 4, BitmapOrigin::UpperLeft);

  // We use size 1 arrays here to ensure that we dont get a null ptr from data()
  std::array<uint8_t, (4 * 4) * 4> dst{};

  {    // Upscale X
    RawBitmapEx dstRawBitmap(dst.data(), 2, 1, PixelFormat::R8G8B8A8_UNORM, 8, BitmapOrigin::UpperLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmap), std::invalid_argument);
  }
  {    // Upscale Y
    RawBitmapEx dstRawBitmap(dst.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 4, BitmapOrigin::UpperLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmap), std::invalid_argument);
  }
  {    // Mismatched origin
    RawBitmapEx dstRawBitmap(dst.data(), 1, 1, PixelFormat::R8G8B8A8_UNORM, 4, BitmapOrigin::LowerLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmap), std::invalid_argument);
  }
  {    // Mismatched pixel format
    RawBitmapEx dstRawBitmap(dst.data(), 1, 1, PixelFormat::R8G8B8_UNORM, 4, BitmapOrigin::UpperLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmap), std::invalid_argument);
  }
  {    // invalid src
    RawBitmap srcRawBitmapInvalid;
    RawBitmapEx dstRawBitmap(dst.data(), 1, 1, PixelFormat::R8G8B8A8_UNORM, 8, BitmapOrigin::UpperLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmapInvalid), std::invalid_argument);
  }
  {    // invalid dst
    RawBitmapEx dstRawBitmap;
    EXPECT_THROW(RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmap), std::invalid_argument);
  }
  {    // invalid dst stride
    RawBitmapEx dstRawBitmap(dst.data(), 1, 1, PixelFormat::R8G8B8A8_UNORM, 5, BitmapOrigin::UpperLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmap), NotSupportedException);
  }
  {    // invalid src stride
    RawBitmapEx srcRawBitmapInvalid(dst.data(), 1, 1, PixelFormat::R8G8B8A8_UNORM, 5, BitmapOrigin::UpperLeft);
    RawBitmapEx dstRawBitmap(dst.data(), 1, 1, PixelFormat::R8G8B8A8_UNORM, 4, BitmapOrigin::UpperLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmapInvalid), NotSupportedException);
  }
}

TEST(TestBitmap_RawBitmapUtil, DownscaleNearest32_WidthExceeded)
{
  const std::vector<uint8_t> src(0x10000 * 4);

  const RawBitmap srcRawBitmap(src.data(), 0x10000, 1, PixelFormat::R8G8B8A8_UNORM, 0x10000 * 4, BitmapOrigin::UpperLeft);

  // We use size 1 arrays here to ensure that we dont get a null ptr from data()
  std::vector<uint8_t> dst(0x10000 * 4);

  RawBitmapEx dstRawBitmap(dst.data(), 0x10000, 1, PixelFormat::R8G8B8A8_UNORM, 0x10000 * 4, BitmapOrigin::UpperLeft);
  EXPECT_THROW(RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmap), NotSupportedException);
}

TEST(TestBitmap_RawBitmapUtil, DownscaleNearest32_HeightExceeded)
{
  const std::vector<uint8_t> src(0x10000 * 4);

  const RawBitmap srcRawBitmap(src.data(), 1, 0x10000, PixelFormat::R8G8B8A8_UNORM, 4, BitmapOrigin::UpperLeft);

  // We use size 1 arrays here to ensure that we dont get a null ptr from data()
  std::vector<uint8_t> dst(0x10000 * 4);

  RawBitmapEx dstRawBitmap(dst.data(), 1, 0x10000, PixelFormat::R8G8B8A8_UNORM, 4, BitmapOrigin::UpperLeft);
  EXPECT_THROW(RawBitmapUtil::DownscaleNearest32(dstRawBitmap, srcRawBitmap), NotSupportedException);
}


TEST(TestBitmap_RawBitmapUtil, DownscaleBoxFilter32_Unsupported)
{
  // 1X2 with four bytes skipped in stride
  const std::array<uint8_t, (4 * 4) * 4> src{};

  const RawBitmap srcRawBitmap(src.data(), 4, 4, PixelFormat::R8G8B8A8_UNORM, 4 * 4, BitmapOrigin::UpperLeft);

  // We use size 1 arrays here to ensure that we dont get a null ptr from data()
  std::array<uint8_t, (2 * 2) * 4> dst{};

  {    // Invalid src width
    const RawBitmap srcRawBitmap2(src.data(), 5, 4, PixelFormat::R8G8B8A8_UNORM, 5 * 4, BitmapOrigin::UpperLeft);
    RawBitmapEx dstRawBitmap(dst.data(), 2, 2, PixelFormat::R8G8B8A8_UNORM, 2 * 4, BitmapOrigin::UpperLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleBoxFilter32(dstRawBitmap, srcRawBitmap2), std::invalid_argument);
  }

  {    // Invalid width
    RawBitmapEx dstRawBitmap(dst.data(), 3, 2, PixelFormat::R8G8B8A8_UNORM, 3 * 4, BitmapOrigin::UpperLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleBoxFilter32(dstRawBitmap, srcRawBitmap), std::invalid_argument);
  }
  {    // Invalid Height
    RawBitmapEx dstRawBitmap(dst.data(), 2, 3, PixelFormat::R8G8B8A8_UNORM, 2 * 4, BitmapOrigin::UpperLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleBoxFilter32(dstRawBitmap, srcRawBitmap), std::invalid_argument);
  }
  {    // Mismatched origin
    RawBitmapEx dstRawBitmap(dst.data(), 2, 2, PixelFormat::R8G8B8A8_UNORM, 2 * 4, BitmapOrigin::LowerLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleBoxFilter32(dstRawBitmap, srcRawBitmap), std::invalid_argument);
  }
  {    // Mismatched pixel format
    RawBitmapEx dstRawBitmap(dst.data(), 2, 2, PixelFormat::R8G8B8_UNORM, 2 * 4, BitmapOrigin::UpperLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleBoxFilter32(dstRawBitmap, srcRawBitmap), std::invalid_argument);
  }
  {    // Mismatched pixel channels
    const RawBitmap srcRawBitmap2(src.data(), 4, 4, PixelFormat::R16G16_SFLOAT, 4 * 4, BitmapOrigin::UpperLeft);
    RawBitmapEx dstRawBitmap(dst.data(), 2, 2, PixelFormat::R16G16_SFLOAT, 2 * 4, BitmapOrigin::UpperLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleBoxFilter32(dstRawBitmap, srcRawBitmap2), std::invalid_argument);
  }
  {    // invalid src
    RawBitmap srcRawBitmapInvalid;
    RawBitmapEx dstRawBitmap(dst.data(), 1, 1, PixelFormat::R8G8B8A8_UNORM, 4, BitmapOrigin::UpperLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleBoxFilter32(dstRawBitmap, srcRawBitmapInvalid), std::invalid_argument);
  }
  {    // invalid dst
    RawBitmapEx dstRawBitmap;
    EXPECT_THROW(RawBitmapUtil::DownscaleBoxFilter32(dstRawBitmap, srcRawBitmap), std::invalid_argument);
  }
  {    // invalid dst stride
    RawBitmapEx dstRawBitmap(dst.data(), 2, 2, PixelFormat::R8G8B8A8_UNORM, (2 * 4) + 1, BitmapOrigin::UpperLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleBoxFilter32(dstRawBitmap, srcRawBitmap), NotSupportedException);
  }
  {    // invalid src stride
    RawBitmapEx srcRawBitmapInvalid(dst.data(), 4, 4, PixelFormat::R8G8B8A8_UNORM, (4 * 4) + 1, BitmapOrigin::UpperLeft);
    RawBitmapEx dstRawBitmap(dst.data(), 2, 2, PixelFormat::R8G8B8A8_UNORM, 2 * 4, BitmapOrigin::UpperLeft);
    EXPECT_THROW(RawBitmapUtil::DownscaleBoxFilter32(dstRawBitmap, srcRawBitmapInvalid), NotSupportedException);
  }
}

TEST(TestBitmap_RawBitmapUtil, DownscaleBoxFilter32_From1x1)
{
  const std::array<uint8_t, (1 * 1) * 4> src{0x80, 0x60, 0x40, 0x20};

  const RawBitmap srcRawBitmap(src.data(), 1, 1, PixelFormat::R8G8B8A8_UNORM, 1 * 4, BitmapOrigin::UpperLeft);

  // We use size 1 arrays here to ensure that we dont get a null ptr from data()
  std::array<uint8_t, (1 * 1) * 4> dst{};

  {
    RawBitmapEx dstRawBitmap(dst.data(), 0, 0, PixelFormat::R8G8B8A8_UNORM, 0 * 4, BitmapOrigin::UpperLeft);
    RawBitmapUtil::DownscaleBoxFilter32(dstRawBitmap, srcRawBitmap);
  }
}


TEST(TestBitmap_RawBitmapUtil, DownscaleBoxFilter32_From2x2A)
{
  //                                            A,    A,    A,    A,    B,    B,    B,    B,    C,    C,    C,    C,    D,    D,    D,    D
  const std::array<uint8_t, (2 * 2) * 4> src{0xFF, 0x00, 0x00, 0x00, 0x00, 0xEF, 0x00, 0x00, 0x00, 0x00, 0xDF, 0x00, 0x00, 0x00, 0x00, 0xCF};

  const RawBitmap srcRawBitmap(src.data(), 2, 2, PixelFormat::R8G8B8A8_UNORM, 2 * 4, BitmapOrigin::UpperLeft);

  // We use size 1 arrays here to ensure that we dont get a null ptr from data()
  std::array<uint8_t, (1 * 1) * 4> dst{};

  {
    RawBitmapEx dstRawBitmap(dst.data(), 1, 1, PixelFormat::R8G8B8A8_UNORM, 1 * 4, BitmapOrigin::UpperLeft);
    RawBitmapUtil::DownscaleBoxFilter32(dstRawBitmap, srcRawBitmap);

    //                                                 A,    A,    A,    A
    const std::array<uint8_t, (1 * 1) * 4> expected{0x3F, 0x3B, 0x37, 0x33};

    EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
  }
}

TEST(TestBitmap_RawBitmapUtil, DownscaleBoxFilter32_From2x2B)
{
  //                                            A,    A,    A,    A,    B,    B,    B,    B,    C,    C,    C,    C,    D,    D,    D,    D
  const std::array<uint8_t, (2 * 2) * 4> src{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0};

  const RawBitmap srcRawBitmap(src.data(), 2, 2, PixelFormat::R8G8B8A8_UNORM, 2 * 4, BitmapOrigin::UpperLeft);

  // We use size 1 arrays here to ensure that we dont get a null ptr from data()
  std::array<uint8_t, (1 * 1) * 4> dst{};

  {
    RawBitmapEx dstRawBitmap(dst.data(), 1, 1, PixelFormat::R8G8B8A8_UNORM, 1 * 4, BitmapOrigin::UpperLeft);
    RawBitmapUtil::DownscaleBoxFilter32(dstRawBitmap, srcRawBitmap);

    const std::array<uint8_t, (1 * 1) * 4> expected{(0x00 + 0x40 + 0x80 + 0xC0) / 4, (0x10 + 0x50 + 0x90 + 0xD0) / 4, (0x20 + 0x60 + 0xA0 + 0xE0) / 4,
                                                    (0x30 + 0x70 + 0xB0 + 0xF0) / 4};

    EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
  }
}


TEST(TestBitmap_RawBitmapUtil, DownscaleBoxFilter32_From4x2)
{
  //                                            A,    A,    A,    A,    B,    B,    B,    B,    C,    C,    C,    C,    D,    D,    D,    D,
  const std::array<uint8_t, (4 * 2) * 4> src{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0,
                                             0xE8, 0xD8, 0xC8, 0xB8, 0xA8, 0x98, 0x88, 0x78, 0x68, 0x58, 0x48, 0x38, 0x28, 0x18, 0x08, 0x03};

  const RawBitmap srcRawBitmap(src.data(), 4, 2, PixelFormat::R8G8B8A8_UNORM, 4 * 4, BitmapOrigin::UpperLeft);

  // We use size 1 arrays here to ensure that we dont get a null ptr from data()
  std::array<uint8_t, (2 * 1) * 4> dst{};

  {
    RawBitmapEx dstRawBitmap(dst.data(), 2, 1, PixelFormat::R8G8B8A8_UNORM, 2 * 4, BitmapOrigin::UpperLeft);
    RawBitmapUtil::DownscaleBoxFilter32(dstRawBitmap, srcRawBitmap);

    const std::array<uint8_t, (2 * 1) * 4> expected{(0x00 + 0x40 + 0xE8 + 0xA8) / 4, (0x10 + 0x50 + 0xD8 + 0x98) / 4, (0x20 + 0x60 + 0xC8 + 0x88) / 4,
                                                    (0x30 + 0x70 + 0xB8 + 0x78) / 4, (0x80 + 0xC0 + 0x68 + 0x28) / 4, (0x90 + 0xD0 + 0x58 + 0x18) / 4,
                                                    (0xA0 + 0xE0 + 0x48 + 0x08) / 4, (0xB0 + 0xF0 + 0x38 + 0x03) / 4};

    EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
  }
}


TEST(TestBitmap_RawBitmapUtil, DownscaleBoxFilter32_From2x4)
{
  //                                            A,    A,    A,    A,    B,    B,    B,    B,
  const std::array<uint8_t, (2 * 4) * 4> src{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0,
                                             0xE8, 0xD8, 0xC8, 0xB8, 0xA8, 0x98, 0x88, 0x78, 0x68, 0x58, 0x48, 0x38, 0x28, 0x18, 0x08, 0x03};

  const RawBitmap srcRawBitmap(src.data(), 2, 4, PixelFormat::R8G8B8A8_UNORM, 2 * 4, BitmapOrigin::UpperLeft);

  // We use size 1 arrays here to ensure that we dont get a null ptr from data()
  std::array<uint8_t, (1 * 2) * 4> dst{};

  {
    RawBitmapEx dstRawBitmap(dst.data(), 1, 2, PixelFormat::R8G8B8A8_UNORM, 1 * 4, BitmapOrigin::UpperLeft);
    RawBitmapUtil::DownscaleBoxFilter32(dstRawBitmap, srcRawBitmap);

    const std::array<uint8_t, (1 * 2) * 4> expected{(0x00 + 0x40 + 0x80 + 0xC0) / 4, (0x10 + 0x50 + 0x90 + 0xD0) / 4, (0x20 + 0x60 + 0xA0 + 0xE0) / 4,
                                                    (0x30 + 0x70 + 0xB0 + 0xF0) / 4, (0xE8 + 0xA8 + 0x68 + 0x28) / 4, (0xD8 + 0x98 + 0x58 + 0x18) / 4,
                                                    (0xC8 + 0x88 + 0x48 + 0x08) / 4, (0xB8 + 0x78 + 0x38 + 0x03) / 4};

    EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(expected, dst));
  }
}
