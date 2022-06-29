/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/Log/Math/Pixel/LogPxPoint2.hpp>
#include <FslBase/Math/MinMax.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <limits>

using namespace Fsl;

namespace
{
  using TestMath_MinMax = TestFixtureFslBase;
}


TEST(TestMath_MinMax, Construct_Default)
{
  MinMax<int32_t> value;

  EXPECT_EQ(0, value.Min());
  EXPECT_EQ(0, value.Max());
  EXPECT_EQ(0, value.Center());
}


TEST(TestMath_MinMax, Construct_MinMax)
{
  const int32_t min = 10;
  const int32_t max = 15;
  MinMax<int32_t> value(min, max);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(((max - min) / 2) + min, value.Center());
}

TEST(TestMath_MinMax, Construct_MinMax_Invalid)
{
  const int32_t min = 10;
  const int32_t max = 15;
  MinMax<int32_t> value(max, min);

  EXPECT_EQ(max, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(max, value.Center());
}


TEST(TestMath_MinMax, Construct_ValueMinMax)
{
  const int32_t min = 10;
  const int32_t max = 15;
  MinMax<int32_t> value(min, max);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
}

TEST(TestMath_MinMax, Set)
{
  MinMax<int32_t> value;

  EXPECT_EQ(0, value.Min());
  EXPECT_EQ(0, value.Max());

  const int32_t min = 10;
  const int32_t max = 15;
  EXPECT_TRUE(value.Set(min, max));

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());

  EXPECT_FALSE(value.Set(min, max));
}

TEST(TestMath_MinMax, Set_Invalid)
{
  MinMax<int32_t> value;

  EXPECT_EQ(0, value.Min());
  EXPECT_EQ(0, value.Max());

  const int32_t min = 10;
  const int32_t max = 15;
  EXPECT_TRUE(value.Set(max, min));

  EXPECT_EQ(max, value.Min());
  EXPECT_EQ(max, value.Max());

  EXPECT_FALSE(value.Set(max, min));
}

TEST(TestMath_MinMax, OpEqual)
{
  EXPECT_TRUE(MinMax<int32_t>(2, 3) == MinMax<int32_t>(2, 3));
  EXPECT_FALSE(MinMax<int32_t>(2, 3) == MinMax<int32_t>(4, 3));
  EXPECT_FALSE(MinMax<int32_t>(2, 3) == MinMax<int32_t>(2, 4));
}


TEST(TestMath_MinMax, OpNotEqual)
{
  EXPECT_FALSE(MinMax<int32_t>(2, 3) != MinMax<int32_t>(2, 3));
  EXPECT_TRUE(MinMax<int32_t>(2, 3) != MinMax<int32_t>(4, 3));
  EXPECT_TRUE(MinMax<int32_t>(2, 3) != MinMax<int32_t>(2, 4));
}
