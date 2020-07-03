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
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/Log/Math/LogVector2.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/PxAvailableSize.hpp>
#include <FslSimpleUI/Base/PxAvailableSizeUtil.hpp>
#include <FslSimpleUI/Base/UnitTest/Control/UTControl.hpp>
#include <FslSimpleUI/Base/UnitTest/Control/UTImageControl.hpp>
#include <FslSimpleUI/Base/UnitTest/TestFixtureFslSimpleUIUITree.hpp>
#include <array>

using namespace Fsl;

namespace
{
  class TestGridLayout : public TestFixtureFslSimpleUIUITree
  {
  protected:
    const UI::PxAvailableSize m_defaultSizePx{1920, 1080};
    const PxRectangle m_finalRectPx{1, 2, 1920, 1080};

  public:
    TestGridLayout() = default;
  };

  const SpriteUnitConverter g_unitConverter(SpriteDpConfig::BaseDpi);

}


TEST_F(TestGridLayout, Construct)
{
  UI::GridLayout layout(m_windowContext);
}


TEST_F(TestGridLayout, MeasureNoDefs_NoChildren)
{
  UI::GridLayout layout(m_windowContext);
  layout.Measure(m_defaultSizePx);

  ASSERT_EQ(layout.DesiredSizePx(), PxSize2D());

  layout.Arrange(m_finalRectPx);
}


TEST_F(TestGridLayout, MeasureFixedDefs_NoChildren)
{
  UI::GridLayout layout(m_windowContext);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));

  layout.Measure(m_defaultSizePx);

  ASSERT_EQ(layout.DesiredSizePx(), TypeConverter::UncheckedChangeTo<PxSize2D>(Vector2(fixedWidth, fixedHeight)));

  layout.Arrange(m_finalRectPx);
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

  layout.Measure(m_defaultSizePx);

  ASSERT_EQ(layout.DesiredSizePx(), TypeConverter::UncheckedChangeTo<PxSize2D>(Vector2(fixedWidth1 + fixedWidth2, fixedHeight1 + fixedHeight2)));

  layout.Arrange(m_finalRectPx);
}


TEST_F(TestGridLayout, MeasureAutoDefs_NoChildren)
{
  UI::GridLayout layout(m_windowContext);

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));

  layout.Measure(m_defaultSizePx);

  ASSERT_EQ(layout.DesiredSizePx(), PxSize2D());

  layout.Arrange(m_finalRectPx);
}


TEST_F(TestGridLayout, MeasureStarDefs_NoChildren)
{
  UI::GridLayout layout(m_windowContext);

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));

  layout.Measure(m_defaultSizePx);

  // Since its pure "star" grid we expect it to consume all available space
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  layout.Arrange(m_finalRectPx);
}


TEST_F(TestGridLayout, MeasureNoDefs_ChildNoSize)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  layout.Measure(m_defaultSizePx);

  ASSERT_EQ(layout.DesiredSizePx(), PxSize2D());

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureNoDefsX_ChildNoSize)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  const float fixedHeight = 10.0f;

  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.SetRow(child, 0);

  layout.Measure(m_defaultSizePx);

  ASSERT_EQ(layout.DesiredSizePx(), TypeConverter::UncheckedChangeTo<PxSize2D>(Vector2(0.0f, fixedHeight)));

  layout.Arrange(m_finalRectPx);
}


TEST_F(TestGridLayout, MeasureNoDefsY_ChildNoSize)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.SetColumn(child, 0);

  layout.Measure(m_defaultSizePx);

  ASSERT_EQ(layout.DesiredSizePx(), TypeConverter::UncheckedChangeTo<PxSize2D>(Vector2(fixedWidth, 0.0f)));

  layout.Arrange(m_finalRectPx);
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

  layout.Measure(m_defaultSizePx);

  ASSERT_EQ(layout.DesiredSizePx(), TypeConverter::UncheckedChangeTo<PxSize2D>(Vector2(fixedWidth, fixedHeight)));

  layout.Arrange(m_finalRectPx);
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

  layout.Measure(m_defaultSizePx);

  ASSERT_EQ(layout.DesiredSizePx(), TypeConverter::UncheckedChangeTo<PxSize2D>(Vector2(fixedWidth1 + fixedWidth2, fixedHeight1 + fixedHeight2)));

  layout.Arrange(m_finalRectPx);
}


