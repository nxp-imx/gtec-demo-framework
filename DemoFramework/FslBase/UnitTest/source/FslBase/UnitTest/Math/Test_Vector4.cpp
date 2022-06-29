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
#include <FslBase/Log/Math/LogVector4.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/Test/Math/TestVector4.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Vector4 = TestFixtureFslBase;

  // This code relies on MathHelper being correct
  void CheckBarycentric(const Vector4& value1, const Vector4& value2, const Vector4& value3, const float amount1, const float amount2)
  {
    {
      const auto res = Vector4::Barycentric(value1, value2, value3, amount1, amount2);
      EXPECT_EQ(MathHelper::Barycentric(value1.X, value2.X, value3.X, amount1, amount2), res.X);
      EXPECT_EQ(MathHelper::Barycentric(value1.Y, value2.Y, value3.Y, amount1, amount2), res.Y);
      EXPECT_EQ(MathHelper::Barycentric(value1.Z, value2.Z, value3.Z, amount1, amount2), res.Z);
      EXPECT_EQ(MathHelper::Barycentric(value1.W, value2.W, value3.W, amount1, amount2), res.W);
    }
    {
      Vector4 res;
      Vector4::Barycentric(value1, value2, value3, amount1, amount2, res);
      EXPECT_EQ(MathHelper::Barycentric(value1.X, value2.X, value3.X, amount1, amount2), res.X);
      EXPECT_EQ(MathHelper::Barycentric(value1.Y, value2.Y, value3.Y, amount1, amount2), res.Y);
      EXPECT_EQ(MathHelper::Barycentric(value1.Z, value2.Z, value3.Z, amount1, amount2), res.Z);
      EXPECT_EQ(MathHelper::Barycentric(value1.W, value2.W, value3.W, amount1, amount2), res.W);
    }
  }

  // This code relies on MathHelper being correct
  void CheckCatmullRom(const Vector4& value1, const Vector4& value2, const Vector4& value3, const Vector4& value4, const float amount)
  {
    {
      const auto res = Vector4::CatmullRom(value1, value2, value3, value4, amount);
      EXPECT_EQ(MathHelper::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount), res.X);
      EXPECT_EQ(MathHelper::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount), res.Y);
      EXPECT_EQ(MathHelper::CatmullRom(value1.Z, value2.Z, value3.Z, value4.Z, amount), res.Z);
      EXPECT_EQ(MathHelper::CatmullRom(value1.W, value2.W, value3.W, value4.W, amount), res.W);
    }
    {
      Vector4 res;
      Vector4::CatmullRom(value1, value2, value3, value4, amount, res);
      EXPECT_EQ(MathHelper::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount), res.X);
      EXPECT_EQ(MathHelper::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount), res.Y);
      EXPECT_EQ(MathHelper::CatmullRom(value1.Z, value2.Z, value3.Z, value4.Z, amount), res.Z);
      EXPECT_EQ(MathHelper::CatmullRom(value1.W, value2.W, value3.W, value4.W, amount), res.W);
    }
  }


  // This code relies on MathHelper being correct
  void CheckClamp(const Vector4& value, const Vector4& minValue, const Vector4& maxValue)
  {
    {
      const auto res = Vector4::Clamp(value, minValue, maxValue);
      EXPECT_EQ(MathHelper::Clamp(value.X, minValue.X, maxValue.X), res.X);
      EXPECT_EQ(MathHelper::Clamp(value.Y, minValue.Y, maxValue.Y), res.Y);
      EXPECT_EQ(MathHelper::Clamp(value.Z, minValue.Z, maxValue.Z), res.Z);
      EXPECT_EQ(MathHelper::Clamp(value.W, minValue.W, maxValue.W), res.W);
    }
    {
      Vector4 res;
      Vector4::Clamp(value, minValue, maxValue, res);
      EXPECT_EQ(MathHelper::Clamp(value.X, minValue.X, maxValue.X), res.X);
      EXPECT_EQ(MathHelper::Clamp(value.Y, minValue.Y, maxValue.Y), res.Y);
      EXPECT_EQ(MathHelper::Clamp(value.Z, minValue.Z, maxValue.Z), res.Z);
      EXPECT_EQ(MathHelper::Clamp(value.W, minValue.W, maxValue.W), res.W);
    }
  }

  // This code relies on MathHelper being correct
  void CheckHermite(const Vector4& value1, const Vector4& value2, const Vector4& value3, const Vector4& value4, const float amount)
  {
    {
      const auto res = Vector4::Hermite(value1, value2, value3, value4, amount);
      EXPECT_EQ(MathHelper::Hermite(value1.X, value2.X, value3.X, value4.X, amount), res.X);
      EXPECT_EQ(MathHelper::Hermite(value1.Y, value2.Y, value3.Y, value4.Y, amount), res.Y);
      EXPECT_EQ(MathHelper::Hermite(value1.Z, value2.Z, value3.Z, value4.Z, amount), res.Z);
      EXPECT_EQ(MathHelper::Hermite(value1.W, value2.W, value3.W, value4.W, amount), res.W);
    }
    {
      Vector4 res;
      Vector4::Hermite(value1, value2, value3, value4, amount, res);
      EXPECT_EQ(MathHelper::Hermite(value1.X, value2.X, value3.X, value4.X, amount), res.X);
      EXPECT_EQ(MathHelper::Hermite(value1.Y, value2.Y, value3.Y, value4.Y, amount), res.Y);
      EXPECT_EQ(MathHelper::Hermite(value1.Z, value2.Z, value3.Z, value4.Z, amount), res.Z);
      EXPECT_EQ(MathHelper::Hermite(value1.W, value2.W, value3.W, value4.W, amount), res.W);
    }
  }

  // This code relies on MathHelper being correct
  void CheckLerp(const Vector4& value1, const Vector4& value2, const float t, const Vector4& res)
  {
    EXPECT_EQ(MathHelper::Lerp(value1.X, value2.X, t), res.X);
    EXPECT_EQ(MathHelper::Lerp(value1.Y, value2.Y, t), res.Y);
    EXPECT_EQ(MathHelper::Lerp(value1.Z, value2.Z, t), res.Z);
    EXPECT_EQ(MathHelper::Lerp(value1.W, value2.W, t), res.W);
  }


  // This code relies on MathHelper being correct
  void CheckSmoothStep(const Vector4& value1, const Vector4& value2, const float amount)
  {
    {
      const auto res = Vector4::SmoothStep(value1, value2, amount);
      EXPECT_EQ(MathHelper::SmoothStep(value1.X, value2.X, amount), res.X);
      EXPECT_EQ(MathHelper::SmoothStep(value1.Y, value2.Y, amount), res.Y);
      EXPECT_EQ(MathHelper::SmoothStep(value1.Z, value2.Z, amount), res.Z);
      EXPECT_EQ(MathHelper::SmoothStep(value1.W, value2.W, amount), res.W);
    }
    {
      Vector4 res;
      Vector4::SmoothStep(value1, value2, amount, res);
      EXPECT_EQ(MathHelper::SmoothStep(value1.X, value2.X, amount), res.X);
      EXPECT_EQ(MathHelper::SmoothStep(value1.Y, value2.Y, amount), res.Y);
      EXPECT_EQ(MathHelper::SmoothStep(value1.Z, value2.Z, amount), res.Z);
      EXPECT_EQ(MathHelper::SmoothStep(value1.W, value2.W, amount), res.W);
    }
  }
}


