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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Log/Math/LogVector2.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslBase/UnitTest/Helper/Test/Math/TestVector2.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Vector2 = TestFixtureFslBase;

  // This code relies on MathHelper being correct
  void CheckBarycentric(const Vector2& value1, const Vector2& value2, const Vector2& value3, const float amount1, const float amount2)
  {
    {
      const auto res = Vector2::Barycentric(value1, value2, value3, amount1, amount2);
      EXPECT_EQ(MathHelper::Barycentric(value1.X, value2.X, value3.X, amount1, amount2), res.X);
      EXPECT_EQ(MathHelper::Barycentric(value1.Y, value2.Y, value3.Y, amount1, amount2), res.Y);
    }
    {
      Vector2 res;
      Vector2::Barycentric(value1, value2, value3, amount1, amount2, res);
      EXPECT_EQ(MathHelper::Barycentric(value1.X, value2.X, value3.X, amount1, amount2), res.X);
      EXPECT_EQ(MathHelper::Barycentric(value1.Y, value2.Y, value3.Y, amount1, amount2), res.Y);
    }
  }

  // This code relies on MathHelper being correct
  void CheckCatmullRom(const Vector2& value1, const Vector2& value2, const Vector2& value3, const Vector2& value4, const float amount)
  {
    {
      const auto res = Vector2::CatmullRom(value1, value2, value3, value4, amount);
      EXPECT_EQ(MathHelper::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount), res.X);
      EXPECT_EQ(MathHelper::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount), res.Y);
    }
    {
      Vector2 res;
      Vector2::CatmullRom(value1, value2, value3, value4, amount, res);
      EXPECT_EQ(MathHelper::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount), res.X);
      EXPECT_EQ(MathHelper::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount), res.Y);
    }
  }

  // This code relies on MathHelper being correct
  void CheckClamp(const Vector2& value, const Vector2& minValue, const Vector2& maxValue)
  {
    {
      const auto res = Vector2::Clamp(value, minValue, maxValue);
      EXPECT_EQ(MathHelper::Clamp(value.X, minValue.X, maxValue.X), res.X);
      EXPECT_EQ(MathHelper::Clamp(value.Y, minValue.Y, maxValue.Y), res.Y);
    }
    {
      Vector2 res;
      Vector2::Clamp(value, minValue, maxValue, res);
      EXPECT_EQ(MathHelper::Clamp(value.X, minValue.X, maxValue.X), res.X);
      EXPECT_EQ(MathHelper::Clamp(value.Y, minValue.Y, maxValue.Y), res.Y);
    }
  }

  // This code relies on MathHelper being correct
  void CheckHermite(const Vector2& value1, const Vector2& value2, const Vector2& value3, const Vector2& value4, const float amount)
  {
    {
      const auto res = Vector2::Hermite(value1, value2, value3, value4, amount);
      EXPECT_EQ(MathHelper::Hermite(value1.X, value2.X, value3.X, value4.X, amount), res.X);
      EXPECT_EQ(MathHelper::Hermite(value1.Y, value2.Y, value3.Y, value4.Y, amount), res.Y);
    }
    {
      Vector2 res;
      Vector2::Hermite(value1, value2, value3, value4, amount, res);
      EXPECT_EQ(MathHelper::Hermite(value1.X, value2.X, value3.X, value4.X, amount), res.X);
      EXPECT_EQ(MathHelper::Hermite(value1.Y, value2.Y, value3.Y, value4.Y, amount), res.Y);
    }
  }


  void CheckLerp(const Vector2& value1, const Vector2& value2, const float t, const Vector2& res)
  {
    EXPECT_EQ(MathHelper::Lerp(value1.X, value2.X, t), res.X);
    EXPECT_EQ(MathHelper::Lerp(value1.Y, value2.Y, t), res.Y);
  }


  // This code relies on MathHelper being correct
  void CheckSmoothStep(const Vector2& value1, const Vector2& value2, const float amount)
  {
    {
      const auto res = Vector2::SmoothStep(value1, value2, amount);
      EXPECT_EQ(MathHelper::SmoothStep(value1.X, value2.X, amount), res.X);
      EXPECT_EQ(MathHelper::SmoothStep(value1.Y, value2.Y, amount), res.Y);
    }
    {
      Vector2 res;
      Vector2::SmoothStep(value1, value2, amount, res);
      EXPECT_EQ(MathHelper::SmoothStep(value1.X, value2.X, amount), res.X);
      EXPECT_EQ(MathHelper::SmoothStep(value1.Y, value2.Y, amount), res.Y);
    }
  }
}


