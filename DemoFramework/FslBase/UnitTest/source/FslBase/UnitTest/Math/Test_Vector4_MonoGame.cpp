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

// The functions in this file are a port of an MIT licensed library: MonoGame - Vector4Test.cs.

#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Quaternion.hpp>
#include <FslBase/Log/Math/LogVector4.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/Test/Math/TestVector4.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Vector4_MonoGame = TestFixtureFslBase;
}


TEST(TestMath_Vector4_MonoGame, Constructors)
{
  const Vector4 expectedResult(1.0f, 2.0f, 3.0f, 4.0f);
  // const Vector4 expectedResult2(2.2f, 2.2f, 2.2f, 2.2f);


  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult, Vector4(1, 2, 3, 4)));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult, Vector4(Vector2(1, 2), 3, 4)));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult, Vector4(Vector3(1, 2, 3), 4)));
  // EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult2, Vector4(2.2f));
}


TEST(TestMath_Vector4_MonoGame, Properties)
{
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.0f, 0.0f, 0.0f, 0.0f), Vector4::Zero()));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4::One()));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(1.0f, 0.0f, 0.0f, 0.0f), Vector4::UnitX()));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.0f, 1.0f, 0.0f, 0.0f), Vector4::UnitY()));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.0f, 0.0f, 1.0f, 0.0f), Vector4::UnitZ()));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4::UnitW()));
}


TEST(TestMath_Vector4_MonoGame, Dot)
{
  const Vector4 vector1(1.0f, 2.0f, 3.0f, 4.0f);
  const Vector4 vector2(0.5f, 1.1f, -3.8f, 1.2f);
  const float expectedResult = -3.89999962f;

  EXPECT_FLOAT_EQ(expectedResult, Vector4::Dot(vector1, vector2));

  // float result;
  // Vector4::Dot(vector1, vector2, result);

  // EXPECT_FLOAT_EQ(expectedResult, result);
}


TEST(TestMath_Vector4_MonoGame, Hermite)
{
  const Vector4 t1(1.40625f, 1.40625f, 0.2f, 0.92f);
  const Vector4 t2(2.662375f, 2.26537514f, 10.0f, 2.0f);
  const Vector4 v1(1, 2, 3, 4);
  const Vector4 v2(-1.3f, 0.1f, 30.0f, 365.20f);
  const float a = 2.234f;

  const Vector4 expected(39.0311f, 34.65557f, -132.5473f, -2626.85938f);

  {
    auto result1 = Vector4::Hermite(v1, t1, v2, t2, a);
    EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, result1));
  }

  {
    Vector4 result2;
    // same as result1 ? - it must be same
    Vector4::Hermite(v1, t1, v2, t2, a, result2);
    EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, result2));
  }
}


TEST(TestMath_Vector4_MonoGame, Length)
{
  Vector4 vector1(1.0f, 2.0f, 3.0f, 4.0f);

  EXPECT_FLOAT_EQ(5.477226f, vector1.Length());
}


TEST(TestMath_Vector4_MonoGame, LengthSquared)
{
  Vector4 vector1(1.0f, 2.0f, 3.0f, 4.0f);

  EXPECT_FLOAT_EQ(30.0f, vector1.LengthSquared());
}


TEST(TestMath_Vector4_MonoGame, Normalize)
{
  Vector4 vector1(1.0f, 2.0f, 3.0f, 4.0f);
  vector1.Normalize();

  const Vector4 expected(0.1825742f, 0.3651484f, 0.5477225f, 0.7302967f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, vector1));

  const Vector4 vector2(1.0f, 2.0f, 3.0f, 4.0f);

  auto result = Vector4::Normalize(vector2);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, result));
}


TEST(TestMath_Vector4_MonoGame, Construct)
{
  constexpr float x = std::numeric_limits<float>::min();
  constexpr float y = std::numeric_limits<float>::max();
  constexpr float z = 0.5f;
  constexpr float w = -0.75f;
  Vector4 value(x, y, z, w);

  EXPECT_EQ(x, value.X);
  EXPECT_EQ(y, value.Y);
  EXPECT_EQ(z, value.Z);
  EXPECT_EQ(w, value.W);
}
