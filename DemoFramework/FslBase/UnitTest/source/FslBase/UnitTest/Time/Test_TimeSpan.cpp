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

#include <FslBase/Time/TimeSpan.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestTime_TimeSpan = TestFixtureFslBase;
}


TEST(TestTime_TimeSpan, Construct_Default)
{
  const TimeSpan value;
  EXPECT_EQ(0, value.Ticks());
}


TEST(TestTime_TimeSpan, Construct_Ticks)
{
  const TimeSpan value(10);
  EXPECT_EQ(10, value.Ticks());
}

TEST(TestTime_TimeSpan, Days)
{
  constexpr auto units = 2;
  constexpr auto ticks = units * TimeInfo::TicksPerDay;

  const TimeSpan value(ticks);
  EXPECT_EQ(ticks, value.Ticks());
  EXPECT_EQ(units, value.Days());
}


TEST(TestTime_TimeSpan, Minutes)
{
  constexpr auto units = 2;
  constexpr auto ticks = units * TimeInfo::TicksPerMinute;

  const TimeSpan value(ticks);
  EXPECT_EQ(ticks, value.Ticks());
  EXPECT_EQ(units, value.Minutes());
}


TEST(TestTime_TimeSpan, Seconds)
{
  constexpr auto units = 2;
  constexpr auto ticks = units * TimeInfo::TicksPerSecond;

  const TimeSpan value(ticks);
  EXPECT_EQ(ticks, value.Ticks());
  EXPECT_EQ(units, value.Seconds());
}


TEST(TestTime_TimeSpan, Milliseconds)
{
  constexpr auto units = 2;
  constexpr auto ticks = units * TimeInfo::TicksPerMillisecond;

  const TimeSpan value(ticks);
  EXPECT_EQ(ticks, value.Ticks());
  EXPECT_EQ(units, value.Milliseconds());
}


TEST(TestTime_TimeSpan, Microseconds)
{
  constexpr auto units = 2;
  constexpr auto ticks = units * TimeInfo::TicksPerMicrosecond;

  const TimeSpan value(ticks);
  EXPECT_EQ(ticks, value.Ticks());
  EXPECT_EQ(units, value.Microseconds());
}


TEST(TestTime_TimeSpan, Op_AddEqual)
{
  constexpr auto ticks0 = 7;
  constexpr auto ticks1 = 21;
  TimeSpan value0(ticks0);
  const TimeSpan value1(ticks1);

  value0 += value1;

  EXPECT_EQ(ticks0 + ticks1, value0.Ticks());
}


TEST(TestTime_TimeSpan, Op_SubEqual)
{
  constexpr auto ticks0 = 7;
  constexpr auto ticks1 = 21;
  TimeSpan value0(ticks0);
  const TimeSpan value1(ticks1);

  value0 -= value1;

  EXPECT_EQ(ticks0 - ticks1, value0.Ticks());
}


TEST(TestTime_TimeSpan, Op_MulEqual)
{
  constexpr auto ticks0 = 7;
  constexpr int32_t value = 2;
  TimeSpan value0(ticks0);

  value0 *= value;

  EXPECT_EQ(ticks0 * value, value0.Ticks());
}


TEST(TestTime_TimeSpan, Op_DivEqual)
{
  constexpr auto ticks0 = 8;
  constexpr int32_t value = 2;
  TimeSpan value0(ticks0);

  value0 /= value;

  EXPECT_EQ(ticks0 / value, value0.Ticks());
}


TEST(TestTime_TimeSpan, Op_Add)
{
  constexpr auto ticks0 = 7;
  constexpr auto ticks1 = 21;
  const TimeSpan value0(ticks0);
  const TimeSpan value1(ticks1);

  EXPECT_EQ(ticks0 + ticks1, (value0 + value1).Ticks());
}


TEST(TestTime_TimeSpan, Op_Sub)
{
  constexpr auto ticks0 = 7;
  constexpr auto ticks1 = 21;
  const TimeSpan value0(ticks0);
  const TimeSpan value1(ticks1);

  EXPECT_EQ(ticks0 - ticks1, (value0 - value1).Ticks());
}


TEST(TestTime_TimeSpan, Op_MulTimeSpanVal)
{
  constexpr auto ticks0 = 7;
  constexpr int32_t value = 2;
  const TimeSpan value0(ticks0);

  EXPECT_EQ(ticks0 * value, (value0 * value).Ticks());
}


TEST(TestTime_TimeSpan, Op_MulValTimeSpan)
{
  constexpr auto ticks0 = 7;
  constexpr int32_t value = 2;
  const TimeSpan value0(ticks0);

  EXPECT_EQ(value * ticks0, (value * value0).Ticks());
}


TEST(TestTime_TimeSpan, Op_DivTimeSpanVal)
{
  constexpr auto ticks0 = 8;
  constexpr int32_t value = 2;
  const TimeSpan value0(ticks0);

  EXPECT_EQ(ticks0 / value, (value0 / value).Ticks());
}


TEST(TestTime_TimeSpan, Op_Equal)
{
  EXPECT_TRUE(TimeSpan(7) == TimeSpan(7));
}

TEST(TestTime_TimeSpan, Op_NotEqual)
{
  EXPECT_TRUE(TimeSpan(7) != TimeSpan(8));
}

TEST(TestTime_TimeSpan, Op_Less)
{
  EXPECT_TRUE(TimeSpan(7) < TimeSpan(8));
  EXPECT_FALSE(TimeSpan(8) < TimeSpan(8));
  EXPECT_FALSE(TimeSpan(9) < TimeSpan(8));
}

TEST(TestTime_TimeSpan, Op_LessOrEqual)
{
  EXPECT_TRUE(TimeSpan(7) <= TimeSpan(8));
  EXPECT_TRUE(TimeSpan(8) <= TimeSpan(8));
  EXPECT_FALSE(TimeSpan(9) <= TimeSpan(8));
}

TEST(TestTime_TimeSpan, Op_Greater)
{
  EXPECT_FALSE(TimeSpan(7) > TimeSpan(8));
  EXPECT_FALSE(TimeSpan(8) > TimeSpan(8));
  EXPECT_TRUE(TimeSpan(9) > TimeSpan(8));
}

TEST(TestTime_TimeSpan, Op_GreaterOrEqual)
{
  EXPECT_FALSE(TimeSpan(7) >= TimeSpan(8));
  EXPECT_TRUE(TimeSpan(8) >= TimeSpan(8));
  EXPECT_TRUE(TimeSpan(9) >= TimeSpan(8));
}
