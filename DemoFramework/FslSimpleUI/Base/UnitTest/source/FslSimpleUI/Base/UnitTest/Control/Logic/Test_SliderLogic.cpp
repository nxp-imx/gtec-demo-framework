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

#include <FslSimpleUI/Base/Control/Logic/SliderLogic.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestControlLogic_SliderLogic = TestFixtureFslBase;
}


TEST(TestControlLogic_SliderLogic, Construct)
{
  UI::SliderLogic<uint32_t> logic;

  EXPECT_EQ(0u, logic.Min());
  EXPECT_EQ(0u, logic.Max());
  EXPECT_EQ(0u, logic.GetValue());

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(0, logic.GetPositionPx());
  EXPECT_EQ(0u, logic.GetValue());
  EXPECT_EQ(0u, logic.GetTickFrequency());

  auto spanInfo = logic.GetSpanInfo();
  EXPECT_EQ(0, spanInfo.GetStartPx());
  EXPECT_EQ(0, spanInfo.GetLengthPx());
  EXPECT_FALSE(spanInfo.IsReversedDirection());
}

TEST(TestControlLogic_SliderLogic, Construct_Span_MinValue)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx, false));

  EXPECT_EQ(constrainedValue.Min(), logic.Min());
  EXPECT_EQ(constrainedValue.Max(), logic.Max());
  EXPECT_EQ(constrainedValue.Get(), logic.GetValue());

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(0u, logic.GetTickFrequency());

  auto spanInfo = logic.GetSpanInfo();
  EXPECT_EQ(startPx, spanInfo.GetStartPx());
  EXPECT_EQ(lengthPx, spanInfo.GetLengthPx());
  EXPECT_FALSE(spanInfo.IsReversedDirection());
}

TEST(TestControlLogic_SliderLogic, Construct_Span_MaxValue)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(2000, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx, false));

  EXPECT_EQ(constrainedValue.Min(), logic.Min());
  EXPECT_EQ(constrainedValue.Max(), logic.Max());
  EXPECT_EQ(constrainedValue.Get(), logic.GetValue());

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + lengthPx - 1, logic.GetPositionPx());
  EXPECT_EQ(0u, logic.GetTickFrequency());

  auto spanInfo = logic.GetSpanInfo();
  EXPECT_EQ(startPx, spanInfo.GetStartPx());
  EXPECT_EQ(lengthPx, spanInfo.GetLengthPx());
  EXPECT_FALSE(spanInfo.IsReversedDirection());
}


TEST(TestControlLogic_SliderLogic, Construct_Span_Value)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1250, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx, false));

  EXPECT_EQ(constrainedValue.Min(), logic.Min());
  EXPECT_EQ(constrainedValue.Max(), logic.Max());
  EXPECT_EQ(constrainedValue.Get(), logic.GetValue());

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  EXPECT_FLOAT_EQ(0.25f, logic.GetPercentage());
  EXPECT_EQ(startPx + 50, logic.GetPositionPx());
  EXPECT_EQ(0u, logic.GetTickFrequency());

  auto spanInfo = logic.GetSpanInfo();
  EXPECT_EQ(startPx, spanInfo.GetStartPx());
  EXPECT_EQ(lengthPx, spanInfo.GetLengthPx());
  EXPECT_FALSE(spanInfo.IsReversedDirection());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestControlLogic_SliderLogic, SetPercentage_InRange_MinValue)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetPercentage(1.0f));

  EXPECT_TRUE(logic.SetPercentage(0.0f));

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());

  {    // Calling it again should be a no-op
    EXPECT_FALSE(logic.SetPercentage(0.0f));

    EXPECT_EQ(0.0f, logic.GetPercentage());
    EXPECT_EQ(startPx, logic.GetPositionPx());
    EXPECT_EQ(constrainedValue.Min(), logic.GetValue());
  }
}


TEST(TestControlLogic_SliderLogic, SetPercentage_InRange_MiddleValue)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetPercentage(0.5f));

  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + ((lengthPx - 1) / 2)), logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());

  {    // Calling it again should be a no-op
    EXPECT_FALSE(logic.SetPercentage(0.5f));

    EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + ((lengthPx - 1) / 2)), logic.GetPositionPx());
    EXPECT_EQ(constrainedValue.Center(), logic.GetValue());
  }
}


TEST(TestControlLogic_SliderLogic, SetPercentage_InRange_MaxValue)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetPercentage(1.0f));

  EXPECT_EQ(1.0f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + lengthPx - 1), logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Max(), logic.GetValue());

  {    // Calling it again should be a no-op
    EXPECT_FALSE(logic.SetPercentage(1.0f));

    EXPECT_EQ(1.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + lengthPx - 1), logic.GetPositionPx());
    EXPECT_EQ(constrainedValue.Max(), logic.GetValue());
  }
}


TEST(TestControlLogic_SliderLogic, SetPercentage_OutOfRange_Low)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetPercentage(1.0f));

  EXPECT_TRUE(logic.SetPercentage(-0.1f));

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());

  {    // Calling it again with a out of range value  should be a no-op
    EXPECT_FALSE(logic.SetPercentage(-10.0f));

    EXPECT_EQ(0.0f, logic.GetPercentage());
    EXPECT_EQ(startPx, logic.GetPositionPx());
    EXPECT_EQ(constrainedValue.Min(), logic.GetValue());
  }
}