TEST(TestMath_Vector4, Construct)
{
  const Vector4 value(1.0f, 2.0f, 3.0f, 4.0f);

  EXPECT_EQ(1.0f, value.X);
  EXPECT_EQ(2.0f, value.Y);
  EXPECT_EQ(3.0f, value.Z);
  EXPECT_EQ(4.0f, value.W);
}


TEST(TestMath_Vector4, Barycentric)
{
  const Vector4 value1(1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 value2(11.0f, 12.0f, 13.0f, 14.0f);
  const Vector4 value3(-11.0f, -12.0f, -13.0f, -14.0f);

  CheckBarycentric(value1, value2, value3, 0.0f, 0.0f);
  CheckBarycentric(value1, value2, value3, 1.0f, 0.0f);
  CheckBarycentric(value1, value2, value3, 0.0f, 1.0f);
  CheckBarycentric(value1, value2, value3, 0.5f, 0.5f);
  CheckBarycentric(value1, value2, value3, 1.0f, 1.0f);
}


TEST(TestMath_Vector4, CatmullRom)
{
  const Vector4 value1(1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 value2(11.0f, 12.0f, 13.0f, 14.0f);
  const Vector4 value3(-11.0f, -12.0f, -13.0f, -14.0f);
  const Vector4 value4(-21.0f, -22.0f, -23.0f, -24.0f);

  CheckCatmullRom(value1, value2, value3, value4, 0.00f);
  CheckCatmullRom(value1, value2, value3, value4, 1.00f);
  CheckCatmullRom(value1, value2, value3, value4, 0.25f);
  CheckCatmullRom(value1, value2, value3, value4, 0.50f);
  CheckCatmullRom(value1, value2, value3, value4, 0.75f);
  CheckCatmullRom(value1, value2, value3, value4, 1.00f);
}


TEST(TestMath_Vector4, CheckClamp)
{
  const Vector4 minValue(1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 maxValue(5.0f, 6.0f, 7.0f, 8.0f);
  const Vector4 value(-1, 7.0f, 5.0f, 7.5f);

  CheckClamp(value, minValue, maxValue);
}


TEST(TestMath_Vector4, Distance)
{
  const Vector4 vec1(1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 vec2(5.0f, 6.0f, 7.0f, 8.0f);

  EXPECT_FLOAT_EQ(8.0f, Vector4::Distance(vec1, vec2));
}


TEST(TestMath_Vector4, DistanceSquared)
{
  const Vector4 vec1(1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 vec2(5.0f, 6.0f, 7.0f, 8.0f);

  EXPECT_FLOAT_EQ(64.0f, Vector4::DistanceSquared(vec1, vec2));
}


TEST(TestMath_Vector4, Hermite)
{
  const Vector4 value1(1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 value2(11.0f, 12.0f, 13.0f, 14.0f);
  const Vector4 value3(-11.0f, -12.0f, -13.0f, -14.0f);
  const Vector4 value4(-21.0f, -22.0f, -23.0f, -24.0f);

  CheckHermite(value1, value2, value3, value4, 0.00f);
  CheckHermite(value1, value2, value3, value4, 1.00f);
  CheckHermite(value1, value2, value3, value4, 0.25f);
  CheckHermite(value1, value2, value3, value4, 0.50f);
  CheckHermite(value1, value2, value3, value4, 0.75f);
  CheckHermite(value1, value2, value3, value4, 1.00f);
}


TEST(TestMath_Vector4, Max)
{
  const Vector4 val1(1.0f, 6.0f, 3.0f, 8.0f);
  const Vector4 val2(5.0f, 2.0f, 7.0f, 4.0f);
  const Vector4 expValue(5.0f, 6.0f, 7.0f, 8.0f);

  EXPECT_EQ(expValue, Vector4::Max(val1, val2));

  Vector4 res;
  Vector4::Max(val1, val2, res);
  EXPECT_EQ(expValue, res);
}


TEST(TestMath_Vector4, Min)
{
  const Vector4 val1(1.0f, 6.0f, 3.0f, 8.0f);
  const Vector4 val2(5.0f, 2.0f, 7.0f, 4.0f);
  const Vector4 expValue(1.0f, 2.0f, 3.0f, 4.0f);

  EXPECT_EQ(expValue, Vector4::Min(val1, val2));

  Vector4 res;
  Vector4::Min(val1, val2, res);
  EXPECT_EQ(expValue, res);
}


TEST(TestMath_Vector4, Negate)
{
  const Vector4 value(1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 expectedValue(-1.0f, -2.0f, -3.0f, -4.0f);

  EXPECT_EQ(expectedValue, Vector4::Negate(value));

  const auto res = -value;

  EXPECT_EQ(expectedValue, res);

  Vector4 res2;
  Vector4::Negate(value, res2);
  EXPECT_EQ(expectedValue, res2);
}


TEST(TestMath_Vector4, Length)
{
  const Vector4 value(1.0f, 2.0f, 4.0f, 10.0f);

  EXPECT_FLOAT_EQ(11.0f, value.Length());
}


TEST(TestMath_Vector4, LengthSquared)
{
  const Vector4 value(1.0f, 2.0f, 4.0f, 10.0f);

  EXPECT_FLOAT_EQ(121.0f, value.LengthSquared());
}


TEST(TestMath_Vector4, Lerp)
{
  const Vector4 value1(1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 value2(101.0f, 102.0f, 103.0f, 104.0f);

  EXPECT_EQ(value1, Vector4::Lerp(value1, value2, 0.0f));
  EXPECT_EQ(value2, Vector4::Lerp(value1, value2, 1.0f));

  CheckLerp(value1, value2, 0.25f, Vector4::Lerp(value1, value2, 0.25f));
  CheckLerp(value1, value2, 0.50f, Vector4::Lerp(value1, value2, 0.50f));
  CheckLerp(value1, value2, 0.75f, Vector4::Lerp(value1, value2, 0.75f));
}


TEST(TestMath_Vector4, Lerp_Ref)
{
  const Vector4 value1(1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 value2(101.0f, 102.0f, 103.0f, 104.0f);

  Vector4 res1;
  Vector4::Lerp(value1, value2, 0.0f, res1);
  Vector4 res2;
  Vector4::Lerp(value1, value2, 1.0f, res2);
  EXPECT_EQ(value1, res1);
  EXPECT_EQ(value2, res2);

  Vector4 res3;
  Vector4::Lerp(value1, value2, 0.25f, res3);
  CheckLerp(value1, value2, 0.25f, res3);
  Vector4::Lerp(value1, value2, 0.50f, res3);
  CheckLerp(value1, value2, 0.50f, res3);
  Vector4::Lerp(value1, value2, 0.75f, res3);
  CheckLerp(value1, value2, 0.75f, res3);
}

TEST(TestMath_Vector4, Normalize)
{
  Vector4 value1(4.0f, 0.0f, 0.0f, 0.0f);
  Vector4 value2(0.0f, 4.0f, 0.0f, 0.0f);
  Vector4 value3(0.0f, 0.0f, 4.0f, 0.0f);
  Vector4 value4(0.0f, 0.0f, 0.0f, 4.0f);
  Vector4 value5(2.0f, 4.0f, 8.0f, 4.0f);
  Vector4 value6(4.0f, 8.0f, 4.0f, 2.0f);
  Vector4 value7(8.0f, 4.0f, 2.0f, 4.0f);
  Vector4 value8(4.0f, 2.0f, 4.0f, 8.0f);
  value1.Normalize();
  value2.Normalize();
  value3.Normalize();
  value4.Normalize();
  value5.Normalize();
  value6.Normalize();
  value7.Normalize();
  value8.Normalize();

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(1.0f, 0.0f, 0.0f, 0.0f), value1));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.0f, 1.0f, 0.0f, 0.0f), value2));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.0f, 0.0f, 1.0f, 0.0f), value3));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.0f, 0.0f, 0.0f, 1.0f), value4));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.2f, 0.4f, 0.8f, 0.4f), value5));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.4f, 0.8f, 0.4f, 0.2f), value6));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.8f, 0.4f, 0.2f, 0.4f), value7));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.4f, 0.2f, 0.4f, 0.8f), value8));
}


