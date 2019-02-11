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
#include <FslBase/Log/Math/LogBoundingBox.hpp>
#include <FslBase/Log/Math/LogBoundingSphere.hpp>
#include <FslBase/Log/Math/LogContainmentType.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslBase/UnitTest/Helper/Test/Math/TestBoundingSphere.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_BoundingSphere = TestFixtureFslBase;
}


TEST(TestMath_BoundingSphere, Empty)
{
  Vector3 position(1.0f, 2.0f, 3.0f);
  float radius = 10.0f;
  BoundingSphere sphere(position, radius);

  EXPECT_EQ(position, sphere.Center);
  EXPECT_EQ(radius, sphere.Radius);
}


// TEST(TestMath_BoundingSphere, Transform)
//{
//}
//
//
// TEST(TestMath_BoundingSphere, Transform_Ref)
//{
//}


TEST(TestMath_BoundingSphere, Contains_BoundingBox)
{
  const BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(5.0f, 5.0f, 5.0f));
  const BoundingSphere sphere1(Vector3(0.0f, 0.0f, 0.0f), 2.5f);
  const BoundingSphere sphere2(Vector3(2.5f, 2.5f, 2.5f), 2.5f);
  const BoundingSphere sphere3(Vector3(5.0f, 5.0f, 5.0f), 2.5f);
  const BoundingSphere sphere4(Vector3(2.5f, 2.5f, 2.5f), 2.0f);
  const BoundingSphere sphere5(Vector3(8.0f, 0.0f, 0.0f), 2.5f);
  const BoundingSphere sphere6(Vector3(0.0f, 8.0f, 0.0f), 2.5f);
  const BoundingSphere sphere7(Vector3(0.0f, 0.0f, 8.0f), 2.5f);
  const BoundingSphere sphere8(Vector3(2.5f, 2.5f, 2.5f), 10.0f);

  EXPECT_EQ(ContainmentType::Intersects, sphere1.Contains(box)) << "sphere1: " << sphere1 << " box: " << box;
  EXPECT_EQ(ContainmentType::Intersects, sphere2.Contains(box)) << "sphere2: " << sphere2 << " box: " << box;
  EXPECT_EQ(ContainmentType::Intersects, sphere3.Contains(box)) << "sphere3: " << sphere3 << " box: " << box;
  EXPECT_EQ(ContainmentType::Intersects, sphere4.Contains(box)) << "sphere4: " << sphere4 << " box: " << box;
  EXPECT_EQ(ContainmentType::Disjoint, sphere5.Contains(box)) << "sphere5: " << sphere5 << " box: " << box;
  EXPECT_EQ(ContainmentType::Disjoint, sphere6.Contains(box)) << "sphere6: " << sphere6 << " box: " << box;
  EXPECT_EQ(ContainmentType::Disjoint, sphere7.Contains(box)) << "sphere7: " << sphere7 << " box: " << box;
  EXPECT_EQ(ContainmentType::Contains, sphere8.Contains(box)) << "sphere8: " << sphere8 << " box: " << box;
}


// TEST(TestMath_BoundingSphere, Contains_BoundingFrustum)
//{
//}