TEST(TestControlLogic_SliderLogic, SetPercentage_OutOfRange_High)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetPercentage(1.2f));

  EXPECT_EQ(1.0f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + lengthPx - 1), logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Max(), logic.GetValue());

  {    // Calling it again should be a no-op
    EXPECT_FALSE(logic.SetPercentage(10.0f));

    EXPECT_EQ(1.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + lengthPx - 1), logic.GetPositionPx());
    EXPECT_EQ(constrainedValue.Max(), logic.GetValue());
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestControlLogic_SliderLogic, TryBeginDrag_InRange_MinA)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, TryBeginDrag_InRange_MinB)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  EXPECT_TRUE(logic.SetPercentage(0.5f));

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());
}

TEST(TestControlLogic_SliderLogic, TryBeginDrag_InRange_Middle)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 100));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());
}

TEST(TestControlLogic_SliderLogic, TryBeginDrag_InRange_Max)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 200));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(1.0f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + lengthPx - 1), logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Max(), logic.GetValue());
}

TEST(TestControlLogic_SliderLogic, TryBeginDrag_InRange_FullCoverage)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 11;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 1));
  EXPECT_FLOAT_EQ(0.1f, logic.GetPercentage());
  EXPECT_EQ(startPx + 1, logic.GetPositionPx());
  EXPECT_EQ(1100u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 2));
  EXPECT_FLOAT_EQ(0.2f, logic.GetPercentage());
  EXPECT_EQ(startPx + 2, logic.GetPositionPx());
  EXPECT_EQ(1200u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 3));
  EXPECT_FLOAT_EQ(0.3f, logic.GetPercentage());
  EXPECT_EQ(startPx + 3, logic.GetPositionPx());
  EXPECT_EQ(1300u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 4));
  EXPECT_FLOAT_EQ(0.4f, logic.GetPercentage());
  EXPECT_EQ(startPx + 4, logic.GetPositionPx());
  EXPECT_EQ(1400u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 5));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 6));
  EXPECT_FLOAT_EQ(0.6f, logic.GetPercentage());
  EXPECT_EQ(startPx + 6, logic.GetPositionPx());
  EXPECT_EQ(1600u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 7));
  EXPECT_FLOAT_EQ(0.7f, logic.GetPercentage());
  EXPECT_EQ(startPx + 7, logic.GetPositionPx());
  EXPECT_EQ(1700u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 8));
  EXPECT_FLOAT_EQ(0.8f, logic.GetPercentage());
  EXPECT_EQ(startPx + 8, logic.GetPositionPx());
  EXPECT_EQ(1800u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 9));
  EXPECT_FLOAT_EQ(0.9f, logic.GetPercentage());
  EXPECT_EQ(startPx + 9, logic.GetPositionPx());
  EXPECT_EQ(1900u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));
}

TEST(TestControlLogic_SliderLogic, TryBeginDrag_OutOfRange_Low1)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx - 1));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, TryBeginDrag_OutOfRange_Low2)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx - 100));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, TryBeginDrag_OutOfRange_High1)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + lengthPx));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(1.0f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + lengthPx - 1), logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Max(), logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, TryBeginDrag_OutOfRange_High2)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + lengthPx + 100));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(1.0f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + lengthPx - 1), logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Max(), logic.GetValue());
}

TEST(TestControlLogic_SliderLogic, TryDrag_InRange2)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 100));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + lengthPx - 1));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(1.0f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + lengthPx - 1), logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Max(), logic.GetValue());
}

TEST(TestControlLogic_SliderLogic, TryDrag_InRange_FullCoverage)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 11;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.IsDragging());

  EXPECT_TRUE(logic.TryDrag(startPx));
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 1));
  EXPECT_FLOAT_EQ(0.1f, logic.GetPercentage());
  EXPECT_EQ(startPx + 1, logic.GetPositionPx());
  EXPECT_EQ(1100u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 2));
  EXPECT_FLOAT_EQ(0.2f, logic.GetPercentage());
  EXPECT_EQ(startPx + 2, logic.GetPositionPx());
  EXPECT_EQ(1200u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 3));
  EXPECT_FLOAT_EQ(0.3f, logic.GetPercentage());
  EXPECT_EQ(startPx + 3, logic.GetPositionPx());
  EXPECT_EQ(1300u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 4));
  EXPECT_FLOAT_EQ(0.4f, logic.GetPercentage());
  EXPECT_EQ(startPx + 4, logic.GetPositionPx());
  EXPECT_EQ(1400u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 5));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 6));
  EXPECT_FLOAT_EQ(0.6f, logic.GetPercentage());
  EXPECT_EQ(startPx + 6, logic.GetPositionPx());
  EXPECT_EQ(1600u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 7));
  EXPECT_FLOAT_EQ(0.7f, logic.GetPercentage());
  EXPECT_EQ(startPx + 7, logic.GetPositionPx());
  EXPECT_EQ(1700u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 8));
  EXPECT_FLOAT_EQ(0.8f, logic.GetPercentage());
  EXPECT_EQ(startPx + 8, logic.GetPositionPx());
  EXPECT_EQ(1800u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 9));
  EXPECT_FLOAT_EQ(0.9f, logic.GetPercentage());
  EXPECT_EQ(startPx + 9, logic.GetPositionPx());
  EXPECT_EQ(1900u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 10));
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, TryDrag_NotDraging)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  EXPECT_TRUE(logic.SetPercentage(0.5f));

  EXPECT_FALSE(logic.IsDragging());

  // As we are not dragging, the TryDrag does nothing
  EXPECT_FALSE(logic.TryDrag(startPx));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
}


