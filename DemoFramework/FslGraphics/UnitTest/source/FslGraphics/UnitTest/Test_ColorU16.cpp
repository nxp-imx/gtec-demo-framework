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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslGraphics/ColorChannelConverter.hpp>
#include <FslGraphics/ColorChannelValueU16.hpp>
#include <FslGraphics/ColorU16.hpp>
#include <FslGraphics/ColorsU16.hpp>
#include <FslGraphics/Log/FmtColorU16.hpp>
#include <FslGraphics/Log/LogColorU16.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>
#include <cmath>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using Test_ColorU16 = TestFixtureFslGraphics;

  uint16_t CreateU16(const float value)
  {
    const float val = MathHelper::Clamp(value, 0.0f, 1.0f);
    return static_cast<uint16_t>(std::round(val * static_cast<float>(0xFFFF)));
  }

  constexpr ColorChannelValueU16 ExtractA(const uint32_t value)
  {
    return ColorChannelConverter::ToU16(ColorChannelValueU8((value >> 24u) & 0xFF));
  }

  // constexpr ColorChannelValueU16 ExtractA(const uint64_t value)
  // {
  //   return ColorChannelValueU16((value >> 48u) & 0xFFFF);
  // }

  constexpr ColorChannelValueU16 ExtractR(const uint32_t value)
  {
    return ColorChannelConverter::ToU16(ColorChannelValueU8((value >> 16u) & 0xFF));
  }

  // constexpr ColorChannelValueU16 ExtractR(const uint64_t value)
  // {
  //   return ColorChannelValueU16((value >> 32u) & 0xFFFF);
  // }

  constexpr ColorChannelValueU16 ExtractG(const uint32_t value)
  {
    return ColorChannelConverter::ToU16(ColorChannelValueU8((value >> 8u) & 0xFF));
  }

  // constexpr ColorChannelValueU16 ExtractG(const uint64_t value)
  // {
  //   return ColorChannelValueU16((value >> 16u) & 0xFFFF);
  // }

  constexpr ColorChannelValueU16 ExtractB(const uint32_t value)
  {
    return ColorChannelConverter::ToU16(ColorChannelValueU8(value & 0xFF));
  }

  // constexpr ColorChannelValueU16 ExtractB(const uint64_t value)
  // {
  //   return ColorChannelValueU16(value & 0xFFFF);
  // }

  // For coverage purpose ensure that ToVector4 gets called
  Vector4 WorkAroundConstExpr(const ColorU16 color)
  {
    return color.ToVector4();
  }


  ColorU16 MultiplyRGB(const ColorU16 color, const float factor)
  {
    double alpha = MathHelper::Clamp(factor, 0.0f, 1.0f);
    auto r = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawR()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    auto g = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawG()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    auto b = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawB()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    return ColorU16::CreateR16G16B16A16UNorm(r, g, b, static_cast<uint32_t>(color.RawA()));
  }

  ColorU16 MultiplyRGBA(const ColorU16 color, const float factor)
  {
    double alpha = MathHelper::Clamp(factor, 0.0f, 1.0f);
    auto r = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawR()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    auto g = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawG()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    auto b = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawB()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    auto a = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawA()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    return ColorU16::CreateR16G16B16A16UNorm(r, g, b, a);
  }

  ColorU16 MultiplyA(const ColorU16 color, const float factor)
  {
    double alpha = MathHelper::Clamp(factor, 0.0f, 1.0f);
    auto a = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawA()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    return ColorU16::CreateR16G16B16A16UNorm(color.RawR(), color.RawG(), color.RawB(), NumericCast<uint16_t>(a));
  }

  ColorU16 MultiplyA(const ColorU16 color, const ColorChannelValueU16 factor)
  {
    double alpha = static_cast<double>(factor.RawValue) / static_cast<double>(0xFFFF);
    auto a = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawA()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    return ColorU16::CreateR16G16B16A16UNorm(color.RawR(), color.RawG(), color.RawB(), NumericCast<uint16_t>(a));
  }

  ColorU16 Premultiply(const ColorU16 color)
  {
    double alpha = static_cast<double>(color.RawA()) / static_cast<double>(0xFFFF);
    auto r = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawR()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    auto g = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawG()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    auto b = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawB()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    return ColorU16::CreateR16G16B16A16UNorm(r, g, b, static_cast<uint32_t>(color.RawA()));
  }

  ColorU16 Premultiply(const ColorU16 color, const float alphaMul)
  {
    double alpha = (static_cast<double>(color.RawA()) / static_cast<double>(0xFFFF)) * MathHelper::Clamp(alphaMul, 0.0f, 100.0f);
    auto r = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawR()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    auto g = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawG()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    auto b = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawB()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    auto a = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(0xFFFF))), static_cast<int64_t>(0),
                                                     static_cast<int64_t>(0xFFFF)));
    return ColorU16::CreateR16G16B16A16UNorm(r, g, b, a);
  }

  ColorU16 PremultiplyRGB(const ColorU16 color, const float alphaMul, const uint32_t newAlpha)
  {
    double alpha = (static_cast<double>(color.RawA()) / static_cast<double>(0xFFFF)) * MathHelper::Clamp(alphaMul, 0.0f, 100.0f);
    auto r = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawR()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    auto g = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawG()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    auto b = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawB()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF)));
    return ColorU16::CreateR16G16B16A16UNorm(r, g, b, newAlpha);
  }
}


