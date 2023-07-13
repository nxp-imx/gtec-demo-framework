#ifndef FSLSIMPLEUI_BASE_CONTROL_TOGGLEBUTTON_HPP
#define FSLSIMPLEUI_BASE_CONTROL_TOGGLEBUTTON_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2023 NXP
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
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslBase/Transition/TransitionVector2.hpp>
#include <FslGraphics/Transition/TransitionColor.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/Mesh/SimpleSpriteFontMesh.hpp>
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
      using base_type = BaseWindow;

      struct FontRecord
      {
        SimpleSpriteFontMesh Mesh;
        DataBinding::TypedDependencyProperty<Color> PropertyColorChecked{DefaultColor::Palette::Font};
        DataBinding::TypedDependencyProperty<Color> PropertyColorUnchecked{DefaultColor::Palette::Font};
        DataBinding::TypedDependencyProperty<Color> PropertyColorDisabled{DefaultColor::Palette::FontDisabled};
        TransitionColor CurrentColor;

        explicit FontRecord(const std::shared_ptr<IMeshManager>& meshManager, const std::shared_ptr<SpriteFont>& font,
                            TransitionCache& rTransitionCache, const TimeSpan& time, const TransitionType type)
          : Mesh(meshManager)
          , CurrentColor(rTransitionCache, time, type)
        {
          Mesh.SetSprite(font);
        }
      };

      struct GraphicsRecord
      {
        SizedSpriteMesh Mesh;
        DataBinding::TypedDependencyProperty<Color> PropertyColorChecked{DefaultColor::ToggleButton::CursorChecked};
        DataBinding::TypedDependencyProperty<Color> PropertyColorUnchecked{DefaultColor::ToggleButton::CursorChecked};
        DataBinding::TypedDependencyProperty<Color> PropertyColorCheckedDisabled{DefaultColor::ToggleButton::CursorCheckedDisabled};
        DataBinding::TypedDependencyProperty<Color> PropertyColorUncheckedDisabled{DefaultColor::ToggleButton::CursorUncheckedDisabled};
        TransitionColor CurrentColor;

        explicit GraphicsRecord(const std::shared_ptr<IMeshManager>& meshManager, TransitionCache& rTransitionCache, const TimeSpan& time,
                                const TransitionType type)
          : Mesh(meshManager)
          , CurrentColor(rTransitionCache, time, type)
        {
        }
      };

      struct HoverStateRecord
      {
        Color PrimaryColor;
        DpPoint2 PositionDp;

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

        HoverOverlayRecord(const std::shared_ptr<IMeshManager>& meshManager, TransitionCache& rTransitionCache, const TimeSpan& timeColor,
                           const TransitionType typeColor, const TimeSpan& timePosition, const TransitionType typePosition)
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

      DataBinding::TypedDependencyProperty<ItemAlignment> m_propertyImageAlignment{ItemAlignment::Near};
      DataBinding::TypedDependencyProperty<bool> m_propertyIsChecked{false};
      DataBinding::TypedDependencyProperty<bool> m_propertyIsEnabled{true};
      DataBinding::TypedDependencyProperty<StringViewLite> m_propertyText;

    public:
      static DataBinding::DependencyPropertyDefinition PropertyFontColorChecked;
      static DataBinding::DependencyPropertyDefinition PropertyFontColorUnchecked;
      static DataBinding::DependencyPropertyDefinition PropertyFontColorDisabled;
      static DataBinding::DependencyPropertyDefinition PropertyCursorColorChecked;
      static DataBinding::DependencyPropertyDefinition PropertyCursorColorUnchecked;
      static DataBinding::DependencyPropertyDefinition PropertyCursorColorCheckedDisabled;
      static DataBinding::DependencyPropertyDefinition PropertyCursorColorUncheckedDisabled;
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorChecked;
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorUnchecked;
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorCheckedDisabled;
      static DataBinding::DependencyPropertyDefinition PropertyBackgroundColorUncheckedDisabled;
      static DataBinding::DependencyPropertyDefinition PropertyImageAlignment;
      static DataBinding::DependencyPropertyDefinition PropertyIsChecked;
      static DataBinding::DependencyPropertyDefinition PropertyIsEnabled;
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
        return SetText(StringViewLiteUtil::AsStringViewLite(str));
      }

      ItemAlignment GetImageAlignment() const noexcept
      {
        return m_propertyImageAlignment.Get();
      }

      bool SetImageAlignment(const ItemAlignment value);


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


      Color GetCursorColorChecked() const noexcept
      {
        return m_cursor.PropertyColorChecked.Get();
      }
      bool SetCursorColorChecked(const Color value);


      Color GetCursorColorCheckedDisabled() const noexcept
      {
        return m_cursor.PropertyColorCheckedDisabled.Get();
      }
      bool SetCursorColorCheckedDisabled(const Color value);


      Color GetCursorColorUnchecked() const noexcept
      {
        return m_cursor.PropertyColorUnchecked.Get();
      }
      bool SetCursorColorUnchecked(const Color value);


      Color GetCursorColorUncheckedDisabled() const noexcept
      {
        return m_cursor.PropertyColorUncheckedDisabled.Get();
      }
      bool SetCursorColorUncheckedDisabled(const Color value);


      Color GetBackgroundColorChecked() const noexcept
      {
        return m_background.PropertyColorChecked.Get();
      }
      bool SetBackgroundColorChecked(const Color value);


      Color GetBackgroundColorCheckedDisabled() const noexcept
      {
        return m_background.PropertyColorCheckedDisabled.Get();
      }
      bool SetBackgroundColorCheckedDisabled(const Color value);


      Color GetBackgroundColorUnchecked() const noexcept
      {
        return m_background.PropertyColorUnchecked.Get();
      }
      bool SetBackgroundColorUnchecked(const Color value);


      Color GetBackgroundColorUncheckedDisabled() const noexcept
      {
        return m_background.PropertyColorUncheckedDisabled.Get();
      }
      bool SetBackgroundColorUncheckedDisabled(const Color value);


      Color GetFontColorChecked() const noexcept
      {
        return m_font.PropertyColorChecked.Get();
      }
      bool SetFontColorChecked(const Color value);


      Color GetFontColorUnchecked() const noexcept
      {
        return m_font.PropertyColorUnchecked.Get();
      }
      bool SetFontColorUnchecked(const Color value);


      Color GetFontColorDisabled() const noexcept
      {
        return m_font.PropertyColorDisabled.Get();
      }
      bool SetFontColorDisabled(const Color value);


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
      void OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent) final;
      void OnMouseOver(const RoutedEventArgs& args, const std::shared_ptr<WindowMouseOverEvent>& theEvent) final;

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
