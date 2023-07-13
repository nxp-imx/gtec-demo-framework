/****************************************************************************************************************************************************
 * Copyright 2018, 2023 NXP
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

#include <FslBase/Log/Math/LogVector2.hpp>
#include <FslBase/Log/Math/Pixel/LogPxSize2D.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
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
    const UI::PxAvailableSize m_defaultSizePx{UI::PxAvailableSize::Create(1920, 1080)};
    const PxRectangle m_finalRectPx{PxRectangle::Create(1, 2, 1920, 1080)};

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

  // const auto controlDesiredSizePx = PxSize2D::Create(30, 15);

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

  const auto controlDesiredSizePx = PxSize2D::Create(30, 15);

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
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Width(), g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Height(), g_unitConverter.DpToPxSize1D(fixedHeight));

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt1x0)
{
  UI::GridLayout layout(m_windowContext);

  const auto controlDesiredSizePx = PxSize2D::Create(30, 15);

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
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(child->TestLastMeasureOverrideAvailableSize.RawWidth()));
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Height(), g_unitConverter.DpToPxSize1D(fixedHeight));

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt2x0)
{
  UI::GridLayout layout(m_windowContext);

  const auto controlDesiredSizePx = PxSize2D::Create(30, 15);

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
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.ToPxWidth() - g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Height(), g_unitConverter.DpToPxSize1D(fixedHeight));

  layout.Arrange(m_finalRectPx);
}


TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt0x1)
{
  UI::GridLayout layout(m_windowContext);

  const auto controlDesiredSizePx = PxSize2D::Create(30, 15);

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
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Width(), g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(child->TestLastMeasureOverrideAvailableSize.RawHeight()));

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt1x1)
{
  UI::GridLayout layout(m_windowContext);

  const auto controlDesiredSizePx = PxSize2D::Create(30, 15);

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
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(child->TestLastMeasureOverrideAvailableSize.RawWidth()));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(child->TestLastMeasureOverrideAvailableSize.RawHeight()));

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt2x1)
{
  UI::GridLayout layout(m_windowContext);

  const auto controlDesiredSizePx = PxSize2D::Create(30, 15);

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
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.ToPxWidth() - g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(child->TestLastMeasureOverrideAvailableSize.RawHeight()));

  layout.Arrange(m_finalRectPx);
}


TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt0x2)
{
  UI::GridLayout layout(m_windowContext);

  const auto controlDesiredSizePx = PxSize2D::Create(30, 15);

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
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Width(), g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Height(), m_defaultSizePx.ToPxHeight() - g_unitConverter.DpToPxSize1D(fixedHeight));

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt1x2)
{
  UI::GridLayout layout(m_windowContext);

  const auto controlDesiredSizePx = PxSize2D::Create(30, 15);

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
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(child->TestLastMeasureOverrideAvailableSize.RawWidth()));
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Height(), m_defaultSizePx.ToPxHeight() - g_unitConverter.DpToPxSize1D(fixedHeight));

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_ChildNoSizeAt2x2)
{
  UI::GridLayout layout(m_windowContext);

  const auto controlDesiredSizePx = PxSize2D::Create(30, 15);

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
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.ToPxWidth() - g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_EQ(child->TestLastMeasureOverrideAvailableSize.Height(), m_defaultSizePx.ToPxHeight() - g_unitConverter.DpToPxSize1D(fixedHeight));

  layout.Arrange(m_finalRectPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_9ChildNoSizeAt2x2)
{
  UI::GridLayout layout(m_windowContext);

  const auto controlDesiredSizePx = PxSize2D::Create(30, 15);

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
  ASSERT_EQ(children[0]->TestLastMeasureOverrideAvailableSize.Width(), g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_EQ(children[0]->TestLastMeasureOverrideAvailableSize.Height(), g_unitConverter.DpToPxSize1D(fixedHeight));

  // Auto x fixed
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[1]->TestLastMeasureOverrideAvailableSize.RawWidth()));
  ASSERT_EQ(children[1]->TestLastMeasureOverrideAvailableSize.Height(), g_unitConverter.DpToPxSize1D(fixedHeight));

  // Star x Fixed
  ASSERT_EQ(children[2]->TestLastMeasureOverrideAvailableSize.Width(),
            m_defaultSizePx.ToPxWidth() - controlDesiredSizePx.Width() - g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_EQ(children[2]->TestLastMeasureOverrideAvailableSize.Height(), g_unitConverter.DpToPxSize1D(fixedHeight));

  // Fixed x Auto
  ASSERT_EQ(children[3]->TestLastMeasureOverrideAvailableSize.Width(), g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[3]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Auto x auto
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[4]->TestLastMeasureOverrideAvailableSize.RawWidth()));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[4]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Star x auto
  ASSERT_EQ(children[5]->TestLastMeasureOverrideAvailableSize.Width(),
            m_defaultSizePx.ToPxWidth() - controlDesiredSizePx.Width() - g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[5]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Fixed x star
  ASSERT_EQ(children[6]->TestLastMeasureOverrideAvailableSize.Width(), g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_EQ(children[6]->TestLastMeasureOverrideAvailableSize.Height(),
            m_defaultSizePx.ToPxHeight() - controlDesiredSizePx.Height() - g_unitConverter.DpToPxSize1D(fixedHeight));

  // Auto x star
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[7]->TestLastMeasureOverrideAvailableSize.RawWidth()));
  ASSERT_EQ(children[7]->TestLastMeasureOverrideAvailableSize.Height(),
            m_defaultSizePx.ToPxHeight() - controlDesiredSizePx.Height() - g_unitConverter.DpToPxSize1D(fixedHeight));

  // Star x star
  ASSERT_EQ(children[8]->TestLastMeasureOverrideAvailableSize.Width(),
            m_defaultSizePx.ToPxWidth() - controlDesiredSizePx.Width() - g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_EQ(children[8]->TestLastMeasureOverrideAvailableSize.Height(),
            m_defaultSizePx.ToPxHeight() - controlDesiredSizePx.Height() - g_unitConverter.DpToPxSize1D(fixedHeight));

  layout.Arrange(m_finalRectPx);
}


TEST_F(TestGridLayout, MeasureAllDefs_Custom0)
{
  UI::GridLayout layout(m_windowContext);
  layout.SetAlignmentX(UI::ItemAlignment::Stretch);
  layout.SetAlignmentY(UI::ItemAlignment::Stretch);

  const PxSize2D controlDesiredSize0Px = PxSize2D::Create(35, 15);
  const PxSize2D controlDesiredSize1Px = PxSize2D::Create(38, 25);
  const PxSize2D controlDesiredSize2Px = PxSize2D::Create(38, 35);

  std::array<std::shared_ptr<UI::UTImageControl>, 3> children;
  for (auto& rEntry : children)
  {
    rEntry = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize0Px);
    rEntry->SetScalePolicy(UI::ItemScalePolicy::Fit);
    rEntry->SetAlignmentX(UI::ItemAlignment::Stretch);
    rEntry->SetAlignmentY(UI::ItemAlignment::Stretch);
    layout.AddChild(rEntry);
  }
  children[0]->SetImageSize(controlDesiredSize0Px);
  children[1]->SetImageSize(controlDesiredSize1Px);
  children[2]->SetImageSize(controlDesiredSize2Px);
  children[0]->SetScalePolicy(UI::ItemScalePolicy::NoScaling);
  children[1]->SetScalePolicy(UI::ItemScalePolicy::NoScaling);
  children[2]->SetScalePolicy(UI::ItemScalePolicy::NoScaling);
  children[0]->SetAlignmentY(UI::ItemAlignment::Center);
  children[1]->SetAlignmentY(UI::ItemAlignment::Center);
  children[2]->SetAlignmentY(UI::ItemAlignment::Center);

  const float fixedWidth = 4.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  for (uint32_t x = 0; x < 3; ++x)
  {
    layout.Set(children[x], x, 0);
  }

  // Since the first row is auto we expect it to be the sum of all the heights
  layout.Measure(m_defaultSizePx);

  const PxSize1D expectedFirstRowHeightPx =
    PxSize1D::Max(controlDesiredSize0Px.Height(), controlDesiredSize1Px.Height(), controlDesiredSize2Px.Height());

  // We expect the grid to consume all x-space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), PxSize2D(layout.DesiredSizePx().Width(), expectedFirstRowHeightPx));

  // Auto x Auto
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[0]->TestLastMeasureOverrideAvailableSize.RawWidth()));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[0]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Fixed x Auto
  ASSERT_EQ(children[1]->TestLastMeasureOverrideAvailableSize.Width(), g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[1]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Star x Auto
  ASSERT_EQ(children[2]->TestLastMeasureOverrideAvailableSize.Width(),
            m_defaultSizePx.ToPxWidth() - controlDesiredSize0Px.Width() - g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[2]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  layout.Arrange(m_finalRectPx);
}


TEST_F(TestGridLayout, MeasureAllDefs_Custom1)
{
  UI::GridLayout layout(m_windowContext);

  const auto controlDesiredSizePx = PxSize2D::Create(30, 15);
  const PxSize2D controlDesiredSize0Px = PxSize2D::Create(35, 15);
  const PxSize2D controlDesiredSize1Px = PxSize2D::Create(38, 25);
  const PxSize2D controlDesiredSize2Px = PxSize2D::Create(38, 35);

  std::array<std::shared_ptr<UI::UTImageControl>, 3 * 2> children;
  for (auto& rEntry : children)
  {
    rEntry = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSizePx);
    layout.AddChild(rEntry);
  }
  children[0]->SetImageSize(controlDesiredSize0Px);
  children[1]->SetImageSize(controlDesiredSize1Px);
  children[2]->SetImageSize(controlDesiredSize2Px);
  children[0]->SetScalePolicy(UI::ItemScalePolicy::NoScaling);
  children[1]->SetScalePolicy(UI::ItemScalePolicy::NoScaling);
  children[2]->SetScalePolicy(UI::ItemScalePolicy::NoScaling);
  children[0]->SetAlignmentY(UI::ItemAlignment::Center);
  children[1]->SetAlignmentY(UI::ItemAlignment::Center);
  children[2]->SetAlignmentY(UI::ItemAlignment::Center);

  const float fixedWidth = 4.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  for (uint32_t y = 0; y < 2; ++y)
  {
    for (uint32_t x = 0; x < 3; ++x)
    {
      layout.Set(children[x + (y * 3)], x, y);
    }
  }

  layout.Measure(m_defaultSizePx);

  const PxSize1D minWidthCell0 = PxSize1D::Max(controlDesiredSizePx.Width(), controlDesiredSize0Px.Width());
  // const int32_t minWidthCell1 = std::max(controlDesiredSizePx.Width(), controlDesiredSize1Px.Width());
  // const int32_t minWidthCell2 = std::max(controlDesiredSizePx.Width(), controlDesiredSize2Px.Width());
  // const int32_t minHeightCell0 = std::max(controlDesiredSizePx.Width(), controlDesiredSize0Px.Width());
  // const int32_t minHeightCell1 = std::max(controlDesiredSizePx.Width(), controlDesiredSize1Px.Width());
  // const int32_t minHeightCell2 = std::max(controlDesiredSizePx.Width(), controlDesiredSize2Px.Width());

  // Since the first row is auto we expect it to be the sum of all the heights
  const PxSize1D expectedFirstRowHeightPx =
    PxSize1D::Max(controlDesiredSize0Px.Height(), controlDesiredSize1Px.Height(), controlDesiredSize2Px.Height());

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  // Cell 0, 0: Auto x Auto
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[0]->TestLastMeasureOverrideAvailableSize.RawWidth()));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[0]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Cell 1, 0: Fixed x Auto
  ASSERT_EQ(children[1]->TestLastMeasureOverrideAvailableSize.Width(), g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[1]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Cell 2, 0: Star x Auto
  ASSERT_EQ(children[2]->TestLastMeasureOverrideAvailableSize.Width(),
            m_defaultSizePx.ToPxWidth() - minWidthCell0 - g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[2]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Cell 0, 1: Auto x Star
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[3]->TestLastMeasureOverrideAvailableSize.RawWidth()));
  ASSERT_EQ(children[3]->TestLastMeasureOverrideAvailableSize.Height(), m_defaultSizePx.ToPxHeight() - expectedFirstRowHeightPx);

  // Cell 1, 1: Fixed x Star
  ASSERT_EQ(children[4]->TestLastMeasureOverrideAvailableSize.Width(), g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_EQ(children[4]->TestLastMeasureOverrideAvailableSize.Height(), m_defaultSizePx.ToPxHeight() - expectedFirstRowHeightPx);

  // Cell 2, 1:, Star x Star
  ASSERT_EQ(children[5]->TestLastMeasureOverrideAvailableSize.Width(),
            m_defaultSizePx.ToPxWidth() - minWidthCell0 - g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_EQ(children[5]->TestLastMeasureOverrideAvailableSize.Height(), m_defaultSizePx.ToPxHeight() - expectedFirstRowHeightPx);

  layout.Arrange(m_finalRectPx);
}


TEST_F(TestGridLayout, MeasureAllDefs_Custom2)
{
  UI::GridLayout layout(m_windowContext);

  const auto controlDesiredSizePx = PxSize2D::Create(30, 15);
  const PxSize2D controlDesiredSize0Px = PxSize2D::Create(35, 15);
  const PxSize2D controlDesiredSize1Px = PxSize2D::Create(38, 25);

  std::array<std::shared_ptr<UI::UTImageControl>, 2 * 2> children;
  for (auto& rEntry : children)
  {
    rEntry = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSizePx);
    layout.AddChild(rEntry);
  }
  children[0]->SetImageSize(controlDesiredSize0Px);
  children[1]->SetImageSize(controlDesiredSize1Px);
  children[0]->SetScalePolicy(UI::ItemScalePolicy::NoScaling);
  children[1]->SetScalePolicy(UI::ItemScalePolicy::NoScaling);
  children[2]->SetScalePolicy(UI::ItemScalePolicy::Fit);
  children[3]->SetScalePolicy(UI::ItemScalePolicy::Fit);
  children[0]->SetAlignmentX(UI::ItemAlignment::Near);
  children[0]->SetAlignmentY(UI::ItemAlignment::Center);
  children[1]->SetAlignmentX(UI::ItemAlignment::Near);
  children[1]->SetAlignmentY(UI::ItemAlignment::Center);
  children[2]->SetAlignmentX(UI::ItemAlignment::Near);
  children[2]->SetAlignmentY(UI::ItemAlignment::Stretch);
  children[3]->SetAlignmentX(UI::ItemAlignment::Near);
  children[3]->SetAlignmentY(UI::ItemAlignment::Stretch);

  const float fixedWidth = 4.0f;

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, fixedWidth));
  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(children[0], 0, 0);
  layout.Set(children[1], 2, 0);
  layout.Set(children[2], 0, 1);
  layout.Set(children[3], 2, 1);

  layout.Measure(m_defaultSizePx);

  const PxSize1D minWidthCell0 = PxSize1D::Max(controlDesiredSizePx.Width(), controlDesiredSize0Px.Width());
  // Since the first row is auto we expect it to be the sum of all the heights
  const PxSize1D expectedFirstRowHeightPx = PxSize1D::Max(controlDesiredSize0Px.Height(), controlDesiredSize1Px.Height());

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  // Cell 0, 0: Auto x Auto (Classification: 0)
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[0]->TestLastMeasureOverrideAvailableSize.RawWidth()));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[0]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Cell 2, 0: Star x Auto (Classification: 2)
  ASSERT_EQ(children[1]->TestLastMeasureOverrideAvailableSize.Width(),
            m_defaultSizePx.ToPxWidth() - minWidthCell0 - g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[1]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Cell 0, 1: Auto x Star (Classification: 1)
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[2]->TestLastMeasureOverrideAvailableSize.RawWidth()));
  ASSERT_EQ(children[2]->TestLastMeasureOverrideAvailableSize.Height(), m_defaultSizePx.ToPxHeight() - expectedFirstRowHeightPx);

  // Cell 2, 0: Star x Star (Classification: 3)
  ASSERT_EQ(children[3]->TestLastMeasureOverrideAvailableSize.Width(),
            m_defaultSizePx.ToPxWidth() - minWidthCell0 - g_unitConverter.DpToPxSize1D(fixedWidth));
  ASSERT_EQ(children[3]->TestLastMeasureOverrideAvailableSize.Height(), m_defaultSizePx.ToPxHeight() - expectedFirstRowHeightPx);

  layout.Arrange(m_finalRectPx);
}


TEST_F(TestGridLayout, MeasureAllDefs_OneStarByTwoStar)
{
  UI::GridLayout layout(m_windowContext);

  const auto controlDesiredSize0Px = PxSize2D::Create(35, 15);
  const auto controlDesiredSize1Px = PxSize2D::Create(38, 25);

  std::array<std::shared_ptr<UI::UTImageControl>, 1 * 2> children;
  for (auto& rEntry : children)
  {
    rEntry = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize0Px);
    layout.AddChild(rEntry);
  }
  children[0]->SetImageSize(controlDesiredSize0Px);
  children[1]->SetImageSize(controlDesiredSize1Px);
  children[0]->SetScalePolicy(UI::ItemScalePolicy::Fit);
  children[1]->SetScalePolicy(UI::ItemScalePolicy::Fit);
  children[0]->SetAlignmentX(UI::ItemAlignment::Stretch);
  children[0]->SetAlignmentY(UI::ItemAlignment::Stretch);
  children[1]->SetAlignmentX(UI::ItemAlignment::Stretch);
  children[1]->SetAlignmentY(UI::ItemAlignment::Stretch);

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(children[0], 0, 0);
  layout.Set(children[1], 0, 1);

  layout.Measure(m_defaultSizePx);

  const PxSize1D halfHeightPx = m_defaultSizePx.ToPxHeight() / PxSize1D::Create(2);
  // const int32_t minWidthCell0 = std::max(controlDesiredSize0Px.Width(), controlDesiredSize1Px.Width());

  // Since we have two star rows we expect the space to be equally divided
  const PxSize1D expectedRow0HeightPx = std::max(halfHeightPx, controlDesiredSize0Px.Height());
  const PxSize1D expectedRow1HeightPx = std::max(PxSize1D(m_defaultSizePx.ToPxHeight() - halfHeightPx), controlDesiredSize1Px.Height());

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  // Cell 0, 0: Star x Star (Classification: 3)
  ASSERT_EQ(children[0]->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width());
  ASSERT_EQ(children[0]->TestLastMeasureOverrideAvailableSize.Height(), expectedRow0HeightPx);

  // Cell 0, 1: Star x Star (Classification: 3)
  ASSERT_EQ(children[1]->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width());
  ASSERT_EQ(children[1]->TestLastMeasureOverrideAvailableSize.Height(), expectedRow1HeightPx);

  layout.Arrange(m_finalRectPx);

  // Cell 0, 0: Star x Star (Classification: 3)
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().X(), PxValue(0));
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().Y(), PxValue(0));
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().Width(), m_defaultSizePx.Width());
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().Height(), expectedRow0HeightPx);

  // Cell 0, 1: Star x Star (Classification: 3)
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().X(), PxValue(0));
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().Y(), expectedRow0HeightPx);
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().Width(), m_defaultSizePx.Width());
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().Height(), expectedRow1HeightPx);
}


TEST_F(TestGridLayout, MeasureAllDefs_FixedSize_OneStarByAutoThenTwoStars)
{
  UI::GridLayout layout(m_windowContext);

  const PxSize2D controlDesiredSize0Px = PxSize2D::Create(32, 10);
  const PxSize2D controlDesiredSize1Px = PxSize2D::Create(35, 15);
  const PxSize2D controlDesiredSize2Px = PxSize2D::Create(38, 25);

  std::array<std::shared_ptr<UI::UTImageControl>, 1 * 3> children;
  for (auto& rEntry : children)
  {
    rEntry = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize0Px);
    layout.AddChild(rEntry);
  }
  children[0]->SetImageSize(controlDesiredSize0Px);
  children[1]->SetImageSize(controlDesiredSize1Px);
  children[2]->SetImageSize(controlDesiredSize1Px);
  children[0]->SetScalePolicy(UI::ItemScalePolicy::Fit);
  children[1]->SetScalePolicy(UI::ItemScalePolicy::Fit);
  children[2]->SetScalePolicy(UI::ItemScalePolicy::Fit);
  children[0]->SetAlignmentX(UI::ItemAlignment::Near);
  children[0]->SetAlignmentY(UI::ItemAlignment::Near);
  children[1]->SetAlignmentX(UI::ItemAlignment::Stretch);
  children[1]->SetAlignmentY(UI::ItemAlignment::Stretch);
  children[2]->SetAlignmentX(UI::ItemAlignment::Stretch);
  children[2]->SetAlignmentY(UI::ItemAlignment::Stretch);

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(children[0], 0, 0);
  layout.Set(children[1], 0, 1);
  layout.Set(children[2], 0, 2);

  layout.Measure(m_defaultSizePx);

  const PxSize1D availableStarHeight = PxSize1D(m_defaultSizePx.ToPxHeight() - controlDesiredSize0Px.Height());
  const PxSize1D halfAvailableStarHeight = availableStarHeight / PxSize1D::Create(2);
  // const int32_t minWidthCell0 = std::max(std::max(controlDesiredSize0Px.Width(), controlDesiredSize1Px.Width()), controlDesiredSize2Px.Width());

  // Since we have two star rows we expect the space to be equally divided
  // const int32_t expectedRow0HeightPx = controlDesiredSize0Px.Height();
  const PxSize1D expectedRow1HeightPx = std::max(halfAvailableStarHeight, controlDesiredSize1Px.Height());
  const PxSize1D expectedRow2HeightPx = PxSize1D(std::max(availableStarHeight - halfAvailableStarHeight, controlDesiredSize2Px.Height().Value()));

  // We expect the grid to consume all space as we have a star row and column
  ASSERT_EQ(layout.DesiredSizePx(), m_defaultSizePx.ToPxSize2D());

  // Cell 0, 0: Star x Auto (Classification: 2)
  ASSERT_EQ(children[0]->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width());
  // ASSERT_EQ(children[0]->TestLastMeasureOverrideAvailableSize.Height(), expectedRow0HeightPx);
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[0]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Cell 0, 1: Star x Star (Classification: 3)
  ASSERT_EQ(children[1]->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width());
  ASSERT_EQ(children[1]->TestLastMeasureOverrideAvailableSize.Height(), expectedRow1HeightPx);

  // Cell 0, 2: Star x Star (Classification: 3)
  ASSERT_EQ(children[2]->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width());
  ASSERT_EQ(children[2]->TestLastMeasureOverrideAvailableSize.Height(), expectedRow2HeightPx);

  layout.Arrange(m_finalRectPx);

  PxSize1D currentYPos = PxSize1D::Create(0);
  // Cell 0, 0: Star x Star (Classification: 2)
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().X(), PxValue(0));
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().Y(), currentYPos);
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().Width(), controlDesiredSize0Px.Width());
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().Height(), controlDesiredSize0Px.Height());

  currentYPos += controlDesiredSize0Px.Height();

  // Cell 0, 1: Star x Star (Classification: 3)
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().X(), PxValue(0));
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().Y(), currentYPos);
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().Width(), m_defaultSizePx.Width());
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().Height(), expectedRow1HeightPx);

  currentYPos += expectedRow1HeightPx;

  // Cell 0, 2: Star x Star (Classification: 3)
  ASSERT_EQ(children[2]->WinGetContentRectanglePx().X(), PxValue(0));
  ASSERT_EQ(children[2]->WinGetContentRectanglePx().Y(), currentYPos);
  ASSERT_EQ(children[2]->WinGetContentRectanglePx().Width(), m_defaultSizePx.Width());
  ASSERT_EQ(children[2]->WinGetContentRectanglePx().Height(), expectedRow2HeightPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_FixedWidthInfHeight_OneStarByAutoThenTwoStars)
{
  UI::GridLayout layout(m_windowContext);

  const PxSize2D controlDesiredSize0Px = PxSize2D::Create(32, 10);
  const PxSize2D controlDesiredSize1Px = PxSize2D::Create(35, 15);
  const PxSize2D controlDesiredSize2Px = PxSize2D::Create(38, 25);

  std::array<std::shared_ptr<UI::UTImageControl>, 1 * 3> children;
  for (auto& rEntry : children)
  {
    rEntry = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize0Px);
    layout.AddChild(rEntry);
  }
  children[0]->SetImageSize(controlDesiredSize0Px);
  children[1]->SetImageSize(controlDesiredSize1Px);
  children[2]->SetImageSize(controlDesiredSize2Px);
  children[0]->SetScalePolicy(UI::ItemScalePolicy::Fit);
  children[1]->SetScalePolicy(UI::ItemScalePolicy::Fit);
  children[2]->SetScalePolicy(UI::ItemScalePolicy::Fit);
  children[0]->SetAlignmentX(UI::ItemAlignment::Near);
  children[0]->SetAlignmentY(UI::ItemAlignment::Near);
  children[1]->SetAlignmentX(UI::ItemAlignment::Stretch);
  children[1]->SetAlignmentY(UI::ItemAlignment::Stretch);
  children[2]->SetAlignmentX(UI::ItemAlignment::Stretch);
  children[2]->SetAlignmentY(UI::ItemAlignment::Stretch);

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(children[0], 0, 0);
  layout.Set(children[1], 0, 1);
  layout.Set(children[2], 0, 2);

  UI::PxAvailableSize layoutAvailableSpacePx(m_defaultSizePx.Width(), UI::PxAvailableSize1D::InfiniteSpacePx());
  layout.Measure(layoutAvailableSpacePx);

  // const int32_t availableStarHeight = (m_defaultSizePx.Height() - controlDesiredSize0Px.Height());
  // const int32_t halfAvailableStarHeight = availableStarHeight / 2;
  // const int32_t minWidthCell0 = std::max(std::max(controlDesiredSize0Px.Width(), controlDesiredSize1Px.Width()), controlDesiredSize2Px.Width());

  // Since we have two star rows we expect the space to be equally divided
  // const int32_t expectedRow0HeightPx = controlDesiredSize0Px.Height();
  // const int32_t expectedRow1HeightPx = std::max(halfAvailableStarHeight, controlDesiredSize1Px.Height());
  // const int32_t expectedRow2HeightPx = std::max(availableStarHeight - halfAvailableStarHeight, controlDesiredSize2Px.Height());

  // We expect the grid to consume all space as we have a star row and column
  const PxSize1D minimumHeightPx = (controlDesiredSize0Px.Height() + controlDesiredSize1Px.Height() + controlDesiredSize2Px.Height());
  ASSERT_EQ(layout.DesiredSizePx(), PxSize2D(layoutAvailableSpacePx.ToPxWidth(), minimumHeightPx));

  // Cell 0, 0: Star x Auto (Classification: 2)
  ASSERT_EQ(children[0]->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width());
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[0]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Cell 0, 1: Star x Star (Classification: 3)
  ASSERT_EQ(children[1]->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width());
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[1]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Cell 0, 2: Star x Star (Classification: 3)
  ASSERT_EQ(children[2]->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width());
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[2]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  layout.Arrange(m_finalRectPx);

  PxSize1D currentXPos = PxSize1D::Create(0);
  PxSize1D currentYPos = PxSize1D::Create(0);
  // Cell 0, 0: Star x Star (Classification: 2)
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().X(), currentXPos);
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().Y(), currentYPos);
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().Width(), controlDesiredSize0Px.Width());
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().Height(), controlDesiredSize0Px.Height());

  currentYPos += controlDesiredSize0Px.Height();

  // Cell 0, 1: Star x Star (Classification: 3)
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().X(), currentXPos);
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().Y(), currentYPos);
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().Width(), m_defaultSizePx.Width());
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().Height(), controlDesiredSize1Px.Height());

  currentYPos += controlDesiredSize1Px.Height();

  // Cell 0, 2: Star x Star (Classification: 3)
  ASSERT_EQ(children[2]->WinGetContentRectanglePx().X(), currentXPos);
  ASSERT_EQ(children[2]->WinGetContentRectanglePx().Y(), currentYPos);
  ASSERT_EQ(children[2]->WinGetContentRectanglePx().Width(), m_defaultSizePx.Width());
  ASSERT_EQ(children[2]->WinGetContentRectanglePx().Height(), controlDesiredSize2Px.Height());
}

TEST_F(TestGridLayout, MeasureAllDefs_FixedWidthInfHeight_Stretch_OneStarByAutoThenTwoStars)
{
  UI::GridLayout layout(m_windowContext);
  layout.SetAlignmentX(UI::ItemAlignment::Stretch);
  layout.SetAlignmentY(UI::ItemAlignment::Stretch);

  const PxSize2D controlDesiredSize0Px = PxSize2D::Create(32, 10);
  const PxSize2D controlDesiredSize1Px = PxSize2D::Create(35, 15);
  const PxSize2D controlDesiredSize2Px = PxSize2D::Create(38, 25);

  std::array<std::shared_ptr<UI::UTImageControl>, 1 * 3> children;
  for (auto& rEntry : children)
  {
    rEntry = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize0Px);
    layout.AddChild(rEntry);
  }
  children[0]->SetImageSize(controlDesiredSize0Px);
  children[1]->SetImageSize(controlDesiredSize1Px);
  children[2]->SetImageSize(controlDesiredSize2Px);
  children[0]->SetScalePolicy(UI::ItemScalePolicy::Fit);
  children[1]->SetScalePolicy(UI::ItemScalePolicy::Fit);
  children[2]->SetScalePolicy(UI::ItemScalePolicy::Fit);
  children[0]->SetAlignmentX(UI::ItemAlignment::Near);
  children[0]->SetAlignmentY(UI::ItemAlignment::Near);
  children[1]->SetAlignmentX(UI::ItemAlignment::Stretch);
  children[1]->SetAlignmentY(UI::ItemAlignment::Stretch);
  children[2]->SetAlignmentX(UI::ItemAlignment::Stretch);
  children[2]->SetAlignmentY(UI::ItemAlignment::Stretch);

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(children[0], 0, 0);
  layout.Set(children[1], 0, 1);
  layout.Set(children[2], 0, 2);

  UI::PxAvailableSize layoutAvailableSpacePx(m_defaultSizePx.ToPxWidth(), UI::PxAvailableSize1D::InfiniteSpacePx());
  layout.Measure(layoutAvailableSpacePx);

  const PxSize1D availableStarHeight = PxSize1D(m_defaultSizePx.ToPxHeight() - controlDesiredSize0Px.Height());
  const PxSize1D halfAvailableStarHeight = availableStarHeight / PxSize1D::Create(2);
  // const int32_t minWidthCell0 = std::max(std::max(controlDesiredSize0Px.Width(), controlDesiredSize1Px.Width()), controlDesiredSize2Px.Width());

  // Since we have two star rows we expect the space to be equally divided
  // const int32_t expectedRow0HeightPx = controlDesiredSize0Px.Height();
  const PxSize1D expectedRow1HeightPx = std::max(halfAvailableStarHeight, controlDesiredSize1Px.Height());
  const PxSize1D expectedRow2HeightPx = std::max(PxSize1D(availableStarHeight - halfAvailableStarHeight), controlDesiredSize2Px.Height());

  // We expect the grid to consume all space as we have a star row and column
  const PxSize1D minimumHeightPx = (controlDesiredSize0Px.Height() + controlDesiredSize1Px.Height() + controlDesiredSize2Px.Height());
  ASSERT_EQ(layout.DesiredSizePx(), PxSize2D(layoutAvailableSpacePx.ToPxWidth(), minimumHeightPx));

  // Cell 0, 0: Star x Auto (Classification: 2)
  ASSERT_EQ(children[0]->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width());
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[0]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Cell 0, 1: Star x Star (Classification: 3)
  ASSERT_EQ(children[1]->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width());
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[1]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Cell 0, 2: Star x Star (Classification: 3)
  ASSERT_EQ(children[2]->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width());
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[2]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  layout.Arrange(m_finalRectPx);

  PxSize1D currentXPos = PxSize1D::Create(0);
  PxSize1D currentYPos = PxSize1D::Create(0);
  // Cell 0, 0: Star x Star (Classification: 2)
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().X(), currentXPos);
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().Y(), currentYPos);
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().Width(), controlDesiredSize0Px.Width());
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().Height(), controlDesiredSize0Px.Height());

  currentYPos += controlDesiredSize0Px.Height();

  // Cell 0, 1: Star x Star (Classification: 3)
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().X(), currentXPos);
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().Y(), currentYPos);
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().Width(), m_defaultSizePx.Width());
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().Height(), expectedRow1HeightPx);

  currentYPos += expectedRow1HeightPx;

  // Cell 0, 2: Star x Star (Classification: 3)
  ASSERT_EQ(children[2]->WinGetContentRectanglePx().X(), currentXPos);
  ASSERT_EQ(children[2]->WinGetContentRectanglePx().Y(), currentYPos);
  ASSERT_EQ(children[2]->WinGetContentRectanglePx().Width(), m_defaultSizePx.Width());
  ASSERT_EQ(children[2]->WinGetContentRectanglePx().Height(), expectedRow2HeightPx);
}

TEST_F(TestGridLayout, MeasureAllDefs_FixedWidthInfHeight_Stretch_OneStarByAutoThenTwoStars_EmptyAuto)
{
  UI::GridLayout layout(m_windowContext);
  layout.SetAlignmentX(UI::ItemAlignment::Stretch);
  layout.SetAlignmentY(UI::ItemAlignment::Stretch);

  const PxSize2D controlDesiredSize1Px = PxSize2D::Create(35, 15);
  const PxSize2D controlDesiredSize2Px = PxSize2D::Create(38, 25);

  std::array<std::shared_ptr<UI::UTImageControl>, 1 * 2> children;
  for (auto& rEntry : children)
  {
    rEntry = std::make_shared<UI::UTImageControl>(m_windowContext, controlDesiredSize1Px);
    layout.AddChild(rEntry);
  }
  children[0]->SetImageSize(controlDesiredSize1Px);
  children[1]->SetImageSize(controlDesiredSize2Px);
  children[0]->SetScalePolicy(UI::ItemScalePolicy::Fit);
  children[1]->SetScalePolicy(UI::ItemScalePolicy::Fit);
  children[0]->SetAlignmentX(UI::ItemAlignment::Stretch);
  children[0]->SetAlignmentY(UI::ItemAlignment::Stretch);
  children[1]->SetAlignmentX(UI::ItemAlignment::Stretch);
  children[1]->SetAlignmentY(UI::ItemAlignment::Stretch);

  layout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
  layout.Set(children[0], 0, 1);
  layout.Set(children[1], 0, 2);

  UI::PxAvailableSize layoutAvailableSpacePx(m_defaultSizePx.ToPxWidth(), UI::PxAvailableSize1D::InfiniteSpacePx());
  layout.Measure(layoutAvailableSpacePx);

  const PxSize1D availableStarHeight =
    PxSize1D(m_defaultSizePx.ToPxHeight() - PxValue(1));    // to ensure that any rounding of the available space for stars will cause issues
  const auto halfAvailableStarHeight = PxSize1D::Create(static_cast<int32_t>(std::round(static_cast<float>(availableStarHeight.RawValue()) / 2.0f)));
  // const int32_t minWidthCell0 = std::max(std::max(controlDesiredSize0Px.Width(), controlDesiredSize1Px.Width()), controlDesiredSize2Px.Width());

  // Since we have two star rows we expect the space to be equally divided
  // const int32_t expectedRow0HeightPx = 0;
  const PxSize1D expectedRow1HeightPx = std::max(halfAvailableStarHeight, controlDesiredSize1Px.Height());
  const PxSize1D expectedRow2HeightPx = std::max(PxSize1D(availableStarHeight - halfAvailableStarHeight), controlDesiredSize2Px.Height());

  // We expect the grid to consume all space as we have a star row and column
  const PxSize1D minimumHeightPx = (controlDesiredSize1Px.Height() + controlDesiredSize2Px.Height());
  ASSERT_EQ(layout.DesiredSizePx(), PxSize2D(layoutAvailableSpacePx.ToPxWidth(), minimumHeightPx));

  // Cell 0, 0: Star x Auto (Classification: 2)

  // Cell 0, 1: Star x Star (Classification: 3)
  ASSERT_EQ(children[0]->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width());
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[0]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  // Cell 0, 2: Star x Star (Classification: 3)
  ASSERT_EQ(children[1]->TestLastMeasureOverrideAvailableSize.Width(), m_defaultSizePx.Width());
  ASSERT_TRUE(UI::PxAvailableSizeUtil::IsConsideredInfiniteSpace(children[1]->TestLastMeasureOverrideAvailableSize.RawHeight()));

  layout.Arrange(PxRectangle(m_finalRectPx.Left(), m_finalRectPx.Top(), m_finalRectPx.Width(), m_finalRectPx.Height() - PxValue(1)));

  PxSize1D currentXPos = PxSize1D::Create(0);
  PxSize1D currentYPos = PxSize1D::Create(0);
  // Cell 0, 0: Star x Star (Classification: 2)

  // Cell 0, 1: Star x Star (Classification: 3)
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().X(), currentXPos);
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().Y(), currentYPos);
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().Width(), m_defaultSizePx.Width());
  ASSERT_EQ(children[0]->WinGetContentRectanglePx().Height(), expectedRow1HeightPx);

  currentYPos += expectedRow1HeightPx;

  // Cell 0, 2: Star x Star (Classification: 3)
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().X(), currentXPos);
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().Y(), currentYPos);
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().Width(), m_defaultSizePx.Width());
  ASSERT_EQ(children[1]->WinGetContentRectanglePx().Height(), expectedRow2HeightPx);
}
