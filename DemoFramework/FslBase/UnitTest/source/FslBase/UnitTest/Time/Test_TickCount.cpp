/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/NumericCast.hpp>
#include <FslBase/Time/TickCount.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestTime_TickCount = TestFixtureFslBase;
}


TEST(TestTime_TickCount, Construct_Default)
{
  const TickCount value;
  EXPECT_EQ(0u, value.Ticks());
}


TEST(TestTime_TickCount, Construct_Ticks)
{
  const TickCount value(10);
  EXPECT_EQ(10u, value.Ticks());
}

TEST(TestTime_TickCount, Construct_Limits)
{
  const TickCount minValue(std::numeric_limits<int64_t>::min());
  const TickCount maxValue(std::numeric_limits<int64_t>::max());
  const TickCount negativeValue(-1);
  const TickCount zeroValue(0);
  const TickCount plusValue(1);
  EXPECT_EQ(minValue.Ticks(), std::numeric_limits<int64_t>::min());
  EXPECT_EQ(maxValue.Ticks(), std::numeric_limits<int64_t>::max());
  EXPECT_EQ(negativeValue.Ticks(), -1);
  EXPECT_EQ(zeroValue.Ticks(), 0);
  EXPECT_EQ(plusValue.Ticks(), 1);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TickCount, Days)
{
  constexpr int64_t Units = 2;
  constexpr int64_t Ticks = Units * TickCount::TicksPerDay;

  const TickCount value(Ticks);
  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(Units, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());
}


TEST(TestTime_TickCount, Hours)
{
  constexpr int64_t Units = 2;
  constexpr int64_t Ticks = Units * TickCount::TicksPerHour;

  const TickCount value(Ticks);
  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(Units, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());
}


TEST(TestTime_TickCount, Minutes)
{
  constexpr int64_t Units = 2;
  constexpr int64_t Ticks = Units * TickCount::TicksPerMinute;

  const TickCount value(Ticks);
  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(Units, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());
}


TEST(TestTime_TickCount, Seconds)
{
  constexpr int64_t Units = 2;
  constexpr int64_t Ticks = Units * TickCount::TicksPerSecond;

  const TickCount value(Ticks);
  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(Units, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());
}


TEST(TestTime_TickCount, Milliseconds)
{
  constexpr int64_t Units = 2;
  constexpr int64_t Ticks = Units * TickCount::TicksPerMillisecond;

  const TickCount value(Ticks);
  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(Units, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());
}


TEST(TestTime_TickCount, Microseconds)
{
  constexpr int64_t Units = 2;
  constexpr int64_t Ticks = Units * TickCount::TicksPerMicrosecond;

  const TickCount value(Ticks);
  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(Units, value.Microseconds());
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TickCount, FromDays_Signed)
{
  constexpr int32_t Units = 2;
  constexpr int64_t Ticks = Units * TickCount::TicksPerDay;
  auto value = TickCount::FromDays(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(Units, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());


  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalDays());
}

TEST(TestTime_TickCount, FromDays_Signed_MinValue)
{
  constexpr auto Units = NumericCast<int32_t>(std::numeric_limits<int64_t>::min() / TickCount::TicksPerDay);
  constexpr int64_t Ticks = Units * TickCount::TicksPerDay;
  auto value = TickCount::FromDays(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(Units, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());


  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalDays());
}

TEST(TestTime_TickCount, FromDays_Signed_MaxValue)
{
  constexpr auto Units = NumericCast<int32_t>(std::numeric_limits<int64_t>::max() / TickCount::TicksPerDay);
  constexpr int64_t Ticks = Units * TickCount::TicksPerDay;
  auto value = TickCount::FromDays(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(Units, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalDays());
}

TEST(TestTime_TickCount, FromDays_Signed_Underflow)
{
  constexpr auto Units = NumericCast<int32_t>((std::numeric_limits<int64_t>::min() / TickCount::TicksPerDay) - 1);
  EXPECT_THROW(TickCount::FromDays(Units), UnderflowException);
}


TEST(TestTime_TickCount, FromDays_Signed_Overflow)
{
  constexpr auto Units = NumericCast<int32_t>((std::numeric_limits<int64_t>::max() / TickCount::TicksPerDay) + 1);
  EXPECT_THROW(TickCount::FromDays(Units), OverflowException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TickCount, FromHours_Signed)
{
  constexpr int32_t Units = 2;
  constexpr int64_t Ticks = Units * TickCount::TicksPerHour;
  auto value = TickCount::FromHours(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 24), value.Days());
  EXPECT_EQ(Units % 24, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());


  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalHours());
}


