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
#include <FslBase/Math/BoundingBox.hpp>
#include <FslBase/Math/BoundingSphere.hpp>
#include <FslBase/Math/BoundingFrustum.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Ray.hpp>
#include <FslBase/Log/Math/LogBoundingBox.hpp>
#include <FslBase/Log/Math/LogContainmentType.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslBase/UnitTest/Helper/Test/Math/TestBoundingBox.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_BoundingBox = TestFixtureFslBase;
}


TEST(TestMath_BoundingBox, Empty)
{
  const BoundingBox value;

  // We expect the exact value
  EXPECT_EQ(0.0f, value.Min.X);
  EXPECT_EQ(0.0f, value.Min.Y);
  EXPECT_EQ(0.0f, value.Min.Z);
  EXPECT_EQ(0.0f, value.Max.X);
  EXPECT_EQ(0.0f, value.Max.Y);
  EXPECT_EQ(0.0f, value.Max.Z);
}


TEST(TestMath_BoundingBox, Filled)
{
  const Vector3 boundsMin(1, 2, 3);
  const Vector3 boundsMax(10, 20, 30);
  const BoundingBox value(boundsMin, boundsMax);

  // We expect the exact value
  EXPECT_EQ(boundsMin, value.Min);
  EXPECT_EQ(boundsMax, value.Max);
}


TEST(TestMath_BoundingBox, Contains_Same)
{
  const BoundingBox outer(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
  const BoundingBox inner(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));

  EXPECT_EQ(ContainmentType::Contains, outer.Contains(inner));
  EXPECT_EQ(ContainmentType::Contains, inner.Contains(outer));
  EXPECT_EQ(ContainmentType::Contains, outer.Contains(outer));
  EXPECT_EQ(ContainmentType::Contains, inner.Contains(inner));
}


TEST(TestMath_BoundingBox, Contains)
{
  const BoundingBox outer(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
  const BoundingBox inner(Vector3(0.0f, 0.0f, 0.0f), Vector3(5.0f, 5.0f, 5.0f));

  EXPECT_EQ(ContainmentType::Contains, outer.Contains(inner));
  EXPECT_EQ(ContainmentType::Intersects, inner.Contains(outer));
}


TEST(TestMath_BoundingBox, Contains_Disjoint)
{
  const BoundingBox box1(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
  const BoundingBox box2(Vector3(11.0f, 0.0f, 00.0f), Vector3(15.0f, 5.0f, 5.0f));
  const BoundingBox box3(Vector3(1.0f, 11.0f, 00.0f), Vector3(8.0f, 12.0f, 5.0f));

  EXPECT_EQ(ContainmentType::Disjoint, box1.Contains(box2));
  EXPECT_EQ(ContainmentType::Disjoint, box2.Contains(box1));
  EXPECT_EQ(ContainmentType::Disjoint, box1.Contains(box3));
}


TEST(TestMath_BoundingBox, Contains_Sphere_Contains)
{
  const BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
  const BoundingSphere sphere(Vector3(5.0f, 5.0f, 5.0f), 5.0f);

  EXPECT_EQ(ContainmentType::Contains, box.Contains(sphere));
}


TEST(TestMath_BoundingBox, Contains_Sphere_Intersects)
{
  const BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
  const BoundingSphere sphere(Vector3(5.0f, 5.1f, 5.0f), 5.0f);

  EXPECT_EQ(ContainmentType::Intersects, box.Contains(sphere));
}


TEST(TestMath_BoundingBox, Contains_Sphere_Disjoint)
{
  const BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
  const BoundingSphere sphere(Vector3(16.0f, 5.0f, 5.0f), 5.0f);

  EXPECT_EQ(ContainmentType::Disjoint, box.Contains(sphere));
}


TEST(TestMath_BoundingBox, Contains_Point_Contains)
{
  const BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
  const Vector3 point(5.0f, 5.0f, 5.0f);

  EXPECT_EQ(ContainmentType::Contains, box.Contains(point));
}


// 'intersected points' are considered contained
TEST(TestMath_BoundingBox, Contains_Point_Intersects)
{
  const BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
  const Vector3 point(0.0f, 5.0f, 5.0f);
  EXPECT_EQ(ContainmentType::Contains, box.Contains(point));
}


TEST(TestMath_BoundingBox, Contains_Point_Disjoint)
{
  const BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
  const Vector3 point(11.0f, 5.0f, 5.0f);

  EXPECT_EQ(ContainmentType::Disjoint, box.Contains(point));
}


TEST(TestMath_BoundingBox, CreateFromPoints)
{
  std::vector<Vector3> points = {Vector3(0.0f, 0.0f, 0.0f), Vector3(3.0f, 3.0f, 5.0f), Vector3(5.0f, 5.0f, 0.0f)};

  auto box = BoundingBox::CreateFromPoints(points);

  EXPECT_EQ(BoundingBox(Vector3(0.0f, 0.0f, 0.0f), Vector3(5.0f, 5.0f, 5.0f)), box);
}

TEST(TestMath_BoundingBox, CreateFromSphere)
{
  const BoundingSphere sphere(Vector3(5.0f, 5.0f, 5.0f), 5.0f);
  auto box = BoundingBox::CreateFromSphere(sphere);
  const BoundingBox expectedBox(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedBox, box));

  BoundingBox box2;
  BoundingBox::CreateFromSphere(sphere, box2);

  EXPECT_TRUE(Fsl::Test::IsFloatEqual(expectedBox, box2));
}


TEST(TestMath_BoundingBox, GetCorners)
{
  BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(5.0f, 5.0f, 5.0f));
  std::vector<Vector3> corners = box.GetCorners();

  EXPECT_EQ(Vector3(0.0f, 5.0f, 5.0f), corners[0]);
  EXPECT_EQ(Vector3(5.0f, 5.0f, 5.0f), corners[1]);
  EXPECT_EQ(Vector3(5.0f, 0.0f, 5.0f), corners[2]);
  EXPECT_EQ(Vector3(0.0f, 0.0f, 5.0f), corners[3]);
  EXPECT_EQ(Vector3(0.0f, 5.0f, 0.0f), corners[4]);
  EXPECT_EQ(Vector3(5.0f, 5.0f, 0.0f), corners[5]);
  EXPECT_EQ(Vector3(5.0f, 0.0f, 0.0f), corners[6]);
  EXPECT_EQ(Vector3(0.0f, 0.0f, 0.0f), corners[7]);
}