TEST(TestMath_Vector4, Normalize_Static)
{
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(1.0f, 0.0f, 0.0f, 0.0f), Vector4::Normalize(Vector4(4.0f, 0.0f, 0.0f, 0.0f))));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.0f, 1.0f, 0.0f, 0.0f), Vector4::Normalize(Vector4(0.0f, 4.0f, 0.0f, 0.0f))));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.0f, 0.0f, 1.0f, 0.0f), Vector4::Normalize(Vector4(0.0f, 0.0f, 4.0f, 0.0f))));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4::Normalize(Vector4(0.0f, 0.0f, 0.0f, 4.0f))));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.2f, 0.4f, 0.8f, 0.4f), Vector4::Normalize(Vector4(2.0f, 4.0f, 8.0f, 4.0f))));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.4f, 0.8f, 0.4f, 0.2f), Vector4::Normalize(Vector4(4.0f, 8.0f, 4.0f, 2.0f))));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.8f, 0.4f, 0.2f, 0.4f), Vector4::Normalize(Vector4(8.0f, 4.0f, 2.0f, 4.0f))));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.4f, 0.2f, 0.4f, 0.8f), Vector4::Normalize(Vector4(4.0f, 2.0f, 4.0f, 8.0f))));
}

TEST(TestMath_Vector4, Normalize_Static_Ref)
{
  Vector4 res1;
  Vector4 res2;
  Vector4 res3;
  Vector4 res4;
  Vector4 res5;
  Vector4 res6;
  Vector4 res7;
  Vector4 res8;
  Vector4::Normalize(Vector4(4.0f, 0.0f, 0.0f, 0.0f), res1);
  Vector4::Normalize(Vector4(0.0f, 4.0f, 0.0f, 0.0f), res2);
  Vector4::Normalize(Vector4(0.0f, 0.0f, 4.0f, 0.0f), res3);
  Vector4::Normalize(Vector4(0.0f, 0.0f, 0.0f, 4.0f), res4);
  Vector4::Normalize(Vector4(2.0f, 4.0f, 8.0f, 4.0f), res5);
  Vector4::Normalize(Vector4(4.0f, 8.0f, 4.0f, 2.0f), res6);
  Vector4::Normalize(Vector4(8.0f, 4.0f, 2.0f, 4.0f), res7);
  Vector4::Normalize(Vector4(4.0f, 2.0f, 4.0f, 8.0f), res8);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(1.0f, 0.0f, 0.0f, 0.0f), res1));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.0f, 1.0f, 0.0f, 0.0f), res2));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.0f, 0.0f, 1.0f, 0.0f), res3));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.0f, 0.0f, 0.0f, 1.0f), res4));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.2f, 0.4f, 0.8f, 0.4f), res5));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.4f, 0.8f, 0.4f, 0.2f), res6));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.8f, 0.4f, 0.2f, 0.4f), res7));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.4f, 0.2f, 0.4f, 0.8f), res8));
}