TEST(TestMath_Vector2, Construct)
{
  Vector2 value(1.0f, 2.0f);

  EXPECT_EQ(1.0f, value.X);
  EXPECT_EQ(2.0f, value.Y);
}


TEST(TestMath_Vector2, Barycentric)
{
  const Vector2 value1(1.0f, 2.0f);
  const Vector2 value2(11.0f, 12.0f);
  const Vector2 value3(-11.0f, -12.0f);

  CheckBarycentric(value1, value2, value3, 0.0f, 0.0f);
  CheckBarycentric(value1, value2, value3, 1.0f, 0.0f);
  CheckBarycentric(value1, value2, value3, 0.0f, 1.0f);
  CheckBarycentric(value1, value2, value3, 0.5f, 0.5f);
  CheckBarycentric(value1, value2, value3, 1.0f, 1.0f);
}


TEST(TestMath_Vector2, CatmullRom)
{
  const Vector2 value1(1.0f, 2.0f);
  const Vector2 value2(11.0f, 12.0f);
  const Vector2 value3(-11.0f, -12.0f);
  const Vector2 value4(-21.0f, -22.0f);

  CheckCatmullRom(value1, value2, value3, value4, 0.00f);
  CheckCatmullRom(value1, value2, value3, value4, 1.00f);
  CheckCatmullRom(value1, value2, value3, value4, 0.25f);
  CheckCatmullRom(value1, value2, value3, value4, 0.50f);
  CheckCatmullRom(value1, value2, value3, value4, 0.75f);
  CheckCatmullRom(value1, value2, value3, value4, 1.00f);
}


TEST(TestMath_Vector2, CheckClamp)
{
  const Vector2 minValue(1.0f, 2.0f);
  const Vector2 maxValue(5.0f, 6.0f);
  const Vector2 value(-1, 7.0f);

  CheckClamp(value, minValue, maxValue);
}


TEST(TestMath_Vector2, Distance)
{
  const Vector2 vec1(1.0f, 2.0f);
  const Vector2 vec2(4.0f, 6.0f);

  EXPECT_FLOAT_EQ(5.0f, Vector2::Distance(vec1, vec2));
}


TEST(TestMath_Vector2, DistanceSquared)
{
  const Vector2 vec1(1.0f, 2.0f);
  const Vector2 vec2(4.0f, 6.0f);

  EXPECT_FLOAT_EQ(25.0f, Vector2::DistanceSquared(vec1, vec2));
}


TEST(TestMath_Vector2, Hermite)
{
  const Vector2 value1(1.0f, 2.0f);
  const Vector2 value2(11.0f, 12.0f);
  const Vector2 value3(-11.0f, -12.0f);
  const Vector2 value4(-21.0f, -22.0f);

  CheckHermite(value1, value2, value3, value4, 0.00f);
  CheckHermite(value1, value2, value3, value4, 1.00f);
  CheckHermite(value1, value2, value3, value4, 0.25f);
  CheckHermite(value1, value2, value3, value4, 0.50f);
  CheckHermite(value1, value2, value3, value4, 0.75f);
  CheckHermite(value1, value2, value3, value4, 1.00f);
}


TEST(TestMath_Vector2, Length)
{
  const Vector2 value(3.0f, 4.0f);

  EXPECT_FLOAT_EQ(5.0f, value.Length());
}


TEST(TestMath_Vector2, LengthSquared)
{
  const Vector2 value(3.0f, 4.0f);

  EXPECT_FLOAT_EQ(25.0f, value.LengthSquared());
}


