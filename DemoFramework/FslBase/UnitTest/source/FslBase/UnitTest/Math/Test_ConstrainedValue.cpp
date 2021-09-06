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

#include <FslBase/Math/ConstrainedValue.hpp>
#include <FslBase/Log/Math/Pixel/LogPxPoint2.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <limits>

using namespace Fsl;

namespace
{
  using TestMath_ConstrainedValue = TestFixtureFslBase;
}


TEST(TestMath_ConstrainedValue, Construct_Default)
{
  ConstrainedValue<int32_t> value;

  EXPECT_EQ(0, value.Min());
  EXPECT_EQ(0, value.Max());
  EXPECT_EQ(0, value.Get());
  EXPECT_EQ(0, value.Center());
}


TEST(TestMath_ConstrainedValue, Construct_MinMax)
{
  const int32_t min = 10;
  const int32_t max = 15;
  ConstrainedValue<int32_t> value(min, max);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(min, value.Get());
  EXPECT_EQ(((max - min) / 2) + min, value.Center());
}

TEST(TestMath_ConstrainedValue, Construct_MinMax_Invalid)
{
  const int32_t min = 10;
  const int32_t max = 15;
  ConstrainedValue<int32_t> value(max, min);

  EXPECT_EQ(max, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(max, value.Get());
  EXPECT_EQ(max, value.Center());
}


TEST(TestMath_ConstrainedValue, Construct_ValueMinMax)
{
  const int32_t val = 12;
  const int32_t min = 10;
  const int32_t max = 15;
  ConstrainedValue<int32_t> value(val, min, max);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(val, value.Get());
}


TEST(TestMath_ConstrainedValue, Construct_ValueMinMax_InvalidMinMax)
{
  const int32_t val = 12;
  const int32_t min = 10;
  const int32_t max = 15;
  ConstrainedValue<int32_t> value(val, max, min);

  EXPECT_EQ(max, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(max, value.Get());
}


TEST(TestMath_ConstrainedValue, Construct_ValueMinMax_ValueOutOfBounds_Low)
{
  const int32_t val = 9;
  const int32_t min = 10;
  const int32_t max = 15;
  ConstrainedValue<int32_t> value(val, min, max);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(min, value.Get());
}


TEST(TestMath_ConstrainedValue, Construct_ValueMinMax_ValueOutOfBounds_High)
{
  const int32_t val = 16;
  const int32_t min = 10;
  const int32_t max = 15;
  ConstrainedValue<int32_t> value(val, min, max);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(max, value.Get());
}


TEST(TestMath_ConstrainedValue, SetRange)
{
  ConstrainedValue<int32_t> value;

  EXPECT_EQ(0, value.Min());
  EXPECT_EQ(0, value.Max());
  EXPECT_EQ(0, value.Get());

  const int32_t min = 10;
  const int32_t max = 15;
  EXPECT_TRUE(value.SetRange(min, max));

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(min, value.Get());

  EXPECT_FALSE(value.SetRange(min, max));
}

TEST(TestMath_ConstrainedValue, SetRange_Invalid)
{
  ConstrainedValue<int32_t> value;

  EXPECT_EQ(0, value.Min());
  EXPECT_EQ(0, value.Max());
  EXPECT_EQ(0, value.Get());

  const int32_t min = 10;
  const int32_t max = 15;
  EXPECT_TRUE(value.SetRange(max, min));

  EXPECT_EQ(max, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(max, value.Get());
  EXPECT_FALSE(value.SetRange(max, min));
}


TEST(TestMath_ConstrainedValue, Set_SameValue)
{
  const int32_t min = 10;
  const int32_t max = 15;
  ConstrainedValue<int32_t> value(min, max);
  EXPECT_EQ(min, value.Get());

  // Setting the value to the same should do nothing
  EXPECT_FALSE(value.Set(10));

  EXPECT_EQ(min, value.Get());
}

TEST(TestMath_ConstrainedValue, Set_NewValue_InRange)
{
  const int32_t min = 10;
  const int32_t max = 15;
  ConstrainedValue<int32_t> value(min, max);
  EXPECT_EQ(min, value.Get());

  EXPECT_TRUE(value.Set(15));
  EXPECT_EQ(15, value.Get());
  EXPECT_TRUE(value.Set(14));
  EXPECT_EQ(14, value.Get());
  EXPECT_TRUE(value.Set(13));
  EXPECT_EQ(13, value.Get());
  EXPECT_TRUE(value.Set(12));
  EXPECT_EQ(12, value.Get());
  EXPECT_TRUE(value.Set(11));
  EXPECT_EQ(11, value.Get());
  EXPECT_TRUE(value.Set(10));
  EXPECT_EQ(10, value.Get());
}


TEST(TestMath_ConstrainedValue, Set_NewValue_Below)
{
  const int32_t min = 10;
  const int32_t max = 15;
  ConstrainedValue<int32_t> value(min, max);
  {
    EXPECT_TRUE(value.Set(max));

    EXPECT_TRUE(value.Set(min - 1));
    EXPECT_EQ(min, value.Get());
  }
  {
    EXPECT_TRUE(value.Set(max));

    EXPECT_TRUE(value.Set(std::numeric_limits<int32_t>::min()));
    EXPECT_EQ(min, value.Get());
  }
}


TEST(TestMath_ConstrainedValue, Set_NewValue_Above)
{
  const int32_t min = 10;
  const int32_t max = 15;
  ConstrainedValue<int32_t> value(min, max);
  EXPECT_TRUE(value.Set(min + 1));

  {
    EXPECT_TRUE(value.Set(min));

    EXPECT_TRUE(value.Set(max + 1));
    EXPECT_EQ(max, value.Get());
  }
  {
    EXPECT_TRUE(value.Set(min));

    EXPECT_TRUE(value.Set(std::numeric_limits<int32_t>::max()));
    EXPECT_EQ(max, value.Get());
  }
}

TEST(TestMath_ConstrainedValue, SetPercentage)
{
  const int32_t min = 100;
  const int32_t max = 200;
  ConstrainedValue<int32_t> value(min, max);

  EXPECT_TRUE(value.SetPercentage(1.0f));
  EXPECT_EQ(1.0f, value.GetPercentage());
  EXPECT_EQ(max, value.Get());

  EXPECT_TRUE(value.SetPercentage(0.5f));
  EXPECT_EQ(0.5f, value.GetPercentage());
  EXPECT_EQ(min + ((max - min) / 2), value.Get());

  EXPECT_TRUE(value.SetPercentage(0.0f));
  EXPECT_EQ(0.0f, value.GetPercentage());
  EXPECT_EQ(min, value.Get());
}

TEST(TestMath_ConstrainedValue, SetPercentage_OutOfRange)
{
  const int32_t min = 100;
  const int32_t max = 200;
  ConstrainedValue<int32_t> value(min, max);

  EXPECT_TRUE(value.SetPercentage(std::numeric_limits<float>::max()));
  EXPECT_EQ(1.0f, value.GetPercentage());
  EXPECT_EQ(max, value.Get());

  EXPECT_TRUE(value.SetPercentage(std::numeric_limits<float>::min()));
  EXPECT_EQ(0.0f, value.GetPercentage());
  EXPECT_EQ(min, value.Get());
}


TEST(TestMath_ConstrainedValue, GetPercentage)
{
  const int32_t min = 100;
  const int32_t max = 200;
  ConstrainedValue<int32_t> value(min, max);

  EXPECT_TRUE(value.SetPercentage(1.0f));
  EXPECT_EQ(1.0f, value.GetPercentage());
  EXPECT_TRUE(value.SetPercentage(0.5f));
  EXPECT_EQ(0.5f, value.GetPercentage());
  EXPECT_TRUE(value.SetPercentage(0.0f));
  EXPECT_EQ(0.0f, value.GetPercentage());
}


TEST(TestMath_ConstrainedValue, OpEqual)
{
  EXPECT_TRUE(ConstrainedValue<int32_t>(1, 2, 3) == ConstrainedValue<int32_t>(1, 2, 3));
  EXPECT_FALSE(ConstrainedValue<int32_t>(1, 2, 3) == ConstrainedValue<int32_t>(4, 2, 3));
  EXPECT_FALSE(ConstrainedValue<int32_t>(1, 2, 3) == ConstrainedValue<int32_t>(1, 4, 3));
  EXPECT_FALSE(ConstrainedValue<int32_t>(1, 2, 3) == ConstrainedValue<int32_t>(1, 2, 4));
}


TEST(TestMath_ConstrainedValue, OpNotEqual)
{
  EXPECT_FALSE(ConstrainedValue<int32_t>(1, 2, 3) != ConstrainedValue<int32_t>(1, 2, 3));
  EXPECT_TRUE(ConstrainedValue<int32_t>(1, 2, 3) != ConstrainedValue<int32_t>(4, 2, 3));
  EXPECT_TRUE(ConstrainedValue<int32_t>(1, 2, 3) != ConstrainedValue<int32_t>(1, 4, 3));
  EXPECT_TRUE(ConstrainedValue<int32_t>(1, 2, 3) != ConstrainedValue<int32_t>(1, 2, 4));
}