TEST(TestMath_Vector4, Reflect)
{
  const Vector4 value1(1.0f, 2.0f, 3.0f, 4.0f);
  EXPECT_EQ(Vector4(-1.0f, 2.0f, 3.0f, 4.0f), Vector4::Reflect(value1, Vector4(1.0f, 0.0f, 0.0f, 0.0f)));
  EXPECT_EQ(Vector4(1.0f, -2.0f, 3.0f, 4.0f), Vector4::Reflect(value1, Vector4(0.0f, 1.0f, 0.0f, 0.0f)));
  EXPECT_EQ(Vector4(1.0f, 2.0f, -3.0f, 4.0f), Vector4::Reflect(value1, Vector4(0.0f, 0.0f, 1.0f, 0.0f)));
  EXPECT_EQ(Vector4(1.0f, 2.0f, 3.0f, -4.0f), Vector4::Reflect(value1, Vector4(0.0f, 0.0f, 0.0f, 1.0f)));
}


TEST(TestMath_Vector4, Reflect_Ref)
{
  const Vector4 value1(1.0f, 2.0f, 3.0f, 4.0f);

  Vector4 res1;
  Vector4 res2;
  Vector4 res3;
  Vector4 res4;
  Vector4::Reflect(value1, Vector4(1.0f, 0.0f, 0.0f, 0.0f), res1);
  Vector4::Reflect(value1, Vector4(0.0f, 1.0f, 0.0f, 0.0f), res2);
  Vector4::Reflect(value1, Vector4(0.0f, 0.0f, 1.0f, 0.0f), res3);
  Vector4::Reflect(value1, Vector4(0.0f, 0.0f, 0.0f, 1.0f), res4);
  EXPECT_EQ(Vector4(-1.0f, 2.0f, 3.0f, 4.0f), res1);
  EXPECT_EQ(Vector4(1.0f, -2.0f, 3.0f, 4.0f), res2);
  EXPECT_EQ(Vector4(1.0f, 2.0f, -3.0f, 4.0f), res3);
  EXPECT_EQ(Vector4(1.0f, 2.0f, 3.0f, -4.0f), res4);
}


