/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/Log/Math/LogVector2.hpp>
#include <FslSimpleUI/Base/UnitTest/Control/UTControl.hpp>
#include <FslSimpleUI/Base/UnitTest/Control/UTImageControl.hpp>
#include <FslSimpleUI/Base/UnitTest/TestFixtureFslSimpleUIUITree.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/LayoutHelper.hpp>
#include <array>

using namespace Fsl;

namespace
{
  class TestGridLayout : public TestFixtureFslSimpleUIUITree
  {
  protected:
    Vector2 m_defaultSize{1920, 1080};
    Rect m_finalRect{1, 2, 1920, 1080};

  public:
    TestGridLayout() = default;
  };
}


TEST_F(TestGridLayout, Construct)
{
  UI::GridLayout layout(m_windowContext);
}


TEST_F(TestGridLayout, MeasureNoDefs_NoChildren)
{
  UI::GridLayout layout(m_windowContext);
  layout.Measure(m_defaultSize);

  ASSERT_EQ(layout.DesiredSize(), Vector2(0.0f, 0.0f));

  layout.Arrange(m_finalRect);
}


TEST_F(TestGridLayout, MeasureFixedDefs_NoChildren)
{
  UI::GridLayout layout(m_windowContext);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));

  layout.Measure(m_defaultSize);

  ASSERT_EQ(layout.DesiredSize(), Vector2(fixedWidth, fixedHeight));

  layout.Arrange(m_finalRect);
}


TEST_F(TestGridLayout, MeasureFixedDefs2_NoChildren)
{
  UI::GridLayout layout(m_windowContext);

  const float fixedWidth1 = 20.0f;
  const float fixedHeight1 = 10.0f;
  const float fixedWidth2 = 5.0f;
  const float fixedHeight2 = 3.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth1));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth2));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight1));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight2));

  layout.Measure(m_defaultSize);

  ASSERT_EQ(layout.DesiredSize(), Vector2(fixedWidth1 + fixedWidth2, fixedHeight1 + fixedHeight2));

  layout.Arrange(m_finalRect);
}


TEST_F(TestGridLayout, MeasureAutoDefs_NoChildren)
{
  UI::GridLayout layout(m_windowContext);

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));

  layout.Measure(m_defaultSize);

  ASSERT_EQ(layout.DesiredSize(), Vector2(0, 0));

  layout.Arrange(m_finalRect);
}


TEST_F(TestGridLayout, MeasureWeightDefs_NoChildren)
{
  UI::GridLayout layout(m_windowContext);

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));

  layout.Measure(m_defaultSize);

  // Since its pure "weight" grid we expect it to consume all available space
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  layout.Arrange(m_finalRect);
}


TEST_F(TestGridLayout, MeasureNoDefs_ChildNoSize)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  layout.Measure(m_defaultSize);

  ASSERT_EQ(layout.DesiredSize(), Vector2(0.0f, 0.0f));

  layout.Arrange(m_finalRect);
}

TEST_F(TestGridLayout, MeasureNoDefsX_ChildNoSize)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  const float fixedHeight = 10.0f;

  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.SetRow(child, 0);

  layout.Measure(m_defaultSize);

  ASSERT_EQ(layout.DesiredSize(), Vector2(0.0f, fixedHeight));

  layout.Arrange(m_finalRect);
}


TEST_F(TestGridLayout, MeasureNoDefsY_ChildNoSize)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.SetColumn(child, 0);

  layout.Measure(m_defaultSize);

  ASSERT_EQ(layout.DesiredSize(), Vector2(fixedWidth, 0.0f));

  layout.Arrange(m_finalRect);
}


TEST_F(TestGridLayout, MeasureFixedDefs_ChildNoSize)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.Set(child, 0, 0);

  layout.Measure(m_defaultSize);

  ASSERT_EQ(layout.DesiredSize(), Vector2(fixedWidth, fixedHeight));

  layout.Arrange(m_finalRect);
}


