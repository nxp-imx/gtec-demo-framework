#ifndef FSLSIMPLEUI_BASE_CONTROL_BACKGROUNDLABELBUTTON_HPP
#define FSLSIMPLEUI_BASE_CONTROL_BACKGROUNDLABELBUTTON_HPP
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

#include <FslBase/Math/Dp/DpThickness.hpp>
#include <FslSimpleUI/Base/Control/ButtonBase.hpp>
#include <FslSimpleUI/Base/DefaultValues.hpp>
#include <FslSimpleUI/Base/Mesh/ContentSpriteMesh.hpp>
#include <FslSimpleUI/Base/Mesh/SimpleSpriteFontMesh.hpp>
#include <FslSimpleUI/Base/Mesh/SpriteMesh.hpp>
#include <FslSimpleUI/Base/Property/DependencyPropertyUIColor.hpp>
#include <FslSimpleUI/Base/Transition/TransitionUIRenderColor.hpp>
#include <FslSimpleUI/Base/UIColor.hpp>
#include <string>

namespace Fsl
{
  class SpriteFont;

  namespace UI
  {
    class WindowContext;

    class BackgroundLabelButton final : public ButtonBase
    {
      using base_type = ButtonBase;

    protected:
      // NOLINTNEXTLINE(readability-identifier-naming)
      const std::shared_ptr<WindowContext> m_windowContext;

    private:
      SimpleSpriteFontMesh m_fontMesh;
      ContentSpriteMesh m_backgroundNormal;
      ContentSpriteMesh m_backgroundHover;
      ContentSpriteMesh m_backgroundHoverOverlay;

      DependencyPropertyUIColor m_propertyBackgroundHoverOverlayColorUp;
      DependencyPropertyUIColor m_propertyBackgroundHoverOverlayColorDown;
      DependencyPropertyUIColor m_propertyBackgroundColorHoverUp;
      DependencyPropertyUIColor m_propertyBackgroundColorUp;
      DependencyPropertyUIColor m_propertyBackgroundColorDown;
      DependencyPropertyUIColor m_propertyBackgroundColorDisabled;
      DependencyPropertyUIColor m_propertyFontColorUp;
      DependencyPropertyUIColor m_propertyFontColorDown;
      DependencyPropertyUIColor m_propertyFontColorDisabled;
      DataBinding::TypedDependencyProperty<DpThicknessF> m_propertyPaddingDp;
      DataBinding::TypedDependencyProperty<ItemAlignment> m_propertyContentAlignmentX{ItemAlignment::Near};
      DataBinding::TypedDependencyProperty<ItemAlignment> m_propertyContentAlignmentY{ItemAlignment::Near};
      DataBinding::TypedDependencyProperty<StringViewLite> m_propertyContent;

      SpriteFontMeasureInfo m_labelMeasureInfo;
      bool m_isHovering{false};

      TransitionUIRenderColor m_backgroundCurrentColor;
      TransitionUIRenderColor m_backgroundCurrentHoverOverlayColor;
      TransitionUIRenderColor m_fontCurrentColor;

    public:
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundHoverOverlayColorUp;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundHoverOverlayColorDown;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorHoverUp;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorUp;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorDown;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorDisabled;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyFontColorUp;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyFontColorDown;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyFontColorDisabled;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyPadding;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyContentAlignmentX;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyContentAlignmentY;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyContent;

      explicit BackgroundLabelButton(const std::shared_ptr<WindowContext>& context);


      const std::shared_ptr<IContentSprite>& GetBackground() const
      {
        return m_backgroundNormal.GetSprite();
      }

      void SetBackground(const std::shared_ptr<IContentSprite>& value);

      const std::shared_ptr<IContentSprite>& GetBackgroundHover() const
      {
        return m_backgroundHover.GetSprite();
      }

      void SetBackgroundHover(const std::shared_ptr<IContentSprite>& value);

      const std::shared_ptr<IContentSprite>& GetBackgroundHoverOverlay() const
      {
        return m_backgroundHoverOverlay.GetSprite();
      }

      void SetBackgroundHoverOverlay(const std::shared_ptr<IContentSprite>& value);


      StringViewLite GetContent() const noexcept
      {
        return m_propertyContent.Get();
      }

      bool SetContent(const StringViewLite value);
      bool SetContent(std::string&& value);
      bool SetContent(const char* const psz)
      {
        return SetContent(StringViewLite(psz));
      }
      bool SetContent(const std::string& str)
      {
        return SetContent(StringViewLite(str));
      }

      const std::shared_ptr<SpriteFont>& GetFont() const
      {
        return m_fontMesh.GetSprite();
      }
      void SetFont(const std::shared_ptr<SpriteFont>& value);


      UIColor GetBackgroundHoverOverlayColorUp() const noexcept
      {
        return m_propertyBackgroundHoverOverlayColorUp.Get();
      }

      bool SetBackgroundHoverOverlayColorUp(const UIColor value);


      UIColor GetBackgroundHoverOverlayColorDown() const noexcept
      {
        return m_propertyBackgroundHoverOverlayColorDown.Get();
      }

      bool SetBackgroundHoverOverlayColorDown(const UIColor value);


      UIColor GetBackgroundColorHoverUp() const noexcept
      {
        return m_propertyBackgroundColorHoverUp.Get();
      }

      bool SetBackgroundColorHoverUp(const UIColor value);


      UIColor GetBackgroundColorUp() const noexcept
      {
        return m_propertyBackgroundColorUp.Get();
      }

      bool SetBackgroundColorUp(const UIColor value);


      UIColor GetBackgroundColorDown() const noexcept
      {
        return m_propertyBackgroundColorDown.Get();
      }

      bool SetBackgroundColorDown(const UIColor value);


      UIColor GetBackgroundColorDisabled() const noexcept
      {
        return m_propertyBackgroundColorDisabled.Get();
      }

      bool SetBackgroundColorDisabled(const UIColor value);


      UIColor GetFontColorUp() const noexcept
      {
        return m_propertyFontColorUp.Get();
      }

      bool SetFontColorUp(const UIColor value);


      UIColor GetFontColorDown() const noexcept
      {
        return m_propertyFontColorDown.Get();
      }

      bool SetFontColorDown(const UIColor value);


      UIColor GetFontColorDisabled() const noexcept
      {
        return m_propertyFontColorDisabled.Get();
      }

      bool SetFontColorDisabled(const UIColor value);


      DpThicknessF GetPadding() const noexcept
      {
        return m_propertyPaddingDp.Get();
      }

      bool SetPadding(const DpThicknessF value);


      ItemAlignment GetContentAlignmentX() const noexcept
      {
        return m_propertyContentAlignmentX.Get();
      };

      bool SetContentAlignmentX(const ItemAlignment value);


      ItemAlignment GetContentAlignmentY() const noexcept
      {
        return m_propertyContentAlignmentY.Get();
      };

      bool SetContentAlignmentY(const ItemAlignment value);

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

    private:
      UIRenderColor GetBackgroundColor(const bool isEnabled, const bool isDown, const bool isHovering) const;
    };
  }
}

#endif
