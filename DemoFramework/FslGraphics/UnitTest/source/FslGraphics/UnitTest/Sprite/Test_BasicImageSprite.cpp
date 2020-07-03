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

#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslBase/Log/Math/LogExtent2D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxExtent2D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxRectangleU.hpp>
#include <FslBase/Log/Math/Pixel/LogPxSize2D.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslGraphics/Log/LogNativeTextureArea.hpp>
#include <FslGraphics/Sprite/SpriteDpConfig.hpp>
#include <FslGraphics/UnitTest/Helper/Sprite/Material/Test/SpriteMaterialImpl.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using TestSprite_BasicImageSprite = TestFixtureFslGraphics;
}


TEST(TestSprite_BasicImageSprite, Construct_Default)
{
  BasicImageSprite value;

  const auto& info = value.GetInfo();

  EXPECT_FALSE(info.MaterialInfo.IsValid());
  EXPECT_EQ(PxExtent2D(), info.ImageInfo.ExtentPx);
  EXPECT_EQ(PxRectangleU(), info.ImageInfo.RectanglePx);
  EXPECT_EQ(DpExtent(), info.ImageInfo.ExtentDp);
  EXPECT_EQ(SpriteDpConfig::BaseDpi, info.ImageDpi);
  EXPECT_EQ(NativeTextureArea(), info.RenderInfo.TextureArea);
  EXPECT_EQ(PxSize2D(), info.RenderInfo.ScaledSizePx);
}


TEST(TestSprite_BasicImageSprite, Construct_Default_SetContent)
{
  BasicImageSprite value;
  {
    const auto& info = value.GetInfo();
    EXPECT_FALSE(info.MaterialInfo.IsValid());
    EXPECT_EQ(PxExtent2D(), info.ImageInfo.ExtentPx);
    EXPECT_EQ(PxRectangleU(), info.ImageInfo.RectanglePx);
    EXPECT_EQ(DpExtent(), info.ImageInfo.ExtentDp);
    EXPECT_EQ(SpriteDpConfig::BaseDpi, info.ImageDpi);
    EXPECT_EQ(NativeTextureArea(), info.RenderInfo.TextureArea);
    EXPECT_EQ(PxSize2D(), info.RenderInfo.ScaledSizePx);
  }

  constexpr PxExtent2D textureExtent(32, 64);
  constexpr SpriteMaterialId spriteMaterialId(1);
  auto material = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, false, material, 127u);
  constexpr PxRectangleU imageRectanglePx(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi;
  constexpr StringViewLite debugName("hello world");
  constexpr uint32_t densityDpi = SpriteDpConfig::BaseDpi;
  constexpr auto nativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(textureExtent, imageRectanglePx);

  value.SetContent(spriteMaterialInfo, imageRectanglePx, imageDpi, debugName, densityDpi);
  {
    const auto& info = value.GetInfo();
    EXPECT_EQ(spriteMaterialInfo, info.MaterialInfo);
    EXPECT_EQ(imageRectanglePx.GetExtent(), info.ImageInfo.ExtentPx);
    EXPECT_EQ(imageRectanglePx, info.ImageInfo.RectanglePx);
    EXPECT_EQ(DpExtent(imageRectanglePx.GetExtent().Width, imageRectanglePx.GetExtent().Height), info.ImageInfo.ExtentDp);
    EXPECT_EQ(imageDpi, info.ImageDpi);
    EXPECT_EQ(nativeTextureArea, info.RenderInfo.TextureArea);
    EXPECT_EQ(TypeConverter::To<PxSize2D>(imageRectanglePx.GetExtent()), info.RenderInfo.ScaledSizePx);
  }
}

