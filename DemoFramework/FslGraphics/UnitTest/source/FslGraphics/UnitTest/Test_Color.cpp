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

#include <FslGraphics/Color.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslGraphics/Log/LogColor.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>
#include <cmath>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using Test_Color = TestFixtureFslGraphics;

  constexpr uint32_t ExtractA(const uint32_t value)
  {
    return (value >> 24u) & 0xFF;
  }

  constexpr uint32_t ExtractR(const uint32_t value)
  {
    return (value >> 16u) & 0xFF;
  }

  constexpr uint32_t ExtractG(const uint32_t value)
  {
    return (value >> 8u) & 0xFF;
  }

  constexpr uint32_t ExtractB(const uint32_t value)
  {
    return value & 0xFF;
  }

  // For coverage purpose ensure that ToVector4 gets called
  Vector4 WorkAroundConstExpr(const Color& color)
  {
    return color.ToVector4();
  }

  Color Premultiply(const Color& color)
  {
    float alpha = float(color.A()) / 255.0f;
    uint32_t r = MathHelper::Clamp(static_cast<uint32_t>(std::round(alpha * float(color.R()))), 0u, 255u);
    uint32_t g = MathHelper::Clamp(static_cast<uint32_t>(std::round(alpha * float(color.G()))), 0u, 255u);
    uint32_t b = MathHelper::Clamp(static_cast<uint32_t>(std::round(alpha * float(color.B()))), 0u, 255u);
    return {r, g, b, uint32_t(color.A())};
  }

  Color Premultiply(const Color& color, const float alphaMul)
  {
    float alpha = (float(color.A()) / 255.0f) * alphaMul;
    uint32_t r = MathHelper::Clamp(static_cast<uint32_t>(std::round(alpha * float(color.R()))), 0u, 255u);
    uint32_t g = MathHelper::Clamp(static_cast<uint32_t>(std::round(alpha * float(color.G()))), 0u, 255u);
    uint32_t b = MathHelper::Clamp(static_cast<uint32_t>(std::round(alpha * float(color.B()))), 0u, 255u);
    uint32_t a = MathHelper::Clamp(static_cast<uint32_t>(std::round(alpha * float(color.A()))), 0u, 255u);
    return {r, g, b, a};
  }

  Color PremultiplyRGB(const Color& color, const float alphaMul, const uint32_t newAlpha)
  {
    float alpha = (float(color.A()) / 255.0f) * alphaMul;
    uint32_t r = MathHelper::Clamp(static_cast<uint32_t>(std::round(alpha * float(color.R()))), 0u, 255u);
    uint32_t g = MathHelper::Clamp(static_cast<uint32_t>(std::round(alpha * float(color.G()))), 0u, 255u);
    uint32_t b = MathHelper::Clamp(static_cast<uint32_t>(std::round(alpha * float(color.B()))), 0u, 255u);
    return {r, g, b, newAlpha};
  }
}


TEST(Test_Color, Construct_Empty)
{
  Color color;

  EXPECT_EQ(0u, color.R());
  EXPECT_EQ(0u, color.G());
  EXPECT_EQ(0u, color.B());
  EXPECT_EQ(0u, color.A());
  EXPECT_EQ(0u, color.PackedValue());
}


