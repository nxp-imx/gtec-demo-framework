/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2023 NXP
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

#include <FslBase/Log/Math/Dp/LogDpExtent2D.hpp>
#include <FslBase/Log/Math/Dp/LogDpPoint2.hpp>
#include <FslBase/Log/Math/Dp/LogDpPoint2F.hpp>
#include <FslBase/Log/Math/Dp/LogDpSize2D.hpp>
#include <FslBase/Log/Math/Dp/LogDpThicknessU.hpp>
#include <FslBase/Log/Math/Pixel/LogPxPoint2.hpp>
#include <FslBase/Log/Math/Pixel/LogPxSize2D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxThickness.hpp>
#include <FslBase/Log/Math/Pixel/LogPxVector2.hpp>
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

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// DP 1x
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSprite_SpriteUnitConverter, PxToDpInt32_int32_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(int32_t(42), converter.PxToDpInt32(int32_t(42)));
}

TEST(TestSprite_SpriteUnitConverter, PxToDpInt32_float_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(int32_t(42), converter.PxToDpInt32(float(42)));
}

TEST(TestSprite_SpriteUnitConverter, DpToPxInt32_int32_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(int32_t(42), converter.DpToPxInt32(int32_t(42)));
}

TEST(TestSprite_SpriteUnitConverter, DpToPxInt32_float_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(int32_t(42), converter.DpToPxInt32(float(42)));
}


