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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslSimpleUI/Base/Layout/WrapLayout.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include <cmath>

namespace Fsl::UI
{
  WrapLayout::WrapLayout(const std::shared_ptr<BaseWindowContext>& context)
    : ComplexLayout<WrapLayoutWindowRecord>(context)
    , m_orientation(LayoutOrientation::Vertical)
  {
  }

  void WrapLayout::SetOrientation(const LayoutOrientation& value)
  {
    if (value != m_orientation)
    {
      m_orientation = value;
      PropertyUpdated(PropertyType::Layout);
    }
  }

  bool WrapLayout::SetSpacing(const DpPoint2F valueDp)
  {
    const bool changed = valueDp != m_spacingDp;
    if (changed)
    {
      m_spacingDp = valueDp;
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }

  PxSize2D WrapLayout::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    if (empty())
    {
      return Layout::ArrangeOverride(finalSizePx);
    }

    for (auto itr = begin(); itr != end(); ++itr)
    {
      const auto desiredPx = itr->Window->DesiredSizePx();
      itr->Window->Arrange(PxRectangle(itr->PositionPx, desiredPx));
    }
    return finalSizePx;
  }


  PxSize2D WrapLayout::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    if (empty())
    {
      return Layout::MeasureOverride(availableSizePx);
    }

    const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;

    PxSize2D minSizePx;
    if (m_orientation == LayoutOrientation::Horizontal)
    {
      // If we are supplied with infinity we behave like a stack panel
      if (!availableSizePx.IsInfinityWidth())
      {
        auto spacingPx = unitConverter.ToPxPoint2(m_spacingDp);
        minSizePx = MeasureHorizontalWrapLayout(begin(), end(), spacingPx, availableSizePx);
      }
      else
      {
        auto spacingPx = unitConverter.ToPxValue(m_spacingDp.X);
        minSizePx = MeasureHorizontalStackLayout(begin(), end(), spacingPx, availableSizePx);
      }
    }
    else
    {
      // If we are supplied with infinity we behave like a stack panel
      if (!availableSizePx.IsInfinityHeight())
      {
        auto spacingPx = unitConverter.ToPxPoint2(m_spacingDp);
        minSizePx = MeasureVerticalWrapLayout(begin(), end(), spacingPx, availableSizePx);
      }
      else
      {
        auto spacingPx = unitConverter.ToPxValue(m_spacingDp.Y);
        minSizePx = MeasureVerticalStackLayout(begin(), end(), spacingPx, availableSizePx);
      }
    }

