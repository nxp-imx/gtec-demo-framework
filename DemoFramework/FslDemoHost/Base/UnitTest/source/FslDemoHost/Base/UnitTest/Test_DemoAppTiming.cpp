/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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
#include <FslDemoHost/Base/DemoAppTiming.hpp>

using namespace Fsl;

#if 0
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_CHECK_DELTATIME(X, Y) EXPECT_FLOAT_EQ((X), (Y))
#else
// FIX: until we get higher precission on the delta time calc this will have to do
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_CHECK_DELTATIME(X, Y) EXPECT_NEAR((X), (Y), 0.0000001f)
#endif

namespace
{
  using Test_DemoAppTiming = TestFixtureFslBase;

  namespace LocalConfig
  {
    constexpr TimeSpan FixedTime1(100000);
    constexpr TimeSpan FixedTime2(300000);
  }

  constexpr float CalcDeltaTime(const TimeSpan time) noexcept
  {
    return static_cast<float>(static_cast<double>(time.Ticks()) / static_cast<double>(TimeInfo::TicksPerSecond));
  }

  constexpr float CalcDeltaTimeFromFixedUpdatesperSecond(const uint16_t updatesPerSecond) noexcept
  {
    return updatesPerSecond > 0 ? 1.0f / static_cast<float>(updatesPerSecond) : 0.0f;
  }

  constexpr TimeSpan CalcDeltaTimeSpanFromFixedUpdatesPerSecond(const uint16_t updatesPerSecond) noexcept
  {
    return TimeSpan(updatesPerSecond > 0
                      ? static_cast<int64_t>(std::round(static_cast<double>(TimeInfo::TicksPerSecond) / static_cast<double>(updatesPerSecond)))
                      : static_cast<int64_t>(0));
  }
}

// In force update mode the TimeNow and AdvanceTimestep function always advance by the forceUpdateTime

TEST(Test_DemoAppTiming, Construct)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  // We dont expect there to be any pending fixed update after creation and no time has passed
  EXPECT_FALSE(timing.HasPendingFixedUpdate());
}


TEST(Test_DemoAppTiming, Construct_ForcedTime)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan(100000));
  // We dont expect there to be any pending fixed update after creation and no time has passed
  EXPECT_FALSE(timing.HasPendingFixedUpdate());
}