TEST(Test_ColorU16, Construct_Empty)
{
  ColorU16 color;

  EXPECT_EQ(0u, color.RawR());
  EXPECT_EQ(0u, color.RawG());
  EXPECT_EQ(0u, color.RawB());
  EXPECT_EQ(0u, color.RawA());
  EXPECT_EQ(0u, color.AsPackedColor32().RawValue);
  EXPECT_EQ(0u, color.AsPackedColor64().RawValue);
}


TEST(Test_ColorU16, Construct_Float)
{
  ColorU16 colWhite = ColorU16::Create(1.0f, 1.0f, 1.0f, 1.0f);
  ColorU16 colR = ColorU16::Create(1.0f, 0.0f, 0.0f, 0.0f);
  ColorU16 colG = ColorU16::Create(0.0f, 1.0f, 0.0f, 0.0f);
  ColorU16 colB = ColorU16::Create(0.0f, 0.0f, 1.0f, 0.0f);
  ColorU16 colA = ColorU16::Create(0.0f, 0.0f, 0.0f, 1.0f);
  ColorU16 colMisc = ColorU16::Create(0.5f, 0.5f, 0.5f, 0.5f);

  EXPECT_EQ(0xFFFFu, colWhite.RawR());
  EXPECT_EQ(0xFFFFu, colWhite.RawG());
  EXPECT_EQ(0xFFFFu, colWhite.RawB());
  EXPECT_EQ(0xFFFFu, colWhite.RawA());
  EXPECT_EQ(0xFFFFFFFFu, colWhite.AsPackedColor32().RawValue);
  EXPECT_EQ(0xFFFFFFFFFFFFFFFFu, colWhite.AsPackedColor64().RawValue);

  EXPECT_EQ(0xFFFFu, colR.RawR());
  EXPECT_EQ(0x0000u, colR.RawG());
  EXPECT_EQ(0x0000u, colR.RawB());
  EXPECT_EQ(0x0000u, colR.RawA());
  EXPECT_EQ(0x00FF0000u, colR.AsPackedColor32().RawValue);
  EXPECT_EQ(0x0000FFFF00000000u, colR.AsPackedColor64().RawValue);

  EXPECT_EQ(0x0000u, colG.RawR());
  EXPECT_EQ(0xFFFFu, colG.RawG());
  EXPECT_EQ(0x0000u, colG.RawB());
  EXPECT_EQ(0x0000u, colG.RawA());
  EXPECT_EQ(0x0000FF00u, colG.AsPackedColor32().RawValue);
  EXPECT_EQ(0x00000000FFFF0000u, colG.AsPackedColor64().RawValue);

  EXPECT_EQ(0x0000u, colB.RawR());
  EXPECT_EQ(0x0000u, colB.RawG());
  EXPECT_EQ(0xFFFFu, colB.RawB());
  EXPECT_EQ(0x0000u, colB.RawA());
  EXPECT_EQ(0x000000FFu, colB.AsPackedColor32().RawValue);
  EXPECT_EQ(0x000000000000FFFFu, colB.AsPackedColor64().RawValue);

  EXPECT_EQ(0x0000u, colA.RawR());
  EXPECT_EQ(0x0000u, colA.RawG());
  EXPECT_EQ(0x0000u, colA.RawB());
  EXPECT_EQ(0xFFFFu, colA.RawA());
  EXPECT_EQ(0xFF000000u, colA.AsPackedColor32().RawValue);
  EXPECT_EQ(0xFFFF000000000000u, colA.AsPackedColor64().RawValue);

  EXPECT_EQ(0x8000u, colMisc.RawR());
  EXPECT_EQ(0x8000u, colMisc.RawG());
  EXPECT_EQ(0x8000u, colMisc.RawB());
  EXPECT_EQ(0x8000u, colMisc.RawA());
  EXPECT_EQ(0x80808080u, colMisc.AsPackedColor32().RawValue);
  EXPECT_EQ(0x8000800080008000u, colMisc.AsPackedColor64().RawValue);
}


TEST(Test_ColorU16, Construct_Floats)
{
  for (uint32_t i = 0; i <= 0x2FFFF; ++i)
  {
    const float value = static_cast<float>(i) / static_cast<float>(0x2FFFF);

    const uint16_t expected = CreateU16(value);
    const uint16_t maxValue = 0xFFFF;
    ColorU16 expectedR = ColorU16::CreateR16G16B16A16UNorm(expected, maxValue, maxValue, maxValue);
    ColorU16 expectedG = ColorU16::CreateR16G16B16A16UNorm(maxValue, expected, maxValue, maxValue);
    ColorU16 expectedB = ColorU16::CreateR16G16B16A16UNorm(maxValue, maxValue, expected, maxValue);
    ColorU16 expectedA = ColorU16::CreateR16G16B16A16UNorm(maxValue, maxValue, maxValue, expected);

    ColorU16 gotR = ColorU16::Create(value, 1.0f, 1.0f, 1.0f);
    ASSERT_EQ(expectedR, gotR);
    ColorU16 gotG = ColorU16::Create(1.0f, value, 1.0f, 1.0f);
    ASSERT_EQ(expectedG, gotG);
    ColorU16 gotB = ColorU16::Create(1.0f, 1.0f, value, 1.0f);
    ASSERT_EQ(expectedB, gotB);
    ColorU16 gotA = ColorU16::Create(1.0f, 1.0f, 1.0f, value);
    ASSERT_EQ(expectedA, gotA);
  }
}