TEST(TestMath_BoundingBox, GetCorners_Vector_Ref)
{
  BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(5.0f, 5.0f, 5.0f));
  std::vector<Vector3> corners(8);
  box.GetCorners(corners);

  EXPECT_EQ(Vector3(0.0f, 5.0f, 5.0f), corners[0]);
  EXPECT_EQ(Vector3(5.0f, 5.0f, 5.0f), corners[1]);
  EXPECT_EQ(Vector3(5.0f, 0.0f, 5.0f), corners[2]);
  EXPECT_EQ(Vector3(0.0f, 0.0f, 5.0f), corners[3]);
  EXPECT_EQ(Vector3(0.0f, 5.0f, 0.0f), corners[4]);
  EXPECT_EQ(Vector3(5.0f, 5.0f, 0.0f), corners[5]);
  EXPECT_EQ(Vector3(5.0f, 0.0f, 0.0f), corners[6]);
  EXPECT_EQ(Vector3(0.0f, 0.0f, 0.0f), corners[7]);
}

TEST(TestMath_BoundingBox, GetCorners_Array_Ref)
{
  BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(5.0f, 5.0f, 5.0f));
  std::array<Vector3, 8> corners;
  box.GetCorners(corners);

  EXPECT_EQ(Vector3(0.0f, 5.0f, 5.0f), corners[0]);
  EXPECT_EQ(Vector3(5.0f, 5.0f, 5.0f), corners[1]);
  EXPECT_EQ(Vector3(5.0f, 0.0f, 5.0f), corners[2]);
  EXPECT_EQ(Vector3(0.0f, 0.0f, 5.0f), corners[3]);
  EXPECT_EQ(Vector3(0.0f, 5.0f, 0.0f), corners[4]);
  EXPECT_EQ(Vector3(5.0f, 5.0f, 0.0f), corners[5]);
  EXPECT_EQ(Vector3(5.0f, 0.0f, 0.0f), corners[6]);
  EXPECT_EQ(Vector3(0.0f, 0.0f, 0.0f), corners[7]);
}

TEST(TestMath_BoundingBox, GetCorners_Ref_InvalidSize)
{
  BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(5.0f, 5.0f, 5.0f));
  std::vector<Vector3> corners(7);
  EXPECT_THROW(box.GetCorners(corners), std::invalid_argument);
}


