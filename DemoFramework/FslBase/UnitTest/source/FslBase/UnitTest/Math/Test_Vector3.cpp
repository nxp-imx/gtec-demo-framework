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
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Log/Math/LogVector3.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslBase/UnitTest/Helper/Test/Math/TestVector3.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Vector3 = TestFixtureFslBase;

  // This code relies on MathHelper being correct
  void CheckBarycentric(const Vector3& value1, const Vector3& value2, const Vector3& value3, const float amount1, const float amount2)
  {
    {
      const auto res = Vector3::Barycentric(value1, value2, value3, amount1, amount2);
      EXPECT_EQ(MathHelper::Barycentric(value1.X, value2.X, value3.X, amount1, amount2), res.X);
      EXPECT_EQ(MathHelper::Barycentric(value1.Y, value2.Y, value3.Y, amount1, amount2), res.Y);
      EXPECT_EQ(MathHelper::Barycentric(value1.Z, value2.Z, value3.Z, amount1, amount2), res.Z);
    }
    {
      Vector3 res;
      Vector3::Barycentric(value1, value2, value3, amount1, amount2, res);
      EXPECT_EQ(MathHelper::Barycentric(value1.X, value2.X, value3.X, amount1, amount2), res.X);
      EXPECT_EQ(MathHelper::Barycentric(value1.Y, value2.Y, value3.Y, amount1, amount2), res.Y);
      EXPECT_EQ(MathHelper::Barycentric(value1.Z, value2.Z, value3.Z, amount1, amount2), res.Z);
    }
  }

  // This code relies on MathHelper being correct
  void CheckCatmullRom(const Vector3& value1, const Vector3& value2, const Vector3& value3, const Vector3& value4, const float amount)
  {
    {
      const auto res = Vector3::CatmullRom(value1, value2, value3, value4, amount);
      EXPECT_EQ(MathHelper::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount), res.X);
      EXPECT_EQ(MathHelper::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount), res.Y);
      EXPECT_EQ(MathHelper::CatmullRom(value1.Z, value2.Z, value3.Z, value4.Z, amount), res.Z);
    }
    {
      Vector3 res;
      Vector3::CatmullRom(value1, value2, value3, value4, amount, res);
      EXPECT_EQ(MathHelper::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount), res.X);
      EXPECT_EQ(MathHelper::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount), res.Y);
      EXPECT_EQ(MathHelper::CatmullRom(value1.Z, value2.Z, value3.Z, value4.Z, amount), res.Z);
    }
  }

  // This code relies on MathHelper being correct
  void CheckClamp(const Vector3& value, const Vector3& minValue, const Vector3& maxValue)
  {
    {
      const auto res = Vector3::Clamp(value, minValue, maxValue);
      EXPECT_EQ(MathHelper::Clamp(value.X, minValue.X, maxValue.X), res.X);
      EXPECT_EQ(MathHelper::Clamp(value.Y, minValue.Y, maxValue.Y), res.Y);
      EXPECT_EQ(MathHelper::Clamp(value.Z, minValue.Z, maxValue.Z), res.Z);
    }
    {
      Vector3 res;
      Vector3::Clamp(value, minValue, maxValue, res);
      EXPECT_EQ(MathHelper::Clamp(value.X, minValue.X, maxValue.X), res.X);
      EXPECT_EQ(MathHelper::Clamp(value.Y, minValue.Y, maxValue.Y), res.Y);
      EXPECT_EQ(MathHelper::Clamp(value.Z, minValue.Z, maxValue.Z), res.Z);
    }
  }

  // This code relies on MathHelper being correct
  void CheckHermite(const Vector3& value1, const Vector3& value2, const Vector3& value3, const Vector3& value4, const float amount)
  {
    {
      const auto res = Vector3::Hermite(value1, value2, value3, value4, amount);
      EXPECT_EQ(MathHelper::Hermite(value1.X, value2.X, value3.X, value4.X, amount), res.X);
      EXPECT_EQ(MathHelper::Hermite(value1.Y, value2.Y, value3.Y, value4.Y, amount), res.Y);
      EXPECT_EQ(MathHelper::Hermite(value1.Z, value2.Z, value3.Z, value4.Z, amount), res.Z);
    }
    {
      Vector3 res;
      Vector3::Hermite(value1, value2, value3, value4, amount, res);
      EXPECT_EQ(MathHelper::Hermite(value1.X, value2.X, value3.X, value4.X, amount), res.X);
      EXPECT_EQ(MathHelper::Hermite(value1.Y, value2.Y, value3.Y, value4.Y, amount), res.Y);
      EXPECT_EQ(MathHelper::Hermite(value1.Z, value2.Z, value3.Z, value4.Z, amount), res.Z);
    }
  }

  void CheckLerp(const Vector3& value1, const Vector3& value2, const float t, const Vector3& res)
  {
    EXPECT_EQ(MathHelper::Lerp(value1.X, value2.X, t), res.X);
    EXPECT_EQ(MathHelper::Lerp(value1.Y, value2.Y, t), res.Y);
    EXPECT_EQ(MathHelper::Lerp(value1.Z, value2.Z, t), res.Z);
  }


  // This code relies on MathHelper being correct
  void CheckSmoothStep(const Vector3& value1, const Vector3& value2, const float amount)
  {
    {
      const auto res = Vector3::SmoothStep(value1, value2, amount);
      EXPECT_EQ(MathHelper::SmoothStep(value1.X, value2.X, amount), res.X);
      EXPECT_EQ(MathHelper::SmoothStep(value1.Y, value2.Y, amount), res.Y);
      EXPECT_EQ(MathHelper::SmoothStep(value1.Z, value2.Z, amount), res.Z);
    }
    {
      Vector3 res;
      Vector3::SmoothStep(value1, value2, amount, res);
      EXPECT_EQ(MathHelper::SmoothStep(value1.X, value2.X, amount), res.X);
      EXPECT_EQ(MathHelper::SmoothStep(value1.Y, value2.Y, amount), res.Y);
      EXPECT_EQ(MathHelper::SmoothStep(value1.Z, value2.Z, amount), res.Z);
    }
  }
}


