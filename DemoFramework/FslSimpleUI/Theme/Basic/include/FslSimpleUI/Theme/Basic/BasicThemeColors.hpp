#ifndef FSLSIMPLEUI_THEME_BASIC_BASICTHEMECOLORS_HPP
#define FSLSIMPLEUI_THEME_BASIC_BASICTHEMECOLORS_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022, 2024 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslGraphics/ColorSpace.hpp>
#include <FslSimpleUI/Base/UIColors.hpp>
#include <FslSimpleUI/Theme/Base/BarType.hpp>
#include <FslSimpleUI/Theme/Base/WindowType.hpp>
#include <cassert>
#include <utility>

namespace Fsl::UI::Theme
{
  class BasicThemeColors
  {
  public:
    struct OpacityRecord
    {
      const float DisabledDarken{0.58f};
      const float SliderHoverAlpha{0.15f};
      const float HoverAlpha{0.125f};
      const float HoverUnselectedAlpha{0.04f};
      const float ButtonDownDarken{0.85f};
    };

    struct PaletteRecord
    {
      const UIColor Primary{PackedColor32(0xFF1976d2)};
      const UIColor FontOnPrimary{PackedColor32(0xFFFFFFFF)};
      const UIColor ButtonOutlineFont{PackedColor32(0xFF2196f3)};
      const UIColor RadioButtonBackground{PackedColor32(0xFF6a6a6a)};

      const UIColor DividerOnPrimary{PackedColor32(0xFFFFFFFF)};

      const UIColor SwitchCursorChecked{Primary};
      const UIColor SwitchCursorUnchecked{PackedColor32(0xFFFAFAFA)};
      const UIColor SwitchBackground{PackedColor32(0xFF9a9999)};

      const UIColor BarColor{PackedColor32(0xFF303030)};
      const UIColor WindowColor{PackedColor32(0xFF303030)};

      const UIColor PrimaryDark{PackedColor32(0xFF004BA0)};
      // const Color FontOnPrimaryDark{0xFFFFFFFF};

      const UIColor ImageOnPrimary{UIColors::White()};

      constexpr explicit PaletteRecord(const OpacityRecord& opacity)
      {
        FSL_PARAM_NOT_USED(opacity);
      }

      constexpr explicit PaletteRecord(const OpacityRecord& opacity, const bool usePrimary)
        : Primary(usePrimary ? PackedColor32(0xFF1976D2) : PackedColor32(0xFFCE28B2))
        , FontOnPrimary(PackedColor32(0xFFFFFFFF))
        , ButtonOutlineFont(PackedColor32(usePrimary ? 0xFF2196F3 : 0xFFFF81E9))
        , RadioButtonBackground(PackedColor32(0xFF6A6A6A))
        , SwitchCursorChecked(Primary)
        , SwitchCursorUnchecked(PackedColor32(0xFFFAFAFA))
        , SwitchBackground(PackedColor32(0xFF9A9999))
        , BarColor(PackedColor32(usePrimary ? 0xFF303030 : 0xFF302030))
        , WindowColor(PackedColor32(usePrimary ? 0xFF303030 : 0xFF302030))
        , PrimaryDark(PackedColor32(usePrimary ? 0xFF004BA0 : 0xFF990082))
        , ImageOnPrimary(UIColors::White())
      {
        FSL_PARAM_NOT_USED(opacity);
      }

      constexpr PaletteRecord(const Color primary, const Color fontOnPrimary, const Color buttonOutlineFont, const Color radioButtonBackground,
                              const Color dividerOnPrimary, const Color switchCursorChecked, const Color switchCursorUnchecked,
                              const Color switchBackground, const Color barColor, const Color windowColor, const Color primaryDark,
                              const Color imageOnPrimary)
        : Primary(primary)
        , FontOnPrimary(fontOnPrimary)
        , ButtonOutlineFont(buttonOutlineFont)
        , RadioButtonBackground(radioButtonBackground)
        , SwitchCursorChecked(switchCursorChecked)
        , SwitchCursorUnchecked(switchCursorUnchecked)
        , SwitchBackground(switchBackground)
        , BarColor(barColor)
        , WindowColor(windowColor)
        , PrimaryDark(primaryDark)
        , ImageOnPrimary(imageOnPrimary)
      {
      }
    };

