/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

#include <FslBase/Log/Time/LogTimeSpan.hpp>
#include <FslBase/System/HighResolutionTimer.hpp>
#include <FslBase/System/HighResolutionTimerUtil.hpp>
#include <FslBase/System/Threading/Thread.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <limits>

using namespace Fsl;

namespace
{
  using TestSystem_HighResolutionTimerUtil = TestFixtureFslBase;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSystem_HighResolutionTimerUtil, ToTimeSpan)
{
  HighResolutionTimer timer;

  auto startTime = timer.GetNativeTicks();
  Thread::SleepMilliseconds(2);
  auto endTime = timer.GetNativeTicks();

  TimeSpan timeSpan = HighResolutionTimerUtil::ToTimeSpan(endTime - startTime, timer.GetNativeTickFrequency());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSystem_HighResolutionTimerUtil, ToTimeSpanKnown)
{
  // 10000 / 100 = 100seconds
  TimeSpan timeSpan = HighResolutionTimerUtil::ToTimeSpan(10000, 100);

  EXPECT_EQ(timeSpan, TimeSpan::FromSeconds(100));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSystem_HighResolutionTimerUtil, ToTimeSpanKnown2)
{
  // 10000 / 100 = 10000 seconds
  TimeSpan timeSpan = HighResolutionTimerUtil::ToTimeSpan(10000, 1);

  EXPECT_EQ(timeSpan, TimeSpan::FromSeconds(10000));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