TEST(TestMath_BoundingSphere, Contains_BoundingSphere)
{
  const BoundingSphere sphere1(Vector3(0.0f, 0.0f, 0.0f), 2.5f);
  const BoundingSphere sphere2(Vector3(2.5f, 2.5f, 2.5f), 2.5f);
  const BoundingSphere sphere3(Vector3(5.0f, 5.0f, 5.0f), 2.5f);
  const BoundingSphere sphere4(Vector3(2.5f, 2.5f, 2.5f), 2.0f);
  const BoundingSphere sphere5(Vector3(8.0f, 0.0f, 0.0f), 2.5f);
  const BoundingSphere sphere6(Vector3(0.0f, 8.0f, 0.0f), 2.5f);
  const BoundingSphere sphere7(Vector3(0.0f, 0.0f, 8.0f), 2.5f);

  EXPECT_EQ(ContainmentType::Contains, sphere1.Contains(sphere1)) << "Sphere1: " << sphere1;
  EXPECT_EQ(ContainmentType::Intersects, sphere1.Contains(sphere2)) << "Sphere1: " << sphere1 << " Sphere2: " << sphere2;
  EXPECT_EQ(ContainmentType::Disjoint, sphere1.Contains(sphere3)) << "Sphere1: " << sphere1 << " Sphere3: " << sphere3;
  EXPECT_EQ(ContainmentType::Intersects, sphere1.Contains(sphere4)) << "Sphere1: " << sphere1 << " Sphere4: " << sphere4;
  EXPECT_EQ(ContainmentType::Disjoint, sphere1.Contains(sphere5)) << "Sphere1: " << sphere1 << " Sphere5: " << sphere5;
  EXPECT_EQ(ContainmentType::Disjoint, sphere1.Contains(sphere6)) << "Sphere1: " << sphere1 << " Sphere6: " << sphere6;
  EXPECT_EQ(ContainmentType::Disjoint, sphere1.Contains(sphere7)) << "Sphere1: " << sphere1 << " Sphere7: " << sphere7;

  EXPECT_EQ(ContainmentType::Intersects, sphere2.Contains(sphere1)) << "Sphere2: " << sphere2 << " Sphere1: " << sphere1;
  EXPECT_EQ(ContainmentType::Contains, sphere2.Contains(sphere2)) << "Sphere2: " << sphere2;
  EXPECT_EQ(ContainmentType::Intersects, sphere2.Contains(sphere3)) << "Sphere2: " << sphere2 << " Sphere3: " << sphere3;
  EXPECT_EQ(ContainmentType::Contains, sphere2.Contains(sphere4)) << "Sphere2: " << sphere2 << " Sphere4: " << sphere4;
  EXPECT_EQ(ContainmentType::Disjoint, sphere2.Contains(sphere5)) << "Sphere2: " << sphere2 << " Sphere5: " << sphere5;
  EXPECT_EQ(ContainmentType::Disjoint, sphere2.Contains(sphere6)) << "Sphere2: " << sphere2 << " Sphere6: " << sphere6;
  EXPECT_EQ(ContainmentType::Disjoint, sphere2.Contains(sphere7)) << "Sphere2: " << sphere2 << " Sphere7: " << sphere7;

  EXPECT_EQ(ContainmentType::Disjoint, sphere3.Contains(sphere1)) << "Sphere3: " << sphere3 << " Sphere1: " << sphere1;
  EXPECT_EQ(ContainmentType::Intersects, sphere3.Contains(sphere2)) << "Sphere3: " << sphere3 << " Sphere2: " << sphere2;
  EXPECT_EQ(ContainmentType::Contains, sphere3.Contains(sphere3)) << "Sphere3: " << sphere3;
  EXPECT_EQ(ContainmentType::Intersects, sphere3.Contains(sphere4)) << "Sphere3: " << sphere3 << " Sphere4: " << sphere4;
  EXPECT_EQ(ContainmentType::Disjoint, sphere3.Contains(sphere5)) << "Sphere3: " << sphere3 << " Sphere5: " << sphere5;
  EXPECT_EQ(ContainmentType::Disjoint, sphere3.Contains(sphere6)) << "Sphere3: " << sphere3 << " Sphere6: " << sphere6;
  EXPECT_EQ(ContainmentType::Disjoint, sphere3.Contains(sphere7)) << "Sphere3: " << sphere3 << " Sphere7: " << sphere7;

  EXPECT_EQ(ContainmentType::Intersects, sphere4.Contains(sphere1)) << "Sphere4: " << sphere4 << " Sphere1: " << sphere1;
  EXPECT_EQ(ContainmentType::Intersects, sphere4.Contains(sphere2)) << "Sphere4: " << sphere4 << " Sphere2: " << sphere2;
  EXPECT_EQ(ContainmentType::Intersects, sphere4.Contains(sphere3)) << "Sphere4: " << sphere4 << " Sphere3: " << sphere3;
  EXPECT_EQ(ContainmentType::Contains, sphere4.Contains(sphere4)) << "Sphere4: " << sphere4;
  EXPECT_EQ(ContainmentType::Disjoint, sphere4.Contains(sphere5)) << "Sphere4: " << sphere4 << " Sphere5: " << sphere5;
  EXPECT_EQ(ContainmentType::Disjoint, sphere4.Contains(sphere6)) << "Sphere4: " << sphere4 << " Sphere6: " << sphere6;
  EXPECT_EQ(ContainmentType::Disjoint, sphere4.Contains(sphere7)) << "Sphere4: " << sphere4 << " Sphere7: " << sphere7;

  EXPECT_EQ(ContainmentType::Disjoint, sphere5.Contains(sphere1)) << "Sphere5: " << sphere5 << " Sphere1: " << sphere1;
  EXPECT_EQ(ContainmentType::Disjoint, sphere5.Contains(sphere2)) << "Sphere5: " << sphere5 << " Sphere2: " << sphere2;
  EXPECT_EQ(ContainmentType::Disjoint, sphere5.Contains(sphere3)) << "Sphere5: " << sphere5 << " Sphere3: " << sphere3;
  EXPECT_EQ(ContainmentType::Disjoint, sphere5.Contains(sphere4)) << "Sphere5: " << sphere5 << " Sphere4: " << sphere4;
  EXPECT_EQ(ContainmentType::Contains, sphere5.Contains(sphere5)) << "Sphere5: " << sphere5;
  EXPECT_EQ(ContainmentType::Disjoint, sphere5.Contains(sphere6)) << "Sphere5: " << sphere5 << " Sphere6: " << sphere6;
  EXPECT_EQ(ContainmentType::Disjoint, sphere5.Contains(sphere7)) << "Sphere5: " << sphere5 << " Sphere7: " << sphere7;

  EXPECT_EQ(ContainmentType::Contains, sphere6.Contains(sphere6)) << "Sphere6: " << sphere6;
  EXPECT_EQ(ContainmentType::Contains, sphere7.Contains(sphere7)) << "Sphere7: " << sphere7;
}


