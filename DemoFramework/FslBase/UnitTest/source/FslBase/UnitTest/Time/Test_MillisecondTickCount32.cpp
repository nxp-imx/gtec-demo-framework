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
#include <FslBase/Time/MillisecondTickCount32.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestTime_MillisecondTickCount32 = TestFixtureFslBase;
}


TEST(TestTime_MillisecondTickCount32, Construct_Default)
{
  const MillisecondTickCount32 value;
  EXPECT_EQ(0, value.Ticks());
}


TEST(TestTime_MillisecondTickCount32, Construct_Ticks)
{
  const MillisecondTickCount32 value(10);
  EXPECT_EQ(10, value.Ticks());
}

TEST(TestTime_MillisecondTickCount32, Construct_Limits)
{
  const MillisecondTickCount32 minValue(std::numeric_limits<int32_t>::min());
  const MillisecondTickCount32 maxValue(std::numeric_limits<int32_t>::max());
  const MillisecondTickCount32 negativeValue(-1);
  const MillisecondTickCount32 zeroValue(0);
  const MillisecondTickCount32 plusValue(1);
  EXPECT_EQ(minValue.Ticks(), std::numeric_limits<int32_t>::min());
  EXPECT_EQ(maxValue.Ticks(), std::numeric_limits<int32_t>::max());
  EXPECT_EQ(negativeValue.Ticks(), -1);
  EXPECT_EQ(zeroValue.Ticks(), 0);
  EXPECT_EQ(plusValue.Ticks(), 1);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestTime_MillisecondTickCount32, Days)
{
  constexpr int32_t Units = 2;
  constexpr int32_t Ticks = Units * MillisecondTickCount32::TicksPerDay;

  const MillisecondTickCount32 value(Ticks);
  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(Units, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
}


TEST(TestTime_MillisecondTickCount32, Hours)
{
  constexpr int32_t Units = 2;
  constexpr int32_t Ticks = Units * MillisecondTickCount32::TicksPerHour;

  const MillisecondTickCount32 value(Ticks);
  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(Units, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
}


TEST(TestTime_MillisecondTickCount32, Minutes)
{
  constexpr int32_t Units = 2;
  constexpr int32_t Ticks = Units * MillisecondTickCount32::TicksPerMinute;

  const MillisecondTickCount32 value(Ticks);
  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(Units, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
}


TEST(TestTime_MillisecondTickCount32, Seconds)
{
  constexpr int32_t Units = 2;
  constexpr int32_t Ticks = Units * MillisecondTickCount32::TicksPerSecond;

  const MillisecondTickCount32 value(Ticks);
  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(Units, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());
}


TEST(TestTime_MillisecondTickCount32, Milliseconds)
{
  constexpr int32_t Units = 2;
  constexpr int32_t Ticks = Units * MillisecondTickCount32::TicksPerMillisecond;

  const MillisecondTickCount32 value(Ticks);
  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(Units, value.Milliseconds());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_MillisecondTickCount32, FromDays_Signed)
{
  constexpr int32_t Units = 2;
  constexpr auto Ticks = NumericCast<int32_t>(Units * MillisecondTickCount32::TicksPerDay);
  auto value = MillisecondTickCount32::FromDays(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(Units, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalDays());
}

TEST(TestTime_MillisecondTickCount32, FromDays_Signed_MinValue)
{
  constexpr int32_t Units = (std::numeric_limits<int32_t>::min() / MillisecondTickCount32::TicksPerDay);
  constexpr auto Ticks = NumericCast<int32_t>(Units * MillisecondTickCount32::TicksPerDay);
  auto value = MillisecondTickCount32::FromDays(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(Units, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalDays());
}

TEST(TestTime_MillisecondTickCount32, FromDays_Signed_MaxValue)
{
  constexpr int32_t Units = std::numeric_limits<int32_t>::max() / MillisecondTickCount32::TicksPerDay;
  constexpr auto Ticks = NumericCast<int32_t>(Units * MillisecondTickCount32::TicksPerDay);
  auto value = MillisecondTickCount32::FromDays(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(Units, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalDays());
}

TEST(TestTime_MillisecondTickCount32, FromDays_Signed_Underflow)
{
  constexpr int32_t Units = (std::numeric_limits<int32_t>::min() / MillisecondTickCount32::TicksPerDay) - 1;
  EXPECT_THROW(MillisecondTickCount32::FromDays(Units), UnderflowException);
}


TEST(TestTime_MillisecondTickCount32, FromDays_Signed_Overflow)
{
  constexpr int32_t Units = (std::numeric_limits<int32_t>::max() / MillisecondTickCount32::TicksPerDay) + 1;
  EXPECT_THROW(MillisecondTickCount32::FromDays(Units), OverflowException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_MillisecondTickCount32, FromHours_Signed)
{
  constexpr int32_t Units = 2;
  constexpr auto Ticks = NumericCast<int32_t>(Units * MillisecondTickCount32::TicksPerHour);
  auto value = MillisecondTickCount32::FromHours(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(Units, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalHours());
}


TEST(TestTime_MillisecondTickCount32, FromHours_Signed_MinValue)
{
  constexpr int32_t Units = (std::numeric_limits<int32_t>::min() / MillisecondTickCount32::TicksPerHour);
  constexpr auto Ticks = NumericCast<int32_t>(Units * MillisecondTickCount32::TicksPerHour);
  auto value = MillisecondTickCount32::FromHours(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 24), value.Days());
  EXPECT_EQ(Units % 24, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalHours());
}

TEST(TestTime_MillisecondTickCount32, FromHours_Signed_MaxValue)
{
  constexpr int32_t Units = std::numeric_limits<int32_t>::max() / MillisecondTickCount32::TicksPerHour;
  constexpr auto Ticks = NumericCast<int32_t>(Units * MillisecondTickCount32::TicksPerHour);
  auto value = MillisecondTickCount32::FromHours(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 24), value.Days());
  EXPECT_EQ(Units % 24, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalHours());
}


TEST(TestTime_MillisecondTickCount32, FromHours_Signed_Underflow)
{
  constexpr int32_t Units = (std::numeric_limits<int32_t>::min() / MillisecondTickCount32::TicksPerHour) - 1;
  EXPECT_THROW(MillisecondTickCount32::FromDays(Units), UnderflowException);
}


TEST(TestTime_MillisecondTickCount32, FromHours_Signed_Overflow)
{
  constexpr int32_t Units = (std::numeric_limits<int32_t>::max() / MillisecondTickCount32::TicksPerHour) + 1;
  EXPECT_THROW(MillisecondTickCount32::FromHours(Units), OverflowException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_MillisecondTickCount32, FromMinutes_Signed)
{
  constexpr int32_t Units = 2;
  constexpr auto Ticks = NumericCast<int32_t>(Units * MillisecondTickCount32::TicksPerMinute);
  auto value = MillisecondTickCount32::FromMinutes(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(Units, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalMinutes());
}


TEST(TestTime_MillisecondTickCount32, FromMinutes_Signed_MinValue)
{
  constexpr int32_t Units = (std::numeric_limits<int32_t>::min() / MillisecondTickCount32::TicksPerMinute);
  constexpr auto Ticks = NumericCast<int32_t>(Units * MillisecondTickCount32::TicksPerMinute);
  auto value = MillisecondTickCount32::FromMinutes(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 60 / 24), value.Days());
  EXPECT_EQ((Units / 60) % 24, value.Hours());
  EXPECT_EQ(Units % 60, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalMinutes());
}

TEST(TestTime_MillisecondTickCount32, FromMinutes_Signed_MaxValue)
{
  constexpr int32_t Units = std::numeric_limits<int32_t>::max() / MillisecondTickCount32::TicksPerMinute;
  constexpr auto Ticks = NumericCast<int32_t>(Units * MillisecondTickCount32::TicksPerMinute);
  auto value = MillisecondTickCount32::FromMinutes(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 60 / 24), value.Days());
  EXPECT_EQ((Units / 60) % 24, value.Hours());
  EXPECT_EQ(Units % 60, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalMinutes());
}

TEST(TestTime_MillisecondTickCount32, FromMinutes_Signed_Underflow)
{
  constexpr int32_t Units = (std::numeric_limits<int32_t>::min() / MillisecondTickCount32::TicksPerMinute) - 1;
  EXPECT_THROW(MillisecondTickCount32::FromMinutes(Units), UnderflowException);
}


TEST(TestTime_MillisecondTickCount32, FromMinutes_Signed_Overflow)
{
  constexpr int32_t Units = (std::numeric_limits<int32_t>::max() / MillisecondTickCount32::TicksPerMinute) + 1;
  EXPECT_THROW(MillisecondTickCount32::FromMinutes(Units), OverflowException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_MillisecondTickCount32, FromSeconds_Signed)
{
  constexpr int32_t Units = 2;
  constexpr auto Ticks = NumericCast<int32_t>(Units * MillisecondTickCount32::TicksPerSecond);
  auto value = MillisecondTickCount32::FromSeconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(Units, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalSeconds());
}


TEST(TestTime_MillisecondTickCount32, FromSeconds_Signed_MinValue)
{
  constexpr int32_t Units = (std::numeric_limits<int32_t>::min() / MillisecondTickCount32::TicksPerSecond);
  constexpr auto Ticks = NumericCast<int32_t>(Units * MillisecondTickCount32::TicksPerSecond);
  auto value = MillisecondTickCount32::FromSeconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 60) % 60, value.Minutes());
  EXPECT_EQ(Units % 60, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalSeconds());
}

TEST(TestTime_MillisecondTickCount32, FromSeconds_Signed_MaxValue)
{
  constexpr int32_t Units = std::numeric_limits<int32_t>::max() / MillisecondTickCount32::TicksPerSecond;
  constexpr auto Ticks = NumericCast<int32_t>(Units * MillisecondTickCount32::TicksPerSecond);
  auto value = MillisecondTickCount32::FromSeconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 60) % 60, value.Minutes());
  EXPECT_EQ(Units % 60, value.Seconds());
  EXPECT_EQ(0, value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalSeconds());
}

TEST(TestTime_MillisecondTickCount32, FromSeconds_Signed_Underflow)
{
  constexpr int32_t Units = (std::numeric_limits<int32_t>::min() / MillisecondTickCount32::TicksPerSecond) - 1;
  EXPECT_THROW(MillisecondTickCount32::FromSeconds(Units), UnderflowException);
}


TEST(TestTime_MillisecondTickCount32, FromSeconds_Signed_Overflow)
{
  constexpr int32_t Units = (std::numeric_limits<int32_t>::max() / MillisecondTickCount32::TicksPerSecond) + 1;
  EXPECT_THROW(MillisecondTickCount32::FromSeconds(Units), OverflowException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_MillisecondTickCount32, FromMilliseconds_Signed)
{
  constexpr int32_t Units = 2;
  constexpr auto Ticks = NumericCast<int32_t>(Units * MillisecondTickCount32::TicksPerMillisecond);
  auto value = MillisecondTickCount32::FromMilliseconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(Units, value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalMilliseconds());
}


TEST(TestTime_MillisecondTickCount32, FromMilliseconds_Signed_MinValue)
{
  constexpr int32_t Units = (std::numeric_limits<int32_t>::min() / MillisecondTickCount32::TicksPerMillisecond);
  constexpr auto Ticks = NumericCast<int32_t>(Units * MillisecondTickCount32::TicksPerMillisecond);
  auto value = MillisecondTickCount32::FromMilliseconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 1000 / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 1000 / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 1000 / 60) % 60, value.Minutes());
  EXPECT_EQ((Units / 1000) % 60, value.Seconds());
  EXPECT_EQ(Units % 1000, value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalMilliseconds());
}

TEST(TestTime_MillisecondTickCount32, FromMilliseconds_Signed_MaxValue)
{
  constexpr int32_t Units = std::numeric_limits<int32_t>::max() / MillisecondTickCount32::TicksPerMillisecond;
  constexpr auto Ticks = NumericCast<int32_t>(Units * MillisecondTickCount32::TicksPerMillisecond);
  auto value = MillisecondTickCount32::FromMilliseconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 1000 / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 1000 / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 1000 / 60) % 60, value.Minutes());
  EXPECT_EQ((Units / 1000) % 60, value.Seconds());
  EXPECT_EQ(Units % 1000, value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Units), value.TotalMilliseconds());
}

