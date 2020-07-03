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

#include <FslSimpleUI/Base/Control/Logic/SliderPixelSpanInfo.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <limits>

using namespace Fsl;

namespace
{
  using TestControlLogic_SliderPixelSpanInfo = TestFixtureFslBase;
}


TEST(TestControlLogic_SliderPixelSpanInfo, Construct_Default)
{
  UI::SliderPixelSpanInfo spanInfo;
  EXPECT_EQ(0, spanInfo.GetStartPx());
  EXPECT_EQ(0, spanInfo.GetLengthPx());
  EXPECT_FALSE(spanInfo.IsReversedDirection());
}

TEST(TestControlLogic_SliderPixelSpanInfo, Construct)
{
  UI::SliderPixelSpanInfo spanInfo(1, 10, false);
  EXPECT_EQ(1, spanInfo.GetStartPx());
  EXPECT_EQ(10, spanInfo.GetLengthPx());
  EXPECT_FALSE(spanInfo.IsReversedDirection());
}

TEST(TestControlLogic_SliderPixelSpanInfo, OpEqual)
{
  EXPECT_TRUE(UI::SliderPixelSpanInfo(1, 10, false) == UI::SliderPixelSpanInfo(1, 10, false));
  EXPECT_FALSE(UI::SliderPixelSpanInfo(1, 10, false) == UI::SliderPixelSpanInfo(2, 10, false));
  EXPECT_FALSE(UI::SliderPixelSpanInfo(1, 10, false) == UI::SliderPixelSpanInfo(1, 11, false));
  EXPECT_FALSE(UI::SliderPixelSpanInfo(1, 10, false) == UI::SliderPixelSpanInfo(1, 10, true));
};

TEST(TestControlLogic_SliderPixelSpanInfo, OpNotEqual)
{
  EXPECT_FALSE(UI::SliderPixelSpanInfo(1, 10, false) != UI::SliderPixelSpanInfo(1, 10, false));
  EXPECT_TRUE(UI::SliderPixelSpanInfo(1, 10, false) != UI::SliderPixelSpanInfo(2, 10, false));
  EXPECT_TRUE(UI::SliderPixelSpanInfo(1, 10, false) != UI::SliderPixelSpanInfo(1, 11, false));
  EXPECT_TRUE(UI::SliderPixelSpanInfo(1, 10, false) != UI::SliderPixelSpanInfo(1, 10, true));
};


TEST(TestControlLogic_SliderPixelSpanInfo, Construct_Reverse)
{
  UI::SliderPixelSpanInfo spanInfo(1, 10, true);
  EXPECT_EQ(1, spanInfo.GetStartPx());
  EXPECT_EQ(10, spanInfo.GetLengthPx());
  EXPECT_TRUE(spanInfo.IsReversedDirection());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderPixelSpanInfo, CalculatePercentage)
{
  UI::SliderPixelSpanInfo spanInfo(0, 21, false);

  EXPECT_FLOAT_EQ(0 / 20.0f, spanInfo.CalculatePercentage(0));
  EXPECT_FLOAT_EQ(1 / 20.0f, spanInfo.CalculatePercentage(1));
  EXPECT_FLOAT_EQ(2 / 20.0f, spanInfo.CalculatePercentage(2));

  EXPECT_FLOAT_EQ(3 / 20.0f, spanInfo.CalculatePercentage(3));
  EXPECT_FLOAT_EQ(4 / 20.0f, spanInfo.CalculatePercentage(4));
  EXPECT_FLOAT_EQ(5 / 20.0f, spanInfo.CalculatePercentage(5));
  EXPECT_FLOAT_EQ(6 / 20.0f, spanInfo.CalculatePercentage(6));
  EXPECT_FLOAT_EQ(7 / 20.0f, spanInfo.CalculatePercentage(7));

  EXPECT_FLOAT_EQ(8 / 20.0f, spanInfo.CalculatePercentage(8));
  EXPECT_FLOAT_EQ(9 / 20.0f, spanInfo.CalculatePercentage(9));
  EXPECT_FLOAT_EQ(10 / 20.0f, spanInfo.CalculatePercentage(10));
  EXPECT_FLOAT_EQ(11 / 20.0f, spanInfo.CalculatePercentage(11));
  EXPECT_FLOAT_EQ(12 / 20.0f, spanInfo.CalculatePercentage(12));

  EXPECT_FLOAT_EQ(13 / 20.0f, spanInfo.CalculatePercentage(13));
  EXPECT_FLOAT_EQ(14 / 20.0f, spanInfo.CalculatePercentage(14));
  EXPECT_FLOAT_EQ(15 / 20.0f, spanInfo.CalculatePercentage(15));
  EXPECT_FLOAT_EQ(16 / 20.0f, spanInfo.CalculatePercentage(16));
  EXPECT_FLOAT_EQ(17 / 20.0f, spanInfo.CalculatePercentage(17));

  EXPECT_FLOAT_EQ(18 / 20.0f, spanInfo.CalculatePercentage(18));
  EXPECT_FLOAT_EQ(19 / 20.0f, spanInfo.CalculatePercentage(19));
  EXPECT_FLOAT_EQ(20 / 20.0f, spanInfo.CalculatePercentage(20));
}


