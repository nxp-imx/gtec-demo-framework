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

// The functions in this file are a port of an MIT licensed library: MonoGame - PlaneTest.cs.

#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Plane.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Quaternion.hpp>
#include <FslBase/Log/Math/LogPlane.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslBase/UnitTest/Helper/Test/Math/TestPlane.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Plane_MonoGame = TestFixtureFslBase;
}


TEST(TestMath_Plane_MonoGame, TransformByMatrix)
{
  // Our test plane.
  const auto plane = Plane::Normalize(Plane(Vector3(0.0f, 1.0f, 1.0f), 2.5f));

  // Our matrix.
  const auto matrix = Matrix::CreateRotationX(MathHelper::PiOver2);

  // Test transform.
  const Plane expectedResult(Vector3(0.0f, -0.7071068f, 0.7071067f), 1.767767f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult, Plane::Transform(plane, matrix)));
}


TEST(TestMath_Plane_MonoGame, TransformByRefMatrix)
{
  // Our test plane.
  const auto plane = Plane::Normalize(Plane(Vector3(1.0f, 0.8f, 0.5f), 2.5f));
  const auto originalPlane = plane;

  // Our matrix.
  const auto matrix = Matrix::CreateRotationX(MathHelper::PiOver2);
  const auto originalMatrix = matrix;

  // Test transform.
  Plane result;
  Plane::Transform(plane, matrix, result);

  const Plane expectedResult(Vector3(0.7273929f, -0.3636965f, 0.5819144f), 1.818482f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult, result));

  EXPECT_EQ(originalPlane, plane);
  EXPECT_EQ(originalMatrix, matrix);
}


TEST(TestMath_Plane_MonoGame, TransformByQuaternion)
{
  // Our test plane.
  const auto plane = Plane::Normalize(Plane(Vector3(0.0f, 1.0f, 1.0f), 2.5f));

  // Our quaternion.
  const auto quaternion = Quaternion::CreateFromAxisAngle(Vector3::UnitX(), MathHelper::PiOver2);

  // Test transform.
  const Plane expectedResult(Vector3(0.0f, -0.7071068f, 0.7071067f), 1.767767f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult, Plane::Transform(plane, quaternion)));
}


TEST(TestMath_Plane_MonoGame, TransformByRefQuaternion)
{
  // Our test plane.
  const auto plane = Plane::Normalize(Plane(Vector3(1.0f, 0.8f, 0.5f), 2.5f));
  const auto originalPlane = plane;

  // Our quaternion.
  const auto quaternion = Quaternion::CreateFromAxisAngle(Vector3::UnitX(), MathHelper::PiOver2);
  const auto originalQuaternion = quaternion;

  // Test transform.
  Plane result;
  Plane::Transform(plane, quaternion, result);

  Plane expectedResult(Vector3(0.7273929f, -0.3636965f, 0.5819144f), 1.818482f);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedResult, result));

  EXPECT_EQ(originalPlane, plane);
  EXPECT_EQ(originalQuaternion, quaternion);
}
