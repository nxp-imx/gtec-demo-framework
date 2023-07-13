/****************************************************************************************************************************************************
 * Copyright 2020, 2023 NXP
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
#include <FslSimpleUI/Base/Control/Logic/SliderDragLogic.hpp>

using namespace Fsl;

namespace
{
  using TestControlLogic_SliderDragLogic = TestFixtureFslBase;
}


TEST(TestControlLogic_SliderDragLogic, Construct)
{
  UI::SliderDragLogic logic;
  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(PxValue(0), logic.GetPositionPx(logic.GetPercentage()));

  auto spanInfo = logic.GetSpanInfo();
  EXPECT_EQ(PxValue(0), spanInfo.GetStartPx());
  EXPECT_EQ(PxValue(0), spanInfo.GetLengthPx());
  EXPECT_FALSE(spanInfo.IsReversedDirection());
}


TEST(TestControlLogic_SliderDragLogic, Construct_Span)
{
  const PxValue startPx(10);
  const auto lengthPx = PxSize1D::Create(5);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));

  auto spanInfo = logic.GetSpanInfo();
  EXPECT_EQ(startPx, spanInfo.GetStartPx());
  EXPECT_EQ(lengthPx, spanInfo.GetLengthPx());
  EXPECT_FALSE(spanInfo.IsReversedDirection());
}


TEST(TestControlLogic_SliderDragLogic, SetPercentage_InRange_MinValue)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  logic.SetPercentage(1.0f);

  EXPECT_TRUE(logic.SetPercentage(0.0f));

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));

  {    // Calling it again should be a no-op
    EXPECT_FALSE(logic.SetPercentage(0.0f));

    EXPECT_EQ(0.0f, logic.GetPercentage());
    EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));
  }
}


TEST(TestControlLogic_SliderDragLogic, SetPercentage_InRange_MiddleValue)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetPercentage(0.5f));

  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + ((lengthPx - PxValue(1)) / PxValue(2))), logic.GetPositionPx(logic.GetPercentage()));

  {    // Calling it again should be a no-op
    EXPECT_FALSE(logic.SetPercentage(0.5f));

    EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + ((lengthPx - PxValue(1)) / PxValue(2))), logic.GetPositionPx(logic.GetPercentage()));
  }
}


TEST(TestControlLogic_SliderDragLogic, SetPercentage_InRange_MaxValue)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetPercentage(1.0f));

  EXPECT_EQ(1.0f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + lengthPx - PxValue(1)), logic.GetPositionPx(logic.GetPercentage()));

  {    // Calling it again should be a no-op
    EXPECT_FALSE(logic.SetPercentage(1.0f));

    EXPECT_EQ(1.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + lengthPx - PxValue(1)), logic.GetPositionPx(logic.GetPercentage()));
  }
}


TEST(TestControlLogic_SliderDragLogic, SetPercentage_OutOfRange_Low)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  logic.SetPercentage(1.0f);

  EXPECT_TRUE(logic.SetPercentage(-0.1f));

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));

  {    // Calling it again with a out of range value  should be a no-op
    EXPECT_FALSE(logic.SetPercentage(-10.0f));

    EXPECT_EQ(0.0f, logic.GetPercentage());
    EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));
  }
}


TEST(TestControlLogic_SliderDragLogic, SetPercentage_OutOfRange_High)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.SetPercentage(1.2f));

  EXPECT_EQ(1.0f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + lengthPx - PxValue(1)), logic.GetPositionPx(logic.GetPercentage()));

  {    // Calling it again should be a no-op
    EXPECT_FALSE(logic.SetPercentage(10.0f));

    EXPECT_EQ(1.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + lengthPx - PxValue(1)), logic.GetPositionPx(logic.GetPercentage()));
  }
}


TEST(TestControlLogic_SliderDragLogic, TryBeginDrag_InRange_MinA)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));
}


TEST(TestControlLogic_SliderDragLogic, TryBeginDrag_InRange_MinB)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));
  logic.SetPercentage(0.5f);

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));
}

TEST(TestControlLogic_SliderDragLogic, TryBeginDrag_InRange_Middle)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + PxValue(100)));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + PxValue(100), logic.GetPositionPx(logic.GetPercentage()));
}

TEST(TestControlLogic_SliderDragLogic, TryBeginDrag_InRange_Max)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + PxValue(200)));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(1.0f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + lengthPx - PxValue(1)), logic.GetPositionPx(logic.GetPercentage()));
}


TEST(TestControlLogic_SliderDragLogic, TryBeginDrag_OutOfRange_Low1)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx - PxValue(1)));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));
}


TEST(TestControlLogic_SliderDragLogic, TryBeginDrag_OutOfRange_Low2)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx - PxValue(100)));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));
}


TEST(TestControlLogic_SliderDragLogic, TryBeginDrag_OutOfRange_High1)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + lengthPx));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(1.0f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + lengthPx - PxValue(1)), logic.GetPositionPx(logic.GetPercentage()));
}


TEST(TestControlLogic_SliderDragLogic, TryBeginDrag_OutOfRange_High2)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + lengthPx + PxValue(100)));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(1.0f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + lengthPx - PxValue(1)), logic.GetPositionPx(logic.GetPercentage()));
}

TEST(TestControlLogic_SliderDragLogic, TryDrag_InRange)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + PxValue(100)));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + PxValue(100), logic.GetPositionPx(logic.GetPercentage()));

  EXPECT_TRUE(logic.TryDrag(startPx));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));

  EXPECT_TRUE(logic.TryDrag(startPx + lengthPx - PxValue(1)));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(1.0f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + lengthPx - PxValue(1)), logic.GetPositionPx(logic.GetPercentage()));
}

TEST(TestControlLogic_SliderDragLogic, TryDrag_NotDraging)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));
  logic.SetPercentage(0.5f);

  EXPECT_FALSE(logic.IsDragging());

  // As we are not dragging, the TryDrag does nothing
  EXPECT_FALSE(logic.TryDrag(startPx));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
}


TEST(TestControlLogic_SliderDragLogic, EndDrag_InRange)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + PxValue(100)));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + PxValue(100), logic.GetPositionPx(logic.GetPercentage()));

  EXPECT_TRUE(logic.TryDrag(startPx));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));

  EXPECT_TRUE(logic.EndDrag(startPx + lengthPx - PxValue(1)));
  EXPECT_FALSE(logic.IsDragging());
  EXPECT_EQ(1.0f, logic.GetPercentage());
  // lengthPx -1 as the pixel at length is not included (its out of bounds)
  EXPECT_EQ((startPx + lengthPx - PxValue(1)), logic.GetPositionPx(logic.GetPercentage()));
}


TEST(TestControlLogic_SliderDragLogic, CancelDrag)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));
  logic.SetPercentage(0.5f);
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + PxValue(100), logic.GetPositionPx(logic.GetPercentage()));

  EXPECT_TRUE(logic.TryBeginDrag(startPx));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));

  EXPECT_TRUE(logic.TryDrag(startPx + lengthPx));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + lengthPx - PxValue(1), logic.GetPositionPx(logic.GetPercentage()));

  // The cancel restores the pre-drag state
  EXPECT_TRUE(logic.CancelDrag());
  EXPECT_FALSE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + PxValue(100), logic.GetPositionPx(logic.GetPercentage()));
}


TEST(TestControlLogic_SliderDragLogic, EndDrag_NotDraging)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));
  logic.SetPercentage(0.5f);

  EXPECT_FALSE(logic.IsDragging());

  // As we are not dragging, the end drag does nothing
  EXPECT_FALSE(logic.EndDrag(startPx));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
}


TEST(TestControlLogic_SliderDragLogic, CancelDrag_NotDraging)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));
  logic.SetPercentage(0.5f);

  EXPECT_FALSE(logic.IsDragging());

  // As we are not dragging, the end drag does nothing
  EXPECT_FALSE(logic.CancelDrag());

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
}


TEST(TestControlLogic_SliderDragLogic, SetEnabled)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

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


TEST(TestControlLogic_SliderDragLogic, SetEnabled_True_CancelDrag)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));


  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());
  {    // force the initial value to be something other than the default
    logic.SetPercentage(1.0f);
    EXPECT_EQ(1.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + lengthPx - PxValue(1)), logic.GetPositionPx(logic.GetPercentage()));
  }

  // Initiate drag from the middle
  EXPECT_TRUE(logic.TryBeginDrag(startPx + PxValue(100)));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + PxValue(100), logic.GetPositionPx(logic.GetPercentage()));

  {    // Setting to enabled does nothing as we are already enabled
    auto flags = logic.SetEnabled(true);
    EXPECT_FALSE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::Completed));
    EXPECT_FALSE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::DragCancelled));
    EXPECT_TRUE(logic.IsEnabled());
  }

  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + PxValue(100), logic.GetPositionPx(logic.GetPercentage()));
}

TEST(TestControlLogic_SliderDragLogic, SetEnabled_False_CancelDrag)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));


  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());
  {    // force the initial value to be something other than the default
    logic.SetPercentage(1.0f);
    EXPECT_EQ(1.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + lengthPx - PxValue(1)), logic.GetPositionPx(logic.GetPercentage()));
  }

  // Initiate drag from the middle
  EXPECT_TRUE(logic.TryBeginDrag(startPx + PxValue(100)));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + PxValue(100), logic.GetPositionPx(logic.GetPercentage()));

  // Cancel the drag, restoring the pre-drag state
  auto flags = logic.SetEnabled(false);
  EXPECT_TRUE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::Completed));
  EXPECT_TRUE(UI::SliderResultFlagsUtil::IsFlagged(flags, UI::SliderResultFlags::DragCancelled));
  EXPECT_FALSE(logic.IsEnabled());

  {    // Verify that we returned to the pre-drag state
    EXPECT_FALSE(logic.IsDragging());
    EXPECT_EQ(1.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + lengthPx - PxValue(1)), logic.GetPositionPx(logic.GetPercentage()));
  }
}


TEST(TestControlLogic_SliderDragLogic, TryBeginDrag_Disabled)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));
  logic.SetEnabled(false);

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));

  // The begin drag should be a 'no-op' when disabled
  EXPECT_FALSE(logic.TryBeginDrag(startPx + PxValue(100)));
  EXPECT_FALSE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));
}


TEST(TestControlLogic_SliderDragLogic, TryDrag_Disabled)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));
  logic.SetEnabled(false);

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));

  // The drag should be a 'no-op' when disabled
  EXPECT_FALSE(logic.TryDrag(startPx + PxValue(100)));
  EXPECT_FALSE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));
}


TEST(TestControlLogic_SliderDragLogic, EndDrag_Disabled)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));
  logic.SetEnabled(false);

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));

  // The EndDrag should be a 'no-op' when disabled
  EXPECT_FALSE(logic.EndDrag(startPx + PxValue(100)));
  EXPECT_FALSE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));
}


TEST(TestControlLogic_SliderDragLogic, CancelDrag_Disabled)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));
  logic.SetEnabled(false);

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));

  // The EndDrag should be a 'no-op' when disabled
  EXPECT_FALSE(logic.CancelDrag());
  EXPECT_FALSE(logic.IsDragging());
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));
}

TEST(TestControlLogic_SliderDragLogic, SetSpanInfo_Percentage0)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));
  logic.SetPercentage(0);
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  const PxValue newStartPx(55);
  const auto newLengthPx = PxSize1D::Create(11);
  logic.SetSpanInfo(UI::SliderPixelSpanInfo(newStartPx, newLengthPx));
  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(newStartPx, logic.GetPositionPx(logic.GetPercentage()));
}

TEST(TestControlLogic_SliderDragLogic, SetSpanInfo_Percentage1)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));
  logic.SetPercentage(1);
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + lengthPx - PxValue(1), logic.GetPositionPx(logic.GetPercentage()));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  const PxValue newStartPx(55);
  const auto newLengthPx = PxSize1D::Create(11);
  EXPECT_TRUE(logic.SetSpanInfo(UI::SliderPixelSpanInfo(newStartPx, newLengthPx)));
  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(newStartPx + newLengthPx - PxValue(1), logic.GetPositionPx(logic.GetPercentage()));
}

TEST(TestControlLogic_SliderDragLogic, SetSpanInfo_Same_CancelDrag)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());
  {    // force the initial value to be something other than the default
    logic.SetPercentage(1.0f);
    EXPECT_EQ(1.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + lengthPx - PxValue(1)), logic.GetPositionPx(logic.GetPercentage()));
  }

  // Initiate drag from the middle
  EXPECT_TRUE(logic.TryBeginDrag(startPx + PxValue(100)));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + PxValue(100), logic.GetPositionPx(logic.GetPercentage()));

  {    // SetSpanInfo to the same value does nothing
    EXPECT_FALSE(logic.SetSpanInfo(UI::SliderPixelSpanInfo(startPx, lengthPx)));
  }

  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + PxValue(100), logic.GetPositionPx(logic.GetPercentage()));
}

TEST(TestControlLogic_SliderDragLogic, SetSpanInfo_New_CancelDrag)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());
  {    // force the initial value to be something other than the default
    logic.SetPercentage(1.0f);
    EXPECT_EQ(1.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((startPx + lengthPx - PxValue(1)), logic.GetPositionPx(logic.GetPercentage()));
  }

  // Initiate drag from the middle
  EXPECT_TRUE(logic.TryBeginDrag(startPx + PxValue(100)));
  EXPECT_TRUE(logic.IsDragging());
  EXPECT_FLOAT_EQ(0.5f, logic.GetPercentage());
  EXPECT_EQ(startPx + PxValue(100), logic.GetPositionPx(logic.GetPercentage()));

  const PxValue newStartPx(55);
  const auto newLengthPx = PxSize1D::Create(11);
  {    // SetSpanInfo to a new value recalculates the percentage
    EXPECT_TRUE(logic.SetSpanInfo(UI::SliderPixelSpanInfo(newStartPx, newLengthPx)));
  }

  {    // Verify that the drag wasn't broken
    EXPECT_TRUE(logic.IsDragging());
    EXPECT_EQ(1.0f, logic.GetPercentage());
    // lengthPx -1 as the pixel at length is not included (its out of bounds)
    EXPECT_EQ((newStartPx + newLengthPx - PxValue(1)), logic.GetPositionPx(logic.GetPercentage()));
  }
}


TEST(TestControlLogic_SliderDragLogic, Construct_Span_Reversed)
{
  const PxValue startPx(10);
  const auto lengthPx = PxSize1D::Create(5);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx, true));

  EXPECT_FALSE(logic.IsDragging());
  EXPECT_TRUE(logic.IsEnabled());

  EXPECT_EQ(0.0f, logic.GetPercentage());
  EXPECT_EQ(startPx + lengthPx - PxValue(1), logic.GetPositionPx(logic.GetPercentage()));

  auto spanInfo = logic.GetSpanInfo();
  EXPECT_EQ(startPx, spanInfo.GetStartPx());
  EXPECT_EQ(lengthPx, spanInfo.GetLengthPx());
  EXPECT_TRUE(spanInfo.IsReversedDirection());
}


TEST(TestControlLogic_SliderDragLogic, SetPercentage_SpanReversed)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx, true));

  logic.SetPercentage(1.0f);

  EXPECT_EQ(1.0f, logic.GetPercentage());
  EXPECT_EQ(startPx, logic.GetPositionPx(logic.GetPercentage()));
}


TEST(TestControlLogic_SliderDragLogic, TryDrag_SpanReversed)
{
  const PxValue startPx(100);
  const auto lengthPx = PxSize1D::Create(201);
  UI::SliderDragLogic logic(UI::SliderPixelSpanInfo(startPx, lengthPx, true));

  EXPECT_TRUE(logic.TryBeginDrag(startPx + PxValue(50)));

  EXPECT_FLOAT_EQ(0.75f, logic.GetPercentage());
  EXPECT_EQ(startPx + PxValue(50), logic.GetPositionPx(logic.GetPercentage()));
}