TEST(TestSprite_BasicImageSprite, Construct_InvalidMaterial)
{
  SpriteMaterialInfo invalidMaterial;
  constexpr PxRectangleU imageRectanglePx(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi;
  constexpr StringViewLite debugName("hello world");
  constexpr uint32_t densityDpi = SpriteDpConfig::BaseDpi * 2;

  EXPECT_THROW(BasicImageSprite(invalidMaterial, imageRectanglePx, imageDpi, debugName, densityDpi), std::invalid_argument);
}

TEST(TestSprite_BasicImageSprite, Construct_1x_1x)
{
  constexpr PxExtent2D textureExtent(32, 64);
  constexpr SpriteMaterialId spriteMaterialId(1);
  auto material = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, false, material, 127u);
  constexpr PxRectangleU imageRectanglePx(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi;
  constexpr StringViewLite debugName("hello world");
  constexpr uint32_t densityDpi = SpriteDpConfig::BaseDpi;
  constexpr auto nativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(textureExtent, imageRectanglePx);

  BasicImageSprite value(spriteMaterialInfo, imageRectanglePx, imageDpi, debugName, densityDpi);
  const auto& info = value.GetInfo();

  EXPECT_EQ(spriteMaterialInfo, info.MaterialInfo);
  EXPECT_EQ(imageRectanglePx.GetExtent(), info.ImageInfo.ExtentPx);
  EXPECT_EQ(imageRectanglePx, info.ImageInfo.RectanglePx);
  EXPECT_EQ(DpExtent(imageRectanglePx.GetExtent().Width, imageRectanglePx.GetExtent().Height), info.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, info.ImageDpi);
  EXPECT_EQ(nativeTextureArea, info.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(imageRectanglePx.GetExtent()), info.RenderInfo.ScaledSizePx);
}


TEST(TestSprite_BasicImageSprite, Construct_1x_2x)
{
  constexpr PxExtent2D textureExtent(32, 64);
  constexpr SpriteMaterialId spriteMaterialId(1);
  auto material = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, false, material, 127u);
  constexpr PxRectangleU imageRectanglePx(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi;
  constexpr StringViewLite debugName("hello world");
  constexpr uint32_t densityDpi = SpriteDpConfig::BaseDpi * 2;

  constexpr auto nativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(textureExtent, imageRectanglePx);

  BasicImageSprite value(spriteMaterialInfo, imageRectanglePx, imageDpi, debugName, densityDpi);
  const auto& info = value.GetInfo();

  EXPECT_EQ(spriteMaterialInfo, info.MaterialInfo);
  EXPECT_EQ(imageRectanglePx.GetExtent(), info.ImageInfo.ExtentPx);
  EXPECT_EQ(imageRectanglePx, info.ImageInfo.RectanglePx);
  EXPECT_EQ(DpExtent(imageRectanglePx.GetExtent().Width, imageRectanglePx.GetExtent().Height), info.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, info.ImageDpi);
  EXPECT_EQ(nativeTextureArea, info.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(imageRectanglePx.GetExtent() * 2u), info.RenderInfo.ScaledSizePx);
}

TEST(TestSprite_BasicImageSprite, Construct_2x_1x)
{
  constexpr PxExtent2D textureExtent(32, 64);
  constexpr SpriteMaterialId spriteMaterialId(1);
  auto material = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, false, material, 127u);
  constexpr PxRectangleU imageRectanglePx(10, 20, 12, 28);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi * 2;
  constexpr StringViewLite debugName("hello world");
  constexpr uint32_t densityDpi = SpriteDpConfig::BaseDpi;
  constexpr auto nativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(textureExtent, imageRectanglePx);

  BasicImageSprite value(spriteMaterialInfo, imageRectanglePx, imageDpi, debugName, densityDpi);
  const auto& info = value.GetInfo();

  EXPECT_EQ(spriteMaterialInfo, info.MaterialInfo);
  EXPECT_EQ(imageRectanglePx.GetExtent(), info.ImageInfo.ExtentPx);
  EXPECT_EQ(imageRectanglePx, info.ImageInfo.RectanglePx);
  EXPECT_EQ(DpExtent(imageRectanglePx.GetExtent().Width / 2u, imageRectanglePx.GetExtent().Height / 2u), info.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, info.ImageDpi);
  EXPECT_EQ(nativeTextureArea, info.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(imageRectanglePx.GetExtent() / 2u), info.RenderInfo.ScaledSizePx);
}

TEST(TestSprite_BasicImageSprite, Construct_2x_2x)
{
  constexpr PxExtent2D textureExtent(32, 64);
  constexpr SpriteMaterialId spriteMaterialId(1);
  auto material = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, false, material, 127u);
  constexpr PxRectangleU imageRectanglePx(10, 20, 12, 28);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi * 2;
  constexpr StringViewLite debugName("hello world");
  constexpr uint32_t densityDpi = SpriteDpConfig::BaseDpi * 2;

  constexpr auto nativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(textureExtent, imageRectanglePx);

  BasicImageSprite value(spriteMaterialInfo, imageRectanglePx, imageDpi, debugName, densityDpi);
  const auto& info = value.GetInfo();

  EXPECT_EQ(spriteMaterialInfo, info.MaterialInfo);
  EXPECT_EQ(imageRectanglePx.GetExtent(), info.ImageInfo.ExtentPx);
  EXPECT_EQ(imageRectanglePx, info.ImageInfo.RectanglePx);
  EXPECT_EQ(DpExtent(imageRectanglePx.GetExtent().Width / 2u, imageRectanglePx.GetExtent().Height / 2u), info.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, info.ImageDpi);
  EXPECT_EQ(nativeTextureArea, info.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(imageRectanglePx.GetExtent()), info.RenderInfo.ScaledSizePx);
}


TEST(TestSprite_BasicImageSprite, Resize_Default)
{
  BasicImageSprite value;
  EXPECT_NO_THROW(value.Resize(SpriteDpConfig::BaseDpi));
}


TEST(TestSprite_BasicImageSprite, Resize_2x)
{
  constexpr PxExtent2D textureExtent(32, 64);
  constexpr SpriteMaterialId spriteMaterialId(1);
  auto material = std::make_shared<SpriteMaterialImpl>(spriteMaterialId, textureExtent);
  const SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureExtent, false, material, 127u);
  constexpr PxRectangleU imageRectanglePx(10, 20, 12, 26);
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi;
  constexpr StringViewLite debugName("hello world");
  constexpr uint32_t densityDpi = SpriteDpConfig::BaseDpi;
  constexpr auto nativeTextureArea = SpriteMaterialImpl::TestCalcNativeTextureArea(textureExtent, imageRectanglePx);

  BasicImageSprite value(spriteMaterialInfo, imageRectanglePx, imageDpi, debugName, densityDpi);

  EXPECT_NO_THROW(value.Resize(SpriteDpConfig::BaseDpi * 2));

  const auto& info = value.GetInfo();

  EXPECT_EQ(spriteMaterialInfo, info.MaterialInfo);
  EXPECT_EQ(imageRectanglePx.GetExtent(), info.ImageInfo.ExtentPx);
  EXPECT_EQ(imageRectanglePx, info.ImageInfo.RectanglePx);
  EXPECT_EQ(DpExtent(imageRectanglePx.GetExtent().Width, imageRectanglePx.GetExtent().Height), info.ImageInfo.ExtentDp);
  EXPECT_EQ(imageDpi, info.ImageDpi);
  EXPECT_EQ(nativeTextureArea, info.RenderInfo.TextureArea);
  EXPECT_EQ(TypeConverter::To<PxSize2D>(imageRectanglePx.GetExtent() * 2u), info.RenderInfo.ScaledSizePx);
}
