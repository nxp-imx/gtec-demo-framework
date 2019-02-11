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

#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogExtent2D.hpp>
#include <FslBase/Log/Math/LogExtent3D.hpp>
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslGraphics/Log/LogPixelFormat.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <FslGraphics/UnitTest/Helper/Render/NativeGraphicsTestImpl.hpp>

using namespace Fsl;

namespace
{
  using TestRender_AtlasTexture2D = TestFixtureFslGraphics;
}


TEST(TestRender_AtlasTexture2D, Construct_Default)
{
  AtlasTexture2D texture;

  EXPECT_FALSE(texture.IsValid());
  EXPECT_EQ(Point2(0, 0), texture.GetAtlasSize());
  EXPECT_EQ(Point2(0, 0), texture.GetSize());
  EXPECT_EQ(AtlasTextureInfo(), texture.GetInfo());
  EXPECT_EQ(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_EQ(Texture2D(), texture.GetAtlasTexture());
}


TEST(TestRender_AtlasTexture2D, Construct_Invalid)
{
  const Texture2D atlasTex;
  const AtlasTextureInfo atlasTexInfo;

  AtlasTexture2D texture(atlasTex, atlasTexInfo);

  EXPECT_FALSE(texture.IsValid());
  EXPECT_EQ(Point2(0, 0), texture.GetAtlasSize());
  EXPECT_EQ(Point2(0, 0), texture.GetSize());
  EXPECT_EQ(AtlasTextureInfo(), texture.GetInfo());
  EXPECT_EQ(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_EQ(Texture2D(), texture.GetAtlasTexture());
}


TEST(TestRender_AtlasTexture2D, Construct)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();

  const Bitmap atlasBitmap(Extent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  const Texture2D atlasTex(nativeGraphics, atlasBitmap, Texture2DFilterHint::Smooth);
  const AtlasTextureInfo atlasTexInfo(Rectangle(0, 0, 128, 128), Rectangle(64, 64, 1, 1));

  AtlasTexture2D texture(atlasTex, atlasTexInfo);

  EXPECT_TRUE(texture.IsValid());
  EXPECT_EQ(atlasTex.GetSize(), texture.GetAtlasSize());
  EXPECT_EQ(atlasTexInfo.OriginalSize, texture.GetSize());
  EXPECT_EQ(atlasTexInfo, texture.GetInfo());
  EXPECT_EQ(atlasTex.GetNative(), texture.TryGetNative());
  EXPECT_EQ(atlasTex, texture.GetAtlasTexture());
}


TEST(TestRender_AtlasTexture2D, Reset_Empty)
{
  AtlasTexture2D texture;
  EXPECT_FALSE(texture.IsValid());
  texture.Reset();

  EXPECT_FALSE(texture.IsValid());
  EXPECT_EQ(Point2(0, 0), texture.GetAtlasSize());
  EXPECT_EQ(Point2(0, 0), texture.GetSize());
  EXPECT_EQ(AtlasTextureInfo(), texture.GetInfo());
  EXPECT_EQ(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_EQ(Texture2D(), texture.GetAtlasTexture());
}

TEST(TestRender_AtlasTexture2D, Reset_Existing)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();

  const Bitmap atlasBitmap(Extent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  const Texture2D atlasTex(nativeGraphics, atlasBitmap, Texture2DFilterHint::Smooth);
  const AtlasTextureInfo atlasTexInfo(Rectangle(0, 0, 128, 128), Rectangle(64, 64, 1, 1));

  AtlasTexture2D texture(atlasTex, atlasTexInfo);

  EXPECT_TRUE(texture.IsValid());

  texture.Reset();

  EXPECT_FALSE(texture.IsValid());
  EXPECT_EQ(Point2(0, 0), texture.GetAtlasSize());
  EXPECT_EQ(Point2(0, 0), texture.GetSize());
  EXPECT_EQ(AtlasTextureInfo(), texture.GetInfo());
  EXPECT_EQ(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_EQ(Texture2D(), texture.GetAtlasTexture());
}


TEST(TestRender_AtlasTexture2D, Reset_FromEmptyToNew)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();

  AtlasTexture2D texture;
  ASSERT_FALSE(texture.IsValid());

  const Bitmap atlasBitmap(Extent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  const Texture2D atlasTex(nativeGraphics, atlasBitmap, Texture2DFilterHint::Smooth);
  const AtlasTextureInfo atlasTexInfo(Rectangle(0, 0, 128, 128), Rectangle(64, 64, 1, 1));

  texture.Reset(atlasTex, atlasTexInfo);

  EXPECT_TRUE(texture.IsValid());
  EXPECT_EQ(atlasTex.GetSize(), texture.GetAtlasSize());
  EXPECT_EQ(atlasTexInfo.OriginalSize, texture.GetSize());
  EXPECT_EQ(atlasTexInfo, texture.GetInfo());
  EXPECT_EQ(atlasTex.GetNative(), texture.TryGetNative());
  EXPECT_EQ(atlasTex, texture.GetAtlasTexture());
}


TEST(TestRender_AtlasTexture2D, Reset_FromExistingToNew)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();

  const Bitmap atlasBitmap1(Extent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  const Texture2D atlasTex1(nativeGraphics, atlasBitmap1, Texture2DFilterHint::Smooth);
  const AtlasTextureInfo atlasTexInfo1(Rectangle(0, 0, 128, 128), Rectangle(64, 64, 1, 1));

  AtlasTexture2D texture(atlasTex1, atlasTexInfo1);

  const Bitmap atlasBitmap2(Extent2D(64, 64), PixelFormat::R8G8B8_UNORM);
  const Texture2D atlasTex2(nativeGraphics, atlasBitmap2, Texture2DFilterHint::Nearest);
  const AtlasTextureInfo atlasTexInfo2(Rectangle(0, 0, 64, 64), Rectangle(32, 32, 2, 2));
  texture.Reset(atlasTex2, atlasTexInfo2);

  EXPECT_TRUE(texture.IsValid());
  EXPECT_EQ(atlasTex2.GetSize(), texture.GetAtlasSize());
  EXPECT_EQ(atlasTexInfo2.OriginalSize, texture.GetSize());
  EXPECT_EQ(atlasTexInfo2, texture.GetInfo());
  EXPECT_EQ(atlasTex2.GetNative(), texture.TryGetNative());
  EXPECT_EQ(atlasTex2, texture.GetAtlasTexture());
}
