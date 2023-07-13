/****************************************************************************************************************************************************
 * Copyright 2018, 2022-2023 NXP
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
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslSimpleUI/Base/Layout/WrapLayout.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include <cmath>

namespace Fsl::UI
{
  using TClass = WrapLayout;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyOrientation = TFactory::Create<LayoutOrientation, TClass, &TClass::GetOrientation, &TClass::SetOrientation>("Orientation");
  TDef TClass::PropertySpacing = TFactory::Create<DpSize2DF, TClass, &TClass::GetSpacing, &TClass::SetSpacing>("Spacing");
}

namespace Fsl::UI
{
  WrapLayout::WrapLayout(const std::shared_ptr<BaseWindowContext>& context)
    : ComplexLayout<WrapLayoutWindowRecord>(context)
    , m_propertyOrientation(LayoutOrientation::Vertical)
  {
  }

  bool WrapLayout::SetOrientation(const LayoutOrientation value)
  {
    const bool changed = m_propertyOrientation.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }

  bool WrapLayout::SetSpacing(const DpSize2DF value)
  {
    const bool changed = m_propertySpacingDp.Set(ThisDependencyObject(), value);
    if (changed)
    {
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
    auto spacingDp = m_propertySpacingDp.Get();
    if (m_propertyOrientation.Get() == LayoutOrientation::Horizontal)
    {
      // If we are supplied with infinity we behave like a stack panel
      if (!availableSizePx.IsInfinityWidth())
      {
        auto spacingPx = unitConverter.ToPxSize2D(spacingDp);
        minSizePx = MeasureHorizontalWrapLayout(begin(), end(), spacingPx, availableSizePx);
      }
      else
      {
        auto spacingPx = unitConverter.ToPxSize1D(spacingDp.Width());
        minSizePx = MeasureHorizontalStackLayout(begin(), end(), spacingPx, availableSizePx);
      }
    }
    else
    {
      // If we are supplied with infinity we behave like a stack panel
      if (!availableSizePx.IsInfinityHeight())
      {
        auto spacingPx = unitConverter.ToPxSize2D(spacingDp);
        minSizePx = MeasureVerticalWrapLayout(begin(), end(), spacingPx, availableSizePx);
      }
      else
      {
        auto spacingPx = unitConverter.ToPxSize1D(spacingDp.Height());
        minSizePx = MeasureVerticalStackLayout(begin(), end(), spacingPx, availableSizePx);
      }
    }

    return minSizePx;
  }


  DataBinding::DataBindingInstanceHandle WrapLayout::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(this, ThisDependencyObject(), sourceDef,
                                                                         DataBinding::PropLinkRefs(PropertyOrientation, m_propertyOrientation),
                                                                         DataBinding::PropLinkRefs(PropertySpacing, m_propertySpacingDp));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult WrapLayout::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                     const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(this, ThisDependencyObject(), targetDef, binding,
                                                                  DataBinding::PropLinkRefs(PropertyOrientation, m_propertyOrientation),
                                                                  DataBinding::PropLinkRefs(PropertySpacing, m_propertySpacingDp));
    return res != DataBinding::PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void WrapLayout::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyOrientation);
    rProperties.push_back(PropertySpacing);
  }


  PxSize2D WrapLayout::MeasureHorizontalStackLayout(const collection_type::queue_type::iterator& itrBegin,
                                                    const collection_type::queue_type::iterator& itrEnd, const PxSize1D spacingXPx,
                                                    const PxAvailableSize availableSizePx)
  {
    assert(itrBegin != itrEnd);
    PxSize1D minSizeYPx;
    PxSize1D posPx;
    for (auto itr = itrBegin; itr != itrEnd; ++itr)
    {
      itr->Window->Measure(availableSizePx);
      PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
      if (desiredSizePx.Height() > minSizeYPx)
      {
        minSizeYPx = desiredSizePx.Height();
      }

      itr->PositionPx = PxPoint2(posPx, PxValue(0));
      posPx += desiredSizePx.Width() + spacingXPx;
    }
    return {posPx - spacingXPx, minSizeYPx};
  }


  PxSize2D WrapLayout::MeasureVerticalStackLayout(const collection_type::queue_type::iterator& itrBegin,
                                                  const collection_type::queue_type::iterator& itrEnd, const PxSize1D spacingYPx,
                                                  const PxAvailableSize availableSizePx)
  {
    assert(itrBegin != itrEnd);
    PxSize1D minSizeXPx;
    PxSize1D posPx;
    for (auto itr = itrBegin; itr != itrEnd; ++itr)
    {
      itr->Window->Measure(availableSizePx);
      auto desiredSizePx = itr->Window->DesiredSizePx();
      itr->PositionPx = PxPoint2(PxValue(0), posPx);
      if (desiredSizePx.Width() > minSizeXPx)
      {
        minSizeXPx = desiredSizePx.Width();
      }
      posPx += desiredSizePx.Height() + spacingYPx;
    }
    return {minSizeXPx, posPx - spacingYPx};
  }


  PxSize2D WrapLayout::MeasureHorizontalWrapLayout(const collection_type::queue_type::iterator& itrBegin,
                                                   const collection_type::queue_type::iterator& itrEnd, const PxSize2D spacingPx,
                                                   const PxAvailableSize availableSizePx)
  {
    assert(itrBegin != itrEnd);
    // Process the first element to simplify the loop
    auto itr = itrBegin;
    itr->Window->Measure(availableSizePx);
    itr->PositionPx = {};

    PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
    PxSize1D rowWidthPx = desiredSizePx.Width();
    PxSize1D rowHeightPx = desiredSizePx.Height();
    PxSize1D maxRowWidthPx = rowWidthPx;
    PxSize1D posXPx = desiredSizePx.Width();
    PxSize1D posYPx;

    ++itr;
    while (itr != itrEnd)
    {
      itr->Window->Measure(availableSizePx);
      desiredSizePx = itr->Window->DesiredSizePx();

      // Check if we need to force a line switch
      rowWidthPx += spacingPx.Width() + desiredSizePx.Width();
      if (rowWidthPx > availableSizePx.Width())
      {
        // ok we exceeded the available space so we wrap and
        // insert the current item as the first item in the new row
        if (posXPx > maxRowWidthPx)
        {
          maxRowWidthPx = posXPx;
        }

        posYPx += spacingPx.Height() + rowHeightPx;
        rowHeightPx = desiredSizePx.Height();
        rowWidthPx = desiredSizePx.Width();
        itr->PositionPx = PxPoint2(PxValue(0), posYPx);
      }
      else if (desiredSizePx.Height() > rowHeightPx)
      {
        // Keep track of the largest desired height we encounter
        rowHeightPx = desiredSizePx.Height();
        itr->PositionPx = PxPoint2(posXPx + spacingPx.Width(), posYPx);
      }
      else
      {
        itr->PositionPx = PxPoint2(posXPx + spacingPx.Width(), posYPx);
      }
      posXPx = rowWidthPx;

      ++itr;
    }
    if (rowWidthPx > maxRowWidthPx)
    {
      maxRowWidthPx = rowWidthPx;
    }
    posYPx += rowHeightPx;

    return {maxRowWidthPx, posYPx};
  }


  PxSize2D WrapLayout::MeasureVerticalWrapLayout(const collection_type::queue_type::iterator& itrBegin,
                                                 const collection_type::queue_type::iterator& itrEnd, const PxSize2D spacingPx,
                                                 const PxAvailableSize availableSizePx)
  {
    assert(itrBegin != itrEnd);

    // Process the first element to simplify the loop
    auto itr = itrBegin;
    itr->Window->Measure(availableSizePx);
    itr->PositionPx = {};

    PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
    PxSize1D rowWidthPx = desiredSizePx.Width();
    PxSize1D rowHeightPx = desiredSizePx.Height();
    PxSize1D maxRowHeightPx = rowHeightPx;
    PxSize1D posXPx;
    PxSize1D posYPx = desiredSizePx.Height();

    ++itr;
    while (itr != itrEnd)
    {
      itr->Window->Measure(availableSizePx);
      desiredSizePx = itr->Window->DesiredSizePx();

      // Check if we need to force a line switch
      rowHeightPx += spacingPx.Height() + desiredSizePx.Height();
      if (rowHeightPx > availableSizePx.Height())
      {
        // ok we exceeded the available space so we wrap and
        // insert the current item as the first item in the new row
        if (posYPx > maxRowHeightPx)
        {
          maxRowHeightPx = posYPx;
        }

        posXPx += spacingPx.Width() + rowWidthPx;
        rowWidthPx = desiredSizePx.Width();
        rowHeightPx = desiredSizePx.Height();
        itr->PositionPx = PxPoint2(posXPx, PxValue(0));
      }
      else if (desiredSizePx.Width() > rowWidthPx)
      {
        // Keep track of the largest desired width we encounter
        rowWidthPx = desiredSizePx.Width();
        itr->PositionPx = PxPoint2(posXPx, posYPx + spacingPx.Height());
      }
      else
      {
        itr->PositionPx = PxPoint2(posXPx, posYPx + spacingPx.Height());
      }
      posYPx = rowHeightPx;

      ++itr;
    }
    if (rowHeightPx > maxRowHeightPx)
    {
      maxRowHeightPx = rowHeightPx;
    }
    posXPx += rowWidthPx;

    return {posXPx, maxRowHeightPx};
  }
}