TEST(TestMath_Vector2, Lerp)
{
  const Vector2 value1(1.0f, 2.0f);
  const Vector2 value2(101.0f, 102.0f);

  EXPECT_EQ(value1, Vector2::Lerp(value1, value2, 0.0f));
  EXPECT_EQ(value2, Vector2::Lerp(value1, value2, 1.0f));

  CheckLerp(value1, value2, 0.25f, Vector2::Lerp(value1, value2, 0.25f));
  CheckLerp(value1, value2, 0.50f, Vector2::Lerp(value1, value2, 0.50f));
  CheckLerp(value1, value2, 0.75f, Vector2::Lerp(value1, value2, 0.75f));
}


TEST(TestMath_Vector2, Lerp_Ref)
{
  const Vector2 value1(1.0f, 2.0f);
  const Vector2 value2(101.0f, 102.0f);

  Vector2 res1;
  Vector2::Lerp(value1, value2, 0.0f, res1);
  Vector2 res2;
  Vector2::Lerp(value1, value2, 1.0f, res2);
  EXPECT_EQ(value1, res1);
  EXPECT_EQ(value2, res2);

  Vector2 res3;
  Vector2::Lerp(value1, value2, 0.25f, res3);
  CheckLerp(value1, value2, 0.25f, res3);
  Vector2::Lerp(value1, value2, 0.50f, res3);
  CheckLerp(value1, value2, 0.50f, res3);
  Vector2::Lerp(value1, value2, 0.75f, res3);
  CheckLerp(value1, value2, 0.75f, res3);
}

TEST(TestMath_Vector2, Max)
{
  const Vector2 val1(1.0f, 6.0f);
  const Vector2 val2(5.0f, 2.0f);
  const Vector2 expValue(5.0f, 6.0f);

  EXPECT_EQ(expValue, Vector2::Max(val1, val2));

  Vector2 res;
  Vector2::Max(val1, val2, res);
  EXPECT_EQ(expValue, res);
}


TEST(TestMath_Vector2, Min)
{
  const Vector2 val1(1.0f, 6.0f);
  const Vector2 val2(5.0f, 2.0f);
  const Vector2 expValue(1.0f, 2.0f);

  EXPECT_EQ(expValue, Vector2::Min(val1, val2));

  Vector2 res;
  Vector2::Min(val1, val2, res);
  EXPECT_EQ(expValue, res);
}


TEST(TestMath_Vector2, Negate)
{
  const Vector2 value(1.0f, 2.0f);
  const Vector2 expectedValue(-1.0f, -2.0f);

  EXPECT_EQ(expectedValue, Vector2::Negate(value));

  const auto res = -value;

  EXPECT_EQ(expectedValue, res);

  Vector2 res2;
  Vector2::Negate(value, res2);
  EXPECT_EQ(expectedValue, res2);
}


TEST(TestMath_Vector2, Normalize)
{
  Vector2 value1(4.0f, 0.0f);
  Vector2 value2(0.0f, 4.0f);
  Vector2 value3(3.0f, 4.0f);
  Vector2 value4(4.0f, 3.0f);
  value1.Normalize();
  value2.Normalize();
  value3.Normalize();
  value4.Normalize();

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(1.0f, 0.0f), value1));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(0.0f, 1.0f), value2));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(0.6f, 0.8f), value3));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(0.8f, 0.6f), value4));
}


TEST(TestMath_Vector2, Normalize_Static)
{
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(1.0f, 0.0f), Vector2::Normalize(Vector2(4.0f, 0.0f))));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(0.0f, 1.0f), Vector2::Normalize(Vector2(0.0f, 4.0f))));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(0.6f, 0.8f), Vector2::Normalize(Vector2(3.0f, 4.0f))));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(0.8f, 0.6f), Vector2::Normalize(Vector2(4.0f, 3.0f))));
}

TEST(TestMath_Vector2, Normalize_Static_Ref)
{
  Vector2 res1, res2, res3, res4;
  Vector2::Normalize(Vector2(4.0f, 0.0f), res1);
  Vector2::Normalize(Vector2(0.0f, 4.0f), res2);
  Vector2::Normalize(Vector2(3.0f, 4.0f), res3);
  Vector2::Normalize(Vector2(4.0f, 3.0f), res4);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(1.0f, 0.0f), res1));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(0.0f, 1.0f), res2));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(0.6f, 0.8f), res3));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(0.8f, 0.6f), res4));
}

