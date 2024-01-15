/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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
#include <FslGraphics/ColorSpaceConversion.hpp>
#include <FslSimpleUI/Theme/Basic/BasicThemeColors.hpp>
#include <cassert>

namespace Fsl::UI::Theme
{
  namespace
  {
    BasicThemeColors::PaletteRecord ConvertSRGBToLDRLinear(const BasicThemeColors::PaletteRecord& src)
    {
      return {ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.Primary),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.FontOnPrimary),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.ButtonOutlineFont),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.RadioButtonBackground),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.DividerOnPrimary),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.SwitchCursorChecked),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.SwitchCursorUnchecked),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.SwitchBackground),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BarColor),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.WindowColor),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.PrimaryDark),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.ImageOnPrimary)};
    }

    BasicThemeColors::BarRecord ConvertSRGBToLDRLinear(const BasicThemeColors::BarRecord& src)
    {
      return {ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.Normal), ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.Transparent)};
    }


    BasicThemeColors::DefaultFontRecord ConvertSRGBToLDRLinear(const BasicThemeColors::DefaultFontRecord& src)
    {
      return {ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.Primary),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.PrimaryDisabled)};
    }


    BasicThemeColors::ButtonRecord ConvertSRGBToLDRLinear(const BasicThemeColors::ButtonRecord& src)
    {
      return {
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundHoverUp),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundUp),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundDown),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundDisabled),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundHoverOverlayUp),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundHoverOverlayDown),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.FontUp),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.FontDown),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.FontDisabled),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.OutlineFontUp),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.OutlineFontDown),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.OutlineFontDisabled),
      };
    }

    BasicThemeColors::CheckBoxRecord ConvertSRGBToLDRLinear(const BasicThemeColors::CheckBoxRecord& src)
    {
      return {
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.HoverChecked),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.HoverUnchecked),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CursorChecked),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CursorCheckedDisabled),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CursorUnchecked),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CursorUncheckedDisabled),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundChecked),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundCheckedDisabled),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundUnchecked),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundUncheckedDisabled),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CheckedFont),
        ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.UncheckedFont),
      };
    }

    BasicThemeColors::DividerRecord ConvertSRGBToLDRLinear(const BasicThemeColors::DividerRecord& src)
    {
      return BasicThemeColors::DividerRecord(ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.Primary));
    }

    BasicThemeColors::ImageButtonRecord ConvertSRGBToLDRLinear(const BasicThemeColors::ImageButtonRecord& src)
    {
      return {ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundHoverUp),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundUp),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundDown),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundDisabled),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.ImageUp),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.ImageDown),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.ImageDisabled)};
    }

    BasicThemeColors::RadioButtonRecord ConvertSRGBToLDRLinear(const BasicThemeColors::RadioButtonRecord& src)
    {
      return {ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.HoverChecked),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.HoverUnchecked),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CursorChecked),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CursorCheckedDisabled),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CursorUnchecked),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CursorUncheckedDisabled),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundChecked),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundCheckedDisabled),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundUnchecked),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundUncheckedDisabled),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CheckedFont),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.UncheckedFont)};
    }

    BasicThemeColors::SwitchRecord ConvertSRGBToLDRLinear(const BasicThemeColors::SwitchRecord& src)
    {
      return {ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.HoverChecked),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.HoverUnchecked),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CursorChecked),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CursorCheckedDisabled),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CursorUnchecked),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CursorUncheckedDisabled),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundChecked),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundCheckedDisabled),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundUnchecked),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundUncheckedDisabled),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CheckedFont),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.UncheckedFont)};
    }

    BasicThemeColors::SliderRecord ConvertSRGBToLDRLinear(const BasicThemeColors::SliderRecord& src)
    {
      return {ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.Background),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundDisabled),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.Cursor),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CursorDisabled),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.CursorOverlay)};
    }

    BasicThemeColors::WindowRecord ConvertSRGBToLDRLinear(const BasicThemeColors::WindowRecord& src)
    {
      return {ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.Background),
              ColorSpaceConversion::LossyConvertSRGBToLinearColor(src.BackgroundTransparent)};
    }

    BasicThemeColors ConvertSRGBToLDRLinear(const BasicThemeColors& colors)
    {
      assert(colors.ActiveColorSpace == ColorSpace::SRGBNonLinear);
      return BasicThemeColors(ColorSpace::SRGBLinear, colors.Opacity, ConvertSRGBToLDRLinear(colors.Palette), ConvertSRGBToLDRLinear(colors.Bar),
                              ConvertSRGBToLDRLinear(colors.DefaultFont), ConvertSRGBToLDRLinear(colors.Button),
                              ConvertSRGBToLDRLinear(colors.CheckBox), ConvertSRGBToLDRLinear(colors.Divider),
                              ConvertSRGBToLDRLinear(colors.ImageButton), ConvertSRGBToLDRLinear(colors.RadioButton),
                              ConvertSRGBToLDRLinear(colors.Switch), ConvertSRGBToLDRLinear(colors.Slider), ConvertSRGBToLDRLinear(colors.Window));
    }
  }


  BasicThemeColors BasicThemeColors::ApplyColorSpaceLossy(const BasicThemeColors& colors, const ColorSpace colorSpace)
  {
    if (colors.ActiveColorSpace != ColorSpace::SRGBNonLinear)
    {
      throw UsageErrorException("Source colorSpace must be SRGBNonLinear");
    }
    switch (colorSpace)
    {
    case ColorSpace::SRGBNonLinear:
      return colors;
    case ColorSpace::SRGBLinear:
    case ColorSpace::SCRGBLinear:
      return ConvertSRGBToLDRLinear(colors);
    default:
      throw NotSupportedException("ColorSpace not supported");
    }
  }
}