    return minSizePx;
  }


  PxSize2D WrapLayout::MeasureHorizontalStackLayout(const collection_type::queue_type::iterator& itrBegin,
                                                    const collection_type::queue_type::iterator& itrEnd, const PxValue spacingXPx,
                                                    const PxAvailableSize& availableSizePx)
  {
    assert(itrBegin != itrEnd);
    PxPoint2 minSizePx;
    int32_t posPx = 0;
    for (auto itr = itrBegin; itr != itrEnd; ++itr)
    {
      itr->Window->Measure(availableSizePx);
      PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
      if (desiredSizePx.Height() > minSizePx.Y)
      {
        minSizePx.Y = desiredSizePx.Height();
      }

      itr->PositionPx = PxPoint2(posPx, 0);
      posPx += desiredSizePx.Width() + spacingXPx.Value;
    }
    minSizePx.X = posPx - spacingXPx.Value;
    return TypeConverter::UncheckedTo<PxSize2D>(minSizePx);
  }


  PxSize2D WrapLayout::MeasureVerticalStackLayout(const collection_type::queue_type::iterator& itrBegin,
                                                  const collection_type::queue_type::iterator& itrEnd, const PxValue spacingYPx,
                                                  const PxAvailableSize& availableSizePx)
  {
    assert(itrBegin != itrEnd);

    PxPoint2 minSizePx;
    int32_t posPx = 0;
    for (auto itr = itrBegin; itr != itrEnd; ++itr)
    {
      itr->Window->Measure(availableSizePx);
      auto desiredSizePx = itr->Window->DesiredSizePx();
      itr->PositionPx = PxPoint2(0, posPx);
      if (desiredSizePx.Width() > minSizePx.X)
      {
        minSizePx.X = desiredSizePx.Width();
      }
      posPx += desiredSizePx.Height() + spacingYPx.Value;
    }
    minSizePx.Y = posPx - spacingYPx.Value;
    return TypeConverter::UncheckedTo<PxSize2D>(minSizePx);
  }


  PxSize2D WrapLayout::MeasureHorizontalWrapLayout(const collection_type::queue_type::iterator& itrBegin,
                                                   const collection_type::queue_type::iterator& itrEnd, const PxPoint2& spacingPx,
                                                   const PxAvailableSize& availableSizePx)
  {
    assert(itrBegin != itrEnd);
    // Process the first element to simplify the loop
    auto itr = itrBegin;
    itr->Window->Measure(availableSizePx);
    itr->PositionPx = {};

    PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
    int32_t rowWidthPx = desiredSizePx.Width();
    int32_t rowHeightPx = desiredSizePx.Height();
    int32_t maxRowWidthPx = rowWidthPx;
    PxPoint2 posPx(desiredSizePx.Width(), 0);

    ++itr;
    while (itr != itrEnd)
    {
      itr->Window->Measure(availableSizePx);
      desiredSizePx = itr->Window->DesiredSizePx();

      // Check if we need to force a line switch
      rowWidthPx += spacingPx.X + desiredSizePx.Width();
      if (rowWidthPx > availableSizePx.Width())
      {
        // ok we exceeded the available space so we wrap and
        // insert the current item as the first item in the new row
        if (posPx.X > maxRowWidthPx)
        {
          maxRowWidthPx = posPx.X;
        }

        posPx.Y += spacingPx.Y + rowHeightPx;
        rowHeightPx = desiredSizePx.Height();
        rowWidthPx = desiredSizePx.Width();
        itr->PositionPx = PxPoint2(0, posPx.Y);
      }
      else if (desiredSizePx.Height() > rowHeightPx)
      {
        // Keep track of the largest desired height we encounter
        rowHeightPx = desiredSizePx.Height();
        itr->PositionPx = PxPoint2(posPx.X + spacingPx.X, posPx.Y);
      }
      else
      {
        itr->PositionPx = PxPoint2(posPx.X + spacingPx.X, posPx.Y);
      }
      posPx.X = rowWidthPx;

      ++itr;
    }
    if (rowWidthPx > maxRowWidthPx)
    {
      maxRowWidthPx = rowWidthPx;
    }
    posPx.Y += rowHeightPx;

    return {maxRowWidthPx, posPx.Y};
  }


  PxSize2D WrapLayout::MeasureVerticalWrapLayout(const collection_type::queue_type::iterator& itrBegin,
                                                 const collection_type::queue_type::iterator& itrEnd, const PxPoint2& spacingPx,
                                                 const PxAvailableSize& availableSizePx)
  {
    assert(itrBegin != itrEnd);

    // Process the first element to simplify the loop
    auto itr = itrBegin;
    itr->Window->Measure(availableSizePx);
    itr->PositionPx = {};

    PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
    int32_t rowWidthPx = desiredSizePx.Width();
    int32_t rowHeightPx = desiredSizePx.Height();
    int32_t maxRowHeightPx = rowHeightPx;

    PxPoint2 posPx(0, desiredSizePx.Height());

    ++itr;
    while (itr != itrEnd)
    {
      itr->Window->Measure(availableSizePx);
      desiredSizePx = itr->Window->DesiredSizePx();

      // Check if we need to force a line switch
      rowHeightPx += spacingPx.Y + desiredSizePx.Height();
      if (rowHeightPx > availableSizePx.Height())
      {
        // ok we exceeded the available space so we wrap and
        // insert the current item as the first item in the new row
        if (posPx.Y > maxRowHeightPx)
        {
          maxRowHeightPx = posPx.Y;
        }

        posPx.X += spacingPx.X + rowWidthPx;
        rowWidthPx = desiredSizePx.Width();
        rowHeightPx = desiredSizePx.Height();
        itr->PositionPx = PxPoint2(posPx.X, 0);
      }
      else if (desiredSizePx.Width() > rowWidthPx)
      {
        // Keep track of the largest desired width we encounter
        rowWidthPx = desiredSizePx.Width();
        itr->PositionPx = PxPoint2(posPx.X, posPx.Y + spacingPx.Y);
      }
      else
      {
        itr->PositionPx = PxPoint2(posPx.X, posPx.Y + spacingPx.Y);
      }
      posPx.Y = rowHeightPx;

      ++itr;
    }
    if (rowHeightPx > maxRowHeightPx)
    {
      maxRowHeightPx = rowHeightPx;
    }
    posPx.X += rowWidthPx;

    return {posPx.X, maxRowHeightPx};
  }
}