TEST(TestMath_Vector3, Construct)
{
  Vector3 value(1.0f, 2.0f, 3.0f);

  EXPECT_EQ(1.0f, value.X);
  EXPECT_EQ(2.0f, value.Y);
  EXPECT_EQ(3.0f, value.Z);
}


TEST(TestMath_Vector3, Barycentric)
{
  const Vector3 value1(1.0f, 2.0f, 3.0f);
  const Vector3 value2(11.0f, 12.0f, 13.0f);
  const Vector3 value3(-11.0f, -12.0f, -13.0f);

  CheckBarycentric(value1, value2, value3, 0.0f, 0.0f);
  CheckBarycentric(value1, value2, value3, 1.0f, 0.0f);
  CheckBarycentric(value1, value2, value3, 0.0f, 1.0f);
  CheckBarycentric(value1, value2, value3, 0.5f, 0.5f);
  CheckBarycentric(value1, value2, value3, 1.0f, 1.0f);
}


TEST(TestMath_Vector3, CatmullRom)
{
  const Vector3 value1(1.0f, 2.0f, 3.0f);
  const Vector3 value2(11.0f, 12.0f, 13.0f);
  const Vector3 value3(-11.0f, -12.0f, -13.0f);
  const Vector3 value4(-21.0f, -22.0f, -23.0f);

  CheckCatmullRom(value1, value2, value3, value4, 0.00f);
  CheckCatmullRom(value1, value2, value3, value4, 1.00f);
  CheckCatmullRom(value1, value2, value3, value4, 0.25f);
  CheckCatmullRom(value1, value2, value3, value4, 0.50f);
  CheckCatmullRom(value1, value2, value3, value4, 0.75f);
  CheckCatmullRom(value1, value2, value3, value4, 1.00f);
}


TEST(TestMath_Vector3, CheckClamp)
{
  const Vector3 minValue(1.0f, 2.0f, 3.0f);
  const Vector3 maxValue(5.0f, 6.0f, 7.0f);
  const Vector3 value(-1, 7.0f, 5.0f);

  CheckClamp(value, minValue, maxValue);
}