TEST(TestTime_TickCount, FromHours_Signed_MinValue)
{
  constexpr auto Units = NumericCast<int32_t>(std::numeric_limits<int64_t>::min() / TickCount::TicksPerHour);
  constexpr int64_t Ticks = Units * TickCount::TicksPerHour;
  auto value = TickCount::FromHours(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 24), value.Days());
  EXPECT_EQ(Units % 24, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalHours());
}

TEST(TestTime_TickCount, FromHours_Signed_MaxValue)
{
  constexpr auto Units = NumericCast<int32_t>(std::numeric_limits<int64_t>::max() / TickCount::TicksPerHour);
  constexpr int64_t Ticks = Units * TickCount::TicksPerHour;
  auto value = TickCount::FromHours(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 24), value.Days());
  EXPECT_EQ(Units % 24, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalHours());
}


TEST(TestTime_TickCount, FromHours_Signed_Underflow)
{
  constexpr auto Units = NumericCast<int32_t>((std::numeric_limits<int64_t>::min() / TickCount::TicksPerHour) - 1);
  EXPECT_THROW(TickCount::FromDays(Units), UnderflowException);
}


TEST(TestTime_TickCount, FromHours_Signed_Overflow)
{
  constexpr auto Units = NumericCast<int32_t>((std::numeric_limits<int64_t>::max() / TickCount::TicksPerHour) + 1);
  EXPECT_THROW(TickCount::FromHours(Units), OverflowException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TickCount, FromMinutes_Signed)
{
  constexpr int64_t Units = 2;
  constexpr int64_t Ticks = NumericCast<int32_t>(Units * TickCount::TicksPerMinute);
  auto value = TickCount::FromMinutes(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 60 / 24), value.Days());
  EXPECT_EQ((Units / 60) % 24, value.Hours());
  EXPECT_EQ(Units % 60, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalMinutes());
}


TEST(TestTime_TickCount, FromMinutes_Signed_MinValue)
{
  constexpr int64_t Units = (std::numeric_limits<int64_t>::min() / TickCount::TicksPerMinute);
  constexpr int64_t Ticks = Units * TickCount::TicksPerMinute;
  auto value = TickCount::FromMinutes(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 60 / 24), value.Days());
  EXPECT_EQ((Units / 60) % 24, value.Hours());
  EXPECT_EQ(Units % 60, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalMinutes());
}

TEST(TestTime_TickCount, FromMinutes_Signed_MaxValue)
{
  constexpr int64_t Units = std::numeric_limits<int64_t>::max() / TickCount::TicksPerMinute;
  constexpr int64_t Ticks = Units * TickCount::TicksPerMinute;
  auto value = TickCount::FromMinutes(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 60 / 24), value.Days());
  EXPECT_EQ((Units / 60) % 24, value.Hours());
  EXPECT_EQ(Units % 60, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalMinutes());
}

TEST(TestTime_TickCount, FromMinutes_Signed_Underflow)
{
  constexpr int64_t Units = (std::numeric_limits<int64_t>::min() / TickCount::TicksPerMinute) - 1;
  EXPECT_THROW(TickCount::FromMinutes(Units), UnderflowException);
}


TEST(TestTime_TickCount, FromMinutes_Signed_Overflow)
{
  constexpr int64_t Units = (std::numeric_limits<int64_t>::max() / TickCount::TicksPerMinute) + 1;
  EXPECT_THROW(TickCount::FromMinutes(Units), OverflowException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TickCount, FromSeconds_Signed)
{
  constexpr int64_t Units = 2;
  constexpr int64_t Ticks = Units * TickCount::TicksPerSecond;
  auto value = TickCount::FromSeconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 60) % 60, value.Minutes());
  EXPECT_EQ(Units % 60, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalSeconds());
}