    struct BarRecord
    {
      const UIColor Normal;
      const UIColor Transparent;

      constexpr BarRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : Normal(palette.BarColor)
        , Transparent(UIColor::MultiplyA(palette.BarColor, 0.85f))
      {
        FSL_PARAM_NOT_USED(opacity);
      }

      constexpr BarRecord(const Color normal, const Color transparent)
        : Normal(normal)
        , Transparent(transparent)
      {
      }
    };

    struct DefaultFontRecord
    {
      const UIColor Primary;
      const UIColor PrimaryDisabled;

      constexpr DefaultFontRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : Primary(palette.FontOnPrimary)
        , PrimaryDisabled(UIColor::MultiplyRGB(palette.FontOnPrimary, opacity.DisabledDarken))
      {
      }

      constexpr DefaultFontRecord(const Color primary, const Color primaryDisabled)
        : Primary(primary)
        , PrimaryDisabled(primaryDisabled)
      {
      }
    };

    struct ButtonRecord
    {
      const UIColor BackgroundHoverUp;
      const UIColor BackgroundUp;
      const UIColor BackgroundDown;
      const UIColor BackgroundDisabled;
      const UIColor BackgroundHoverOverlayUp;
      const UIColor BackgroundHoverOverlayDown;

      const UIColor FontUp;
      const UIColor FontDown;
      const UIColor FontDisabled;

      const UIColor OutlineFontUp;
      const UIColor OutlineFontDown;
      const UIColor OutlineFontDisabled;

      constexpr ButtonRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : BackgroundHoverUp(UIColor::MultiplyRGB(palette.Primary, 1.05f))
        , BackgroundUp(palette.Primary)
        , BackgroundDown(UIColor::MultiplyRGB(palette.Primary, opacity.ButtonDownDarken))
        , BackgroundDisabled(UIColor::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , BackgroundHoverOverlayUp(UIColor::MultiplyA(palette.Primary, 0.10f))
        , BackgroundHoverOverlayDown(UIColor::MultiplyA(palette.Primary, 0.08f))
        , FontUp(palette.FontOnPrimary)
        , FontDown(UIColor::MultiplyRGB(palette.FontOnPrimary, opacity.ButtonDownDarken))
        , FontDisabled(UIColor::MultiplyRGB(palette.FontOnPrimary, opacity.DisabledDarken))
        , OutlineFontUp(palette.ButtonOutlineFont)
        , OutlineFontDown(UIColor::MultiplyRGB(palette.ButtonOutlineFont, opacity.ButtonDownDarken))
        , OutlineFontDisabled(UIColor::MultiplyRGB(palette.ButtonOutlineFont, opacity.DisabledDarken))
      {
      }


      constexpr ButtonRecord(const Color backgroundHoverUp, const Color backgroundUp, const Color backgroundDown, const Color backgroundDisabled,
                             const Color backgroundHoverOverlayUp, const Color backgroundHoverOverlayDown, const Color fontUp, const Color fontDown,
                             const Color fontDisabled, const Color outlineFontUp, const Color outlineFontDown, const Color outlineFontDisabled)
        : BackgroundHoverUp(backgroundHoverUp)
        , BackgroundUp(backgroundUp)
        , BackgroundDown(backgroundDown)
        , BackgroundDisabled(backgroundDisabled)
        , BackgroundHoverOverlayUp(backgroundHoverOverlayUp)
        , BackgroundHoverOverlayDown(backgroundHoverOverlayDown)
        , FontUp(fontUp)
        , FontDown(fontDown)
        , FontDisabled(fontDisabled)
        , OutlineFontUp(outlineFontUp)
        , OutlineFontDown(outlineFontDown)
        , OutlineFontDisabled(outlineFontDisabled)
      {
      }
    };