TEST(TestMath_Vector4, SmoothStep)
{
  const Vector4 value1(1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 value2(11.0f, 12.0f, 13.0f, 14.0f);

  CheckSmoothStep(value1, value2, 0.00f);
  CheckSmoothStep(value1, value2, 1.00f);
  CheckSmoothStep(value1, value2, 0.25f);
  CheckSmoothStep(value1, value2, 0.50f);
  CheckSmoothStep(value1, value2, 0.75f);
  CheckSmoothStep(value1, value2, 1.00f);
}

TEST(TestMath_Vector4, Transform)
{
  const auto matRotateX90 = Matrix::CreateRotationX(MathHelper::RADS90);
  const auto matRotateY90 = Matrix::CreateRotationY(MathHelper::RADS90);
  const auto matRotateZ90 = Matrix::CreateRotationZ(MathHelper::RADS90);
  const Vector4 value1(1.0f, 0.0f, 0.0f, 0.0f);
  const Vector4 value2(0.0f, 1.0f, 0.0f, 0.0f);
  const Vector4 value3(0.0f, 0.0f, 1.0f, 0.0f);
  const Vector4 value4(0.0f, 0.0f, 0.0f, 1.0f);
  constexpr float tolerance = 0.0000001f;

  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(1.0f, 0.0f, 0.0f, 0.0f), Vector4::Transform(value1, matRotateX90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 1.0f, 0.0f), Vector4::Transform(value2, matRotateX90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, -1.0f, 0.0f, 0.0f), Vector4::Transform(value3, matRotateX90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4::Transform(value4, matRotateX90), tolerance));

  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, -1.0f, 0.0f), Vector4::Transform(value1, matRotateY90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 1.0f, 0.0f, 0.0f), Vector4::Transform(value2, matRotateY90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(1.0f, 0.0f, 0.0f, 0.0f), Vector4::Transform(value3, matRotateY90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4::Transform(value4, matRotateY90), tolerance));

  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 1.0f, 0.0f, 0.0f), Vector4::Transform(value1, matRotateZ90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(-1.0f, 0.0f, 0.0f, 0.0f), Vector4::Transform(value2, matRotateZ90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 1.0f, 0.0f), Vector4::Transform(value3, matRotateZ90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4::Transform(value4, matRotateZ90), tolerance));
}

