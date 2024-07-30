/****************************************************************************************************************************************************
 * Copyright 2020, 2024 NXP
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

#include <FslBase/Log/Math/LogExtent2D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxExtent2D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxRectangleU16.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslGraphics/Log/LogNativeTextureArea.hpp>
#include <FslGraphics/Sprite/Info/BasicImageSpriteInfo.hpp>
#include <FslGraphics/Sprite/SpriteNativeAreaCalc.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <FslGraphics/UnitTest/Helper/Sprite/Material/Test/SpriteMaterialImpl.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using TestSprite_BasicImageSpriteInfo = TestFixtureFslGraphics;
}


TEST(TestSprite_BasicImageSpriteInfo, Construct_Default)
{
  BasicImageSpriteInfo value;

  EXPECT_FALSE(value.MaterialInfo.IsValid());
  EXPECT_EQ(PxExtent2D(), value.ImageInfo.ExtentPx);
  EXPECT_EQ(PxRectangleU16(), value.ImageInfo.RectanglePx);
  EXPECT_EQ(DpExtent2D(), value.ImageInfo.ExtentDp);
  EXPECT_EQ(SpriteDpConfig::BaseDpi, value.ImageDpi);
  EXPECT_EQ(NativeTextureArea(), value.RenderInfo.TextureArea);
  EXPECT_EQ(PxSize2D(), value.RenderInfo.ScaledSizePx);
}

TEST(TestSprite_BasicImageSpriteInfo, Construct_InvalidMaterial)
{
  SpriteNativeAreaCalc area(false);

  SpriteMaterialInfo invalidMaterial;
  constexpr auto ImageRectanglePx = PxRectangleU16::Create(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi;
  constexpr StringViewLite DebugName("hello world");

  EXPECT_THROW(BasicImageSpriteInfo(area, invalidMaterial, ImageRectanglePx, imageDpi, DebugName), std::invalid_argument);
}


TEST(TestSprite_BasicImageSpriteInfo, Construct)
{
  SpriteNativeAreaCalc area(false);
  constexpr auto TextureExtent = PxExtent2D::Create(32, 64);
  constexpr SpriteMaterialId SpriteMaterialId(1);
  auto material = std::make_shared<SpriteMaterialImpl>(SpriteMaterialId, TextureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(SpriteMaterialId, TextureExtent, false, BasicPrimitiveTopology::TriangleList, material);
  constexpr auto ImageRectanglePx = PxRectangleU16::Create(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi;
  constexpr StringViewLite DebugName("hello world");
  constexpr auto NativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(TextureExtent, ImageRectanglePx);

  BasicImageSpriteInfo value(area, spriteMaterialInfo, ImageRectanglePx, imageDpi, DebugName);

  EXPECT_EQ(spriteMaterialInfo, value.MaterialInfo);
  EXPECT_EQ(ImageRectanglePx.GetExtent(), value.ImageInfo.ExtentPx);
  EXPECT_EQ(ImageRectanglePx, value.ImageInfo.RectanglePx);
  EXPECT_EQ(DpExtent2D::Create(ImageRectanglePx.GetExtent().Width.Value, ImageRectanglePx.GetExtent().Height.Value), value.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, value.ImageDpi);
  EXPECT_EQ(NativeTextureArea, value.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(ImageRectanglePx.GetExtent()), value.RenderInfo.ScaledSizePx);
}


TEST(TestSprite_BasicImageSpriteInfo, Construct2X)
{
  SpriteNativeAreaCalc area(false);
  constexpr auto TextureExtent = PxExtent2D::Create(32, 64);
  constexpr SpriteMaterialId SpriteMaterialId(1);
  auto material = std::make_shared<SpriteMaterialImpl>(SpriteMaterialId, TextureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(SpriteMaterialId, TextureExtent, false, BasicPrimitiveTopology::TriangleList, material);
  constexpr auto ImageRectanglePx = PxRectangleU16::Create(10, 20, 12, 28);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi * 2;
  constexpr StringViewLite DebugName("hello world");
  constexpr auto NativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(TextureExtent, ImageRectanglePx);

  BasicImageSpriteInfo value(area, spriteMaterialInfo, ImageRectanglePx, imageDpi, DebugName);

  const DpExtent2D extentDp = SpriteUnitConverter::CalcImageDpExtent2D(ImageRectanglePx.GetExtent(), imageDpi);

  EXPECT_EQ(spriteMaterialInfo, value.MaterialInfo);
  EXPECT_EQ(ImageRectanglePx.GetExtent(), value.ImageInfo.ExtentPx);
  EXPECT_EQ(ImageRectanglePx, value.ImageInfo.RectanglePx);
  EXPECT_EQ(extentDp, value.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, value.ImageDpi);
  EXPECT_EQ(NativeTextureArea, value.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(ImageRectanglePx.GetExtent()), value.RenderInfo.ScaledSizePx);
}


TEST(TestSprite_BasicImageSpriteInfo, Construct_NotCompatible)
{
  SpriteNativeAreaCalc area(false);
  constexpr auto TextureExtent = PxExtent2D::Create(32, 64);
  constexpr SpriteMaterialId SpriteMaterialId(1);
  auto material = std::make_shared<SpriteMaterialImpl>(SpriteMaterialId, TextureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(SpriteMaterialId, TextureExtent, false, BasicPrimitiveTopology::TriangleList, material);
  constexpr auto ImageRectanglePx = PxRectangleU16::Create(10, 20, 12, 25);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi * 2;
  constexpr StringViewLite DebugName("hello world");

  BasicImageSpriteInfo value(area, spriteMaterialInfo, ImageRectanglePx, imageDpi, DebugName);

  const DpExtent2D extentDp = SpriteUnitConverter::CalcImageDpExtent2D(ImageRectanglePx.GetExtent(), imageDpi);
  constexpr auto NativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(TextureExtent, ImageRectanglePx);

  EXPECT_EQ(spriteMaterialInfo, value.MaterialInfo);
  EXPECT_EQ(ImageRectanglePx.GetExtent(), value.ImageInfo.ExtentPx);
  EXPECT_EQ(ImageRectanglePx, value.ImageInfo.RectanglePx);
  EXPECT_EQ(extentDp, value.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, value.ImageDpi);
  EXPECT_EQ(NativeTextureArea, value.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(ImageRectanglePx.GetExtent()), value.RenderInfo.ScaledSizePx);
}
