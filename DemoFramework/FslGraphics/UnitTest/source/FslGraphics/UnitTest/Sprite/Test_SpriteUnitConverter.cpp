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

#include <FslBase/NumericCast.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using TestSprite_SpriteUnitConverter = TestFixtureFslGraphics;
}

TEST(TestSprite_SpriteUnitConverter, Construct_Invalid)
{
  EXPECT_THROW(SpriteUnitConverter(0), std::invalid_argument);
}


TEST(TestSprite_SpriteUnitConverter, Construct_BaseDpi)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  EXPECT_EQ(SpriteDpConfig::BaseDpi, converter.GetDensityDpi());
}


TEST(TestSprite_SpriteUnitConverter, SetDensityDpi_Invalid)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  EXPECT_THROW(converter.SetDensityDpi(0), std::invalid_argument);
}


TEST(TestSprite_SpriteUnitConverter, SetDensityDpi)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  converter.SetDensityDpi(42);

  EXPECT_EQ(42u, converter.GetDensityDpi());
}


TEST(TestSprite_SpriteUnitConverter, ToDpPoint_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpPoint(42, 1337), converter.ToDpPoint(PxPoint2(42, 1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxPoint2_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxPoint2(42, 1337), converter.ToPxPoint2(DpPoint(42, 1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxSize2D_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxSize2D(42, 1337), converter.ToPxSize2D(DpExtent(42, 1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToDpPoint_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpPoint(42 / 2, (1337 / 2) + 1), converter.ToDpPoint(PxPoint2(42, 1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxPoint2_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxPoint2(42 * 2, 1337 * 2), converter.ToPxPoint2(DpPoint(42, 1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxSize2D_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxSize2D(42 * 2, 1337 * 2), converter.ToPxSize2D(DpExtent(42, 1337)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestSprite_SpriteUnitConverter, ToPxSize2D_Scale1)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  constexpr DpExtent extent(32u, 128u);

  auto res = converter.ToPxSize2D(extent);
  EXPECT_EQ(NumericCast<int32_t>(extent.Width), res.Width());
  EXPECT_EQ(NumericCast<int32_t>(extent.Height), res.Height());
}


TEST(TestSprite_SpriteUnitConverter, ToPxSize2D_Scale2)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  constexpr DpExtent extent(32u, 128u);

  auto res = converter.ToPxSize2D(extent);
  EXPECT_EQ(NumericCast<int32_t>(extent.Width * 2u), res.Width());
  EXPECT_EQ(NumericCast<int32_t>(extent.Height * 2u), res.Height());
}

TEST(TestSprite_SpriteUnitConverter, ToPxSize2D_Scale0_25)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi / 4);

  constexpr DpExtent extent0(32u, 128u);
  constexpr DpExtent extent1(0u, 1u);
  constexpr DpExtent extent2(2u, 3u);

  auto res = converter.ToPxSize2D(extent0);
  EXPECT_EQ(NumericCast<int32_t>(extent0.Width / 4u), res.Width());
  EXPECT_EQ(NumericCast<int32_t>(extent0.Height / 4u), res.Height());

  res = converter.ToPxSize2D(extent1);
  EXPECT_EQ(0, res.Width());
  EXPECT_EQ(0, res.Height());

  res = converter.ToPxSize2D(extent2);
  EXPECT_EQ(1, res.Width());
  EXPECT_EQ(1, res.Height());
}


TEST(TestSprite_SpriteUnitConverter, ToPxSize2D_Scale0_5)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi / 2);

  constexpr DpExtent extent0(32u, 128u);
  constexpr DpExtent extent1(31u, 127u);

  auto res = converter.ToPxSize2D(extent0);
  EXPECT_EQ(NumericCast<int32_t>(extent0.Width / 2u), res.Width());
  EXPECT_EQ(NumericCast<int32_t>(extent0.Height / 2u), res.Height());

  res = converter.ToPxSize2D(extent1);
  EXPECT_EQ(NumericCast<int32_t>((extent1.Width / 2u) + 1u), res.Width());
  EXPECT_EQ(NumericCast<int32_t>((extent1.Height / 2u) + 1u), res.Height());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSprite_SpriteUnitConverter, ToPxExtent2D_Scale1)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  constexpr DpExtent extent(32u, 128u);

  auto res = converter.ToPxExtent2D(extent);
  EXPECT_EQ(extent.Width, res.Width);
  EXPECT_EQ(extent.Height, res.Height);
}


TEST(TestSprite_SpriteUnitConverter, ToPxExtent2D_Scale2)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  constexpr DpExtent extent(32u, 128u);

  auto res = converter.ToPxExtent2D(extent);
  EXPECT_EQ(extent.Width * 2u, res.Width);
  EXPECT_EQ(extent.Height * 2u, res.Height);
}

TEST(TestSprite_SpriteUnitConverter, ToPxExtent2D_Scale0_25)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi / 4);

  constexpr DpExtent extent0(32u, 128u);
  constexpr DpExtent extent1(0u, 1u);
  constexpr DpExtent extent2(2u, 3u);

  auto res = converter.ToPxExtent2D(extent0);
  EXPECT_EQ(extent0.Width / 4u, res.Width);
  EXPECT_EQ(extent0.Height / 4u, res.Height);

  res = converter.ToPxExtent2D(extent1);
  EXPECT_EQ(0u, res.Width);
  EXPECT_EQ(0u, res.Height);

  res = converter.ToPxExtent2D(extent2);
  EXPECT_EQ(1u, res.Width);
  EXPECT_EQ(1u, res.Height);
}


TEST(TestSprite_SpriteUnitConverter, ToPxExtent2D_Scale0_5)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi / 2);

  constexpr DpExtent extent0(32u, 128u);
  constexpr DpExtent extent1(31u, 127u);

  auto res = converter.ToPxExtent2D(extent0);
  EXPECT_EQ(extent0.Width / 2u, res.Width);
  EXPECT_EQ(extent0.Height / 2u, res.Height);

  res = converter.ToPxExtent2D(extent1);
  EXPECT_EQ((extent1.Width / 2u) + 1u, res.Width);
  EXPECT_EQ((extent1.Height / 2u) + 1u, res.Height);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestSprite_SpriteUnitConverter, CalcImageDensityScale_1_1)
{
  // the screen has a 1x density dpi. and the texture is in the 1x base dpi.
  // So the texture pixels need to be scaled by 1/1 = 1 to fill the same area.

  const uint32_t screenScale = 1;
  const uint32_t imageScale = 1;
  const uint32_t densityDpi = SpriteDpConfig::BaseDpi * screenScale;
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi * imageScale;
  SpriteUnitConverter unitConverter(densityDpi);

  EXPECT_FLOAT_EQ((float(screenScale) / float(imageScale)), unitConverter.CalcImageDensityScale(imageDpi));
}

