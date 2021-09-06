#ifndef FSLSIMPLEUI_BASE_DEFAULTVALUES_HPP
#define FSLSIMPLEUI_BASE_DEFAULTVALUES_HPP
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

namespace Fsl
{
  namespace UI
  {
    namespace DefaultColor
    {
      namespace Palette
      {
        constexpr const Color Primary = Color::White();
        constexpr const Color PrimaryDisabled = Color(0xFFB0B0B0);
        constexpr const Color Font = Color::White();
        constexpr const Color FontDisabled = PrimaryDisabled;
      }

      namespace Button
      {
        constexpr const Color Up = Palette::Primary;
        constexpr const Color Down = Color(0xB0B0B0B0);
        constexpr const Color BackgroundHoverUp = Palette::Primary;
        constexpr const Color BackgroundHoverOverlayUp = Color::SetA(Palette::Primary, 0.12f);
        constexpr const Color BackgroundHoverOverlayDown = Color::SetA(Palette::Primary, 0.08f);
        constexpr const Color BackgroundUp = Palette::Primary;
        constexpr const Color BackgroundDown = Down;
        constexpr const Color BackgroundDisabled = Palette::PrimaryDisabled;
        constexpr const Color FontUp = Palette::Font;
        constexpr const Color FontDown = Down;
        constexpr const Color FontDisabled = Palette::FontDisabled;
      }

      namespace ToggleButton
      {
        constexpr const Color CursorChecked = Palette::Primary;
        constexpr const Color CursorCheckedDisabled = Color::ClearA(CursorChecked);
        constexpr const Color CursorUnchecked = Palette::Primary;
        constexpr const Color CursorUncheckedDisabled = Color::ClearA(CursorUnchecked);
        constexpr const Color BackgroundEnabled = Palette::Primary;
        constexpr const Color BackgroundDisabled = Palette::PrimaryDisabled;
        constexpr const Color HoverOverlayChecked = Color::SetA(CursorChecked, 0.4f);
        constexpr const Color HoverOverlayUnchecked = Color::SetA(CursorUnchecked, 0.4f);
      }


      namespace CheckBox
      {
        constexpr const Color CursorChecked = Palette::Primary;
        constexpr const Color CursorCheckedDisabled = Color::SetA(CursorChecked, 0.4f);
        constexpr const Color CursorUnchecked = Color::ClearA(CursorChecked);
        constexpr const Color CursorUncheckedDisabled = Color::ClearA(CursorCheckedDisabled);
        constexpr const Color BackgroundChecked = Palette::Primary;
        constexpr const Color BackgroundCheckedDisabled = Palette::PrimaryDisabled;
        constexpr const Color BackgroundUnchecked = Palette::Primary;
        constexpr const Color BackgroundUncheckedDisabled = Palette::PrimaryDisabled;
        constexpr const Color HoverOverlayChecked = Color::SetA(CursorChecked, 0.4f);
        constexpr const Color HoverOverlayUnchecked = Color::SetA(BackgroundUnchecked, 0.4f);
      }

      namespace Slider
      {
        constexpr const Color HoverOverlay = Color::SetA(Palette::Primary, 0.4f);
      }

      namespace Switch
      {
        constexpr const Color CursorChecked = Palette::Primary;
        constexpr const Color CursorCheckedDisabled = Palette::PrimaryDisabled;
        constexpr const Color CursorUnchecked = Palette::Primary;
        constexpr const Color CursorUncheckedDisabled = Palette::PrimaryDisabled;
        constexpr const Color BackgroundChecked = Palette::Primary;
        constexpr const Color BackgroundCheckedDisabled = Palette::PrimaryDisabled;
        constexpr const Color BackgroundUnchecked = Palette::Primary;
        constexpr const Color BackgroundUncheckedDisabled = Palette::PrimaryDisabled;
        constexpr const Color HoverOverlayChecked = Color::SetA(CursorChecked, 0.4f);
        constexpr const Color HoverOverlayUnchecked = Color::SetA(BackgroundUnchecked, 0.4f);
      }
    }
  }
}

#endif
