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

#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslSimpleUI/Base/Control/Logic/SliderConstrainedValue.hpp>

using namespace Fsl;

namespace
{
  using TestControlLogic_SliderConstrainedValue_Float = TestFixtureFslBase;
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_Default)
{
  UI::SliderConstrainedValue<float> value;

  EXPECT_EQ(0, value.Min());
  EXPECT_EQ(0, value.Max());
  EXPECT_EQ(0, value.Get());
  EXPECT_EQ(0, value.Center());
  EXPECT_EQ(0, value.TickFrequency());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_MinMax)
{
  const float min = 10;
  const float max = 15;
  UI::SliderConstrainedValue<float> value(min, max);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(min, value.Get());
  EXPECT_EQ(((max - min) / 2) + min, value.Center());
  EXPECT_EQ(0, value.TickFrequency());
}

TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_MinMax_Invalid)
{
  const float min = 10;
  const float max = 15;
  UI::SliderConstrainedValue<float> value(max, min);

  EXPECT_EQ(max, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(max, value.Get());
  EXPECT_EQ(max, value.Center());
  EXPECT_EQ(0, value.TickFrequency());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_ValueMinMax)
{
  const float val = 12;
  const float min = 10;
  const float max = 15;
  UI::SliderConstrainedValue<float> value(val, min, max);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(val, value.Get());
  EXPECT_EQ(0, value.TickFrequency());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_ValueMinMax_InvalidMinMax)
{
  const float val = 12;
  const float min = 10;
  const float max = 15;
  UI::SliderConstrainedValue<float> value(val, max, min);

  EXPECT_EQ(max, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(max, value.Get());
  EXPECT_EQ(0, value.TickFrequency());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_ValueMinMax_ValueOutOfBounds_Low)
{
  const float val = 9;
  const float min = 10;
  const float max = 15;
  UI::SliderConstrainedValue<float> value(val, min, max);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(min, value.Get());
  EXPECT_EQ(0, value.TickFrequency());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_ValueMinMax_ValueOutOfBounds_High)
{
  const float val = 16;
  const float min = 10;
  const float max = 15;
  UI::SliderConstrainedValue<float> value(val, min, max);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(max, value.Get());
  EXPECT_EQ(0, value.TickFrequency());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_ValueMinMaxTickFrequency)
{
  const float val = 12;
  const float min = 10;
  const float max = 15;
  const float tickFrequency = 2;
  UI::SliderConstrainedValue<float> value(val, min, max, tickFrequency);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(val, value.Get());
  EXPECT_EQ(tickFrequency, value.TickFrequency());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_ValueMinMaxTickFrequency_TickFrequencyOutOfBounds_Low0)
{
  const float val = 12;
  const float min = 10;
  const float max = 15;
  UI::SliderConstrainedValue<float> value(val, min, max, 0);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(val, value.Get());
  EXPECT_EQ(0, value.TickFrequency());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_ValueMinMaxTickFrequency_TickFrequencyOutOfBounds_Low1)
{
  const float val = 12;
  const float min = 10;
  const float max = 15;
  UI::SliderConstrainedValue<float> value(val, min, max, -1);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(val, value.Get());
  EXPECT_EQ(0, value.TickFrequency());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_ValueMinMaxTickFrequency_TickFrequencyOutOfBounds_Low2)
{
  const float val = 12;
  const float min = 10;
  const float max = 15;
  UI::SliderConstrainedValue<float> value(val, min, max, std::numeric_limits<float>::lowest());

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(val, value.Get());
  EXPECT_EQ(0, value.TickFrequency());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_ValueMinMaxTickFrequency_TickFrequencyOutOfBounds_High)
{
  const float val = 12;
  const float min = 10;
  const float max = 15;
  UI::SliderConstrainedValue<float> value(val, min, max, max + 1);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(min, value.Get());
  EXPECT_EQ(max, value.TickFrequency());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_ValueMinMaxTickFrequency_TickFrequencyOutOfBounds_High2)
{
  const float val = 12;
  const float min = 10;
  const float max = 15;
  UI::SliderConstrainedValue<float> value(val, min, max, std::numeric_limits<float>::max());

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(min, value.Get());
  EXPECT_EQ(max, value.TickFrequency());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_ValueMinMaxTickFrequency_ValueSnapped)
{
  const float val = 14;
  const int32_t min = 10;
  const int32_t max = 20;
  const int32_t tickFrequency = 5;
  UI::SliderConstrainedValue<float> value(val, min, max, tickFrequency);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(15, value.Get());
  EXPECT_EQ(tickFrequency, value.TickFrequency());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_ValueMinMaxTickFrequency_ValueSnapped_Float)
{
  const float val = 14;
  const float min = 10;
  const float max = 20;
  const float tickFrequency = 5;
  UI::SliderConstrainedValue<float> value(val, min, max, tickFrequency);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(15.0f, value.Get());
  EXPECT_EQ(tickFrequency, value.TickFrequency());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Construct_ValueMinMaxTickFrequency_ValueSnapped_Double)
{
  const double val = 14;
  const double min = 10;
  const double max = 20;
  const double tickFrequency = 5;
  UI::SliderConstrainedValue<double> value(val, min, max, tickFrequency);

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(15.0, value.Get());
  EXPECT_EQ(tickFrequency, value.TickFrequency());
}

TEST(TestControlLogic_SliderConstrainedValue_Float, SetRange)
{
  UI::SliderConstrainedValue<float> value;

  EXPECT_EQ(0, value.Min());
  EXPECT_EQ(0, value.Max());
  EXPECT_EQ(0, value.Get());

  const float min = 10;
  const float max = 15;
  EXPECT_TRUE(value.SetRange(min, max));

  EXPECT_EQ(min, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(min, value.Get());

  EXPECT_FALSE(value.SetRange(min, max));
}

TEST(TestControlLogic_SliderConstrainedValue_Float, SetRange_Invalid)
{
  UI::SliderConstrainedValue<float> value;

  EXPECT_EQ(0, value.Min());
  EXPECT_EQ(0, value.Max());
  EXPECT_EQ(0, value.Get());

  const float min = 10;
  const float max = 15;
  value.SetRange(max, min);

  EXPECT_EQ(max, value.Min());
  EXPECT_EQ(max, value.Max());
  EXPECT_EQ(max, value.Get());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Set_SameValue)
{
  const float min = 10;
  const float max = 15;
  UI::SliderConstrainedValue<float> value(min, max);
  EXPECT_EQ(min, value.Get());

  // Setting the value to the same should do nothing
  EXPECT_FALSE(value.Set(10));

  EXPECT_EQ(min, value.Get());
}

TEST(TestControlLogic_SliderConstrainedValue_Float, Set_NewValue_InRange)
{
  const float min = 10;
  const float max = 15;
  UI::SliderConstrainedValue<float> value(min, max);
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


TEST(TestControlLogic_SliderConstrainedValue_Float, Set_NewValue_Below)
{
  const float min = 10;
  const float max = 15;
  UI::SliderConstrainedValue<float> value(min, max);
  {
    EXPECT_TRUE(value.Set(max));

    EXPECT_TRUE(value.Set(min - 1));
    EXPECT_EQ(min, value.Get());
  }
  {
    EXPECT_TRUE(value.Set(max));

    EXPECT_TRUE(value.Set(std::numeric_limits<float>::min()));
    EXPECT_EQ(min, value.Get());
  }
}


TEST(TestControlLogic_SliderConstrainedValue_Float, Set_NewValue_Above)
{
  const float min = 10;
  const float max = 15;
  UI::SliderConstrainedValue<float> value(min, max);
  EXPECT_TRUE(value.Set(min + 1));

  {
    EXPECT_TRUE(value.Set(min));

    EXPECT_TRUE(value.Set(max + 1));
    EXPECT_EQ(max, value.Get());
  }
  {
    EXPECT_TRUE(value.Set(min));

    EXPECT_TRUE(value.Set(std::numeric_limits<float>::max()));
    EXPECT_EQ(max, value.Get());
  }
}

TEST(TestControlLogic_SliderConstrainedValue_Float, SetPercentage)
{
  const float min = 100;
  const float max = 200;
  UI::SliderConstrainedValue<float> value(min, max);

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

TEST(TestControlLogic_SliderConstrainedValue_Float, SetPercentage_DecimalRange)
{
  const float min = 0.0f;
  const float max = 1.0f;
  UI::SliderConstrainedValue<float> value(min, max);

  EXPECT_TRUE(value.SetPercentage(1.0f));
  EXPECT_EQ(1.0f, value.GetPercentage());
  EXPECT_EQ(max, value.Get());

  EXPECT_TRUE(value.SetPercentage(0.5f));
  EXPECT_EQ(0.5f, value.GetPercentage());
  EXPECT_FLOAT_EQ(min + ((max - min) / 2.0f), value.Get());

  EXPECT_TRUE(value.SetPercentage(0.0f));
  EXPECT_EQ(0.0f, value.GetPercentage());
  EXPECT_EQ(min, value.Get());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, SetPercentage_OutOfRange)
{
  const float min = 100;
  const float max = 200;
  UI::SliderConstrainedValue<float> value(min, max);

  EXPECT_TRUE(value.SetPercentage(std::numeric_limits<float>::max()));
  EXPECT_EQ(1.0f, value.GetPercentage());
  EXPECT_EQ(max, value.Get());

  EXPECT_TRUE(value.SetPercentage(std::numeric_limits<float>::min()));
  EXPECT_EQ(0.0f, value.GetPercentage());
  EXPECT_EQ(min, value.Get());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, GetPercentage)
{
  const float min = 100;
  const float max = 200;
  UI::SliderConstrainedValue<float> value(min, max);

  EXPECT_TRUE(value.SetPercentage(1.0f));
  EXPECT_EQ(1.0f, value.GetPercentage());
  EXPECT_TRUE(value.SetPercentage(0.5f));
  EXPECT_EQ(0.5f, value.GetPercentage());
  EXPECT_TRUE(value.SetPercentage(0.0f));
  EXPECT_EQ(0.0f, value.GetPercentage());
}


TEST(TestControlLogic_SliderConstrainedValue_Float, OpEqual)
{
  EXPECT_TRUE(UI::SliderConstrainedValue<float>(1, 2, 9, 3) == UI::SliderConstrainedValue<float>(1, 2, 9, 3));
  EXPECT_FALSE(UI::SliderConstrainedValue<float>(1, 2, 9, 3) == UI::SliderConstrainedValue<float>(7, 2, 9, 3));
  EXPECT_FALSE(UI::SliderConstrainedValue<float>(1, 2, 9, 3) == UI::SliderConstrainedValue<float>(1, 7, 9, 3));
  EXPECT_FALSE(UI::SliderConstrainedValue<float>(1, 2, 9, 3) == UI::SliderConstrainedValue<float>(1, 2, 7, 3));
  EXPECT_FALSE(UI::SliderConstrainedValue<float>(1, 2, 9, 3) == UI::SliderConstrainedValue<float>(1, 2, 9, 7));
}


TEST(TestControlLogic_SliderConstrainedValue_Float, OpNotEqual)
{
  EXPECT_FALSE(UI::SliderConstrainedValue<float>(1, 2, 9, 3) != UI::SliderConstrainedValue<float>(1, 2, 9, 3));
  EXPECT_TRUE(UI::SliderConstrainedValue<float>(1, 2, 9, 3) != UI::SliderConstrainedValue<float>(7, 2, 9, 3));
  EXPECT_TRUE(UI::SliderConstrainedValue<float>(1, 2, 9, 3) != UI::SliderConstrainedValue<float>(1, 7, 9, 3));
  EXPECT_TRUE(UI::SliderConstrainedValue<float>(1, 2, 9, 3) != UI::SliderConstrainedValue<float>(1, 2, 7, 3));
  EXPECT_TRUE(UI::SliderConstrainedValue<float>(1, 2, 9, 3) != UI::SliderConstrainedValue<float>(1, 2, 9, 7));
}