// overflow and underflow can not occur in FromMilliseconds


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_MillisecondTickCount32, FromMicroseconds_Signed)
{
  constexpr int32_t Milliseconds = 2;
  constexpr auto Ticks = NumericCast<int32_t>(Milliseconds);
  constexpr auto Microseconds = NumericCast<int32_t>(Ticks * MillisecondTickCount32::MicrosecondPerTick);
  auto value = MillisecondTickCount32::FromMicroseconds(Microseconds);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(NumericCast<int32_t>(Milliseconds), value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Microseconds), value.TotalMicroseconds());
}


TEST(TestTime_MillisecondTickCount32, FromMicroseconds_Signed_MinValue)
{
  constexpr int32_t Units = std::numeric_limits<int32_t>::min();
  constexpr auto Ticks = NumericCast<int32_t>(Units / MillisecondTickCount32::MicrosecondPerTick);
  auto value = MillisecondTickCount32::FromMicroseconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 1000 / 1000 / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 1000 / 1000 / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 1000 / 1000 / 60) % 60, value.Minutes());
  EXPECT_EQ((Units / 1000 / 1000) % 60, value.Seconds());
  EXPECT_EQ((Units / 1000) % 1000, value.Milliseconds());

  const int32_t actualMicroseconds = (Units / MillisecondTickCount32::MicrosecondPerTick) * MillisecondTickCount32::MicrosecondPerTick;
  EXPECT_DOUBLE_EQ(static_cast<double>(actualMicroseconds), value.TotalMicroseconds());
}