TEST(TestMath_Vector2, Reflect)
{
  const Vector2 value1(1.0f, 2.0f);
  EXPECT_EQ(Vector2(-1.0f, 2.0f), Vector2::Reflect(value1, Vector2(1.0f, 0.0f)));
  EXPECT_EQ(Vector2(1.0f, -2.0f), Vector2::Reflect(value1, Vector2(0.0f, 1.0f)));
}

TEST(TestMath_Vector2, Reflect_Ref)
{
  const Vector2 value1(1.0f, 2.0f);

  Vector2 res1;
  Vector2 res2;
  Vector2::Reflect(value1, Vector2(1.0f, 0.0f), res1);
  Vector2::Reflect(value1, Vector2(0.0f, 1.0f), res2);
  EXPECT_EQ(Vector2(-1.0f, 2.0f), res1);
  EXPECT_EQ(Vector2(1.0f, -2.0f), res2);
}

TEST(TestMath_Vector2, SmoothStep)
{
  const Vector2 value1(1.0f, 2.0f);
  const Vector2 value2(11.0f, 12.0f);

  CheckSmoothStep(value1, value2, 0.00f);
  CheckSmoothStep(value1, value2, 1.00f);
  CheckSmoothStep(value1, value2, 0.25f);
  CheckSmoothStep(value1, value2, 0.50f);
  CheckSmoothStep(value1, value2, 0.75f);
  CheckSmoothStep(value1, value2, 1.00f);
}

TEST(TestMath_Vector2, Add)
{
  const Vector2 value1(1.0f, 2.0f);
  const Vector2 value2(11.0f, 12.0f);

  Vector2 res1;
  Vector2::Add(value1, value2, res1);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(12.0f, 14.0f), res1));
}

TEST(TestMath_Vector2, Add_Operator)
{
  const Vector2 value1(1.0f, 2.0f);
  const Vector2 value2(11.0f, 12.0f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(12.0f, 14.0f), value1 + value2));
}

TEST(TestMath_Vector2, Subtract)
{
  const Vector2 value1(11.0f, 13.0f);
  const Vector2 value2(1.0f, 2.0f);

  Vector2 res1;
  Vector2::Subtract(value1, value2, res1);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(10.0f, 11.0f), res1));
}


TEST(TestMath_Vector2, Subtract_Operator)
{
  const Vector2 value1(11.0f, 13.0f);
  const Vector2 value2(1.0f, 2.0f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(10.0f, 11.0f), value1 - value2));
}

TEST(TestMath_Vector2, Multiply)
{
  const Vector2 value1(2.0f, 3.0f);
  const Vector2 value2(4.0f, 5.0f);

  Vector2 res1;
  Vector2::Multiply(value1, value2, res1);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(8.0f, 15.0f), res1));
}


TEST(TestMath_Vector2, Multiply_Operator)
{
  const Vector2 value1(2.0f, 3.0f);
  const Vector2 value2(4.0f, 5.0f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(8.0f, 15.0f), value1 * value2));
}


TEST(TestMath_Vector2, Divide)
{
  const Vector2 value1(6.0f, 20.0f);
  const Vector2 value2(2.0f, 4.0f);

  Vector2 res1;
  Vector2::Divide(value1, value2, res1);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(3.0f, 5.0f), res1));
}


TEST(TestMath_Vector2, Divide_Operator)
{
  const Vector2 value1(6.0f, 20.0f);
  const Vector2 value2(2.0f, 4.0f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector2(3.0f, 5.0f), value1 / value2));
}


TEST(TestMath_Vector2, Equal)
{
  const Vector2 value1(1.0f, 2.0f);
  const Vector2 value2(1.0f, 2.0f);

  EXPECT_EQ(value1, value2);
  EXPECT_EQ(value2, value1);
  EXPECT_EQ(value1, value1);
  EXPECT_EQ(value2, value2);
}


TEST(TestMath_Vector2, NotEqual)
{
  const Vector2 value1(1.0f, 2.0f);
  const Vector2 value2(-1.0f, 2.0f);
  const Vector2 value3(1.0f, -2.0f);

  EXPECT_NE(value1, value2);
  EXPECT_NE(value1, value3);
}
