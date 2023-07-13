/****************************************************************************************************************************************************
 * Copyright 2018, 2023 NXP
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
#include <FslGraphics/Log/LogPixelFormat.hpp>
#include <FslGraphics/Log/LogStrideRequirement.hpp>
#include <FslGraphics/Texture/TextureMipMapUtil.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <cassert>

using namespace Fsl;

namespace
{
  using TestTexture_TextureMipMapUtil = TestFixtureFslGraphics;

  uint32_t GetR8G8B8A8Pixel(const Texture& texture, const uint32_t level, const uint32_t face, const uint32_t layer, const PxPoint2& point)
  {
    const uint32_t r = texture.GetUInt8(level, face, layer, (point.X.Value * 4) + 0, point.Y.Value, 0, false);
    const uint32_t g = texture.GetUInt8(level, face, layer, (point.X.Value * 4) + 1, point.Y.Value, 0, false);
    const uint32_t b = texture.GetUInt8(level, face, layer, (point.X.Value * 4) + 2, point.Y.Value, 0, false);
    const uint32_t a = texture.GetUInt8(level, face, layer, (point.X.Value * 4) + 3, point.Y.Value, 0, false);
    const uint32_t result = r | (g << 8) | (b << 16) | (a << 24);
    return result;
  }

  uint32_t BoxFilterChannel(const uint32_t pixelColor00, const uint32_t pixelColor10, const uint32_t pixelColor01, const uint32_t pixelColor11)
  {
    assert(pixelColor00 <= 255);
    assert(pixelColor01 <= 255);
    assert(pixelColor10 <= 255);
    assert(pixelColor11 <= 255);
    return (pixelColor00 + pixelColor10 + pixelColor01 + pixelColor11) / 4;
  }

  uint32_t BoxFilter(const uint32_t pixelColor00, const uint32_t pixelColor10, const uint32_t pixelColor01, const uint32_t pixelColor11)
  {
    auto c0 = BoxFilterChannel((pixelColor00 >> 24) & 0xFF, (pixelColor10 >> 24) & 0xFF, (pixelColor01 >> 24) & 0xFF, +(pixelColor11 >> 24) & 0xFF);
    auto c1 = BoxFilterChannel((pixelColor00 >> 16) & 0xFF, (pixelColor10 >> 16) & 0xFF, (pixelColor01 >> 16) & 0xFF, +(pixelColor11 >> 16) & 0xFF);
    auto c2 = BoxFilterChannel((pixelColor00 >> 8) & 0xFF, (pixelColor10 >> 8) & 0xFF, (pixelColor01 >> 8) & 0xFF, +(pixelColor11 >> 8) & 0xFF);
    auto c3 = BoxFilterChannel(pixelColor00 & 0xFF, pixelColor10 & 0xFF, pixelColor01 & 0xFF, pixelColor11 & 0xFF);
    return (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;
  }
}

TEST(TestTexture_TextureMipMapUtil, GenerateMipMaps_From1X1Bitmap_Box)
{
  const uint32_t pixelColor = 0x80604020;
  Bitmap src(1, 1, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft);
  src.SetNativePixel(0, 0, pixelColor);
  Texture result = TextureMipMapUtil::GenerateMipMaps(src, TextureMipMapFilter::Box);

  EXPECT_EQ(1u, result.GetLayers());
  EXPECT_EQ(1u, result.GetLevels());
  EXPECT_EQ(1u, result.GetFaces());
  EXPECT_EQ(1u, result.GetBlobCount());
  EXPECT_EQ(src.GetOrigin(), result.GetBitmapOrigin());
  EXPECT_EQ(src.GetPixelFormat(), result.GetPixelFormat());
  EXPECT_EQ(src.GetExtent(), result.GetExtent2D());
  EXPECT_EQ(PxValueU(1u), result.GetExtent().Depth);

  // Check mip0
  EXPECT_EQ(pixelColor, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(0, 0)));
}

TEST(TestTexture_TextureMipMapUtil, GenerateMipMaps_From1X1Bitmap_Nearest)
{
  const uint32_t pixelColor = 0x80604020;
  Bitmap src(1, 1, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft);
  src.SetNativePixel(0, 0, pixelColor);
  Texture result = TextureMipMapUtil::GenerateMipMaps(src, TextureMipMapFilter::Nearest);

  EXPECT_EQ(1u, result.GetLayers());
  EXPECT_EQ(1u, result.GetLevels());
  EXPECT_EQ(1u, result.GetFaces());
  EXPECT_EQ(1u, result.GetBlobCount());
  EXPECT_EQ(src.GetOrigin(), result.GetBitmapOrigin());
  EXPECT_EQ(src.GetPixelFormat(), result.GetPixelFormat());
  EXPECT_EQ(src.GetExtent(), result.GetExtent2D());
  EXPECT_EQ(PxValueU(1u), result.GetExtent().Depth);

  // Check mip0
  EXPECT_EQ(pixelColor, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(0, 0)));
}

TEST(TestTexture_TextureMipMapUtil, GenerateMipMaps_From2X2Bitmap_Box)
{
  const uint32_t pixelColor00 = 0x80604020;
  const uint32_t pixelColor10 = 0x10203040;
  const uint32_t pixelColor01 = 0x0F0C0804;
  const uint32_t pixelColor11 = 0x01020304;
  Bitmap src(2, 2, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft);
  src.SetNativePixel(0, 0, pixelColor00);
  src.SetNativePixel(1, 0, pixelColor10);
  src.SetNativePixel(0, 1, pixelColor01);
  src.SetNativePixel(1, 1, pixelColor11);
  Texture result = TextureMipMapUtil::GenerateMipMaps(src, TextureMipMapFilter::Box);

  EXPECT_EQ(1u, result.GetLayers());
  EXPECT_EQ(2u, result.GetLevels());
  EXPECT_EQ(1u, result.GetFaces());
  EXPECT_EQ(2u, result.GetBlobCount());
  EXPECT_EQ(src.GetOrigin(), result.GetBitmapOrigin());
  EXPECT_EQ(src.GetPixelFormat(), result.GetPixelFormat());
  EXPECT_EQ(src.GetExtent(), result.GetExtent2D());
  EXPECT_EQ(PxValueU(1u), result.GetExtent().Depth);

  // Check mip0
  EXPECT_EQ(pixelColor00, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(0, 0)));
  EXPECT_EQ(pixelColor10, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(1, 0)));
  EXPECT_EQ(pixelColor01, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(0, 1)));
  EXPECT_EQ(pixelColor11, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(1, 1)));
  // Check mip1
  const uint32_t mip1Color00 = BoxFilter(pixelColor00, pixelColor10, pixelColor01, pixelColor11);
  EXPECT_EQ(mip1Color00, GetR8G8B8A8Pixel(result, 1, 0, 0, PxPoint2::Create(0, 0)));
}


TEST(TestTexture_TextureMipMapUtil, GenerateMipMaps_From4X4Bitmap_Box)
{
  const uint32_t pixelColor00 = 0xf42e6509;
  const uint32_t pixelColor10 = 0x6dfe5395;
  const uint32_t pixelColor20 = 0x70ac04a3;
  const uint32_t pixelColor30 = 0xa63a1bab;
  const uint32_t pixelColor01 = 0xc9d3c9c6;
  const uint32_t pixelColor11 = 0xc5a48157;
  const uint32_t pixelColor21 = 0xee6a01d1;
  const uint32_t pixelColor31 = 0x7a492673;
  const uint32_t pixelColor02 = 0xe9a7953d;
  const uint32_t pixelColor12 = 0xf2158d79;
  const uint32_t pixelColor22 = 0x560ce99f;
  const uint32_t pixelColor32 = 0x015a7a90;
  const uint32_t pixelColor03 = 0xe401e75a;
  const uint32_t pixelColor13 = 0xb56d92db;
  const uint32_t pixelColor23 = 0x278cd25d;
  const uint32_t pixelColor33 = 0x4d091056;

  Bitmap src(4, 4, PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft);
  src.SetNativePixel(0, 0, pixelColor00);
  src.SetNativePixel(1, 0, pixelColor10);
  src.SetNativePixel(2, 0, pixelColor20);
  src.SetNativePixel(3, 0, pixelColor30);
  src.SetNativePixel(0, 1, pixelColor01);
  src.SetNativePixel(1, 1, pixelColor11);
  src.SetNativePixel(2, 1, pixelColor21);
  src.SetNativePixel(3, 1, pixelColor31);
  src.SetNativePixel(0, 2, pixelColor02);
  src.SetNativePixel(1, 2, pixelColor12);
  src.SetNativePixel(2, 2, pixelColor22);
  src.SetNativePixel(3, 2, pixelColor32);
  src.SetNativePixel(0, 3, pixelColor03);
  src.SetNativePixel(1, 3, pixelColor13);
  src.SetNativePixel(2, 3, pixelColor23);
  src.SetNativePixel(3, 3, pixelColor33);
  Texture result = TextureMipMapUtil::GenerateMipMaps(src, TextureMipMapFilter::Box);

  EXPECT_EQ(1u, result.GetLayers());
  EXPECT_EQ(3u, result.GetLevels());
  EXPECT_EQ(1u, result.GetFaces());
  EXPECT_EQ(3u, result.GetBlobCount());
  EXPECT_EQ(src.GetOrigin(), result.GetBitmapOrigin());
  EXPECT_EQ(src.GetPixelFormat(), result.GetPixelFormat());
  EXPECT_EQ(src.GetExtent(), result.GetExtent2D());
  EXPECT_EQ(PxValueU(1u), result.GetExtent().Depth);

  // Check mip0
  EXPECT_EQ(pixelColor00, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(0, 0)));
  EXPECT_EQ(pixelColor10, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(1, 0)));
  EXPECT_EQ(pixelColor20, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(2, 0)));
  EXPECT_EQ(pixelColor30, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(3, 0)));
  EXPECT_EQ(pixelColor01, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(0, 1)));
  EXPECT_EQ(pixelColor11, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(1, 1)));
  EXPECT_EQ(pixelColor21, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(2, 1)));
  EXPECT_EQ(pixelColor31, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(3, 1)));
  EXPECT_EQ(pixelColor02, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(0, 2)));
  EXPECT_EQ(pixelColor12, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(1, 2)));
  EXPECT_EQ(pixelColor22, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(2, 2)));
  EXPECT_EQ(pixelColor32, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(3, 2)));
  EXPECT_EQ(pixelColor03, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(0, 3)));
  EXPECT_EQ(pixelColor13, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(1, 3)));
  EXPECT_EQ(pixelColor23, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(2, 3)));
  EXPECT_EQ(pixelColor33, GetR8G8B8A8Pixel(result, 0, 0, 0, PxPoint2::Create(3, 3)));

  // Check mip1
  const uint32_t mip1Color00 = BoxFilter(pixelColor00, pixelColor10, pixelColor01, pixelColor11);
  const uint32_t mip1Color10 = BoxFilter(pixelColor20, pixelColor30, pixelColor21, pixelColor31);
  const uint32_t mip1Color01 = BoxFilter(pixelColor02, pixelColor12, pixelColor03, pixelColor13);
  const uint32_t mip1Color11 = BoxFilter(pixelColor22, pixelColor32, pixelColor23, pixelColor33);
  EXPECT_EQ(mip1Color00, GetR8G8B8A8Pixel(result, 1, 0, 0, PxPoint2::Create(0, 0)));
  EXPECT_EQ(mip1Color10, GetR8G8B8A8Pixel(result, 1, 0, 0, PxPoint2::Create(1, 0)));
  EXPECT_EQ(mip1Color01, GetR8G8B8A8Pixel(result, 1, 0, 0, PxPoint2::Create(0, 1)));
  EXPECT_EQ(mip1Color11, GetR8G8B8A8Pixel(result, 1, 0, 0, PxPoint2::Create(1, 1)));

  // Check mip2
  const uint32_t mip2Color00 = BoxFilter(mip1Color00, mip1Color10, mip1Color01, mip1Color11);
  EXPECT_EQ(mip2Color00, GetR8G8B8A8Pixel(result, 2, 0, 0, PxPoint2::Create(0, 0)));
}
