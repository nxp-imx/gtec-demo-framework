#ifndef FSLSIMPLEUI_BASE_DEFAULTVALUES_HPP
#define FSLSIMPLEUI_BASE_DEFAULTVALUES_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022, 2024 NXP
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

#include <FslSimpleUI/Base/UIColor.hpp>
#include <FslSimpleUI/Base/UIColors.hpp>

namespace Fsl::UI::DefaultColor
{
  namespace Palette
  {
    constexpr const UIColor Primary(UIColors::White());
    constexpr const UIColor PrimaryDisabled(PackedColor32(0xFFB0B0B0));
    constexpr const UIColor Font(UIColors::White());
    constexpr const UIColor FontDisabled(PrimaryDisabled);
  }

  namespace Button
  {
    constexpr const UIColor Up(Palette::Primary);
    constexpr const UIColor Down(PackedColor32(0xB0B0B0B0));
    constexpr const UIColor BackgroundHoverUp(Palette::Primary);
    constexpr const UIColor BackgroundHoverOverlayUp = UIColor::SetA(Palette::Primary, 0.12f);
    constexpr const UIColor BackgroundHoverOverlayDown = UIColor::SetA(Palette::Primary, 0.08f);
    constexpr const UIColor BackgroundUp = Palette::Primary;
    constexpr const UIColor BackgroundDown = Down;
    constexpr const UIColor BackgroundDisabled = Palette::PrimaryDisabled;
    constexpr const UIColor FontUp = Palette::Font;
    constexpr const UIColor FontDown = Down;
    constexpr const UIColor FontDisabled = Palette::FontDisabled;
  }

  namespace ToggleButton
  {
    constexpr const UIColor CursorChecked = Palette::Primary;
    constexpr const UIColor CursorCheckedDisabled = UIColor::ClearA(CursorChecked);
    constexpr const UIColor CursorUnchecked = Palette::Primary;
    constexpr const UIColor CursorUncheckedDisabled = UIColor::ClearA(CursorUnchecked);
    constexpr const UIColor BackgroundEnabled = Palette::Primary;
    constexpr const UIColor BackgroundDisabled = Palette::PrimaryDisabled;
    constexpr const UIColor HoverOverlayChecked = UIColor::SetA(CursorChecked, 0.4f);
    constexpr const UIColor HoverOverlayUnchecked = UIColor::SetA(CursorUnchecked, 0.4f);
  }


  namespace CheckBox
  {
    constexpr const UIColor CursorChecked = Palette::Primary;
    constexpr const UIColor CursorCheckedDisabled = UIColor::SetA(CursorChecked, 0.4f);
    constexpr const UIColor CursorUnchecked = UIColor::ClearA(CursorChecked);
    constexpr const UIColor CursorUncheckedDisabled = UIColor::ClearA(CursorCheckedDisabled);
    constexpr const UIColor BackgroundChecked = Palette::Primary;
    constexpr const UIColor BackgroundCheckedDisabled = Palette::PrimaryDisabled;
    constexpr const UIColor BackgroundUnchecked = Palette::Primary;
    constexpr const UIColor BackgroundUncheckedDisabled = Palette::PrimaryDisabled;
    constexpr const UIColor HoverOverlayChecked = UIColor::SetA(CursorChecked, 0.4f);
    constexpr const UIColor HoverOverlayUnchecked = UIColor::SetA(BackgroundUnchecked, 0.4f);
  }

  namespace Slider
  {
    constexpr const UIColor HoverOverlay = UIColor::SetA(Palette::Primary, 0.4f);
  }

  namespace Switch
  {
    constexpr const UIColor CursorChecked = Palette::Primary;
    constexpr const UIColor CursorCheckedDisabled = Palette::PrimaryDisabled;
    constexpr const UIColor CursorUnchecked = Palette::Primary;
    constexpr const UIColor CursorUncheckedDisabled = Palette::PrimaryDisabled;
    constexpr const UIColor BackgroundChecked = Palette::Primary;
    constexpr const UIColor BackgroundCheckedDisabled = Palette::PrimaryDisabled;
    constexpr const UIColor BackgroundUnchecked = Palette::Primary;
    constexpr const UIColor BackgroundUncheckedDisabled = Palette::PrimaryDisabled;
    constexpr const UIColor HoverOverlayChecked = UIColor::SetA(CursorChecked, 0.4f);
    constexpr const UIColor HoverOverlayUnchecked = UIColor::SetA(BackgroundUnchecked, 0.4f);
  }
}

#endif