TEST(TestMath_BoundingSphere, Contains_Point)
{
  EXPECT_EQ(ContainmentType::Contains, BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 2.5f).Contains(Vector3(0.0f, 0.0f, 0.0f)));
  EXPECT_EQ(ContainmentType::Intersects, BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 2.5f).Contains(Vector3(2.5f, 0.0f, 0.0f)));
  EXPECT_EQ(ContainmentType::Intersects, BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 2.5f).Contains(Vector3(0.0f, 2.5f, 0.0f)));
  EXPECT_EQ(ContainmentType::Intersects, BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 2.5f).Contains(Vector3(0.0f, 0.0f, 2.5f)));
  EXPECT_EQ(ContainmentType::Intersects, BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 2.5f).Contains(Vector3(-2.5f, 0.0f, 0.0f)));
  EXPECT_EQ(ContainmentType::Intersects, BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 2.5f).Contains(Vector3(0.0f, -2.5f, 0.0f)));
  EXPECT_EQ(ContainmentType::Intersects, BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 2.5f).Contains(Vector3(0.0f, 0.0f, -2.5f)));

  EXPECT_EQ(ContainmentType::Disjoint, BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 2.5f).Contains(Vector3(2.51f, 0.00f, 0.00f)));
  EXPECT_EQ(ContainmentType::Disjoint, BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 2.5f).Contains(Vector3(0.00f, 2.51f, 0.00f)));
  EXPECT_EQ(ContainmentType::Disjoint, BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 2.5f).Contains(Vector3(0.00f, 0.00f, 2.51f)));
  EXPECT_EQ(ContainmentType::Disjoint, BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 2.5f).Contains(Vector3(-2.51f, 0.00f, 0.00f)));
  EXPECT_EQ(ContainmentType::Disjoint, BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 2.5f).Contains(Vector3(0.00f, -2.51f, 0.00f)));
  EXPECT_EQ(ContainmentType::Disjoint, BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 2.5f).Contains(Vector3(0.00f, 0.00f, -2.51f)));
}


TEST(TestMath_BoundingSphere, CreateFromBoundingBox)
{
  const BoundingBox box1(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f));
  const BoundingBox box2(Vector3(-5.0f, -5.0f, -5.0f), Vector3(5.0f, 5.0f, 5.0f));
  constexpr float tolerance = 0.000001f;

  // Not a very accurate check but better than nothing for now
  EXPECT_TRUE(Fsl::Test::IsFloatNear(BoundingSphere(Vector3(0, 0, 0), 0.866025f), BoundingSphere::CreateFromBoundingBox(box1), tolerance))
    << "box1: " << box1;
  EXPECT_TRUE(Fsl::Test::IsFloatNear(BoundingSphere(Vector3(0, 0, 0), 8.660254f), BoundingSphere::CreateFromBoundingBox(box2), tolerance))
    << "box2: " << box2;
}


