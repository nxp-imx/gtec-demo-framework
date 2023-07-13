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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper_MinMax.hpp>
#include <FslDataBinding/Base/Object/DependencyObjectHelper.hpp>
#include <FslDataBinding/Base/Object/DependencyPropertyDefinitionVector.hpp>
#include <FslDataBinding/Base/Property/DependencyPropertyDefinitionFactory.hpp>
#include <FslSimpleUI/Base/Layout/ComplexStackLayout.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/PxAvailableSizeUtil.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <cassert>
#include <cmath>

namespace Fsl::UI
{
  using TClass = ComplexStackLayout;
  using TDef = DataBinding::DependencyPropertyDefinition;
  using TFactory = DataBinding::DependencyPropertyDefinitionFactory;

  TDef TClass::PropertyOrientation = TFactory::Create<LayoutOrientation, TClass, &TClass::GetOrientation, &TClass::SetOrientation>("Orientation");
  TDef TClass::PropertySpacing = TFactory::Create<DpSize1DF, TClass, &TClass::GetSpacing, &TClass::SetSpacing>("Spacing");
}

namespace Fsl::UI
{
  ComplexStackLayout::ComplexStackLayout(const std::shared_ptr<BaseWindowContext>& context)
    : ComplexLayout<ComplexStackLayoutWindowRecord>(context)
    , m_propertyOrientation(LayoutOrientation::Vertical)
  {
  }

  bool ComplexStackLayout::SetOrientation(const LayoutOrientation value)
  {
    const bool changed = m_propertyOrientation.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }


  bool ComplexStackLayout::SetSpacing(const DpSize1DF value)
  {
    const bool changed = m_propertySpacingDp.Set(ThisDependencyObject(), value);
    if (changed)
    {
      PropertyUpdated(PropertyType::Layout);
    }
    return changed;
  }


  void ComplexStackLayout::ClearLayoutLengths()
  {
    if (!m_layoutLength.empty())
    {
      m_layoutLength.clear();
      PropertyUpdated(PropertyType::Layout);
    }
  }


  void ComplexStackLayout::PushLayoutLength(const LayoutLength& layoutLength)
  {
    m_layoutLength.push_back(layoutLength);
    PropertyUpdated(PropertyType::Layout);
  }


  void ComplexStackLayout::PopLayoutLength()
  {
    if (m_layoutLength.empty())
    {
      FSLLOG3_WARNING("There are not layouts to pop, request ignored");
      return;
    }

    m_layoutLength.pop_back();
    PropertyUpdated(PropertyType::Layout);
  }


  PxSize2D ComplexStackLayout::ArrangeOverride(const PxSize2D& finalSizePx)
  {
    PxSize2D resultSizePx;
    const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;
    if (m_propertyOrientation.Get() == LayoutOrientation::Horizontal)
    {
      resultSizePx = CalcFixedStarSizeHorizontal(unitConverter, finalSizePx);
      ArrangeHorizontal(finalSizePx.Height());
    }
    else
    {
      resultSizePx = CalcFixedStarSizeVertical(unitConverter, finalSizePx);
      ArrangeVertical(finalSizePx.Width());
    }
    return resultSizePx;
  }