TEST(TestTime_TickCount, FromSeconds_Signed_MinValue)
{
  constexpr int64_t Units = (std::numeric_limits<int64_t>::min() / TickCount::TicksPerSecond);
  constexpr int64_t Ticks = Units * TickCount::TicksPerSecond;
  auto value = TickCount::FromSeconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 60) % 60, value.Minutes());
  EXPECT_EQ(Units % 60, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalSeconds());
}

TEST(TestTime_TickCount, FromSeconds_Signed_MaxValue)
{
  constexpr int64_t Units = std::numeric_limits<int64_t>::max() / TickCount::TicksPerSecond;
  constexpr int64_t Ticks = Units * TickCount::TicksPerSecond;
  auto value = TickCount::FromSeconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 60) % 60, value.Minutes());
  EXPECT_EQ(Units % 60, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalSeconds());
}

TEST(TestTime_TickCount, FromSeconds_Signed_Underflow)
{
  constexpr int64_t Units = (std::numeric_limits<int64_t>::min() / TickCount::TicksPerSecond) - 1;
  EXPECT_THROW(TickCount::FromSeconds(Units), UnderflowException);
}


TEST(TestTime_TickCount, FromSeconds_Signed_Overflow)
{
  constexpr int64_t Units = (std::numeric_limits<int64_t>::max() / TickCount::TicksPerSecond) + 1;
  EXPECT_THROW(TickCount::FromSeconds(Units), OverflowException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TickCount, FromMilliseconds_Signed)
{
  constexpr int64_t Units = 2;
  constexpr int64_t Ticks = Units * TickCount::TicksPerMillisecond;
  auto value = TickCount::FromMilliseconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 1000 / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 1000 / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 1000 / 60) % 60, value.Minutes());
  EXPECT_EQ((Units / 1000) % 60, value.Seconds());
  EXPECT_EQ(Units % 1000, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());


  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalMilliseconds());
}


TEST(TestTime_TickCount, FromMilliseconds_Signed_MinValue)
{
  constexpr int64_t Units = (std::numeric_limits<int64_t>::min() / TickCount::TicksPerMillisecond);
  constexpr int64_t Ticks = Units * TickCount::TicksPerMillisecond;
  auto value = TickCount::FromMilliseconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 1000 / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 1000 / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 1000 / 60) % 60, value.Minutes());
  EXPECT_EQ((Units / 1000) % 60, value.Seconds());
  EXPECT_EQ(Units % 1000, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalMilliseconds());
}

TEST(TestTime_TickCount, FromMilliseconds_Signed_MaxValue)
{
  constexpr int64_t Units = std::numeric_limits<int64_t>::max() / TickCount::TicksPerMillisecond;
  constexpr int64_t Ticks = Units * TickCount::TicksPerMillisecond;
  auto value = TickCount::FromMilliseconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 1000 / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 1000 / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 1000 / 60) % 60, value.Minutes());
  EXPECT_EQ((Units / 1000) % 60, value.Seconds());
  EXPECT_EQ(Units % 1000, value.Milliseconds());
  EXPECT_EQ(0, value.Microseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalMilliseconds());
}

// overflow and underflow can not occur in FromMilliseconds


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TickCount, FromMicroseconds_Signed)
{
  constexpr int64_t Units = 2;
  constexpr int64_t Ticks = Units * TickCount::TicksPerMicrosecond;
  auto value = TickCount::FromMicroseconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 1000 / 1000 / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 1000 / 1000 / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 1000 / 1000 / 60) % 60, value.Minutes());
  EXPECT_EQ((Units / 1000 / 1000) % 60, value.Seconds());
  EXPECT_EQ((Units / 1000) % 1000, value.Milliseconds());
  EXPECT_EQ(Units % 1000, value.Microseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalMicroseconds());
}