TEST(TestMath_BoundingBox, CreateMerged)
{
  const BoundingBox box1(Vector3(0.0f, 0.0f, 0.0f), Vector3(5.0f, 5.0f, 5.0f));
  const BoundingBox box2(Vector3(3.0f, -1.0f, 2.0f), Vector3(3.0f, 4.0f, 6.0f));

  auto mergedBox = BoundingBox::CreateMerged(box1, box2);

  EXPECT_EQ(0.0f, mergedBox.Min.X);
  EXPECT_EQ(-1.0f, mergedBox.Min.Y);
  EXPECT_EQ(0.0f, mergedBox.Min.Z);
  EXPECT_EQ(5.0f, mergedBox.Max.X);
  EXPECT_EQ(5.0f, mergedBox.Max.Y);
  EXPECT_EQ(6.0f, mergedBox.Max.Z);
}


TEST(TestMath_BoundingBox, Intersects)
{
  const BoundingBox outer(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
  const BoundingBox inner(Vector3(0.0f, 0.0f, 0.0f), Vector3(5.0f, 5.0f, 5.0f));

  EXPECT_TRUE(outer.Intersects(inner));
  EXPECT_TRUE(inner.Intersects(outer));
}


TEST(TestMath_BoundingBox, Intersects_Disjoint)
{
  const BoundingBox box1(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
  const BoundingBox box2(Vector3(11.0f, 0.0f, 00.0f), Vector3(15.0f, 5.0f, 5.0f));
  const BoundingBox box3(Vector3(1.0f, 11.0f, 00.0f), Vector3(8.0f, 12.0f, 5.0f));

  EXPECT_FALSE(box1.Intersects(box2));
  EXPECT_FALSE(box2.Intersects(box1));
  EXPECT_FALSE(box1.Intersects(box3));
}


TEST(TestMath_BoundingBox, Intersects_Frustum)
{
  const auto matPerspective = Matrix::CreatePerspective(320, 240, 0.001f, 10.0f);
  BoundingFrustum frustum(matPerspective);

  const BoundingBox box(Vector3(-5.0f, -5.0f, -5.0f), Vector3(10.0f, 10.0f, 10.0f));

  EXPECT_TRUE(box.Intersects(frustum));
}


TEST(TestMath_BoundingBox, Intersects_Frustum_Disjoint)
{
  const auto matPerspective = Matrix::CreatePerspective(320, 240, 0.001f, 10.0f);
  BoundingFrustum frustum(matPerspective);

  const BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));

  EXPECT_FALSE(box.Intersects(frustum));
}


TEST(TestMath_BoundingBox, Intersects_Sphere_Contains)
{
  const BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
  const BoundingSphere sphere(Vector3(5.0f, 5.0f, 5.0f), 5.0f);

  EXPECT_TRUE(box.Intersects(sphere));
}


TEST(TestMath_BoundingBox, Intersects_Sphere_Intersects)
{
  const BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
  const BoundingSphere sphere(Vector3(5.0f, 5.1f, 5.0f), 5.0f);

  EXPECT_TRUE(box.Intersects(sphere));
}


TEST(TestMath_BoundingBox, Intersects_Sphere_Disjoint)
{
  const BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
  const BoundingSphere sphere(Vector3(16.0f, 5.0f, 5.0f), 5.0f);

  EXPECT_FALSE(box.Intersects(sphere));
}


TEST(TestMath_BoundingBox, Intersects_Ray)
{
  const BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
  Ray insideRay(Vector3(5.0f, 5.0f, 5.0f), Vector3(0.0f, 1.0f, 0.0f));
  Ray outsideRay(Vector3(5.0f, -5.0f, 5.0f), Vector3(0.0f, 1.0f, 0.0f));
  Ray missingRay(Vector3(5.0f, 15.0f, 5.0f), Vector3(0.0f, 1.0f, 0.0f));

  float result;
  EXPECT_TRUE(box.Intersects(insideRay, result));
  EXPECT_FLOAT_EQ(0.0f, result);
  EXPECT_TRUE(box.Intersects(outsideRay, result));
  EXPECT_FLOAT_EQ(5.0f, result);

  EXPECT_FALSE(box.Intersects(missingRay, result));
}