TEST(TestSprite_SpriteUnitConverter, ToDpValue_DpValue_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValue(1337), converter.ToDpValue(PxValue(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpValue_DpValueF_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValue(1337), converter.ToDpValue(PxValueF(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpValue_DpValueU_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValue(1337), converter.ToDpValue(PxValueU(1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToDpValueF_DpValue_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValueF(1337), converter.ToDpValueF(PxValue(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpValueF_DpValueF_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValueF(1337), converter.ToDpValueF(PxValueF(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpValueF_DpValueU_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValueF(1337), converter.ToDpValueF(PxValueU(1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToDpValueU_DpValue_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValueU(1337), converter.ToDpValueU(PxValue(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpValueU_DpValueF_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValueU(1337), converter.ToDpValueU(PxValueF(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpValueU_DpValueU_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValueU(1337), converter.ToDpValueU(PxValueU(1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToDpExtent2D_PxExtent2D_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpExtent2D::Create(42, 1337), converter.ToDpExtent2D(PxExtent2D::Create(42, 1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpPoint2_PxPoint2_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpPoint2::Create(42, 1337), converter.ToDpPoint2(PxPoint2::Create(42, 1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpSize2D_PxPoint2_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpSize2D::Create(42, 1337), converter.ToDpSize2D(PxPoint2::Create(42, 1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToDpSize2D_PxSize2D_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpSize2D::Create(42, 1337), converter.ToDpSize2D(PxSize2D::Create(42, 1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpThicknessU_PxThicknessU_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpThicknessU::Create(42, 1337, 10, 20), converter.ToDpThicknessU(PxThicknessU::Create(42, 1337, 10, 20)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// PX 1x
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestSprite_SpriteUnitConverter, PxToDpf_int32_t_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(float(42), converter.PxToDpf(int32_t(42)));
}

TEST(TestSprite_SpriteUnitConverter, PxToDpf_float_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(float(42), converter.PxToDpf(float(42)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxInt32_DpValue_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(int32_t(1337), converter.ToPxInt32(DpValue(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxInt32_DpValueF_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(int32_t(1337), converter.ToPxInt32(DpValueF(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxInt32_DpValueU_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(int32_t(1337), converter.ToPxInt32(DpValueU(1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxValue_DpValue_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValue(1337), converter.ToPxValue(DpValue(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxValue_DpValueF_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValue(1337), converter.ToPxValue(DpValueF(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxValue_DpValueU_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValue(1337), converter.ToPxValue(DpValueU(1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxValueF_DpValue_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValueF(1337), converter.ToPxValueF(DpValue(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxValueF_DpValueF_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValueF(1337), converter.ToPxValueF(DpValueF(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxValueF_DpValueU_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValueF(1337), converter.ToPxValueF(DpValueU(1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxValueU_DpValue_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValueU(1337), converter.ToPxValueU(DpValue(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxValueU_DpValueF_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValueU(1337), converter.ToPxValueU(DpValueF(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxValueU_DpValueU_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValueU(1337), converter.ToPxValueU(DpValueU(1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxPoint2_DpPoint2_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxPoint2::Create(42, 1337), converter.ToPxPoint2(DpPoint2::Create(42, 1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxPoint2_DpPoint2F_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxPoint2::Create(42, 1337), converter.ToPxPoint2(DpPoint2F::Create(42, 1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxPoint2_DpSize2D_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxPoint2::Create(42, 1337), converter.ToPxPoint2(DpSize2D::Create(42, 1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxSize2D_DpExtent2D_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxSize2D::Create(42, 1337), converter.ToPxSize2D(DpExtent2D::Create(42, 1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxSize2D_DpSize2DF_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxSize2D::Create(42, 1337), converter.ToPxSize2D(DpSize2DF::Create(42, 1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxThickness_DpThickness_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxThickness::Create(42, 1337, 10, 20), converter.ToPxThickness(DpThickness::Create(42, 1337, 10, 20)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxThickness_DpThicknessF_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxThickness::Create(42, 1337, 10, 20), converter.ToPxThickness(DpThicknessF::Create(42, 1337, 10, 20)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxThickness_DpThicknessU_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxThickness::Create(42, 1337, 10, 20), converter.ToPxThickness(DpThicknessU::Create(42, 1337, 10, 20)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxVector2_DpPoint2F_1x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxVector2::Create(42, 1337), converter.ToPxVector2(DpPoint2F::Create(42, 1337)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// DP 2x
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestSprite_SpriteUnitConverter, PxToDpInt32_int32_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(int32_t(42 / 2), converter.PxToDpInt32(int32_t(42)));
}

TEST(TestSprite_SpriteUnitConverter, PxToDpInt32_float_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(int32_t(42 / 2), converter.PxToDpInt32(float(42)));
}

TEST(TestSprite_SpriteUnitConverter, PxToDpf_int32_t_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(float(42.0f / 2.0f), converter.PxToDpf(int32_t(42)));
}

TEST(TestSprite_SpriteUnitConverter, PxToDpf_float_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(float(42.0f / 2.0f), converter.PxToDpf(float(42)));
}


TEST(TestSprite_SpriteUnitConverter, ToDpValue_DpValue_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValue((1337 / 2) + 1), converter.ToDpValue(PxValue(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpValue_DpValueF_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValue((1337 / 2) + 1), converter.ToDpValue(PxValueF(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpValue_DpValueU_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValue((1337 / 2) + 1), converter.ToDpValue(PxValueU(1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToDpValueF_DpValue_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValueF(1337.0f / 2.0f), converter.ToDpValueF(PxValue(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpValueF_DpValueF_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValueF(1337.0f / 2.0f), converter.ToDpValueF(PxValueF(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpValueF_DpValueU_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValueF(1337.0f / 2.0f), converter.ToDpValueF(PxValueU(1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToDpValueU_DpValue_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValueU((1337 / 2) + 1), converter.ToDpValueU(PxValue(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpValueU_DpValueF_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValueU((1337 / 2) + 1), converter.ToDpValueU(PxValueF(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpValueU_DpValueU_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpValueU((1337 / 2) + 1), converter.ToDpValueU(PxValueU(1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToDpExtent2D_PxExtent2D_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpExtent2D::Create(42 / 2, (1337 / 2) + 1), converter.ToDpExtent2D(PxExtent2D::Create(42, 1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToDpPoint2_PxPoint2_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpPoint2::Create(42 / 2, (1337 / 2) + 1), converter.ToDpPoint2(PxPoint2::Create(42, 1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToDpSize2D_PxPoint2_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpSize2D::Create(42 / 2, (1337 / 2) + 1), converter.ToDpSize2D(PxPoint2::Create(42, 1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToDpSize2D_PxSize2D_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpSize2D::Create(42 / 2, (1337 / 2) + 1), converter.ToDpSize2D(PxSize2D::Create(42, 1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToDpThicknessU_PxThicknessU_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(DpThicknessU::Create(42 / 2, (1337 / 2) + 1, 10 / 2, 20 / 2), converter.ToDpThicknessU(PxThicknessU::Create(42, 1337, 10, 20)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// PX 2x
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSprite_SpriteUnitConverter, DpToPxInt32_int32_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(int32_t(42 * 2), converter.DpToPxInt32(int32_t(42)));
}

TEST(TestSprite_SpriteUnitConverter, DpToPxInt32_float_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(int32_t(42 * 2), converter.DpToPxInt32(float(42)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxInt32_DpValue_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(int32_t(1337 * 2), converter.ToPxInt32(DpValue(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxInt32_DpValueF_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(int32_t(1337 * 2), converter.ToPxInt32(DpValueF(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxInt32_DpValueU_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(int32_t(1337 * 2), converter.ToPxInt32(DpValueU(1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxValue_DpValue_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValue(1337 * 2), converter.ToPxValue(DpValue(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxValue_DpValueF_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValue(1337 * 2), converter.ToPxValue(DpValueF(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxValue_DpValueU_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValue(1337 * 2), converter.ToPxValue(DpValueU(1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxValueF_DpValue_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValueF(1337 * 2), converter.ToPxValueF(DpValue(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxValueF_DpValueF_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValueF(1337 * 2), converter.ToPxValueF(DpValueF(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxValueF_DpValueU_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValueF(1337 * 2), converter.ToPxValueF(DpValueU(1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxValueU_DpValue_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValueU::Create(1337 * 2), converter.ToPxValueU(DpValue(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxValueU_DpValueF_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValueU::Create(1337 * 2), converter.ToPxValueU(DpValueF(1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxValueU_DpValueU_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxValueU::Create(1337 * 2), converter.ToPxValueU(DpValueU(1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxPoint2_DpPoint2_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxPoint2::Create(42 * 2, 1337 * 2), converter.ToPxPoint2(DpPoint2::Create(42, 1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxPoint2_DpPoint2F_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxPoint2::Create(42 * 2, 1337 * 2), converter.ToPxPoint2(DpPoint2F::Create(42, 1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxPoint2_DpSize2D_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxPoint2::Create(42 * 2, 1337 * 2), converter.ToPxPoint2(DpSize2D::Create(42, 1337)));
}


TEST(TestSprite_SpriteUnitConverter, ToPxSize2D_DpExtent2D_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxSize2D::Create(42 * 2, 1337 * 2), converter.ToPxSize2D(DpExtent2D::Create(42, 1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxSize2D_DpSize2DF_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxSize2D::Create(42 * 2, 1337 * 2), converter.ToPxSize2D(DpSize2DF::Create(42, 1337)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxThickness_DpThickness_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxThickness::Create(42 * 2, 1337 * 2, 10 * 2, 20 * 2), converter.ToPxThickness(DpThickness::Create(42, 1337, 10, 20)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxThickness_DpThicknessF_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxThickness::Create(42 * 2, 1337 * 2, 10 * 2, 20 * 2), converter.ToPxThickness(DpThicknessF::Create(42, 1337, 10, 20)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxThickness_DpThicknessU_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxThickness::Create(42 * 2, 1337 * 2, 10 * 2, 20 * 2), converter.ToPxThickness(DpThicknessU::Create(42, 1337, 10, 20)));
}

TEST(TestSprite_SpriteUnitConverter, ToPxVector2_DpPoint2F_2x)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  // Since its the base dpi no conversion should take place
  EXPECT_EQ(PxVector2::Create(42 * 2, 1337 * 2), converter.ToPxVector2(DpPoint2F::Create(42, 1337)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestSprite_SpriteUnitConverter, ToPxSize2D_Scale1)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  constexpr auto extent = DpExtent2D::Create(32u, 128u);

  auto res = converter.ToPxSize2D(extent);
  EXPECT_EQ(NumericCast<int32_t>(extent.Width.Value), res.RawWidth());
  EXPECT_EQ(NumericCast<int32_t>(extent.Height.Value), res.RawHeight());
}


TEST(TestSprite_SpriteUnitConverter, ToPxSize2D_Scale2)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  constexpr auto extent = DpExtent2D::Create(32u, 128u);

  auto res = converter.ToPxSize2D(extent);
  EXPECT_EQ(NumericCast<int32_t>(extent.Width.Value * 2u), res.RawWidth());
  EXPECT_EQ(NumericCast<int32_t>(extent.Height.Value * 2u), res.RawHeight());
}

TEST(TestSprite_SpriteUnitConverter, ToPxSize2D_Scale0_25)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi / 4);

  constexpr auto extent0 = DpExtent2D::Create(32u, 128u);
  constexpr auto extent1 = DpExtent2D::Create(0u, 1u);
  constexpr auto extent2 = DpExtent2D::Create(2u, 3u);

  auto res = converter.ToPxSize2D(extent0);
  EXPECT_EQ(NumericCast<int32_t>(extent0.Width.Value / 4u), res.RawWidth());
  EXPECT_EQ(NumericCast<int32_t>(extent0.Height.Value / 4u), res.RawHeight());

  res = converter.ToPxSize2D(extent1);
  EXPECT_EQ(0, res.RawWidth());
  EXPECT_EQ(0, res.RawHeight());

  res = converter.ToPxSize2D(extent2);
  EXPECT_EQ(1, res.RawWidth());
  EXPECT_EQ(1, res.RawHeight());
}


TEST(TestSprite_SpriteUnitConverter, ToPxSize2D_Scale0_5)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi / 2);

  constexpr auto extent0 = DpExtent2D::Create(32u, 128u);
  constexpr auto extent1 = DpExtent2D::Create(31u, 127u);

  auto res = converter.ToPxSize2D(extent0);
  EXPECT_EQ(NumericCast<int32_t>(extent0.Width.Value / 2u), res.RawWidth());
  EXPECT_EQ(NumericCast<int32_t>(extent0.Height.Value / 2u), res.RawHeight());

  res = converter.ToPxSize2D(extent1);
  EXPECT_EQ(NumericCast<int32_t>((extent1.Width.Value / 2u) + 1u), res.RawWidth());
  EXPECT_EQ(NumericCast<int32_t>((extent1.Height.Value / 2u) + 1u), res.RawHeight());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSprite_SpriteUnitConverter, ToPxExtent2D_Scale1)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi);

  constexpr auto extent = DpExtent2D::Create(32u, 128u);

  auto res = converter.ToPxExtent2D(extent);
  EXPECT_EQ(extent.Width.Value, res.Width.Value);
  EXPECT_EQ(extent.Height.Value, res.Height.Value);
}


TEST(TestSprite_SpriteUnitConverter, ToPxExtent2D_Scale2)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi * 2);

  constexpr auto extent = DpExtent2D::Create(32u, 128u);

  auto res = converter.ToPxExtent2D(extent);
  EXPECT_EQ(extent.Width.Value * 2u, res.Width.Value);
  EXPECT_EQ(extent.Height.Value * 2u, res.Height.Value);
}

TEST(TestSprite_SpriteUnitConverter, ToPxExtent2D_Scale0_25)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi / 4);

  constexpr auto extent0 = DpExtent2D::Create(32u, 128u);
  constexpr auto extent1 = DpExtent2D::Create(0u, 1u);
  constexpr auto extent2 = DpExtent2D::Create(2u, 3u);

  auto res = converter.ToPxExtent2D(extent0);
  EXPECT_EQ(extent0.Width.Value / 4u, res.Width.Value);
  EXPECT_EQ(extent0.Height.Value / 4u, res.Height.Value);

  res = converter.ToPxExtent2D(extent1);
  EXPECT_EQ(PxValueU(0u), res.Width);
  EXPECT_EQ(PxValueU(0u), res.Height);

  res = converter.ToPxExtent2D(extent2);
  EXPECT_EQ(PxValueU(1u), res.Width);
  EXPECT_EQ(PxValueU(1u), res.Height);
}


TEST(TestSprite_SpriteUnitConverter, ToPxExtent2D_Scale0_5)
{
  SpriteUnitConverter converter(SpriteDpConfig::BaseDpi / 2);

  constexpr auto extent0 = DpExtent2D::Create(32u, 128u);
  constexpr auto extent1 = DpExtent2D::Create(31u, 127u);

  auto res = converter.ToPxExtent2D(extent0);
  EXPECT_EQ(extent0.Width.Value / 2u, res.Width.Value);
  EXPECT_EQ(extent0.Height.Value / 2u, res.Height.Value);

  res = converter.ToPxExtent2D(extent1);
  EXPECT_EQ((extent1.Width.Value / 2u) + 1u, res.Width.Value);
  EXPECT_EQ((extent1.Height.Value / 2u) + 1u, res.Height.Value);
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
  constexpr auto extentPx = PxExtent2D::Create(32u, 128u);

  EXPECT_THROW(SpriteUnitConverter::CalcImageDpExtent2D(extentPx, 0u), std::invalid_argument);
}

TEST(TestSprite_SpriteUnitConverter, CalcImageDpExtent2D)
{
  constexpr uint32_t scaleFactor = 4;
  constexpr uint32_t imageDpi = SpriteDpConfig::BaseDpi * scaleFactor;
  constexpr auto extentPx = PxExtent2D::Create(32u, 128u);
  const auto res = SpriteUnitConverter::CalcImageDpExtent2D(extentPx, imageDpi);

  EXPECT_EQ(extentPx.Width.Value / scaleFactor, res.Width.Value);
  EXPECT_EQ(extentPx.Height.Value / scaleFactor, res.Height.Value);
}

TEST(TestSprite_SpriteUnitConverter, CalcImageExtentDp2)
{
  constexpr uint32_t scaleFactor = 4;
  constexpr uint32_t imageDpi = SpriteDpConfig::BaseDpi * scaleFactor;
  constexpr auto extentPx = PxExtent2D::Create(33u, 128u);
  DpExtent2D valuePx = SpriteUnitConverter::CalcImageDpExtent2D(extentPx, imageDpi);

  const float extentXDpf = std::round(static_cast<float>(extentPx.Width.Value) / static_cast<float>(scaleFactor));
  const float extentYDpf = std::round(static_cast<float>(extentPx.Height.Value) / static_cast<float>(scaleFactor));
  const auto extentDp = DpExtent2D::Create(static_cast<PxExtent2D::raw_value_type>(extentXDpf), static_cast<PxExtent2D::raw_value_type>(extentYDpf));
  EXPECT_EQ(extentDp, valuePx);
}