TEST(Test_ColorU16, Colors_32)
{
  uint32_t colTransparent = 0x00000000;
  uint32_t colBlack = 0xFF000000;
  uint32_t colRed = 0xFFFF0000;
  uint32_t colGreen = 0xFF00FF00;
  uint32_t colBlue = 0xFF0000FF;
  uint32_t colCyan = 0xFF00FFFF;
  uint32_t colYellow = 0xFFFFFF00;
  uint32_t colWhite = 0xFFFFFFFF;
  uint32_t colOrange = 0xFFFFA500;
  uint32_t colPink = 0xFFFAAFBE;
  uint32_t colPurple = 0xFF800080;
  uint32_t colMarrom = 0xFF800000;
  uint32_t colBrown = 0xFFA52A2A;
  uint32_t colOlive = 0xFF808000;
  uint32_t colSilver = 0xFFC0C0C0;
  uint32_t colDarkBlue = 0xFF0000A0;
  uint32_t colLightBlue = 0xFFADD8E6;
  uint32_t colLime = 0xFF00FF00;

  EXPECT_EQ(colTransparent, ColorsU16::Transparent().AsPackedColor32().RawValue);
  EXPECT_EQ(colBlack, ColorsU16::Black().AsPackedColor32().RawValue);
  EXPECT_EQ(colRed, ColorsU16::Red().AsPackedColor32().RawValue);
  EXPECT_EQ(colGreen, ColorsU16::Green().AsPackedColor32().RawValue);
  EXPECT_EQ(colBlue, ColorsU16::Blue().AsPackedColor32().RawValue);
  EXPECT_EQ(colCyan, ColorsU16::Cyan().AsPackedColor32().RawValue);
  EXPECT_EQ(colYellow, ColorsU16::Yellow().AsPackedColor32().RawValue);
  EXPECT_EQ(colWhite, ColorsU16::White().AsPackedColor32().RawValue);
  EXPECT_EQ(colOrange, ColorsU16::Orange().AsPackedColor32().RawValue);
  EXPECT_EQ(colPink, ColorsU16::Pink().AsPackedColor32().RawValue);
  EXPECT_EQ(colPurple, ColorsU16::Purple().AsPackedColor32().RawValue);
  EXPECT_EQ(colMarrom, ColorsU16::Marrom().AsPackedColor32().RawValue);
  EXPECT_EQ(colBrown, ColorsU16::Brown().AsPackedColor32().RawValue);
  EXPECT_EQ(colOlive, ColorsU16::Olive().AsPackedColor32().RawValue);
  EXPECT_EQ(colSilver, ColorsU16::Silver().AsPackedColor32().RawValue);
  EXPECT_EQ(colDarkBlue, ColorsU16::DarkBlue().AsPackedColor32().RawValue);
  EXPECT_EQ(colLightBlue, ColorsU16::LightBlue().AsPackedColor32().RawValue);
  EXPECT_EQ(colLime, ColorsU16::Lime().AsPackedColor32().RawValue);

  // Check alpha channel
  EXPECT_EQ(ExtractA(colTransparent), ColorsU16::Transparent().A());
  EXPECT_EQ(ExtractA(colBlack), ColorsU16::Black().A());
  EXPECT_EQ(ExtractA(colRed), ColorsU16::Red().A());
  EXPECT_EQ(ExtractA(colGreen), ColorsU16::Green().A());
  EXPECT_EQ(ExtractA(colBlue), ColorsU16::Blue().A());
  EXPECT_EQ(ExtractA(colCyan), ColorsU16::Cyan().A());
  EXPECT_EQ(ExtractA(colYellow), ColorsU16::Yellow().A());
  EXPECT_EQ(ExtractA(colWhite), ColorsU16::White().A());
  EXPECT_EQ(ExtractA(colOrange), ColorsU16::Orange().A());
  EXPECT_EQ(ExtractA(colPink), ColorsU16::Pink().A());
  EXPECT_EQ(ExtractA(colPurple), ColorsU16::Purple().A());
  EXPECT_EQ(ExtractA(colMarrom), ColorsU16::Marrom().A());
  EXPECT_EQ(ExtractA(colBrown), ColorsU16::Brown().A());
  EXPECT_EQ(ExtractA(colOlive), ColorsU16::Olive().A());
  EXPECT_EQ(ExtractA(colSilver), ColorsU16::Silver().A());
  EXPECT_EQ(ExtractA(colDarkBlue), ColorsU16::DarkBlue().A());
  EXPECT_EQ(ExtractA(colLightBlue), ColorsU16::LightBlue().A());
  EXPECT_EQ(ExtractA(colLime), ColorsU16::Lime().A());

  // Red
  EXPECT_EQ(ExtractR(colTransparent), ColorsU16::Transparent().R());
  EXPECT_EQ(ExtractR(colBlack), ColorsU16::Black().R());
  EXPECT_EQ(ExtractR(colRed), ColorsU16::Red().R());
  EXPECT_EQ(ExtractR(colGreen), ColorsU16::Green().R());
  EXPECT_EQ(ExtractR(colBlue), ColorsU16::Blue().R());
  EXPECT_EQ(ExtractR(colCyan), ColorsU16::Cyan().R());
  EXPECT_EQ(ExtractR(colYellow), ColorsU16::Yellow().R());
  EXPECT_EQ(ExtractR(colWhite), ColorsU16::White().R());
  EXPECT_EQ(ExtractR(colOrange), ColorsU16::Orange().R());
  EXPECT_EQ(ExtractR(colPink), ColorsU16::Pink().R());
  EXPECT_EQ(ExtractR(colPurple), ColorsU16::Purple().R());
  EXPECT_EQ(ExtractR(colMarrom), ColorsU16::Marrom().R());
  EXPECT_EQ(ExtractR(colBrown), ColorsU16::Brown().R());
  EXPECT_EQ(ExtractR(colOlive), ColorsU16::Olive().R());
  EXPECT_EQ(ExtractR(colSilver), ColorsU16::Silver().R());
  EXPECT_EQ(ExtractR(colDarkBlue), ColorsU16::DarkBlue().R());
  EXPECT_EQ(ExtractR(colLightBlue), ColorsU16::LightBlue().R());
  EXPECT_EQ(ExtractR(colLime), ColorsU16::Lime().R());

  // Green
  EXPECT_EQ(ExtractG(colTransparent), ColorsU16::Transparent().G());
  EXPECT_EQ(ExtractG(colBlack), ColorsU16::Black().G());
  EXPECT_EQ(ExtractG(colRed), ColorsU16::Red().G());
  EXPECT_EQ(ExtractG(colGreen), ColorsU16::Green().G());
  EXPECT_EQ(ExtractG(colBlue), ColorsU16::Blue().G());
  EXPECT_EQ(ExtractG(colCyan), ColorsU16::Cyan().G());
  EXPECT_EQ(ExtractG(colYellow), ColorsU16::Yellow().G());
  EXPECT_EQ(ExtractG(colWhite), ColorsU16::White().G());
  EXPECT_EQ(ExtractG(colOrange), ColorsU16::Orange().G());
  EXPECT_EQ(ExtractG(colPink), ColorsU16::Pink().G());
  EXPECT_EQ(ExtractG(colPurple), ColorsU16::Purple().G());
  EXPECT_EQ(ExtractG(colMarrom), ColorsU16::Marrom().G());
  EXPECT_EQ(ExtractG(colBrown), ColorsU16::Brown().G());
  EXPECT_EQ(ExtractG(colOlive), ColorsU16::Olive().G());
  EXPECT_EQ(ExtractG(colSilver), ColorsU16::Silver().G());
  EXPECT_EQ(ExtractG(colDarkBlue), ColorsU16::DarkBlue().G());
  EXPECT_EQ(ExtractG(colLightBlue), ColorsU16::LightBlue().G());
  EXPECT_EQ(ExtractG(colLime), ColorsU16::Lime().G());

  // Blue
  EXPECT_EQ(ExtractB(colTransparent), ColorsU16::Transparent().B());
  EXPECT_EQ(ExtractB(colBlack), ColorsU16::Black().B());
  EXPECT_EQ(ExtractB(colRed), ColorsU16::Red().B());
  EXPECT_EQ(ExtractB(colGreen), ColorsU16::Green().B());
  EXPECT_EQ(ExtractB(colBlue), ColorsU16::Blue().B());
  EXPECT_EQ(ExtractB(colCyan), ColorsU16::Cyan().B());
  EXPECT_EQ(ExtractB(colYellow), ColorsU16::Yellow().B());
  EXPECT_EQ(ExtractB(colWhite), ColorsU16::White().B());
  EXPECT_EQ(ExtractB(colOrange), ColorsU16::Orange().B());
  EXPECT_EQ(ExtractB(colPink), ColorsU16::Pink().B());
  EXPECT_EQ(ExtractB(colPurple), ColorsU16::Purple().B());
  EXPECT_EQ(ExtractB(colMarrom), ColorsU16::Marrom().B());
  EXPECT_EQ(ExtractB(colBrown), ColorsU16::Brown().B());
  EXPECT_EQ(ExtractB(colOlive), ColorsU16::Olive().B());
  EXPECT_EQ(ExtractB(colSilver), ColorsU16::Silver().B());
  EXPECT_EQ(ExtractB(colDarkBlue), ColorsU16::DarkBlue().B());
  EXPECT_EQ(ExtractB(colLightBlue), ColorsU16::LightBlue().B());
  EXPECT_EQ(ExtractB(colLime), ColorsU16::Lime().B());
}