TEST(TestTime_TickCount, FromMicroseconds_Signed_MinValue)
{
  constexpr int64_t Units = std::numeric_limits<int64_t>::min() / TickCount::TicksPerMicrosecond;
  constexpr int64_t Ticks = Units * TickCount::TicksPerMicrosecond;
  auto value = TickCount::FromMicroseconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 1000 / 1000 / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 1000 / 1000 / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 1000 / 1000 / 60) % 60, value.Minutes());
  EXPECT_EQ((Units / 1000 / 1000) % 60, value.Seconds());
  EXPECT_EQ((Units / 1000) % 1000, value.Milliseconds());
  EXPECT_EQ(Units % 1000, value.Microseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalMicroseconds());
}

TEST(TestTime_TickCount, FromMicroseconds_Signed_MaxValue)
{
  constexpr int64_t Units = std::numeric_limits<int64_t>::max() / TickCount::TicksPerMicrosecond;
  constexpr int64_t Ticks = Units * TickCount::TicksPerMicrosecond;
  auto value = TickCount::FromMicroseconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 1000 / 1000 / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 1000 / 1000 / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 1000 / 1000 / 60) % 60, value.Minutes());
  EXPECT_EQ((Units / 1000 / 1000) % 60, value.Seconds());
  EXPECT_EQ((Units / 1000) % 1000, value.Milliseconds());
  EXPECT_EQ(Units % 1000, value.Microseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalMicroseconds());
}

// overflow and underflow can not occur in FromMicroseconds

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TickCount, FromNanoseconds_Signed)
{
  constexpr int64_t Milliseconds = 2;
  constexpr int64_t Ticks = Milliseconds * TickCount::TicksPerMillisecond;
  constexpr int64_t Nanoseconds = Ticks * TickCount::NanoSecondsPerTick;
  auto value = TickCount::FromNanoseconds(Nanoseconds);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(NumericCast<int32_t>(Milliseconds), value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Nanoseconds), value.TotalNanoseconds());
}

TEST(TestTime_TickCount, FromNanoseconds_Signed_MinValue)
{
  constexpr int64_t Units = std::numeric_limits<int64_t>::min();
  constexpr int64_t Ticks = Units / TickCount::NanoSecondsPerTick;
  auto value = TickCount::FromNanoseconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 1000 / 1000 / 1000 / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 1000 / 1000 / 1000 / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 1000 / 1000 / 1000 / 60) % 60, value.Minutes());
  EXPECT_EQ((Units / 1000 / 1000 / 1000) % 60, value.Seconds());
  EXPECT_EQ((Units / 1000 / 1000) % 1000, value.Milliseconds());
  EXPECT_EQ((Units / 1000) % 1000, value.Microseconds());

  const int64_t actualMilliseconds = (Units / TickCount::NanoSecondsPerTick) * TickCount::NanoSecondsPerTick;
  EXPECT_DOUBLE_EQ(static_cast<double>(actualMilliseconds), value.TotalNanoseconds());
}

TEST(TestTime_TickCount, FromNanoseconds_Signed_MaxValue)
{
  constexpr int64_t Units = std::numeric_limits<int64_t>::max();
  constexpr int64_t Ticks = Units / TickCount::NanoSecondsPerTick;
  auto value = TickCount::FromNanoseconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 1000 / 1000 / 1000 / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 1000 / 1000 / 1000 / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 1000 / 1000 / 1000 / 60) % 60, value.Minutes());
  EXPECT_EQ((Units / 1000 / 1000 / 1000) % 60, value.Seconds());
  EXPECT_EQ((Units / 1000 / 1000) % 1000, value.Milliseconds());
  EXPECT_EQ((Units / 1000) % 1000, value.Microseconds());

  const int64_t actualMilliseconds = (Units / TickCount::NanoSecondsPerTick) * TickCount::NanoSecondsPerTick;
  EXPECT_DOUBLE_EQ(static_cast<double>(actualMilliseconds), value.TotalNanoseconds());
}

