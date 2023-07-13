/****************************************************************************************************************************************************
 * Copyright 2020, 2023 NXP
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
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/SpriteDpConfig.hpp>
#include <FslGraphics/Sprite/SpriteNativeAreaCalc.hpp>
#include <FslGraphics/UnitTest/Helper/Sprite/Material/Test/SpriteMaterialImpl.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using TestSprite_ImageSprite = TestFixtureFslGraphics;
}


TEST(TestSprite_ImageSprite, Construct_Default)
{
  ImageSprite value;

  const auto& info = value.GetInfo();

  EXPECT_FALSE(info.MaterialInfo.IsValid());
  EXPECT_EQ(PxThicknessU(), info.ImageInfo.TrimMarginPx);
  EXPECT_EQ(PxExtent2D(), info.ImageInfo.ExtentPx);
  EXPECT_EQ(PxRectangleU16(), info.ImageInfo.TrimmedRectanglePx);
  EXPECT_EQ(DpExtent2D(), info.ImageInfo.ExtentDp);
  EXPECT_EQ(SpriteDpConfig::BaseDpi, info.ImageDpi);
  EXPECT_EQ(NativeTextureArea(), info.RenderInfo.TextureArea);
  EXPECT_EQ(PxSize2D(), info.RenderInfo.ScaledSizePx);
}


TEST(TestSprite_ImageSprite, Construct_Default_SetContent)
{
  SpriteNativeAreaCalc area(false);
  ImageSprite value;

  {
    const auto& info = value.GetInfo();
    EXPECT_FALSE(info.MaterialInfo.IsValid());
    EXPECT_EQ(PxThicknessU(), info.ImageInfo.TrimMarginPx);
    EXPECT_EQ(PxExtent2D(), info.ImageInfo.ExtentPx);
    EXPECT_EQ(PxRectangleU16(), info.ImageInfo.TrimmedRectanglePx);
    EXPECT_EQ(DpExtent2D(), info.ImageInfo.ExtentDp);
    EXPECT_EQ(SpriteDpConfig::BaseDpi, info.ImageDpi);
    EXPECT_EQ(NativeTextureArea(), info.RenderInfo.TextureArea);
    EXPECT_EQ(PxSize2D(), info.RenderInfo.ScaledSizePx);
  }

  constexpr auto textureExtent = PxExtent2D::Create(32, 64);
  constexpr SpriteMaterialId spriteMaterialId(1);
  auto material = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, false, material);
  constexpr auto imageTrimMarginPx = PxThicknessU::Create(1, 2, 3, 4);
  constexpr auto imageTrimmedRectanglePx = PxRectangleU16::Create(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi;
  constexpr StringViewLite debugName("hello world");
  constexpr uint32_t densityDpi = SpriteDpConfig::BaseDpi;
  constexpr auto nativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(textureExtent, imageTrimmedRectanglePx);

  value.SetContent(area, spriteMaterialInfo, imageTrimMarginPx, imageTrimmedRectanglePx, imageDpi, debugName, densityDpi);
  {
    constexpr const auto imageExtent = imageTrimMarginPx.Sum() + imageTrimmedRectanglePx.GetExtent();
    const auto& info = value.GetInfo();
    EXPECT_EQ(spriteMaterialInfo, info.MaterialInfo);
    EXPECT_EQ(imageTrimMarginPx, info.ImageInfo.TrimMarginPx);
    EXPECT_EQ(imageExtent, info.ImageInfo.ExtentPx);
    EXPECT_EQ(imageTrimmedRectanglePx, info.ImageInfo.TrimmedRectanglePx);
    EXPECT_EQ(DpExtent2D::Create(imageExtent.Width.Value, imageExtent.Height.Value), info.ImageInfo.ExtentDp);
    EXPECT_EQ(imageDpi, info.ImageDpi);
    EXPECT_EQ(nativeTextureArea, info.RenderInfo.TextureArea);
    EXPECT_EQ(TypeConverter::To<PxSize2D>(imageExtent), info.RenderInfo.ScaledSizePx);
  }
}


TEST(TestSprite_ImageSprite, Construct_InvalidMaterial)
{
  SpriteNativeAreaCalc area(false);

  SpriteMaterialInfo invalidMaterial;
  constexpr auto imageTrimMarginPx = PxThicknessU::Create(1, 2, 3, 4);
  constexpr auto imageTrimmedRectanglePx = PxRectangleU16::Create(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi;
  constexpr StringViewLite debugName("hello world");
  constexpr uint32_t densityDpi = SpriteDpConfig::BaseDpi * 2;

  EXPECT_THROW(ImageSprite(area, invalidMaterial, imageTrimMarginPx, imageTrimmedRectanglePx, imageDpi, debugName, densityDpi),
               std::invalid_argument);
}

TEST(TestSprite_ImageSprite, Construct_1x_1x)
{
  SpriteNativeAreaCalc area(false);

  constexpr auto textureExtent = PxExtent2D::Create(32, 64);
  constexpr SpriteMaterialId spriteMaterialId(1);
  auto material = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, false, material);
  constexpr auto imageTrimMarginPx = PxThicknessU::Create(1, 2, 3, 4);
  constexpr auto imageTrimmedRectanglePx = PxRectangleU16::Create(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi;
  constexpr StringViewLite debugName("hello world");
  constexpr uint32_t densityDpi = SpriteDpConfig::BaseDpi;
  constexpr auto nativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(textureExtent, imageTrimmedRectanglePx);

  ImageSprite value(area, spriteMaterialInfo, imageTrimMarginPx, imageTrimmedRectanglePx, imageDpi, debugName, densityDpi);

  constexpr const auto imageExtent = imageTrimMarginPx.Sum() + imageTrimmedRectanglePx.GetExtent();
  const auto& info = value.GetInfo();

  EXPECT_EQ(spriteMaterialInfo, info.MaterialInfo);
  EXPECT_EQ(imageTrimMarginPx, info.ImageInfo.TrimMarginPx);
  EXPECT_EQ(imageExtent, info.ImageInfo.ExtentPx);
  EXPECT_EQ(imageTrimmedRectanglePx, info.ImageInfo.TrimmedRectanglePx);
  EXPECT_EQ(DpExtent2D::Create(imageExtent.Width.Value, imageExtent.Height.Value), info.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, info.ImageDpi);
  EXPECT_EQ(nativeTextureArea, info.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(imageExtent), info.RenderInfo.ScaledSizePx);
}


TEST(TestSprite_ImageSprite, Construct_1x_2x)
{
  SpriteNativeAreaCalc area(false);
  constexpr auto textureExtent = PxExtent2D::Create(32, 64);
  constexpr SpriteMaterialId spriteMaterialId(1);
  auto material = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, false, material);
  constexpr auto imageTrimMarginPx = PxThicknessU::Create(1, 2, 3, 4);
  constexpr auto imageTrimmedRectanglePx = PxRectangleU16::Create(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi;
  constexpr StringViewLite debugName("hello world");
  constexpr uint32_t densityDpi = SpriteDpConfig::BaseDpi * 2;
  constexpr auto nativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(textureExtent, imageTrimmedRectanglePx);

  ImageSprite value(area, spriteMaterialInfo, imageTrimMarginPx, imageTrimmedRectanglePx, imageDpi, debugName, densityDpi);

  constexpr PxExtent2D imageExtent = imageTrimMarginPx.Sum() + imageTrimmedRectanglePx.GetExtent();
  const auto& info = value.GetInfo();

  EXPECT_EQ(spriteMaterialInfo, info.MaterialInfo);
  EXPECT_EQ(imageTrimMarginPx, info.ImageInfo.TrimMarginPx);
  EXPECT_EQ(imageExtent, info.ImageInfo.ExtentPx);
  EXPECT_EQ(imageTrimmedRectanglePx, info.ImageInfo.TrimmedRectanglePx);
  EXPECT_EQ(DpExtent2D::Create(imageExtent.Width.Value, imageExtent.Height.Value), info.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, info.ImageDpi);
  EXPECT_EQ(nativeTextureArea, info.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(imageExtent * PxValueU(2u)), info.RenderInfo.ScaledSizePx);
}

TEST(TestSprite_ImageSprite, Construct_2x_1x)
{
  SpriteNativeAreaCalc area(false);
  constexpr auto textureExtent = PxExtent2D::Create(32, 64);
  constexpr SpriteMaterialId spriteMaterialId(1);
  auto material = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, false, material);
  constexpr auto imageTrimMarginPx = PxThicknessU::Create(1, 2, 3, 4);
  constexpr auto imageTrimmedRectanglePx = PxRectangleU16::Create(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi * 2;
  constexpr StringViewLite debugName("hello world");
  constexpr uint32_t densityDpi = SpriteDpConfig::BaseDpi;
  constexpr auto nativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(textureExtent, imageTrimmedRectanglePx);

  ImageSprite value(area, spriteMaterialInfo, imageTrimMarginPx, imageTrimmedRectanglePx, imageDpi, debugName, densityDpi);

  constexpr const auto imageExtent = imageTrimMarginPx.Sum() + imageTrimmedRectanglePx.GetExtent();
  const auto& info = value.GetInfo();

  EXPECT_EQ(spriteMaterialInfo, info.MaterialInfo);
  EXPECT_EQ(imageTrimMarginPx, info.ImageInfo.TrimMarginPx);
  EXPECT_EQ(imageExtent, info.ImageInfo.ExtentPx);
  EXPECT_EQ(imageTrimmedRectanglePx, info.ImageInfo.TrimmedRectanglePx);
  EXPECT_EQ(DpExtent2D::Create(imageExtent.Width.Value / 2u, imageExtent.Height.Value / 2u), info.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, info.ImageDpi);
  EXPECT_EQ(nativeTextureArea, info.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(imageExtent / PxValueU(2u)), info.RenderInfo.ScaledSizePx);
}

TEST(TestSprite_ImageSprite, Construct_2x_2x)
{
  SpriteNativeAreaCalc area(false);
  constexpr auto textureExtent = PxExtent2D::Create(32, 64);
  constexpr SpriteMaterialId spriteMaterialId(1);
  auto material = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, false, material);
  constexpr auto imageTrimMarginPx = PxThicknessU::Create(1, 2, 3, 4);
  constexpr auto imageTrimmedRectanglePx = PxRectangleU16::Create(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi * 2;
  constexpr StringViewLite debugName("hello world");
  constexpr uint32_t densityDpi = SpriteDpConfig::BaseDpi * 2;
  constexpr auto nativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(textureExtent, imageTrimmedRectanglePx);

  ImageSprite value(area, spriteMaterialInfo, imageTrimMarginPx, imageTrimmedRectanglePx, imageDpi, debugName, densityDpi);

  constexpr const auto imageExtent = imageTrimMarginPx.Sum() + imageTrimmedRectanglePx.GetExtent();
  const auto& info = value.GetInfo();

  EXPECT_EQ(spriteMaterialInfo, info.MaterialInfo);
  EXPECT_EQ(imageTrimMarginPx, info.ImageInfo.TrimMarginPx);
  EXPECT_EQ(imageExtent, info.ImageInfo.ExtentPx);
  EXPECT_EQ(imageTrimmedRectanglePx, info.ImageInfo.TrimmedRectanglePx);
  EXPECT_EQ(DpExtent2D::Create(imageExtent.Width.Value / 2u, imageExtent.Height.Value / 2u), info.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, info.ImageDpi);
  EXPECT_EQ(nativeTextureArea, info.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(imageExtent), info.RenderInfo.ScaledSizePx);
}


TEST(TestSprite_ImageSprite, Resize_Default)
{
  ImageSprite value;
  EXPECT_NO_THROW(value.Resize(SpriteDpConfig::BaseDpi));
}

TEST(TestSprite_ImageSprite, Resize_2x)
{
  SpriteNativeAreaCalc area(false);
  constexpr auto textureExtent = PxExtent2D::Create(32, 64);
  constexpr SpriteMaterialId spriteMaterialId(1);
  auto material = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, false, material);
  constexpr auto imageTrimMarginPx = PxThicknessU::Create(1, 2, 3, 4);
  constexpr auto imageTrimmedRectanglePx = PxRectangleU16::Create(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi;
  constexpr StringViewLite debugName("hello world");
  constexpr uint32_t densityDpi = SpriteDpConfig::BaseDpi;
  constexpr auto nativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(textureExtent, imageTrimmedRectanglePx);

  ImageSprite value(area, spriteMaterialInfo, imageTrimMarginPx, imageTrimmedRectanglePx, imageDpi, debugName, densityDpi);

  EXPECT_NO_THROW(value.Resize(SpriteDpConfig::BaseDpi * 2));

  constexpr const auto imageExtent = imageTrimMarginPx.Sum() + imageTrimmedRectanglePx.GetExtent();
  const auto& info = value.GetInfo();

  EXPECT_EQ(spriteMaterialInfo, info.MaterialInfo);
  EXPECT_EQ(imageTrimMarginPx, info.ImageInfo.TrimMarginPx);
  EXPECT_EQ(imageExtent, info.ImageInfo.ExtentPx);
  EXPECT_EQ(imageTrimmedRectanglePx, info.ImageInfo.TrimmedRectanglePx);
  EXPECT_EQ(DpExtent2D::Create(imageExtent.Width.Value, imageExtent.Height.Value), info.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, info.ImageDpi);
  EXPECT_EQ(nativeTextureArea, info.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(imageExtent * PxValueU(2u)), info.RenderInfo.ScaledSizePx);
}
