#ifndef FSLSIMPLEUI_BASE_CONTROL_TOGGLEBUTTON_HPP
#define FSLSIMPLEUI_BASE_CONTROL_TOGGLEBUTTON_HPP
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

#include <FslBase/Math/Dp/DpPoint.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslBase/Transition/TransitionVector2.hpp>
#include <FslGraphics/Transition/TransitionColor.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/Mesh/SpriteFontMesh.hpp>
#include <FslSimpleUI/Base/Mesh/SizedSpriteMesh.hpp>
#include <memory>
#include <string>
#include <utility>

namespace Fsl
{
  class ISizedSprite;
  class SpriteFont;

  namespace UI
  {
    class WindowContext;

    class ToggleButton : public BaseWindow
    {
      struct FontRecord
      {
        SpriteFontMesh Mesh;
        Color CheckedColor{DefaultColor::Palette::Font};
        Color UncheckedColor{DefaultColor::Palette::Font};
        Color DisabledColor{DefaultColor::Palette::FontDisabled};
        TransitionColor CurrentColor;

        explicit FontRecord(const std::shared_ptr<IMeshManager>& meshManager, const std::shared_ptr<SpriteFont>& font,
                            TransitionCache& rTransitionCache, const TransitionTimeSpan& time, const TransitionType type)
          : Mesh(meshManager)
          , CurrentColor(rTransitionCache, time, type)
        {
          Mesh.SetSprite(font);
        }
      };

      struct GraphicsRecord
      {
        SizedSpriteMesh Mesh;
        Color CheckedColor{DefaultColor::ToggleButton::CursorChecked};
        Color UncheckedColor{DefaultColor::ToggleButton::CursorChecked};
        Color CheckedDisabledColor{DefaultColor::ToggleButton::CursorCheckedDisabled};
        Color UncheckedDisabledColor{DefaultColor::ToggleButton::CursorUncheckedDisabled};
        TransitionColor CurrentColor;

        explicit GraphicsRecord(const std::shared_ptr<IMeshManager>& meshManager, TransitionCache& rTransitionCache, const TransitionTimeSpan& time,
                                const TransitionType type)
          : Mesh(meshManager)
          , CurrentColor(rTransitionCache, time, type)
        {
        }
      };

      struct HoverStateRecord
      {
        Color PrimaryColor;
        DpPoint PositionDp;

        HoverStateRecord() = default;
        explicit HoverStateRecord(const Color& primaryColor)
          : PrimaryColor(primaryColor)
        {
        }
      };

      struct HoverOverlayRecord
      {
        bool IsHovering{false};
        bool IsConstrainToGraphics{false};
        SizedSpriteMesh Mesh;
        HoverStateRecord Checked{DefaultColor::ToggleButton::HoverOverlayChecked};
        HoverStateRecord Unchecked{DefaultColor::ToggleButton::HoverOverlayUnchecked};
        TransitionColor CurrentColor;
        TransitionVector2 CurrentPositionDp;
        PxPoint2 LastScreenPositionPx;

        HoverOverlayRecord(const std::shared_ptr<IMeshManager>& meshManager, TransitionCache& rTransitionCache, const TransitionTimeSpan& timeColor,
                           const TransitionType typeColor, const TransitionTimeSpan& timePosition, const TransitionType typePosition)
          : Mesh(meshManager)
          , CurrentColor(rTransitionCache, timeColor, typeColor)
          , CurrentPositionDp(rTransitionCache, timePosition, typePosition)
        {
        }
      };

    protected:
      const std::shared_ptr<WindowContext> m_windowContext;

    private:
      FontRecord m_font;
      GraphicsRecord m_cursor;
      GraphicsRecord m_background;
      HoverOverlayRecord m_hoverOverlay;
      SpriteFontMeasureInfo m_cachedFontMeasureInfo;

      ItemAlignment m_imageAlignment{ItemAlignment::Near};
      bool m_isChecked{false};
      bool m_isEnabled{true};

    public:
      explicit ToggleButton(const std::shared_ptr<WindowContext>& context);

      bool IsEnabled() const
      {
        return m_isEnabled;
      }

      //! @brief When disabled the slider can not be modified by the user
      bool SetEnabled(const bool enabled);

      const std::shared_ptr<SpriteFont>& GetFont() const
      {
        return m_font.Mesh.GetSprite();
      }

      void SetFont(const std::shared_ptr<SpriteFont>& value);

      const std::string& GetText() const
      {
        return m_font.Mesh.GetText();
      }