TEST(TestControlLogic_SliderLogic, EndDrag_InRange)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 100));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());

  EXPECT_TRUE(logic.EndDrag(startPx + lengthPx - 1));
  EXPECT_FALSE(logic.IsDragging());
  EXPECT_EQ(1.0f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + lengthPx - 1), logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Max(), logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, EndDrag_InRange_FullCoverage)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 11;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.EndDrag(startPx));
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 1));
  EXPECT_FLOAT_EQ(0.1f, logic.GetPercentage());
  EXPECT_EQ(startPx + 1, logic.GetPositionPx());
  EXPECT_EQ(1100u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 2));
  EXPECT_FLOAT_EQ(0.2f, logic.GetPercentage());
  EXPECT_EQ(startPx + 2, logic.GetPositionPx());
  EXPECT_EQ(1200u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 3));
  EXPECT_FLOAT_EQ(0.3f, logic.GetPercentage());
  EXPECT_EQ(startPx + 3, logic.GetPositionPx());
  EXPECT_EQ(1300u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 4));
  EXPECT_FLOAT_EQ(0.4f, logic.GetPercentage());
  EXPECT_EQ(startPx + 4, logic.GetPositionPx());
  EXPECT_EQ(1400u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 5));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 6));
  EXPECT_FLOAT_EQ(0.6f, logic.GetPercentage());
  EXPECT_EQ(startPx + 6, logic.GetPositionPx());
  EXPECT_EQ(1600u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 7));
  EXPECT_FLOAT_EQ(0.7f, logic.GetPercentage());
  EXPECT_EQ(startPx + 7, logic.GetPositionPx());
  EXPECT_EQ(1700u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 8));
  EXPECT_FLOAT_EQ(0.8f, logic.GetPercentage());
  EXPECT_EQ(startPx + 8, logic.GetPositionPx());
  EXPECT_EQ(1800u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 9));
  EXPECT_FLOAT_EQ(0.9f, logic.GetPercentage());
  EXPECT_EQ(startPx + 9, logic.GetPositionPx());
  EXPECT_EQ(1900u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 10));
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, CancelDrag)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetPercentage(0.5f));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + lengthPx));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + lengthPx - 1, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Max(), logic.GetValue());

  // The cancel restores the pre-drag state
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FALSE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, EndDrag_NotDraging)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetPercentage(0.5f));

  EXPECT_FALSE(logic.IsDragging());

  // As we are not dragging, the end drag does nothing
  EXPECT_FALSE(logic.EndDrag(startPx));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
}


TEST(TestControlLogic_SliderLogic, CancelDrag_NotDraging)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetPercentage(0.5f));

  EXPECT_FALSE(logic.IsDragging());

  // As we are not dragging, the end drag does nothing
  EXPECT_FALSE(logic.CancelDrag());

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, SetEnabled)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  {    // First disable -> disabled it
    auto flags = logic.SetEnabled(false);
    EXPECT_TRUE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::Completed));
    EXPECT_FALSE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::DragCancelled));
    EXPECT_FALSE(logic.IsEnabled());
  }

  {    // second disable does nothing
    auto flags = logic.SetEnabled(false);
    EXPECT_FALSE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::Completed));
    EXPECT_FALSE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::DragCancelled));
    EXPECT_FALSE(logic.IsEnabled());
  }

  {    // First enable -> enables it
    auto flags = logic.SetEnabled(true);
    EXPECT_TRUE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::Completed));
    EXPECT_FALSE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::DragCancelled));
    EXPECT_TRUE(logic.IsEnabled());
  }

  {    // second enable does nothing
    auto flags = logic.SetEnabled(true);
    EXPECT_FALSE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::Completed));
    EXPECT_FALSE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::DragCancelled));
    EXPECT_TRUE(logic.IsEnabled());
  }
}


TEST(TestControlLogic_SliderLogic, SetEnabled_True_NoCancelDrag)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));


  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());
  {    // force the initial value to be something other than the default
    EXPECT_TRUE(logic.SetPercentage(1.0f));
    EXPECT_EQ(1.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + lengthPx - 1), logic.GetPositionPx());
    EXPECT_EQ(constrainedValue.Max(), logic.GetValue());
  }

  // Initiate drag from the middle
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 100));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());

  {    // Setting to enabled does nothing as we are already enabled
    auto flags = logic.SetEnabled(true);
    EXPECT_FALSE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::Completed));
    EXPECT_FALSE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::DragCancelled));
    EXPECT_TRUE(logic.IsEnabled());
  }

  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, SetEnabled_False_CancelDrag)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));


  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());
  {    // force the initial value to be something other than the default
    EXPECT_TRUE(logic.SetPercentage(1.0f));
    EXPECT_EQ(1.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + lengthPx - 1), logic.GetPositionPx());
    EXPECT_EQ(constrainedValue.Max(), logic.GetValue());
  }

  // Initiate drag from the middle
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 100));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());

  // Cancel the drag, restoring the pre-drag state
  auto flags = logic.SetEnabled(false);
  EXPECT_TRUE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::Completed));
  EXPECT_TRUE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::DragCancelled));
  EXPECT_FALSE(logic.IsEnabled());

  {    // Verify that we returned to the pre-drag state
    EXPECT_FALSE(logic.IsDragging());
    EXPECT_EQ(1.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + lengthPx - 1), logic.GetPositionPx());
    EXPECT_EQ(constrainedValue.Max(), logic.GetValue());
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, TryBeginDrag_Disabled)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  logic.SetEnabled(false);

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());

  // The begin drag should be a 'no-op' when disabled
  EXPECT_FALSE(logic.TryBeginDrag(startPx + 100));
  EXPECT_FALSE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, TryDrag_Disabled)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  logic.SetEnabled(false);

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());

  // The drag should be a 'no-op' when disabled
  EXPECT_FALSE(logic.TryDrag(startPx + 100));
  EXPECT_FALSE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, EndDrag_Disabled)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  logic.SetEnabled(false);

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());

  // The EndDrag should be a 'no-op' when disabled
  EXPECT_FALSE(logic.EndDrag(startPx + 100));
  EXPECT_FALSE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, CancelDrag_Disabled)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  logic.SetEnabled(false);

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());

  // The EndDrag should be a 'no-op' when disabled
  EXPECT_FALSE(logic.CancelDrag());
  EXPECT_FALSE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, SetSpanInfo_Percentage0)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1500, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  EXPECT_TRUE(logic.SetPercentage(0));
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  const int32_t newStartPx = 55;
  const int32_t newLengthPx = 11;
  logic.SetSpanInfo(UI::SliderPixelSpanInfo(newStartPx, newLengthPx));
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(newStartPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());
}

