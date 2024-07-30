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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Time/LogTimeSpan.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>

using namespace Fsl;

namespace
{
  using Test_DemoTime = TestFixtureFslBase;
}

// In force update mode the TimeNow and AdvanceTimestep function always advance by the forceUpdateTime

TEST(Test_DemoTime, Construct)
{
  DemoTime demoTime;
  EXPECT_EQ(demoTime.CurrentTickCount.Ticks(), 0);
  EXPECT_EQ(demoTime.ElapsedTime.Ticks(), 0);
  EXPECT_FLOAT_EQ(demoTime.DeltaTime, 0.0f);
}

TEST(Test_DemoTime, Construct_OneSecond)
{
  constexpr auto Ticks = TimeSpan::TicksPerSecond;
  TickCount totalTimeSpan(Ticks * 2);
  TimeSpan elapsedTimeSpan(Ticks);
  DemoTime demoTime(totalTimeSpan, elapsedTimeSpan);

  EXPECT_EQ(demoTime.CurrentTickCount, totalTimeSpan);
  EXPECT_EQ(demoTime.ElapsedTime, elapsedTimeSpan);
  EXPECT_FLOAT_EQ(demoTime.DeltaTime, 1.0f);
}


TEST(Test_DemoTime, Construct_HalfASecond)
{
  constexpr auto Ticks = TimeSpan::TicksPerSecond / 2;
  TickCount totalTimeSpan(Ticks * 2);
  TimeSpan elapsedTimeSpan(Ticks);
  DemoTime demoTime(totalTimeSpan, elapsedTimeSpan);

  EXPECT_EQ(demoTime.CurrentTickCount, totalTimeSpan);
  EXPECT_EQ(demoTime.ElapsedTime, elapsedTimeSpan);
  EXPECT_FLOAT_EQ(demoTime.DeltaTime, 0.5f);
}