      void SetText(const StringViewLite& strView);
      void SetText(std::string&& value);
      void SetText(const char* const psz)
      {
        SetText(StringViewLite(psz));
      }
      void SetText(const std::string& str)
      {
        SetText(StringViewLiteUtil::AsStringViewLite(str));
      }

      ItemAlignment GetImageAlignment() const
      {
        return m_imageAlignment;
      }

      bool SetImageAlignment(const ItemAlignment alignment);


      Color GetHoverOverlayCheckedColor() const
      {
        return m_hoverOverlay.Checked.PrimaryColor;
      }
      void SetHoverOverlayCheckedColor(const Color& value);

      Color GetHoverOverlayUncheckedColor() const
      {
        return m_hoverOverlay.Unchecked.PrimaryColor;
      }
      void SetHoverOverlayUncheckedColor(const Color& value);

      DpPoint GetCursorCheckedPosition() const
      {
        return m_hoverOverlay.Checked.PositionDp;
      }

      void SetCursorCheckedPosition(const DpPoint& valueDp);

      DpPoint GetCursorUncheckedPosition() const
      {
        return m_hoverOverlay.Unchecked.PositionDp;
      }

      void SetCursorUncheckedPosition(const DpPoint& valueDp);

      const std::shared_ptr<ISizedSprite>& SetHoverOverlaySprite() const
      {
        return m_hoverOverlay.Mesh.GetSprite();
      }

      void SetHoverOverlaySprite(const std::shared_ptr<ISizedSprite>& value);


      Color GetCursorCheckedColor() const
      {
        return m_cursor.CheckedColor;
      }
      void SetCursorCheckedColor(const Color& value);

      Color GetCursorCheckedDisabledColor() const
      {
        return m_cursor.CheckedDisabledColor;
      }
      void SetCursorCheckedDisabledColor(const Color& value);

      Color GetCursorUncheckedColor() const
      {
        return m_cursor.UncheckedColor;
      }
      void SetCursorUncheckedColor(const Color& value);

      Color GetCursorUncheckedDisabledColor() const
      {
        return m_cursor.UncheckedDisabledColor;
      }
      void SetCursorUncheckedDisabledColor(const Color& value);

      Color GetBackgroundCheckedColor() const
      {
        return m_background.CheckedColor;
      }
      void SetBackgroundCheckedColor(const Color& value);

      Color GetBackgroundCheckedDisabledColor() const
      {
        return m_background.CheckedDisabledColor;
      }
      void SetBackgroundCheckedDisabledColor(const Color& value);

      Color GetBackgroundUncheckedColor() const
      {
        return m_background.UncheckedColor;
      }
      void SetBackgroundUncheckedColor(const Color& value);

      Color GetBackgroundUncheckedDisabledColor() const
      {
        return m_background.UncheckedDisabledColor;
      }
      void SetBackgroundUncheckedDisabledColor(const Color& value);

      Color GetFontCheckedkColor() const
      {
        return m_font.CheckedColor;
      }
      void SetFontCheckedkColor(const Color& value);

      Color GetFontUncheckColor() const
      {
        return m_font.UncheckedColor;
      }
      void SetFontUncheckColor(const Color& value);

      Color GetFontDisabledColor() const
      {
        return m_font.DisabledColor;
      }
      void SetFontDisabledColor(const Color& value);

      const std::shared_ptr<ISizedSprite>& GetCheckedTexture() const
      {
        return m_cursor.Mesh.GetSprite();
      }
      void SetCheckedSprite(const std::shared_ptr<ISizedSprite>& value);

      const std::shared_ptr<ISizedSprite>& GetUncheckedTexture() const
      {
        return m_background.Mesh.GetSprite();
      }
      void SetUncheckedSprite(const std::shared_ptr<ISizedSprite>& value);

      bool IsChecked() const
      {
        return m_isChecked;
      }

      virtual void SetIsChecked(const bool value);

      void Toggle();

      void WinDraw(const UIDrawContext& context) override;

    protected:
      void OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent) final;
      void OnMouseOver(const RoutedEventArgs& args, const std::shared_ptr<WindowMouseOverEvent>& theEvent) final;

      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) final;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) final;

      void UpdateAnimation(const TransitionTimeSpan& timeSpan) override;
      bool UpdateAnimationState(const bool forceCompleteAnimation) override;

    private:
      PxSize2D GetMaxSpriteSize() const;
    };
  }
}

#endif
