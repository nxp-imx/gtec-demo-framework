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
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslGraphics/Bitmap/BitmapMemory.hpp>
#include <FslGraphics/Log/LogPixelFormat.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>
#include <vector>

using namespace Fsl;

namespace
{
  using TestBitmap_BitmapMemory = TestFixtureFslGraphics;

  void CompareSpans(const ReadOnlySpan<uint8_t> expected, const ReadOnlySpan<uint8_t> result)
  {
    ASSERT_EQ(expected.size(), result.size());
    for (std::size_t i = 0; i < result.size(); ++i)
    {
      EXPECT_EQ(result[i], expected[i]);
    }
  }
}


TEST(TestBitmap_BitmapMemory, Construct_Default)
{
  BitmapMemory bitmap;
}

TEST(TestBitmap_BitmapMemory, ConstructFromReadOnlySpan_Size)
{
  constexpr PxSize2D Size = PxSize2D::Create(2, 3);
  constexpr PixelFormat SrcPixelFormat = PixelFormat::R8G8B8_UNORM;
  constexpr BitmapOrigin Origin = BitmapOrigin::LowerLeft;
  const std::array<uint8_t, 3 * Size.RawWidth() * Size.RawHeight()> srcContent{0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x01, 0x11, 0x21,
                                                                               0x31, 0x41, 0x51, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52};

  BitmapMemory result(BitmapMemory::Create(SpanUtil::ToVector(SpanUtil::AsReadOnlySpan(srcContent)), Size, SrcPixelFormat, Origin));

  EXPECT_EQ(result.GetSize(), Size);
  EXPECT_EQ(result.GetPixelFormat(), SrcPixelFormat);
  EXPECT_EQ(result.GetOrigin(), Origin);
  EXPECT_EQ(result.GetBytesPerPixel(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat));
  EXPECT_EQ(result.Stride(), PixelFormatUtil::CalcMinimumStride(Size.Width(), PixelFormatUtil::GetBytesPerPixel(SrcPixelFormat)));

  std::vector<uint8_t> resultVector = result.Release();

  // Since the content is stored with the minimum stride we can just use a span to access the content

  CompareSpans(SpanUtil::AsReadOnlySpan(resultVector), SpanUtil::AsReadOnlySpan(srcContent));
}
