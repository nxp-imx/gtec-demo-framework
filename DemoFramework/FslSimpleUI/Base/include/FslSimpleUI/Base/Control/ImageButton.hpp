#ifndef FSLSIMPLEUI_BASE_CONTROL_IMAGEBUTTON_HPP
#define FSLSIMPLEUI_BASE_CONTROL_IMAGEBUTTON_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2023-2024 NXP
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

#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslSimpleUI/Base/Control/ButtonBase.hpp>
#include <FslSimpleUI/Base/ItemScalePolicy.hpp>
#include <FslSimpleUI/Base/Mesh/SizedSpriteMesh.hpp>
#include <FslSimpleUI/Base/Property/DependencyPropertyUIColor.hpp>
#include <FslSimpleUI/Base/Transition/TransitionUIRenderColor.hpp>
#include <FslSimpleUI/Base/UIColor.hpp>

namespace Fsl
{
  class ISizedSprite;

  namespace UI
  {
    class WindowContext;

    //! @brief A simple image button. While you can create a image button with the more
    //         generic button class this one performs better.
    class ImageButton final : public ButtonBase
    {
      using base_type = ButtonBase;

      struct Background
      {
        SizedSpriteMesh Sprite;
        SizedSpriteMesh HoverSprite;
        DependencyPropertyUIColor PropertyColorHoverUp;
        DependencyPropertyUIColor PropertyColorUp;
        DependencyPropertyUIColor PropertyColorDown;
        DependencyPropertyUIColor PropertyColorDisabled;
        DataBinding::TypedDependencyProperty<ItemScalePolicy> PropertyScalePolicy{ItemScalePolicy::NoScaling};

        explicit Background(const std::shared_ptr<IMeshManager>& meshManager, const UIColorConverter converter)
          : Sprite(meshManager)
          , HoverSprite(meshManager)
          , PropertyColorHoverUp(converter, DefaultColor::Button::BackgroundHoverUp)
          , PropertyColorUp(converter, DefaultColor::Button::BackgroundUp)
          , PropertyColorDown(converter, DefaultColor::Button::BackgroundDown)
          , PropertyColorDisabled(converter, DefaultColor::Button::BackgroundDisabled)
        {
        }
      };
      // NOLINTNEXTLINE(readability-identifier-naming)
      const std::shared_ptr<WindowContext> m_windowContext;

      SizedSpriteMesh m_content;

      Background m_background;
      // BackgroundHoverOverlay m_backgroundHoverOverlay;

      DataBinding::TypedDependencyProperty<ItemScalePolicy> m_propertyScalePolicy{ItemScalePolicy::NoScaling};
      DependencyPropertyUIColor m_propertyColorUp;
      DependencyPropertyUIColor m_propertyColorDown;
      DependencyPropertyUIColor m_propertyColorDisabled;


      bool m_isHovering{false};

      TransitionUIRenderColor m_currentColor;
      TransitionUIRenderColor m_backgroundCurrentColor;

    public:
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorHoverUp;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorUp;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorDown;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorDisabled;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundScalePolicy;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyScalePolicy;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyColorUp;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyColorDown;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyColorDisabled;

      explicit ImageButton(const std::shared_ptr<WindowContext>& context);

      const std::shared_ptr<ISizedSprite>& GetContent() const
      {
        return m_content.GetSprite();
      }
      void SetContent(const std::shared_ptr<ISizedSprite>& value);
      void SetContent(std::shared_ptr<ISizedSprite>&& value);


      UIColor GetBackgroundColorHoverUp() const noexcept
      {
        return m_background.PropertyColorHoverUp.Get();
      }

      bool SetBackgroundColorHoverUp(const UIColor value);


      UIColor GetBackgroundColorUp() const noexcept
      {
        return m_background.PropertyColorUp.Get();
      }
      bool SetBackgroundColorUp(const UIColor value);


      UIColor GetBackgroundColorDown() const noexcept
      {
        return m_background.PropertyColorDown.Get();
      }
      bool SetBackgroundColorDown(const UIColor value);


      UIColor GetBackgroundColorDisabled() const noexcept
      {
        return m_background.PropertyColorDisabled.Get();
      }
      bool SetBackgroundColorDisabled(const UIColor value);


      ItemScalePolicy GetBackgroundScalePolicy() const noexcept
      {
        return m_background.PropertyScalePolicy.Get();
      }
      bool SetBackgroundScalePolicy(const ItemScalePolicy value);


      ItemScalePolicy GetScalePolicy() const noexcept
      {
        return m_propertyScalePolicy.Get();
      }
      bool SetScalePolicy(const ItemScalePolicy value);


      UIColor GetColorUp() const noexcept
      {
        return m_propertyColorUp.Get();
      }

      bool SetColorUp(const UIColor value);


      UIColor GetColorDown() const noexcept
      {
        return m_propertyColorDown.Get();
      }
      bool SetColorDown(const UIColor value);


      UIColor GetColorDisabled() const
      {
        return m_propertyColorDisabled.Get();
      }
      bool SetColorDisabled(const UIColor value);


      const std::shared_ptr<ISizedSprite>& GetBackground() const
      {
        return m_background.Sprite.GetSprite();
      }

      void SetBackground(const std::shared_ptr<ISizedSprite>& value);

      const std::shared_ptr<ISizedSprite>& GetBackgroundHover() const
      {
        return m_background.HoverSprite.GetSprite();
      }

      void SetBackgroundHover(const std::shared_ptr<ISizedSprite>& value);


      void WinDraw(const UIDrawContext& context) final;

    protected:
      void OnMouseOver(const std::shared_ptr<WindowMouseOverEvent>& theEvent) final;

      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) final;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) final;

      DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override;
      DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                             const DataBinding::Binding& binding) override;
      void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) override;

      void UpdateAnimation(const TimeSpan& timeSpan) final;
      bool UpdateAnimationState(const bool forceCompleteAnimation) final;
    };
  }
}

#endif