  PxSize2D ComplexStackLayout::MeasureOverride(const PxAvailableSize& availableSizePx)
  {
    PxSize2D desiredSizePx;
    PxSize1D minSizeXPx;
    PxSize1D minSizeYPx;

    const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;

    auto layoutLengthItr = m_layoutLength.begin();
    const PxSize1D spacingPx = unitConverter.ToPxSize1D(m_propertySpacingDp.Get());
    bool isFirstShared = true;
    if (m_propertyOrientation.Get() == LayoutOrientation::Horizontal)
    {
      // Fake that we have unlimited space in X and keep Y constrained.
      const PxAvailableSize fakeAvailableSizePx(PxAvailableSize1D::InfiniteSpacePx(), availableSizePx.Height());
      LayoutLength layoutLength;
      for (auto itr = begin(); itr != end(); ++itr)
      {
        if (layoutLengthItr != m_layoutLength.end())
        {
          layoutLength = *layoutLengthItr;
          ++layoutLengthItr;
        }

        switch (layoutLength.UnitType())
        {
        case LayoutUnitType::Auto:
        case LayoutUnitType::Star:
          itr->Window->Measure(fakeAvailableSizePx);
          desiredSizePx = itr->Window->DesiredSizePx();
          break;
        case LayoutUnitType::Fixed:
          {
            const PxSize1D fixedLayoutLengthPx = unitConverter.DpToPxSize1D(layoutLength.Value());
            const PxAvailableSize fixedAvailableSizePx(fixedLayoutLengthPx, availableSizePx.Height());
            itr->Window->Measure(fixedAvailableSizePx);
            desiredSizePx = itr->Window->DesiredSizePx();
            desiredSizePx.SetWidth(fixedLayoutLengthPx);
            break;
          }
        case LayoutUnitType::Px:
          {
            const PxSize1D fixedLayoutLengthPx = unitConverter.PxfToPxSize1D(layoutLength.Value());
            const PxAvailableSize fixedAvailableSizePx(fixedLayoutLengthPx, availableSizePx.Height());
            itr->Window->Measure(fixedAvailableSizePx);
            desiredSizePx = itr->Window->DesiredSizePx();
            desiredSizePx.SetWidth(fixedLayoutLengthPx);
            break;
          }
        default:
          FSLLOG3_WARNING("Unsupported LayoutUnitType: {}", static_cast<int32_t>(layoutLength.UnitType()));
          itr->Window->Measure(fakeAvailableSizePx);
          desiredSizePx = itr->Window->DesiredSizePx();
          break;
        }
        auto* pSharedSizeGroup = layoutLength.SharedSizeGroup().get();
        if (pSharedSizeGroup != nullptr)
        {
          if (isFirstShared)
          {
            isFirstShared = false;
            pSharedSizeGroup->ClearMeasure();
          }
          desiredSizePx = PxSize2D(pSharedSizeGroup->MeasureSizePx(desiredSizePx.Width()), desiredSizePx.Height());
        }

        minSizeXPx += desiredSizePx.Width() + spacingPx;
        if (desiredSizePx.Height() > minSizeYPx)
        {
          minSizeYPx = desiredSizePx.Height();
        }
      }
      if (!empty())
      {
        minSizeXPx -= spacingPx;
      }
    }
    else
    {
      // Fake that we have unlimited space in Y and keep X constrained.
      const PxAvailableSize fakeAvailableSizePx(availableSizePx.Width(), PxAvailableSize1D::InfiniteSpacePx());
      LayoutLength layoutLength;
      for (auto itr = begin(); itr != end(); ++itr)
      {
        if (layoutLengthItr != m_layoutLength.end())
        {
          layoutLength = *layoutLengthItr;
          ++layoutLengthItr;
        }

        switch (layoutLength.UnitType())
        {
        case LayoutUnitType::Auto:
        case LayoutUnitType::Star:
          itr->Window->Measure(fakeAvailableSizePx);
          desiredSizePx = itr->Window->DesiredSizePx();
          break;
        case LayoutUnitType::Fixed:
          {
            const PxAvailableSize1D fixedLayoutLengthPx = PxAvailableSize1D::UncheckedCreate(unitConverter.DpToPxInt32(layoutLength.Value()));
            const PxAvailableSize fixedAvailableSizePx(availableSizePx.Width(), fixedLayoutLengthPx);
            itr->Window->Measure(fixedAvailableSizePx);
            desiredSizePx = itr->Window->DesiredSizePx();
            desiredSizePx.SetHeight(fixedLayoutLengthPx.ToPxSize1D());
            break;
          }
        case LayoutUnitType::Px:
          {
            const PxAvailableSize1D fixedLayoutLengthPx = PxAvailableSize1D::UncheckedCreate(unitConverter.PxfToPxInt32(layoutLength.Value()));
            const PxAvailableSize fixedAvailableSizePx(availableSizePx.Width(), fixedLayoutLengthPx);
            itr->Window->Measure(fixedAvailableSizePx);
            desiredSizePx = itr->Window->DesiredSizePx();
            desiredSizePx.SetHeight(fixedLayoutLengthPx.ToPxSize1D());
            break;
          }
        default:
          FSLLOG3_WARNING("Unsupported LayoutUnitType: {0}", static_cast<int32_t>(layoutLength.UnitType()));
          itr->Window->Measure(fakeAvailableSizePx);
          desiredSizePx = itr->Window->DesiredSizePx();
          break;
        }
        auto* pSharedSizeGroup = layoutLength.SharedSizeGroup().get();
        if (pSharedSizeGroup != nullptr)
        {
          if (isFirstShared)
          {
            isFirstShared = false;
            pSharedSizeGroup->ClearMeasure();
          }
          desiredSizePx = PxSize2D(desiredSizePx.Width(), pSharedSizeGroup->MeasureSizePx(desiredSizePx.Height()));
        }

        minSizeYPx += desiredSizePx.Height() + spacingPx;
        if (desiredSizePx.Width() > minSizeXPx)
        {
          minSizeXPx = desiredSizePx.Width();
        }
      }
      if (!empty())
      {
        minSizeYPx -= spacingPx;
      }
    }

    assert(PxAvailableSizeUtil::IsNormalValue(minSizeXPx.RawValue()));
    assert(PxAvailableSizeUtil::IsNormalValue(minSizeYPx.RawValue()));
    return {minSizeXPx, minSizeYPx};
  }

