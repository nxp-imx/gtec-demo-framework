#ifndef FSLSIMPLEUI_BASE_CONTROL_LABELNINESLICEBUTTON_HPP
#define FSLSIMPLEUI_BASE_CONTROL_LABELNINESLICEBUTTON_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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
#include <FslSimpleUI/Base/DefaultValues.hpp>
#include <FslSimpleUI/Base/Control/ButtonBase.hpp>
#include <string>

namespace Fsl
{
  class NineSliceSprite;
  class SpriteFont;

  namespace UI
  {
    class WindowContext;

    class LabelNineSliceButton final : public ButtonBase
    {
    protected:
      const std::shared_ptr<WindowContext> m_windowContext;

    private:
      std::string m_content;
      std::shared_ptr<SpriteFont> m_font;
      Color m_backgroundColorHoverOverlayUp{DefaultColor::Button::BackgroundHoverOverlayUp};
      Color m_backgroundColorHoverOverlayDown{DefaultColor::Button::BackgroundHoverOverlayDown};
      Color m_backgroundColorHoverUp{DefaultColor::Button::BackgroundHoverUp};
      Color m_backgroundColorUp{DefaultColor::Button::BackgroundUp};
      Color m_backgroundColorDown{DefaultColor::Button::BackgroundDown};
      Color m_backgroundColorDisabled{DefaultColor::Button::BackgroundDisabled};
      Color m_fontColorUp{DefaultColor::Button::FontUp};
      Color m_fontColorDown{DefaultColor::Button::FontDown};
      Color m_fontColorDisabled{DefaultColor::Button::FontDisabled};

      DpThickness m_paddingDp;
      std::shared_ptr<NineSliceSprite> m_backgroundNormal;
      std::shared_ptr<NineSliceSprite> m_backgroundHover;
      std::shared_ptr<NineSliceSprite> m_backgroundHoverOverlay;
      ItemAlignment m_contentAlignmentX{ItemAlignment::Near};
      ItemAlignment m_contentAlignmentY{ItemAlignment::Near};
      PxSize2D m_labelSizePx;
      bool m_isHovering{false};

      TransitionColor m_backgroundCurrentColor;
      TransitionColor m_backgroundCurrentHoverOverlayColor;
      TransitionColor m_fontCurrentColor;

    public:
      explicit LabelNineSliceButton(const std::shared_ptr<WindowContext>& context);

      ItemAlignment GetContentAlignmentX() const
      {
        return m_contentAlignmentX;
      }

      void SetContentAlignmentX(const ItemAlignment& value);

      ItemAlignment GetContentAlignmentY() const
      {
        return m_contentAlignmentY;
      };

      void SetContentAlignmentY(const ItemAlignment& value);


      DpThickness GetPadding() const
      {
        return m_paddingDp;
      }

      void SetPadding(const DpThickness& valueDp);


      const std::shared_ptr<NineSliceSprite>& GetBackground() const
      {
        return m_backgroundNormal;
      }

      void SetBackground(const std::shared_ptr<NineSliceSprite>& value);

      const std::shared_ptr<NineSliceSprite>& GetBackgroundHover() const
      {
        return m_backgroundHover;
      }

      void SetBackgroundHover(const std::shared_ptr<NineSliceSprite>& value);

      const std::shared_ptr<NineSliceSprite>& GetBackgroundHoverOverlay() const
      {
        return m_backgroundHoverOverlay;
      }

      void SetBackgroundHoverOverlay(const std::shared_ptr<NineSliceSprite>& value);

      const std::string& GetContent() const
      {
        return m_content;
      }

      void SetContent(const StringViewLite& strView);
      void SetContent(std::string&& value);
      void SetContent(const char* const psz)
      {
        SetContent(StringViewLite(psz));
      }
      void SetContent(const std::string& str)
      {
        SetContent(StringViewLiteUtil::AsStringViewLite(str));
      }

      const std::shared_ptr<SpriteFont>& GetFont() const
      {
        return m_font;
      }
      void SetFont(const std::shared_ptr<SpriteFont>& value);

      Color GetBackgroundHoverOverlayColorUp() const
      {
        return m_backgroundColorHoverOverlayUp;
      }
      void SetBackgroundHoverOverlayColorUp(const Color& value);

      Color GetBackgroundHoverOverlayColorDown() const
      {
        return m_backgroundColorHoverOverlayDown;
      }
      void SetBackgroundHoverOverlayColorDown(const Color& value);

      Color GetBackgroundColorHoverUp() const
      {
        return m_backgroundColorHoverUp;
      }
      void SetBackgroundColorHoverUp(const Color& value);

      Color GetBackgroundColorUp() const
      {
        return m_backgroundColorUp;
      }
      void SetBackgroundColorUp(const Color& value);

      Color GetBackgroundColorDown() const
      {
        return m_backgroundColorDown;
      }
      void SetBackgroundColorDown(const Color& value);

      Color GetBackgroundColorDisabled() const
      {
        return m_backgroundColorDisabled;
      }
      void SetBackgroundColorDisabled(const Color& value);


      Color GetFontColorUp() const
      {
        return m_fontColorUp;
      }
      void SetFontColorUp(const Color& value);

      Color GetFontColorDown() const
      {
        return m_fontColorDown;
      }
      void SetFontColorDown(const Color& value);

      Color GetFontColorDisabled() const
      {
        return m_fontColorDisabled;
      }
      void SetFontColorDisabled(const Color& value);

      void WinDraw(const UIDrawContext& context) final;

    protected:
      void OnMouseOver(const RoutedEventArgs& args, const std::shared_ptr<WindowMouseOverEvent>& theEvent) final;
      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) final;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) final;

      void UpdateAnimation(const TransitionTimeSpan& timeSpan) final;
      bool UpdateAnimationState(const bool forceCompleteAnimation) final;

    private:
      Color GetBackgroundColor(const bool isEnabled, const bool isDown, const bool isHovering) const;
    };
  }
}

#endif
