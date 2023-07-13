/****************************************************************************************************************************************************
 * Copyright 2021-2023 NXP
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
#include <FslBase/Math/MathHelper_MinMax.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslSimpleUI/Base/Layout/UniformWrapLayout.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include <cmath>

namespace Fsl::UI
{
  using TClass = UniformWrapLayout;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyOrientation = TFactory::Create<LayoutOrientation, TClass, &TClass::GetOrientation, &TClass::SetOrientation>("Orientation");
  TDef TClass::PropertySpacing = TFactory::Create<DpSize2DF, TClass, &TClass::GetSpacing, &TClass::SetSpacing>("Spacing");
}


namespace Fsl::UI
{
  UniformWrapLayout::UniformWrapLayout(const std::shared_ptr<BaseWindowContext>& context)
    : SimpleLayout(context)
    , m_propertyOrientation(LayoutOrientation::Vertical)
  {
  }

  bool UniformWrapLayout::SetOrientation(const LayoutOrientation value)
  {
    const bool changed = m_propertyOrientation.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }

  bool UniformWrapLayout::SetSpacing(const DpSize2DF value)
  {
    const bool changed = m_propertySpacingDp.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }

  PxSize2D UniformWrapLayout::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    if (empty())
    {
      return Layout::ArrangeOverride(finalSizePx);
    }
    const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;
    auto spacingPx = unitConverter.ToPxSize2D(m_propertySpacingDp.Get());


    if (m_propertyOrientation.Get() == LayoutOrientation::Horizontal)
    {
      const PxSize2D addPx(m_elementUniformSizePx + spacingPx);
      PxPoint2 positionPx;
      for (auto itr = begin(); itr != end(); ++itr)
      {
        itr->Window->Arrange(PxRectangle(positionPx, m_elementUniformSizePx));
        positionPx.X += addPx.Width();
        if ((positionPx.X + m_elementUniformSizePx.Width()) > finalSizePx.Width())
        {
          positionPx.X = PxValue(0);
          positionPx.Y += addPx.Height();
        }
      }
    }
    else
    {
      const PxSize2D addPx(m_elementUniformSizePx + spacingPx);
      PxPoint2 positionPx;
      for (auto itr = begin(); itr != end(); ++itr)
      {
        itr->Window->Arrange(PxRectangle(positionPx, m_elementUniformSizePx));
        positionPx.Y += addPx.Height();
        if ((positionPx.Y + m_elementUniformSizePx.Height()) > finalSizePx.Height())
        {
          positionPx.X += addPx.Width();
          positionPx.Y = PxValue(0);
        }
      }
    }
    return finalSizePx;
  }


  PxSize2D UniformWrapLayout::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    if (empty())
    {
      return Layout::MeasureOverride(availableSizePx);
    }

    const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;

    MeasureResult res;
    auto spacingDp = m_propertySpacingDp.Get();
    if (m_propertyOrientation.Get() == LayoutOrientation::Horizontal)
    {
      // If we are supplied with infinity we behave like a stack panel
      if (!availableSizePx.IsInfinityWidth())
      {
        auto spacingPx = unitConverter.ToPxSize2D(spacingDp);
        res = MeasureHorizontalWrapLayout(begin(), end(), spacingPx, availableSizePx);
      }
      else
      {
        auto spacingPx = unitConverter.ToPxSize1D(spacingDp.Width());
        res = MeasureHorizontalStackLayout(begin(), end(), spacingPx, availableSizePx);
      }
    }
    else
    {
      // If we are supplied with infinity we behave like a stack panel
      if (!availableSizePx.IsInfinityHeight())
      {
        auto spacingPx = unitConverter.ToPxSize2D(spacingDp);
        res = MeasureVerticalWrapLayout(begin(), end(), spacingPx, availableSizePx);
      }
      else
      {
        auto spacingPx = unitConverter.ToPxSize1D(spacingDp.Height());
        res = MeasureVerticalStackLayout(begin(), end(), spacingPx, availableSizePx);
      }
    }

    m_elementUniformSizePx = res.ElementUniformSizePx;
    return res.MeasuredSizePx;
  }

  DataBinding::DataBindingInstanceHandle UniformWrapLayout::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(this, ThisDependencyObject(), sourceDef,
                                                                         DataBinding::PropLinkRefs(PropertyOrientation, m_propertyOrientation),
                                                                         DataBinding::PropLinkRefs(PropertySpacing, m_propertySpacingDp));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult UniformWrapLayout::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                            const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(this, ThisDependencyObject(), targetDef, binding,
                                                                  DataBinding::PropLinkRefs(PropertyOrientation, m_propertyOrientation),
                                                                  DataBinding::PropLinkRefs(PropertySpacing, m_propertySpacingDp));
    return res != DataBinding::PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void UniformWrapLayout::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    base_type::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyOrientation);
    rProperties.push_back(PropertySpacing);
  }

  UniformWrapLayout::MeasureResult UniformWrapLayout::MeasureHorizontalStackLayout(const collection_type::queue_type::const_iterator& itrBegin,
                                                                                   const collection_type::queue_type::const_iterator& itrEnd,
                                                                                   const PxSize1D spacingXPx, const PxAvailableSize availableSizePx)
  {
    assert(availableSizePx.IsInfinityWidth());
    assert(itrBegin != itrEnd);

    PxSize2D finalSizePx;
    PxSize2D maxElementSizePx;
    if (itrBegin != itrEnd)    // if not empty
    {
      // Fake that we have unlimited space in X and keep Y constrained.
      const PxAvailableSize fakeAvailableSizePx(PxAvailableSize1D::InfiniteSpacePx(), availableSizePx.Height());

      // During this pass we measure all elements to find the max size.
      for (auto itr = itrBegin; itr != itrEnd; ++itr)
      {
        itr->Window->Measure(fakeAvailableSizePx);
        PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
        maxElementSizePx.SetMax(desiredSizePx);
      }

      const auto childCount = NumericCast<int32_t>(std::distance(itrBegin, itrEnd));
      assert(childCount > 0);
      finalSizePx = {(maxElementSizePx.Width() * PxSize1D::UncheckedCreate(childCount)) + (spacingXPx * PxSize1D::UncheckedCreate(childCount - 1)),
                     maxElementSizePx.Height()};
    }
    return {finalSizePx, maxElementSizePx};
  }


  UniformWrapLayout::MeasureResult UniformWrapLayout::MeasureVerticalStackLayout(const collection_type::queue_type::const_iterator& itrBegin,
                                                                                 const collection_type::queue_type::const_iterator& itrEnd,
                                                                                 const PxSize1D spacingYPx, const PxAvailableSize availableSizePx)
  {
    assert(availableSizePx.IsInfinityHeight());
    assert(itrBegin != itrEnd);

    PxSize2D finalSizePx;
    PxSize2D maxElementSizePx;
    if (itrBegin != itrEnd)    // if not empty
    {
      // Fake that we have unlimited space in Y and keep X constrained.
      const PxAvailableSize fakeAvailableSizePx(availableSizePx.Width(), PxAvailableSize1D::InfiniteSpacePx());

      // During this pass we measure all elements to find the max size.
      for (auto itr = itrBegin; itr != itrEnd; ++itr)
      {
        itr->Window->Measure(fakeAvailableSizePx);
        PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
        maxElementSizePx.SetMax(desiredSizePx);
      }

      const auto childCount = NumericCast<int32_t>(std::distance(itrBegin, itrEnd));
      assert(childCount > 0);
      finalSizePx = {maxElementSizePx.Width(),
                     (maxElementSizePx.Height() * PxSize1D::UncheckedCreate(childCount)) + (spacingYPx * PxSize1D::UncheckedCreate(childCount - 1))};
    }
    return {finalSizePx, maxElementSizePx};
  }


  UniformWrapLayout::MeasureResult UniformWrapLayout::MeasureHorizontalWrapLayout(const collection_type::queue_type::const_iterator& itrBegin,
                                                                                  const collection_type::queue_type::const_iterator& itrEnd,
                                                                                  const PxSize2D spacingPx, const PxAvailableSize availableSizePx)
  {
    assert(itrBegin != itrEnd);
    assert(!availableSizePx.IsInfinityWidth());
    assert(availableSizePx.IsNormalWidth());

    PxSize2D finalSizePx;
    PxSize2D maxElementSizePx;
    if (itrBegin != itrEnd)    // if not empty
    {
      // Fake that we have unlimited space in X and keep Y constrained.
      const PxAvailableSize fakeAvailableSizePx(PxAvailableSize1D::InfiniteSpacePx(), availableSizePx.Height());

      // During this pass we measure all elements to find the max size.
      for (auto itr = itrBegin; itr != itrEnd; ++itr)
      {
        itr->Window->Measure(fakeAvailableSizePx);
        PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
        maxElementSizePx.SetMax(desiredSizePx);
      }
      const auto childCount = NumericCast<int32_t>(std::distance(itrBegin, itrEnd));

      const PxSize1D maxWidthForElementsPx = availableSizePx.ToPxWidth() + spacingPx.Width();

      // Figure out how many elements that can fit per line with the given spacing
      const int32_t maxElementsPerLine =
        maxElementSizePx.RawWidth() > 0 ? MathHelper::Max(maxWidthForElementsPx.RawValue() / (maxElementSizePx.RawWidth() + spacingPx.RawWidth()), 1)
                                        : 1;
      int32_t columns = 0;
      int32_t rows = 0;
      if (childCount <= maxElementsPerLine)
      {
        columns = childCount;
        rows = 1;
      }
      else
      {
        columns = maxElementsPerLine;
        rows = (childCount / maxElementsPerLine) + ((childCount % maxElementsPerLine) > 0 ? 1 : 0);
      }

      assert(columns >= 1);
      assert(rows >= 1);
      const PxSize1D finalWidth =
        (maxElementSizePx.Width() * PxSize1D::UncheckedCreate(columns)) + (spacingPx.Width() * PxSize1D::UncheckedCreate(columns - 1));
      const PxSize1D finalHeight =
        (maxElementSizePx.Height() * PxSize1D::UncheckedCreate(rows)) + (spacingPx.Height() * PxSize1D::UncheckedCreate(rows - 1));
      assert(finalWidth >= maxElementSizePx.Width());
      assert(finalHeight >= maxElementSizePx.Height());
      finalSizePx = PxSize2D(finalWidth, finalHeight);
    }
    return {finalSizePx, maxElementSizePx};
  }


  UniformWrapLayout::MeasureResult UniformWrapLayout::MeasureVerticalWrapLayout(const collection_type::queue_type::const_iterator& itrBegin,
                                                                                const collection_type::queue_type::const_iterator& itrEnd,
                                                                                const PxSize2D spacingPx, const PxAvailableSize availableSizePx)
  {
    assert(!availableSizePx.IsInfinityHeight());
    assert(availableSizePx.IsNormalHeight());
    assert(itrBegin != itrEnd);

    PxSize2D finalSizePx;
    PxSize2D maxElementSizePx;
    if (itrBegin != itrEnd)    // if not empty
    {
      // Fake that we have unlimited space in Y and keep X constrained.
      const PxAvailableSize fakeAvailableSizePx(availableSizePx.Width(), PxAvailableSize1D::InfiniteSpacePx());

      // During this pass we measure all elements to find the max size.
      for (auto itr = itrBegin; itr != itrEnd; ++itr)
      {
        itr->Window->Measure(fakeAvailableSizePx);
        PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
        maxElementSizePx.SetMax(desiredSizePx);
      }
      const auto childCount = NumericCast<int32_t>(std::distance(itrBegin, itrEnd));

      const PxSize1D maxHeightPxForElements = availableSizePx.ToPxHeight() + spacingPx.Height();

      // Figure out how many elements that can fit per line with the given spacing
      const int32_t maxElementsPerLine =
        maxElementSizePx.RawHeight() > 0
          ? MathHelper::Max(maxHeightPxForElements.RawValue() / (maxElementSizePx.RawHeight() + spacingPx.RawHeight()), 1)
          : 1;
      int32_t columns = 0;
      int32_t rows = 0;
      if (childCount <= maxElementsPerLine)
      {
        columns = 1;
        rows = childCount;
      }
      else
      {
        columns = (childCount / maxElementsPerLine) + ((childCount % maxElementsPerLine) > 0 ? 1 : 0);
        rows = maxElementsPerLine;
      }

      assert(columns >= 1);
      assert(rows >= 1);
      const PxSize1D finalWidth =
        (maxElementSizePx.Width() * PxSize1D::UncheckedCreate(columns)) + (spacingPx.Width() * PxSize1D::UncheckedCreate(columns - 1));
      const PxSize1D finalHeight =
        (maxElementSizePx.Height() * PxSize1D::UncheckedCreate(rows)) + (spacingPx.Height() * PxSize1D::UncheckedCreate(rows - 1));
      assert(finalWidth >= maxElementSizePx.Width());
      assert(finalHeight >= maxElementSizePx.Height());
      finalSizePx = PxSize2D(finalWidth, finalHeight);
    }
    return {finalSizePx, maxElementSizePx};
  }
}