TEST(Test_ColorU16, ColorsToVector4)
{
  auto white = ColorU16(PackedColor32(0xFFFFFFFF)).ToVector4();
  auto red = ColorU16(PackedColor32(0x00FF0000)).ToVector4();
  auto green = ColorU16(PackedColor32(0x0000FF00)).ToVector4();
  auto blue = ColorU16(PackedColor32(0x000000FF)).ToVector4();

  EXPECT_FLOAT_EQ(1.0f, white.W);
  EXPECT_FLOAT_EQ(1.0f, white.X);
  EXPECT_FLOAT_EQ(1.0f, white.Y);
  EXPECT_FLOAT_EQ(1.0f, white.Z);

  EXPECT_FLOAT_EQ(0.0f, red.W);
  EXPECT_FLOAT_EQ(1.0f, red.X);
  EXPECT_FLOAT_EQ(0.0f, red.Y);
  EXPECT_FLOAT_EQ(0.0f, red.Z);

  EXPECT_FLOAT_EQ(0.0f, green.W);
  EXPECT_FLOAT_EQ(0.0f, green.X);
  EXPECT_FLOAT_EQ(1.0f, green.Y);
  EXPECT_FLOAT_EQ(0.0f, green.Z);

  EXPECT_FLOAT_EQ(0.0f, blue.W);
  EXPECT_FLOAT_EQ(0.0f, blue.X);
  EXPECT_FLOAT_EQ(0.0f, blue.Y);
  EXPECT_FLOAT_EQ(1.0f, blue.Z);
}

