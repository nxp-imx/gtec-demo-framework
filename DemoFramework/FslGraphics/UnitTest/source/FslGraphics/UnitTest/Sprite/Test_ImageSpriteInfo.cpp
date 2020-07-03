/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslGraphics/Sprite/Info/ImageSpriteInfo.hpp>
#include <FslBase/Log/Math/LogExtent2D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxExtent2D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxRectangleU.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslGraphics/Log/LogNativeTextureArea.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <FslGraphics/UnitTest/Helper/Sprite/Material/Test/SpriteMaterialImpl.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using TestSprite_ImageSpriteInfo = TestFixtureFslGraphics;
}


TEST(TestSprite_ImageSpriteInfo, Construct_Default)
{
  ImageSpriteInfo value;

  EXPECT_FALSE(value.MaterialInfo.IsValid());
  EXPECT_EQ(PxExtent2D(), value.ImageInfo.ExtentPx);
  EXPECT_EQ(PxRectangleU(), value.ImageInfo.TrimmedRectanglePx);
  EXPECT_EQ(DpExtent(), value.ImageInfo.ExtentDp);
  EXPECT_EQ(SpriteDpConfig::BaseDpi, value.ImageDpi);
  EXPECT_EQ(NativeTextureArea(), value.RenderInfo.TextureArea);
  EXPECT_EQ(PxSize2D(), value.RenderInfo.ScaledSizePx);
}

TEST(TestSprite_ImageSpriteInfo, Construct_InvalidMaterial)
{
  const SpriteMaterialInfo invalidMaterial;
  constexpr PxThicknessU imageTrimMarginPx(1, 2, 3, 4);
  constexpr PxRectangleU imageTrimmedRectanglePx(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi;
  constexpr StringViewLite debugName("hello world");

  EXPECT_THROW(ImageSpriteInfo(invalidMaterial, imageTrimMarginPx, imageTrimmedRectanglePx, imageDpi, debugName), std::invalid_argument);
}


TEST(TestSprite_ImageSpriteInfo, Construct)
{
  constexpr PxExtent2D textureExtent(32, 64);
  constexpr SpriteMaterialId spriteMaterialid(1);
  auto material = std::make_shared<SpriteMaterialImpl>(spriteMaterialid, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialid, textureExtent, false, material, 127u);
  constexpr PxThicknessU imageTrimMarginPx(1, 2, 3, 4);
  constexpr PxRectangleU imageTrimmedRectanglePx(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi;
  constexpr StringViewLite debugName("hello world");
  constexpr auto nativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(textureExtent, imageTrimmedRectanglePx);

  ImageSpriteInfo value(spriteMaterialInfo, imageTrimMarginPx, imageTrimmedRectanglePx, imageDpi, debugName);

  constexpr const auto imageExtent = imageTrimMarginPx.Sum() + imageTrimmedRectanglePx.GetExtent();

  EXPECT_EQ(spriteMaterialInfo, value.MaterialInfo);
  EXPECT_EQ(imageTrimMarginPx, value.ImageInfo.TrimMarginPx);
  EXPECT_EQ(imageExtent, value.ImageInfo.ExtentPx);
  EXPECT_EQ(imageTrimmedRectanglePx, value.ImageInfo.TrimmedRectanglePx);
  EXPECT_EQ(DpExtent(imageExtent.Width, imageExtent.Height), value.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, value.ImageDpi);
  EXPECT_EQ(nativeTextureArea, value.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(imageExtent), value.RenderInfo.ScaledSizePx);
}


TEST(TestSprite_ImageSpriteInfo, Construct2X)
{
  constexpr PxExtent2D textureExtent(32, 64);
  constexpr SpriteMaterialId spriteMaterialid(1);
  auto material = std::make_shared<SpriteMaterialImpl>(spriteMaterialid, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialid, textureExtent, false, material, 127u);
  constexpr PxThicknessU imageTrimMarginPx(1, 2, 3, 4);
  constexpr PxRectangleU imageTrimmedRectanglePx(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi * 2;
  constexpr StringViewLite debugName("hello world");

  ImageSpriteInfo value(spriteMaterialInfo, imageTrimMarginPx, imageTrimmedRectanglePx, imageDpi, debugName);

  constexpr auto nativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(textureExtent, imageTrimmedRectanglePx);
  constexpr const auto imageExtent = imageTrimMarginPx.Sum() + imageTrimmedRectanglePx.GetExtent();

  const DpExtent extentDp = SpriteUnitConverter::CalcImageDpExtent(imageExtent, imageDpi);

  EXPECT_EQ(spriteMaterialInfo, value.MaterialInfo);
  EXPECT_EQ(imageTrimMarginPx, value.ImageInfo.TrimMarginPx);
  EXPECT_EQ(imageExtent, value.ImageInfo.ExtentPx);
  EXPECT_EQ(imageTrimmedRectanglePx, value.ImageInfo.TrimmedRectanglePx);
  EXPECT_EQ(extentDp, value.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, value.ImageDpi);
  EXPECT_EQ(nativeTextureArea, value.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(imageExtent), value.RenderInfo.ScaledSizePx);
}


TEST(TestSprite_ImageSpriteInfo, Construct_NotCompatible)
{
  constexpr PxExtent2D textureExtent(32, 64);
  constexpr SpriteMaterialId spriteMaterialid(1);
  auto material = std::make_shared<SpriteMaterialImpl>(spriteMaterialid, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialid, textureExtent, false, material, 127u);
  constexpr PxThicknessU imageTrimMarginPx(1, 2, 3, 4);
  constexpr PxRectangleU imageTrimmedRectanglePx(10, 20, 12, 25);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi * 2;
  constexpr StringViewLite debugName("hello world");

  ImageSpriteInfo value(spriteMaterialInfo, imageTrimMarginPx, imageTrimmedRectanglePx, imageDpi, debugName);

  constexpr auto nativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(textureExtent, imageTrimmedRectanglePx);
  constexpr const auto imageExtent = imageTrimMarginPx.Sum() + imageTrimmedRectanglePx.GetExtent();
  const DpExtent extentDp = SpriteUnitConverter::CalcImageDpExtent(imageExtent, imageDpi);

  EXPECT_EQ(spriteMaterialInfo, value.MaterialInfo);
  EXPECT_EQ(imageTrimMarginPx, value.ImageInfo.TrimMarginPx);
  EXPECT_EQ(imageExtent, value.ImageInfo.ExtentPx);
  EXPECT_EQ(imageTrimmedRectanglePx, value.ImageInfo.TrimmedRectanglePx);
  EXPECT_EQ(extentDp, value.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, value.ImageDpi);
  EXPECT_EQ(nativeTextureArea, value.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(imageExtent), value.RenderInfo.ScaledSizePx);
}