TEST(TestMath_BoundingSphere, CreateFromBoundingBox_Ref)
{
  const BoundingBox box1(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f));
  const BoundingBox box2(Vector3(-5.0f, -5.0f, -5.0f), Vector3(5.0f, 5.0f, 5.0f));
  constexpr float tolerance = 0.000001f;

  BoundingSphere res1, res2;
  BoundingSphere::CreateFromBoundingBox(box1, res1);
  BoundingSphere::CreateFromBoundingBox(box2, res2);

  // Not a very accurate check but better than nothing for now
  EXPECT_TRUE(Fsl::Test::IsFloatNear(BoundingSphere(Vector3(0, 0, 0), 0.866025f), res1, tolerance)) << "box1: " << box1;
  EXPECT_TRUE(Fsl::Test::IsFloatNear(BoundingSphere(Vector3(0, 0, 0), 8.660254f), res2, tolerance)) << "box2: " << box2;
}


// TEST(TestMath_BoundingSphere, CreateFromPoints)
//{
//
//}


// TEST(TestMath_BoundingSphere, CreateMerged)
//{
//}
//
//
// TEST(TestMath_BoundingSphere, CreateMerged_Ref)
//{
//}


TEST(TestMath_BoundingSphere, Intersects_BoundingBox)
{
  const BoundingBox box(Vector3(0.0f, 0.0f, 0.0f), Vector3(5.0f, 5.0f, 5.0f));
  const BoundingSphere sphere1(Vector3(0.0f, 0.0f, 0.0f), 2.5f);
  const BoundingSphere sphere2(Vector3(2.5f, 2.5f, 2.5f), 2.5f);
  const BoundingSphere sphere3(Vector3(5.0f, 5.0f, 5.0f), 2.5f);
  const BoundingSphere sphere4(Vector3(2.5f, 2.5f, 2.5f), 2.0f);
  const BoundingSphere sphere5(Vector3(8.0f, 0.0f, 0.0f), 2.5f);
  const BoundingSphere sphere6(Vector3(0.0f, 8.0f, 0.0f), 2.5f);
  const BoundingSphere sphere7(Vector3(0.0f, 0.0f, 8.0f), 2.5f);
  const BoundingSphere sphere8(Vector3(2.5f, 2.5f, 2.5f), 10.0f);

  EXPECT_TRUE(sphere1.Intersects(box)) << "sphere1: " << sphere1 << " box: " << box;
  EXPECT_TRUE(sphere2.Intersects(box)) << "sphere2: " << sphere2 << " box: " << box;
  EXPECT_TRUE(sphere3.Intersects(box)) << "sphere3: " << sphere3 << " box: " << box;
  EXPECT_TRUE(sphere4.Intersects(box)) << "sphere4: " << sphere4 << " box: " << box;
  EXPECT_FALSE(sphere5.Intersects(box)) << "sphere5: " << sphere5 << " box: " << box;
  EXPECT_FALSE(sphere6.Intersects(box)) << "sphere6: " << sphere6 << " box: " << box;
  EXPECT_FALSE(sphere7.Intersects(box)) << "sphere7: " << sphere7 << " box: " << box;
  EXPECT_TRUE(sphere8.Intersects(box)) << "sphere8: " << sphere8 << " box: " << box;
}