  DataBinding::DataBindingInstanceHandle ComplexStackLayout::TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef)
  {
    auto res = DataBinding::DependencyObjectHelper::TryGetPropertyHandle(this, ThisDependencyObject(), sourceDef,
                                                                         DataBinding::PropLinkRefs(PropertyOrientation, m_propertyOrientation),
                                                                         DataBinding::PropLinkRefs(PropertySpacing, m_propertySpacingDp));
    return res.IsValid() ? res : base_type::TryGetPropertyHandleNow(sourceDef);
  }


  DataBinding::PropertySetBindingResult ComplexStackLayout::TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                                             const DataBinding::Binding& binding)
  {
    auto res = DataBinding::DependencyObjectHelper::TrySetBinding(this, ThisDependencyObject(), targetDef, binding,
                                                                  DataBinding::PropLinkRefs(PropertyOrientation, m_propertyOrientation),
                                                                  DataBinding::PropLinkRefs(PropertySpacing, m_propertySpacingDp));
    return res != DataBinding::PropertySetBindingResult::NotFound ? res : base_type::TrySetBindingNow(targetDef, binding);
  }


  void ComplexStackLayout::ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties)
  {
    ComplexLayout<ComplexStackLayoutWindowRecord>::ExtractAllProperties(rProperties);
    rProperties.push_back(PropertyOrientation);
    rProperties.push_back(PropertySpacing);
  }


  PxSize2D ComplexStackLayout::CalcFixedStarSizeHorizontal(const SpriteUnitConverter& unitConverter, const PxSize2D finalSizePx)
  {
    auto layoutLengthItr = m_layoutLength.begin();

    const auto spacingPx = unitConverter.ToPxSize1D(m_propertySpacingDp.Get());

    // Run through each element and give it the space it desired in Y, but only finalSize.X in X
    float totalStars = 0;
    PxSize1D totalSizePx;
    LayoutLength layoutLength{};
    for (auto itr = begin(); itr != end(); ++itr)
    {
      if (layoutLengthItr != m_layoutLength.end())
      {
        layoutLength = *layoutLengthItr;
        ++layoutLengthItr;
      }

      switch (layoutLength.UnitType())
      {
      case LayoutUnitType::Auto:
        itr->SizePx = !layoutLength.SharedSizeGroup() ? itr->Window->DesiredSizePx().Width() : layoutLength.SharedSizeGroup()->MinimumSizePx;
        itr->LayoutSizeMagic = 0;
        totalSizePx += itr->SizePx;
        break;
      case LayoutUnitType::Fixed:
        itr->SizePx = unitConverter.DpToPxSize1D(layoutLength.Value());
        itr->LayoutSizeMagic = 0;
        totalSizePx += itr->SizePx;
        break;
      case LayoutUnitType::Px:
        itr->SizePx = unitConverter.PxfToPxSize1D(layoutLength.Value());
        itr->LayoutSizeMagic = 0;
        totalSizePx += itr->SizePx;
        break;
      case LayoutUnitType::Star:
        {
          totalStars += layoutLength.Value();
          itr->SizePx = {};    // this will be filled later by FinalizePositionAndStarSizes
          itr->LayoutSizeMagic = layoutLength.Value();
          break;
        }
      default:
        FSLLOG3_WARNING("Unsupported LayoutUnitType: {}", static_cast<int32_t>(layoutLength.UnitType()));
        break;
      }
      itr->UnitType = layoutLength.UnitType();
      totalSizePx += spacingPx;
    }
    if (!empty())
    {
      totalSizePx -= spacingPx;
    }

    // We now know the total size and total stars
    const PxSize1D sizeLeftPx(finalSizePx.Width() - totalSizePx);
    FinalizePositionAndStarSizes(unitConverter, sizeLeftPx, totalStars);
    if (finalSizePx.Width() >= totalSizePx && totalStars > 0)
    {
      totalSizePx = finalSizePx.Width();
    }
    return {totalSizePx, finalSizePx.Height()};
  }


  PxSize2D ComplexStackLayout::CalcFixedStarSizeVertical(const SpriteUnitConverter& unitConverter, const PxSize2D finalSizePx)
  {
    auto layoutLengthItr = m_layoutLength.begin();

    const auto spacingPx = unitConverter.ToPxSize1D(m_propertySpacingDp.Get());

    // Run through each element and give it the space it desired in Y, but only finalSize.X in X
    float totalStars = 0;
    PxSize1D totalSizePx;
    for (auto itr = begin(); itr != end(); ++itr)
    {
      LayoutLength layoutLength;
      if (layoutLengthItr != m_layoutLength.end())
      {
        layoutLength = *layoutLengthItr;
        ++layoutLengthItr;
      }

      switch (layoutLength.UnitType())
      {
      case LayoutUnitType::Auto:
        itr->SizePx = !layoutLength.SharedSizeGroup() ? itr->Window->DesiredSizePx().Height() : layoutLength.SharedSizeGroup()->MinimumSizePx;
        itr->LayoutSizeMagic = 0.0f;
        totalSizePx += itr->SizePx;
        break;
      case LayoutUnitType::Fixed:
        itr->SizePx = unitConverter.DpToPxSize1D(layoutLength.Value());
        itr->LayoutSizeMagic = 0.0f;
        totalSizePx += itr->SizePx;
        break;
      case LayoutUnitType::Px:
        itr->SizePx = unitConverter.PxfToPxSize1D(layoutLength.Value());
        itr->LayoutSizeMagic = 0.0f;
        totalSizePx += itr->SizePx;
        break;
      case LayoutUnitType::Star:
        {
          totalStars += layoutLength.Value();
          itr->SizePx = {};    // this will be filled later by FinalizePositionAndStarSizes
          itr->LayoutSizeMagic = layoutLength.Value();
          break;
        }
      default:
        FSLLOG3_WARNING("Unsupported LayoutUnitType: {}", static_cast<int32_t>(layoutLength.UnitType()));
        break;
      }
      itr->UnitType = layoutLength.UnitType();
      totalSizePx += spacingPx;
    }
    if (!empty())
    {
      totalSizePx -= spacingPx;
    }

    // We now know the total size and total stars
    const PxSize1D sizeLeftPx(finalSizePx.Height() - totalSizePx);
    FinalizePositionAndStarSizes(unitConverter, sizeLeftPx, totalStars);
    if (finalSizePx.Width() >= totalSizePx && totalStars > 0)
    {
      totalSizePx = finalSizePx.Height();
    }
    return {finalSizePx.Width(), totalSizePx};
  }


  void ComplexStackLayout::FinalizePositionAndStarSizes(const SpriteUnitConverter& unitConverter, const PxSize1D spaceLeftPx, const float totalStars)
  {
    // Assign size to the star areas
    const auto spacingPx = unitConverter.ToPxSize1D(m_propertySpacingDp.Get());
    PxValue positionPx;
    PxSize1D maxSpaceLeftPx = spaceLeftPx;
    for (auto itr = begin(); itr != end(); ++itr)
    {
      if (itr->UnitType == LayoutUnitType::Star)
      {
        assert(totalStars > 0.0f);
        const auto finalSizePxf = std::round((itr->LayoutSizeMagic / totalStars) * static_cast<float>(spaceLeftPx.RawValue()));
        const auto finalSizePx = MathHelper::Min(PxSize1D::Create(static_cast<int32_t>(finalSizePxf)), maxSpaceLeftPx);
        itr->SizePx = finalSizePx;
        assert(maxSpaceLeftPx >= finalSizePx);
        maxSpaceLeftPx -= finalSizePx;
      }
      itr->PositionPx = positionPx;
      positionPx += itr->SizePx + spacingPx;
    }
  }


  void ComplexStackLayout::ArrangeHorizontal(const PxSize1D finalSizeYPx)
  {
    // Run through each element and give it the space it desired in X, but only finalSizeY in Y
    for (auto itr = begin(); itr != end(); ++itr)
    {
      itr->Window->Arrange(PxRectangle(itr->PositionPx, PxValue(0), itr->SizePx, finalSizeYPx));
    }
  }


  void ComplexStackLayout::ArrangeVertical(const PxSize1D finalSizeXPx)
  {
    // Run through each element and give it the space it desired in Y, but only finalSizeX in X
    for (auto itr = begin(); itr != end(); ++itr)
    {
      itr->Window->Arrange(PxRectangle(PxValue(0), itr->PositionPx, finalSizeXPx, itr->SizePx));
    }
  }
}