TEST(TestControlLogic_SliderLogic, SetSpanInfo_Percentage1)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  EXPECT_TRUE(logic.SetPercentage(1));
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + lengthPx - 1, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Max(), logic.GetValue());

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  const int32_t newStartPx = 55;
  const int32_t newLengthPx = 11;
  EXPECT_TRUE(logic.SetSpanInfo(UI::SliderPixelSpanInfo(newStartPx, newLengthPx)));
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(newStartPx + newLengthPx - 1, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Max(), logic.GetValue());
}

TEST(TestControlLogic_SliderLogic, SetSpanInfo_Same_NoCancelDrag)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());
  {    // force the initial value to be something other than the default
    EXPECT_TRUE(logic.SetPercentage(1.0f));
    EXPECT_EQ(1.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + lengthPx - 1), logic.GetPositionPx());
    EXPECT_EQ(constrainedValue.Max(), logic.GetValue());
  }

  // Initiate drag from the middle
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 100));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());

  {    // SetSpanInfo to the same value does nothing
    EXPECT_FALSE(logic.SetSpanInfo(UI::SliderPixelSpanInfo(startPx, lengthPx)));
  }

  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());
}

TEST(TestControlLogic_SliderLogic, SetSpanInfo_New_RecalcPercentage)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());
  {    // force the initial value to be something other than the default
    EXPECT_TRUE(logic.SetPercentage(1.0f));
    EXPECT_EQ(1.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + lengthPx - 1), logic.GetPositionPx());
    EXPECT_EQ(constrainedValue.Max(), logic.GetValue());
  }

  // Initiate drag from the middle
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 100));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());

  // Last drag was at startPx + 100 which is 200
  const int32_t newStartPx = 50;    // 50-801
  const int32_t newLengthPx = 751;

  {    // SetSpanInfo to a new value recalculates the percentage
    EXPECT_TRUE(logic.SetSpanInfo(UI::SliderPixelSpanInfo(newStartPx, newLengthPx)));
  }

  {    // Verify that the percentage was changed
    EXPECT_TRUE(logic.IsDragging());
    EXPECT_FLOAT_EQ(0.2f, logic.GetPercentage());
    EXPECT_EQ(startPx + 100, logic.GetPositionPx());
    EXPECT_EQ(constrainedValue.Min() + 200u, logic.GetValue());
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, SetValue_Min)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1500, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  EXPECT_TRUE(logic.SetValue(constrainedValue.Max()));
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + lengthPx - 1, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Max(), logic.GetValue());

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  EXPECT_TRUE(logic.SetValue(constrainedValue.Min()));
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());
}

TEST(TestControlLogic_SliderLogic, SetValue_75percent)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1500, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  EXPECT_TRUE(logic.SetValue(constrainedValue.Min()));
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  const uint32_t newValue = 1750;
  EXPECT_TRUE(logic.SetValue(newValue));
  EXPECT_FLOAT_EQ(0.75f, logic.GetPercentage());
  EXPECT_EQ(startPx + 150, logic.GetPositionPx());
  EXPECT_EQ(newValue, logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, SetValue_Max)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1500, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  EXPECT_TRUE(logic.SetValue(constrainedValue.Min()));
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Min(), logic.GetValue());

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  EXPECT_TRUE(logic.SetValue(constrainedValue.Max()));
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + lengthPx - 1, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Max(), logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, SetValue_Same_NoCancelDrag)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(2000, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  // Initiate drag from the middle
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 100));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());

  {    // SetValue to the same value does not cancel the drag
    EXPECT_FALSE(logic.SetValue(constrainedValue.Center()));
  }

  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, SetValue_New_NoCancelDrag)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(2000, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  // Initiate drag from the middle
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 100));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());

  {    // SetValue to a new value does not cancel the drag
    EXPECT_TRUE(logic.SetValue(constrainedValue.Min()));
  }

  {    // Verify that we are now at the SetValue
    EXPECT_TRUE(logic.IsDragging());
    EXPECT_EQ(0.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ(startPx, logic.GetPositionPx());
    EXPECT_EQ(constrainedValue.Min(), logic.GetValue());
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, SetRange)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  {
    EXPECT_EQ(constrainedValue.Min(), logic.Min());
    EXPECT_EQ(constrainedValue.Max(), logic.Max());
    EXPECT_EQ(constrainedValue.Get(), logic.GetValue());

    EXPECT_FALSE(logic.IsDragging());
    EXPECT_TRUE(logic.IsEnabled());

    EXPECT_EQ(0.0f, logic.GetPercentage());

    auto spanInfo = logic.GetSpanInfo();
    EXPECT_EQ(startPx, spanInfo.GetStartPx());
    EXPECT_EQ(lengthPx, spanInfo.GetLengthPx());
  }

  uint32_t newMin = 5000;
  uint32_t newMax = 6000;
  logic.SetRange(5000, 6000);

  {
    EXPECT_EQ(newMin, logic.Min());
    EXPECT_EQ(newMax, logic.Max());
    EXPECT_EQ(newMin, logic.GetValue());

    EXPECT_FALSE(logic.IsDragging());
    EXPECT_TRUE(logic.IsEnabled());

    EXPECT_EQ(0.0f, logic.GetPercentage());

    auto spanInfo = logic.GetSpanInfo();
    EXPECT_EQ(startPx, spanInfo.GetStartPx());
    EXPECT_EQ(lengthPx, spanInfo.GetLengthPx());
  }
}