TEST(TestMath_BoundingSphere, Intersects_BoundingSphere)
{
  const BoundingSphere sphere1(Vector3(0.0f, 0.0f, 0.0f), 2.5f);
  const BoundingSphere sphere2(Vector3(2.5f, 2.5f, 2.5f), 2.5f);
  const BoundingSphere sphere3(Vector3(5.0f, 5.0f, 5.0f), 2.5f);
  const BoundingSphere sphere4(Vector3(2.5f, 2.5f, 2.5f), 2.0f);
  const BoundingSphere sphere5(Vector3(8.0f, 0.0f, 0.0f), 2.5f);
  const BoundingSphere sphere6(Vector3(0.0f, 8.0f, 0.0f), 2.5f);
  const BoundingSphere sphere7(Vector3(0.0f, 0.0f, 8.0f), 2.5f);

  EXPECT_TRUE(sphere1.Intersects(sphere1)) << "Sphere1: " << sphere1;
  EXPECT_TRUE(sphere1.Intersects(sphere2)) << "Sphere1: " << sphere1 << " Sphere2: " << sphere2;
  EXPECT_FALSE(sphere1.Intersects(sphere3)) << "Sphere1: " << sphere1 << " Sphere3: " << sphere3;
  EXPECT_TRUE(sphere1.Intersects(sphere4)) << "Sphere1: " << sphere1 << " Sphere4: " << sphere4;
  EXPECT_FALSE(sphere1.Intersects(sphere5)) << "Sphere1: " << sphere1 << " Sphere5: " << sphere5;
  EXPECT_FALSE(sphere1.Intersects(sphere6)) << "Sphere1: " << sphere1 << " Sphere6: " << sphere6;
  EXPECT_FALSE(sphere1.Intersects(sphere7)) << "Sphere1: " << sphere1 << " Sphere7: " << sphere7;

  EXPECT_TRUE(sphere2.Intersects(sphere1)) << "Sphere2: " << sphere2 << " Sphere1: " << sphere1;
  EXPECT_TRUE(sphere2.Intersects(sphere2)) << "Sphere2: " << sphere2;
  EXPECT_TRUE(sphere2.Intersects(sphere3)) << "Sphere2: " << sphere2 << " Sphere3: " << sphere3;
  EXPECT_TRUE(sphere2.Intersects(sphere4)) << "Sphere2: " << sphere2 << " Sphere4: " << sphere4;
  EXPECT_FALSE(sphere2.Intersects(sphere5)) << "Sphere2: " << sphere2 << " Sphere5: " << sphere5;
  EXPECT_FALSE(sphere2.Intersects(sphere6)) << "Sphere2: " << sphere2 << " Sphere6: " << sphere6;
  EXPECT_FALSE(sphere2.Intersects(sphere7)) << "Sphere2: " << sphere2 << " Sphere7: " << sphere7;

  EXPECT_FALSE(sphere3.Intersects(sphere1)) << "Sphere3: " << sphere3 << " Sphere1: " << sphere1;
  EXPECT_TRUE(sphere3.Intersects(sphere2)) << "Sphere3: " << sphere3 << " Sphere2: " << sphere2;
  EXPECT_TRUE(sphere3.Intersects(sphere3)) << "Sphere3: " << sphere3;
  EXPECT_TRUE(sphere3.Intersects(sphere4)) << "Sphere3: " << sphere3 << " Sphere4: " << sphere4;
  EXPECT_FALSE(sphere3.Intersects(sphere5)) << "Sphere3: " << sphere3 << " Sphere5: " << sphere5;
  EXPECT_FALSE(sphere3.Intersects(sphere6)) << "Sphere3: " << sphere3 << " Sphere6: " << sphere6;
  EXPECT_FALSE(sphere3.Intersects(sphere7)) << "Sphere3: " << sphere3 << " Sphere7: " << sphere7;

  EXPECT_TRUE(sphere4.Intersects(sphere1)) << "Sphere4: " << sphere4 << " Sphere1: " << sphere1;
  EXPECT_TRUE(sphere4.Intersects(sphere2)) << "Sphere4: " << sphere4 << " Sphere2: " << sphere2;
  EXPECT_TRUE(sphere4.Intersects(sphere3)) << "Sphere4: " << sphere4 << " Sphere3: " << sphere3;
  EXPECT_TRUE(sphere4.Intersects(sphere4)) << "Sphere4: " << sphere4;
  EXPECT_FALSE(sphere4.Intersects(sphere5)) << "Sphere4: " << sphere4 << " Sphere5: " << sphere5;
  EXPECT_FALSE(sphere4.Intersects(sphere6)) << "Sphere4: " << sphere4 << " Sphere6: " << sphere6;
  EXPECT_FALSE(sphere4.Intersects(sphere7)) << "Sphere4: " << sphere4 << " Sphere7: " << sphere7;

  EXPECT_FALSE(sphere5.Intersects(sphere1)) << "Sphere5: " << sphere5 << " Sphere1: " << sphere1;
  EXPECT_FALSE(sphere5.Intersects(sphere2)) << "Sphere5: " << sphere5 << " Sphere2: " << sphere2;
  EXPECT_FALSE(sphere5.Intersects(sphere3)) << "Sphere5: " << sphere5 << " Sphere3: " << sphere3;
  EXPECT_FALSE(sphere5.Intersects(sphere4)) << "Sphere5: " << sphere5 << " Sphere4: " << sphere4;
  EXPECT_TRUE(sphere5.Intersects(sphere5)) << "Sphere5: " << sphere5;
  EXPECT_FALSE(sphere5.Intersects(sphere6)) << "Sphere5: " << sphere5 << " Sphere6: " << sphere6;
  EXPECT_FALSE(sphere5.Intersects(sphere7)) << "Sphere5: " << sphere5 << " Sphere7: " << sphere7;

  EXPECT_TRUE(sphere6.Intersects(sphere6)) << "Sphere6: " << sphere6;
  EXPECT_TRUE(sphere7.Intersects(sphere7)) << "Sphere7: " << sphere7;
}


// TEST(TestMath_BoundingSphere, Intersects_Ray)
//{
//}