TEST(TestTime_MillisecondTickCount32, FromMicroseconds_Signed_MaxValue)
{
  constexpr int32_t Units = std::numeric_limits<int32_t>::max();
  constexpr auto Ticks = NumericCast<int32_t>(Units / MillisecondTickCount32::MicrosecondPerTick);
  auto value = MillisecondTickCount32::FromMicroseconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 1000 / 1000 / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 1000 / 1000 / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 1000 / 1000 / 60) % 60, value.Minutes());
  EXPECT_EQ((Units / 1000 / 1000) % 60, value.Seconds());
  EXPECT_EQ((Units / 1000) % 1000, value.Milliseconds());

  const int32_t actualMicroseconds = (Units / MillisecondTickCount32::MicrosecondPerTick) * MillisecondTickCount32::MicrosecondPerTick;
  EXPECT_DOUBLE_EQ(static_cast<double>(actualMicroseconds), value.TotalMicroseconds());
}

// overflow and underflow can not occur in FromMicroseconds

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_MillisecondTickCount32, FromNanoseconds_Signed)
{
  constexpr int32_t Milliseconds = 2;
  constexpr auto Ticks = NumericCast<int32_t>(Milliseconds);
  constexpr auto Nanoseconds = NumericCast<int32_t>(Ticks * MillisecondTickCount32::NanoSecondsPerTick);
  auto value = MillisecondTickCount32::FromNanoseconds(Nanoseconds);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ(0, value.Days());
  EXPECT_EQ(0, value.Hours());
  EXPECT_EQ(0, value.Minutes());
  EXPECT_EQ(0, value.Seconds());
  EXPECT_EQ(NumericCast<int32_t>(Milliseconds), value.Milliseconds());

  EXPECT_DOUBLE_EQ(static_cast<double>(Nanoseconds), value.TotalNanoseconds());
}