TEST(Test_ColorU16, ColorsToVector4B)
{
  auto white = WorkAroundConstExpr(ColorU16(PackedColor32(0xFFFFFFFF)));
  auto red = WorkAroundConstExpr(ColorU16(PackedColor32(0x00FF0000)));
  auto green = WorkAroundConstExpr(ColorU16(PackedColor32(0x0000FF00)));
  auto blue = WorkAroundConstExpr(ColorU16(PackedColor32(0x000000FF)));

  EXPECT_FLOAT_EQ(1.0f, white.W);
  EXPECT_FLOAT_EQ(1.0f, white.X);
  EXPECT_FLOAT_EQ(1.0f, white.Y);
  EXPECT_FLOAT_EQ(1.0f, white.Z);

  EXPECT_FLOAT_EQ(0.0f, red.W);
  EXPECT_FLOAT_EQ(1.0f, red.X);
  EXPECT_FLOAT_EQ(0.0f, red.Y);
  EXPECT_FLOAT_EQ(0.0f, red.Z);

  EXPECT_FLOAT_EQ(0.0f, green.W);
  EXPECT_FLOAT_EQ(0.0f, green.X);
  EXPECT_FLOAT_EQ(1.0f, green.Y);
  EXPECT_FLOAT_EQ(0.0f, green.Z);

  EXPECT_FLOAT_EQ(0.0f, blue.W);
  EXPECT_FLOAT_EQ(0.0f, blue.X);
  EXPECT_FLOAT_EQ(0.0f, blue.Y);
  EXPECT_FLOAT_EQ(1.0f, blue.Z);
}


TEST(Test_ColorU16, MultiplyRGB)
{
  for (uint32_t i = 0; i <= 0xFFFF; ++i)
  {
    const float factor = static_cast<float>(i) / static_cast<float>(0xFFFF);
    {
      ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0xFF32u, 0x8053u, 0x2077u, i));
      auto expected = MultiplyRGB(color, factor);
      auto got = ColorU16::MultiplyRGB(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
    for (uint32_t j = 0; j < (2 * 256); j += 128)
    {
      ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(j, (85 * 256) + j, (85 * 2 * 256) + j, i));
      ColorU16 expected = MultiplyRGB(color, factor);
      ColorU16 got = ColorU16::MultiplyRGB(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
  }
}

TEST(Test_ColorU16, MultiplyRGB_Constant)
{
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0xFFFF, 0, 0, 0));
    auto expected = MultiplyRGB(color, 1.0f);
    auto got = ColorU16::MultiplyRGB(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0, 0xFFFF, 0, 0));
    auto expected = MultiplyRGB(color, 1.0f);
    auto got = ColorU16::MultiplyRGB(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0, 0, 0xFFFF, 0));
    auto expected = MultiplyRGB(color, 1.0f);
    auto got = ColorU16::MultiplyRGB(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0, 0, 0, 0xFFFF));
    auto expected = MultiplyRGB(color, 1.0f);
    auto got = ColorU16::MultiplyRGB(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
}

TEST(Test_ColorU16, MultiplyRGBA)
{
  for (uint32_t i = 0; i <= 0xFFFF; ++i)
  {
    const float factor = static_cast<float>(i) / static_cast<float>(0xFFFF);
    {
      ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0xFF32u, 0x8053u, 0x2077u, i));
      auto expected = MultiplyRGBA(color, factor);
      auto got = ColorU16::MultiplyRGBA(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
    for (uint32_t j = 0; j < (2 * 256); j += 128)
    {
      ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(j, (85 * 256) + j, (85 * 2 * 256) + j, i));
      ColorU16 expected = MultiplyRGBA(color, factor);
      ColorU16 got = ColorU16::MultiplyRGBA(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
  }
}

TEST(Test_ColorU16, MultiplyRGBA_Constant)
{
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0xFFFF, 0, 0, 0));
    auto expected = MultiplyRGBA(color, 1.0f);
    auto got = ColorU16::MultiplyRGBA(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0, 0xFFFF, 0, 0));
    auto expected = MultiplyRGBA(color, 1.0f);
    auto got = ColorU16::MultiplyRGBA(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0, 0, 0xFFFF, 0));
    auto expected = MultiplyRGBA(color, 1.0f);
    auto got = ColorU16::MultiplyRGBA(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0, 0, 0, 0xFFFF));
    auto expected = MultiplyRGBA(color, 1.0f);
    auto got = ColorU16::MultiplyRGBA(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
}