TEST(TestMath_Vector4, Transform_Ref)
{
  const auto matRotateX90 = Matrix::CreateRotationX(MathHelper::RADS90);
  const auto matRotateY90 = Matrix::CreateRotationY(MathHelper::RADS90);
  const auto matRotateZ90 = Matrix::CreateRotationZ(MathHelper::RADS90);
  const Vector4 value1(1.0f, 0.0f, 0.0f, 0.0f);
  const Vector4 value2(0.0f, 1.0f, 0.0f, 0.0f);
  const Vector4 value3(0.0f, 0.0f, 1.0f, 0.0f);
  const Vector4 value4(0.0f, 0.0f, 0.0f, 1.0f);
  constexpr float tolerance = 0.0000001f;

  {
    Vector4 res1;
    Vector4 res2;
    Vector4 res3;
    Vector4 res4;
    Vector4::Transform(value1, matRotateX90, res1);
    Vector4::Transform(value2, matRotateX90, res2);
    Vector4::Transform(value3, matRotateX90, res3);
    Vector4::Transform(value4, matRotateX90, res4);

    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(1.0f, 0.0f, 0.0f, 0.0f), res1, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 1.0f, 0.0f), res2, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, -1.0f, 0.0f, 0.0f), res3, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 0.0f, 1.0f), res4, tolerance));
  }

  {
    Vector4 res1;
    Vector4 res2;
    Vector4 res3;
    Vector4 res4;
    Vector4::Transform(value1, matRotateY90, res1);
    Vector4::Transform(value2, matRotateY90, res2);
    Vector4::Transform(value3, matRotateY90, res3);
    Vector4::Transform(value4, matRotateY90, res4);

    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, -1.0f, 0.0f), res1, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 1.0f, 0.0f, 0.0f), res2, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(1.0f, 0.0f, 0.0f, 0.0f), res3, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 0.0f, 1.0f), res4, tolerance));
  }

  {
    Vector4 res1;
    Vector4 res2;
    Vector4 res3;
    Vector4 res4;
    Vector4::Transform(value1, matRotateZ90, res1);
    Vector4::Transform(value2, matRotateZ90, res2);
    Vector4::Transform(value3, matRotateZ90, res3);
    Vector4::Transform(value4, matRotateZ90, res4);

    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 1.0f, 0.0f, 0.0f), res1, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(-1.0f, 0.0f, 0.0f, 0.0f), res2, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 1.0f, 0.0f), res3, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 0.0f, 1.0f), res4, tolerance));
  }
}


TEST(TestMath_Vector4, Transform_Vector3)
{
  const auto matRotateX90 = Matrix::CreateRotationX(MathHelper::RADS90);
  const auto matRotateY90 = Matrix::CreateRotationY(MathHelper::RADS90);
  const auto matRotateZ90 = Matrix::CreateRotationZ(MathHelper::RADS90);
  const Vector3 value1(1.0f, 0.0f, 0.0f);
  const Vector3 value2(0.0f, 1.0f, 0.0f);
  const Vector3 value3(0.0f, 0.0f, 1.0f);
  constexpr float tolerance = 0.0000001f;

  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(1.0f, 0.0f, 0.0f, 0.0f), Vector4::Transform(value1, matRotateX90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 1.0f, 0.0f), Vector4::Transform(value2, matRotateX90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, -1.0f, 0.0f, 0.0f), Vector4::Transform(value3, matRotateX90), tolerance));

  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, -1.0f, 0.0f), Vector4::Transform(value1, matRotateY90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 1.0f, 0.0f, 0.0f), Vector4::Transform(value2, matRotateY90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(1.0f, 0.0f, 0.0f, 0.0f), Vector4::Transform(value3, matRotateY90), tolerance));

  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 1.0f, 0.0f, 0.0f), Vector4::Transform(value1, matRotateZ90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(-1.0f, 0.0f, 0.0f, 0.0f), Vector4::Transform(value2, matRotateZ90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 1.0f, 0.0f), Vector4::Transform(value3, matRotateZ90), tolerance));
}


