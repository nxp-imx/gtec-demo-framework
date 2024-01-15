#ifndef FSLSIMPLEUI_THEME_BASIC_BASICTHEMECOLORS_HPP
#define FSLSIMPLEUI_THEME_BASIC_BASICTHEMECOLORS_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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
#include <FslGraphics/Color.hpp>
#include <FslGraphics/ColorSpace.hpp>
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
      const Color Primary{0xFF1976d2};
      const Color FontOnPrimary{0xFFFFFFFF};
      const Color ButtonOutlineFont{0xFF2196f3};
      const Color RadioButtonBackground{0xFF6a6a6a};

      const Color DividerOnPrimary{0xFFFFFFFF};

      const Color SwitchCursorChecked{Primary};
      const Color SwitchCursorUnchecked{0xFFfafafa};
      const Color SwitchBackground{0xFF9a9999};

      const Color BarColor{0xFF303030};
      const Color WindowColor{0xFF303030};

      const Color PrimaryDark{0xFF004BA0};
      // const Color FontOnPrimaryDark{0xFFFFFFFF};

      const Color ImageOnPrimary{Color::White()};

      constexpr explicit PaletteRecord(const OpacityRecord& opacity)
      {
        FSL_PARAM_NOT_USED(opacity);
      }

      constexpr explicit PaletteRecord(const OpacityRecord& opacity, const bool usePrimary)
        : Primary(usePrimary ? 0xFF1976d2 : 0xFFce28b2)
        , FontOnPrimary(0xFFFFFFFF)
        , ButtonOutlineFont(usePrimary ? 0xFF2196f3 : 0xFFff81e9)
        , RadioButtonBackground(0xFF6a6a6a)
        , SwitchCursorChecked(Primary)
        , SwitchCursorUnchecked(0xFFfafafa)
        , SwitchBackground(0xFF9a9999)
        , BarColor(usePrimary ? 0xFF303030 : 0xFF302030)
        , WindowColor(usePrimary ? 0xFF303030 : 0xFF302030)
        , PrimaryDark(usePrimary ? 0xFF004BA0 : 0xFF990082)
        , ImageOnPrimary(Color::White())
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
      const Color Normal;
      const Color Transparent;

      constexpr BarRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : Normal(palette.BarColor)
        , Transparent(Color::ApplyAlpha(palette.BarColor, 0.85f))
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
      const Color Primary;
      const Color PrimaryDisabled;

      constexpr DefaultFontRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : Primary(palette.FontOnPrimary)
        , PrimaryDisabled(Color::MultiplyRGB(palette.FontOnPrimary, opacity.DisabledDarken))
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
      const Color BackgroundHoverUp;
      const Color BackgroundUp;
      const Color BackgroundDown;
      const Color BackgroundDisabled;
      const Color BackgroundHoverOverlayUp;
      const Color BackgroundHoverOverlayDown;

      const Color FontUp;
      const Color FontDown;
      const Color FontDisabled;

      const Color OutlineFontUp;
      const Color OutlineFontDown;
      const Color OutlineFontDisabled;

      constexpr ButtonRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : BackgroundHoverUp(Color::MultiplyRGB(palette.Primary, 1.05f))
        , BackgroundUp(palette.Primary)
        , BackgroundDown(Color::MultiplyRGB(palette.Primary, opacity.ButtonDownDarken))
        , BackgroundDisabled(Color::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , BackgroundHoverOverlayUp(Color::ApplyAlpha(palette.Primary, 0.10f))
        , BackgroundHoverOverlayDown(Color::ApplyAlpha(palette.Primary, 0.08f))
        , FontUp(palette.FontOnPrimary)
        , FontDown(Color::MultiplyRGB(palette.FontOnPrimary, opacity.ButtonDownDarken))
        , FontDisabled(Color::MultiplyRGB(palette.FontOnPrimary, opacity.DisabledDarken))
        , OutlineFontUp(palette.ButtonOutlineFont)
        , OutlineFontDown(Color::MultiplyRGB(palette.ButtonOutlineFont, opacity.ButtonDownDarken))
        , OutlineFontDisabled(Color::MultiplyRGB(palette.ButtonOutlineFont, opacity.DisabledDarken))
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
      const Color HoverChecked;
      const Color HoverUnchecked;
      const Color CursorChecked;
      const Color CursorCheckedDisabled;
      const Color CursorUnchecked;
      const Color CursorUncheckedDisabled;
      const Color BackgroundChecked;
      const Color BackgroundCheckedDisabled;
      const Color BackgroundUnchecked;
      const Color BackgroundUncheckedDisabled;
      const Color CheckedFont;
      const Color UncheckedFont;

      constexpr CheckBoxRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : HoverChecked(Color::ApplyAlpha(palette.Primary, opacity.HoverAlpha))
        , HoverUnchecked(Color::ApplyAlpha(palette.RadioButtonBackground, opacity.HoverAlpha))
        , CursorChecked(palette.Primary)
        , CursorCheckedDisabled(Color::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , CursorUnchecked(Color::Transparent())
        , CursorUncheckedDisabled(Color::Transparent())
        , BackgroundChecked(palette.Primary)
        , BackgroundCheckedDisabled(Color::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , BackgroundUnchecked(palette.RadioButtonBackground)
        , BackgroundUncheckedDisabled(Color::MultiplyRGB(palette.RadioButtonBackground, opacity.DisabledDarken))
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
      const Color Primary;
      // const Color PrimaryDisabled;

      constexpr DividerRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : Primary(Color::SetA(palette.DividerOnPrimary, 0.12f))
      //, PrimaryDisabled(Color::MultiplyRGB(Primary, opacity.DisabledDarken))
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
      const Color BackgroundHoverUp;
      const Color BackgroundUp;
      const Color BackgroundDown;
      const Color BackgroundDisabled;

      const Color ImageUp;
      const Color ImageDown;
      const Color ImageDisabled;

      constexpr ImageButtonRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : BackgroundHoverUp(Color::MultiplyRGB(palette.Primary, 1.10f))
        , BackgroundUp(palette.Primary)
        , BackgroundDown(Color::MultiplyRGB(palette.Primary, opacity.ButtonDownDarken))
        , BackgroundDisabled(Color::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , ImageUp(palette.ImageOnPrimary)
        , ImageDown(Color::MultiplyRGB(palette.ImageOnPrimary, opacity.ButtonDownDarken))
        , ImageDisabled(Color::MultiplyRGB(palette.ImageOnPrimary, opacity.DisabledDarken))
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
      const Color HoverChecked;
      const Color HoverUnchecked;
      const Color CursorChecked;
      const Color CursorCheckedDisabled;
      const Color CursorUnchecked;
      const Color CursorUncheckedDisabled;
      const Color BackgroundChecked;
      const Color BackgroundCheckedDisabled;
      const Color BackgroundUnchecked;
      const Color BackgroundUncheckedDisabled;
      const Color CheckedFont;
      const Color UncheckedFont;

      constexpr RadioButtonRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : HoverChecked(Color::ApplyAlpha(palette.Primary, opacity.HoverAlpha))
        , HoverUnchecked(Color::ApplyAlpha(palette.RadioButtonBackground, opacity.HoverAlpha))
        , CursorChecked(palette.Primary)
        , CursorCheckedDisabled(Color::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , CursorUnchecked(Color::Transparent())
        , CursorUncheckedDisabled(Color::Transparent())
        , BackgroundChecked(palette.Primary)
        , BackgroundCheckedDisabled(Color::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , BackgroundUnchecked(palette.RadioButtonBackground)
        , BackgroundUncheckedDisabled(Color::MultiplyRGB(palette.RadioButtonBackground, opacity.DisabledDarken))
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
      const Color HoverChecked;
      const Color HoverUnchecked;
      const Color CursorChecked;
      const Color CursorCheckedDisabled;
      const Color CursorUnchecked;
      const Color CursorUncheckedDisabled;
      const Color BackgroundChecked;
      const Color BackgroundCheckedDisabled;
      const Color BackgroundUnchecked;
      const Color BackgroundUncheckedDisabled;

      const Color CheckedFont;
      const Color UncheckedFont;

      constexpr SwitchRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : HoverChecked(Color::ApplyAlpha(palette.SwitchCursorChecked, opacity.HoverAlpha))
        , HoverUnchecked(Color::ApplyAlpha(palette.SwitchCursorUnchecked, opacity.HoverUnselectedAlpha))
        , CursorChecked(palette.SwitchCursorChecked)
        , CursorCheckedDisabled(Color::MultiplyRGB(palette.SwitchCursorChecked, opacity.DisabledDarken))
        , CursorUnchecked(palette.SwitchCursorUnchecked)
        , CursorUncheckedDisabled(Color::MultiplyRGB(palette.SwitchCursorUnchecked, opacity.DisabledDarken))
        , BackgroundChecked(palette.PrimaryDark)
        , BackgroundCheckedDisabled(Color::MultiplyRGB(palette.PrimaryDark, opacity.DisabledDarken))
        , BackgroundUnchecked(palette.SwitchBackground)
        , BackgroundUncheckedDisabled(Color::MultiplyRGB(palette.SwitchBackground, opacity.DisabledDarken))
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
      const Color Background;
      const Color BackgroundDisabled;
      const Color Cursor;
      const Color CursorDisabled;
      const Color CursorOverlay;

      constexpr SliderRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : Background(palette.Primary)
        , BackgroundDisabled(Color::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , Cursor(palette.Primary)
        , CursorDisabled(Color::MultiplyRGB(palette.Primary, opacity.DisabledDarken))
        , CursorOverlay(Color::ApplyAlpha(palette.Primary, opacity.SliderHoverAlpha))
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
      const Color Background;
      const Color BackgroundTransparent;

      constexpr WindowRecord(const OpacityRecord& opacity, const PaletteRecord& palette)
        : Background(palette.WindowColor)
        , BackgroundTransparent(Color::ApplyAlpha(palette.WindowColor, 0.85f))
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

    constexpr Color GetBarColor(const BarType type) const
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

    constexpr Color GetWindowColor(const WindowType type) const
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
