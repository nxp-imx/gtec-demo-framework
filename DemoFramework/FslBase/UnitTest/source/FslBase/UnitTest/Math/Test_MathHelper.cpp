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

#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMath_MathHelper = TestFixtureFslBase;
}


TEST(TestMath_MathHelper, Barycentric)
{
  EXPECT_FLOAT_EQ(1.0f, MathHelper::Barycentric(1.0f, 2.0f, 3.0f, 0.0f, 0.0f));
  EXPECT_FLOAT_EQ(2.0f, MathHelper::Barycentric(1.0f, 2.0f, 3.0f, 1.0f, 0.0f));
  EXPECT_FLOAT_EQ(3.0f, MathHelper::Barycentric(1.0f, 2.0f, 3.0f, 0.0f, 1.0f));
  EXPECT_FLOAT_EQ(2.5f, MathHelper::Barycentric(1.0f, 2.0f, 3.0f, 0.5f, 0.5f));
  EXPECT_FLOAT_EQ(4.0f, MathHelper::Barycentric(1.0f, 2.0f, 3.0f, 1.0f, 1.0f));

  EXPECT_FLOAT_EQ(-10.0f, MathHelper::Barycentric(-10.0f, 10.0f, 40.0f, 0.0f, 0.0f));
  EXPECT_FLOAT_EQ(10.0f, MathHelper::Barycentric(-10.0f, 10.0f, 40.0f, 1.0f, 0.0f));
  EXPECT_FLOAT_EQ(40.0f, MathHelper::Barycentric(-10.0f, 10.0f, 40.0f, 0.0f, 1.0f));
  EXPECT_FLOAT_EQ(25.0f, MathHelper::Barycentric(-10.0f, 10.0f, 40.0f, 0.5f, 0.5f));
  EXPECT_FLOAT_EQ(60.0f, MathHelper::Barycentric(-10.0f, 10.0f, 40.0f, 1.0f, 1.0f));
}


TEST(TestMath_MathHelper, DistBetweenAngles)
{
  EXPECT_FLOAT_EQ(MathHelper::RADS45, MathHelper::DistBetweenAngles(MathHelper::RADS0, MathHelper::RADS45));
  EXPECT_FLOAT_EQ(-MathHelper::RADS45, MathHelper::DistBetweenAngles(MathHelper::RADS45, MathHelper::RADS0));
}


TEST(TestMath_MathHelper, Hermite)
{
  EXPECT_EQ(1.0f, MathHelper::Hermite(1.0f, 2.0f, 3.0f, 4.0f, 0.0f));
  EXPECT_EQ(3.0f, MathHelper::Hermite(1.0f, 2.0f, 3.0f, 4.0f, 1.0f));
  EXPECT_FLOAT_EQ(1.75f, MathHelper::Hermite(1.0f, 2.0f, 3.0f, 4.0f, 0.5f));
}


TEST(TestMath_MathHelper, SmoothStep)
{
  EXPECT_EQ(10.0f, MathHelper::SmoothStep(10.0f, 20.0f, 0.0f));
  EXPECT_EQ(20.0f, MathHelper::SmoothStep(10.0f, 20.0f, 1.0f));
  EXPECT_FLOAT_EQ(15.0f, MathHelper::SmoothStep(10.0f, 20.0f, 0.5f));

  EXPECT_EQ(10.0f, MathHelper::SmoothStep(10.0f, 20.0f, -10.0f));
  EXPECT_EQ(20.0f, MathHelper::SmoothStep(10.0f, 20.0f, 10.0f));
}


TEST(TestMath_MathHelper, ToPowerOfTwo)
{
  EXPECT_EQ(1, MathHelper::ToPowerOfTwo(0u));
  EXPECT_EQ(1, MathHelper::ToPowerOfTwo(1u));
  EXPECT_EQ(2, MathHelper::ToPowerOfTwo(2u));
  EXPECT_EQ(4, MathHelper::ToPowerOfTwo(3u));
  EXPECT_EQ(4, MathHelper::ToPowerOfTwo(4u));
  EXPECT_EQ(8, MathHelper::ToPowerOfTwo(5u));
  EXPECT_EQ(8, MathHelper::ToPowerOfTwo(6u));
  EXPECT_EQ(8, MathHelper::ToPowerOfTwo(7u));
  EXPECT_EQ(8, MathHelper::ToPowerOfTwo(8u));
  EXPECT_EQ(16, MathHelper::ToPowerOfTwo(9u));

  EXPECT_EQ(32, MathHelper::ToPowerOfTwo(31u));
  EXPECT_EQ(32, MathHelper::ToPowerOfTwo(32u));
  EXPECT_EQ(64, MathHelper::ToPowerOfTwo(33u));

  EXPECT_EQ(128, MathHelper::ToPowerOfTwo(127u));
  EXPECT_EQ(128, MathHelper::ToPowerOfTwo(128u));
  EXPECT_EQ(256, MathHelper::ToPowerOfTwo(129u));
}


