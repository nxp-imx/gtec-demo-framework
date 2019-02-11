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

// The functions in this file are a port of an MIT licensed library: MonoGame - RayTest.cs.

#include <FslBase/Math/Ray.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/BoundingBox.hpp>
#include <FslBase/Log/Math/LogRay.hpp>
#include <FslBase/Log/Math/LogBoundingBox.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Ray_MonoGame = TestFixtureFslBase;
}


TEST(TestMath_Ray_MonoGame, BoundingBoxIntersects)
{
  // Our test box.
  BoundingBox box(Vector3(-10.0f, -20.0f, -30.0f), Vector3(10.0f, 20.0f, 30.0f));
  const auto center = (box.Max + box.Min) * 0.5f;

  // Test misses.
  float result;
  EXPECT_FALSE(Ray(center - Vector3::UnitX() * 40.0f, -Vector3::UnitX()).Intersects(box, result));
  EXPECT_FALSE(Ray(center + Vector3::UnitX() * 40.0f, Vector3::UnitX()).Intersects(box, result));
  EXPECT_FALSE(Ray(center - Vector3::UnitY() * 40.0f, -Vector3::UnitY()).Intersects(box, result));
  EXPECT_FALSE(Ray(center + Vector3::UnitY() * 40.0f, Vector3::UnitY()).Intersects(box, result));
  EXPECT_FALSE(Ray(center - Vector3::UnitZ() * 40.0f, -Vector3::UnitZ()).Intersects(box, result));
  EXPECT_FALSE(Ray(center + Vector3::UnitZ() * 40.0f, Vector3::UnitZ()).Intersects(box, result));

  // Test middle of each face.
  EXPECT_TRUE(Ray(center - Vector3::UnitX() * 40.0f, Vector3::UnitX()).Intersects(box, result));
  EXPECT_EQ(30.0f, result);
  EXPECT_TRUE(Ray(center + Vector3::UnitX() * 40.0f, -Vector3::UnitX()).Intersects(box, result));
  EXPECT_EQ(30.0f, result);
  EXPECT_TRUE(Ray(center - Vector3::UnitY() * 40.0f, Vector3::UnitY()).Intersects(box, result));
  EXPECT_EQ(20.0f, result);
  EXPECT_TRUE(Ray(center + Vector3::UnitY() * 40.0f, -Vector3::UnitY()).Intersects(box, result));
  EXPECT_FLOAT_EQ(20.0f, result);
  EXPECT_TRUE(Ray(center - Vector3::UnitZ() * 40.0f, Vector3::UnitZ()).Intersects(box, result));
  EXPECT_EQ(10.0f, result);
  EXPECT_TRUE(Ray(center + Vector3::UnitZ() * 40.0f, -Vector3::UnitZ()).Intersects(box, result));
  EXPECT_EQ(10.0f, result);

  // Test the corners along each axis.
  EXPECT_TRUE(Ray(box.Min - Vector3::UnitX() * 10.0f, Vector3::UnitX()).Intersects(box, result));
  EXPECT_EQ(10.0f, result);
  EXPECT_TRUE(Ray(box.Min - Vector3::UnitY() * 10.0f, Vector3::UnitY()).Intersects(box, result));
  EXPECT_EQ(10.0f, result);
  EXPECT_TRUE(Ray(box.Min - Vector3::UnitZ() * 10.0f, Vector3::UnitZ()).Intersects(box, result));
  EXPECT_EQ(10.0f, result);
  EXPECT_TRUE(Ray(box.Max + Vector3::UnitX() * 10.0f, -Vector3::UnitX()).Intersects(box, result));
  EXPECT_EQ(10.0f, result);
  EXPECT_TRUE(Ray(box.Max + Vector3::UnitY() * 10.0f, -Vector3::UnitY()).Intersects(box, result));
  EXPECT_EQ(10.0f, result);
  EXPECT_TRUE(Ray(box.Max + Vector3::UnitZ() * 10.0f, -Vector3::UnitZ()).Intersects(box, result));
  EXPECT_EQ(10.0f, result);

  // Test inside out.
  EXPECT_TRUE(Ray(center, Vector3::UnitX()).Intersects(box, result));
  EXPECT_EQ(0.0f, result);
  EXPECT_TRUE(Ray(center, -Vector3::UnitX()).Intersects(box, result));
  EXPECT_EQ(0.0f, result);
  EXPECT_TRUE(Ray(center, Vector3::UnitY()).Intersects(box, result));
  EXPECT_EQ(0.0f, result);
  EXPECT_TRUE(Ray(center, -Vector3::UnitY()).Intersects(box, result));
  EXPECT_EQ(0.0f, result);
  EXPECT_TRUE(Ray(center, Vector3::UnitZ()).Intersects(box, result));
  EXPECT_EQ(0.0f, result);
  EXPECT_TRUE(Ray(center, -Vector3::UnitZ()).Intersects(box, result));
  EXPECT_EQ(0.0f, result);
}