TEST_F(TestGridLayout, MeasureFixedDefs2_ChildNoSize)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  const float fixedWidth1 = 20.0f;
  const float fixedHeight1 = 10.0f;
  const float fixedWidth2 = 5.0f;
  const float fixedHeight2 = 3.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth1));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth2));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight1));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight2));
  layout.Set(child, 0, 0);

  layout.Measure(m_defaultSize);

  ASSERT_EQ(layout.DesiredSize(), Vector2(fixedWidth1 + fixedWidth2, fixedHeight1 + fixedHeight2));

  layout.Arrange(m_finalRect);
}


TEST_F(TestGridLayout, MeasureAutoDefs_ChildNoSize)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.Set(child, 0, 0);

  layout.Measure(m_defaultSize);

  ASSERT_EQ(layout.DesiredSize(), Vector2(0, 0));

  layout.Arrange(m_finalRect);
}

TEST_F(TestGridLayout, MeasureWeightDefs_ChildNoSize)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.Set(child, 0, 0);

  layout.Measure(m_defaultSize);

  // Since its pure "weight" grid we expect it to consume all available space
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  layout.Arrange(m_finalRect);
}

TEST_F(TestGridLayout, MeasureDefsFixedWeightXFixedWeight_ChildNoSizeAt0x0)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.Set(child, 0, 0);

  layout.Measure(m_defaultSize);

  // We expect the grid to consume all space as we have a weight row and column
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  layout.Arrange(m_finalRect);
}

TEST_F(TestGridLayout, MeasureDefsFixedWeightXFixedWeight_ChildNoSizeAt1x0)
{
  UI::GridLayout layout(m_windowContext);

  const Vector2 controlDesiredSize(30.0f, 15.0f);

  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.Set(child, 1, 0);

  layout.Measure(m_defaultSize);

  // We expect the grid to consume all space as we have a weight row and column
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  layout.Arrange(m_finalRect);
}

TEST_F(TestGridLayout, MeasureDefsFixedWeightXFixedWeight_ChildNoSizeAt0x1)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.Set(child, 0, 1);

  layout.Measure(m_defaultSize);

  // We expect the grid to consume all space as we have a weight row and column
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  layout.Arrange(m_finalRect);
}

TEST_F(TestGridLayout, MeasureDefsFixedWeightXFixedWeight_ChildNoSizeAt1x1)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.Set(child, 1, 1);

  layout.Measure(m_defaultSize);

  // We expect the grid to consume all space as we have a weight row and column
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  layout.Arrange(m_finalRect);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt0x0)
{
  UI::GridLayout layout(m_windowContext);

  const Vector2 controlDesiredSize(30.0f, 15.0f);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.Set(child, 0, 0);

  layout.Measure(m_defaultSize);

  // We expect the grid to consume all space as we have a weight row and column
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  // Fixed x Fixed
  ASSERT_FLOAT_EQ(child->TestLastMeasureOverrideAvailableSize.X, fixedWidth);
  ASSERT_FLOAT_EQ(child->TestLastMeasureOverrideAvailableSize.Y, fixedHeight);

  layout.Arrange(m_finalRect);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt1x0)
{
  UI::GridLayout layout(m_windowContext);

  const Vector2 controlDesiredSize(30.0f, 15.0f);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.Set(child, 1, 0);

  layout.Measure(m_defaultSize);

  // We expect the grid to consume all space as we have a weight row and column
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  // Auto x fixed
  ASSERT_TRUE(std::isinf(child->TestLastMeasureOverrideAvailableSize.X));
  ASSERT_FLOAT_EQ(child->TestLastMeasureOverrideAvailableSize.Y, fixedHeight);

  layout.Arrange(m_finalRect);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt2x0)
{
  UI::GridLayout layout(m_windowContext);

  const Vector2 controlDesiredSize(30.0f, 15.0f);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.Set(child, 2, 0);

  layout.Measure(m_defaultSize);

  // We expect the grid to consume all space as we have a weight row and column
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  // Weight x Fixed
  ASSERT_FLOAT_EQ(child->TestLastMeasureOverrideAvailableSize.X, m_defaultSize.X - fixedWidth);
  ASSERT_FLOAT_EQ(child->TestLastMeasureOverrideAvailableSize.Y, fixedHeight);

  layout.Arrange(m_finalRect);
}


TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt0x1)
{
  UI::GridLayout layout(m_windowContext);

  const Vector2 controlDesiredSize(30.0f, 15.0f);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.Set(child, 0, 1);

  layout.Measure(m_defaultSize);

  // We expect the grid to consume all space as we have a weight row and column
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  // Fixed x Auto
  ASSERT_FLOAT_EQ(child->TestLastMeasureOverrideAvailableSize.X, fixedWidth);
  ASSERT_TRUE(std::isinf(child->TestLastMeasureOverrideAvailableSize.Y));

  layout.Arrange(m_finalRect);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt1x1)
{
  UI::GridLayout layout(m_windowContext);

  const Vector2 controlDesiredSize(30.0f, 15.0f);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.Set(child, 1, 1);

  layout.Measure(m_defaultSize);

  // We expect the grid to consume all space as we have a weight row and column
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  // Auto x auto
  ASSERT_TRUE(std::isinf(child->TestLastMeasureOverrideAvailableSize.X));
  ASSERT_TRUE(std::isinf(child->TestLastMeasureOverrideAvailableSize.Y));

  layout.Arrange(m_finalRect);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt2x1)
{
  UI::GridLayout layout(m_windowContext);

  const Vector2 controlDesiredSize(30.0f, 15.0f);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.Set(child, 2, 1);

  layout.Measure(m_defaultSize);

  // We expect the grid to consume all space as we have a weight row and column
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  // Weight x auto
  ASSERT_FLOAT_EQ(child->TestLastMeasureOverrideAvailableSize.X, m_defaultSize.X - fixedWidth);
  ASSERT_TRUE(std::isinf(child->TestLastMeasureOverrideAvailableSize.Y));

  layout.Arrange(m_finalRect);
}


TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt0x2)
{
  UI::GridLayout layout(m_windowContext);

  const Vector2 controlDesiredSize(30.0f, 15.0f);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.Set(child, 0, 2);

  layout.Measure(m_defaultSize);

  // We expect the grid to consume all space as we have a weight row and column
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  // Fixed x weight
  ASSERT_FLOAT_EQ(child->TestLastMeasureOverrideAvailableSize.X, fixedWidth);
  ASSERT_FLOAT_EQ(child->TestLastMeasureOverrideAvailableSize.Y, m_defaultSize.Y - fixedHeight);

  layout.Arrange(m_finalRect);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt1x2)
{
  UI::GridLayout layout(m_windowContext);

  const Vector2 controlDesiredSize(30.0f, 15.0f);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.Set(child, 1, 2);

  layout.Measure(m_defaultSize);

  // We expect the grid to consume all space as we have a weight row and column
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  // Auto x weight
  ASSERT_TRUE(std::isinf(child->TestLastMeasureOverrideAvailableSize.X));
  ASSERT_FLOAT_EQ(child->TestLastMeasureOverrideAvailableSize.Y, m_defaultSize.Y - fixedHeight);

  layout.Arrange(m_finalRect);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt2x2)
{
  UI::GridLayout layout(m_windowContext);

  const Vector2 controlDesiredSize(30.0f, 15.0f);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.Set(child, 2, 2);

  layout.Measure(m_defaultSize);

  // We expect the grid to consume all space as we have a weight row and column
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  // Weight x weight
  ASSERT_FLOAT_EQ(child->TestLastMeasureOverrideAvailableSize.X, m_defaultSize.X - fixedWidth);
  ASSERT_FLOAT_EQ(child->TestLastMeasureOverrideAvailableSize.Y, m_defaultSize.Y - fixedHeight);

  layout.Arrange(m_finalRect);
}