TEST(Test_ColorU16, MultiplyA_float)
{
  for (uint32_t i = 0; i <= 0xFFFF; ++i)
  {
    const float factor = static_cast<float>(i) / static_cast<float>(0xFFFF);
    {
      ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0xFF32u, 0x8053u, 0x2077u, i));
      auto expected = MultiplyA(color, factor);
      auto got = ColorU16::MultiplyA(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
    for (uint32_t j = 0; j < (2 * 256); j += 128)
    {
      ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(j, (85 * 256) + j, (85 * 2 * 256) + j, i));
      ColorU16 expected = MultiplyA(color, factor);
      ColorU16 got = ColorU16::MultiplyA(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
  }
}

TEST(Test_ColorU16, MultiplyA_Constant_float)
{
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0xFFFF, 0x89AB, 0x7751, 0));
    auto expected = MultiplyA(color, 0.0f);
    auto got = ColorU16::MultiplyA(color, 0.0f);
    ASSERT_EQ(expected, got);
  }
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0xFFFF, 0x89AB, 0x7751, 0));
    auto expected = MultiplyA(color, 1.0f);
    auto got = ColorU16::MultiplyA(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0xFFFF, 0x89AB, 0x7751, 0xFFFF));
    auto expected = MultiplyA(color, 0.0f);
    auto got = ColorU16::MultiplyA(color, 0.0f);
    ASSERT_EQ(expected, got);
  }
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0xFFFF, 0x89AB, 0x7751, 0xFFFF));
    auto expected = MultiplyA(color, 1.0f);
    auto got = ColorU16::MultiplyA(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
}

TEST(Test_ColorU16, MultiplyA_ColorChannelValueU16)
{
  for (uint32_t i = 0; i <= 0xFFFF; ++i)
  {
    const ColorChannelValueU16 factor(i);
    {
      ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0xFF32u, 0x8053u, 0x2077u, i));
      auto expected = MultiplyA(color, factor);
      auto got = ColorU16::MultiplyA(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
    for (uint32_t j = 0; j < (2 * 256); j += 128)
    {
      ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(j, (85 * 256) + j, (85 * 2 * 256) + j, i));
      ColorU16 expected = MultiplyA(color, factor);
      ColorU16 got = ColorU16::MultiplyA(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
  }
}

TEST(Test_ColorU16, MultiplyA_Constant_ColorChannelValueU16)
{
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0xFFFF, 0x89AB, 0x7751, 0));
    auto expected = MultiplyA(color, ColorChannelValueU16(0));
    auto got = ColorU16::MultiplyA(color, ColorChannelValueU16(0));
    ASSERT_EQ(expected, got);
  }
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0xFFFF, 0x89AB, 0x7751, 0));
    auto expected = MultiplyA(color, ColorChannelValueU16(0xFFFF));
    auto got = ColorU16::MultiplyA(color, ColorChannelValueU16(0xFFFF));
    ASSERT_EQ(expected, got);
  }
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0xFFFF, 0x89AB, 0x7751, 0xFFFF));
    auto expected = MultiplyA(color, ColorChannelValueU16(0));
    auto got = ColorU16::MultiplyA(color, ColorChannelValueU16(0));
    ASSERT_EQ(expected, got);
  }
  {
    ColorU16 color(ColorU16::CreateR16G16B16A16UNorm(0xFFFF, 0x89AB, 0x7751, 0xFFFF));
    auto expected = MultiplyA(color, ColorChannelValueU16(0xFFFF));
    auto got = ColorU16::MultiplyA(color, ColorChannelValueU16(0xFFFF));
    ASSERT_EQ(expected, got);
  }
}


TEST(Test_ColorU16, Premultiply)
{
  for (uint32_t i = 0; i < 256; ++i)
  {
    {
      ColorU16 color(ColorU16::CreateR8G8B8A8UNorm(0xFFu, 0x80u, 0x20u, i));
      auto expectedPremultiply = Premultiply(color);
      auto gotPremultiplied = ColorU16::Premultiply(color);
      ASSERT_EQ(expectedPremultiply, gotPremultiplied);
    }
    for (uint32_t j = 0; j < 85; ++j)
    {
      ColorU16 color(ColorU16::CreateR8G8B8A8UNorm(j, 85 + j, (85 * 2) + j, i));
      ColorU16 expectedPremultiply = Premultiply(color);
      ColorU16 gotPremultiplied = ColorU16::Premultiply(color);
      if (expectedPremultiply != gotPremultiplied)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expectedPremultiply, gotPremultiplied, color);
      }
      ASSERT_EQ(expectedPremultiply, gotPremultiplied);
    }
  }
}

TEST(Test_ColorU16, Premultiply_Values)
{
  {
    ColorU16 color = ColorU16::CreateR16G16B16A16UNorm(6939u, 28784u, 50629u, 34181u);
    auto expectedPremultiply = Premultiply(color);
    auto gotPremultiplied = ColorU16::Premultiply(color);
    ASSERT_EQ(expectedPremultiply, gotPremultiplied);
  }
}