    struct CheckBoxRecord
    {
      const UIColor HoverChecked;
      const UIColor HoverUnchecked;
      const UIColor CursorChecked;
      const UIColor CursorCheckedDisabled;
      const UIColor CursorUnchecked;
      const UIColor CursorUncheckedDisabled;
      const UIColor BackgroundChecked;
      const UIColor BackgroundCheckedDisabled;
      const UIColor BackgroundUnchecked;
      const UIColor BackgroundUncheckedDisabled;
      const UIColor CheckedFont;
      const UIColor UncheckedFont;

      constexpr CheckBoxRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : HoverChecked(UIColor::MultiplyA(palette.Primary, opacity.HoverAlpha))
        , HoverUnchecked(UIColor::MultiplyA(palette.RadioButtonBackground, opacity.HoverAlpha))
        , CursorChecked(palette.Primary)
        , CursorCheckedDisabled(UIColor::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , CursorUnchecked(UIColors::Transparent())
        , CursorUncheckedDisabled(UIColors::Transparent())
        , BackgroundChecked(palette.Primary)
        , BackgroundCheckedDisabled(UIColor::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , BackgroundUnchecked(palette.RadioButtonBackground)
        , BackgroundUncheckedDisabled(UIColor::MultiplyRGB(palette.RadioButtonBackground, opacity.DisabledDarken))
        , CheckedFont(palette.FontOnPrimary)
        , UncheckedFont(palette.FontOnPrimary)
      {
      }

      constexpr CheckBoxRecord(const Color hoverChecked, const Color hoverUnchecked, const Color cursorChecked, const Color cursorCheckedDisabled,
                               const Color cursorUnchecked, const Color cursorUncheckedDisabled, const Color backgroundChecked,
                               const Color backgroundCheckedDisabled, const Color backgroundUnchecked, const Color backgroundUncheckedDisabled,
                               const Color checkedFont, const Color uncheckedFont)
        : HoverChecked(hoverChecked)
        , HoverUnchecked(hoverUnchecked)
        , CursorChecked(cursorChecked)
        , CursorCheckedDisabled(cursorCheckedDisabled)
        , CursorUnchecked(cursorUnchecked)
        , CursorUncheckedDisabled(cursorUncheckedDisabled)
        , BackgroundChecked(backgroundChecked)
        , BackgroundCheckedDisabled(backgroundCheckedDisabled)
        , BackgroundUnchecked(backgroundUnchecked)
        , BackgroundUncheckedDisabled(backgroundUncheckedDisabled)
        , CheckedFont(checkedFont)
        , UncheckedFont(uncheckedFont)
      {
      }
    };

    struct DividerRecord
    {
      const UIColor Primary;
      // const Color PrimaryDisabled;

      constexpr DividerRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : Primary(UIColor::SetA(palette.DividerOnPrimary, 0.12f))
      //, PrimaryDisabled(UIColor::MultiplyRGB(Primary, opacity.DisabledDarken))
      {
        FSL_PARAM_NOT_USED(opacity);
      }

      constexpr explicit DividerRecord(const Color primary)
        : Primary(primary)
      {
      }
    };

    struct ImageButtonRecord
    {
      const UIColor BackgroundHoverUp;
      const UIColor BackgroundUp;
      const UIColor BackgroundDown;
      const UIColor BackgroundDisabled;

      const UIColor ImageUp;
      const UIColor ImageDown;
      const UIColor ImageDisabled;

      constexpr ImageButtonRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : BackgroundHoverUp(UIColor::MultiplyRGB(palette.Primary, 1.10f))
        , BackgroundUp(palette.Primary)
        , BackgroundDown(UIColor::MultiplyRGB(palette.Primary, opacity.ButtonDownDarken))
        , BackgroundDisabled(UIColor::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , ImageUp(palette.ImageOnPrimary)
        , ImageDown(UIColor::MultiplyRGB(palette.ImageOnPrimary, opacity.ButtonDownDarken))
        , ImageDisabled(UIColor::MultiplyRGB(palette.ImageOnPrimary, opacity.DisabledDarken))
      {
      }

      constexpr ImageButtonRecord(const Color backgroundHoverUp, const Color backgroundUp, const Color backgroundDown, const Color backgroundDisabled,
                                  const Color imageUp, const Color imageDown, const Color imageDisabled)
        : BackgroundHoverUp(backgroundHoverUp)
        , BackgroundUp(backgroundUp)
        , BackgroundDown(backgroundDown)
        , BackgroundDisabled(backgroundDisabled)
        , ImageUp(imageUp)
        , ImageDown(imageDown)
        , ImageDisabled(imageDisabled)
      {
      }
    };

    struct RadioButtonRecord
    {
      const UIColor HoverChecked;
      const UIColor HoverUnchecked;
      const UIColor CursorChecked;
      const UIColor CursorCheckedDisabled;
      const UIColor CursorUnchecked;
      const UIColor CursorUncheckedDisabled;
      const UIColor BackgroundChecked;
      const UIColor BackgroundCheckedDisabled;
      const UIColor BackgroundUnchecked;
      const UIColor BackgroundUncheckedDisabled;
      const UIColor CheckedFont;
      const UIColor UncheckedFont;

      constexpr RadioButtonRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : HoverChecked(UIColor::MultiplyA(palette.Primary, opacity.HoverAlpha))
        , HoverUnchecked(UIColor::MultiplyA(palette.RadioButtonBackground, opacity.HoverAlpha))
        , CursorChecked(palette.Primary)
        , CursorCheckedDisabled(UIColor::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , CursorUnchecked(UIColors::Transparent())
        , CursorUncheckedDisabled(UIColors::Transparent())
        , BackgroundChecked(palette.Primary)
        , BackgroundCheckedDisabled(UIColor::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , BackgroundUnchecked(palette.RadioButtonBackground)
        , BackgroundUncheckedDisabled(UIColor::MultiplyRGB(palette.RadioButtonBackground, opacity.DisabledDarken))
        , CheckedFont(palette.FontOnPrimary)
        , UncheckedFont(palette.FontOnPrimary)
      {
      }

      constexpr RadioButtonRecord(const Color hoverChecked, const Color hoverUnchecked, const Color cursorChecked, const Color cursorCheckedDisabled,
                                  const Color cursorUnchecked, const Color cursorUncheckedDisabled, const Color backgroundChecked,
                                  const Color backgroundCheckedDisabled, const Color backgroundUnchecked, const Color backgroundUncheckedDisabled,
                                  const Color checkedFont, const Color uncheckedFont)
        : HoverChecked(hoverChecked)
        , HoverUnchecked(hoverUnchecked)
        , CursorChecked(cursorChecked)
        , CursorCheckedDisabled(cursorCheckedDisabled)
        , CursorUnchecked(cursorUnchecked)
        , CursorUncheckedDisabled(cursorUncheckedDisabled)
        , BackgroundChecked(backgroundChecked)
        , BackgroundCheckedDisabled(backgroundCheckedDisabled)
        , BackgroundUnchecked(backgroundUnchecked)
        , BackgroundUncheckedDisabled(backgroundUncheckedDisabled)
        , CheckedFont(checkedFont)
        , UncheckedFont(uncheckedFont)
      {
      }
    };

    struct SwitchRecord
    {
      const UIColor HoverChecked;
      const UIColor HoverUnchecked;
      const UIColor CursorChecked;
      const UIColor CursorCheckedDisabled;
      const UIColor CursorUnchecked;
      const UIColor CursorUncheckedDisabled;
      const UIColor BackgroundChecked;
      const UIColor BackgroundCheckedDisabled;
      const UIColor BackgroundUnchecked;
      const UIColor BackgroundUncheckedDisabled;

      const UIColor CheckedFont;
      const UIColor UncheckedFont;

      constexpr SwitchRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : HoverChecked(UIColor::MultiplyA(palette.SwitchCursorChecked, opacity.HoverAlpha))
        , HoverUnchecked(UIColor::MultiplyA(palette.SwitchCursorUnchecked, opacity.HoverUnselectedAlpha))
        , CursorChecked(palette.SwitchCursorChecked)
        , CursorCheckedDisabled(UIColor::MultiplyRGB(palette.SwitchCursorChecked, opacity.DisabledDarken))
        , CursorUnchecked(palette.SwitchCursorUnchecked)
        , CursorUncheckedDisabled(UIColor::MultiplyRGB(palette.SwitchCursorUnchecked, opacity.DisabledDarken))
        , BackgroundChecked(palette.PrimaryDark)
        , BackgroundCheckedDisabled(UIColor::MultiplyRGB(palette.PrimaryDark, opacity.DisabledDarken))
        , BackgroundUnchecked(palette.SwitchBackground)
        , BackgroundUncheckedDisabled(UIColor::MultiplyRGB(palette.SwitchBackground, opacity.DisabledDarken))
        , CheckedFont(palette.FontOnPrimary)
        , UncheckedFont(palette.FontOnPrimary)
      {
      }

      constexpr SwitchRecord(const Color hoverChecked, const Color hoverUnchecked, const Color cursorChecked, const Color cursorCheckedDisabled,
                             const Color cursorUnchecked, const Color cursorUncheckedDisabled, const Color backgroundChecked,
                             const Color backgroundCheckedDisabled, const Color backgroundUnchecked, const Color backgroundUncheckedDisabled,
                             const Color checkedFont, const Color uncheckedFont)
        : HoverChecked(hoverChecked)
        , HoverUnchecked(hoverUnchecked)
        , CursorChecked(cursorChecked)
        , CursorCheckedDisabled(cursorCheckedDisabled)
        , CursorUnchecked(cursorUnchecked)
        , CursorUncheckedDisabled(cursorUncheckedDisabled)
        , BackgroundChecked(backgroundChecked)
        , BackgroundCheckedDisabled(backgroundCheckedDisabled)
        , BackgroundUnchecked(backgroundUnchecked)
        , BackgroundUncheckedDisabled(backgroundUncheckedDisabled)
        , CheckedFont(checkedFont)
        , UncheckedFont(uncheckedFont)
      {
      }
    };

    struct SliderRecord
    {
      const UIColor Background;
      const UIColor BackgroundDisabled;
      const UIColor Cursor;
      const UIColor CursorDisabled;
      const UIColor CursorOverlay;

      constexpr SliderRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : Background(palette.Primary)
        , BackgroundDisabled(UIColor::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , Cursor(palette.Primary)
        , CursorDisabled(UIColor::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , CursorOverlay(UIColor::MultiplyA(palette.Primary, opacity.SliderHoverAlpha))
      {
      }

      constexpr SliderRecord(const Color background, const Color backgroundDisabled, const Color cursor, const Color cursorDisabled,
                             const Color cursorOverlay)
        : Background(background)
        , BackgroundDisabled(backgroundDisabled)
        , Cursor(cursor)
        , CursorDisabled(cursorDisabled)
        , CursorOverlay(cursorOverlay)
      {
      }
    };

    struct WindowRecord
    {
      const UIColor Background;
      const UIColor BackgroundTransparent;

      constexpr WindowRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : Background(palette.WindowColor)
        , BackgroundTransparent(UIColor::MultiplyA(palette.WindowColor, 0.85f))
      {
        FSL_PARAM_NOT_USED(opacity);
      }

      constexpr WindowRecord(const Color background, const Color backgroundTransparent)
        : Background(background)
        , BackgroundTransparent(backgroundTransparent)
      {
      }
    };

    const ColorSpace ActiveColorSpace;
    const OpacityRecord Opacity;
    const PaletteRecord Palette;
    const BarRecord Bar;
    const DefaultFontRecord DefaultFont;
    const ButtonRecord Button;
    const CheckBoxRecord CheckBox;
    const DividerRecord Divider;
    const ImageButtonRecord ImageButton;
    const RadioButtonRecord RadioButton;
    const SwitchRecord Switch;
    const SliderRecord Slider;
    const WindowRecord Window;

    constexpr explicit BasicThemeColors(const bool usePrimary)
      : ActiveColorSpace(ColorSpace::SRGBNonLinear)
      , Palette(Opacity, usePrimary)
      , Bar(Opacity, Palette)
      , DefaultFont(Opacity, Palette)
      , Button(Opacity, Palette)
      , CheckBox(Opacity, Palette)
      , Divider(Opacity, Palette)
      , ImageButton(Opacity, Palette)
      , RadioButton(Opacity, Palette)
      , Switch(Opacity, Palette)
      , Slider(Opacity, Palette)
      , Window(Opacity, Palette)
    {
    }

    constexpr explicit BasicThemeColors(const ColorSpace activeColorSpace, const OpacityRecord& opacity, PaletteRecord palette, BarRecord bar,
                                        DefaultFontRecord defaultFont, ButtonRecord button, CheckBoxRecord checkBox, DividerRecord divider,
                                        ImageButtonRecord imageButton, RadioButtonRecord radioButton, SwitchRecord switchRecord, SliderRecord slider,
                                        WindowRecord window)
      : ActiveColorSpace(activeColorSpace)
      , Opacity(opacity)
      , Palette(std::move(palette))
      , Bar(std::move(bar))
      , DefaultFont(std::move(defaultFont))
      , Button(std::move(button))
      , CheckBox(std::move(checkBox))
      , Divider(std::move(divider))
      , ImageButton(std::move(imageButton))
      , RadioButton(std::move(radioButton))
      , Switch(std::move(switchRecord))
      , Slider(std::move(slider))
      , Window(std::move(window))
    {
    }

    constexpr UIColor GetBarColor(const BarType type) const
    {
      switch (type)
      {
      case BarType::Transparent:
        return Bar.Transparent;
      case BarType::Normal:
      default:
        return Bar.Normal;
      }
    }

    constexpr UIColor GetWindowColor(const WindowType type) const
    {
      switch (type)
      {
      case WindowType::Transparent:
        return Window.BackgroundTransparent;
      case WindowType::Normal:
      default:
        return Window.Background;
      }
    }

    static BasicThemeColors ApplyColorSpaceLossy(const BasicThemeColors& colors, const ColorSpace colorSpace);
  };

  //  Color BasicThemeResources::GetThemePrimaryColor() const    // NOLINT(readability-convert-member-functions-to-static)
  //  {
  //    return ThemeColor::Palette::Primary;
  //  }

  //  Color BasicThemeResources::GetThemePrimaryDarkColor() const    // NOLINT(readability-convert-member-functions-to-static)
  //  {
  //    return ThemeColor::Palette::PrimaryDark;
  //  }

  //  Color BasicThemeResources::GetThemeSidebarColor() const    // NOLINT(readability-convert-member-functions-to-static)
  //  {
  //    return ThemeColor::Bar::Normal;
  //  }


  //  Color BasicThemeResources::GetThemeFontOnPrimaryColor() const
  //  {
  //    return ThemeColor::DefaultFont::Primary;
  //  }

  //  Color BasicThemeResources::GetThemeFontOnPrimaryDisabledColor() const
  //  {
  //    return ThemeColor::DefaultFont::PrimaryDisabled;
  //  }
}

#endif
