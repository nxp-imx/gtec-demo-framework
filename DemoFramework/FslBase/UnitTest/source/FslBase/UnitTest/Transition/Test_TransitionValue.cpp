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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Transition/TransitionCache.hpp>
#include <FslBase/Transition/TransitionValue.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestTransition_TransitionValue = TestFixtureFslBase;
}


TEST(TestTransition_TransitionValue, Construct_Default)
{
  TransitionValue transitionValue;

  EXPECT_TRUE(transitionValue.IsCompleted());
  EXPECT_EQ(TimeSpan(0), transitionValue.GetStartDelay());
  EXPECT_EQ(0.0f, transitionValue.GetValue());
  EXPECT_EQ(0.0f, transitionValue.GetActualValue());
  EXPECT_EQ(TimeSpan(0), transitionValue.GetTransitionTime());
}


TEST(TestTransition_TransitionValue, Construct_CacheAndTimespan)
{
  TransitionCache cache;
  TransitionValue transitionValue(cache, TimeSpan(10));

  EXPECT_TRUE(transitionValue.IsCompleted());
  EXPECT_EQ(TimeSpan(0), transitionValue.GetStartDelay());
  EXPECT_EQ(0.0f, transitionValue.GetValue());
  EXPECT_EQ(0.0f, transitionValue.GetActualValue());
  EXPECT_EQ(TimeSpan(10), transitionValue.GetTransitionTime());
}


TEST(TestTransition_TransitionValue, SetTransitionTime)
{
  TransitionCache cache;
  TransitionValue transitionValue;

  EXPECT_TRUE(transitionValue.IsCompleted());
  EXPECT_EQ(TimeSpan(0), transitionValue.GetStartDelay());
  EXPECT_EQ(0.0f, transitionValue.GetValue());
  EXPECT_EQ(0.0f, transitionValue.GetActualValue());
  EXPECT_EQ(TimeSpan(0), transitionValue.GetTransitionTime());

  transitionValue.SetTransitionTime(cache, TimeSpan(10));

  EXPECT_TRUE(transitionValue.IsCompleted());
  EXPECT_EQ(TimeSpan(0), transitionValue.GetStartDelay());
  EXPECT_EQ(0.0f, transitionValue.GetValue());
  EXPECT_EQ(0.0f, transitionValue.GetActualValue());
  EXPECT_EQ(TimeSpan(10), transitionValue.GetTransitionTime());
}

TEST(TestTransition_TransitionValue, SetValue)
{
  TransitionCache cache;
  TransitionValue transitionValue(cache, TimeSpan(2), TransitionType::Linear);

  const float value = 42.0f;

  EXPECT_TRUE(transitionValue.IsCompleted());
  transitionValue.SetValue(value);

  EXPECT_FALSE(transitionValue.IsCompleted());
  EXPECT_EQ(0.0f, transitionValue.GetValue());
  // We do expect to hit the exact provided value (so no float eq here)
  EXPECT_EQ(value, transitionValue.GetActualValue());

  transitionValue.Update(TimeSpan(1));
  EXPECT_FALSE(transitionValue.IsCompleted());
  EXPECT_FLOAT_EQ(value * 0.5f, transitionValue.GetValue());
  // We do expect to hit the exact provided value (so no float eq here)
  EXPECT_EQ(value, transitionValue.GetActualValue());

  transitionValue.Update(TimeSpan(1));
  EXPECT_TRUE(transitionValue.IsCompleted());
  // We do expect to hit the exact provided value (so no float eq here)
  EXPECT_EQ(value, transitionValue.GetValue());
  EXPECT_EQ(value, transitionValue.GetActualValue());

  // Calling update after the end time should change nothing
  transitionValue.Update(TimeSpan(1));
  EXPECT_TRUE(transitionValue.IsCompleted());
  // We do expect to hit the exact provided value (so no float eq here)
  EXPECT_EQ(value, transitionValue.GetValue());
  EXPECT_EQ(value, transitionValue.GetActualValue());
}


TEST(TestTransition_TransitionValue, SetActualValue)
{
  TransitionCache cache;
  TransitionValue transitionValue(cache, TimeSpan(2), TransitionType::Linear);

  const float value = 42.0f;
  const float overrideValue = 80.0f;

  EXPECT_TRUE(transitionValue.IsCompleted());
  transitionValue.SetValue(value);

  EXPECT_FALSE(transitionValue.IsCompleted());
  EXPECT_EQ(0.0f, transitionValue.GetValue());
  // We do expect to hit the exact provided value (so no float eq here)
  EXPECT_EQ(value, transitionValue.GetActualValue());

  transitionValue.Update(TimeSpan(1));
  EXPECT_FALSE(transitionValue.IsCompleted());
  EXPECT_FLOAT_EQ(value * 0.5f, transitionValue.GetValue());
  // We do expect to hit the exact provided value (so no float eq here)
  EXPECT_EQ(value, transitionValue.GetActualValue());

  // Override the transition by forcing the actual value
  transitionValue.SetActualValue(overrideValue);
  EXPECT_TRUE(transitionValue.IsCompleted());
  // We do expect to hit the exact provided value (so no float eq here)
  EXPECT_EQ(overrideValue, transitionValue.GetValue());
  EXPECT_EQ(overrideValue, transitionValue.GetActualValue());
}


TEST(TestTransition_TransitionValue, SetStartDelay)
{
  TransitionCache cache;
  TransitionValue transitionValue(cache, TimeSpan(2), TransitionType::Linear);

  const float value = 42.0f;

  EXPECT_TRUE(transitionValue.IsCompleted());
  EXPECT_EQ(TimeSpan(0), transitionValue.GetStartDelay());
  transitionValue.SetStartDelay(TimeSpan(3));
  EXPECT_EQ(TimeSpan(3), transitionValue.GetStartDelay());
  EXPECT_TRUE(transitionValue.IsCompleted());
  transitionValue.SetValue(value);

  EXPECT_FALSE(transitionValue.IsCompleted());
  EXPECT_EQ(0.0f, transitionValue.GetValue());
  // We do expect to hit the exact provided value (so no float eq here)
  EXPECT_EQ(value, transitionValue.GetActualValue());

  // Start delay
  for (int i = 0; i < 3; ++i)
  {
    transitionValue.Update(TimeSpan(1));
    EXPECT_FALSE(transitionValue.IsCompleted());
    EXPECT_EQ(0.0f, transitionValue.GetValue());
    EXPECT_EQ(value, transitionValue.GetActualValue());
  }

  transitionValue.Update(TimeSpan(1));
  EXPECT_FALSE(transitionValue.IsCompleted());
  EXPECT_FLOAT_EQ(value * 0.5f, transitionValue.GetValue());
  // We do expect to hit the exact provided value (so no float eq here)
  EXPECT_EQ(value, transitionValue.GetActualValue());

  transitionValue.Update(TimeSpan(1));
  EXPECT_TRUE(transitionValue.IsCompleted());
  // We do expect to hit the exact provided value (so no float eq here)
  EXPECT_EQ(value, transitionValue.GetValue());
  EXPECT_EQ(value, transitionValue.GetActualValue());
}