TEST(TestMath_MathHelper, WrapAngle)
{
  // EXPECT_FLOAT_EQ((MathHelper::RADS0 - MathHelper::RADS45), MathHelper::WrapAngle((MathHelper::RADS360 * 2.0f) - MathHelper::RADS45));
  EXPECT_FLOAT_EQ((MathHelper::RADS0 - MathHelper::RADS90), MathHelper::WrapAngle((MathHelper::RADS360 * 2.0f) - MathHelper::RADS90));
  EXPECT_FLOAT_EQ((MathHelper::RADS0 - MathHelper::RADS135), MathHelper::WrapAngle((MathHelper::RADS360 * 2.0f) - MathHelper::RADS135));
  EXPECT_FLOAT_EQ((MathHelper::RADS360 - MathHelper::RADS180), MathHelper::WrapAngle((MathHelper::RADS360 * 2.0f) - MathHelper::RADS180));
  EXPECT_FLOAT_EQ((MathHelper::RADS360 - MathHelper::RADS225), MathHelper::WrapAngle((MathHelper::RADS360 * 2.0f) - MathHelper::RADS225));
  EXPECT_FLOAT_EQ((MathHelper::RADS360 - MathHelper::RADS270), MathHelper::WrapAngle((MathHelper::RADS360 * 2.0f) - MathHelper::RADS270));
  EXPECT_FLOAT_EQ((MathHelper::RADS360 - MathHelper::RADS315), MathHelper::WrapAngle((MathHelper::RADS360 * 2.0f) - MathHelper::RADS315));
  EXPECT_FLOAT_EQ((MathHelper::RADS360 - MathHelper::RADS360), MathHelper::WrapAngle((MathHelper::RADS360 * 2.0f) - MathHelper::RADS360));
}


TEST(TestMath_MathHelper, WrapAngle_negative)
{
  // EXPECT_FLOAT_EQ((MathHelper::RADS0 - MathHelper::RADS45), MathHelper::WrapAngle((-MathHelper::RADS360 * 2.0f) - MathHelper::RADS45));
  EXPECT_FLOAT_EQ((MathHelper::RADS0 - MathHelper::RADS90), MathHelper::WrapAngle((-MathHelper::RADS360 * 2.0f) - MathHelper::RADS90));
  EXPECT_FLOAT_EQ((MathHelper::RADS0 - MathHelper::RADS135), MathHelper::WrapAngle((-MathHelper::RADS360 * 2.0f) - MathHelper::RADS135));
  EXPECT_FLOAT_EQ((MathHelper::RADS360 - MathHelper::RADS180), MathHelper::WrapAngle((-MathHelper::RADS360 * 2.0f) - MathHelper::RADS180));
  EXPECT_FLOAT_EQ((MathHelper::RADS360 - MathHelper::RADS225), MathHelper::WrapAngle((-MathHelper::RADS360 * 2.0f) - MathHelper::RADS225));
  EXPECT_FLOAT_EQ(1.5707974f, MathHelper::WrapAngle((-MathHelper::RADS360 * 2.0f) - MathHelper::RADS270));
  EXPECT_FLOAT_EQ(0.78539896f, MathHelper::WrapAngle((-MathHelper::RADS360 * 2.0f) - MathHelper::RADS315));
  // EXPECT_FLOAT_EQ((MathHelper::RADS360 - MathHelper::RADS360), MathHelper::WrapAngle((-MathHelper::RADS360 * 2.0f) - MathHelper::RADS360));
}


TEST(TestMath_MathHelper, CalcOptimalSize)
{
  EXPECT_EQ(Point2(10 * 20, 10), MathHelper::CalcOptimalSize(Point2(10, 10), 20, RectangleSizeRestrictionFlag::NoRestrictions));

  EXPECT_EQ(Point2(64, 64), MathHelper::CalcOptimalSize(Point2(10, 10), 20, RectangleSizeRestrictionFlag::Power2));
  EXPECT_EQ(Point2(50, 50), MathHelper::CalcOptimalSize(Point2(10, 10), 20, RectangleSizeRestrictionFlag::Square));

  EXPECT_EQ(Point2(64, 32), MathHelper::CalcOptimalSize(Point2(5, 10), 20, RectangleSizeRestrictionFlag::Power2));
  EXPECT_EQ(Point2(64, 32), MathHelper::CalcOptimalSize(Point2(10, 5), 20, RectangleSizeRestrictionFlag::Power2));
  // 35 / 5 = 7     35 / 10 = 3
  // 7*3 = 21
  EXPECT_EQ(Point2(35, 35), MathHelper::CalcOptimalSize(Point2(5, 10), 20, RectangleSizeRestrictionFlag::Square));
  EXPECT_EQ(Point2(35, 35), MathHelper::CalcOptimalSize(Point2(10, 5), 20, RectangleSizeRestrictionFlag::Square));

  EXPECT_EQ(Point2(64, 64), MathHelper::CalcOptimalSize(Point2(5, 10), 20,
                                                        static_cast<RectangleSizeRestrictionFlag::Enum>(RectangleSizeRestrictionFlag::Power2 |
                                                                                                        RectangleSizeRestrictionFlag::Square)));
  EXPECT_EQ(Point2(64, 64), MathHelper::CalcOptimalSize(Point2(10, 5), 20,
                                                        static_cast<RectangleSizeRestrictionFlag::Enum>(RectangleSizeRestrictionFlag::Power2 |
                                                                                                        RectangleSizeRestrictionFlag::Square)));
}