TEST(TestMath_Vector3, Distance)
{
  const Vector3 vec1(1.0f, 2.0f, 3.0f);
  const Vector3 vec2(4.0f, 8.0f, 9.0f);

  EXPECT_FLOAT_EQ(9.0f, Vector3::Distance(vec1, vec2));
}


TEST(TestMath_Vector3, DistanceSquared)
{
  const Vector3 vec1(1.0f, 2.0f, 3.0f);
  const Vector3 vec2(4.0f, 8.0f, 9.0f);

  EXPECT_FLOAT_EQ(81.0f, Vector3::DistanceSquared(vec1, vec2));
}


TEST(TestMath_Vector3, Hermite)
{
  const Vector3 value1(1.0f, 2.0f, 3.0f);
  const Vector3 value2(11.0f, 12.0f, 13.0f);
  const Vector3 value3(-11.0f, -12.0f, -13.0f);
  const Vector3 value4(-21.0f, -22.0f, -23.0f);

  CheckHermite(value1, value2, value3, value4, 0.00f);
  CheckHermite(value1, value2, value3, value4, 1.00f);
  CheckHermite(value1, value2, value3, value4, 0.25f);
  CheckHermite(value1, value2, value3, value4, 0.50f);
  CheckHermite(value1, value2, value3, value4, 0.75f);
  CheckHermite(value1, value2, value3, value4, 1.00f);
}


TEST(TestMath_Vector3, Length)
{
  const Vector3 value(2.0f, 3.0f, 6.0f);

  EXPECT_FLOAT_EQ(7.0f, value.Length());
}


TEST(TestMath_Vector3, LengthSquared)
{
  const Vector3 value(2.0f, 3.0f, 6.0f);

  EXPECT_FLOAT_EQ(49.0f, value.LengthSquared());
}


TEST(TestMath_Vector3, Lerp)
{
  const Vector3 value1(1.0f, 2.0f, 3.0f);
  const Vector3 value2(101.0f, 102.0f, 103.0f);

  EXPECT_EQ(value1, Vector3::Lerp(value1, value2, 0.0f));
  EXPECT_EQ(value2, Vector3::Lerp(value1, value2, 1.0f));

  CheckLerp(value1, value2, 0.25f, Vector3::Lerp(value1, value2, 0.25f));
  CheckLerp(value1, value2, 0.50f, Vector3::Lerp(value1, value2, 0.50f));
  CheckLerp(value1, value2, 0.75f, Vector3::Lerp(value1, value2, 0.75f));
}


TEST(TestMath_Vector3, Lerp_Ref)
{
  const Vector3 value1(1.0f, 2.0f, 3.0f);
  const Vector3 value2(101.0f, 102.0f, 103.0f);

  Vector3 res1;
  Vector3::Lerp(value1, value2, 0.0f, res1);
  Vector3 res2;
  Vector3::Lerp(value1, value2, 1.0f, res2);
  EXPECT_EQ(value1, res1);
  EXPECT_EQ(value2, res2);

  Vector3 res3;
  Vector3::Lerp(value1, value2, 0.25f, res3);
  CheckLerp(value1, value2, 0.25f, res3);
  Vector3::Lerp(value1, value2, 0.50f, res3);
  CheckLerp(value1, value2, 0.50f, res3);
  Vector3::Lerp(value1, value2, 0.75f, res3);
  CheckLerp(value1, value2, 0.75f, res3);
}

TEST(TestMath_Vector3, Max)
{
  const Vector3 val1(1.0f, 6.0f, 3.0f);
  const Vector3 val2(5.0f, 2.0f, 7.0f);
  const Vector3 expValue(5.0f, 6.0f, 7.0f);

  EXPECT_EQ(expValue, Vector3::Max(val1, val2));

  Vector3 res;
  Vector3::Max(val1, val2, res);
  EXPECT_EQ(expValue, res);
}


TEST(TestMath_Vector3, Min)
{
  const Vector3 val1(1.0f, 6.0f, 3.0f);
  const Vector3 val2(5.0f, 2.0f, 7.0f);
  const Vector3 expValue(1.0f, 2.0f, 3.0f);

  EXPECT_EQ(expValue, Vector3::Min(val1, val2));

  Vector3 res;
  Vector3::Min(val1, val2, res);
  EXPECT_EQ(expValue, res);
}