TEST(TestControlLogic_SliderLogic, SetRange_Same_NoCancelDrag)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(2000, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  // Initiate drag from the middle
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 100));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());

  {    // SetRange to the same does not cancel the drag
    EXPECT_FALSE(logic.SetRange(constrainedValue.Min(), constrainedValue.Max()));
  }

  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, SetRange_New_RecalculateValue)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(2000, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  // Initiate drag from the middle
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 100));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(constrainedValue.Center(), logic.GetValue());

  const uint32_t newMin = 5000;
  const uint32_t newMax = 7000;
  const uint32_t newCenter = newMin + ((newMax - newMin) / 2);
  {    // SetRange to a new value does not cancel the drag
    EXPECT_TRUE(logic.SetRange(newMin, newMax));
  }

  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 100, logic.GetPositionPx());
  EXPECT_EQ(newCenter, logic.GetValue());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, SetTickFrequency)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetTickFrequency(200u));
  EXPECT_EQ(200u, logic.GetTickFrequency());

  // Setting it to the same value does nothing
  EXPECT_FALSE(logic.SetTickFrequency(200u));
  EXPECT_EQ(200u, logic.GetTickFrequency());

  EXPECT_TRUE(logic.SetTickFrequency(100u));
  EXPECT_EQ(100u, logic.GetTickFrequency());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, SetTickFrequency_OutOfRange_Low)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetTickFrequency(200u));
  EXPECT_TRUE(logic.SetTickFrequency(0u));
  EXPECT_EQ(0u, logic.GetTickFrequency());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, SetTickFrequency_OutOfRange_Low2)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<int32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<int32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetTickFrequency(200));
  EXPECT_TRUE(logic.SetTickFrequency(0));
  EXPECT_EQ(0, logic.GetTickFrequency());

  EXPECT_TRUE(logic.SetTickFrequency(200));
  EXPECT_TRUE(logic.SetTickFrequency(-1));
  EXPECT_EQ(0, logic.GetTickFrequency());

  EXPECT_TRUE(logic.SetTickFrequency(200));
  EXPECT_TRUE(logic.SetTickFrequency(std::numeric_limits<int32_t>::min()));
  EXPECT_EQ(0, logic.GetTickFrequency());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, SetTickFrequency_OutOfRange_Max)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetTickFrequency(200u));
  EXPECT_TRUE(logic.SetTickFrequency(2001u));
  EXPECT_EQ(2000u, logic.GetTickFrequency());

  EXPECT_TRUE(logic.SetTickFrequency(200u));
  EXPECT_TRUE(logic.SetTickFrequency(std::numeric_limits<uint32_t>::max()));
  EXPECT_EQ(2000u, logic.GetTickFrequency());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, SetTickFrequency_CheckValueSnap)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1900, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetTickFrequency(500u));
  EXPECT_EQ(500u, logic.GetTickFrequency());

  // A tick frequency of 500 means the following values are valid:
  // 1000, 1500, 2000
  // So the current value of 1900 is closest to 2000

  EXPECT_EQ(2000u, logic.GetValue());
  EXPECT_FLOAT_EQ(1.0f, logic.GetPercentage());

  logic.SetValue(1750);
  EXPECT_EQ(2000u, logic.GetValue());
  EXPECT_FLOAT_EQ(1.0f, logic.GetPercentage());

  logic.SetValue(1749);
  EXPECT_EQ(1500u, logic.GetValue());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  logic.SetValue(1600);
  EXPECT_EQ(1500u, logic.GetValue());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  logic.SetValue(1400);
  EXPECT_EQ(1500u, logic.GetValue());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  logic.SetValue(1250);
  EXPECT_EQ(1500u, logic.GetValue());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());

  logic.SetValue(1249);
  EXPECT_EQ(1000u, logic.GetValue());
  EXPECT_FLOAT_EQ(0.0f, logic.GetPercentage());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, Construct_Span_Reversed_MinValue)
{
  const int32_t startPx = 10;
  const int32_t lengthPx = 5;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx, true));

  EXPECT_EQ(constrainedValue.Min(), logic.Min());
  EXPECT_EQ(constrainedValue.Max(), logic.Max());
  EXPECT_EQ(constrainedValue.Get(), logic.GetValue());

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + lengthPx - 1, logic.GetPositionPx());

  auto spanInfo = logic.GetSpanInfo();
  EXPECT_EQ(startPx, spanInfo.GetStartPx());
  EXPECT_EQ(lengthPx, spanInfo.GetLengthPx());
  EXPECT_TRUE(spanInfo.IsReversedDirection());
}