TEST(Test_ColorU16, Premultiply_Constant_Values)
{
  {
    const float alpha = static_cast<float>(109) / 255.0f;
    ColorU16 color = ColorU16::CreateR16G16B16A16UNorm(65535u, 32896u, 8224u, 5397u);
    auto expectedPremultiply = Premultiply(color, alpha);
    auto gotPremultiplied = ColorU16::Premultiply(color, alpha);
    ASSERT_EQ(expectedPremultiply, gotPremultiplied);
  }
  {
    const float alpha = 1.0f;
    ColorU16 color = ColorU16::CreateR16G16B16A16UNorm(65535u, 32896u, 8224u, 5397u);
    auto expectedPremultiply = Premultiply(color, alpha);
    auto gotPremultiplied = ColorU16::Premultiply(color, alpha);
    ASSERT_EQ(expectedPremultiply, gotPremultiplied);
  }
  {
    const float alpha = 1.0f;
    ColorU16 color = ColorU16::CreateR16G16B16A16UNorm(0xFFFFu, 32896u, 8224u, 5397u);
    auto expectedPremultiply = Premultiply(color, alpha);
    auto gotPremultiplied = ColorU16::Premultiply(color, alpha);
    ASSERT_EQ(expectedPremultiply, gotPremultiplied);
  }
  {
    const float alpha = 1.0f;
    ColorU16 color = ColorU16::CreateR16G16B16A16UNorm(65535u, 0xFFFFu, 8224u, 5397u);
    auto expectedPremultiply = Premultiply(color, alpha);
    auto gotPremultiplied = ColorU16::Premultiply(color, alpha);
    ASSERT_EQ(expectedPremultiply, gotPremultiplied);
  }
  {
    const float alpha = 1.0f;
    ColorU16 color = ColorU16::CreateR16G16B16A16UNorm(65535u, 32896u, 0xFFFFu, 5397u);
    auto expectedPremultiply = Premultiply(color, alpha);
    auto gotPremultiplied = ColorU16::Premultiply(color, alpha);
    ASSERT_EQ(expectedPremultiply, gotPremultiplied);
  }
  {
    const float alpha = 1.0f;
    ColorU16 color = ColorU16::CreateR16G16B16A16UNorm(65535u, 32896u, 8224u, 0xFFFFu);
    auto expectedPremultiply = Premultiply(color, alpha);
    auto gotPremultiplied = ColorU16::Premultiply(color, alpha);
    ASSERT_EQ(expectedPremultiply, gotPremultiplied);
  }
}

TEST(Test_ColorU16, Premultiply_Constant)
{
  for (uint32_t i = 0; i < 256; ++i)
  {
    for (uint32_t a = 0; a < 256; ++a)
    {
      const float alpha = static_cast<float>(a) / 255.0f;
      {
        ColorU16 color(ColorU16::CreateR8G8B8A8UNorm(0xFFu, 0x80u, 0x20u, i));
        auto expectedPremultiply = Premultiply(color, alpha);
        auto gotPremultiplied = ColorU16::Premultiply(color, alpha);
        if (expectedPremultiply != gotPremultiplied)
        {
          FSLLOG3_INFO("{} != {} (color: {}, alpha: {} {})", expectedPremultiply, gotPremultiplied, color, alpha, a);
        }
        ASSERT_EQ(expectedPremultiply, gotPremultiplied);
      }
    }
  }
}

TEST(Test_ColorU16, PremultiplyRGB_Constant)
{
  for (uint32_t i = 0; i < 256; ++i)
  {
    for (uint32_t a = 0; a < 256; ++a)
    {
      const float alpha = static_cast<float>(a) / 255.0f;
      {
        ColorU16 color(ColorU16::CreateR8G8B8A8UNorm(0xFFu, 0x80u, 0x20u, i));
        auto expectedPremultiply = PremultiplyRGB(color, alpha, 32u);
        auto gotPremultiplied = ColorU16::PremultiplyRGB(color, alpha, ColorChannelValueU16(32u));
        if (expectedPremultiply != gotPremultiplied)
        {
          FSLLOG3_INFO("{} != {} (color: {})", expectedPremultiply, gotPremultiplied, color);
        }
        ASSERT_EQ(expectedPremultiply, gotPremultiplied);
      }
    }
  }
}

#if 0
//TEST(Test_ColorU16, Premultiply_Constant_Random)
//{
//  std::array<uint16_t, 8> randomValues{};
//  for (const uint16_t val : randomValues)
//  {
//    // Fill with random values
//    // NOT IMPLEMENTED
//  }
//
//
//  for (const uint16_t val : randomValues)
//  {
//    for (uint32_t a = 0; a <= 0xFFFF; ++a)
//    {
//      const float alpha = static_cast<float>(a) / static_cast<float>(0xFFFF);
//      {
//        ColorU16 color = ColorU16::CreateR16G16B16A16UNorm(0xFFFFu, 0x8000u, 0x2000u, val);
//        auto expectedPremultiply = Premultiply(color, alpha);
//        auto gotPremultiplied = ColorU16::Premultiply(color, alpha);
//        ASSERT_EQ(expectedPremultiply, gotPremultiplied);
//      }
//      for (uint32_t j = 0; j < 85; ++j)
//      {
//        ColorU16 color = ColorU16::CreateR16G16B16A16UNorm(j, 85 + j, (85 * 2) + j, val);
//        auto expectedPremultiply = Premultiply(color, alpha);
//        auto gotPremultiplied = ColorU16::Premultiply(color, alpha);
//        ASSERT_EQ(expectedPremultiply, gotPremultiplied);
//      }
//    }
//  }
//}

