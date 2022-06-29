/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslSimpleUI/Base/BaseWindowContext.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <cassert>
#include <cmath>

namespace Fsl::UI
{
  using TClass = StackLayout;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyOrientation = TFactory::Create<LayoutOrientation, TClass, &TClass::GetOrientation, &TClass::SetOrientation>("Orientation");
  TDef TClass::PropertySpacing = TFactory::Create<DpSize1DF, TClass, &TClass::GetSpacing, &TClass::SetSpacing>("Spacing");
}


namespace Fsl::UI
{
  StackLayout::StackLayout(const std::shared_ptr<BaseWindowContext>& context)
    : SimpleLayout(context)
    , m_propertyOrientation(LayoutOrientation::Vertical)
  {
  }


  bool StackLayout::SetOrientation(const LayoutOrientation value)
  {
    const bool changed = m_propertyOrientation.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }


  bool StackLayout::SetSpacing(const DpSize1DF value)
  {
    const bool changed = m_propertySpacingDp.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }


  PxSize2D StackLayout::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;
    const auto spacePx = unitConverter.ToPxSize1D(m_propertySpacingDp.Get());
    if (m_propertyOrientation.Get() == LayoutOrientation::Horizontal)
    {
      // Run through each element and give it the space it desired in X, but only finalSize.Y in Y
      int32_t posPx = 0;
      int32_t elementDesiredXPx = 0;
      for (auto itr = begin(); itr != end(); ++itr)
      {
        elementDesiredXPx = itr->Window->DesiredSizePx().Width();
        itr->Window->Arrange(PxRectangle(posPx, 0, elementDesiredXPx, finalSizePx.Height()));
        posPx += elementDesiredXPx + spacePx.RawValue();
      }
      if (!empty())
      {
        posPx -= spacePx.RawValue();
      }
      return {posPx, finalSizePx.Height()};
    }


    // Run through each element and give it the space it desired in Y, but only finalSize.X in X
    int32_t posPx = 0;
    int32_t elementDesiredYPx = 0;
    for (auto itr = begin(); itr != end(); ++itr)
    {
      elementDesiredYPx = itr->Window->DesiredSizePx().Height();
      itr->Window->Arrange(PxRectangle(0, posPx, finalSizePx.Width(), elementDesiredYPx));
      posPx += elementDesiredYPx + spacePx.RawValue();
    }
    if (!empty())
    {
      posPx -= spacePx.RawValue();
    }
    return {finalSizePx.Width(), posPx};
  }


  PxSize2D StackLayout::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    PxPoint2 minSizePx;

    const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;

    const auto spacePx = unitConverter.ToPxSize1D(m_propertySpacingDp.Get());
    if (m_propertyOrientation.Get() == LayoutOrientation::Horizontal)
    {
      // Fake that we have unlimited space in X and keep Y constrained.
      const PxAvailableSize fakeAvailableSizePx(PxAvailableSizeUtil::InfiniteSpacePx, availableSizePx.Height());
      for (auto itr = begin(); itr != end(); ++itr)
      {
        itr->Window->Measure(fakeAvailableSizePx);
        PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
        minSizePx.X += desiredSizePx.Width() + spacePx.RawValue();
        if (desiredSizePx.Height() > minSizePx.Y)
        {
          minSizePx.Y = desiredSizePx.Height();
        }
      }
      if (!empty())
      {
        minSizePx.X -= spacePx.RawValue();
      }
    }
    else
    {
      // Fake that we have unlimited space in Y and keep X constrained.
      const PxAvailableSize fakeAvailableSizePx(availableSizePx.Width(), PxAvailableSizeUtil::InfiniteSpacePx);
      for (auto itr = begin(); itr != end(); ++itr)
      {
        itr->Window->Measure(fakeAvailableSizePx);
        PxSize2D desiredSizePx = itr->Window->DesiredSizePx();
        minSizePx.Y += desiredSizePx.Height() + spacePx.RawValue();
        if (desiredSizePx.Width() > minSizePx.X)
        {
          minSizePx.X = desiredSizePx.Width();
        }
      }
      if (!empty())
      {
        minSizePx.Y -= spacePx.RawValue();
      }
    }

    return TypeConverter::UncheckedTo<PxSize2D>(minSizePx);
  }

  DataBinding::DataBindingInstanceHandle StackLayout::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(this, ThisDependencyObject(), sourceDef,
                                                                         DataBinding::PropLinkRefs(PropertyOrientation, m_propertyOrientation),
                                                                         DataBinding::PropLinkRefs(PropertySpacing, m_propertySpacingDp));
    return res.IsValid() ? res : SimpleLayout::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult StackLayout::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                      const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(this, ThisDependencyObject(), targetDef, binding,
                                                                  DataBinding::PropLinkRefs(PropertyOrientation, m_propertyOrientation),
                                                                  DataBinding::PropLinkRefs(PropertySpacing, m_propertySpacingDp));
    return res != DataBinding::PropertySetBindingResult::NotFound ? res : SimpleLayout::TrySetBindingNow(targetDef, binding);
  }


  void StackLayout::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    SimpleLayout::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyOrientation);
    rProperties.push_back(PropertySpacing);
  }

}