TEST_F(TestGridLayout, MeasureAllDefs_9ChildNoSizeAt2x2)
{
  UI::GridLayout layout(m_windowContext);

  const Vector2 controlDesiredSize(30.0f, 15.0f);

  std::array<std::shared_ptr<UI::UTImageControl>, 3 * 3> children;
  for (auto& rEntry : children)
  {
    rEntry = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize);
    layout.AddChild(rEntry);
  }

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Weight, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Weight, 1.0f));
  for (uint32_t y = 0; y < 3; ++y)
  {
    for (uint32_t x = 0; x < 3; ++x)
    {
      layout.Set(children[x + (y * 3)], x, y);
    }
  }

  layout.Measure(m_defaultSize);

  // We expect the grid to consume all space as we have a weight row and column
  ASSERT_EQ(layout.DesiredSize(), m_defaultSize);

  // Fixed x Fixed
  ASSERT_FLOAT_EQ(children[0]->TestLastMeasureOverrideAvailableSize.X, fixedWidth);
  ASSERT_FLOAT_EQ(children[0]->TestLastMeasureOverrideAvailableSize.Y, fixedHeight);

  // Auto x fixed
  ASSERT_TRUE(std::isinf(children[1]->TestLastMeasureOverrideAvailableSize.X));
  ASSERT_FLOAT_EQ(children[1]->TestLastMeasureOverrideAvailableSize.Y, fixedHeight);

  // Weight x Fixed
  ASSERT_FLOAT_EQ(children[2]->TestLastMeasureOverrideAvailableSize.X, m_defaultSize.X - fixedWidth - controlDesiredSize.X);
  ASSERT_FLOAT_EQ(children[2]->TestLastMeasureOverrideAvailableSize.Y, fixedHeight);

  // Fixed x Auto
  ASSERT_FLOAT_EQ(children[3]->TestLastMeasureOverrideAvailableSize.X, fixedWidth);
  ASSERT_TRUE(std::isinf(children[3]->TestLastMeasureOverrideAvailableSize.Y));

  // Auto x auto
  ASSERT_TRUE(std::isinf(children[4]->TestLastMeasureOverrideAvailableSize.X));
  ASSERT_TRUE(std::isinf(children[4]->TestLastMeasureOverrideAvailableSize.Y));

  // Weight x auto
  ASSERT_FLOAT_EQ(children[5]->TestLastMeasureOverrideAvailableSize.X, m_defaultSize.X - fixedWidth - controlDesiredSize.X);
  ASSERT_TRUE(std::isinf(children[5]->TestLastMeasureOverrideAvailableSize.Y));

  // Fixed x weight
  ASSERT_FLOAT_EQ(children[6]->TestLastMeasureOverrideAvailableSize.X, fixedWidth);
  ASSERT_FLOAT_EQ(children[6]->TestLastMeasureOverrideAvailableSize.Y, m_defaultSize.Y - fixedHeight - controlDesiredSize.Y);

  // Auto x weight
  ASSERT_TRUE(std::isinf(children[7]->TestLastMeasureOverrideAvailableSize.X));
  ASSERT_FLOAT_EQ(children[7]->TestLastMeasureOverrideAvailableSize.Y, m_defaultSize.Y - fixedHeight - controlDesiredSize.Y);

  // Weight x weight
  ASSERT_FLOAT_EQ(children[8]->TestLastMeasureOverrideAvailableSize.X, m_defaultSize.X - fixedWidth - controlDesiredSize.X);
  ASSERT_FLOAT_EQ(children[8]->TestLastMeasureOverrideAvailableSize.Y, m_defaultSize.Y - fixedHeight - controlDesiredSize.Y);

  layout.Arrange(m_finalRect);
}