// overflow and underflow can not occur in FromNanoseconds

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TickCount, Op_AddEqual_TickCount_TimeSpan)
{
  constexpr int64_t Ticks0 = 7;
  constexpr int64_t Ticks1 = 21;
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  TickCount result(value0);
  result += value1;

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) + static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_TickCount, Op_AddEqual_TickCount_TimeSpan_Underflow1)
{
  constexpr int64_t Ticks0 = std::numeric_limits<int64_t>::min();
  constexpr int64_t Ticks1 = -21;
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  TickCount result(value0);
  result += value1;

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) + static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_TickCount, Op_AddEqual_TickCount_TimeSpan_Underflow2)
{
  constexpr int64_t Ticks0 = -21;
  constexpr int64_t Ticks1 = std::numeric_limits<int64_t>::min();
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  TickCount result(value0);
  result += value1;

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) + static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_TickCount, Op_AddEqual_TickCount_TimeSpan_Overflow1)
{
  constexpr int64_t Ticks0 = std::numeric_limits<int64_t>::max();
  constexpr int64_t Ticks1 = 21;
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  TickCount result(value0);
  result += value1;

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) + static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_TickCount, Op_AddEqual_TickCount_TimeSpan_Overflow2)
{
  constexpr int64_t Ticks0 = 7;
  constexpr int64_t Ticks1 = std::numeric_limits<int64_t>::max();
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  TickCount result(value0);
  result += value1;

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) + static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TickCount, Op_SubEqual_TickCount_TimeSpan)
{
  constexpr int64_t Ticks0 = 7;
  constexpr int64_t Ticks1 = 21;
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  TickCount result(value0);
  result -= value1;
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) - static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_TickCount, Op_SubEqual_TickCount_TimeSpan_Underflow1)
{
  constexpr int64_t Ticks0 = std::numeric_limits<int64_t>::min();
  constexpr int64_t Ticks1 = 21;
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  TickCount result(value0);
  result -= value1;
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) - static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_TickCount, Op_SubEqual_TickCount_TimeSpan_Underflow2)
{
  constexpr int64_t Ticks0 = -3;
  constexpr int64_t Ticks1 = std::numeric_limits<int64_t>::max();
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  TickCount result(value0);
  result -= value1;
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) - static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_TickCount, Op_SubEqual_TickCount_TimeSpan_Overflow1)
{
  constexpr int64_t Ticks0 = std::numeric_limits<int64_t>::max();
  constexpr int64_t Ticks1 = -21;
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  TickCount result(value0);
  result -= value1;
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) - static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_TickCount, Op_SubEqual_TickCount_TimeSpan_Overflow2)
{
  constexpr int64_t Ticks0 = 7;
  constexpr int64_t Ticks1 = std::numeric_limits<int64_t>::min();
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  TickCount result(value0);
  result -= value1;
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) - static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TickCount, Op_Equal)
{
  EXPECT_TRUE(TickCount(7) == TickCount(7));
}

TEST(TestTime_TickCount, Op_NotEqual)
{
  EXPECT_TRUE(TickCount(7) != TickCount(8));
}

TEST(TestTime_TickCount, Op_Less)
{
  EXPECT_TRUE(TickCount(7) < TickCount(8));
  EXPECT_FALSE(TickCount(8) < TickCount(8));
  EXPECT_FALSE(TickCount(9) < TickCount(8));
}

TEST(TestTime_TickCount, Op_LessOrEqual)
{
  EXPECT_TRUE(TickCount(7) <= TickCount(8));
  EXPECT_TRUE(TickCount(8) <= TickCount(8));
  EXPECT_FALSE(TickCount(9) <= TickCount(8));
}

TEST(TestTime_TickCount, Op_Greater)
{
  EXPECT_FALSE(TickCount(7) > TickCount(8));
  EXPECT_FALSE(TickCount(8) > TickCount(8));
  EXPECT_TRUE(TickCount(9) > TickCount(8));
}