TEST(TestControlLogic_SliderPixelSpanInfo, CalculatePercentage_Offset)
{
  UI::SliderPixelSpanInfo spanInfo(10, 21, false);

  EXPECT_FLOAT_EQ(0 / 20.0f, spanInfo.CalculatePercentage(10));
  EXPECT_FLOAT_EQ(1 / 20.0f, spanInfo.CalculatePercentage(11));
  EXPECT_FLOAT_EQ(2 / 20.0f, spanInfo.CalculatePercentage(12));

  EXPECT_FLOAT_EQ(3 / 20.0f, spanInfo.CalculatePercentage(13));
  EXPECT_FLOAT_EQ(4 / 20.0f, spanInfo.CalculatePercentage(14));
  EXPECT_FLOAT_EQ(5 / 20.0f, spanInfo.CalculatePercentage(15));
  EXPECT_FLOAT_EQ(6 / 20.0f, spanInfo.CalculatePercentage(16));
  EXPECT_FLOAT_EQ(7 / 20.0f, spanInfo.CalculatePercentage(17));

  EXPECT_FLOAT_EQ(8 / 20.0f, spanInfo.CalculatePercentage(18));
  EXPECT_FLOAT_EQ(9 / 20.0f, spanInfo.CalculatePercentage(19));
  EXPECT_FLOAT_EQ(10 / 20.0f, spanInfo.CalculatePercentage(20));
  EXPECT_FLOAT_EQ(11 / 20.0f, spanInfo.CalculatePercentage(21));
  EXPECT_FLOAT_EQ(12 / 20.0f, spanInfo.CalculatePercentage(22));

  EXPECT_FLOAT_EQ(13 / 20.0f, spanInfo.CalculatePercentage(23));
  EXPECT_FLOAT_EQ(14 / 20.0f, spanInfo.CalculatePercentage(24));
  EXPECT_FLOAT_EQ(15 / 20.0f, spanInfo.CalculatePercentage(25));
  EXPECT_FLOAT_EQ(16 / 20.0f, spanInfo.CalculatePercentage(26));
  EXPECT_FLOAT_EQ(17 / 20.0f, spanInfo.CalculatePercentage(27));

  EXPECT_FLOAT_EQ(18 / 20.0f, spanInfo.CalculatePercentage(28));
  EXPECT_FLOAT_EQ(19 / 20.0f, spanInfo.CalculatePercentage(29));
  EXPECT_FLOAT_EQ(20 / 20.0f, spanInfo.CalculatePercentage(30));
}


TEST(TestControlLogic_SliderPixelSpanInfo, CalculatePercentage_OutOfRange_Low)
{
  UI::SliderPixelSpanInfo spanInfo(10, 21, false);

  EXPECT_FLOAT_EQ(0.0f, spanInfo.CalculatePercentage(9));
  EXPECT_FLOAT_EQ(0.0f, spanInfo.CalculatePercentage(0));
  EXPECT_FLOAT_EQ(0.0f, spanInfo.CalculatePercentage(std::numeric_limits<int32_t>::min()));
}


TEST(TestControlLogic_SliderPixelSpanInfo, CalculatePercentage_OutOfRange_High)
{
  UI::SliderPixelSpanInfo spanInfo(10, 21, false);

  EXPECT_FLOAT_EQ(1.0f, spanInfo.CalculatePercentage(31));
  EXPECT_FLOAT_EQ(1.0f, spanInfo.CalculatePercentage(32));
  EXPECT_FLOAT_EQ(1.0f, spanInfo.CalculatePercentage(std::numeric_limits<int32_t>::max()));
}