TEST(TestMath_Vector3, Negate)
{
  const Vector3 value(1.0f, 2.0f, 3.0f);
  const Vector3 expectedValue(-1.0f, -2.0f, -3.0f);

  EXPECT_EQ(expectedValue, Vector3::Negate(value));

  const auto res = -value;

  EXPECT_EQ(expectedValue, res);

  Vector3 res2;
  Vector3::Negate(value, res2);
  EXPECT_EQ(expectedValue, res2);
}


TEST(TestMath_Vector3, Normalize)
{
  constexpr float tolerance = 0.000001f;
  Vector3 value1(4.0f, 0.0f, 0.0f);
  Vector3 value2(0.0f, 4.0f, 0.0f);
  Vector3 value3(0.0f, 0.0f, 4.0f);
  Vector3 value4(1.0f, 2.0f, 3.0f);
  Vector3 value5(2.0f, 3.0f, 1.0f);
  Vector3 value6(3.0f, 1.0f, 2.0f);
  value1.Normalize();
  value2.Normalize();
  value3.Normalize();
  value4.Normalize();
  value5.Normalize();
  value6.Normalize();

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(1.0f, 0.0f, 0.0f), value1));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(0.0f, 1.0f, 0.0f), value2));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(0.0f, 0.0f, 1.0f), value3));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.267261f, 0.534522f, 0.801784f), value4, tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.534522f, 0.801784f, 0.267261f), value5, tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.801784f, 0.267261f, 0.534522f), value6, tolerance));
}


TEST(TestMath_Vector3, Normalize_Static)
{
  constexpr float tolerance = 0.000001f;

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(1.0f, 0.0f, 0.0f), Vector3::Normalize(Vector3(4.0f, 0.0f, 0.0f))));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(0.0f, 1.0f, 0.0f), Vector3::Normalize(Vector3(0.0f, 4.0f, 0.0f))));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(0.0f, 0.0f, 1.0f), Vector3::Normalize(Vector3(0.0f, 0.0f, 4.0f))));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.267261f, 0.534522f, 0.801784f), Vector3::Normalize(Vector3(1.0f, 2.0f, 3.0f)), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.534522f, 0.801784f, 0.267261f), Vector3::Normalize(Vector3(2.0f, 3.0f, 1.0f)), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.801784f, 0.267261f, 0.534522f), Vector3::Normalize(Vector3(3.0f, 1.0f, 2.0f)), tolerance));
}


TEST(TestMath_Vector3, Normalize_Static_Ref)
{
  constexpr float tolerance = 0.000001f;

  Vector3 res1, res2, res3, res4, res5, res6;
  Vector3::Normalize(Vector3(4.0f, 0.0f, 0.0f), res1);
  Vector3::Normalize(Vector3(0.0f, 4.0f, 0.0f), res2);
  Vector3::Normalize(Vector3(0.0f, 0.0f, 4.0f), res3);
  Vector3::Normalize(Vector3(1.0f, 2.0f, 3.0f), res4);
  Vector3::Normalize(Vector3(2.0f, 3.0f, 1.0f), res5);
  Vector3::Normalize(Vector3(3.0f, 1.0f, 2.0f), res6);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(1.0f, 0.0f, 0.0f), res1));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(0.0f, 1.0f, 0.0f), res2));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(0.0f, 0.0f, 1.0f), res3));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.267261f, 0.534522f, 0.801784f), res4, tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.534522f, 0.801784f, 0.267261f), res5, tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.801784f, 0.267261f, 0.534522f), res6, tolerance));
}


TEST(TestMath_Vector3, Reflect)
{
  const Vector3 value1(1.0f, 2.0f, 3.0f);
  EXPECT_EQ(Vector3(-1.0f, 2.0f, 3.0f), Vector3::Reflect(value1, Vector3(1.0f, 0.0f, 0.0f)));
  EXPECT_EQ(Vector3(1.0f, -2.0f, 3.0f), Vector3::Reflect(value1, Vector3(0.0f, 1.0f, 0.0f)));
  EXPECT_EQ(Vector3(1.0f, 2.0f, -3.0f), Vector3::Reflect(value1, Vector3(0.0f, 0.0f, 1.0f)));
}


