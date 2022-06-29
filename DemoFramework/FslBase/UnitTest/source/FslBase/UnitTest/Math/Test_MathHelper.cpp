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
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Math/MathHelper.hpp>
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


TEST(TestMath_MathHelper, ToPowerOfTwo_Int)
{
  EXPECT_EQ(1, MathHelper::ToPowerOfTwo(0));
  EXPECT_EQ(1, MathHelper::ToPowerOfTwo(1));
  EXPECT_EQ(2, MathHelper::ToPowerOfTwo(2));
  EXPECT_EQ(4, MathHelper::ToPowerOfTwo(3));
  EXPECT_EQ(4, MathHelper::ToPowerOfTwo(4));
  EXPECT_EQ(8, MathHelper::ToPowerOfTwo(5));
  EXPECT_EQ(8, MathHelper::ToPowerOfTwo(6));
  EXPECT_EQ(8, MathHelper::ToPowerOfTwo(7));
  EXPECT_EQ(8, MathHelper::ToPowerOfTwo(8));
  EXPECT_EQ(16, MathHelper::ToPowerOfTwo(9));

  EXPECT_EQ(32, MathHelper::ToPowerOfTwo(31));
  EXPECT_EQ(32, MathHelper::ToPowerOfTwo(32));
  EXPECT_EQ(64, MathHelper::ToPowerOfTwo(33));

  EXPECT_EQ(128, MathHelper::ToPowerOfTwo(127));
  EXPECT_EQ(128, MathHelper::ToPowerOfTwo(128));
  EXPECT_EQ(256, MathHelper::ToPowerOfTwo(129));
}

TEST(TestMath_MathHelper, ToPowerOfTwo_UInt)
{
  EXPECT_EQ(1u, MathHelper::ToPowerOfTwo(0u));
  EXPECT_EQ(1u, MathHelper::ToPowerOfTwo(1u));
  EXPECT_EQ(2u, MathHelper::ToPowerOfTwo(2u));
  EXPECT_EQ(4u, MathHelper::ToPowerOfTwo(3u));
  EXPECT_EQ(4u, MathHelper::ToPowerOfTwo(4u));
  EXPECT_EQ(8u, MathHelper::ToPowerOfTwo(5u));
  EXPECT_EQ(8u, MathHelper::ToPowerOfTwo(6u));
  EXPECT_EQ(8u, MathHelper::ToPowerOfTwo(7u));
  EXPECT_EQ(8u, MathHelper::ToPowerOfTwo(8u));
  EXPECT_EQ(16u, MathHelper::ToPowerOfTwo(9u));

  EXPECT_EQ(32u, MathHelper::ToPowerOfTwo(31u));
  EXPECT_EQ(32u, MathHelper::ToPowerOfTwo(32u));
  EXPECT_EQ(64u, MathHelper::ToPowerOfTwo(33u));

  EXPECT_EQ(128u, MathHelper::ToPowerOfTwo(127u));
  EXPECT_EQ(128u, MathHelper::ToPowerOfTwo(128u));
  EXPECT_EQ(256u, MathHelper::ToPowerOfTwo(129u));
}


TEST(TestMath_MathHelper, Wrap)
{
  EXPECT_FLOAT_EQ(-2.0f, MathHelper::Wrap(-2.0f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(0.0f, MathHelper::Wrap(0.0f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(-2.0f, MathHelper::Wrap(1.0f, -2.0f, 1.0f));

  EXPECT_FLOAT_EQ(0.0f, MathHelper::Wrap(-3.0f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(-0.5f, MathHelper::Wrap(-3.5f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(-1.0f, MathHelper::Wrap(-4.0f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(-1.5f, MathHelper::Wrap(-4.5f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(-2.0f, MathHelper::Wrap(-5.0f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(0.5f, MathHelper::Wrap(-5.5f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(0.0f, MathHelper::Wrap(-6.0f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(-0.5f, MathHelper::Wrap(-6.5f, -2.0f, 1.0f));

  EXPECT_FLOAT_EQ(0.0f, MathHelper::Wrap(3.0f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(0.5f, MathHelper::Wrap(3.5f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(-2.0f, MathHelper::Wrap(4.0f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(-1.5f, MathHelper::Wrap(4.5f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(-1.0f, MathHelper::Wrap(5.0f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(-0.5f, MathHelper::Wrap(5.5f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(0.0f, MathHelper::Wrap(6.0f, -2.0f, 1.0f));
  EXPECT_FLOAT_EQ(0.5f, MathHelper::Wrap(6.5f, -2.0f, 1.0f));
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
