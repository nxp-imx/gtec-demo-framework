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

// The functions in this file are a port of an MIT licensed library: MonoGame - QuaternionTest.cs.

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogQuaternion.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Quaternion.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/Test/Math/TestQuaternion.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Quaternion_MonoGame = TestFixtureFslBase;
}


TEST(TestMath_Quaternion_MonoGame, Constructors)
{
  Quaternion expected;
  expected.X = 1.0f;
  expected.Y = 2.0f;
  expected.Z = 3.0f;
  expected.W = 4.0f;
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, Quaternion(1.0f, 2.0f, 3.0f, 4.0f)));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, Quaternion(Vector3(1.0f, 2.0f, 3.0f), 4.0f)));
}


TEST(TestMath_Quaternion_MonoGame, Properties)
{
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(Quaternion(0, 0, 0, 1), Quaternion::Identity()));
}


TEST(TestMath_Quaternion_MonoGame, Add)
{
  const Quaternion q1(1.0f, 2.0f, 3.0f, 4.0f);
  const Quaternion q2(1.0f, 2.0f, 3.0f, 4.0f);
  const Quaternion expected(2.0f, 4.0f, 6.0f, 8.0f);
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, Quaternion::Add(q1, q2)));

  Quaternion result;
  Quaternion::Add(result, q1, q2);
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, result));
}


TEST(TestMath_Quaternion_MonoGame, Concatenate)
{
  const Quaternion q1(1.0f, 2.5f, 3.0f, 4.0f);
  const Quaternion q2(1.0f, 2.0f, -3.8f, 2.0f);
  const Quaternion expected(21.5f, 6.2f, -8.7f, 13.4f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, Quaternion::Concatenate(q1, q2)));

  Quaternion result;
  Quaternion::Concatenate(result, q1, q2);
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, result));
}


TEST(TestMath_Quaternion_MonoGame, Conjugate)
{
  const Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);
  const Quaternion expected(-1.0f, -2.0f, -3.0f, 4.0f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, Quaternion::Conjugate(q)));

  Quaternion result;
  Quaternion::Conjugate(result, q);
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, result));

  auto qCopy = q;
  qCopy.Conjugate();
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, qCopy));
}


TEST(TestMath_Quaternion_MonoGame, CreateFromAxisAngle)
{
  const Vector3 axis(0.5f, 1.1f, -3.8f);
  const float angle = 0.25f;
  const Quaternion expected(0.06233737f, 0.1371422f, -0.473764f, 0.9921977f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, Quaternion::CreateFromAxisAngle(axis, angle)));

  Quaternion result;
  Quaternion::CreateFromAxisAngle(result, axis, angle);
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, result));
}


TEST(TestMath_Quaternion_MonoGame, CreateFromRotationMatrix)
{
  const auto matrix = Matrix::CreateFromYawPitchRoll(0.15f, 1.18f, -0.22f);
  const Quaternion expected(0.5446088f, 0.1227905f, -0.1323988f, 0.8190203f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, Quaternion::CreateFromRotationMatrix(matrix)));

  Quaternion result;
  Quaternion::CreateFromRotationMatrix(result, matrix);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, result));
}


TEST(TestMath_Quaternion_MonoGame, CreateFromYawPitchRoll)
{
  const Quaternion expected(0.5446088f, 0.1227905f, -0.1323988f, 0.8190203f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, Quaternion::CreateFromYawPitchRoll(0.15f, 1.18f, -0.22f)));

  Quaternion result;
  Quaternion::CreateFromYawPitchRoll(result, 0.15f, 1.18f, -0.22f);
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, result));
}


TEST(TestMath_Quaternion_MonoGame, Divide)
{
  const Quaternion q1(1.0f, 2.0f, 3.0f, 4.0f);
  const Quaternion q2(0.2f, -0.6f, 11.9f, 0.01f);
  const Quaternion expected(-0.1858319f, 0.09661285f, -0.3279344f, 0.2446305f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, Quaternion::Divide(q1, q2)));

  Quaternion result;
  Quaternion::Divide(result, q1, q2);
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, result));
}


TEST(TestMath_Quaternion_MonoGame, Length)
{
  const Quaternion q1(1.0f, 2.0f, 3.0f, 4.0f);

  EXPECT_FLOAT_EQ(5.477226f, q1.Length());
}


TEST(TestMath_Quaternion_MonoGame, LengthSquared)
{
  const Quaternion q1(1.0f, 2.0f, 3.0f, 4.0f);

  EXPECT_FLOAT_EQ(30.0f, q1.LengthSquared());
}


TEST(TestMath_Quaternion_MonoGame, Normalize)
{
  const Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);
  const Quaternion expected(0.1825742f, 0.3651484f, 0.5477226f, 0.7302967f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, Quaternion::Normalize(q)));

  {
    Quaternion result;
    Quaternion::Normalize(result, q);
    EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, result));
  }

  auto qCopy = q;
  qCopy.Normalize();
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expected, qCopy));
}