TEST(TestTime_MillisecondTickCount32, FromNanoseconds_Signed_MinValue)
{
  constexpr int32_t Units = std::numeric_limits<int32_t>::min();
  constexpr auto Ticks = NumericCast<int32_t>(Units / MillisecondTickCount32::NanoSecondsPerTick);
  auto value = MillisecondTickCount32::FromNanoseconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 1000000 / 1000 / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 1000000 / 1000 / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 1000000 / 1000 / 60) % 60, value.Minutes());
  EXPECT_EQ((Units / 1000000 / 1000) % 60, value.Seconds());
  EXPECT_EQ((Units / 1000000) % 1000, value.Milliseconds());

  const int32_t actualMilliseconds = (Units / MillisecondTickCount32::NanoSecondsPerTick) * MillisecondTickCount32::NanoSecondsPerTick;
  EXPECT_DOUBLE_EQ(static_cast<double>(actualMilliseconds), value.TotalNanoseconds());
}

TEST(TestTime_MillisecondTickCount32, FromNanoseconds_Signed_MaxValue)
{
  constexpr int32_t Units = std::numeric_limits<int32_t>::max();
  constexpr auto Ticks = NumericCast<int32_t>(Units / MillisecondTickCount32::NanoSecondsPerTick);
  auto value = MillisecondTickCount32::FromNanoseconds(Units);

  EXPECT_EQ(Ticks, value.Ticks());
  EXPECT_EQ((Units / 1000000 / 1000 / 60 / 60 / 24), value.Days());
  EXPECT_EQ((Units / 1000000 / 1000 / 60 / 60) % 24, value.Hours());
  EXPECT_EQ((Units / 1000000 / 1000 / 60) % 60, value.Minutes());
  EXPECT_EQ((Units / 1000000 / 1000) % 60, value.Seconds());
  EXPECT_EQ((Units / 1000000) % 1000, value.Milliseconds());

  const int32_t actualMilliseconds = (Units / MillisecondTickCount32::NanoSecondsPerTick) * MillisecondTickCount32::NanoSecondsPerTick;
  EXPECT_DOUBLE_EQ(static_cast<double>(actualMilliseconds), value.TotalNanoseconds());
}

