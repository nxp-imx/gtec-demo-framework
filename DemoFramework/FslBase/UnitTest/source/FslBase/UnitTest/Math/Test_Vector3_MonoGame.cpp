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

// The functions in this file are a port of an MIT licensed library: MonoGame - Vector3Test.cs.

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogVector3.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Quaternion.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/Test/Math/TestVector3.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Vector3_MonoGame = TestFixtureFslBase;
}


TEST(TestMath_Vector3_MonoGame, DistanceSquared)
{
  const Vector3 v1(0.1f, 100.0f, -5.5f);
  const Vector3 v2(1.1f, -2.0f, 5.5f);
  const auto d = Vector3::DistanceSquared(v1, v2);
  const float expectedResult = 10526.0f;

  EXPECT_FLOAT_EQ(expectedResult, d);
}


TEST(TestMath_Vector3_MonoGame, Normalize)
{
  Vector3 v1(-10.5f, 0.2f, 1000.0f);
  Vector3 v2(-10.5f, 0.2f, 1000.0f);
  v1.Normalize();

  const Vector3 expectedResult(-0.0104994215f, 0.000199988979f, 0.999944866f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult, v1));

  v2 = Vector3::Normalize(v2);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult, v2));
}


TEST(TestMath_Vector3_MonoGame, Transform)
{
  // STANDART OVERLOADS TEST
  const Vector3 expectedResult1(51.0f, 58.0f, 65.0f);
  const Vector3 expectedResult2(33.0f, -14.0f, -1.0f);

  const Vector3 v1(1.0f, 2.0f, 3.0f);
  const Matrix m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

  const Vector3 v2(1.0f, 2.0f, 3.0f);
  const Quaternion q1(2.0f, 3.0f, 4.0f, 5.0f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult1, Vector3::Transform(v1, m1)));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult2, Vector3::Transform(v2, q1)));

  // OUTPUT OVERLOADS TEST
  {
    Vector3 result1;
    Vector3 result2;

    Vector3::Transform(v1, m1, result1);
    Vector3::Transform(v2, q1, result2);

    EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult1, result1));
    EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult2, result2));
  }
}


TEST(TestMath_Vector3_MonoGame, Construct)
{
  constexpr float X = std::numeric_limits<float>::min();
  constexpr float Y = std::numeric_limits<float>::max();
  constexpr float Z = 0.5f;
  Vector3 value(X, Y, Z);

  EXPECT_EQ(X, value.X);
  EXPECT_EQ(Y, value.Y);
  EXPECT_EQ(Z, value.Z);
}
