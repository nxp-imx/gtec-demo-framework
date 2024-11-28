#ifndef FSLSIMPLEUI_CONTROLS_EXPERIMENTAL_RESIZEABLEAREA_HPP
#define FSLSIMPLEUI_CONTROLS_EXPERIMENTAL_RESIZEABLEAREA_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslDataBinding/Base/Property/TypedDependencyProperty.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/Mesh/CustomBasicSpriteBasicMesh.hpp>
#include <FslSimpleUI/Base/Property/DependencyPropertyUIColor.hpp>
#include <FslSimpleUI/Controls/Experimental/Logic/ResizeableAreaDragLogic.hpp>
#include <FslSimpleUI/Controls/Experimental/ResizeableAreaDragHandleLocation.hpp>
#include <FslSimpleUI/Controls/Experimental/ResizeableAreaRenderData.hpp>

namespace Fsl
{
  class ISizedSprite;

  namespace UI
  {
    class WindowContext;

    class ResizeableArea : public BaseWindow
    {
      using base_type = BaseWindow;

      CustomBasicSpriteBasicMesh m_fill;

      DataBinding::TypedDependencyProperty<bool> m_propertyEnabled;
      DataBinding::TypedDependencyProperty<ResizeableAreaDragHandleLocation> m_propertyDragHandleLocation;
      DataBinding::TypedDependencyProperty<DpSize1DF> m_propertyDragHandleSizeDpf;
      DependencyPropertyUIColor m_propertyDragHandleColor;
      DependencyPropertyUIColor m_propertyRectangleColor;

      PxSize1D m_cachedDragHandleSizePx;
      std::shared_ptr<ResizeableAreaRenderData> m_renderData;

      ResizeableAreaDragLogic m_logic;

    public:
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyEnabled;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyDragHandleLocation;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyDragHandleSizeDpf;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyDragHandleColor;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyRectangleColor;

      explicit ResizeableArea(const std::shared_ptr<WindowContext>& context);
      ~ResizeableArea() override;

      const std::shared_ptr<BasicImageSprite>& GetFillSprite() const
      {
        return m_fill.GetSprite();
      }

      void SetFillSprite(const std::shared_ptr<BasicImageSprite>& value);
      void SetFillSprite(std::shared_ptr<BasicImageSprite>&& value);

      bool GetEnabled() const noexcept
      {
        return m_propertyEnabled.Get();
      }

      bool SetEnabled(const bool value);

      ResizeableAreaDragHandleLocation GetDragHandleLocation() const noexcept
      {
        return m_propertyDragHandleLocation.Get();
      }

      bool SetDragHandleLocation(const ResizeableAreaDragHandleLocation value);

      DpSize1DF GetDragHandleSizeDpf() const noexcept
      {
        return m_propertyDragHandleSizeDpf.Get();
      }

      bool SetDragHandleSizeDpf(const DpSize1DF value);

      UIColor GetDragHandleColor() const noexcept
      {
        return m_propertyDragHandleColor.Get();
      }

      bool SetDragHandleColor(const UIColor value);

      UIColor GetRectangleColor() const noexcept
      {
        return m_propertyRectangleColor.Get();
      }

      PxRectangle GetRectangle() const noexcept
      {
        return m_logic.GetAreaRectangle();
      }

      bool SetRectangleColor(const UIColor value);

      void WinResolutionChanged(const ResolutionChangedInfo& info) override;

      void WinDraw(const UIDrawContext& context) override;


    protected:
      UIRenderColor GetRectangleRenderColor() const noexcept
      {
        return m_propertyRectangleColor.InternalColor;
      }

      void UpdateCachedDragHandleSizePx();

      void OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent) override;
      void OnMouseOver(const std::shared_ptr<WindowMouseOverEvent>& theEvent) override;

      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) override;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) override;

      DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override;
      DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                             const DataBinding::Binding& binding) override;
      void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) override;
    };
  }
}
#endif