// Basically test all alphas vs our reference implementation
// this is very slow.
TEST(Test_ColorU16, Premultiply_Constant_Heavy)
{
  //for (uint32_t i = 0; i <= 0xFFFF; ++i)
  {
    for (uint32_t a = 0; a <= 0xFFFF; ++a)
    {
      const float alpha = static_cast<float>(a) / static_cast<float>(0xFFFF);
      {
        ColorU16 color = ColorU16::CreateR16G16B16A16UNorm(0xFFFFu, 0x8000u, 0x2000u, i);
        auto expectedPremultiply = Premultiply(color, alpha);
        auto gotPremultiplied = ColorU16::Premultiply(color, alpha);
        ASSERT_EQ(expectedPremultiply, gotPremultiplied);
      }
      for (uint32_t j = 0; j < 85; ++j)
      {
        ColorU16 color = ColorU16::CreateR16G16B16A16UNorm(j, 85 + j, (85 * 2) + j, i);
        auto expectedPremultiply = Premultiply(color, alpha);
        auto gotPremultiplied = ColorU16::Premultiply(color, alpha);
        ASSERT_EQ(expectedPremultiply, gotPremultiplied);
      }
    }
  }
}
#endif

TEST(Test_ColorU16, Equals)
{
  EXPECT_EQ(ColorsU16::DarkBlue(), ColorsU16::DarkBlue());
  EXPECT_EQ(ColorsU16::Red(), ColorsU16::Red());

  ColorU16 col1(PackedColor64(0x12345678ABCDEF12));
  ColorU16 col2(PackedColor64(0x12345678ABCDEF13));
  EXPECT_EQ(col1, col1);
  EXPECT_EQ(col2, col2);
}


TEST(Test_ColorU16, NotEquals)
{
  EXPECT_NE(ColorsU16::DarkBlue(), ColorsU16::Red());
  EXPECT_NE(ColorsU16::Red(), ColorsU16::DarkBlue());

  ColorU16 col1(PackedColor64(0x12345678ABCDEF12));
  ColorU16 col2(PackedColor64(0x12345678ABCDEF13));
  ColorU16 col3(PackedColor64(0x22345678ABCDEF12));
  EXPECT_NE(col1, col2);
  EXPECT_NE(col2, col1);
  EXPECT_NE(col1, col3);
}


TEST(Test_ColorU16, Bilinear_Edges)
{
  EXPECT_EQ(ColorU16(PackedColor32(0x10203040)),
            ColorU16::Bilinear(ColorU16(PackedColor32(0x10203040)), ColorU16(PackedColor32(0xFFFFFFFF)), ColorU16(PackedColor32(0xFFFFFFFF)),
                               ColorU16(PackedColor32(0xFFFFFFFF)), 0.0f, 0.0f));
  EXPECT_EQ(ColorU16(PackedColor32(0x10203040)),
            ColorU16::Bilinear(ColorU16(PackedColor32(0xFFFFFFFF)), ColorU16(PackedColor32(0x10203040)), ColorU16(PackedColor32(0xFFFFFFFF)),
                               ColorU16(PackedColor32(0xFFFFFFFF)), 1.0f, 0.0f));
  EXPECT_EQ(ColorU16(PackedColor32(0x10203040)),
            ColorU16::Bilinear(ColorU16(PackedColor32(0xFFFFFFFF)), ColorU16(PackedColor32(0xFFFFFFFF)), ColorU16(PackedColor32(0x10203040)),
                               ColorU16(PackedColor32(0xFFFFFFFF)), 0.0f, 1.0f));
  EXPECT_EQ(ColorU16(PackedColor32(0x10203040)),
            ColorU16::Bilinear(ColorU16(PackedColor32(0xFFFFFFFF)), ColorU16(PackedColor32(0xFFFFFFFF)), ColorU16(PackedColor32(0xFFFFFFFF)),
                               ColorU16(PackedColor32(0x10203040)), 1.0f, 1.0f));
}

TEST(Test_ColorU16, Bilinear_Edges2)
{
  EXPECT_EQ(ColorU16(PackedColor32(0x18304860)),
            ColorU16::Bilinear(ColorU16(PackedColor32(0x10203040)), ColorU16(PackedColor32(0x20406080)), ColorU16(PackedColor32(0xFFFFFFFF)),
                               ColorU16(PackedColor32(0xFFFFFFFF)), 0.5f, 0.0f));
  EXPECT_EQ(ColorU16(PackedColor32(0x18304860)),
            ColorU16::Bilinear(ColorU16(PackedColor32(0xFFFFFFFF)), ColorU16(PackedColor32(0xFFFFFFFF)), ColorU16(PackedColor32(0x10203040)),
                               ColorU16(PackedColor32(0x20406080)), 0.5f, 1.0f));
  EXPECT_EQ(ColorU16(PackedColor32(0x18304860)),
            ColorU16::Bilinear(ColorU16(PackedColor32(0x10203040)), ColorU16(PackedColor32(0xFFFFFFFF)), ColorU16(PackedColor32(0x20406080)),
                               ColorU16(PackedColor32(0xFFFFFFFF)), 0.0f, 0.5f));
  EXPECT_EQ(ColorU16(PackedColor32(0x18304860)),
            ColorU16::Bilinear(ColorU16(PackedColor32(0xFFFFFFFF)), ColorU16(PackedColor32(0x10203040)), ColorU16(PackedColor32(0xFFFFFFFF)),
                               ColorU16(PackedColor32(0x20406080)), 1.0f, 0.5f));
}
