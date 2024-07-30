/****************************************************************************************************************************************************
 * Copyright 2018, 2022, 2024 NXP
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
#include <FslGraphics/Color.hpp>
#include <FslGraphics/ColorChannelConverter.hpp>
#include <FslGraphics/ColorChannelValueU8.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslGraphics/Log/FmtColor.hpp>
#include <FslGraphics/Log/LogColor.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>
#include <cmath>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using Test_Color = TestFixtureFslGraphics;

  uint8_t CreateU8(const float value)
  {
    const float val = MathHelper::Clamp(value, 0.0f, 1.0f);
    return static_cast<uint8_t>(std::round(val * static_cast<float>(0xFF)));
  }

  constexpr ColorChannelValueU8 ExtractA(const uint32_t value)
  {
    return ColorChannelValueU8((value >> 24u) & 0xFF);
  }

  constexpr ColorChannelValueU8 ExtractR(const uint32_t value)
  {
    return ColorChannelValueU8((value >> 16u) & 0xFF);
  }

  constexpr ColorChannelValueU8 ExtractG(const uint32_t value)
  {
    return ColorChannelValueU8((value >> 8u) & 0xFF);
  }

  constexpr ColorChannelValueU8 ExtractB(const uint32_t value)
  {
    return ColorChannelValueU8(value & 0xFF);
  }

  // For coverage purpose ensure that ToVector4 gets called
  Vector4 WorkAroundConstExpr(const Color& color)
  {
    return color.ToVector4();
  }


  Color MultiplyRGB(const Color color, const float factor)
  {
    double alpha = MathHelper::Clamp(factor, 0.0f, 1.0f);
    auto r = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawR()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    auto g = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawG()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    auto b = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawB()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    return Color::CreateR8G8B8A8UNorm(r, g, b, static_cast<uint32_t>(color.RawA()));
  }

  Color MultiplyRGBA(const Color color, const float factor)
  {
    double alpha = MathHelper::Clamp(factor, 0.0f, 1.0f);
    auto r = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawR()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    auto g = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawG()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    auto b = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawB()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    auto a = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawA()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    return Color::CreateR8G8B8A8UNorm(r, g, b, a);
  }

  Color MultiplyA(const Color color, const float factor)
  {
    double alpha = MathHelper::Clamp(factor, 0.0f, 1.0f);
    auto a = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawA()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    return Color::CreateR8G8B8A8UNorm(color.RawR(), color.RawG(), color.RawB(), NumericCast<uint8_t>(a));
  }

  Color MultiplyA(const Color color, const ColorChannelValueU8 factor)
  {
    double alpha = static_cast<double>(factor.RawValue) / static_cast<double>(0xFF);
    auto a = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawA()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    return Color::CreateR8G8B8A8UNorm(color.RawR(), color.RawG(), color.RawB(), NumericCast<uint8_t>(a));
  }

  Color Premultiply(const Color color)
  {
    double alpha = static_cast<double>(color.RawA()) / static_cast<double>(0xFF);
    auto r = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawR()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    auto g = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawG()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    auto b = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawB()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    return Color::CreateR8G8B8A8UNorm(r, g, b, static_cast<uint32_t>(color.RawA()));
  }

  Color Premultiply(const Color color, const float alphaMul)
  {
    double alpha = (static_cast<double>(color.RawA()) / static_cast<double>(0xFF)) * MathHelper::Clamp(alphaMul, 0.0f, 100.0f);
    auto r = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawR()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    auto g = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawG()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    auto b = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawB()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    auto a = NumericCast<uint32_t>(
      MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(0xFF))), static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    return Color::CreateR8G8B8A8UNorm(r, g, b, a);
  }

  Color PremultiplyRGB(const Color color, const float alphaMul, const uint32_t newAlpha)
  {
    double alpha = (static_cast<double>(color.RawA()) / static_cast<double>(0xFF)) * MathHelper::Clamp(alphaMul, 0.0f, 100.0f);
    auto r = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawR()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    auto g = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawG()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    auto b = NumericCast<uint32_t>(MathHelper::Clamp(static_cast<int64_t>(std::round(alpha * static_cast<double>(color.RawB()))),
                                                     static_cast<int64_t>(0), static_cast<int64_t>(0xFF)));
    return Color::CreateR8G8B8A8UNorm(r, g, b, newAlpha);
  }
}


TEST(Test_Color, Construct_Empty)
{
  Color color;

  EXPECT_EQ(0u, color.RawR());
  EXPECT_EQ(0u, color.RawG());
  EXPECT_EQ(0u, color.RawB());
  EXPECT_EQ(0u, color.RawA());
  EXPECT_EQ(0u, color.AsPackedColor32().RawValue);
  EXPECT_EQ(0u, color.AsPackedColor64().RawValue);
}


TEST(Test_Color, Construct_Float)
{
  Color colWhite(1.0f, 1.0f, 1.0f, 1.0f);
  Color colR(1.0f, 0.0f, 0.0f, 0.0f);
  Color colG(0.0f, 1.0f, 0.0f, 0.0f);
  Color colB(0.0f, 0.0f, 1.0f, 0.0f);
  Color colA(0.0f, 0.0f, 0.0f, 1.0f);
  Color colMisc(0.5f, 0.5f, 0.5f, 0.5f);

  EXPECT_EQ(0xFFu, colWhite.RawR());
  EXPECT_EQ(0xFFu, colWhite.RawG());
  EXPECT_EQ(0xFFu, colWhite.RawB());
  EXPECT_EQ(0xFFu, colWhite.RawA());
  EXPECT_EQ(0xFFFFFFFFu, colWhite.AsPackedColor32().RawValue);
  EXPECT_EQ(0xFFFFFFFFFFFFFFFFu, colWhite.AsPackedColor64().RawValue);

  EXPECT_EQ(0xFFu, colR.RawR());
  EXPECT_EQ(0x00u, colR.RawG());
  EXPECT_EQ(0x00u, colR.RawB());
  EXPECT_EQ(0x00u, colR.RawA());
  EXPECT_EQ(0x00FF0000u, colR.AsPackedColor32().RawValue);
  EXPECT_EQ(0x0000FFFF00000000u, colR.AsPackedColor64().RawValue);

  EXPECT_EQ(0x00u, colG.RawR());
  EXPECT_EQ(0xFFu, colG.RawG());
  EXPECT_EQ(0x00u, colG.RawB());
  EXPECT_EQ(0x00u, colG.RawA());
  EXPECT_EQ(0x0000FF00u, colG.AsPackedColor32().RawValue);
  EXPECT_EQ(0x00000000FFFF0000u, colG.AsPackedColor64().RawValue);

  EXPECT_EQ(0x00u, colB.RawR());
  EXPECT_EQ(0x00u, colB.RawG());
  EXPECT_EQ(0xFFu, colB.RawB());
  EXPECT_EQ(0x00u, colB.RawA());
  EXPECT_EQ(0x000000FFu, colB.AsPackedColor32().RawValue);
  EXPECT_EQ(0x000000000000FFFFu, colB.AsPackedColor64().RawValue);

  EXPECT_EQ(0x00u, colA.RawR());
  EXPECT_EQ(0x00u, colA.RawG());
  EXPECT_EQ(0x00u, colA.RawB());
  EXPECT_EQ(0xFFu, colA.RawA());
  EXPECT_EQ(0xFF000000u, colA.AsPackedColor32().RawValue);
  EXPECT_EQ(0xFFFF000000000000u, colA.AsPackedColor64().RawValue);

  EXPECT_EQ(0x80u, colMisc.RawR());
  EXPECT_EQ(0x80u, colMisc.RawG());
  EXPECT_EQ(0x80u, colMisc.RawB());
  EXPECT_EQ(0x80u, colMisc.RawA());
  EXPECT_EQ(0x80808080u, colMisc.AsPackedColor32().RawValue);
  EXPECT_EQ(0x8080808080808080u, colMisc.AsPackedColor64().RawValue);
}


TEST(Test_Color, Construct_Floats)
{
  for (uint32_t i = 0; i <= 0x2FFFF; ++i)
  {
    const float value = static_cast<float>(i) / static_cast<float>(0x2FFFF);

    const uint16_t expected = CreateU8(value);
    const uint16_t maxValue = 0xFF;
    Color expectedR = Color::CreateR8G8B8A8UNorm(expected, maxValue, maxValue, maxValue);
    Color expectedG = Color::CreateR8G8B8A8UNorm(maxValue, expected, maxValue, maxValue);
    Color expectedB = Color::CreateR8G8B8A8UNorm(maxValue, maxValue, expected, maxValue);
    Color expectedA = Color::CreateR8G8B8A8UNorm(maxValue, maxValue, maxValue, expected);

    Color gotR = Color(value, 1.0f, 1.0f, 1.0f);
    ASSERT_EQ(expectedR, gotR);
    Color gotG = Color(1.0f, value, 1.0f, 1.0f);
    ASSERT_EQ(expectedG, gotG);
    Color gotB = Color(1.0f, 1.0f, value, 1.0f);
    ASSERT_EQ(expectedB, gotB);
    Color gotA = Color(1.0f, 1.0f, 1.0f, value);
    ASSERT_EQ(expectedA, gotA);
  }
}

TEST(Test_Color, Colors)
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

  EXPECT_EQ(colTransparent, Colors::Transparent().AsPackedColor32().RawValue);
  EXPECT_EQ(colBlack, Colors::Black().AsPackedColor32().RawValue);
  EXPECT_EQ(colRed, Colors::Red().AsPackedColor32().RawValue);
  EXPECT_EQ(colGreen, Colors::Green().AsPackedColor32().RawValue);
  EXPECT_EQ(colBlue, Colors::Blue().AsPackedColor32().RawValue);
  EXPECT_EQ(colCyan, Colors::Cyan().AsPackedColor32().RawValue);
  EXPECT_EQ(colYellow, Colors::Yellow().AsPackedColor32().RawValue);
  EXPECT_EQ(colWhite, Colors::White().AsPackedColor32().RawValue);
  EXPECT_EQ(colOrange, Colors::Orange().AsPackedColor32().RawValue);
  EXPECT_EQ(colPink, Colors::Pink().AsPackedColor32().RawValue);
  EXPECT_EQ(colPurple, Colors::Purple().AsPackedColor32().RawValue);
  EXPECT_EQ(colMarrom, Colors::Marrom().AsPackedColor32().RawValue);
  EXPECT_EQ(colBrown, Colors::Brown().AsPackedColor32().RawValue);
  EXPECT_EQ(colOlive, Colors::Olive().AsPackedColor32().RawValue);
  EXPECT_EQ(colSilver, Colors::Silver().AsPackedColor32().RawValue);
  EXPECT_EQ(colDarkBlue, Colors::DarkBlue().AsPackedColor32().RawValue);
  EXPECT_EQ(colLightBlue, Colors::LightBlue().AsPackedColor32().RawValue);
  EXPECT_EQ(colLime, Colors::Lime().AsPackedColor32().RawValue);

  // Check alpha channel
  EXPECT_EQ(ExtractA(colTransparent), Colors::Transparent().A());
  EXPECT_EQ(ExtractA(colBlack), Colors::Black().A());
  EXPECT_EQ(ExtractA(colRed), Colors::Red().A());
  EXPECT_EQ(ExtractA(colGreen), Colors::Green().A());
  EXPECT_EQ(ExtractA(colBlue), Colors::Blue().A());
  EXPECT_EQ(ExtractA(colCyan), Colors::Cyan().A());
  EXPECT_EQ(ExtractA(colYellow), Colors::Yellow().A());
  EXPECT_EQ(ExtractA(colWhite), Colors::White().A());
  EXPECT_EQ(ExtractA(colOrange), Colors::Orange().A());
  EXPECT_EQ(ExtractA(colPink), Colors::Pink().A());
  EXPECT_EQ(ExtractA(colPurple), Colors::Purple().A());
  EXPECT_EQ(ExtractA(colMarrom), Colors::Marrom().A());
  EXPECT_EQ(ExtractA(colBrown), Colors::Brown().A());
  EXPECT_EQ(ExtractA(colOlive), Colors::Olive().A());
  EXPECT_EQ(ExtractA(colSilver), Colors::Silver().A());
  EXPECT_EQ(ExtractA(colDarkBlue), Colors::DarkBlue().A());
  EXPECT_EQ(ExtractA(colLightBlue), Colors::LightBlue().A());
  EXPECT_EQ(ExtractA(colLime), Colors::Lime().A());

  // Red
  EXPECT_EQ(ExtractR(colTransparent), Colors::Transparent().R());
  EXPECT_EQ(ExtractR(colBlack), Colors::Black().R());
  EXPECT_EQ(ExtractR(colRed), Colors::Red().R());
  EXPECT_EQ(ExtractR(colGreen), Colors::Green().R());
  EXPECT_EQ(ExtractR(colBlue), Colors::Blue().R());
  EXPECT_EQ(ExtractR(colCyan), Colors::Cyan().R());
  EXPECT_EQ(ExtractR(colYellow), Colors::Yellow().R());
  EXPECT_EQ(ExtractR(colWhite), Colors::White().R());
  EXPECT_EQ(ExtractR(colOrange), Colors::Orange().R());
  EXPECT_EQ(ExtractR(colPink), Colors::Pink().R());
  EXPECT_EQ(ExtractR(colPurple), Colors::Purple().R());
  EXPECT_EQ(ExtractR(colMarrom), Colors::Marrom().R());
  EXPECT_EQ(ExtractR(colBrown), Colors::Brown().R());
  EXPECT_EQ(ExtractR(colOlive), Colors::Olive().R());
  EXPECT_EQ(ExtractR(colSilver), Colors::Silver().R());
  EXPECT_EQ(ExtractR(colDarkBlue), Colors::DarkBlue().R());
  EXPECT_EQ(ExtractR(colLightBlue), Colors::LightBlue().R());
  EXPECT_EQ(ExtractR(colLime), Colors::Lime().R());

  // Green
  EXPECT_EQ(ExtractG(colTransparent), Colors::Transparent().G());
  EXPECT_EQ(ExtractG(colBlack), Colors::Black().G());
  EXPECT_EQ(ExtractG(colRed), Colors::Red().G());
  EXPECT_EQ(ExtractG(colGreen), Colors::Green().G());
  EXPECT_EQ(ExtractG(colBlue), Colors::Blue().G());
  EXPECT_EQ(ExtractG(colCyan), Colors::Cyan().G());
  EXPECT_EQ(ExtractG(colYellow), Colors::Yellow().G());
  EXPECT_EQ(ExtractG(colWhite), Colors::White().G());
  EXPECT_EQ(ExtractG(colOrange), Colors::Orange().G());
  EXPECT_EQ(ExtractG(colPink), Colors::Pink().G());
  EXPECT_EQ(ExtractG(colPurple), Colors::Purple().G());
  EXPECT_EQ(ExtractG(colMarrom), Colors::Marrom().G());
  EXPECT_EQ(ExtractG(colBrown), Colors::Brown().G());
  EXPECT_EQ(ExtractG(colOlive), Colors::Olive().G());
  EXPECT_EQ(ExtractG(colSilver), Colors::Silver().G());
  EXPECT_EQ(ExtractG(colDarkBlue), Colors::DarkBlue().G());
  EXPECT_EQ(ExtractG(colLightBlue), Colors::LightBlue().G());
  EXPECT_EQ(ExtractG(colLime), Colors::Lime().G());

  // Blue
  EXPECT_EQ(ExtractB(colTransparent), Colors::Transparent().B());
  EXPECT_EQ(ExtractB(colBlack), Colors::Black().B());
  EXPECT_EQ(ExtractB(colRed), Colors::Red().B());
  EXPECT_EQ(ExtractB(colGreen), Colors::Green().B());
  EXPECT_EQ(ExtractB(colBlue), Colors::Blue().B());
  EXPECT_EQ(ExtractB(colCyan), Colors::Cyan().B());
  EXPECT_EQ(ExtractB(colYellow), Colors::Yellow().B());
  EXPECT_EQ(ExtractB(colWhite), Colors::White().B());
  EXPECT_EQ(ExtractB(colOrange), Colors::Orange().B());
  EXPECT_EQ(ExtractB(colPink), Colors::Pink().B());
  EXPECT_EQ(ExtractB(colPurple), Colors::Purple().B());
  EXPECT_EQ(ExtractB(colMarrom), Colors::Marrom().B());
  EXPECT_EQ(ExtractB(colBrown), Colors::Brown().B());
  EXPECT_EQ(ExtractB(colOlive), Colors::Olive().B());
  EXPECT_EQ(ExtractB(colSilver), Colors::Silver().B());
  EXPECT_EQ(ExtractB(colDarkBlue), Colors::DarkBlue().B());
  EXPECT_EQ(ExtractB(colLightBlue), Colors::LightBlue().B());
  EXPECT_EQ(ExtractB(colLime), Colors::Lime().B());
}


TEST(Test_Color, ColorsToVector4)
{
  auto white = Color(0xFFFFFFFF).ToVector4();
  auto red = Color(0x00FF0000).ToVector4();
  auto green = Color(0x0000FF00).ToVector4();
  auto blue = Color(0x000000FF).ToVector4();

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

TEST(Test_Color, ColorsToVector4B)
{
  auto white = WorkAroundConstExpr(Color(0xFFFFFFFF));
  auto red = WorkAroundConstExpr(Color(0x00FF0000));
  auto green = WorkAroundConstExpr(Color(0x0000FF00));
  auto blue = WorkAroundConstExpr(Color(0x000000FF));

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


TEST(Test_ColorU, MultiplyRGB)
{
  for (uint32_t i = 0; i <= 0xFF; ++i)
  {
    const float factor = static_cast<float>(i) / static_cast<float>(0xFF);
    {
      Color color(Color::CreateR8G8B8A8UNorm(0xFFu, 0x80u, 0x20u, i));
      auto expected = MultiplyRGB(color, factor);
      auto got = Color::MultiplyRGB(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
    for (uint32_t j = 0; j < 10; ++j)
    {
      Color color(Color::CreateR8G8B8A8UNorm(j, 85 + j, 85 * 2 + j, i));
      Color expected = MultiplyRGB(color, factor);
      Color got = Color::MultiplyRGB(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
  }
}

TEST(Test_Color, MultiplyRGB_Constant)
{
  {
    Color color(Color::CreateR8G8B8A8UNorm(0xFF, 0, 0, 0));
    auto expected = MultiplyRGB(color, 1.0f);
    auto got = Color::MultiplyRGB(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
  {
    Color color(Color::CreateR8G8B8A8UNorm(0, 0xFF, 0, 0));
    auto expected = MultiplyRGB(color, 1.0f);
    auto got = Color::MultiplyRGB(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
  {
    Color color(Color::CreateR8G8B8A8UNorm(0, 0, 0xFF, 0));
    auto expected = MultiplyRGB(color, 1.0f);
    auto got = Color::MultiplyRGB(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
  {
    Color color(Color::CreateR8G8B8A8UNorm(0, 0, 0, 0xFF));
    auto expected = MultiplyRGB(color, 1.0f);
    auto got = Color::MultiplyRGB(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
}

TEST(Test_Color, MultiplyRGBA)
{
  for (uint32_t i = 0; i <= 0xFF; ++i)
  {
    const float factor = static_cast<float>(i) / static_cast<float>(0xFF);
    {
      Color color(Color::CreateR8G8B8A8UNorm(0xFFu, 0x80u, 0x20u, i));
      auto expected = MultiplyRGBA(color, factor);
      auto got = Color::MultiplyRGBA(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
    for (uint32_t j = 0; j < 32; ++j)
    {
      Color color(Color::CreateR8G8B8A8UNorm(j, 85 + j, (85 * 2) + j, i));
      Color expected = MultiplyRGBA(color, factor);
      Color got = Color::MultiplyRGBA(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
  }
}

TEST(Test_Color, MultiplyRGBA_Constant)
{
  {
    Color color(Color::CreateR8G8B8A8UNorm(0xFF, 0, 0, 0));
    auto expected = MultiplyRGBA(color, 1.0f);
    auto got = Color::MultiplyRGBA(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
  {
    Color color(Color::CreateR8G8B8A8UNorm(0, 0xFF, 0, 0));
    auto expected = MultiplyRGBA(color, 1.0f);
    auto got = Color::MultiplyRGBA(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
  {
    Color color(Color::CreateR8G8B8A8UNorm(0, 0, 0xFF, 0));
    auto expected = MultiplyRGBA(color, 1.0f);
    auto got = Color::MultiplyRGBA(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
  {
    Color color(Color::CreateR8G8B8A8UNorm(0, 0, 0, 0xFF));
    auto expected = MultiplyRGBA(color, 1.0f);
    auto got = Color::MultiplyRGBA(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
}

TEST(Test_Color, MultiplyA_float)
{
  for (uint32_t i = 0; i <= 0xFF; ++i)
  {
    const float factor = static_cast<float>(i) / static_cast<float>(0xFF);
    {
      Color color(Color::CreateR8G8B8A8UNorm(0xFFu, 0x80u, 0x20u, i));
      auto expected = MultiplyA(color, factor);
      auto got = Color::MultiplyA(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
    for (uint32_t j = 0; j < 32; ++j)
    {
      Color color(Color::CreateR8G8B8A8UNorm(j, (85) + j, (85 * 2) + j, i));
      Color expected = MultiplyA(color, factor);
      Color got = Color::MultiplyA(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
  }
}

TEST(Test_Color, MultiplyA_Constant_float)
{
  {
    Color color(Color::CreateR8G8B8A8UNorm(0xFF, 0x89, 0x77, 0));
    auto expected = MultiplyA(color, 0.0f);
    auto got = Color::MultiplyA(color, 0.0f);
    ASSERT_EQ(expected, got);
  }
  {
    Color color(Color::CreateR8G8B8A8UNorm(0xFF, 0x89, 0x77, 0));
    auto expected = MultiplyA(color, 1.0f);
    auto got = Color::MultiplyA(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
  {
    Color color(Color::CreateR8G8B8A8UNorm(0xFF, 0x89, 0x77, 0xFF));
    auto expected = MultiplyA(color, 0.0f);
    auto got = Color::MultiplyA(color, 0.0f);
    ASSERT_EQ(expected, got);
  }
  {
    Color color(Color::CreateR8G8B8A8UNorm(0xFF, 0x89, 0x77, 0xFF));
    auto expected = MultiplyA(color, 1.0f);
    auto got = Color::MultiplyA(color, 1.0f);
    ASSERT_EQ(expected, got);
  }
}

TEST(Test_Color, MultiplyA_ColorChannelValueU16)
{
  for (uint32_t i = 0; i <= 0xFF; ++i)
  {
    const ColorChannelValueU8 factor(i);
    {
      Color color(Color::CreateR8G8B8A8UNorm(0xFF32u, 0x8053u, 0x2077u, i));
      auto expected = MultiplyA(color, factor);
      auto got = Color::MultiplyA(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
    for (uint32_t j = 0; j < 32; ++j)
    {
      Color color(Color::CreateR8G8B8A8UNorm(j, 85 + j, (85 * 2) + j, i));
      Color expected = MultiplyA(color, factor);
      Color got = Color::MultiplyA(color, factor);
      if (expected != got)
      {
        FSLLOG3_INFO("{} != {} (color: {})", expected, got, color);
      }
      ASSERT_EQ(expected, got);
    }
  }
}

TEST(Test_Color, MultiplyA_Constant_ColorChannelValueU8)
{
  {
    Color color(Color::CreateR8G8B8A8UNorm(0xFF, 0x89, 0x77, 0));
    auto expected = MultiplyA(color, ColorChannelValueU8(0));
    auto got = Color::MultiplyA(color, ColorChannelValueU8(0));
    ASSERT_EQ(expected, got);
  }
  {
    Color color(Color::CreateR8G8B8A8UNorm(0xFF, 0x89, 0x77, 0));
    auto expected = MultiplyA(color, ColorChannelValueU8(0xFF));
    auto got = Color::MultiplyA(color, ColorChannelValueU8(0xFF));
    ASSERT_EQ(expected, got);
  }
  {
    Color color(Color::CreateR8G8B8A8UNorm(0xFF, 0x89, 0x77, 0xFF));
    auto expected = MultiplyA(color, ColorChannelValueU8(0));
    auto got = Color::MultiplyA(color, ColorChannelValueU8(0));
    ASSERT_EQ(expected, got);
  }
  {
    Color color(Color::CreateR8G8B8A8UNorm(0xFF, 0x89, 0x77, 0xFF));
    auto expected = MultiplyA(color, ColorChannelValueU8(0xFF));
    auto got = Color::MultiplyA(color, ColorChannelValueU8(0xFF));
    ASSERT_EQ(expected, got);
  }
}


TEST(Test_Color, Premultiply)
{
  for (uint32_t i = 0; i < 256; ++i)
  {
    {
      Color color(0xFFu, 0x80u, 0x20u, i);
      auto expectedPremultiply = Premultiply(color);
      auto gotPremultiplied = Color::Premultiply(color);
      ASSERT_EQ(expectedPremultiply, gotPremultiplied);
    }
    for (uint32_t j = 0; j < 85; ++j)
    {
      Color color(j, 85 + j, (85 * 2) + j, i);
      auto expectedPremultiply = Premultiply(color);
      auto gotPremultiplied = Color::Premultiply(color);
      ASSERT_EQ(expectedPremultiply, gotPremultiplied);
    }
  }
}


TEST(Test_Color, Premultiply_Values)
{
  {
    Color color = Color::CreateR8G8B8A8UNorm(69u, 247u, 50u, 241u);
    auto expectedPremultiply = Premultiply(color);
    auto gotPremultiplied = Color::Premultiply(color);
    ASSERT_EQ(expectedPremultiply, gotPremultiplied);
  }
}

TEST(Test_Color, Premultiply_Constant_Values)
{
  {
    const float alpha = static_cast<float>(109) / 255.0f;
    Color color = Color::CreateR8G8B8A8UNorm(69u, 247u, 50u, 241u);
    auto expectedPremultiply = Premultiply(color, alpha);
    auto gotPremultiplied = Color::Premultiply(color, alpha);
    ASSERT_EQ(expectedPremultiply, gotPremultiplied);
  }
  {
    const float alpha = 1.0f;
    Color color = Color::CreateR8G8B8A8UNorm(69u, 247u, 50u, 241u);
    auto expectedPremultiply = Premultiply(color, alpha);
    auto gotPremultiplied = Color::Premultiply(color, alpha);
    ASSERT_EQ(expectedPremultiply, gotPremultiplied);
  }
  {
    const float alpha = 1.0f;
    Color color = Color::CreateR8G8B8A8UNorm(0xFFu, 247u, 50u, 241u);
    auto expectedPremultiply = Premultiply(color, alpha);
    auto gotPremultiplied = Color::Premultiply(color, alpha);
    ASSERT_EQ(expectedPremultiply, gotPremultiplied);
  }
  {
    const float alpha = 1.0f;
    Color color = Color::CreateR8G8B8A8UNorm(69u, 0xFFu, 50u, 241u);
    auto expectedPremultiply = Premultiply(color, alpha);
    auto gotPremultiplied = Color::Premultiply(color, alpha);
    ASSERT_EQ(expectedPremultiply, gotPremultiplied);
  }
  {
    const float alpha = 1.0f;
    Color color = Color::CreateR8G8B8A8UNorm(69u, 247u, 0xFFu, 241u);
    auto expectedPremultiply = Premultiply(color, alpha);
    auto gotPremultiplied = Color::Premultiply(color, alpha);
    ASSERT_EQ(expectedPremultiply, gotPremultiplied);
  }
  {
    const float alpha = 1.0f;
    Color color = Color::CreateR8G8B8A8UNorm(69u, 247u, 50u, 0xFFu);
    auto expectedPremultiply = Premultiply(color, alpha);
    auto gotPremultiplied = Color::Premultiply(color, alpha);
    ASSERT_EQ(expectedPremultiply, gotPremultiplied);
  }
}

TEST(Test_Color, Premultiply_Constant)
{
  for (uint32_t i = 0; i < 256; ++i)
  {
    for (uint32_t a = 0; a < 256; ++a)
    {
      const float alpha = static_cast<float>(a) / 255.0f;
      {
        Color color(0xFFu, 0x80u, 0x20u, i);
        auto expectedPremultiply = Premultiply(color, alpha);
        auto gotPremultiplied = Color::Premultiply(color, alpha);
        if (expectedPremultiply != gotPremultiplied)
        {
          FSLLOG3_INFO("{} != {} (color: {}, alpha: {} {})", expectedPremultiply, gotPremultiplied, color, alpha, a);
        }
        ASSERT_EQ(expectedPremultiply, gotPremultiplied);
      }
    }
  }
}

TEST(Test_Color, PremultiplyRGB_Constant)
{
  for (uint32_t i = 0; i < 256; ++i)
  {
    for (uint32_t a = 0; a < 256; ++a)
    {
      const float alpha = static_cast<float>(a) / 255.0f;
      {
        Color color(0xFFu, 0x80u, 0x20u, i);
        auto expectedPremultiply = PremultiplyRGB(color, alpha, 32u);
        auto gotPremultiplied = Color::PremultiplyRGB(color, alpha, 32u);
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
// Basically test all alphas vs our reference implementation
// this is very slow.
TEST(Test_Color, Premultiply_Constant_Heavy)
{
  for (uint32_t i = 0; i < 256; ++i)
  {
    for (uint32_t a = 0; a < 256; ++a)
    {
      const float alpha = static_cast<float>(a) / 255.0f;
      {
        Color color(0xFFu, 0x80u, 0x20u, i);
        auto expectedPremultiply = Premultiply(color, alpha);
        auto gotPremultiplied = Color::Premultiply(color, alpha);
        ASSERT_EQ(expectedPremultiply, gotPremultiplied);
      }
      for (uint32_t j = 0; j < 85; ++j)
      {
        Color color(j, 85 + j, (85 * 2) + j, i);
        auto expectedPremultiply = Premultiply(color, alpha);
        auto gotPremultiplied = Color::Premultiply(color, alpha);
        ASSERT_EQ(expectedPremultiply, gotPremultiplied);
      }
    }
  }
}
#endif

TEST(Test_Color, Equals)
{
  EXPECT_EQ(Colors::DarkBlue(), Colors::DarkBlue());
  EXPECT_EQ(Colors::Red(), Colors::Red());
}


TEST(Test_Color, NotEquals)
{
  EXPECT_NE(Colors::DarkBlue(), Colors::Red());
  EXPECT_NE(Colors::Red(), Colors::DarkBlue());
}


TEST(Test_Color, Bilinear_Edges)
{
  EXPECT_EQ(Color(0x10203040), Color::Bilinear(Color(0x10203040), Color(0xFFFFFFFF), Color(0xFFFFFFFF), Color(0xFFFFFFFF), 0.0f, 0.0f));
  EXPECT_EQ(Color(0x10203040), Color::Bilinear(Color(0xFFFFFFFF), Color(0x10203040), Color(0xFFFFFFFF), Color(0xFFFFFFFF), 1.0f, 0.0f));
  EXPECT_EQ(Color(0x10203040), Color::Bilinear(Color(0xFFFFFFFF), Color(0xFFFFFFFF), Color(0x10203040), Color(0xFFFFFFFF), 0.0f, 1.0f));
  EXPECT_EQ(Color(0x10203040), Color::Bilinear(Color(0xFFFFFFFF), Color(0xFFFFFFFF), Color(0xFFFFFFFF), Color(0x10203040), 1.0f, 1.0f));
}

TEST(Test_Color, Bilinear_Edges2)
{
  EXPECT_EQ(Color(0x18304860), Color::Bilinear(Color(0x10203040), Color(0x20406080), Color(0xFFFFFFFF), Color(0xFFFFFFFF), 0.5f, 0.0f));
  EXPECT_EQ(Color(0x18304860), Color::Bilinear(Color(0xFFFFFFFF), Color(0xFFFFFFFF), Color(0x10203040), Color(0x20406080), 0.5f, 1.0f));
  EXPECT_EQ(Color(0x18304860), Color::Bilinear(Color(0x10203040), Color(0xFFFFFFFF), Color(0x20406080), Color(0xFFFFFFFF), 0.0f, 0.5f));
  EXPECT_EQ(Color(0x18304860), Color::Bilinear(Color(0xFFFFFFFF), Color(0x10203040), Color(0xFFFFFFFF), Color(0x20406080), 1.0f, 0.5f));
}