TEST(TestSprite_SpriteUnitConverter, CalcImageDensityScale_1_2)
{
  // the screen has a 1x density dpi. and the texture is in the 1x base dpi.
  // So the texture pixels need to be scaled by 1/2 = 0.5 to fill the same area.

  const uint32_t screenScale = 1;
  const uint32_t imageScale = 2;
  const uint32_t densityDpi = SpriteDpConfig::BaseDpi * screenScale;
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi * imageScale;
  SpriteUnitConverter unitConverter(densityDpi);

  EXPECT_FLOAT_EQ((float(screenScale) / float(imageScale)), unitConverter.CalcImageDensityScale(imageDpi));
}

TEST(TestSprite_SpriteUnitConverter, CalcImageDensityScale_1_4)
{
  // the screen has a 1x density dpi. and the texture is in the 1x base dpi.
  // So the texture pixels need to be scaled by 1/4 = 0.25 to fill the same area.

  const uint32_t screenScale = 1;
  const uint32_t imageScale = 4;
  const uint32_t densityDpi = SpriteDpConfig::BaseDpi * screenScale;
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi * imageScale;
  SpriteUnitConverter unitConverter(densityDpi);

  EXPECT_FLOAT_EQ((float(screenScale) / float(imageScale)), unitConverter.CalcImageDensityScale(imageDpi));
}

TEST(TestSprite_SpriteUnitConverter, CalcImageDensityScale_2_1)
{
  // the screen has a 2x density dpi. and the texture is in the 1x base dpi.
  // So the texture pixels need to be scaled by 2/1 = 0.5 to fill the same area.

  const uint32_t screenScale = 2;
  const uint32_t imageScale = 1;
  const uint32_t densityDpi = SpriteDpConfig::BaseDpi * screenScale;
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi * imageScale;
  SpriteUnitConverter unitConverter(densityDpi);

  EXPECT_FLOAT_EQ((float(screenScale) / float(imageScale)), unitConverter.CalcImageDensityScale(imageDpi));
}

