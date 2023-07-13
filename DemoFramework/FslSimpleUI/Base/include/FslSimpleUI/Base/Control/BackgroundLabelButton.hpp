#ifndef FSLSIMPLEUI_BASE_CONTROL_BACKGROUNDLABELBUTTON_HPP
#define FSLSIMPLEUI_BASE_CONTROL_BACKGROUNDLABELBUTTON_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2023 NXP
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
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Transition/TransitionColor.hpp>
#include <FslSimpleUI/Base/Control/ButtonBase.hpp>
#include <FslSimpleUI/Base/DefaultValues.hpp>
#include <FslSimpleUI/Base/Mesh/ContentSpriteMesh.hpp>
#include <FslSimpleUI/Base/Mesh/SimpleSpriteFontMesh.hpp>
#include <FslSimpleUI/Base/Mesh/SpriteMesh.hpp>
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
      const std::shared_ptr<WindowContext> m_windowContext;

    private:
      SimpleSpriteFontMesh m_fontMesh;
      ContentSpriteMesh m_backgroundNormal;
      ContentSpriteMesh m_backgroundHover;
      ContentSpriteMesh m_backgroundHoverOverlay;

      DataBinding::TypedDependencyProperty<Color> m_propertyBackgroundHoverOverlayColorUp{DefaultColor::Button::BackgroundHoverOverlayUp};
      DataBinding::TypedDependencyProperty<Color> m_propertyBackgroundHoverOverlayColorDown{DefaultColor::Button::BackgroundHoverOverlayDown};
      DataBinding::TypedDependencyProperty<Color> m_propertyBackgroundColorHoverUp{DefaultColor::Button::BackgroundHoverUp};
      DataBinding::TypedDependencyProperty<Color> m_propertyBackgroundColorUp{DefaultColor::Button::BackgroundUp};
      DataBinding::TypedDependencyProperty<Color> m_propertyBackgroundColorDown{DefaultColor::Button::BackgroundDown};
      DataBinding::TypedDependencyProperty<Color> m_propertyBackgroundColorDisabled{DefaultColor::Button::BackgroundDisabled};
      DataBinding::TypedDependencyProperty<Color> m_propertyFontColorUp{DefaultColor::Button::FontUp};
      DataBinding::TypedDependencyProperty<Color> m_propertyFontColorDown{DefaultColor::Button::FontDown};
      DataBinding::TypedDependencyProperty<Color> m_propertyFontColorDisabled{DefaultColor::Button::FontDisabled};
      DataBinding::TypedDependencyProperty<DpThicknessF> m_propertyPaddingDp;
      DataBinding::TypedDependencyProperty<ItemAlignment> m_propertyContentAlignmentX{ItemAlignment::Near};
      DataBinding::TypedDependencyProperty<ItemAlignment> m_propertyContentAlignmentY{ItemAlignment::Near};
      DataBinding::TypedDependencyProperty<StringViewLite> m_propertyContent;

      SpriteFontMeasureInfo m_labelMeasureInfo;
      bool m_isHovering{false};

      TransitionColor m_backgroundCurrentColor;
      TransitionColor m_backgroundCurrentHoverOverlayColor;
      TransitionColor m_fontCurrentColor;

    public:
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundHoverOverlayColorUp;
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundHoverOverlayColorDown;
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorHoverUp;
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorUp;
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorDown;
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorDisabled;
      static DataBinding::DependencyPropertyDefinition PropertyFontColorUp;
      static DataBinding::DependencyPropertyDefinition PropertyFontColorDown;
      static DataBinding::DependencyPropertyDefinition PropertyFontColorDisabled;
      static DataBinding::DependencyPropertyDefinition PropertyPadding;
      static DataBinding::DependencyPropertyDefinition PropertyContentAlignmentX;
      static DataBinding::DependencyPropertyDefinition PropertyContentAlignmentY;
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
        return SetContent(StringViewLiteUtil::AsStringViewLite(str));
      }

      const std::shared_ptr<SpriteFont>& GetFont() const
      {
        return m_fontMesh.GetSprite();
      }
      void SetFont(const std::shared_ptr<SpriteFont>& value);


      Color GetBackgroundHoverOverlayColorUp() const noexcept
      {
        return m_propertyBackgroundHoverOverlayColorUp.Get();
      }

      bool SetBackgroundHoverOverlayColorUp(const Color value);


      Color GetBackgroundHoverOverlayColorDown() const noexcept
      {
        return m_propertyBackgroundHoverOverlayColorDown.Get();
      }

      bool SetBackgroundHoverOverlayColorDown(const Color value);


      Color GetBackgroundColorHoverUp() const noexcept
      {
        return m_propertyBackgroundColorHoverUp.Get();
      }

      bool SetBackgroundColorHoverUp(const Color value);


      Color GetBackgroundColorUp() const noexcept
      {
        return m_propertyBackgroundColorUp.Get();
      }

      bool SetBackgroundColorUp(const Color value);


      Color GetBackgroundColorDown() const noexcept
      {
        return m_propertyBackgroundColorDown.Get();
      }

      bool SetBackgroundColorDown(const Color value);


      Color GetBackgroundColorDisabled() const noexcept
      {
        return m_propertyBackgroundColorDisabled.Get();
      }

      bool SetBackgroundColorDisabled(const Color value);


      Color GetFontColorUp() const noexcept
      {
        return m_propertyFontColorUp.Get();
      }

      bool SetFontColorUp(const Color value);


      Color GetFontColorDown() const noexcept
      {
        return m_propertyFontColorDown.Get();
      }

      bool SetFontColorDown(const Color value);


      Color GetFontColorDisabled() const noexcept
      {
        return m_propertyFontColorDisabled.Get();
      }

      bool SetFontColorDisabled(const Color value);


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
      void OnMouseOver(const RoutedEventArgs& args, const std::shared_ptr<WindowMouseOverEvent>& theEvent) final;
      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) final;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) final;

      DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override;
      DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                             const DataBinding::Binding& binding) override;
      void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) override;

      void UpdateAnimation(const TimeSpan& timeSpan) final;
      bool UpdateAnimationState(const bool forceCompleteAnimation) final;

    private:
      Color GetBackgroundColor(const bool isEnabled, const bool isDown, const bool isHovering) const;
    };
  }
}

#endif