TEST_F(TestGridLayout, MeasureAutoDefs_ChildNoSize)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.Set(child, 0, 0);

  layout.Measure(m_defaultSizePx);

  ASSERT_EQ(layout.DesiredSizePx(), PxSize2D());

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureStarDefs_ChildNoSize)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(child, 0, 0);

  layout.Measure(m_defaultSizePx);

  // Since its pure "star" grid we expect it to consume all available space
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureDefsFixedStarXFixedStar_ChildNoSizeAt0x0)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(child, 0, 0);

  layout.Measure(m_defaultSizePx);

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureDefsFixedStarXFixedStar_ChildNoSizeAt1x0)
{
  UI::GridLayout layout(m_windowContext);

  // const PxSize2D controlDesiredSizePx(30, 15);

  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(child, 1, 0);

  layout.Measure(m_defaultSizePx);

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureDefsFixedStarXFixedStar_ChildNoSizeAt0x1)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(child, 0, 1);

  layout.Measure(m_defaultSizePx);

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureDefsFixedStarXFixedStar_ChildNoSizeAt1x1)
{
  UI::GridLayout layout(m_windowContext);
  auto child = std::make_shared<UI::UTControl>(m_windowContext);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(child, 1, 1);

  layout.Measure(m_defaultSizePx);

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt0x0)
{
  UI::GridLayout layout(m_windowContext);

  const PxSize2D controlDesiredSizePx(30, 15);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSizePx);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(child, 0, 0);

  layout.Measure(m_defaultSizePx);

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  // Fixed x Fixed
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Width(), g_unitConverter.DpToPxInt32(fixedWidth));
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Height(), g_unitConverter.DpToPxInt32(fixedHeight));

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt1x0)
{
  UI::GridLayout layout(m_windowContext);

  const PxSize2D controlDesiredSizePx(30, 15);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSizePx);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(child, 1, 0);

  layout.Measure(m_defaultSizePx);

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  // Auto x fixed
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(child->TestLastMeasureOverrideAvailableSize.Width()));
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Height(), g_unitConverter.DpToPxInt32(fixedHeight));

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt2x0)
{
  UI::GridLayout layout(m_windowContext);

  const PxSize2D controlDesiredSizePx(30, 15);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSizePx);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(child, 2, 0);

  layout.Measure(m_defaultSizePx);

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  // Star x Fixed
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width() - g_unitConverter.DpToPxInt32(fixedWidth));
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Height(), g_unitConverter.DpToPxInt32(fixedHeight));

  layout.Arrange(m_finalRectPx);
}


TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt0x1)
{
  UI::GridLayout layout(m_windowContext);

  const PxSize2D controlDesiredSizePx(30, 15);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSizePx);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(child, 0, 1);

  layout.Measure(m_defaultSizePx);

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  // Fixed x Auto
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Width(), g_unitConverter.DpToPxInt32(fixedWidth));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(child->TestLastMeasureOverrideAvailableSize.Height()));

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt1x1)
{
  UI::GridLayout layout(m_windowContext);

  const PxSize2D controlDesiredSizePx(30, 15);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSizePx);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(child, 1, 1);

  layout.Measure(m_defaultSizePx);

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  // Auto x auto
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(child->TestLastMeasureOverrideAvailableSize.Width()));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(child->TestLastMeasureOverrideAvailableSize.Height()));

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt2x1)
{
  UI::GridLayout layout(m_windowContext);

  const PxSize2D controlDesiredSizePx(30, 15);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSizePx);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(child, 2, 1);

  layout.Measure(m_defaultSizePx);

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  // Star x auto
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width() - g_unitConverter.DpToPxInt32(fixedWidth));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(child->TestLastMeasureOverrideAvailableSize.Height()));

  layout.Arrange(m_finalRectPx);
}


TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt0x2)
{
  UI::GridLayout layout(m_windowContext);

  const PxSize2D controlDesiredSizePx(30, 15);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSizePx);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(child, 0, 2);

  layout.Measure(m_defaultSizePx);

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  // Fixed x star
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Width(), g_unitConverter.DpToPxInt32(fixedWidth));
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Height(), m_defaultSizePx.Height() - g_unitConverter.DpToPxInt32(fixedHeight));

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt1x2)
{
  UI::GridLayout layout(m_windowContext);

  const PxSize2D controlDesiredSizePx(30, 15);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSizePx);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(child, 1, 2);

  layout.Measure(m_defaultSizePx);

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  // Auto x star
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(child->TestLastMeasureOverrideAvailableSize.Width()));
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Height(), m_defaultSizePx.Height() - g_unitConverter.DpToPxInt32(fixedHeight));

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt2x2)
{
  UI::GridLayout layout(m_windowContext);

  const PxSize2D controlDesiredSizePx(30, 15);

  auto child = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSizePx);
  layout.AddChild(child);

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(child, 2, 2);

  layout.Measure(m_defaultSizePx);

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  // Star x star
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width() - g_unitConverter.DpToPxInt32(fixedWidth));
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Height(), m_defaultSizePx.Height() - g_unitConverter.DpToPxInt32(fixedHeight));

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_9ChildNoSizeAt2x2)
{
  UI::GridLayout layout(m_windowContext);

  const PxSize2D controlDesiredSizePx(30, 15);

  std::array<std::shared_ptr<UI::UTImageControl>, 3 * 3> children;
  for (auto& rEntry : children)
  {
    rEntry = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSizePx);
    layout.AddChild(rEntry);
  }

  const float fixedWidth = 20.0f;
  const float fixedHeight = 10.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, fixedHeight));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  for (uint32_t y = 0; y < 3; ++y)
  {
    for (uint32_t x = 0; x < 3; ++x)
    {
      layout.Set(children[x + (y * 3)], x, y);
    }
  }

  layout.Measure(m_defaultSizePx);

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  // Fixed x Fixed
  ASSERT_EQ(children[0]->TestLastMeasureOverrideAvailableSize.Width(), g_unitConverter.DpToPxInt32(fixedWidth));
  ASSERT_EQ(children[0]->TestLastMeasureOverrideAvailableSize.Height(), g_unitConverter.DpToPxInt32(fixedHeight));

  // Auto x fixed
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[1]->TestLastMeasureOverrideAvailableSize.Width()));
  ASSERT_EQ(children[1]->TestLastMeasureOverrideAvailableSize.Height(), g_unitConverter.DpToPxInt32(fixedHeight));

  // Star x Fixed
  ASSERT_EQ(children[2]->TestLastMeasureOverrideAvailableSize.Width(),
            m_defaultSizePx.Width() - controlDesiredSizePx.Width() - g_unitConverter.DpToPxInt32(fixedWidth));
  ASSERT_EQ(children[2]->TestLastMeasureOverrideAvailableSize.Height(), g_unitConverter.DpToPxInt32(fixedHeight));

  // Fixed x Auto
  ASSERT_EQ(children[3]->TestLastMeasureOverrideAvailableSize.Width(), g_unitConverter.DpToPxInt32(fixedWidth));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[3]->TestLastMeasureOverrideAvailableSize.Height()));

  // Auto x auto
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[4]->TestLastMeasureOverrideAvailableSize.Width()));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[4]->TestLastMeasureOverrideAvailableSize.Height()));

  // Star x auto
  ASSERT_EQ(children[5]->TestLastMeasureOverrideAvailableSize.Width(),
            m_defaultSizePx.Width() - controlDesiredSizePx.Width() - g_unitConverter.DpToPxInt32(fixedWidth));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[5]->TestLastMeasureOverrideAvailableSize.Height()));

  // Fixed x star
  ASSERT_EQ(children[6]->TestLastMeasureOverrideAvailableSize.Width(), g_unitConverter.DpToPxInt32(fixedWidth));
  ASSERT_EQ(children[6]->TestLastMeasureOverrideAvailableSize.Height(),
            m_defaultSizePx.Height() - controlDesiredSizePx.Height() - g_unitConverter.DpToPxInt32(fixedHeight));

  // Auto x star
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[7]->TestLastMeasureOverrideAvailableSize.Width()));
  ASSERT_EQ(children[7]->TestLastMeasureOverrideAvailableSize.Height(),
            m_defaultSizePx.Height() - controlDesiredSizePx.Height() - g_unitConverter.DpToPxInt32(fixedHeight));

  // Star x star
  ASSERT_EQ(children[8]->TestLastMeasureOverrideAvailableSize.Width(),
            m_defaultSizePx.Width() - controlDesiredSizePx.Width() - g_unitConverter.DpToPxInt32(fixedWidth));
  ASSERT_EQ(children[8]->TestLastMeasureOverrideAvailableSize.Height(),
            m_defaultSizePx.Height() - controlDesiredSizePx.Height() - g_unitConverter.DpToPxInt32(fixedHeight));

  layout.Arrange(m_finalRectPx);
}