TEST(TestControlLogic_SliderLogic, Construct_Span_Reversed_MaxValue)
{
  const int32_t startPx = 10;
  const int32_t lengthPx = 5;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(2000, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx, true));

  EXPECT_EQ(constrainedValue.Min(), logic.Min());
  EXPECT_EQ(constrainedValue.Max(), logic.Max());
  EXPECT_EQ(constrainedValue.Get(), logic.GetValue());

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());

  auto spanInfo = logic.GetSpanInfo();
  EXPECT_EQ(startPx, spanInfo.GetStartPx());
  EXPECT_EQ(lengthPx, spanInfo.GetLengthPx());
  EXPECT_TRUE(spanInfo.IsReversedDirection());
}


TEST(TestControlLogic_SliderLogic, Construct_Span_Reversed_Value)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 201;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1250, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx, true));

  EXPECT_EQ(constrainedValue.Min(), logic.Min());
  EXPECT_EQ(constrainedValue.Max(), logic.Max());
  EXPECT_EQ(constrainedValue.Get(), logic.GetValue());

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  EXPECT_FLOAT_EQ(0.25f, logic.GetPercentage());
  EXPECT_EQ(startPx + 150, logic.GetPositionPx());

  auto spanInfo = logic.GetSpanInfo();
  EXPECT_EQ(startPx, spanInfo.GetStartPx());
  EXPECT_EQ(lengthPx, spanInfo.GetLengthPx());
  EXPECT_TRUE(spanInfo.IsReversedDirection());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, GetPercentage)
{
  const int32_t startPx = 0;
  const int32_t lengthPx = 21;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1500, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx, false));

  EXPECT_TRUE(logic.SetValue(1000));
  EXPECT_FLOAT_EQ(0 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1050));
  EXPECT_FLOAT_EQ(1 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1100));
  EXPECT_FLOAT_EQ(2 / 20.0f, logic.GetPercentage());

  EXPECT_TRUE(logic.SetValue(1150));
  EXPECT_FLOAT_EQ(3 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1200));
  EXPECT_FLOAT_EQ(4 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1250));
  EXPECT_FLOAT_EQ(5 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1300));
  EXPECT_FLOAT_EQ(6 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1350));
  EXPECT_FLOAT_EQ(7 / 20.0f, logic.GetPercentage());

  EXPECT_TRUE(logic.SetValue(1400));
  EXPECT_FLOAT_EQ(8 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1450));
  EXPECT_FLOAT_EQ(9 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1500));
  EXPECT_FLOAT_EQ(10 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1550));
  EXPECT_FLOAT_EQ(11 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1600));
  EXPECT_FLOAT_EQ(12 / 20.0f, logic.GetPercentage());

  EXPECT_TRUE(logic.SetValue(1650));
  EXPECT_FLOAT_EQ(13 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1700));
  EXPECT_FLOAT_EQ(14 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1750));
  EXPECT_FLOAT_EQ(15 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1800));
  EXPECT_FLOAT_EQ(16 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1850));
  EXPECT_FLOAT_EQ(17 / 20.0f, logic.GetPercentage());

  EXPECT_TRUE(logic.SetValue(1900));
  EXPECT_FLOAT_EQ(18 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1950));
  EXPECT_FLOAT_EQ(19 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(2000));
  EXPECT_FLOAT_EQ(20 / 20.0f, logic.GetPercentage());
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, GetValue)
{
  const int32_t startPx = 0;
  const int32_t lengthPx = 21;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1500, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx, false));

  EXPECT_TRUE(logic.SetValue(1000));
  EXPECT_EQ(1000u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(1050));
  EXPECT_EQ(1050u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(1100));
  EXPECT_EQ(1100u, logic.GetValue());

  EXPECT_TRUE(logic.SetValue(1150));
  EXPECT_EQ(1150u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(1200));
  EXPECT_EQ(1200u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(1250));
  EXPECT_EQ(1250u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(1300));
  EXPECT_EQ(1300u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(1350));
  EXPECT_EQ(1350u, logic.GetValue());

  EXPECT_TRUE(logic.SetValue(1400));
  EXPECT_EQ(1400u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(1450));
  EXPECT_EQ(1450u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(1500));
  EXPECT_EQ(1500u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(1550));
  EXPECT_EQ(1550u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(1600));
  EXPECT_EQ(1600u, logic.GetValue());

  EXPECT_TRUE(logic.SetValue(1650));
  EXPECT_EQ(1650u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(1700));
  EXPECT_EQ(1700u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(1750));
  EXPECT_EQ(1750u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(1800));
  EXPECT_EQ(1800u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(1850));
  EXPECT_EQ(1850u, logic.GetValue());

  EXPECT_TRUE(logic.SetValue(1900));
  EXPECT_EQ(1900u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(1950));
  EXPECT_EQ(1950u, logic.GetValue());
  EXPECT_TRUE(logic.SetValue(2000));
  EXPECT_EQ(2000u, logic.GetValue());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, GetPercentage_Offset)
{
  const int32_t startPx = 0;
  const int32_t lengthPx = 21;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1500, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx, false));

  EXPECT_TRUE(logic.SetValue(1000));
  EXPECT_FLOAT_EQ(0 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1050));
  EXPECT_FLOAT_EQ(1 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1100));
  EXPECT_FLOAT_EQ(2 / 20.0f, logic.GetPercentage());

  EXPECT_TRUE(logic.SetValue(1150));
  EXPECT_FLOAT_EQ(3 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1200));
  EXPECT_FLOAT_EQ(4 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1250));
  EXPECT_FLOAT_EQ(5 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1300));
  EXPECT_FLOAT_EQ(6 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1350));
  EXPECT_FLOAT_EQ(7 / 20.0f, logic.GetPercentage());

  EXPECT_TRUE(logic.SetValue(1400));
  EXPECT_FLOAT_EQ(8 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1450));
  EXPECT_FLOAT_EQ(9 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1500));
  EXPECT_FLOAT_EQ(10 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1550));
  EXPECT_FLOAT_EQ(11 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1600));
  EXPECT_FLOAT_EQ(12 / 20.0f, logic.GetPercentage());

  EXPECT_TRUE(logic.SetValue(1650));
  EXPECT_FLOAT_EQ(13 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1700));
  EXPECT_FLOAT_EQ(14 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1750));
  EXPECT_FLOAT_EQ(15 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1800));
  EXPECT_FLOAT_EQ(16 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1850));
  EXPECT_FLOAT_EQ(17 / 20.0f, logic.GetPercentage());

  EXPECT_TRUE(logic.SetValue(1900));
  EXPECT_FLOAT_EQ(18 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(1950));
  EXPECT_FLOAT_EQ(19 / 20.0f, logic.GetPercentage());
  EXPECT_TRUE(logic.SetValue(2000));
  EXPECT_FLOAT_EQ(20 / 20.0f, logic.GetPercentage());
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, GetPercentage_Offset_Tick)
{
  const int32_t startPx = 0;
  const int32_t lengthPx = 21;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1500, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx, false));
  // 1000 1250 1500 750 2000
  logic.SetTickFrequency(250u);


  EXPECT_TRUE(logic.SetValue(1000));
  EXPECT_FLOAT_EQ(0 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(1050));
  EXPECT_FLOAT_EQ(0 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(1100));
  EXPECT_FLOAT_EQ(0 / 1000.0f, logic.GetPercentage());

  EXPECT_TRUE(logic.SetValue(1150));
  EXPECT_FLOAT_EQ(250 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(1200));
  EXPECT_FLOAT_EQ(250 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(1250));
  EXPECT_FLOAT_EQ(250 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(1300));
  EXPECT_FLOAT_EQ(250 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(1350));
  EXPECT_FLOAT_EQ(250 / 1000.0f, logic.GetPercentage());

  EXPECT_TRUE(logic.SetValue(1400));
  EXPECT_FLOAT_EQ(500 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(1450));
  EXPECT_FLOAT_EQ(500 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(1500));
  EXPECT_FLOAT_EQ(500 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(1550));
  EXPECT_FLOAT_EQ(500 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(1600));
  EXPECT_FLOAT_EQ(500 / 1000.0f, logic.GetPercentage());

  EXPECT_TRUE(logic.SetValue(1650));
  EXPECT_FLOAT_EQ(750 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(1700));
  EXPECT_FLOAT_EQ(750 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(1750));
  EXPECT_FLOAT_EQ(750 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(1800));
  EXPECT_FLOAT_EQ(750 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(1850));
  EXPECT_FLOAT_EQ(750 / 1000.0f, logic.GetPercentage());

  EXPECT_TRUE(logic.SetValue(1900));
  EXPECT_FLOAT_EQ(1000 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(1950));
  EXPECT_FLOAT_EQ(1000 / 1000.0f, logic.GetPercentage());
  EXPECT_FALSE(logic.SetValue(2000));
  EXPECT_FLOAT_EQ(1000 / 1000.0f, logic.GetPercentage());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderLogic, GetValue_Tick)
{
  const int32_t startPx = 0;
  const int32_t lengthPx = 21;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1500, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx, false));
  // 1000 1250 1500 750 2000
  logic.SetTickFrequency(250u);

  EXPECT_TRUE(logic.SetValue(1000));
  EXPECT_EQ(1000u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(1050));
  EXPECT_EQ(1000u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(1100));
  EXPECT_EQ(1000u, logic.GetValue());

  EXPECT_TRUE(logic.SetValue(1150));
  EXPECT_EQ(1250u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(1200));
  EXPECT_EQ(1250u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(1250));
  EXPECT_EQ(1250u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(1300));
  EXPECT_EQ(1250u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(1350));
  EXPECT_EQ(1250u, logic.GetValue());

  EXPECT_TRUE(logic.SetValue(1400));
  EXPECT_EQ(1500u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(1450));
  EXPECT_EQ(1500u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(1500));
  EXPECT_EQ(1500u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(1550));
  EXPECT_EQ(1500u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(1600));
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_TRUE(logic.SetValue(1650));
  EXPECT_EQ(1750u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(1700));
  EXPECT_EQ(1750u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(1750));
  EXPECT_EQ(1750u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(1800));
  EXPECT_EQ(1750u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(1850));
  EXPECT_EQ(1750u, logic.GetValue());

  EXPECT_TRUE(logic.SetValue(1900));
  EXPECT_EQ(2000u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(1950));
  EXPECT_EQ(2000u, logic.GetValue());
  EXPECT_FALSE(logic.SetValue(2000));
  EXPECT_EQ(2000u, logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, TryBeginDrag_InRange_FullCoverage_Tick)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 11;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  // 1000 1500 2000
  logic.SetTickFrequency(500u);
  // 01234567890
  // AAABBBBBCCC

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 1));
  EXPECT_FLOAT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 2));
  EXPECT_FLOAT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));
  // ---
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 3));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 4));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 5));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 6));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 7));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));
  // ---
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 8));
  EXPECT_FLOAT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 9));
  EXPECT_FLOAT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());
  EXPECT_TRUE(logic.EndDrag(startPx));
}