TEST(TestControlLogic_SliderPixelSpanInfo, CalcPercentageToPxPosition)
{
  UI::SliderPixelSpanInfo spanInfo(10, 21, false);

  EXPECT_EQ(10, spanInfo.CalcPercentageToPxPosition(0 / 20.0f));
  EXPECT_EQ(11, spanInfo.CalcPercentageToPxPosition(1 / 20.0f));
  EXPECT_EQ(12, spanInfo.CalcPercentageToPxPosition(2 / 20.0f));

  EXPECT_EQ(13, spanInfo.CalcPercentageToPxPosition(3 / 20.0f));
  EXPECT_EQ(14, spanInfo.CalcPercentageToPxPosition(4 / 20.0f));
  EXPECT_EQ(15, spanInfo.CalcPercentageToPxPosition(5 / 20.0f));
  EXPECT_EQ(16, spanInfo.CalcPercentageToPxPosition(6 / 20.0f));
  EXPECT_EQ(17, spanInfo.CalcPercentageToPxPosition(7 / 20.0f));

  EXPECT_EQ(18, spanInfo.CalcPercentageToPxPosition(8 / 20.0f));
  EXPECT_EQ(19, spanInfo.CalcPercentageToPxPosition(9 / 20.0f));
  EXPECT_EQ(20, spanInfo.CalcPercentageToPxPosition(10 / 20.0f));
  EXPECT_EQ(21, spanInfo.CalcPercentageToPxPosition(11 / 20.0f));
  EXPECT_EQ(22, spanInfo.CalcPercentageToPxPosition(12 / 20.0f));

  EXPECT_EQ(23, spanInfo.CalcPercentageToPxPosition(13 / 20.0f));
  EXPECT_EQ(24, spanInfo.CalcPercentageToPxPosition(14 / 20.0f));
  EXPECT_EQ(25, spanInfo.CalcPercentageToPxPosition(15 / 20.0f));
  EXPECT_EQ(26, spanInfo.CalcPercentageToPxPosition(16 / 20.0f));
  EXPECT_EQ(27, spanInfo.CalcPercentageToPxPosition(17 / 20.0f));

  EXPECT_EQ(28, spanInfo.CalcPercentageToPxPosition(18 / 20.0f));
  EXPECT_EQ(29, spanInfo.CalcPercentageToPxPosition(19 / 20.0f));
  EXPECT_EQ(30, spanInfo.CalcPercentageToPxPosition(20 / 20.0f));
}


TEST(TestControlLogic_SliderPixelSpanInfo, CalcPercentageToPxPosition_OutOfRange_Low)
{
  UI::SliderPixelSpanInfo spanInfo(10, 21, false);

  EXPECT_EQ(10, spanInfo.CalcPercentageToPxPosition(-0.1f));
  EXPECT_EQ(10, spanInfo.CalcPercentageToPxPosition(-1.0f));
  EXPECT_EQ(10, spanInfo.CalcPercentageToPxPosition(std::numeric_limits<float>::min()));
}


