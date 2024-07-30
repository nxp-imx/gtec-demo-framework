/****************************************************************************************************************************************************
 * Copyright 2018, 2022, 2024 NXP
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

#include <FslBase/Time/TimeSpanUtil.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <limits>

using namespace Fsl;

namespace
{
  using TestTime_TimeSpanUtil = TestFixtureFslBase;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, FromMicroseconds_UInt32)
{
  TimeSpan value = TimeSpanUtil::FromMicroseconds(static_cast<uint32_t>(1));
  EXPECT_EQ(TimeSpan::TicksPerMicrosecond, value.Ticks());
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_UInt32_Min)
{
  constexpr TimeSpan ExpectedValue(static_cast<int64_t>(TimeSpan::TicksPerMicrosecond) * std::numeric_limits<uint32_t>::min());
  EXPECT_EQ(ExpectedValue, TimeSpanUtil::FromMicroseconds(std::numeric_limits<uint32_t>::min()));
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_UInt32_Max)
{
  constexpr TimeSpan ExpectedValue(static_cast<int64_t>(TimeSpan::TicksPerMicrosecond) * std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(ExpectedValue, TimeSpanUtil::FromMicroseconds(std::numeric_limits<uint32_t>::max()));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, FromMicroseconds_Int32_Positive)
{
  TimeSpan value = TimeSpanUtil::FromMicroseconds(static_cast<int32_t>(1));
  EXPECT_EQ(TimeSpan::TicksPerMicrosecond, value.Ticks());
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_Int32_Negative)
{
  TimeSpan value = TimeSpanUtil::FromMicroseconds(static_cast<int32_t>(-1));
  EXPECT_EQ(-TimeSpan::TicksPerMicrosecond, value.Ticks());
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_Int32_Min)
{
  constexpr TimeSpan ExpectedValue(static_cast<int64_t>(TimeSpan::TicksPerMicrosecond) * std::numeric_limits<int32_t>::min());
  EXPECT_EQ(ExpectedValue, TimeSpanUtil::FromMicroseconds(std::numeric_limits<int32_t>::min()));
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_Int32_Max)
{
  constexpr TimeSpan ExpectedValue(static_cast<int64_t>(TimeSpan::TicksPerMicrosecond) * std::numeric_limits<int32_t>::max());
  EXPECT_EQ(ExpectedValue, TimeSpanUtil::FromMicroseconds(std::numeric_limits<int32_t>::max()));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, FromMicroseconds_Int64_Positive)
{
  TimeSpan value = TimeSpanUtil::FromMicroseconds(static_cast<int64_t>(1));
  EXPECT_EQ(TimeSpan::TicksPerMicrosecond, value.Ticks());
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_Int64_Negative)
{
  TimeSpan value = TimeSpanUtil::FromMicroseconds(static_cast<int64_t>(-1));
  EXPECT_EQ(-TimeSpan::TicksPerMicrosecond, value.Ticks());
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_Int64_Max)
{
  constexpr int64_t ExpectedValue((std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMicrosecond) * TimeSpan::TicksPerMicrosecond);
  ASSERT_LT(std::numeric_limits<int64_t>::max() - ExpectedValue, TimeSpan::TicksPerMicrosecond);
  EXPECT_EQ(TimeSpan(ExpectedValue), TimeSpanUtil::FromMicroseconds(std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMicrosecond));
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_Int64_MaxSubOne)
{
  constexpr int64_t ExpectedValue(((std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMicrosecond) - 1) * TimeSpan::TicksPerMicrosecond);
  ASSERT_LT(std::numeric_limits<int64_t>::max() - ExpectedValue, 2 * TimeSpan::TicksPerMicrosecond);
  EXPECT_EQ(TimeSpan(ExpectedValue), TimeSpanUtil::FromMicroseconds((std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMicrosecond) - 1));
}


TEST(TestTime_TimeSpanUtil, FromMicroseconds_Int64_Min)
{
  constexpr int64_t ExpectedValue((std::numeric_limits<int64_t>::min() / TimeSpan::TicksPerMicrosecond) * TimeSpan::TicksPerMicrosecond);
  ASSERT_LT(ExpectedValue - std::numeric_limits<int64_t>::min(), TimeSpan::TicksPerMicrosecond);
  EXPECT_EQ(TimeSpan(ExpectedValue), TimeSpanUtil::FromMicroseconds(std::numeric_limits<int64_t>::min() / TimeSpan::TicksPerMicrosecond));
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_Int64_MinAddOne)
{
  constexpr int64_t ExpectedValue(((std::numeric_limits<int64_t>::min() / TimeSpan::TicksPerMicrosecond) + 1) * TimeSpan::TicksPerMicrosecond);
  ASSERT_LT(ExpectedValue - std::numeric_limits<int64_t>::min(), 2 * TimeSpan::TicksPerMicrosecond);
  EXPECT_EQ(TimeSpan(ExpectedValue), TimeSpanUtil::FromMicroseconds((std::numeric_limits<int64_t>::min() / TimeSpan::TicksPerMicrosecond) + 1));
}


TEST(TestTime_TimeSpanUtil, FromMicroseconds_Int64_Overflow0)
{
  EXPECT_THROW(TimeSpanUtil::FromMicroseconds((std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMicrosecond) + 1), OverflowException);
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_Int64_Overflow1)
{
  EXPECT_THROW(TimeSpanUtil::FromMicroseconds(std::numeric_limits<int64_t>::max()), OverflowException);
}


TEST(TestTime_TimeSpanUtil, FromMicroseconds_Int64_Underflow0)
{
  EXPECT_THROW(TimeSpanUtil::FromMicroseconds((std::numeric_limits<int64_t>::min() / TimeSpan::TicksPerMicrosecond) - 1), UnderflowException);
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_Int64_Underflow1)
{
  EXPECT_THROW(TimeSpanUtil::FromMicroseconds(std::numeric_limits<int64_t>::min()), UnderflowException);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, FromMicroseconds_UInt64)
{
  TimeSpan value = TimeSpanUtil::FromMicroseconds(static_cast<uint64_t>(1));
  EXPECT_EQ(TimeSpan::TicksPerMicrosecond, value.Ticks());
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_UInt64_Max)
{
  constexpr int64_t ExpectedValue((std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMicrosecond) * TimeSpan::TicksPerMicrosecond);
  ASSERT_LT(std::numeric_limits<int64_t>::max() - ExpectedValue, TimeSpan::TicksPerMicrosecond);
  EXPECT_EQ(TimeSpan(ExpectedValue),
            TimeSpanUtil::FromMicroseconds(NumericCast<uint64_t>(std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMicrosecond)));
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_UInt64_MaxSubOne)
{
  constexpr int64_t ExpectedValue(((std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMicrosecond) - 1) * TimeSpan::TicksPerMicrosecond);
  ASSERT_LT(std::numeric_limits<int64_t>::max() - ExpectedValue, 2 * TimeSpan::TicksPerMicrosecond);
  EXPECT_EQ(TimeSpan(ExpectedValue),
            TimeSpanUtil::FromMicroseconds(NumericCast<uint64_t>((std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMicrosecond) - 1)));
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_UInt64_OverflowMax)
{
  constexpr uint64_t ExpectedValue((std::numeric_limits<uint64_t>::max() / TimeSpan::TicksPerMicrosecond) * TimeSpan::TicksPerMicrosecond);
  ASSERT_LT(std::numeric_limits<uint64_t>::max() - ExpectedValue, TimeSpan::TicksPerMicrosecond);
  EXPECT_THROW(TimeSpanUtil::FromMicroseconds(std::numeric_limits<uint64_t>::max() / TimeSpan::TicksPerMicrosecond), ConversionException);
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_UInt64_OverflowMaxSubOne)
{
  constexpr uint64_t ExpectedValue(((std::numeric_limits<uint64_t>::max() / TimeSpan::TicksPerMicrosecond) - 1) * TimeSpan::TicksPerMicrosecond);
  ASSERT_LT(std::numeric_limits<uint64_t>::max() - ExpectedValue, 2u * TimeSpan::TicksPerMicrosecond);
  EXPECT_THROW(TimeSpanUtil::FromMicroseconds((std::numeric_limits<uint64_t>::max() / TimeSpan::TicksPerMicrosecond) - 1), ConversionException);
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_UInt64_Overflow0)
{
  EXPECT_THROW(
    TimeSpanUtil::FromMicroseconds(UncheckedNumericCast<uint64_t>(std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMicrosecond) + 1),
    ConversionException);
}

TEST(TestTime_TimeSpanUtil, FromMicroseconds_UInt64_Overflow1)
{
  EXPECT_THROW(TimeSpanUtil::FromMicroseconds(std::numeric_limits<uint64_t>::max()), OverflowException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, FromMilliseconds_UInt32)
{
  TimeSpan value = TimeSpanUtil::FromMilliseconds(static_cast<uint32_t>(1));
  EXPECT_EQ(TimeSpan::TicksPerMillisecond, value.Ticks());
}

TEST(TestTime_TimeSpanUtil, FromMilliseconds_UInt32_Min)
{
  constexpr TimeSpan ExpectedValue(static_cast<int64_t>(TimeSpan::TicksPerMillisecond) * std::numeric_limits<uint32_t>::min());
  EXPECT_EQ(ExpectedValue, TimeSpanUtil::FromMilliseconds(std::numeric_limits<uint32_t>::min()));
}

TEST(TestTime_TimeSpanUtil, FromMilliseconds_UInt32_Max)
{
  constexpr TimeSpan ExpectedValue(static_cast<int64_t>(TimeSpan::TicksPerMillisecond) * std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(ExpectedValue, TimeSpanUtil::FromMilliseconds(std::numeric_limits<uint32_t>::max()));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, FromMilliseconds_Int32_Positive)
{
  TimeSpan value = TimeSpanUtil::FromMilliseconds(static_cast<int32_t>(1));
  EXPECT_EQ(TimeSpan::TicksPerMillisecond, value.Ticks());
}

TEST(TestTime_TimeSpanUtil, FromMilliseconds_Int32_Negative)
{
  TimeSpan value = TimeSpanUtil::FromMilliseconds(static_cast<int32_t>(-1));
  EXPECT_EQ(-TimeSpan::TicksPerMillisecond, value.Ticks());
}

TEST(TestTime_TimeSpanUtil, FromMilliseconds_Int32_Min)
{
  constexpr TimeSpan ExpectedValue(static_cast<int64_t>(TimeSpan::TicksPerMillisecond) * std::numeric_limits<int32_t>::min());
  EXPECT_EQ(ExpectedValue, TimeSpanUtil::FromMilliseconds(std::numeric_limits<int32_t>::min()));
}

TEST(TestTime_TimeSpanUtil, FromMilliseconds_Int32_Max)
{
  constexpr TimeSpan ExpectedValue(static_cast<int64_t>(TimeSpan::TicksPerMillisecond) * std::numeric_limits<int32_t>::max());
  EXPECT_EQ(ExpectedValue, TimeSpanUtil::FromMilliseconds(std::numeric_limits<int32_t>::max()));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, FromMilliseconds_Int64_Positive)
{
  TimeSpan value = TimeSpanUtil::FromMilliseconds(static_cast<int64_t>(1));
  EXPECT_EQ(TimeSpan::TicksPerMillisecond, value.Ticks());
}

TEST(TestTime_TimeSpanUtil, FromMilliseconds_Int64_Negative)
{
  TimeSpan value = TimeSpanUtil::FromMilliseconds(static_cast<int64_t>(-1));
  EXPECT_EQ(-TimeSpan::TicksPerMillisecond, value.Ticks());
}


TEST(TestTime_TimeSpanUtil, FromMilliseconds_Int64_Max)
{
  constexpr int64_t ExpectedValue((std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMillisecond) * TimeSpan::TicksPerMillisecond);
  ASSERT_LT(std::numeric_limits<int64_t>::max() - ExpectedValue, TimeSpan::TicksPerMillisecond);
  EXPECT_EQ(TimeSpan(ExpectedValue), TimeSpanUtil::FromMilliseconds(std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMillisecond));
}

TEST(TestTime_TimeSpanUtil, FromMilliseconds_Int64_MaxSubOne)
{
  constexpr int64_t ExpectedValue(((std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMillisecond) - 1) * TimeSpan::TicksPerMillisecond);
  ASSERT_LT(std::numeric_limits<int64_t>::max() - ExpectedValue, 2 * TimeSpan::TicksPerMillisecond);
  EXPECT_EQ(TimeSpan(ExpectedValue), TimeSpanUtil::FromMilliseconds((std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMillisecond) - 1));
}


TEST(TestTime_TimeSpanUtil, FromMilliseconds_Int64_Min)
{
  constexpr int64_t ExpectedValue((std::numeric_limits<int64_t>::min() / TimeSpan::TicksPerMillisecond) * TimeSpan::TicksPerMillisecond);
  ASSERT_LT(ExpectedValue - std::numeric_limits<int64_t>::min(), TimeSpan::TicksPerMillisecond);
  EXPECT_EQ(TimeSpan(ExpectedValue), TimeSpanUtil::FromMilliseconds(std::numeric_limits<int64_t>::min() / TimeSpan::TicksPerMillisecond));
}

TEST(TestTime_TimeSpanUtil, FromMilliseconds_Int64_MinAddOne)
{
  constexpr int64_t ExpectedValue(((std::numeric_limits<int64_t>::min() / TimeSpan::TicksPerMillisecond) + 1) * TimeSpan::TicksPerMillisecond);
  ASSERT_LT(ExpectedValue - std::numeric_limits<int64_t>::min(), 2 * TimeSpan::TicksPerMillisecond);
  EXPECT_EQ(TimeSpan(ExpectedValue), TimeSpanUtil::FromMilliseconds((std::numeric_limits<int64_t>::min() / TimeSpan::TicksPerMillisecond) + 1));
}


TEST(TestTime_TimeSpanUtil, FromMilliseconds_Int64_Overflow0)
{
  EXPECT_THROW(TimeSpanUtil::FromMilliseconds((std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMillisecond) + 1), ConversionException);
}

TEST(TestTime_TimeSpanUtil, FromMilliseconds_Int64_Overflow1)
{
  EXPECT_THROW(TimeSpanUtil::FromMilliseconds(std::numeric_limits<int64_t>::max()), OverflowException);
}

TEST(TestTime_TimeSpanUtil, FromMilliseconds_Int64_Underflow0)
{
  EXPECT_THROW(TimeSpanUtil::FromMilliseconds((std::numeric_limits<int64_t>::min() / TimeSpan::TicksPerMillisecond) - 1), UnderflowException);
}

TEST(TestTime_TimeSpanUtil, FromMilliseconds_Int64_Underflow1)
{
  EXPECT_THROW(TimeSpanUtil::FromMilliseconds(std::numeric_limits<int64_t>::min()), UnderflowException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, FromMilliseconds_UInt64)
{
  TimeSpan value = TimeSpanUtil::FromMilliseconds(static_cast<uint64_t>(1));
  EXPECT_EQ(TimeSpan::TicksPerMillisecond, value.Ticks());
}


TEST(TestTime_TimeSpanUtil, FromMilliseconds_UInt64_Overflow0)
{
  EXPECT_THROW(
    TimeSpanUtil::FromMilliseconds(UncheckedNumericCast<uint64_t>(std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMillisecond) + 1),
    ConversionException);
}


TEST(TestTime_TimeSpanUtil, FromMilliseconds_UInt64_Overflow1)
{
  EXPECT_THROW(TimeSpanUtil::FromMilliseconds(std::numeric_limits<uint64_t>::max()), OverflowException);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, FromSeconds_UInt64)
{
  TimeSpan value = TimeSpanUtil::FromSeconds(static_cast<uint64_t>(1));
  EXPECT_EQ(TimeSpan::TicksPerSecond, value.Ticks());
}


TEST(TestTime_TimeSpanUtil, FromSeconds_UInt64_Overflow0)
{
  EXPECT_THROW(TimeSpanUtil::FromSeconds(UncheckedNumericCast<uint64_t>(std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerSecond) + 1),
               ConversionException);
}


TEST(TestTime_TimeSpanUtil, FromSeconds_UInt64_Overflow1)
{
  EXPECT_THROW(TimeSpanUtil::FromSeconds(std::numeric_limits<uint64_t>::max()), OverflowException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, ToMicrosecondsUInt64)
{
  EXPECT_EQ(2u, TimeSpanUtil::ToMicrosecondsUInt64(TimeSpanUtil::FromMicroseconds(2)));
}


TEST(TestTime_TimeSpanUtil, ToMicrosecondsUInt64_Underflow)
{
  EXPECT_THROW(TimeSpanUtil::ToMicrosecondsUInt64(TimeSpan(-1)), UnderflowException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, ToClampedMicrosecondsUInt64)
{
  EXPECT_EQ(2u, TimeSpanUtil::ToClampedMicrosecondsUInt64(TimeSpanUtil::FromMicroseconds(2)));
}


TEST(TestTime_TimeSpanUtil, ToClampedMicrosecondsUInt64_Underflow)
{
  EXPECT_EQ(0u, TimeSpanUtil::ToClampedMicrosecondsUInt64(TimeSpan(-1)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, ToMicrosecondsInt32_Postive)
{
  EXPECT_EQ(2, TimeSpanUtil::ToMicrosecondsInt32(TimeSpanUtil::FromMicroseconds(2)));
}


TEST(TestTime_TimeSpanUtil, ToMicrosecondsInt32_Negative)
{
  EXPECT_EQ(-2, TimeSpanUtil::ToMicrosecondsInt32(TimeSpanUtil::FromMicroseconds(-2)));
}


TEST(TestTime_TimeSpanUtil, ToMicrosecondsInt32_Underflow)
{
  EXPECT_THROW(TimeSpanUtil::ToMicrosecondsInt32(TimeSpanUtil::FromMicroseconds(static_cast<int64_t>(std::numeric_limits<int32_t>::min()) - 1)),
               ConversionException);
}


TEST(TestTime_TimeSpanUtil, ToMicrosecondsInt32_Overflow)
{
  EXPECT_THROW(TimeSpanUtil::ToMicrosecondsInt32(TimeSpanUtil::FromMicroseconds(static_cast<int64_t>(std::numeric_limits<int32_t>::max()) + 1)),
               ConversionException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, ToClampedMicrosecondsInt32_Postive)
{
  EXPECT_EQ(2, TimeSpanUtil::ToClampedMicrosecondsInt32(TimeSpanUtil::FromMicroseconds(2)));
}


TEST(TestTime_TimeSpanUtil, ToClampedMicrosecondsInt32_Negative)
{
  EXPECT_EQ(-2, TimeSpanUtil::ToClampedMicrosecondsInt32(TimeSpanUtil::FromMicroseconds(-2)));
}


TEST(TestTime_TimeSpanUtil, ToClampedMicrosecondsInt32_Underflow)
{
  EXPECT_EQ(std::numeric_limits<int32_t>::min(),
            TimeSpanUtil::ToClampedMicrosecondsInt32(TimeSpanUtil::FromMicroseconds(static_cast<int64_t>(std::numeric_limits<int32_t>::min()) - 1)));
}


TEST(TestTime_TimeSpanUtil, ToClampedMicrosecondsInt32_Overflow)
{
  EXPECT_EQ(std::numeric_limits<int32_t>::max(),
            TimeSpanUtil::ToClampedMicrosecondsInt32(TimeSpanUtil::FromMicroseconds(static_cast<int64_t>(std::numeric_limits<int32_t>::max()) + 1)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, ToMicrosecondsInt64_Positive)
{
  EXPECT_EQ(2, TimeSpanUtil::ToMicrosecondsInt64(TimeSpanUtil::FromMicroseconds(2)));
}


TEST(TestTime_TimeSpanUtil, ToMicrosecondsInt64_Negative)
{
  EXPECT_EQ(-2, TimeSpanUtil::ToMicrosecondsInt64(TimeSpanUtil::FromMicroseconds(-2)));
}


TEST(TestTime_TimeSpanUtil, ToMicrosecondsInt64_Min)
{
  EXPECT_EQ(std::numeric_limits<int64_t>::min() / TimeSpan::TicksPerMicrosecond,
            TimeSpanUtil::ToMicrosecondsInt64(TimeSpan(std::numeric_limits<int64_t>::min())));
}

TEST(TestTime_TimeSpanUtil, ToMicrosecondsInt64_Max)
{
  EXPECT_EQ(std::numeric_limits<int64_t>::max() / TimeSpan::TicksPerMicrosecond,
            TimeSpanUtil::ToMicrosecondsInt64(TimeSpan(std::numeric_limits<int64_t>::max())));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestTime_TimeSpanUtil, ToMillisecondsUInt64)
{
  EXPECT_EQ(2u, TimeSpanUtil::ToMillisecondsUInt64(TimeSpanUtil::FromMilliseconds(2)));
}


TEST(TestTime_TimeSpanUtil, ToMillisecondsUInt64_Underflow)
{
  EXPECT_THROW(TimeSpanUtil::ToMillisecondsUInt64(TimeSpan(-1)), UnderflowException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, ToClampedMillisecondsUInt64)
{
  EXPECT_EQ(2u, TimeSpanUtil::ToClampedMillisecondsUInt64(TimeSpanUtil::FromMilliseconds(2)));
}


TEST(TestTime_TimeSpanUtil, ToClampedMillisecondsUInt64_Underflow)
{
  EXPECT_EQ(0u, TimeSpanUtil::ToClampedMillisecondsUInt64(TimeSpan(-1)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, ToMicrosecondsF)
{
  EXPECT_FLOAT_EQ(0.5f, TimeSpanUtil::ToMicrosecondsF(TimeSpan(TimeSpan::TicksPerMicrosecond / 2)));
  EXPECT_FLOAT_EQ(1.0f, TimeSpanUtil::ToMicrosecondsF(TimeSpanUtil::FromMicroseconds(1)));
  EXPECT_FLOAT_EQ(2.0f, TimeSpanUtil::ToMicrosecondsF(TimeSpanUtil::FromMicroseconds(2)));
}

TEST(TestTime_TimeSpanUtil, ToMicrosecondsF_Min)
{
  constexpr auto Value = std::numeric_limits<int64_t>::min();
  EXPECT_FLOAT_EQ(static_cast<float>(double(Value) / double(TimeSpan::TicksPerMicrosecond)), TimeSpanUtil::ToMicrosecondsF(TimeSpan(Value)));
}


TEST(TestTime_TimeSpanUtil, ToMicrosecondsF_Max)
{
  constexpr auto Value = std::numeric_limits<int64_t>::max();
  EXPECT_FLOAT_EQ(static_cast<float>(double(Value) / double(TimeSpan::TicksPerMicrosecond)), TimeSpanUtil::ToMicrosecondsF(TimeSpan(Value)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, ToMillisecondsF)
{
  EXPECT_FLOAT_EQ(0.5f, TimeSpanUtil::ToMillisecondsF(TimeSpan(TimeSpan::TicksPerMillisecond / 2)));
  EXPECT_FLOAT_EQ(1.0f, TimeSpanUtil::ToMillisecondsF(TimeSpanUtil::FromMilliseconds(1)));
  EXPECT_FLOAT_EQ(2.0f, TimeSpanUtil::ToMillisecondsF(TimeSpanUtil::FromMilliseconds(2)));
}

TEST(TestTime_TimeSpanUtil, ToMillisecondsF_Min)
{
  constexpr auto Value = std::numeric_limits<int64_t>::min();
  EXPECT_FLOAT_EQ(static_cast<float>(double(Value) / double(TimeSpan::TicksPerMillisecond)), TimeSpanUtil::ToMillisecondsF(TimeSpan(Value)));
}


TEST(TestTime_TimeSpanUtil, ToMillisecondsF_Max)
{
  constexpr auto Value = std::numeric_limits<int64_t>::max();
  EXPECT_FLOAT_EQ(static_cast<float>(double(Value) / double(TimeSpan::TicksPerMillisecond)), TimeSpanUtil::ToMillisecondsF(TimeSpan(Value)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestTime_TimeSpanUtil, ToSecondsF)
{
  EXPECT_FLOAT_EQ(0.5f, TimeSpanUtil::ToSecondsF(TimeSpan(TimeSpan::TicksPerSecond / 2)));
  EXPECT_FLOAT_EQ(1.0f, TimeSpanUtil::ToSecondsF(TimeSpanUtil::FromSeconds(1)));
  EXPECT_FLOAT_EQ(2.0f, TimeSpanUtil::ToSecondsF(TimeSpanUtil::FromSeconds(2)));
}

TEST(TestTime_TimeSpanUtil, ToSecondsF_Min)
{
  constexpr auto Value = std::numeric_limits<int64_t>::min();
  EXPECT_FLOAT_EQ(static_cast<float>(double(Value) / double(TimeSpan::TicksPerSecond)), TimeSpanUtil::ToSecondsF(TimeSpan(Value)));
}


TEST(TestTime_TimeSpanUtil, ToSecondsF_Max)
{
  constexpr auto Value = std::numeric_limits<int64_t>::max();
  EXPECT_FLOAT_EQ(static_cast<float>(double(Value) / double(TimeSpan::TicksPerSecond)), TimeSpanUtil::ToSecondsF(TimeSpan(Value)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