// overflow and underflow can not occur in FromNanoseconds

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_MillisecondTickCount32, Op_AddEqual_TickCount_TimeSpan)
{
  constexpr int32_t Ticks0 = 7;
  constexpr int32_t Ticks1 = 21;
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  MillisecondTickCount32 result(value0);
  result += value1;

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) + static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_MillisecondTickCount32, Op_AddEqual_TickCount_TimeSpan_Underflow1)
{
  constexpr int32_t Ticks0 = std::numeric_limits<int32_t>::min();
  constexpr int32_t Ticks1 = -21;
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  MillisecondTickCount32 result(value0);
  result += value1;

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) + static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_MillisecondTickCount32, Op_AddEqual_TickCount_TimeSpan_Underflow2)
{
  constexpr int32_t Ticks0 = -21;
  constexpr int32_t Ticks1 = std::numeric_limits<int32_t>::min();
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  MillisecondTickCount32 result(value0);
  result += value1;

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) + static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_MillisecondTickCount32, Op_AddEqual_TickCount_TimeSpan_Overflow1)
{
  constexpr int32_t Ticks0 = std::numeric_limits<int32_t>::max();
  constexpr int32_t Ticks1 = 1;
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  MillisecondTickCount32 result(value0);
  result += value1;

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) + static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_MillisecondTickCount32, Op_AddEqual_TickCount_TimeSpan_Overflow2)
{
  constexpr int32_t Ticks0 = 7;
  constexpr int32_t Ticks1 = std::numeric_limits<int32_t>::max();
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  MillisecondTickCount32 result(value0);
  result += value1;

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) + static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_MillisecondTickCount32, Op_AddEqual_TickCount_TimeSpan_Overflow3)
{
  constexpr int32_t Ticks0 = std::numeric_limits<int32_t>::max();
  constexpr int32_t Ticks1 = 21;
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  MillisecondTickCount32 result(value0);
  result += value1;

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) + static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_MillisecondTickCount32, Op_SubEqual_TickCount_TimeSpan)
{
  constexpr int32_t Ticks0 = 7;
  constexpr int32_t Ticks1 = 21;
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  MillisecondTickCount32 result(value0);
  result -= value1;
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const int32_t expectedResult = (Ticks0 - Ticks1);
  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_MillisecondTickCount32, Op_SubEqual_TickCount_TimeSpan_Underflow1)
{
  constexpr int32_t Ticks0 = std::numeric_limits<int32_t>::min();
  constexpr int32_t Ticks1 = 21;
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  MillisecondTickCount32 result(value0);
  result -= value1;
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) - static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_MillisecondTickCount32, Op_SubEqual_TickCount_TimeSpan_Underflow2)
{
  constexpr int32_t Ticks0 = -3;
  constexpr int32_t Ticks1 = std::numeric_limits<int32_t>::max();
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  MillisecondTickCount32 result(value0);
  result -= value1;
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) - static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_MillisecondTickCount32, Op_SubEqual_TickCount_TimeSpan_Overflow1)
{
  constexpr int32_t Ticks0 = std::numeric_limits<int32_t>::max();
  constexpr int32_t Ticks1 = -21;
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  MillisecondTickCount32 result(value0);
  result -= value1;
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) - static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_MillisecondTickCount32, Op_SubEqual_TickCount_TimeSpan_Overflow2)
{
  constexpr int32_t Ticks0 = 7;
  constexpr int32_t Ticks1 = std::numeric_limits<int32_t>::min();
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  MillisecondTickCount32 result(value0);
  result -= value1;
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) - static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestTime_MillisecondTickCount32, Op_Equal)
{
  EXPECT_TRUE(MillisecondTickCount32(7) == MillisecondTickCount32(7));
}

TEST(TestTime_MillisecondTickCount32, Op_NotEqual)
{
  EXPECT_TRUE(MillisecondTickCount32(7) != MillisecondTickCount32(8));
}

