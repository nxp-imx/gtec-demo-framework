/*
MIT License
Copyright (C) 2006 The Mono.Xna Team

All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// The functions in this file are a port of an MIT licensed library: MonoGame - MathHelperTest.cs.

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogVector2.hpp>
#include <FslBase/Log/Math/LogVector3.hpp>
#include <FslBase/Log/Math/LogVector4.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMath_MathHelper_MonoGame = TestFixtureFslBase;
}


TEST(TestMath_MathHelper_MonoGame, ClampFloatTest)
{
  EXPECT_EQ(1.0f, MathHelper::Clamp(1.0f, 0.0f, 2.0f)) << "Failed boundary test, clamp [0,2] on 1 should be 1";
  EXPECT_EQ(1.0f, MathHelper::Clamp(1.0f, 0.0f, 1.0f)) << "Failed boundary test, clamp [0,1] on 1 should be 1";
  EXPECT_EQ(2.0f, MathHelper::Clamp(1.0f, 2.0f, 5.0f)) << "Failed boundary test, clamp [2,5] on 1 should be 2";
  EXPECT_EQ(-10.0f, MathHelper::Clamp(1.0f, -50.0f, -10.0f)) << "Failed boundary test, clamp [-50f, -10f] on 1 should be -10";
  EXPECT_EQ(1.0f, MathHelper::Clamp(1.0f, -50.0f, 25.0f)) << "Failed boundary test, clamp [-50, 25] on 1 should be 1";
  EXPECT_EQ(1.0f, MathHelper::Clamp(0.0f, 1.0f, 1.0f)) << "Failed boundary test, clamp [1,1] on 0 should be 1";
  EXPECT_EQ(-1.0f, MathHelper::Clamp(0.0f, -1.0f, -1.0f)) << "Failed boundary test, clamp [-1,-1] on 0 should be -1";
}


TEST(TestMath_MathHelper_MonoGame, ClampIntTest)
{
  EXPECT_EQ(1, MathHelper::Clamp(1, 0, 2)) << "Failed boundary test, clamp [0,2] on 1 should be 1";
  EXPECT_EQ(1, MathHelper::Clamp(1, 0, 1)) << "Failed boundary test, clamp [0,1] on 1 should be 1";
  EXPECT_EQ(2, MathHelper::Clamp(1, 2, 5)) << "Failed boundary test, clamp [2,5] on 1 should be 2";
  EXPECT_EQ(-10, MathHelper::Clamp(1, -50, -10)) << "Failed boundary test, clamp [-50f, -10f] on 1 should be -10";
  EXPECT_EQ(1, MathHelper::Clamp(1, -50, 25)) << "Failed boundary test, clamp [-50, 25] on 1 should be 1";
  EXPECT_EQ(1, MathHelper::Clamp(0, 1, 1)) << "Failed boundary test, clamp [1,1] on 0 should be 1";
  EXPECT_EQ(-1, MathHelper::Clamp(0, -1, -1)) << "Failed boundary test, clamp [-1,-1] on 0 should be -1";
}


TEST(TestMath_MathHelper_MonoGame, DistanceTest)
{
  EXPECT_EQ(5.0f, MathHelper::Distance(0.0f, 5.0f)) << "Distance test failed on [0,5]";
  EXPECT_EQ(10.0f, MathHelper::Distance(-5.0f, 5.0f)) << "Distance test failed on [-5,5]";
  EXPECT_EQ(0.0f, MathHelper::Distance(0.0f, 0.0f)) << "Distance test failed on [0,0]";
  EXPECT_EQ(4.0f, MathHelper::Distance(-5.0f, -1.0f)) << "Distance test failed on [-5,-1]";
}


TEST(TestMath_MathHelper_MonoGame, LerpTest)
{
  EXPECT_EQ(2.5f, MathHelper::Lerp(0.0f, 5.0f, .5f)) << "Lerp test failed on [0,5,.5]";
  EXPECT_EQ(0.0f, MathHelper::Lerp(-5.0f, 5.0f, 0.5f)) << "Lerp test failed on [-5,5,0.5]";
  EXPECT_EQ(0.0f, MathHelper::Lerp(0.0f, 0.0f, 0.5f)) << "Lerp test failed on [0,0,0.5]";
  EXPECT_EQ(-3.0f, MathHelper::Lerp(-5.0f, -1.0f, 0.5f)) << "Lerp test failed on [-5,-1, 0.5]";
}


TEST(TestMath_MathHelper_MonoGame, WrapAngleReturnsExpectedValues)
{
  std::vector<Vector2> testValues = {
    Vector2(0.0f, 0.0f), Vector2(MathHelper::PiOver4, MathHelper::PiOver4), Vector2(-MathHelper::PiOver4, -MathHelper::PiOver4),
    Vector2(MathHelper::PiOver2, MathHelper::PiOver2), Vector2(-MathHelper::PiOver2, -MathHelper::PiOver2), Vector2(MathHelper::PI, MathHelper::PI),
    Vector2(-MathHelper::PI, MathHelper::PI), Vector2(MathHelper::TwoPi, 0.0f), Vector2(-MathHelper::TwoPi, 0.0f), Vector2(10.0f, -2.566371f),
    Vector2(-10.0f, 2.566371f),
    // Pi boundaries
    Vector2(3.1415927f, 3.1415927f), Vector2(3.141593f, -3.1415925f), Vector2(-3.1415925f, -3.1415925f), Vector2(-3.1415927f, 3.1415927f),
    // 2 * Pi boundaries
    Vector2(6.283185f, -4.7683716E-7f), Vector2(6.2831855f, 0.0f), Vector2(6.283186f, 4.7683716E-7f), Vector2(-6.283185f, 4.7683716E-7f),
    Vector2(-6.2831855f, 0.0f), Vector2(-6.283186f, -4.7683716E-7f),
    // 3 * Pi boundaries
    Vector2(9.424778f, 3.1415925f), Vector2(9.424779f, -3.141592f), Vector2(-9.424778f, -3.1415925f), Vector2(-9.424779f, 3.141592f),
    // 4 * Pi boundaries
    Vector2(12.56637f, -9.536743E-7f), Vector2(12.566371f, 0.0f), Vector2(12.566372f, 9.536743E-7f), Vector2(-12.56637f, 9.536743E-7f),
    Vector2(-12.566371f, 0.0f), Vector2(-12.566372f, -9.536743E-7f),
    // 5 * Pi boundaries
    Vector2(15.707963f, 3.141592f), Vector2(15.707964f, -3.1415925f), Vector2(-15.707963f, -3.141592f), Vector2(-15.707964f, 3.1415925f),
    // 10 * Pi boundaries
    Vector2(31.415926f, -1.4305115E-6f), Vector2(31.415928f, 4.7683716E-7f), Vector2(-31.415926f, 1.4305115E-6f),
    Vector2(-31.415928f, -4.7683716E-7f),
    // 20 * Pi boundaries
    Vector2(62.831852f, -2.861023E-6f), Vector2(62.831856f, 9.536743E-7f), Vector2(-62.831852f, 2.861023E-6f), Vector2(-62.831856f, -9.536743E-7f),
    // 20000000 * Pi boundaries
    Vector2(6.2831852E7f, -2.8202515f), Vector2(6.2831856E7f, 1.1797485f), Vector2(-6.2831852E7f, 2.8202515f), Vector2(-6.2831856E7f, -1.1797485f)};

  for (const auto& entry : testValues)
  {
    const auto angle = entry.X;
    const auto expectedValue = entry.Y;
    auto actualValue = MathHelper::WrapAngle(angle);
    EXPECT_FLOAT_EQ(expectedValue, actualValue);
  }
}