TEST(TestMath_Vector4, Transform_Vector3_Ref)
{
  const auto matRotateX90 = Matrix::CreateRotationX(MathHelper::RADS90);
  const auto matRotateY90 = Matrix::CreateRotationY(MathHelper::RADS90);
  const auto matRotateZ90 = Matrix::CreateRotationZ(MathHelper::RADS90);
  const Vector3 value1(1.0f, 0.0f, 0.0f);
  const Vector3 value2(0.0f, 1.0f, 0.0f);
  const Vector3 value3(0.0f, 0.0f, 1.0f);
  constexpr float tolerance = 0.0000001f;

  {
    Vector4 res1;
    Vector4 res2;
    Vector4 res3;
    Vector4::Transform(value1, matRotateX90, res1);
    Vector4::Transform(value2, matRotateX90, res2);
    Vector4::Transform(value3, matRotateX90, res3);

    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(1.0f, 0.0f, 0.0f, 0.0f), res1, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 1.0f, 0.0f), res2, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, -1.0f, 0.0f, 0.0f), res3, tolerance));
  }

  {
    Vector4 res1;
    Vector4 res2;
    Vector4 res3;
    Vector4::Transform(value1, matRotateY90, res1);
    Vector4::Transform(value2, matRotateY90, res2);
    Vector4::Transform(value3, matRotateY90, res3);

    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, -1.0f, 0.0f), res1, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 1.0f, 0.0f, 0.0f), res2, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(1.0f, 0.0f, 0.0f, 0.0f), res3, tolerance));
  }

  {
    Vector4 res1;
    Vector4 res2;
    Vector4 res3;
    Vector4::Transform(value1, matRotateZ90, res1);
    Vector4::Transform(value2, matRotateZ90, res2);
    Vector4::Transform(value3, matRotateZ90, res3);

    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 1.0f, 0.0f, 0.0f), res1, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(-1.0f, 0.0f, 0.0f, 0.0f), res2, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 1.0f, 0.0f), res3, tolerance));
  }
}


TEST(TestMath_Vector4, Transform_Vector2)
{
  const auto matRotateX90 = Matrix::CreateRotationX(MathHelper::RADS90);
  const auto matRotateY90 = Matrix::CreateRotationY(MathHelper::RADS90);
  const auto matRotateZ90 = Matrix::CreateRotationZ(MathHelper::RADS90);
  const Vector2 value1(1.0f, 0.0f);
  const Vector2 value2(0.0f, 1.0f);
  constexpr float tolerance = 0.0000001f;

  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(1.0f, 0.0f, 0.0f, 0.0f), Vector4::Transform(value1, matRotateX90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 1.0f, 0.0f), Vector4::Transform(value2, matRotateX90), tolerance));

  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, -1.0f, 0.0f), Vector4::Transform(value1, matRotateY90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 1.0f, 0.0f, 0.0f), Vector4::Transform(value2, matRotateY90), tolerance));

  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 1.0f, 0.0f, 0.0f), Vector4::Transform(value1, matRotateZ90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(-1.0f, 0.0f, 0.0f, 0.0f), Vector4::Transform(value2, matRotateZ90), tolerance));
}