TEST(Test_DemoAppTiming, AdvanceFixedTimeStep)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  timing.AdvanceFixedTimeStep();

  const auto expectedDeltaTime = CalcDeltaTimeFromFixedUpdatesperSecond(timing.GetFixedUpdatesPerSecond());

  auto updateTime = timing.GetUpdateTime();
  EXPECT_EQ(updateTime.ElapsedTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
  EXPECT_EQ(updateTime.TotalTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
  LOCAL_CHECK_DELTATIME(updateTime.DeltaTime, expectedDeltaTime);
  EXPECT_TRUE(timing.HasPendingFixedUpdate());
}


TEST(Test_DemoAppTiming, AdvanceFixedTimeStep_120)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  timing.SetFixedUpdatesPerSecond(120);
  timing.AdvanceFixedTimeStep();

  const auto expectedDeltaTime = CalcDeltaTimeFromFixedUpdatesperSecond(timing.GetFixedUpdatesPerSecond());

  auto updateTime = timing.GetUpdateTime();
  EXPECT_EQ(updateTime.ElapsedTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
  EXPECT_EQ(updateTime.TotalTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
  LOCAL_CHECK_DELTATIME(updateTime.DeltaTime, expectedDeltaTime);
  EXPECT_TRUE(timing.HasPendingFixedUpdate());
}

TEST(Test_DemoAppTiming, AdvanceFixedTimeStep_240)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  timing.SetFixedUpdatesPerSecond(240);
  timing.AdvanceFixedTimeStep();

  const auto expectedDeltaTime = CalcDeltaTimeFromFixedUpdatesperSecond(timing.GetFixedUpdatesPerSecond());

  auto updateTime = timing.GetUpdateTime();
  EXPECT_EQ(updateTime.ElapsedTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
  EXPECT_EQ(updateTime.TotalTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
  LOCAL_CHECK_DELTATIME(updateTime.DeltaTime, expectedDeltaTime);
  EXPECT_TRUE(timing.HasPendingFixedUpdate());
}

TEST(Test_DemoAppTiming, AdvanceFixedTimeStep_360)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  timing.SetFixedUpdatesPerSecond(360);
  timing.AdvanceFixedTimeStep();

  const auto expectedDeltaTime = CalcDeltaTimeFromFixedUpdatesperSecond(timing.GetFixedUpdatesPerSecond());

  auto updateTime = timing.GetUpdateTime();
  EXPECT_EQ(updateTime.ElapsedTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
  EXPECT_EQ(updateTime.TotalTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
  LOCAL_CHECK_DELTATIME(updateTime.DeltaTime, expectedDeltaTime);
  EXPECT_TRUE(timing.HasPendingFixedUpdate());
}

TEST(Test_DemoAppTiming, AdvanceFixedTimeStep_900)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  timing.SetFixedUpdatesPerSecond(900);
  timing.AdvanceFixedTimeStep();

  const auto expectedDeltaTime = CalcDeltaTimeFromFixedUpdatesperSecond(timing.GetFixedUpdatesPerSecond());

  auto updateTime = timing.GetUpdateTime();
  EXPECT_EQ(updateTime.ElapsedTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
  EXPECT_EQ(updateTime.TotalTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
  LOCAL_CHECK_DELTATIME(updateTime.DeltaTime, expectedDeltaTime);
  EXPECT_TRUE(timing.HasPendingFixedUpdate());
}

TEST(Test_DemoAppTiming, AdvanceFixedTimeStep_1000)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  timing.SetFixedUpdatesPerSecond(1000);
  timing.AdvanceFixedTimeStep();

  const auto expectedDeltaTime = CalcDeltaTimeFromFixedUpdatesperSecond(timing.GetFixedUpdatesPerSecond());

  auto updateTime = timing.GetUpdateTime();
  EXPECT_EQ(updateTime.ElapsedTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
  EXPECT_EQ(updateTime.TotalTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
  LOCAL_CHECK_DELTATIME(updateTime.DeltaTime, expectedDeltaTime);
  EXPECT_TRUE(timing.HasPendingFixedUpdate());
}

TEST(Test_DemoAppTiming, AdvanceFixedTimeStep_ForcedTime1)
{
  const TimeSpan forcedUpdateTime(100000);
  DemoAppTiming timing(LocalConfig::FixedTime1, forcedUpdateTime);
  timing.AdvanceFixedTimeStep();

  auto updateTime = timing.GetUpdateTime();
  EXPECT_EQ(updateTime.ElapsedTime, forcedUpdateTime);
  EXPECT_EQ(updateTime.TotalTime, forcedUpdateTime);
  LOCAL_CHECK_DELTATIME(updateTime.DeltaTime, CalcDeltaTime(forcedUpdateTime));
  EXPECT_FALSE(timing.HasPendingFixedUpdate());
}


TEST(Test_DemoAppTiming, AdvanceFixedTimeStep_ForcedTime2)
{
  const TimeSpan forcedUpdateTime(200000);
  DemoAppTiming timing(LocalConfig::FixedTime1, forcedUpdateTime);
  timing.AdvanceFixedTimeStep();

  auto updateTime = timing.GetUpdateTime();
  EXPECT_EQ(updateTime.ElapsedTime, forcedUpdateTime);
  EXPECT_EQ(updateTime.TotalTime, forcedUpdateTime);
  LOCAL_CHECK_DELTATIME(updateTime.DeltaTime, CalcDeltaTime(forcedUpdateTime));
  EXPECT_TRUE(timing.HasPendingFixedUpdate());
}


TEST(Test_DemoAppTiming, GetFixedUpdatesPerSecond)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  EXPECT_EQ(timing.GetFixedUpdatesPerSecond(), 60);
}


TEST(Test_DemoAppTiming, SetFixedUpdatesPerSecond)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  EXPECT_TRUE(timing.SetFixedUpdatesPerSecond(30));
  EXPECT_EQ(timing.GetFixedUpdatesPerSecond(), 30);
}


TEST(Test_DemoAppTiming, GetUpdateTime_AfterConstruct)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  auto updateTime = timing.GetUpdateTime();

  EXPECT_EQ(updateTime.ElapsedTime.Ticks(), 0);
  EXPECT_EQ(updateTime.TotalTime.Ticks(), 0);
  EXPECT_FLOAT_EQ(updateTime.DeltaTime, 0.0f);
}

TEST(Test_DemoAppTiming, GetUpdateTime_AdvanceFixedTimeStep)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  timing.AdvanceFixedTimeStep();
  auto updateTime = timing.GetUpdateTime();
  EXPECT_EQ(updateTime.ElapsedTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
  EXPECT_EQ(updateTime.TotalTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
  LOCAL_CHECK_DELTATIME(updateTime.DeltaTime, CalcDeltaTimeFromFixedUpdatesperSecond(timing.GetFixedUpdatesPerSecond()));
}


TEST(Test_DemoAppTiming, GetUpdateTime_AfterConstruct_ForcedUpdateTime)
{
  constexpr TimeSpan forcedUpdateTime(100000);
  DemoAppTiming timing(LocalConfig::FixedTime1, forcedUpdateTime);
  auto updateTime = timing.GetUpdateTime();

  EXPECT_EQ(updateTime.ElapsedTime.Ticks(), 0);
  EXPECT_EQ(updateTime.TotalTime.Ticks(), 0);
  EXPECT_FLOAT_EQ(updateTime.DeltaTime, 0.0f);
}


TEST(Test_DemoAppTiming, GetUpdateTime_AdvanceFixedTimestep_ForcedUpdateTime)
{
  constexpr TimeSpan forcedUpdateTime(100000);
  DemoAppTiming timing(LocalConfig::FixedTime1, forcedUpdateTime);
  timing.AdvanceFixedTimeStep();
  auto updateTime = timing.GetUpdateTime();

  EXPECT_EQ(updateTime.ElapsedTime, forcedUpdateTime);
  EXPECT_EQ(updateTime.TotalTime, forcedUpdateTime);
  LOCAL_CHECK_DELTATIME(updateTime.DeltaTime, CalcDeltaTime(forcedUpdateTime));
}


TEST(Test_DemoAppTiming, ResetTimer_GetUpdateTime)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  timing.ResetTimer(LocalConfig::FixedTime2);

  auto updateTime = timing.GetUpdateTime();
  EXPECT_EQ(updateTime.ElapsedTime.Ticks(), 0);
  EXPECT_EQ(updateTime.TotalTime.Ticks(), 0);
  EXPECT_FLOAT_EQ(updateTime.DeltaTime, 0.0f);

  EXPECT_FALSE(timing.TryFixedUpdate().has_value());
}


TEST(Test_DemoAppTiming, ResetTimer_TryFixedUpdate)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  EXPECT_FALSE(timing.HasPendingFixedUpdate());

  // Reset the timer
  timing.ResetTimer(LocalConfig::FixedTime2);

  EXPECT_FALSE(timing.HasPendingFixedUpdate());
  EXPECT_FALSE(timing.TryFixedUpdate().has_value());
}


TEST(Test_DemoAppTiming, ResetTimer_MultipleResets)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  EXPECT_FALSE(timing.HasPendingFixedUpdate());

  timing.ResetTimer(LocalConfig::FixedTime2);
  timing.ResetTimer(LocalConfig::FixedTime2 + TimeSpan(10));
  timing.ResetTimer(LocalConfig::FixedTime2 + TimeSpan(11));
  timing.ResetTimer(LocalConfig::FixedTime2 + TimeSpan(12));
  timing.ResetTimer(LocalConfig::FixedTime2 + TimeSpan(13));

  auto updateTime = timing.GetUpdateTime();
  EXPECT_EQ(updateTime.ElapsedTime.Ticks(), 0);
  EXPECT_EQ(updateTime.TotalTime.Ticks(), 0);
  EXPECT_FLOAT_EQ(updateTime.DeltaTime, 0.0f);

  EXPECT_FALSE(timing.HasPendingFixedUpdate());
  EXPECT_FALSE(timing.TryFixedUpdate().has_value());
}

TEST(Test_DemoAppTiming, TimeNow_Normal)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  timing.TimeNow(LocalConfig::FixedTime2, TimeStepMode::Normal);
  {
    const TimeSpan deltaTime(LocalConfig::FixedTime2 - LocalConfig::FixedTime1);

    auto updateTime = timing.GetUpdateTime();
    EXPECT_EQ(updateTime.ElapsedTime, deltaTime);
    EXPECT_EQ(updateTime.TotalTime, deltaTime);
    LOCAL_CHECK_DELTATIME(updateTime.DeltaTime, CalcDeltaTime(deltaTime));

    const auto fixedUpdateTime = CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond());

    ASSERT_GT(fixedUpdateTime.Ticks(), 0);
    // NOLINTNEXTLINE(clang-analyzer-core.DivideZero)
    const auto numFixedUpdates = NumericCast<int32_t>(deltaTime.Ticks() / fixedUpdateTime.Ticks());
    EXPECT_EQ(numFixedUpdates, 1);

    EXPECT_TRUE(timing.HasPendingFixedUpdate());
    auto fixedUpdate = timing.TryFixedUpdate();
    ASSERT_TRUE(fixedUpdate.has_value());
    EXPECT_EQ(fixedUpdate.value().ElapsedTime, fixedUpdateTime);
    EXPECT_EQ(fixedUpdate.value().TotalTime, (numFixedUpdates * fixedUpdateTime));
    LOCAL_CHECK_DELTATIME(fixedUpdate.value().DeltaTime, CalcDeltaTime(fixedUpdateTime));
    EXPECT_FALSE(timing.HasPendingFixedUpdate());
  }
}