TEST(TestTime_MillisecondTickCount32, Op_Less)
{
  EXPECT_TRUE(MillisecondTickCount32(7) < MillisecondTickCount32(8));
  EXPECT_FALSE(MillisecondTickCount32(8) < MillisecondTickCount32(8));
  EXPECT_FALSE(MillisecondTickCount32(9) < MillisecondTickCount32(8));
}

TEST(TestTime_MillisecondTickCount32, Op_LessOrEqual)
{
  EXPECT_TRUE(MillisecondTickCount32(7) <= MillisecondTickCount32(8));
  EXPECT_TRUE(MillisecondTickCount32(8) <= MillisecondTickCount32(8));
  EXPECT_FALSE(MillisecondTickCount32(9) <= MillisecondTickCount32(8));
}

TEST(TestTime_MillisecondTickCount32, Op_Greater)
{
  EXPECT_FALSE(MillisecondTickCount32(7) > MillisecondTickCount32(8));
  EXPECT_FALSE(MillisecondTickCount32(8) > MillisecondTickCount32(8));
  EXPECT_TRUE(MillisecondTickCount32(9) > MillisecondTickCount32(8));
}

TEST(TestTime_MillisecondTickCount32, Op_GreaterOrEqual)
{
  EXPECT_FALSE(MillisecondTickCount32(7) >= MillisecondTickCount32(8));
  EXPECT_TRUE(MillisecondTickCount32(8) >= MillisecondTickCount32(8));
  EXPECT_TRUE(MillisecondTickCount32(9) >= MillisecondTickCount32(8));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_MillisecondTickCount32, Op_Add_TickCount_TimeSpan)
{
  constexpr int32_t Ticks0 = 7;
  constexpr int32_t Ticks1 = 21;
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  const MillisecondTickCount32 result(value0 + value1);

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) + static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_MillisecondTickCount32, Op_Add_TickCount_TimeSpan_Underflow1)
{
  constexpr int32_t Ticks0 = std::numeric_limits<int32_t>::min();
  constexpr int32_t Ticks1 = -21;
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  const MillisecondTickCount32 result(value0 + value1);

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) + static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_MillisecondTickCount32, Op_Add_TickCount_TimeSpan_Underflow2)
{
  constexpr int32_t Ticks0 = -21;
  constexpr int32_t Ticks1 = std::numeric_limits<int32_t>::min();
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  const MillisecondTickCount32 result(value0 + value1);

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) + static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_MillisecondTickCount32, Op_Add_TickCount_TimeSpan_Overflow1)
{
  constexpr int32_t Ticks0 = std::numeric_limits<int32_t>::max();
  constexpr int32_t Ticks1 = 1;
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  const MillisecondTickCount32 result(value0 + value1);

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) + static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_MillisecondTickCount32, Op_Add_TickCount_TimeSpan_Overflow2)
{
  constexpr int32_t Ticks0 = 7;
  constexpr int32_t Ticks1 = std::numeric_limits<int32_t>::max();
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  const MillisecondTickCount32 result(value0 + value1);

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) + static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_MillisecondTickCount32, Op_Add_TickCount_TimeSpan_Overflow3)
{
  constexpr int32_t Ticks0 = std::numeric_limits<int32_t>::max();
  constexpr int32_t Ticks1 = 21;
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  const MillisecondTickCount32 result(value0 + value1);

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) + static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_MillisecondTickCount32, Op_Sub_TickCount_TickCount)
{
  constexpr int32_t Ticks0 = 7;
  constexpr int32_t Ticks1 = 21;
  const MillisecondTickCount32 value0(Ticks0);
  const MillisecondTickCount32 value1(Ticks1);

  const TimeSpan result(value0 - value1);
  const int64_t expectedResult = (Ticks0 - Ticks1) * TimeSpan::TicksPerMillisecond;

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_MillisecondTickCount32, Op_Sub_TickCount_TickCount_Underflow1)
{
  constexpr int32_t Ticks0 = std::numeric_limits<int32_t>::min();
  constexpr int32_t Ticks1 = std::numeric_limits<int32_t>::max();
  const MillisecondTickCount32 value0(Ticks0);
  const MillisecondTickCount32 value1(Ticks1);

  EXPECT_EQ(Ticks0, value0.Ticks());
  EXPECT_EQ(Ticks1, value1.Ticks());

  const TimeSpan result(value0 - value1);
  // Due to wraparound this gives a difference of 1
  const int64_t expectedResult = 1u * TimeSpan::TicksPerMillisecond;

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_MillisecondTickCount32, Op_Sub_TickCount_TickCount_Underflow2)
{
  constexpr int32_t Ticks0 = -3;
  constexpr int32_t Ticks1 = std::numeric_limits<int32_t>::max();
  const MillisecondTickCount32 value0(Ticks0);
  const MillisecondTickCount32 value1(Ticks1);

  const TimeSpan result(value0 - value1);
  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) - static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult) * static_cast<int64_t>(TimeSpan::TicksPerMillisecond);

  EXPECT_EQ(expectedResult, result.Ticks());
}

