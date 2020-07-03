/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslBase/Math/MathHelper_CalcOptimalSize.hpp>
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMath_MathHelper_CalcOptimalSize = TestFixtureFslBase;
}


TEST(TestMath_MathHelper_CalcOptimalSize, CalcOptimalSize)
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

  EXPECT_EQ(Point2(64, 64),
            MathHelper::CalcOptimalSize(Point2(5, 10), 20, RectangleSizeRestrictionFlag::Power2 | RectangleSizeRestrictionFlag::Square));
  EXPECT_EQ(Point2(64, 64),
            MathHelper::CalcOptimalSize(Point2(10, 5), 20, RectangleSizeRestrictionFlag::Power2 | RectangleSizeRestrictionFlag::Square));
}

TEST(TestMath_MathHelper_CalcOptimalSize, CalcOptimalSize_Pow2)
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

TEST(TestMath_MathHelper_CalcOptimalSize, CalcOptimalSize_Square)
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


TEST(TestMath_MathHelper_CalcOptimalSize, CalcOptimalSize_SquarePow2)
{
  const auto restrictions = RectangleSizeRestrictionFlag::Power2 | RectangleSizeRestrictionFlag::Square;

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