TEST(TestMath_Vector3, Reflect_Ref)
{
  const Vector3 value1(1.0f, 2.0f, 3.0f);

  Vector3 res1;
  Vector3 res2;
  Vector3 res3;
  Vector3::Reflect(value1, Vector3(1.0f, 0.0f, 0.0f), res1);
  Vector3::Reflect(value1, Vector3(0.0f, 1.0f, 0.0f), res2);
  Vector3::Reflect(value1, Vector3(0.0f, 0.0f, 1.0f), res3);
  EXPECT_EQ(Vector3(-1.0f, 2.0f, 3.0f), res1);
  EXPECT_EQ(Vector3(1.0f, -2.0f, 3.0f), res2);
  EXPECT_EQ(Vector3(1.0f, 2.0f, -3.0f), res3);
}

TEST(TestMath_Vector3, TransformNormal)
{
  const auto matRotateX90 = Matrix::CreateRotationX(MathHelper::RADS90);
  const auto matRotateY90 = Matrix::CreateRotationY(MathHelper::RADS90);
  const auto matRotateZ90 = Matrix::CreateRotationZ(MathHelper::RADS90);
  const Vector3 value1(1.0f, 0.0f, 0.0f);
  const Vector3 value2(0.0f, 1.0f, 0.0f);
  const Vector3 value3(0.0f, 0.0f, 1.0f);
  constexpr float tolerance = 0.0000001f;

  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(1.0f, 0.0f, 0.0f), Vector3::TransformNormal(value1, matRotateX90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.0f, 0.0f, 1.0f), Vector3::TransformNormal(value2, matRotateX90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.0f, -1.0f, 0.0f), Vector3::TransformNormal(value3, matRotateX90), tolerance));

  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.0f, 0.0f, -1.0f), Vector3::TransformNormal(value1, matRotateY90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.0f, 1.0f, 0.0f), Vector3::TransformNormal(value2, matRotateY90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(1.0f, 0.0f, 0.0f), Vector3::TransformNormal(value3, matRotateY90), tolerance));

  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.0f, 1.0f, 0.0f), Vector3::TransformNormal(value1, matRotateZ90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(-1.0f, 0.0f, 0.0f), Vector3::TransformNormal(value2, matRotateZ90), tolerance));
  EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.0f, 0.0f, 1.0f), Vector3::TransformNormal(value3, matRotateZ90), tolerance));
}


TEST(TestMath_Vector3, TransformNormal_Ref)
{
  const auto matRotateX90 = Matrix::CreateRotationX(MathHelper::RADS90);
  const auto matRotateY90 = Matrix::CreateRotationY(MathHelper::RADS90);
  const auto matRotateZ90 = Matrix::CreateRotationZ(MathHelper::RADS90);
  const Vector3 value1(1.0f, 0.0f, 0.0f);
  const Vector3 value2(0.0f, 1.0f, 0.0f);
  const Vector3 value3(0.0f, 0.0f, 1.0f);
  constexpr float tolerance = 0.0000001f;

  {
    Vector3 res1, res2, res3;
    Vector3::TransformNormal(value1, matRotateX90, res1);
    Vector3::TransformNormal(value2, matRotateX90, res2);
    Vector3::TransformNormal(value3, matRotateX90, res3);
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(1.0f, 0.0f, 0.0f), res1, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.0f, 0.0f, 1.0f), res2, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.0f, -1.0f, 0.0f), res3, tolerance));
  }

  {
    Vector3 res1, res2, res3;
    Vector3::TransformNormal(value1, matRotateY90, res1);
    Vector3::TransformNormal(value2, matRotateY90, res2);
    Vector3::TransformNormal(value3, matRotateY90, res3);

    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.0f, 0.0f, -1.0f), res1, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.0f, 1.0f, 0.0f), res2, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(1.0f, 0.0f, 0.0f), res3, tolerance));
  }

  {
    Vector3 res1, res2, res3;
    Vector3::TransformNormal(value1, matRotateZ90, res1);
    Vector3::TransformNormal(value2, matRotateZ90, res2);
    Vector3::TransformNormal(value3, matRotateZ90, res3);

    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.0f, 1.0f, 0.0f), res1, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(-1.0f, 0.0f, 0.0f), res2, tolerance));
    EXPECT_TRUE(Fsl::Test::IsFloatNear(Vector3(0.0f, 0.0f, 1.0f), res3, tolerance));
  }
}


