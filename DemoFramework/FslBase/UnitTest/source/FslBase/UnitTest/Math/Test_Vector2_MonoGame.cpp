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

// The functions in this file are a port of an MIT licensed library: MonoGame - Vector2Test.cs.

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogVector2.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Quaternion.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/Test/Math/TestVector2.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Vector2_MonoGame = TestFixtureFslBase;
}


TEST(TestMath_Vector2_MonoGame, CatmullRom)
{
  const Vector2 expectedResult(5.1944f, 6.1944f);
  const Vector2 v1(1.0f, 2.0f);
  const Vector2 v2(3.0f, 4.0f);
  const Vector2 v3(5.0f, 6.0f);
  const Vector2 v4(7.0f, 8.0f);
  const float value = 1.0972f;

  Vector2 result;
  Vector2::CatmullRom(v1, v2, v3, v4, value, result);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult, result));

  // We expect the two operations to give the exact same result
  auto result2 = Vector2::CatmullRom(v1, v2, v3, v4, value);
  EXPECT_EQ(result.X, result2.X);
  EXPECT_EQ(result.Y, result2.Y);
}


TEST(TestMath_Vector2_MonoGame, Multiply)
{
  const Vector2 vector(1, 2);

  // Test 0.0 scale.
  EXPECT_EQ(Vector2::Zero(), 0.0f * vector);
  EXPECT_EQ(Vector2::Zero(), vector * 0);
  EXPECT_EQ(Vector2::Zero(), vector * 0.0f);

  // Test 1.0 scale.
  EXPECT_EQ(vector, 1.0f * vector);
  EXPECT_EQ(vector, vector * 1.0f);
  EXPECT_EQ(vector, vector * 1.0f);

  auto scaledVec = vector * 2.0f;

  // Test 2.0 scale.
  EXPECT_EQ(scaledVec, 2 * vector);
  EXPECT_EQ(scaledVec, vector * 2);
  EXPECT_EQ(vector * 2.0f, scaledVec);

  scaledVec = vector * 0.999f;

  // Test 0.999 scale.
  EXPECT_EQ(scaledVec, 0.999f * vector);
  EXPECT_EQ(scaledVec, vector * 0.999f);
  EXPECT_EQ(vector * 0.999f, scaledVec);

  Vector2 vector2(2, 2);

  // Test two vectors multiplication.
  EXPECT_EQ(Vector2(vector.X * vector2.X, vector.Y * vector2.Y), vector * vector2);
  EXPECT_EQ(vector2 * vector, Vector2(vector.X * vector2.X, vector.Y * vector2.Y));
}


TEST(TestMath_Vector2_MonoGame, Hermite)
{
  const Vector2 t1(1.40625f, 1.40625f);
  const Vector2 t2(2.662375f, 2.26537514f);

  const Vector2 v1(1, 1);
  const Vector2 v2(2, 2);
  const Vector2 v3(3, 3);
  const Vector2 v4(4, 4);
  const Vector2 v5(4, 3);
  const Vector2 v6(2, 1);
  const Vector2 v7(1, 2);
  const Vector2 v8(3, 4);

  auto resultHermite1 = Vector2::Hermite(v1, v2, v3, v4, 0.25f);
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(t1, resultHermite1));

  auto resultHermite2 = Vector2::Hermite(v5, v6, v7, v8, 0.45f);
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(t2, resultHermite2));

  Vector2 result1;
  Vector2 result2;

  Vector2::Hermite(v1, v2, v3, v4, 0.25f, result1);
  Vector2::Hermite(v5, v6, v7, v8, 0.45f, result2);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(t1, result1));
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(t2, result2));
}


