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

#include <FslBase/Log/Time/FmtTimeSpan.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestLog_Time_FmtTimeSpan = TestFixtureFslBase;
}


TEST(TestLog_Time_FmtTimeSpan, Log_Days)
{
  TimeSpan value0(TimeSpan::FromDays(5));
  TimeSpan value1(TimeSpan::FromDays(4) + TimeSpan::FromHours(3) + TimeSpan::FromMinutes(2) + TimeSpan::FromSeconds(1));
  TimeSpan value2(value1 + TimeSpan::FromMilliseconds(337));

  EXPECT_EQ(std::string("5.00:00:00"), fmt::format("{}", value0));
  EXPECT_EQ(std::string("4.03:02:01"), fmt::format("{}", value1));
  EXPECT_EQ(std::string("4.03:02:01.3370000"), fmt::format("{}", value2));
}


TEST(TestLog_Time_FmtTimeSpan, Log_Hours)
{
  TimeSpan value(TimeSpan::FromHours(5));

  EXPECT_EQ(std::string("05:00:00"), fmt::format("{}", value));
}


TEST(TestLog_Time_FmtTimeSpan, Log_Minutes)
{
  TimeSpan value(TimeSpan::FromMinutes(5));

  EXPECT_EQ(std::string("00:05:00"), fmt::format("{}", value));
}


TEST(TestLog_Time_FmtTimeSpan, Log_Seconds)
{
  TimeSpan value(TimeSpan::FromSeconds(5));

  EXPECT_EQ(std::string("00:00:05"), fmt::format("{}", value));
}

TEST(TestLog_Time_FmtTimeSpan, Log_Milliseconds)
{
  TimeSpan value(TimeSpan::FromMilliseconds(5));
  EXPECT_EQ(std::string("00:00:00.0050000"), fmt::format("{}", value));
}


TEST(TestLog_Time_FmtTimeSpan, Log_Microseconds)
{
  TimeSpan value(TimeSpan::FromMicroseconds(5));
  EXPECT_EQ(std::string("00:00:00.0000050"), fmt::format("{}", value));
}


TEST(TestLog_Time_FmtTimeSpan, Log_NanoSeconds)
{
  TimeSpan value(TimeSpan::FromNanoseconds(500));
  EXPECT_EQ(std::string("00:00:00.0000005"), fmt::format("{}", value));
}