TEST(Test_Color, Construct_Float)
{
  Color colWhite(1.0f, 1.0f, 1.0f, 1.0f);
  Color colR(1.0f, 0.0f, 0.0f, 0.0f);
  Color colG(0.0f, 1.0f, 0.0f, 0.0f);
  Color colB(0.0f, 0.0f, 1.0f, 0.0f);
  Color colA(0.0f, 0.0f, 0.0f, 1.0f);
  Color colMisc(0.5f, 0.5f, 0.5f, 0.5f);

  EXPECT_EQ(0xFFu, colWhite.R());
  EXPECT_EQ(0xFFu, colWhite.G());
  EXPECT_EQ(0xFFu, colWhite.B());
  EXPECT_EQ(0xFFu, colWhite.A());
  EXPECT_EQ(0xFFFFFFFFu, colWhite.PackedValue());

  EXPECT_EQ(0xFFu, colR.R());
  EXPECT_EQ(0x00u, colR.G());
  EXPECT_EQ(0x00u, colR.B());
  EXPECT_EQ(0x00u, colR.A());
  EXPECT_EQ(0x00FF0000u, colR.PackedValue());

  EXPECT_EQ(0x00u, colG.R());
  EXPECT_EQ(0xFFu, colG.G());
  EXPECT_EQ(0x00u, colG.B());
  EXPECT_EQ(0x00u, colG.A());
  EXPECT_EQ(0x0000FF00u, colG.PackedValue());

  EXPECT_EQ(0x00u, colB.R());
  EXPECT_EQ(0x00u, colB.G());
  EXPECT_EQ(0xFFu, colB.B());
  EXPECT_EQ(0x00u, colB.A());
  EXPECT_EQ(0x000000FFu, colB.PackedValue());

  EXPECT_EQ(0x00u, colA.R());
  EXPECT_EQ(0x00u, colA.G());
  EXPECT_EQ(0x00u, colA.B());
  EXPECT_EQ(0xFFu, colA.A());
  EXPECT_EQ(0xFF000000u, colA.PackedValue());

  EXPECT_EQ(0x7fu, colMisc.R());
  EXPECT_EQ(0x7fu, colMisc.G());
  EXPECT_EQ(0x7fu, colMisc.B());
  EXPECT_EQ(0x7fu, colMisc.A());
  EXPECT_EQ(0x7f7f7f7fu, colMisc.PackedValue());
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

  EXPECT_EQ(colTransparent, Color::Transparent().PackedValue());
  EXPECT_EQ(colBlack, Color::Black().PackedValue());
  EXPECT_EQ(colRed, Color::Red().PackedValue());
  EXPECT_EQ(colGreen, Color::Green().PackedValue());
  EXPECT_EQ(colBlue, Color::Blue().PackedValue());
  EXPECT_EQ(colCyan, Color::Cyan().PackedValue());
  EXPECT_EQ(colYellow, Color::Yellow().PackedValue());
  EXPECT_EQ(colWhite, Color::White().PackedValue());
  EXPECT_EQ(colOrange, Color::Orange().PackedValue());
  EXPECT_EQ(colPink, Color::Pink().PackedValue());
  EXPECT_EQ(colPurple, Color::Purple().PackedValue());
  EXPECT_EQ(colMarrom, Color::Marrom().PackedValue());
  EXPECT_EQ(colBrown, Color::Brown().PackedValue());
  EXPECT_EQ(colOlive, Color::Olive().PackedValue());
  EXPECT_EQ(colSilver, Color::Silver().PackedValue());
  EXPECT_EQ(colDarkBlue, Color::DarkBlue().PackedValue());
  EXPECT_EQ(colLightBlue, Color::LightBlue().PackedValue());
  EXPECT_EQ(colLime, Color::Lime().PackedValue());

  // Check alpha channel
  EXPECT_EQ(ExtractA(colTransparent), Color::Transparent().A());
  EXPECT_EQ(ExtractA(colBlack), Color::Black().A());
  EXPECT_EQ(ExtractA(colRed), Color::Red().A());
  EXPECT_EQ(ExtractA(colGreen), Color::Green().A());
  EXPECT_EQ(ExtractA(colBlue), Color::Blue().A());
  EXPECT_EQ(ExtractA(colCyan), Color::Cyan().A());
  EXPECT_EQ(ExtractA(colYellow), Color::Yellow().A());
  EXPECT_EQ(ExtractA(colWhite), Color::White().A());
  EXPECT_EQ(ExtractA(colOrange), Color::Orange().A());
  EXPECT_EQ(ExtractA(colPink), Color::Pink().A());
  EXPECT_EQ(ExtractA(colPurple), Color::Purple().A());
  EXPECT_EQ(ExtractA(colMarrom), Color::Marrom().A());
  EXPECT_EQ(ExtractA(colBrown), Color::Brown().A());
  EXPECT_EQ(ExtractA(colOlive), Color::Olive().A());
  EXPECT_EQ(ExtractA(colSilver), Color::Silver().A());
  EXPECT_EQ(ExtractA(colDarkBlue), Color::DarkBlue().A());
  EXPECT_EQ(ExtractA(colLightBlue), Color::LightBlue().A());
  EXPECT_EQ(ExtractA(colLime), Color::Lime().A());

  // Red
  EXPECT_EQ(ExtractR(colTransparent), Color::Transparent().R());
  EXPECT_EQ(ExtractR(colBlack), Color::Black().R());
  EXPECT_EQ(ExtractR(colRed), Color::Red().R());
  EXPECT_EQ(ExtractR(colGreen), Color::Green().R());
  EXPECT_EQ(ExtractR(colBlue), Color::Blue().R());
  EXPECT_EQ(ExtractR(colCyan), Color::Cyan().R());
  EXPECT_EQ(ExtractR(colYellow), Color::Yellow().R());
  EXPECT_EQ(ExtractR(colWhite), Color::White().R());
  EXPECT_EQ(ExtractR(colOrange), Color::Orange().R());
  EXPECT_EQ(ExtractR(colPink), Color::Pink().R());
  EXPECT_EQ(ExtractR(colPurple), Color::Purple().R());
  EXPECT_EQ(ExtractR(colMarrom), Color::Marrom().R());
  EXPECT_EQ(ExtractR(colBrown), Color::Brown().R());
  EXPECT_EQ(ExtractR(colOlive), Color::Olive().R());
  EXPECT_EQ(ExtractR(colSilver), Color::Silver().R());
  EXPECT_EQ(ExtractR(colDarkBlue), Color::DarkBlue().R());
  EXPECT_EQ(ExtractR(colLightBlue), Color::LightBlue().R());
  EXPECT_EQ(ExtractR(colLime), Color::Lime().R());

  // Green
  EXPECT_EQ(ExtractG(colTransparent), Color::Transparent().G());
  EXPECT_EQ(ExtractG(colBlack), Color::Black().G());
  EXPECT_EQ(ExtractG(colRed), Color::Red().G());
  EXPECT_EQ(ExtractG(colGreen), Color::Green().G());
  EXPECT_EQ(ExtractG(colBlue), Color::Blue().G());
  EXPECT_EQ(ExtractG(colCyan), Color::Cyan().G());
  EXPECT_EQ(ExtractG(colYellow), Color::Yellow().G());
  EXPECT_EQ(ExtractG(colWhite), Color::White().G());
  EXPECT_EQ(ExtractG(colOrange), Color::Orange().G());
  EXPECT_EQ(ExtractG(colPink), Color::Pink().G());
  EXPECT_EQ(ExtractG(colPurple), Color::Purple().G());
  EXPECT_EQ(ExtractG(colMarrom), Color::Marrom().G());
  EXPECT_EQ(ExtractG(colBrown), Color::Brown().G());
  EXPECT_EQ(ExtractG(colOlive), Color::Olive().G());
  EXPECT_EQ(ExtractG(colSilver), Color::Silver().G());
  EXPECT_EQ(ExtractG(colDarkBlue), Color::DarkBlue().G());
  EXPECT_EQ(ExtractG(colLightBlue), Color::LightBlue().G());
  EXPECT_EQ(ExtractG(colLime), Color::Lime().G());

  // Blue
  EXPECT_EQ(ExtractB(colTransparent), Color::Transparent().B());
  EXPECT_EQ(ExtractB(colBlack), Color::Black().B());
  EXPECT_EQ(ExtractB(colRed), Color::Red().B());
  EXPECT_EQ(ExtractB(colGreen), Color::Green().B());
  EXPECT_EQ(ExtractB(colBlue), Color::Blue().B());
  EXPECT_EQ(ExtractB(colCyan), Color::Cyan().B());
  EXPECT_EQ(ExtractB(colYellow), Color::Yellow().B());
  EXPECT_EQ(ExtractB(colWhite), Color::White().B());
  EXPECT_EQ(ExtractB(colOrange), Color::Orange().B());
  EXPECT_EQ(ExtractB(colPink), Color::Pink().B());
  EXPECT_EQ(ExtractB(colPurple), Color::Purple().B());
  EXPECT_EQ(ExtractB(colMarrom), Color::Marrom().B());
  EXPECT_EQ(ExtractB(colBrown), Color::Brown().B());
  EXPECT_EQ(ExtractB(colOlive), Color::Olive().B());
  EXPECT_EQ(ExtractB(colSilver), Color::Silver().B());
  EXPECT_EQ(ExtractB(colDarkBlue), Color::DarkBlue().B());
  EXPECT_EQ(ExtractB(colLightBlue), Color::LightBlue().B());
  EXPECT_EQ(ExtractB(colLime), Color::Lime().B());
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

TEST(Test_Color, Premultiply_Constant)
{
  for (uint32_t i = 0; i < 256; ++i)
  {
    for (uint32_t a = 0; a < 256; ++a)
    {
      const float alpha = a / 255.0f;
      {
        Color color(0xFFu, 0x80u, 0x20u, i);
        auto expectedPremultiply = Premultiply(color, alpha);
        auto gotPremultiplied = Color::Premultiply(color, alpha);
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
      const float alpha = a / 255.0f;
      {
        Color color(0xFFu, 0x80u, 0x20u, i);
        auto expectedPremultiply = PremultiplyRGB(color, alpha, 32u);
        auto gotPremultiplied = Color::PremultiplyRGB(color, alpha, 32u);
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
      const float alpha = a / 255.0f;
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
  EXPECT_EQ(Color::DarkBlue(), Color::DarkBlue());
  EXPECT_EQ(Color::Red(), Color::Red());
}


TEST(Test_Color, NotEquals)
{
  EXPECT_NE(Color::DarkBlue(), Color::Red());
  EXPECT_NE(Color::Red(), Color::DarkBlue());
}