TEST(TestTime_MillisecondTickCount32, Op_Sub_TickCount_TickCount_Underflow3)
{
  constexpr int32_t Ticks0 = -1;
  constexpr int32_t Ticks1 = std::numeric_limits<int32_t>::max();
  const MillisecondTickCount32 value0(Ticks0);
  const MillisecondTickCount32 value1(Ticks1);

  const TimeSpan result(value0 - value1);
  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) - static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult) * static_cast<int64_t>(TimeSpan::TicksPerMillisecond);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_MillisecondTickCount32, Op_Sub_TickCount_TickCount_Overflow1)
{
  constexpr int32_t Ticks0 = std::numeric_limits<int32_t>::max();
  constexpr int32_t Ticks1 = -21;
  const MillisecondTickCount32 value0(Ticks0);
  const MillisecondTickCount32 value1(Ticks1);

  const TimeSpan result(value0 - value1);
  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) - static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult) * static_cast<int64_t>(TimeSpan::TicksPerMillisecond);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_MillisecondTickCount32, Op_Sub_TickCount_TickCount_Overflow2)
{
  constexpr int32_t Ticks0 = 7;
  constexpr int32_t Ticks1 = std::numeric_limits<int32_t>::min();
  const MillisecondTickCount32 value0(Ticks0);
  const MillisecondTickCount32 value1(Ticks1);

  const TimeSpan result(value0 - value1);
  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) - static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult) * static_cast<int64_t>(TimeSpan::TicksPerMillisecond);

  EXPECT_EQ(expectedResult, result.Ticks());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_MillisecondTickCount32, Op_Sub_TickCount_TimeSpan)
{
  constexpr int32_t Ticks0 = 7;
  constexpr int32_t Ticks1 = 21;
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  const MillisecondTickCount32 result(value0 - value1);
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const int32_t expectedResult = (Ticks0 - Ticks1);
  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_MillisecondTickCount32, Op_Sub_TickCount_TimeSpan_Underflow1)
{
  constexpr int32_t Ticks0 = std::numeric_limits<int32_t>::min();
  constexpr int32_t Ticks1 = 21;
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  const MillisecondTickCount32 result(value0 - value1);
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) - static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_MillisecondTickCount32, Op_Sub_TickCount_TimeSpan_Underflow2)
{
  constexpr int32_t Ticks0 = -3;
  constexpr int32_t Ticks1 = std::numeric_limits<int32_t>::max();
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  const MillisecondTickCount32 result(value0 - value1);
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) - static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_MillisecondTickCount32, Op_Sub_TickCount_TimeSpan_Overflow1)
{
  constexpr int32_t Ticks0 = std::numeric_limits<int32_t>::max();
  constexpr int32_t Ticks1 = -21;
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  const MillisecondTickCount32 result(value0 - value1);
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) - static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}


TEST(TestTime_MillisecondTickCount32, Op_Sub_TickCount_TimeSpan_Overflow2)
{
  constexpr int32_t Ticks0 = 7;
  constexpr int32_t Ticks1 = std::numeric_limits<int32_t>::min();
  const MillisecondTickCount32 value0(Ticks0);
  const TimeSpan value1(TimeSpan::FromMilliseconds(Ticks1));

  const MillisecondTickCount32 result(value0 - value1);
  const TimeSpan diff = value0 - result;

  EXPECT_EQ(diff, value1);

  const uint32_t expectedUnsignedResult = static_cast<uint32_t>(Ticks0) - static_cast<uint32_t>(Ticks1);
  const int64_t expectedResult = static_cast<int32_t>(expectedUnsignedResult);

  EXPECT_EQ(expectedResult, result.Ticks());
}