TEST(TestMath_Vector2_MonoGame, Transform)
{
  // STANDART OVERLOADS TEST

  const Vector2 expectedResult1(24.0f, 28.0f);
  const Vector2 expectedResult2(-0.0168301091f, 2.30964f);

  const Vector2 v1(1.0f, 2.0f);
  const Matrix m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

  const Vector2 v2(1.1f, 2.45f);
  const Quaternion q2(0.11f, 0.22f, 0.33f, 0.55f);
  // const Quaternion q3(1, 2, 3, 4);

  {
    {
      auto result1 = Vector2::Transform(v1, m1);
      auto result2 = Vector2::Transform(v2, q2);
      EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult1, result1));
      EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult2, result2));
    }

    // OUTPUT OVERLOADS TEST
    {
      // IMPROVEMENT: fix output parameter ordering so it matches the original monogame order!
      Vector2 result1;
      Vector2 result2;
      Vector2::Transform(v1, m1, result1);
      Vector2::Transform(v2, q2, result2);

      EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult1, result1));
      EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult2, result2));
    }
  }

  //// TRANSFORM ON LIST (MATRIX)
  //{
  //  var sourceList1 = new Vector2[10];
  //  var desinationList1 = new Vector2[10];

  //  for (int i = 0; i < 10; i++)
  //  {
  //    sourceList1[i] = (new Vector2(1 + i, 1 + i));
  //  }

  //  Vector2.Transform(sourceList1, 0, ref m1, desinationList1, 0, 10);

  //  for (int i = 0; i < 10; i++)
  //  {
  //    Assert.That(desinationList1[i], Is.EqualTo(new Vector2(19 + (6 * i), 22 + (8 * i))).Using(Vector2Comparer.Epsilon));
  //  }
  //}
  //// TRANSFORM ON LIST (MATRIX)(DESTINATION & SOURCE)
  //{
  //  var sourceList2 = new Vector2[10];
  //  var desinationList2 = new Vector2[10];

  //  for (int i = 0; i < 10; i++)
  //  {
  //    sourceList2[i] = (new Vector2(1 + i, 1 + i));
  //  }

  //  Vector2.Transform(sourceList2, 2, ref m1, desinationList2, 1, 3);

  //  Assert.That(Vector2.Zero, Is.EqualTo(desinationList2[0]).Using(Vector2Comparer.Epsilon));

  //  Assert.That(new Vector2(31, 38), Is.EqualTo(desinationList2[1]).Using(Vector2Comparer.Epsilon));
  //  Assert.That(new Vector2(37, 46), Is.EqualTo(desinationList2[2]).Using(Vector2Comparer.Epsilon));
  //  Assert.That(new Vector2(43, 54), Is.EqualTo(desinationList2[3]).Using(Vector2Comparer.Epsilon));

  //  for (int i = 4; i < 10; i++)
  //  {
  //    Assert.That(Vector2.Zero, Is.EqualTo(desinationList2[i]).Using(Vector2Comparer.Epsilon));
  //  }
  //}
  //// TRANSFORM ON LIST (MATRIX)(SIMPLE)
  //{
  //  var sourceList3 = new Vector2[10];
  //  var desinationList3 = new Vector2[10];

  //  for (int i = 0; i < 10; i++)
  //  {
  //    sourceList3[i] = (new Vector2(1 + i, 1 + i));
  //  }

  //  Vector2.Transform(sourceList3, ref m1, desinationList3);

  //  for (int i = 0; i < 10; i++)
  //  {
  //    Assert.That(desinationList3[i], Is.EqualTo(new Vector2(19 + (6 * i), 22 + (8 * i))).Using(Vector2Comparer.Epsilon));
  //  }
  //}
  //// TRANSFORM ON LIST (QUATERNION)
  //{
  //  var sourceList4 = new Vector2[10];
  //  var desinationList4 = new Vector2[10];

  //  for (int i = 0; i < 10; i++)
  //  {
  //    sourceList4[i] = (new Vector2(1 + i, 1 + i));
  //  }

  //  Vector2.Transform(sourceList4, 0, ref q3, desinationList4, 0, 10);

  //  for (int i = 0; i < 10; i++)
  //  {
  //    Assert.That(new Vector2(-45 + (-45 * i), 9 + (9 * i)), Is.EqualTo(desinationList4[i]).Using(Vector2Comparer.Epsilon));
  //  }
  //}
  //// TRANSFORM ON LIST (QUATERNION)(DESTINATION & SOURCE)
  //{
  //  var sourceList5 = new Vector2[10];
  //  var desinationList5 = new Vector2[10];

  //  for (int i = 0; i < 10; i++)
  //  {
  //    sourceList5[i] = (new Vector2(1 + i, 1 + i));
  //  }

  //  Vector2.Transform(sourceList5, 2, ref q3, desinationList5, 1, 3);

  //  Assert.That(Vector2.Zero, Is.EqualTo(desinationList5[0]).Using(Vector2Comparer.Epsilon));

  //  Assert.That(new Vector2(-135, 27), Is.EqualTo(desinationList5[1]).Using(Vector2Comparer.Epsilon));
  //  Assert.That(new Vector2(-180, 36), Is.EqualTo(desinationList5[2]).Using(Vector2Comparer.Epsilon));
  //  Assert.That(new Vector2(-225, 45), Is.EqualTo(desinationList5[3]).Using(Vector2Comparer.Epsilon));

  //  for (int i = 4; i < 10; i++)
  //  {
  //    Assert.That(Vector2.Zero, Is.EqualTo(desinationList5[i]).Using(Vector2Comparer.Epsilon));
  //  }
  //}
  //// TRANSFORM ON LIST (QUATERNION)(SIMPLE)
  //{
  //  var sourceList6 = new Vector2[10];
  //  var desinationList6 = new Vector2[10];

  //  for (int i = 0; i < 10; i++)
  //  {
  //    sourceList6[i] = (new Vector2(1 + i, 1 + i));
  //  }

  //  Vector2.Transform(sourceList6, ref q3, desinationList6);

  //  for (int i = 0; i < 10; i++)
  //  {
  //    Assert.That(new Vector2(-45 + (-45 * i), 9 + (9 * i)), Is.EqualTo(desinationList6[i]).Using(Vector2Comparer.Epsilon));
  //  }
  //}
}