TEST(Test_DemoAppTiming, TimeNow_Normal_ForcedUpdateTime1)
{
  constexpr TimeSpan forcedUpdateTime(100000);
  DemoAppTiming timing(LocalConfig::FixedTime1, forcedUpdateTime);
  timing.TimeNow(LocalConfig::FixedTime2, TimeStepMode::Normal);
  {
    auto updateTime = timing.GetUpdateTime();
    EXPECT_EQ(updateTime.ElapsedTime, forcedUpdateTime);
    EXPECT_EQ(updateTime.TotalTime, forcedUpdateTime);
    LOCAL_CHECK_DELTATIME(updateTime.DeltaTime, CalcDeltaTime(forcedUpdateTime));

    const auto fixedUpdateTime = CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond());

    ASSERT_GT(fixedUpdateTime.Ticks(), 0);
    // NOLINTNEXTLINE(clang-analyzer-core.DivideZero)
    const uint64_t numFixedUpdates = (forcedUpdateTime.Ticks() / fixedUpdateTime.Ticks());
    EXPECT_EQ(numFixedUpdates, 0u);

    EXPECT_FALSE(timing.HasPendingFixedUpdate());
    EXPECT_FALSE(timing.TryFixedUpdate().has_value());
  }
}

TEST(Test_DemoAppTiming, TimeNow_Normal_ForcedUpdateTime2)
{
  constexpr TimeSpan forcedUpdateTime(200000);
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan(forcedUpdateTime));
  timing.TimeNow(LocalConfig::FixedTime2, TimeStepMode::Normal);
  {
    auto updateTime = timing.GetUpdateTime();
    EXPECT_EQ(updateTime.ElapsedTime, forcedUpdateTime);
    EXPECT_EQ(updateTime.TotalTime, forcedUpdateTime);
    LOCAL_CHECK_DELTATIME(updateTime.DeltaTime, CalcDeltaTime(forcedUpdateTime));

    const auto fixedUpdateTime = CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond());

    ASSERT_GT(fixedUpdateTime.Ticks(), 0);
    // NOLINTNEXTLINE(clang-analyzer-core.DivideZero)
    const auto numFixedUpdates = NumericCast<int32_t>(forcedUpdateTime.Ticks() / fixedUpdateTime.Ticks());
    EXPECT_EQ(numFixedUpdates, 1);

    EXPECT_TRUE(timing.HasPendingFixedUpdate());
    auto fixedUpdate = timing.TryFixedUpdate();
    ASSERT_TRUE(fixedUpdate.has_value());
    EXPECT_EQ(fixedUpdate.value().ElapsedTime, fixedUpdateTime);
    EXPECT_EQ(fixedUpdate.value().TotalTime, (numFixedUpdates * fixedUpdateTime));
    LOCAL_CHECK_DELTATIME(fixedUpdate.value().DeltaTime, CalcDeltaTime(fixedUpdateTime));
    EXPECT_FALSE(timing.HasPendingFixedUpdate());
  }
}


