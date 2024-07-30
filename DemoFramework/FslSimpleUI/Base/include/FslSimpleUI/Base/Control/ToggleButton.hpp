#ifndef FSLSIMPLEUI_BASE_CONTROL_TOGGLEBUTTON_HPP
#define FSLSIMPLEUI_BASE_CONTROL_TOGGLEBUTTON_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2024 NXP
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

#include <FslBase/Math/Dp/DpPoint2.hpp>
#include <FslBase/Transition/TransitionVector2.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/Mesh/SimpleSpriteFontMesh.hpp>
#include <FslSimpleUI/Base/Mesh/SizedSpriteMesh.hpp>
#include <FslSimpleUI/Base/Property/DependencyPropertyUIColor.hpp>
#include <FslSimpleUI/Base/Transition/TransitionUIRenderColor.hpp>
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
      using base_type = BaseWindow;

      struct FontRecord
      {
        SimpleSpriteFontMesh Mesh;
        DependencyPropertyUIColor PropertyColorChecked;
        DependencyPropertyUIColor PropertyColorUnchecked;
        DependencyPropertyUIColor PropertyColorDisabled;
        TransitionUIRenderColor CurrentColor;

        explicit FontRecord(const std::shared_ptr<IMeshManager>& meshManager, const std::shared_ptr<SpriteFont>& font, const TimeSpan& time,
                            const TransitionType type, const UIColorConverter converter)
          : Mesh(meshManager)
          , PropertyColorChecked(converter, DefaultColor::Palette::Font)
          , PropertyColorUnchecked(converter, DefaultColor::Palette::Font)
          , PropertyColorDisabled(converter, DefaultColor::Palette::FontDisabled)
          , CurrentColor(time, type)
        {
          Mesh.SetSprite(font);
        }
      };

      struct GraphicsRecord
      {
        SizedSpriteMesh Mesh;
        DependencyPropertyUIColor PropertyColorChecked;
        DependencyPropertyUIColor PropertyColorUnchecked;
        DependencyPropertyUIColor PropertyColorCheckedDisabled;
        DependencyPropertyUIColor PropertyColorUncheckedDisabled;
        TransitionUIRenderColor CurrentColor;

        explicit GraphicsRecord(const std::shared_ptr<IMeshManager>& meshManager, const TimeSpan time, const TransitionType type,
                                const UIColorConverter converter)
          : Mesh(meshManager)
          , PropertyColorChecked(converter, DefaultColor::ToggleButton::CursorChecked)
          , PropertyColorUnchecked(converter, DefaultColor::ToggleButton::CursorChecked)
          , PropertyColorCheckedDisabled(converter, DefaultColor::ToggleButton::CursorCheckedDisabled)
          , PropertyColorUncheckedDisabled(converter, DefaultColor::ToggleButton::CursorUncheckedDisabled)
          , CurrentColor(time, type)
        {
        }
      };

      struct HoverStateRecord
      {
        DependencyPropertyUIColor PrimaryColor;
        DpPoint2 PositionDp;

        HoverStateRecord() = delete;
        explicit HoverStateRecord(const UIColorConverter converter, const UIColor primaryColor)
          : PrimaryColor(converter, primaryColor)
        {
        }
      };

      struct HoverOverlayRecord
      {
        bool IsHovering{false};
        bool IsConstrainToGraphics{false};
        SizedSpriteMesh Mesh;
        HoverStateRecord Checked;
        HoverStateRecord Unchecked;
        TransitionUIRenderColor CurrentColor;
        TransitionVector2 CurrentPositionDp;
        PxPoint2 LastScreenPositionPx;

        HoverOverlayRecord(const std::shared_ptr<IMeshManager>& meshManager, const TimeSpan timeColor, const TransitionType typeColor,
                           const TimeSpan timePosition, const TransitionType typePosition, const UIColorConverter converter)
          : Mesh(meshManager)
          , Checked(converter, DefaultColor::ToggleButton::HoverOverlayChecked)
          , Unchecked(converter, DefaultColor::ToggleButton::HoverOverlayUnchecked)
          , CurrentColor(timeColor, typeColor)
          , CurrentPositionDp(timePosition, typePosition)
        {
        }
      };

    protected:
      // NOLINTNEXTLINE(readability-identifier-naming)
      const std::shared_ptr<WindowContext> m_windowContext;

    private:
      FontRecord m_font;
      GraphicsRecord m_cursor;
      GraphicsRecord m_background;
      HoverOverlayRecord m_hoverOverlay;
      SpriteFontMeasureInfo m_cachedFontMeasureInfo;

      DataBinding::TypedDependencyProperty<ItemAlignment> m_propertyImageAlignment{ItemAlignment::Near};
      DataBinding::TypedDependencyProperty<bool> m_propertyIsChecked{false};
      DataBinding::TypedDependencyProperty<bool> m_propertyIsEnabled{true};
      DataBinding::TypedDependencyProperty<StringViewLite> m_propertyText;

    public:
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyFontColorChecked;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyFontColorUnchecked;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyFontColorDisabled;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyCursorColorChecked;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyCursorColorUnchecked;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyCursorColorCheckedDisabled;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyCursorColorUncheckedDisabled;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorChecked;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorUnchecked;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorCheckedDisabled;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorUncheckedDisabled;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyHoverOverlayCheckedColor;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyHoverOverlayUncheckedColor;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyImageAlignment;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyIsChecked;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyIsEnabled;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyText;

      explicit ToggleButton(const std::shared_ptr<WindowContext>& context);

      bool IsEnabled() const noexcept
      {
        return m_propertyIsEnabled.Get();
      }

      //! @brief When disabled the slider can not be modified by the user
      bool SetEnabled(const bool value);

      const std::shared_ptr<SpriteFont>& GetFont() const
      {
        return m_font.Mesh.GetSprite();
      }

      void SetFont(const std::shared_ptr<SpriteFont>& value);

      StringViewLite GetText() const noexcept
      {
        return m_propertyText.Get();
      }

      bool SetText(const StringViewLite strView);
      bool SetText(std::string&& value);
      bool SetText(const char* const psz)
      {
        return SetText(StringViewLite(psz));
      }
      bool SetText(const std::string& str)
      {
        return SetText(StringViewLite(str));
      }

      ItemAlignment GetImageAlignment() const noexcept
      {
        return m_propertyImageAlignment.Get();
      }

      bool SetImageAlignment(const ItemAlignment value);


      UIColor GetHoverOverlayCheckedColor() const noexcept
      {
        return m_hoverOverlay.Checked.PrimaryColor.Get();
      }

      bool SetHoverOverlayCheckedColor(const UIColor value);

      UIColor GetHoverOverlayUncheckedColor() const noexcept
      {
        return m_hoverOverlay.Unchecked.PrimaryColor.Get();
      }

      bool SetHoverOverlayUncheckedColor(const UIColor value);

      DpPoint2 GetCursorCheckedPosition() const
      {
        return m_hoverOverlay.Checked.PositionDp;
      }

      void SetCursorCheckedPosition(const DpPoint2& valueDp);

      DpPoint2 GetCursorUncheckedPosition() const
      {
        return m_hoverOverlay.Unchecked.PositionDp;
      }

      void SetCursorUncheckedPosition(const DpPoint2& valueDp);

      const std::shared_ptr<ISizedSprite>& SetHoverOverlaySprite() const
      {
        return m_hoverOverlay.Mesh.GetSprite();
      }

      void SetHoverOverlaySprite(const std::shared_ptr<ISizedSprite>& value);


      UIColor GetCursorColorChecked() const noexcept
      {
        return m_cursor.PropertyColorChecked.Get();
      }
      bool SetCursorColorChecked(const UIColor value);


      UIColor GetCursorColorCheckedDisabled() const noexcept
      {
        return m_cursor.PropertyColorCheckedDisabled.Get();
      }
      bool SetCursorColorCheckedDisabled(const UIColor value);


      UIColor GetCursorColorUnchecked() const noexcept
      {
        return m_cursor.PropertyColorUnchecked.Get();
      }
      bool SetCursorColorUnchecked(const UIColor value);


      UIColor GetCursorColorUncheckedDisabled() const noexcept
      {
        return m_cursor.PropertyColorUncheckedDisabled.Get();
      }
      bool SetCursorColorUncheckedDisabled(const UIColor value);


      UIColor GetBackgroundColorChecked() const noexcept
      {
        return m_background.PropertyColorChecked.Get();
      }
      bool SetBackgroundColorChecked(const UIColor value);


      UIColor GetBackgroundColorCheckedDisabled() const noexcept
      {
        return m_background.PropertyColorCheckedDisabled.Get();
      }
      bool SetBackgroundColorCheckedDisabled(const UIColor value);


      UIColor GetBackgroundColorUnchecked() const noexcept
      {
        return m_background.PropertyColorUnchecked.Get();
      }
      bool SetBackgroundColorUnchecked(const UIColor value);


      UIColor GetBackgroundColorUncheckedDisabled() const noexcept
      {
        return m_background.PropertyColorUncheckedDisabled.Get();
      }
      bool SetBackgroundColorUncheckedDisabled(const UIColor value);


      UIColor GetFontColorChecked() const noexcept
      {
        return m_font.PropertyColorChecked.Get();
      }
      bool SetFontColorChecked(const UIColor value);


      UIColor GetFontColorUnchecked() const noexcept
      {
        return m_font.PropertyColorUnchecked.Get();
      }
      bool SetFontColorUnchecked(const UIColor value);


      UIColor GetFontColorDisabled() const noexcept
      {
        return m_font.PropertyColorDisabled.Get();
      }
      bool SetFontColorDisabled(const UIColor value);


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

      bool IsChecked() const noexcept
      {
        return m_propertyIsChecked.Get();
      }

      virtual bool SetIsChecked(const bool value);

      void Toggle();

      void WinDraw(const UIDrawContext& context) override;

    protected:
      void OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent) final;
      void OnMouseOver(const std::shared_ptr<WindowMouseOverEvent>& theEvent) final;

      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) final;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) final;

      DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override;
      DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                             const DataBinding::Binding& binding) override;
      void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) override;

      void UpdateAnimation(const TimeSpan& timeSpan) override;
      bool UpdateAnimationState(const bool forceCompleteAnimation) override;

    private:
      PxSize2D GetMaxSpriteSize() const;
    };
  }
}

#endif