TEST(TestControlLogic_SliderPixelSpanInfo, CalcPercentageToPxPosition_OutOfRange_High)
{
  UI::SliderPixelSpanInfo spanInfo(10, 21, false);

  EXPECT_EQ(30, spanInfo.CalcPercentageToPxPosition(1.0f));
  EXPECT_EQ(30, spanInfo.CalcPercentageToPxPosition(1.2f));
  EXPECT_EQ(30, spanInfo.CalcPercentageToPxPosition(std::numeric_limits<float>::max()));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestControlLogic_SliderPixelSpanInfo, CalculatePercentage_Reversed)
{
  UI::SliderPixelSpanInfo spanInfo(0, 21, true);

  EXPECT_FLOAT_EQ(20 / 20.0f, spanInfo.CalculatePercentage(0));
  EXPECT_FLOAT_EQ(19 / 20.0f, spanInfo.CalculatePercentage(1));
  EXPECT_FLOAT_EQ(18 / 20.0f, spanInfo.CalculatePercentage(2));

  EXPECT_FLOAT_EQ(17 / 20.0f, spanInfo.CalculatePercentage(3));
  EXPECT_FLOAT_EQ(16 / 20.0f, spanInfo.CalculatePercentage(4));
  EXPECT_FLOAT_EQ(15 / 20.0f, spanInfo.CalculatePercentage(5));
  EXPECT_FLOAT_EQ(14 / 20.0f, spanInfo.CalculatePercentage(6));
  EXPECT_FLOAT_EQ(13 / 20.0f, spanInfo.CalculatePercentage(7));

  EXPECT_FLOAT_EQ(12 / 20.0f, spanInfo.CalculatePercentage(8));
  EXPECT_FLOAT_EQ(11 / 20.0f, spanInfo.CalculatePercentage(9));
  EXPECT_FLOAT_EQ(10 / 20.0f, spanInfo.CalculatePercentage(10));
  EXPECT_FLOAT_EQ(9 / 20.0f, spanInfo.CalculatePercentage(11));
  EXPECT_FLOAT_EQ(8 / 20.0f, spanInfo.CalculatePercentage(12));

  EXPECT_FLOAT_EQ(7 / 20.0f, spanInfo.CalculatePercentage(13));
  EXPECT_FLOAT_EQ(6 / 20.0f, spanInfo.CalculatePercentage(14));
  EXPECT_FLOAT_EQ(5 / 20.0f, spanInfo.CalculatePercentage(15));
  EXPECT_FLOAT_EQ(4 / 20.0f, spanInfo.CalculatePercentage(16));
  EXPECT_FLOAT_EQ(3 / 20.0f, spanInfo.CalculatePercentage(17));

  EXPECT_FLOAT_EQ(2 / 20.0f, spanInfo.CalculatePercentage(18));
  EXPECT_FLOAT_EQ(1 / 20.0f, spanInfo.CalculatePercentage(19));
  EXPECT_FLOAT_EQ(0 / 20.0f, spanInfo.CalculatePercentage(20));
}


TEST(TestControlLogic_SliderPixelSpanInfo, CalculatePercentage_Reversed_Offset)
{
  UI::SliderPixelSpanInfo spanInfo(10, 21, true);

  EXPECT_FLOAT_EQ(20 / 20.0f, spanInfo.CalculatePercentage(10));
  EXPECT_FLOAT_EQ(19 / 20.0f, spanInfo.CalculatePercentage(11));
  EXPECT_FLOAT_EQ(18 / 20.0f, spanInfo.CalculatePercentage(12));

  EXPECT_FLOAT_EQ(17 / 20.0f, spanInfo.CalculatePercentage(13));
  EXPECT_FLOAT_EQ(16 / 20.0f, spanInfo.CalculatePercentage(14));
  EXPECT_FLOAT_EQ(15 / 20.0f, spanInfo.CalculatePercentage(15));
  EXPECT_FLOAT_EQ(14 / 20.0f, spanInfo.CalculatePercentage(16));
  EXPECT_FLOAT_EQ(13 / 20.0f, spanInfo.CalculatePercentage(17));

  EXPECT_FLOAT_EQ(12 / 20.0f, spanInfo.CalculatePercentage(18));
  EXPECT_FLOAT_EQ(11 / 20.0f, spanInfo.CalculatePercentage(19));
  EXPECT_FLOAT_EQ(10 / 20.0f, spanInfo.CalculatePercentage(20));
  EXPECT_FLOAT_EQ(9 / 20.0f, spanInfo.CalculatePercentage(21));
  EXPECT_FLOAT_EQ(8 / 20.0f, spanInfo.CalculatePercentage(22));

  EXPECT_FLOAT_EQ(7 / 20.0f, spanInfo.CalculatePercentage(23));
  EXPECT_FLOAT_EQ(6 / 20.0f, spanInfo.CalculatePercentage(24));
  EXPECT_FLOAT_EQ(5 / 20.0f, spanInfo.CalculatePercentage(25));
  EXPECT_FLOAT_EQ(4 / 20.0f, spanInfo.CalculatePercentage(26));
  EXPECT_FLOAT_EQ(3 / 20.0f, spanInfo.CalculatePercentage(27));

  EXPECT_FLOAT_EQ(2 / 20.0f, spanInfo.CalculatePercentage(28));
  EXPECT_FLOAT_EQ(1 / 20.0f, spanInfo.CalculatePercentage(29));
  EXPECT_FLOAT_EQ(0 / 20.0f, spanInfo.CalculatePercentage(30));
}


