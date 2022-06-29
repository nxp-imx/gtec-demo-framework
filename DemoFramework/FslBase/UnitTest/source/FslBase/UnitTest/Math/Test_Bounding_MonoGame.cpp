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

// The functions in this file are a port of an MIT licensed library: MonoGame - Bounding.cs.

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogBoundingBox.hpp>
#include <FslBase/Log/Math/LogBoundingFrustum.hpp>
#include <FslBase/Log/Math/LogBoundingSphere.hpp>
#include <FslBase/Log/Math/LogContainmentType.hpp>
#include <FslBase/Log/Math/LogRay.hpp>
#include <FslBase/Math/BoundingBox.hpp>
#include <FslBase/Math/BoundingFrustum.hpp>
#include <FslBase/Math/BoundingSphere.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Ray.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/Test/Math/TestBoundingSphere.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Bounding_MonoGame = TestFixtureFslBase;
}


TEST(TestMath_Plane_MonoGame, BoxContainsVector3)
{
  BoundingBox box(Vector3::Zero(), Vector3::One());

  EXPECT_EQ(box.Contains(-Vector3::One()), ContainmentType::Disjoint);
  EXPECT_EQ(box.Contains(Vector3(0.5f, 0.5f, -1.0f)), ContainmentType::Disjoint);
  EXPECT_EQ(box.Contains(Vector3::Zero()), ContainmentType::Contains);
  EXPECT_EQ(box.Contains(Vector3(0.0f, 0.0f, 0.5f)), ContainmentType::Contains);
  EXPECT_EQ(box.Contains(Vector3(0.0f, 0.5f, 0.5f)), ContainmentType::Contains);
  EXPECT_EQ(box.Contains(Vector3::One()), ContainmentType::Contains);
  EXPECT_EQ(box.Contains(Vector3(1.0f, 1.0f, 0.5f)), ContainmentType::Contains);
  EXPECT_EQ(box.Contains(Vector3(1.0f, 0.5f, 0.5f)), ContainmentType::Contains);
  EXPECT_EQ(box.Contains(Vector3(0.5f, 0.5f, 0.5f)), ContainmentType::Contains);
}


TEST(TestMath_Plane_MonoGame, BoxContainsIdenticalBox)
{
  const BoundingBox b1(Vector3::Zero(), Vector3::One());
  const BoundingBox b2(Vector3::Zero(), Vector3::One());

  EXPECT_EQ(b1.Contains(b2), ContainmentType::Contains);
}


TEST(TestMath_Plane_MonoGame, BoundingSphereTests)
{
  auto zeroPoint = BoundingSphere::CreateFromPoints({Vector3::Zero()});
  EXPECT_EQ(zeroPoint, BoundingSphere());

  auto onePoint = BoundingSphere::CreateFromPoints({Vector3::One()});
  EXPECT_EQ(onePoint, BoundingSphere(Vector3::One(), 0.0f));

  auto twoPoint = BoundingSphere::CreateFromPoints({Vector3::Zero(), Vector3::One()});
  EXPECT_EQ(twoPoint, BoundingSphere(Vector3(0.5f, 0.5f, 0.5f), 0.8660254f));

  auto threePoint = BoundingSphere::CreateFromPoints({Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f)});
  EXPECT_TRUE(Fsl::Test::IsFloatEqual(BoundingSphere(Vector3(0.0f, 0.5f, 0.5f), 1.224745f), threePoint));

  std::vector<Vector3> eightPointTestInput = {Vector3(54.58071f, 124.9063f, 56.0016f),   Vector3(54.52138f, 124.9063f, 56.13985f),
                                              Vector3(54.52208f, 124.8235f, 56.14014f),  Vector3(54.5814f, 124.8235f, 56.0019f),
                                              Vector3(1145.415f, 505.913f, -212.5173f),  Vector3(611.4731f, 505.9535f, 1031.893f),
                                              Vector3(617.7462f, -239.7422f, 1034.584f), Vector3(1151.687f, -239.7035f, -209.8246f)};
  auto eightPoint = BoundingSphere::CreateFromPoints(eightPointTestInput);
  for (std::size_t i = 0; i < eightPointTestInput.size(); ++i)
  {
    EXPECT_NE(eightPoint.Contains(eightPointTestInput[i]), ContainmentType::Disjoint);
  }

  std::vector<Vector3> empty;
  EXPECT_THROW(BoundingSphere::CreateFromPoints(empty), std::invalid_argument);
}


TEST(TestMath_Plane_MonoGame, BoundingBoxContainsBoundingSphere)
{
  BoundingSphere testSphere(Vector3::Zero(), 1.0f);
  const BoundingBox testBox(-Vector3::One(), Vector3::One());

  EXPECT_EQ(testBox.Contains(testSphere), ContainmentType::Contains);

  testSphere.Center -= Vector3::One();

  EXPECT_EQ(testBox.Contains(testSphere), ContainmentType::Intersects);

  testSphere.Center -= Vector3::One();

  EXPECT_EQ(testBox.Contains(testSphere), ContainmentType::Disjoint);
}