TEST(TestMath_Vector2_MonoGame, TransformNormal)
{
  const Vector2 normal(1.5f, 2.5f);
  const Matrix matrix(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

  {
    const Vector2 expectedResult1(14.0f, 18.0f);
    const auto expectedResult2 = expectedResult1;

    {
      auto res1 = Vector2::TransformNormal(normal, matrix);
      EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult1, res1));
    }

    {
      Vector2 result;
      Vector2::TransformNormal(normal, matrix, result);

      EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult2, result));
    }
  }

  // TRANSFORM ON LIST
  //{
  //  var sourceArray1 = new Vector2[10];
  //  var destinationArray1 = new Vector2[10];

  //  for (int i = 0; i < 10; i++)
  //  {
  //    sourceArray1[i] = new Vector2(i, i);
  //  }

  //  Vector2.TransformNormal(sourceArray1, 0, ref matrix, destinationArray1, 0, 10);

  //  for (int i = 0; i < 10; i++)
  //  {
  //    Assert.That(new Vector2(0 + (6 * i), 0 + (8 * i)), Is.EqualTo(destinationArray1[i]).Using(Vector2Comparer.Epsilon));
  //  }
  //}
  //// TRANSFORM ON LIST(SOURCE OFFSET)
  //{
  //  var sourceArray2 = new Vector2[10];
  //  var destinationArray2 = new Vector2[10];

  //  for (int i = 0; i < 10; i++)
  //  {
  //    sourceArray2[i] = new Vector2(i, i);
  //  }

  //  Vector2.TransformNormal(sourceArray2, 5, ref matrix, destinationArray2, 0, 5);

  //  for (int i = 0; i < 5; i++)
  //  {
  //    Assert.That(new Vector2(30 + (6 * i), 40 + (8 * i)), Is.EqualTo(destinationArray2[i]).Using(Vector2Comparer.Epsilon));
  //  }

  //  for (int i = 5; i < 10; i++)
  //  {
  //    Assert.That(Vector2.Zero, Is.EqualTo(destinationArray2[i]).Using(Vector2Comparer.Epsilon));
  //  }
  //}
  //// TRANSFORM ON LIST(DESTINATION OFFSET)
  //{
  //  var sourceArray3 = new Vector2[10];
  //  var destinationArray3 = new Vector2[10];

  //  for (int i = 0; i < 10; ++i)
  //  {
  //    sourceArray3[i] = new Vector2(i, i);
  //  }

  //  Vector2.TransformNormal(sourceArray3, 0, ref matrix, destinationArray3, 5, 5);

  //  for (int i = 0; i < 6; ++i)
  //  {
  //    Assert.That(Vector2.Zero, Is.EqualTo(destinationArray3[i]).Using(Vector2Comparer.Epsilon));
  //  }

  //  Assert.That(new Vector2(6, 8), Is.EqualTo(destinationArray3[6]).Using(Vector2Comparer.Epsilon));
  //  Assert.That(new Vector2(12, 16), Is.EqualTo(destinationArray3[7]).Using(Vector2Comparer.Epsilon));
  //  Assert.That(new Vector2(18, 24), Is.EqualTo(destinationArray3[8]).Using(Vector2Comparer.Epsilon));
  //  Assert.That(new Vector2(24, 32), Is.EqualTo(destinationArray3[9]).Using(Vector2Comparer.Epsilon));
  //}
  //// TRANSFORM ON LIST(DESTINATION & SOURCE)
  //{
  //  var sourceArray4 = new Vector2[10];
  //  var destinationArray4 = new Vector2[10];

  //  for (int i = 0; i < 10; ++i)
  //  {
  //    sourceArray4[i] = new Vector2(i, i);
  //  }

  //  Vector2.TransformNormal(sourceArray4, 2, ref matrix, destinationArray4, 3, 6);

  //  for (int i = 0; i < 3; ++i)
  //  {
  //    Assert.That(Vector2.Zero, Is.EqualTo(destinationArray4[i]).Using(Vector2Comparer.Epsilon));
  //  }

  //  Assert.That(new Vector2(12, 16), Is.EqualTo(destinationArray4[3]).Using(Vector2Comparer.Epsilon));
  //  Assert.That(new Vector2(18, 24), Is.EqualTo(destinationArray4[4]).Using(Vector2Comparer.Epsilon));
  //  Assert.That(new Vector2(24, 32), Is.EqualTo(destinationArray4[5]).Using(Vector2Comparer.Epsilon));
  //  Assert.That(new Vector2(30, 40), Is.EqualTo(destinationArray4[6]).Using(Vector2Comparer.Epsilon));
  //  Assert.That(new Vector2(36, 48), Is.EqualTo(destinationArray4[7]).Using(Vector2Comparer.Epsilon));
  //  Assert.That(new Vector2(42, 56), Is.EqualTo(destinationArray4[8]).Using(Vector2Comparer.Epsilon));

  //  Assert.That(Vector2.Zero, Is.EqualTo(destinationArray4[9]).Using(Vector2Comparer.Epsilon));
  //}
  //// TRANSFORM ON LIST(SIMPLE)
  //{
  //  var sourceArray5 = new Vector2[10];
  //  var destinationArray5 = new Vector2[10];

  //  for (int i = 0; i < 10; ++i)
  //  {
  //    sourceArray5[i] = new Vector2(i, i);
  //  }

  //  Vector2.TransformNormal(sourceArray5, ref matrix, destinationArray5);

  //  for (int i = 0; i < 10; ++i)
  //  {
  //    Assert.That(new Vector2(0 + (6 * i), 0 + (8 * i)), Is.EqualTo(destinationArray5[i]).Using(Vector2Comparer.Epsilon));
  //  }
  //}
}


TEST(TestMath_Vector2_MonoGame, Construct)
{
  constexpr float x = std::numeric_limits<float>::min();
  constexpr float y = std::numeric_limits<float>::max();
  Vector2 value(x, y);

  EXPECT_EQ(x, value.X);
  EXPECT_EQ(y, value.Y);
}