TEST(TestControlLogic_SliderLogic, TryDrag_InRange_FullCoverage_Tick)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 11;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  // 1000 1500 2000
  logic.SetTickFrequency(500u);
  // 01234567890
  // AAABBBBBCCC

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.IsDragging());

  EXPECT_TRUE(logic.TryDrag(startPx));
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 1));
  EXPECT_FLOAT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 2));
  EXPECT_FLOAT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());
  // ---
  EXPECT_TRUE(logic.TryDrag(startPx + 3));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 4));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 5));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 6));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 7));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());
  // ---
  EXPECT_TRUE(logic.TryDrag(startPx + 8));
  EXPECT_FLOAT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 9));
  EXPECT_FLOAT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());

  EXPECT_TRUE(logic.TryDrag(startPx + 10));
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, EndDrag_InRange_FullCoverage_Tick)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 11;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  // 1000 1500 2000
  logic.SetTickFrequency(500u);
  // 01234567890
  // AAABBBBBCCC

  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.EndDrag(startPx));
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 1));
  EXPECT_FLOAT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 0, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 2));
  EXPECT_FLOAT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 0, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());
  // ---
  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 3));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 4));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 5));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 6));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 7));
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());
  // ---
  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 8));
  EXPECT_FLOAT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 9));
  EXPECT_FLOAT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.EndDrag(startPx + 10));
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, CancelDrag_InRange_FullCoverage_Tick)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 11;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1500, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  // 1000 1500 2000
  logic.SetTickFrequency(500u);
  // 01234567890
  // AAABBBBBCCC

  EXPECT_TRUE(logic.SetValue(1000));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());

  EXPECT_FALSE(logic.SetValue(1100));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());

  EXPECT_FALSE(logic.SetValue(1200));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());
  // ---
  EXPECT_TRUE(logic.SetValue(1300));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_FALSE(logic.SetValue(1400));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_FALSE(logic.SetValue(1500));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_FALSE(logic.SetValue(1600));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_FALSE(logic.SetValue(1700));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());
  // ---
  EXPECT_TRUE(logic.SetValue(1800));
  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());

  EXPECT_FALSE(logic.SetValue(1900));
  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());

  EXPECT_FALSE(logic.SetValue(2000));
  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());
}


