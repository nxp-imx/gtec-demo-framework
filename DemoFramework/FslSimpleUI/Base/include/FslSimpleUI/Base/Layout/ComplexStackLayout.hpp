#ifndef FSLSIMPLEUI_BASE_LAYOUT_COMPLEXSTACKLAYOUT_HPP
#define FSLSIMPLEUI_BASE_LAYOUT_COMPLEXSTACKLAYOUT_HPP
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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Math/Dp/DpSize1DF.hpp>
#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslSimpleUI/Base/Layout/ComplexLayout.hpp>
#include <FslSimpleUI/Base/Layout/LayoutLength.hpp>
#include <FslSimpleUI/Base/Layout/LayoutOrientation.hpp>
#include <deque>

namespace Fsl
{
  class SpriteUnitConverter;

  namespace UI
  {
    struct ComplexStackLayoutWindowRecord : GenericWindowCollectionRecordBase
    {
      LayoutUnitType UnitType{LayoutUnitType::Auto};
      PxValue PositionPx;
      PxSize1D SizePx;
      float LayoutSizeMagic{};

      explicit ComplexStackLayoutWindowRecord(const std::shared_ptr<BaseWindow>& window)
        : GenericWindowCollectionRecordBase(window)

      {
      }
    };


    class ComplexStackLayout : public ComplexLayout<ComplexStackLayoutWindowRecord>
    {
      using base_type = ComplexLayout<ComplexStackLayoutWindowRecord>;

      DataBinding::TypedDependencyProperty<LayoutOrientation> m_propertyOrientation;
      DataBinding::TypedDependencyProperty<DpSize1DF> m_propertySpacingDp;
      std::deque<LayoutLength> m_layoutLength;


    public:
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyOrientation;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertySpacing;

      explicit ComplexStackLayout(const std::shared_ptr<BaseWindowContext>& context);

      LayoutOrientation GetOrientation() const
      {
        return m_propertyOrientation.Get();
      }
      bool SetOrientation(const LayoutOrientation value);

      DpSize1DF GetSpacing() const
      {
        return m_propertySpacingDp.Get();
      }
      bool SetSpacing(const DpSize1DF value);

      void ClearLayoutLengths();
      void PushLayoutLength(const LayoutLength& layoutLength);
      void PopLayoutLength();

      using ComplexLayout<ComplexStackLayoutWindowRecord>::AddChild;
      void AddChild(const std::shared_ptr<BaseWindow>& window, const LayoutLength& layoutLength)
      {
        FSLLOG3_DEBUG_WARNING_IF(m_layoutLength.size() != GetChildCount(),
                                 "ComplexStackLayout: AddChild with layoutlength is not setting the layout for the added child!");
        PushLayoutLength(layoutLength);
        ComplexLayout<ComplexStackLayoutWindowRecord>::AddChild(window);
      }

    protected:
      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) override;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) override;

      DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override;
      DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                             const DataBinding::Binding& binding) override;
      void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) override;

    private:
      PxSize2D CalcFixedStarSizeHorizontal(const SpriteUnitConverter& unitConverter, const PxSize2D finalSizePx);
      PxSize2D CalcFixedStarSizeVertical(const SpriteUnitConverter& unitConverter, const PxSize2D finalSizePx);
      void FinalizePositionAndStarSizes(const SpriteUnitConverter& unitConverter, const PxSize1D spaceLeftPx, const float totalStars);
      void ArrangeHorizontal(const PxSize1D finalSizeYPx);
      void ArrangeVertical(const PxSize1D finalSizeXPx);
    };
  }
}

#endif