TEST(Test_DemoAppTiming, TimeNow_AdvanceTimestep_Normal)
{
  DemoAppTiming timing(LocalConfig::FixedTime1, TimeSpan());
  timing.AdvanceFixedTimeStep();

  const auto baseTotalTime = timing.GetUpdateTime().TotalTime;
  TimeSpan baseFixedTotalTime;
  {
    auto fixedUpdate = timing.TryFixedUpdate();
    // We always expect that a newly created object will have a fixed update ready
    EXPECT_TRUE(fixedUpdate.has_value());
    EXPECT_EQ(fixedUpdate.value().ElapsedTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
    EXPECT_EQ(fixedUpdate.value().TotalTime, CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond()));
    LOCAL_CHECK_DELTATIME(fixedUpdate.value().DeltaTime, CalcDeltaTimeFromFixedUpdatesperSecond(timing.GetFixedUpdatesPerSecond()));
    baseFixedTotalTime = fixedUpdate.value().TotalTime;
    EXPECT_FALSE(timing.HasPendingFixedUpdate());
  }

  timing.TimeNow(LocalConfig::FixedTime2, TimeStepMode::Normal);
  {
    const TimeSpan deltaTime(LocalConfig::FixedTime2 - LocalConfig::FixedTime1);

    auto updateTime = timing.GetUpdateTime();
    EXPECT_EQ(updateTime.ElapsedTime, deltaTime);
    EXPECT_EQ(updateTime.TotalTime, baseTotalTime + deltaTime);
    LOCAL_CHECK_DELTATIME(updateTime.DeltaTime, CalcDeltaTime(deltaTime));

    const auto fixedUpdateTime = CalcDeltaTimeSpanFromFixedUpdatesPerSecond(timing.GetFixedUpdatesPerSecond());

    ASSERT_GT(fixedUpdateTime.Ticks(), 0);
    // NOLINTNEXTLINE(clang-analyzer-core.DivideZero)
    const auto numFixedUpdates = NumericCast<int32_t>(deltaTime.Ticks() / fixedUpdateTime.Ticks());
    EXPECT_EQ(numFixedUpdates, 1);

    auto fixedUpdate = timing.TryFixedUpdate();
    ASSERT_TRUE(fixedUpdate.has_value());
    EXPECT_EQ(fixedUpdate.value().ElapsedTime, fixedUpdateTime);
    EXPECT_EQ(fixedUpdate.value().TotalTime, baseFixedTotalTime + fixedUpdateTime);
    LOCAL_CHECK_DELTATIME(fixedUpdate.value().DeltaTime, CalcDeltaTime(fixedUpdateTime));
  }
}