TEST(TestControlLogic_SliderLogic, CancelDrag_InRange_FullCoverage_DragStartTickOff_CancelTickOn)
{
  const int32_t startPx = 100;
  const int32_t lengthPx = 11;
  UI::SliderConstrainedValue<uint32_t> constrainedValue(1500, 1000, 2000);
  UI::SliderLogic<uint32_t> logic(constrainedValue, UI::SliderPixelSpanInfo(startPx, lengthPx));
  // 1000 1500 2000
  // 01234567890
  // AAABBBBBCCC

  logic.SetTickFrequency(0u);
  EXPECT_TRUE(logic.SetValue(1000));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.SetTickFrequency(500u));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());

  EXPECT_TRUE(logic.SetTickFrequency(0u));
  EXPECT_TRUE(logic.SetValue(1100));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.SetTickFrequency(500u));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());

  EXPECT_TRUE(logic.SetTickFrequency(0u));
  EXPECT_TRUE(logic.SetValue(1200));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.SetTickFrequency(500u));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx());
  EXPECT_EQ(1000u, logic.GetValue());
  // ---
  EXPECT_TRUE(logic.SetTickFrequency(0u));
  EXPECT_TRUE(logic.SetValue(1300));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.SetTickFrequency(500u));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_TRUE(logic.SetTickFrequency(0u));
  EXPECT_TRUE(logic.SetValue(1400));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.SetTickFrequency(500u));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_TRUE(logic.SetTickFrequency(0u));
  EXPECT_FALSE(logic.SetValue(1500));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.SetTickFrequency(500u));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_TRUE(logic.SetTickFrequency(0u));
  EXPECT_TRUE(logic.SetValue(1600));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.SetTickFrequency(500u));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());

  EXPECT_TRUE(logic.SetTickFrequency(0u));
  EXPECT_TRUE(logic.SetValue(1700));
  EXPECT_TRUE(logic.TryBeginDrag(startPx + 10));
  EXPECT_TRUE(logic.SetTickFrequency(500u));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + 5, logic.GetPositionPx());
  EXPECT_EQ(1500u, logic.GetValue());
  // ---
  EXPECT_TRUE(logic.SetTickFrequency(0u));
  EXPECT_TRUE(logic.SetValue(1800));
  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.SetTickFrequency(500u));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());

  EXPECT_TRUE(logic.SetTickFrequency(0u));
  EXPECT_TRUE(logic.SetValue(1900));
  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.SetTickFrequency(500u));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FLOAT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());

  EXPECT_TRUE(logic.SetTickFrequency(0u));
  EXPECT_FALSE(logic.SetValue(2000));
  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.SetTickFrequency(500u));
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + 10, logic.GetPositionPx());
  EXPECT_EQ(2000u, logic.GetValue());
}