TEST(TestSprite_SpriteUnitConverter, CalcImageDensityScale_2_2)
{
  // the screen has a 2x density dpi. and the texture is in the 1x base dpi.
  // So the texture pixels need to be scaled by 2/2 = 1.0 to fill the same area.

  const uint32_t screenScale = 2;
  const uint32_t imageScale = 2;
  const uint32_t densityDpi = SpriteDpConfig::BaseDpi * screenScale;
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi * imageScale;
  SpriteUnitConverter unitConverter(densityDpi);

  EXPECT_FLOAT_EQ((float(screenScale) / float(imageScale)), unitConverter.CalcImageDensityScale(imageDpi));
}

TEST(TestSprite_SpriteUnitConverter, CalcImageDensityScale_2_4)
{
  // the screen has a 2x density dpi. and the texture is in the 1x base dpi.
  // So the texture pixels need to be scaled by 2/4 = 0.25 to fill the same area.

  const uint32_t screenScale = 2;
  const uint32_t imageScale = 4;
  const uint32_t densityDpi = SpriteDpConfig::BaseDpi * screenScale;
  const uint32_t imageDpi = SpriteDpConfig::BaseDpi * imageScale;
  SpriteUnitConverter unitConverter(densityDpi);

  EXPECT_FLOAT_EQ((float(screenScale) / float(imageScale)), unitConverter.CalcImageDensityScale(imageDpi));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------


// TEST(TestSprite_SpriteUnitConverter, CalcImageScale_1x)
//{
//  constexpr auto imageDpi = SpriteDpConfig::BaseDpi * 1;
//  constexpr auto imageScale = SpriteUnitConverter::CalcImageScale(imageDpi);
//
//  EXPECT_EQ((imageDpi / SpriteDpConfig::BaseDpi), imageScale);
//}
//
//
// TEST(TestSprite_SpriteUnitConverter, CalcImageScale_2x)
//{
//  constexpr auto imageDpi = SpriteDpConfig::BaseDpi * 2;
//  constexpr auto imageScale = SpriteUnitConverter::CalcImageScale(imageDpi);
//
//  EXPECT_EQ((imageDpi / SpriteDpConfig::BaseDpi), imageScale);
//}
//
//
// TEST(TestSprite_SpriteUnitConverter, CalcImageScalePx_4x)
//{
//  constexpr auto imageDpi = SpriteDpConfig::BaseDpi * 4;
//  constexpr auto imageScale = SpriteUnitConverter::CalcImageScale(imageDpi);
//
//  EXPECT_EQ((imageDpi / SpriteDpConfig::BaseDpi), imageScale);
//}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSprite_SpriteUnitConverter, CalcImageExtentDp_ZeroDpi)
{
  constexpr PxExtent2D extentPx(32u, 128u);

  EXPECT_THROW(SpriteUnitConverter::CalcImageDpExtent(extentPx, 0u), std::invalid_argument);
}

TEST(TestSprite_SpriteUnitConverter, CalcImageDpExtent)
{
  constexpr uint32_t scaleFactor = 4;
  constexpr uint32_t imageDpi = SpriteDpConfig::BaseDpi * scaleFactor;
  constexpr PxExtent2D extentPx(32u, 128u);
  const auto res = SpriteUnitConverter::CalcImageDpExtent(extentPx, imageDpi);

  EXPECT_EQ(extentPx.Width / scaleFactor, res.Width);
  EXPECT_EQ(extentPx.Height / scaleFactor, res.Height);
}

TEST(TestSprite_SpriteUnitConverter, CalcImageExtentDp2)
{
  constexpr uint32_t scaleFactor = 4;
  constexpr uint32_t imageDpi = SpriteDpConfig::BaseDpi * scaleFactor;
  constexpr PxExtent2D extentPx(33u, 128u);
  DpExtent valuePx = SpriteUnitConverter::CalcImageDpExtent(extentPx, imageDpi);

  const float extentXDpf = std::round(float(extentPx.Width) / float(scaleFactor));
  const float extentYDpf = std::round(float(extentPx.Height) / float(scaleFactor));
  const DpExtent extentDp(static_cast<PxExtent2D::value_type>(extentXDpf), static_cast<PxExtent2D::value_type>(extentYDpf));
  EXPECT_EQ(extentDp, valuePx);
}