TEST(TestMath_Vector3, SmoothStep)
{
  const Vector3 value1(1.0f, 2.0f, 3.0f);
  const Vector3 value2(11.0f, 12.0f, 13.0f);

  CheckSmoothStep(value1, value2, 0.00f);
  CheckSmoothStep(value1, value2, 1.00f);
  CheckSmoothStep(value1, value2, 0.25f);
  CheckSmoothStep(value1, value2, 0.50f);
  CheckSmoothStep(value1, value2, 0.75f);
  CheckSmoothStep(value1, value2, 1.00f);
}

TEST(TestMath_Vector3, Add)
{
  const Vector3 value1(1.0f, 2.0f, 3.0f);
  const Vector3 value2(11.0f, 12.0f, 13.0f);

  Vector3 res1;
  Vector3::Add(value1, value2, res1);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(12.0f, 14.0f, 16.0f), res1));
}

TEST(TestMath_Vector3, Add_Operator)
{
  const Vector3 value1(1.0f, 2.0f, 3.0f);
  const Vector3 value2(11.0f, 12.0f, 13.0f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(12.0f, 14.0f, 16.0f), value1 + value2));
}

TEST(TestMath_Vector3, Subtract)
{
  const Vector3 value1(11.0f, 13.0f, 15.0f);
  const Vector3 value2(1.0f, 2.0f, 3.0f);

  Vector3 res1;
  Vector3::Subtract(value1, value2, res1);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(10.0f, 11.0f, 12.0f), res1));
}


TEST(TestMath_Vector3, Subtract_Operator)
{
  const Vector3 value1(11.0f, 13.0f, 15.0f);
  const Vector3 value2(1.0f, 2.0f, 3.0f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(10.0f, 11.0f, 12.0f), value1 - value2));
}

TEST(TestMath_Vector3, Multiply)
{
  const Vector3 value1(2.0f, 3.0f, 4.0f);
  const Vector3 value2(5.0f, 6.0f, 7.0f);

  Vector3 res1;
  Vector3::Multiply(value1, value2, res1);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(10.0f, 18.0f, 28.0f), res1));
}


TEST(TestMath_Vector3, Multiply_Operator)
{
  const Vector3 value1(2.0f, 3.0f, 4.0f);
  const Vector3 value2(5.0f, 6.0f, 7.0f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(10.0f, 18.0f, 28.0f), value1 * value2));
}


TEST(TestMath_Vector3, Divide)
{
  const Vector3 value1(6.0f, 20.0f, 30.0f);
  const Vector3 value2(2.0f, 4.0f, 5.0f);

  Vector3 res1;
  Vector3::Divide(value1, value2, res1);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(3.0f, 5.0f, 6.0f), res1));
}


TEST(TestMath_Vector3, Divide_Operator)
{
  const Vector3 value1(6.0f, 20.0f, 30.0f);
  const Vector3 value2(2.0f, 4.0f, 5.0f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector3(3.0f, 5.0f, 6.0f), value1 / value2));
}

TEST(TestMath_Vector3, Equal)
{
  const Vector3 value1(1.0f, 2.0f, 3.0f);
  const Vector3 value2(1.0f, 2.0f, 3.0f);

  EXPECT_EQ(value1, value2);
  EXPECT_EQ(value2, value1);
  EXPECT_EQ(value1, value1);
  EXPECT_EQ(value2, value2);
}


TEST(TestMath_Vector3, NotEqual)
{
  const Vector3 value1(1.0f, 2.0f, 3.0f);
  const Vector3 value2(-1.0f, 2.0f, 3.0f);
  const Vector3 value3(1.0f, -2.0f, 3.0f);
  const Vector3 value4(1.0f, 2.0f, -3.0f);

  EXPECT_NE(value1, value2);
  EXPECT_NE(value1, value3);
  EXPECT_NE(value1, value4);
}