TEST(TestMath_Vector4, Transform_Vector2_Ref)
{
  const auto matRotateX90 = Matrix::CreateRotationX(MathHelper::RADS90);
  const auto matRotateY90 = Matrix::CreateRotationY(MathHelper::RADS90);
  const auto matRotateZ90 = Matrix::CreateRotationZ(MathHelper::RADS90);
  const Vector2 value1(1.0f, 0.0f);
  const Vector2 value2(0.0f, 1.0f);
  constexpr float tolerance = 0.0000001f;

  {
    Vector4 res1;
    Vector4 res2;
    Vector4::Transform(value1, matRotateX90, res1);
    Vector4::Transform(value2, matRotateX90, res2);

    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(1.0f, 0.0f, 0.0f, 0.0f), res1, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, 1.0f, 0.0f), res2, tolerance));
  }

  {
    Vector4 res1;
    Vector4 res2;
    Vector4::Transform(value1, matRotateY90, res1);
    Vector4::Transform(value2, matRotateY90, res2);

    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 0.0f, -1.0f, 0.0f), res1, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 1.0f, 0.0f, 0.0f), res2, tolerance));
  }

  {
    Vector4 res1;
    Vector4 res2;
    Vector4::Transform(value1, matRotateZ90, res1);
    Vector4::Transform(value2, matRotateZ90, res2);

    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(0.0f, 1.0f, 0.0f, 0.0f), res1, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector4(-1.0f, 0.0f, 0.0f, 0.0f), res2, tolerance));
  }
}

TEST(TestMath_Vector4, Add)
{
  const Vector4 value1(1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 value2(11.0f, 12.0f, 13.0f, 14.0f);

  Vector4 res1;
  Vector4::Add(value1, value2, res1);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(12.0f, 14.0f, 16.0f, 18.0f), res1));
}

TEST(TestMath_Vector4, Add_Operator)
{
  const Vector4 value1(1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 value2(11.0f, 12.0f, 13.0f, 14.0f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(12.0f, 14.0f, 16.0f, 18.0f), value1 + value2));
}

TEST(TestMath_Vector4, Subtract)
{
  const Vector4 value1(11.0f, 13.0f, 15.0f, 17.0f);
  const Vector4 value2(1.0f, 2.0f, 3.0f, 4.0f);

  Vector4 res1;
  Vector4::Subtract(value1, value2, res1);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(10.0f, 11.0f, 12.0f, 13.0f), res1));
}


TEST(TestMath_Vector4, Subtract_Operator)
{
  const Vector4 value1(11.0f, 13.0f, 15.0f, 17.0f);
  const Vector4 value2(1.0f, 2.0f, 3.0f, 4.0f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(10.0f, 11.0f, 12.0f, 13.0f), value1 - value2));
}

TEST(TestMath_Vector4, Multiply)
{
  const Vector4 value1(2.0f, 3.0f, 4.0f, 5.0f);
  const Vector4 value2(6.0f, 7.0f, 8.0f, 9.0f);

  Vector4 res1;
  Vector4::Multiply(value1, value2, res1);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(12.0f, 21.0f, 32.0f, 45.0f), res1));
}


TEST(TestMath_Vector4, Multiply_Operator)
{
  const Vector4 value1(2.0f, 3.0f, 4.0f, 5.0f);
  const Vector4 value2(6.0f, 7.0f, 8.0f, 9.0f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(12.0f, 21.0f, 32.0f, 45.0f), value1 * value2));
}


TEST(TestMath_Vector4, Divide)
{
  const Vector4 value1(6.0f, 20.0f, 30.0f, 42.0f);
  const Vector4 value2(2.0f, 4.0f, 5.0f, 6.0f);

  Vector4 res1;
  Vector4::Divide(value1, value2, res1);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(3.0f, 5.0f, 6.0f, 7.0f), res1));
}


TEST(TestMath_Vector4, Divide_Operator)
{
  const Vector4 value1(6.0f, 20.0f, 30.0f, 42.0f);
  const Vector4 value2(2.0f, 4.0f, 5.0f, 6.0f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(3.0f, 5.0f, 6.0f, 7.0f), value1 / value2));
}

TEST(TestMath_Vector4, Equal)
{
  const Vector4 value1(1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 value2(1.0f, 2.0f, 3.0f, 4.0f);

  EXPECT_EQ(value1, value2);
  EXPECT_EQ(value2, value1);
  EXPECT_EQ(value1, value1);
  EXPECT_EQ(value2, value2);
}


TEST(TestMath_Vector4, NotEqual)
{
  const Vector4 value1(1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 value2(-1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 value3(1.0f, -2.0f, 3.0f, 4.0f);
  const Vector4 value4(1.0f, 2.0f, -3.0f, 4.0f);
  const Vector4 value5(1.0f, 2.0f, 3.0f, -4.0f);

  EXPECT_NE(value1, value2);
  EXPECT_NE(value1, value3);
  EXPECT_NE(value1, value4);
  EXPECT_NE(value1, value5);
}