TEST(TestMath_Plane_MonoGame, BoundingFrustumToBoundingBoxTests)
{
  auto view = Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 5.0f), Vector3::Zero(), Vector3::Up());
  auto projection = Matrix::CreatePerspectiveFieldOfView(MathHelper::PiOver4, 1.0f, 1.0f, 100.0f);
  auto testFrustum = BoundingFrustum(view * projection);

  BoundingBox bbox1(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.f));
  EXPECT_EQ(testFrustum.Contains(bbox1), ContainmentType::Contains);
  EXPECT_TRUE(testFrustum.Intersects(bbox1));

  BoundingBox bbox2(Vector3(-1000.0f, -1000.0f, -1000.0f), Vector3(1000.0f, 1000.0f, 1000.0f));
  EXPECT_EQ(testFrustum.Contains(bbox2), ContainmentType::Intersects);
  EXPECT_TRUE(testFrustum.Intersects(bbox2));

  BoundingBox bbox3(Vector3(-1000.0f, -1000.0f, -1000.0f), Vector3(0.0f, 0.0f, 0.0f));
  EXPECT_EQ(testFrustum.Contains(bbox3), ContainmentType::Intersects);
  EXPECT_TRUE(testFrustum.Intersects(bbox3));

  BoundingBox bbox4(Vector3(-1000.0f, -1000.0f, -1000.0f), Vector3(-500.0f, -500.0f, -500.0f));
  EXPECT_EQ(testFrustum.Contains(bbox4), ContainmentType::Disjoint);
  EXPECT_FALSE(testFrustum.Intersects(bbox4));
}


TEST(TestMath_Plane_MonoGame, BoundingFrustumToBoundingFrustumTests)
{
  auto view = Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 5.0f), Vector3::Zero(), Vector3::Up());
  auto projection = Matrix::CreatePerspectiveFieldOfView(MathHelper::PiOver4, 1.0f, 1.0f, 100.0f);
  BoundingFrustum testFrustum(view * projection);

  // Same frustum.
  EXPECT_EQ(testFrustum.Contains(testFrustum), ContainmentType::Contains);
  EXPECT_TRUE(testFrustum.Intersects(testFrustum));

  BoundingFrustum otherFrustum(Matrix::GetIdentity());

  // Smaller frustum contained entirely inside.
  auto view2 = Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 4.0f), Vector3::Zero(), Vector3::Up());
  auto projection2 = Matrix::CreatePerspectiveFieldOfView(MathHelper::PiOver4, 1.0f, 1.0f, 50.0f);
  otherFrustum.SetMatrix(view2 * projection2);

  EXPECT_EQ(testFrustum.Contains(otherFrustum), ContainmentType::Contains);
  EXPECT_TRUE(testFrustum.Intersects(otherFrustum));

  // Same size frustum, pointing in the same direction and offset by a small amount.
  otherFrustum.SetMatrix(view2 * projection);

  EXPECT_EQ(testFrustum.Contains(otherFrustum), ContainmentType::Intersects);
  EXPECT_TRUE(testFrustum.Intersects(otherFrustum));

  // Same size frustum, pointing in the opposite direction and not overlapping.
  auto view3 = Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 6.0f), Vector3(0.0f, 0.0f, 7.0f), Vector3::Up());
  otherFrustum.SetMatrix(view3 * projection);

  EXPECT_EQ(testFrustum.Contains(otherFrustum), ContainmentType::Disjoint);
  EXPECT_FALSE(testFrustum.Intersects(otherFrustum));

  // Larger frustum, entirely containing test frustum.
  auto view4 = Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 10.0f), Vector3::Zero(), Vector3::Up());
  auto projection4 = Matrix::CreatePerspectiveFieldOfView(MathHelper::PiOver4, 1.0f, 1.0f, 1000.0f);
  otherFrustum.SetMatrix(view4 * projection4);

  EXPECT_EQ(testFrustum.Contains(otherFrustum), ContainmentType::Intersects);
  EXPECT_TRUE(testFrustum.Intersects(otherFrustum));

  BoundingFrustum bf(Matrix::CreateLookAt(Vector3(0.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3::Up()) *
                     Matrix::CreatePerspectiveFieldOfView(MathHelper::PiOver4, 1.3f, 0.1f, 1000.0f));
  Ray ray(Vector3(0.0f, 0.5f, 0.5f), Vector3(0.0f, 0.0f, 0.0f));
  Ray ray2(Vector3(0.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));

  float value = 42.0f;
  float value2 = 42.0f;
  EXPECT_TRUE(bf.Intersects(ray, value));
  EXPECT_FALSE(bf.Intersects(ray2, value2));
  EXPECT_EQ(value, 0.0f);
}