TEST(TestMath_MathHelper, CalcOptimalSize_Pow2)
{
  const auto restrictions = RectangleSizeRestrictionFlag::Power2;
  EXPECT_EQ(Point2(1, 1), MathHelper::CalcOptimalSize(Point2(1, 1), 1, restrictions));
  EXPECT_EQ(Point2(2, 1), MathHelper::CalcOptimalSize(Point2(1, 1), 2, restrictions));
  EXPECT_EQ(Point2(2, 2), MathHelper::CalcOptimalSize(Point2(1, 1), 3, restrictions));
  EXPECT_EQ(Point2(2, 2), MathHelper::CalcOptimalSize(Point2(1, 1), 4, restrictions));
  EXPECT_EQ(Point2(4, 2), MathHelper::CalcOptimalSize(Point2(1, 1), 5, restrictions));
  EXPECT_EQ(Point2(4, 2), MathHelper::CalcOptimalSize(Point2(1, 1), 6, restrictions));
  EXPECT_EQ(Point2(4, 2), MathHelper::CalcOptimalSize(Point2(1, 1), 7, restrictions));
  EXPECT_EQ(Point2(4, 2), MathHelper::CalcOptimalSize(Point2(1, 1), 8, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 9, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 10, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 11, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 12, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 13, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 14, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 15, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 16, restrictions));
}

TEST(TestMath_MathHelper, CalcOptimalSize_Square)
{
  const auto restrictions = RectangleSizeRestrictionFlag::Square;
  EXPECT_EQ(Point2(1, 1), MathHelper::CalcOptimalSize(Point2(1, 1), 1, restrictions));
  EXPECT_EQ(Point2(2, 2), MathHelper::CalcOptimalSize(Point2(1, 1), 2, restrictions));
  EXPECT_EQ(Point2(2, 2), MathHelper::CalcOptimalSize(Point2(1, 1), 3, restrictions));
  EXPECT_EQ(Point2(2, 2), MathHelper::CalcOptimalSize(Point2(1, 1), 4, restrictions));
  EXPECT_EQ(Point2(3, 3), MathHelper::CalcOptimalSize(Point2(1, 1), 5, restrictions));
  EXPECT_EQ(Point2(3, 3), MathHelper::CalcOptimalSize(Point2(1, 1), 6, restrictions));
  EXPECT_EQ(Point2(3, 3), MathHelper::CalcOptimalSize(Point2(1, 1), 7, restrictions));
  EXPECT_EQ(Point2(3, 3), MathHelper::CalcOptimalSize(Point2(1, 1), 8, restrictions));
  EXPECT_EQ(Point2(3, 3), MathHelper::CalcOptimalSize(Point2(1, 1), 9, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 10, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 11, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 12, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 13, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 14, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 15, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 16, restrictions));
}


TEST(TestMath_MathHelper, CalcOptimalSize_SquarePow2)
{
  const auto restrictions =
    static_cast<RectangleSizeRestrictionFlag::Enum>(RectangleSizeRestrictionFlag::Power2 | RectangleSizeRestrictionFlag::Square);

  EXPECT_EQ(Point2(1, 1), MathHelper::CalcOptimalSize(Point2(1, 1), 1, restrictions));
  EXPECT_EQ(Point2(2, 2), MathHelper::CalcOptimalSize(Point2(1, 1), 2, restrictions));
  EXPECT_EQ(Point2(2, 2), MathHelper::CalcOptimalSize(Point2(1, 1), 3, restrictions));
  EXPECT_EQ(Point2(2, 2), MathHelper::CalcOptimalSize(Point2(1, 1), 4, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 5, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 6, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 7, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 8, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 9, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 10, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 11, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 12, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 13, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 14, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 15, restrictions));
  EXPECT_EQ(Point2(4, 4), MathHelper::CalcOptimalSize(Point2(1, 1), 16, restrictions));
}
