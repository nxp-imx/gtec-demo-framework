#ifndef FSLSIMPLEUI_BASE_CONTROL_IMAGEBUTTON_HPP
#define FSLSIMPLEUI_BASE_CONTROL_IMAGEBUTTON_HPP
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

#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslGraphics/Transition/TransitionColor.hpp>
#include <FslSimpleUI/Base/Control/ButtonBase.hpp>
#include <FslSimpleUI/Base/ItemScalePolicy.hpp>
#include <FslSimpleUI/Base/Mesh/SizedSpriteMesh.hpp>

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
      struct Background
      {
        SizedSpriteMesh Sprite;
        SizedSpriteMesh HoverSprite;
        Color HoverUpColor{DefaultColor::Button::BackgroundHoverUp};
        Color UpColor{DefaultColor::Button::BackgroundUp};
        Color DownColor{DefaultColor::Button::BackgroundDown};
        Color DisabledColor{DefaultColor::Button::BackgroundDisabled};
        ItemScalePolicy ScalePolicy{ItemScalePolicy::NoScaling};

        explicit Background(const std::shared_ptr<IMeshManager>& meshManager)
          : Sprite(meshManager)
          , HoverSprite(meshManager)
        {
        }
      };
      const std::shared_ptr<WindowContext> m_windowContext;

      SizedSpriteMesh m_content;

      Background m_background;
      // BackgroundHoverOverlay m_backgroundHoverOverlay;

      ItemScalePolicy m_scalePolicy{ItemScalePolicy::NoScaling};
      Color m_upColor{DefaultColor::Button::Up};
      Color m_downColor{DefaultColor::Button::Down};
      Color m_disabledColor{DefaultColor::Button::BackgroundDisabled};

      bool m_isHovering{false};

      TransitionColor m_currentColor;
      TransitionColor m_backgroundCurrentColor;

    public:
      explicit ImageButton(const std::shared_ptr<WindowContext>& context);

      const std::shared_ptr<ISizedSprite>& GetContent() const
      {
        return m_content.GetSprite();
      }
      void SetContent(const std::shared_ptr<ISizedSprite>& value);
      void SetContent(std::shared_ptr<ISizedSprite>&& value);

      Color GetBackgroundColorHoverUp() const
      {
        return m_background.HoverUpColor;
      }

      void SetBackgroundColorHoverUp(const Color& value);

      Color GetBackgroundColorUp() const
      {
        return m_background.UpColor;
      }
      void SetBackgroundColorUp(const Color& value);

      Color GetBackgroundColorDown() const
      {
        return m_background.DownColor;
      }
      void SetBackgroundColorDown(const Color& value);

      Color GetBackgroundColorDisabled() const
      {
        return m_background.DisabledColor;
      }
      void SetBackgroundColorDisabled(const Color& value);


      ItemScalePolicy GetScalePolicy() const
      {
        return m_scalePolicy;
      }
      void SetScalePolicy(const ItemScalePolicy value);

      Color SetUpColor() const

      {
        return m_upColor;
      }
      void SetUpColor(const Color& value);

      Color GetDownColor() const
      {
        return m_downColor;
      }
      void SetDownColor(const Color& value);

      Color GetDisabledColor() const
      {
        return m_disabledColor;
      }
      void SetDisabledColor(const Color& value);

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
      void OnMouseOver(const RoutedEventArgs& args, const std::shared_ptr<WindowMouseOverEvent>& theEvent) final;

      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) final;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) final;

      void UpdateAnimation(const TransitionTimeSpan& timeSpan) final;
      bool UpdateAnimationState(const bool forceCompleteAnimation) final;
    };
  }
}

#endif