TEST(TestTime_TickCount, Op_GreaterOrEqual)
{
  EXPECT_FALSE(TickCount(7) >= TickCount(8));
  EXPECT_TRUE(TickCount(8) >= TickCount(8));
  EXPECT_TRUE(TickCount(9) >= TickCount(8));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TickCount, Op_Add_TickCount_TimeSpan)
{
  constexpr int64_t Ticks0 = 7;
  constexpr int64_t Ticks1 = 21;
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  const TickCount result(value0 + value1);

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) + static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_TickCount, Op_Add_TickCount_TimeSpan_Underflow1)
{
  constexpr int64_t Ticks0 = std::numeric_limits<int64_t>::min();
  constexpr int64_t Ticks1 = -21;
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  const TickCount result(value0 + value1);

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) + static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_TickCount, Op_Add_TickCount_TimeSpan_Underflow2)
{
  constexpr int64_t Ticks0 = -21;
  constexpr int64_t Ticks1 = std::numeric_limits<int64_t>::min();
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  const TickCount result(value0 + value1);

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) + static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_TickCount, Op_Add_TickCount_TimeSpan_Overflow1)
{
  constexpr int64_t Ticks0 = std::numeric_limits<int64_t>::max();
  constexpr int64_t Ticks1 = 21;
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  const TickCount result(value0 + value1);

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) + static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_TickCount, Op_Add_TickCount_TimeSpan_Overflow2)
{
  constexpr int64_t Ticks0 = 7;
  constexpr int64_t Ticks1 = std::numeric_limits<int64_t>::max();
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  const TickCount result(value0 + value1);

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) + static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TickCount, Op_Sub_TickCount_TickCount)
{
  constexpr int64_t Ticks0 = 7;
  constexpr int64_t Ticks1 = 21;
  const TickCount value0(Ticks0);
  const TickCount value1(Ticks1);

  TimeSpan result(value0 - value1);
  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) - static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_TickCount, Op_Sub_TickCount_TickCount_Underflow1)
{
  constexpr int64_t Ticks0 = std::numeric_limits<int64_t>::min();
  constexpr int64_t Ticks1 = 21;
  const TickCount value0(Ticks0);
  const TickCount value1(Ticks1);

  TimeSpan result(value0 - value1);
  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) - static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_TickCount, Op_Sub_TickCount_TickCount_Underflow2)
{
  constexpr int64_t Ticks0 = -3;
  constexpr int64_t Ticks1 = std::numeric_limits<int64_t>::max();
  const TickCount value0(Ticks0);
  const TickCount value1(Ticks1);

  TimeSpan result(value0 - value1);
  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) - static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_TickCount, Op_Sub_TickCount_TickCount_Overflow1)
{
  constexpr int64_t Ticks0 = std::numeric_limits<int64_t>::max();
  constexpr int64_t Ticks1 = -21;
  const TickCount value0(Ticks0);
  const TickCount value1(Ticks1);

  TimeSpan result(value0 - value1);
  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) - static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_TickCount, Op_Sub_TickCount_TickCount_Overflow2)
{
  constexpr int64_t Ticks0 = 7;
  constexpr int64_t Ticks1 = std::numeric_limits<int64_t>::min();
  const TickCount value0(Ticks0);
  const TickCount value1(Ticks1);

  TimeSpan result(value0 - value1);
  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) - static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TickCount, Op_Sub_TickCount_TimeSpan)
{
  constexpr int64_t Ticks0 = 7;
  constexpr int64_t Ticks1 = 21;
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  const TickCount result(value0 - value1);
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) - static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_TickCount, Op_Sub_TickCount_TimeSpan_Underflow1)
{
  constexpr int64_t Ticks0 = std::numeric_limits<int64_t>::min();
  constexpr int64_t Ticks1 = 21;
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  const TickCount result(value0 - value1);
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) - static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_TickCount, Op_Sub_TickCount_TimeSpan_Underflow2)
{
  constexpr int64_t Ticks0 = -3;
  constexpr int64_t Ticks1 = std::numeric_limits<int64_t>::max();
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  const TickCount result(value0 - value1);
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) - static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_TickCount, Op_Sub_TickCount_TimeSpan_Overflow1)
{
  constexpr int64_t Ticks0 = std::numeric_limits<int64_t>::max();
  constexpr int64_t Ticks1 = -21;
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  const TickCount result(value0 - value1);
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) - static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_TickCount, Op_Sub_TickCount_TimeSpan_Overflow2)
{
  constexpr int64_t Ticks0 = 7;
  constexpr int64_t Ticks1 = std::numeric_limits<int64_t>::min();
  const TickCount value0(Ticks0);
  const TimeSpan value1(Ticks1);

  const TickCount result(value0 - value1);
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint64_t expectedUnsignedResult = static_cast<uint64_t>(Ticks0) - static_cast<uint64_t>(Ticks1);
  const auto expectedResult = static_cast<int64_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}