TEST(TestControlLogic_SliderPixelSpanInfo, CalculatePercentage_Reversed_OutOfRange_Low)
{
  UI::SliderPixelSpanInfo spanInfo(10, 21, true);

  EXPECT_FLOAT_EQ(1.0f, spanInfo.CalculatePercentage(9));
  EXPECT_FLOAT_EQ(1.0f, spanInfo.CalculatePercentage(0));
  EXPECT_FLOAT_EQ(1.0f, spanInfo.CalculatePercentage(std::numeric_limits<int32_t>::min()));
}


TEST(TestControlLogic_SliderPixelSpanInfo, CalculatePercentage_Reversed_OutOfRange_High)
{
  UI::SliderPixelSpanInfo spanInfo(10, 21, true);

  EXPECT_FLOAT_EQ(0.0f, spanInfo.CalculatePercentage(31));
  EXPECT_FLOAT_EQ(0.0f, spanInfo.CalculatePercentage(32));
  EXPECT_FLOAT_EQ(0.0f, spanInfo.CalculatePercentage(std::numeric_limits<int32_t>::max()));
}


TEST(TestControlLogic_SliderPixelSpanInfo, CalcPercentageToPxPosition_Reversed)
{
  UI::SliderPixelSpanInfo spanInfo(10, 21, true);

  EXPECT_EQ(30, spanInfo.CalcPercentageToPxPosition(0 / 20.0f));
  EXPECT_EQ(29, spanInfo.CalcPercentageToPxPosition(1 / 20.0f));
  EXPECT_EQ(28, spanInfo.CalcPercentageToPxPosition(2 / 20.0f));

  EXPECT_EQ(27, spanInfo.CalcPercentageToPxPosition(3 / 20.0f));
  EXPECT_EQ(26, spanInfo.CalcPercentageToPxPosition(4 / 20.0f));
  EXPECT_EQ(25, spanInfo.CalcPercentageToPxPosition(5 / 20.0f));
  EXPECT_EQ(24, spanInfo.CalcPercentageToPxPosition(6 / 20.0f));
  EXPECT_EQ(23, spanInfo.CalcPercentageToPxPosition(7 / 20.0f));

  EXPECT_EQ(22, spanInfo.CalcPercentageToPxPosition(8 / 20.0f));
  EXPECT_EQ(21, spanInfo.CalcPercentageToPxPosition(9 / 20.0f));
  EXPECT_EQ(20, spanInfo.CalcPercentageToPxPosition(10 / 20.0f));
  EXPECT_EQ(19, spanInfo.CalcPercentageToPxPosition(11 / 20.0f));
  EXPECT_EQ(18, spanInfo.CalcPercentageToPxPosition(12 / 20.0f));

  EXPECT_EQ(17, spanInfo.CalcPercentageToPxPosition(13 / 20.0f));
  EXPECT_EQ(16, spanInfo.CalcPercentageToPxPosition(14 / 20.0f));
  EXPECT_EQ(15, spanInfo.CalcPercentageToPxPosition(15 / 20.0f));
  EXPECT_EQ(14, spanInfo.CalcPercentageToPxPosition(16 / 20.0f));
  EXPECT_EQ(13, spanInfo.CalcPercentageToPxPosition(17 / 20.0f));

  EXPECT_EQ(12, spanInfo.CalcPercentageToPxPosition(18 / 20.0f));
  EXPECT_EQ(11, spanInfo.CalcPercentageToPxPosition(19 / 20.0f));
  EXPECT_EQ(10, spanInfo.CalcPercentageToPxPosition(20 / 20.0f));
}


TEST(TestControlLogic_SliderPixelSpanInfo, CalcPercentageToPxPosition_Reversed_OutOfRange_Low)
{
  UI::SliderPixelSpanInfo spanInfo(10, 21, true);

  EXPECT_EQ(30, spanInfo.CalcPercentageToPxPosition(-0.1f));
  EXPECT_EQ(30, spanInfo.CalcPercentageToPxPosition(-1.0f));
  EXPECT_EQ(30, spanInfo.CalcPercentageToPxPosition(std::numeric_limits<float>::min()));
}


TEST(TestControlLogic_SliderPixelSpanInfo, CalcPercentageToPxPosition_Reversed_OutOfRange_High)
{
  UI::SliderPixelSpanInfo spanInfo(10, 21, true);

  EXPECT_EQ(10, spanInfo.CalcPercentageToPxPosition(1.0f));
  EXPECT_EQ(10, spanInfo.CalcPercentageToPxPosition(1.2f));
  EXPECT_EQ(10, spanInfo.CalcPercentageToPxPosition(std::numeric_limits<float>::max()));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------


//
