#ifndef FSLNATIVEWINDOW_BASE_VIRTUALKEYTOSTRING_HPP
#define FSLNATIVEWINDOW_BASE_VIRTUALKEYTOSTRING_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslNativeWindow/Base/VirtualKey.hpp>
#include <fmt/format.h>
#include <string>

namespace Fsl::Debug
{
  inline const char* TryToString(const VirtualKey::Enum key)
  {
    switch (key)
    {
    case VirtualKey::Undefined:
      return "Undefined";

    case VirtualKey::Escape:
      return "Escape";
    case VirtualKey::Tab:
      return "Tab";
    case VirtualKey::Return:
      return "Return";
    // case VirtualKey::Enter:
    case VirtualKey::Space:
      return "Space";
    case VirtualKey::Backspace:
      return "Backspace";

    case VirtualKey::Code0:
      return "0";
    case VirtualKey::Code1:
      return "1";
    case VirtualKey::Code2:
      return "2";
    case VirtualKey::Code3:
      return "3";
    case VirtualKey::Code4:
      return "4";
    case VirtualKey::Code5:
      return "5";
    case VirtualKey::Code6:
      return "6";
    case VirtualKey::Code7:
      return "7";
    case VirtualKey::Code8:
      return "8";
    case VirtualKey::Code9:
      return "9";

    case VirtualKey::A:
      return "A";
    case VirtualKey::B:
      return "B";
    case VirtualKey::C:
      return "C";
    case VirtualKey::D:
      return "D";
    case VirtualKey::E:
      return "E";
    case VirtualKey::F:
      return "F";
    case VirtualKey::G:
      return "G";
    case VirtualKey::H:
      return "H";
    case VirtualKey::I:
      return "I";
    case VirtualKey::J:
      return "J";
    case VirtualKey::K:
      return "K";
    case VirtualKey::L:
      return "L";
    case VirtualKey::M:
      return "M";
    case VirtualKey::N:
      return "N";
    case VirtualKey::O:
      return "O";
    case VirtualKey::P:
      return "P";
    case VirtualKey::Q:
      return "Q";
    case VirtualKey::R:
      return "R";
    case VirtualKey::S:
      return "S";
    case VirtualKey::T:
      return "T";
    case VirtualKey::U:
      return "U";
    case VirtualKey::V:
      return "V";
    case VirtualKey::W:
      return "W";
    case VirtualKey::X:
      return "X";
    case VirtualKey::Y:
      return "Y";
    case VirtualKey::Z:
      return "Z";

    case VirtualKey::LeftShift:
      return "LeftShift";
    case VirtualKey::RightShift:
      return "RightShift";

    // Game pad mappings
    case VirtualKey::GamePadButtonA:
      return "GamePadButtonA";
    case VirtualKey::GamePadButtonB:
      return "GamePadButtonB";
    case VirtualKey::GamePadButtonC:
      return "GamePadButtonC";
    case VirtualKey::GamePadButtonX:
      return "GamePadButtonX";
    case VirtualKey::GamePadButtonY:
      return "GamePadButtonY";
    case VirtualKey::GamePadButtonZ:
      return "GamePadButtonZ";
    case VirtualKey::GamePadButtonLeftShoulder:
      return "GamePadButtonLeftShoulder";
    case VirtualKey::GamePadButtonRightShoulder:
      return "GamePadButtonRightShoulder";
    case VirtualKey::GamePadButtonLeftShoulder2:
      return "GamePadButtonLeftShoulder2";
    case VirtualKey::GamePadButtonRightShoulder2:
      return "GamePadButtonRightShoulder2";
    case VirtualKey::GamePadButtonLeftThumb:
      return "GamePadButtonLeftThumb";
    case VirtualKey::GamePadButtonRightThumb:
      return "GamePadButtonRightThumb";
    case VirtualKey::GamePadButtonStart:
      return "GamePadButtonStart";
    case VirtualKey::GamePadButtonSelect:
      return "GamePadButtonSelect";
    case VirtualKey::GamePadButtonMode:
      return "GamePadButtonMode";
    case VirtualKey::GamePadButtonBack:
      return "GamePadButtonBack";
    case VirtualKey::GamePadDpadUp:
      return "GamePadDpadUp";
    case VirtualKey::GamePadDpadDown:
      return "GamePadDpadDown";
    case VirtualKey::GamePadDpadLeft:
      return "GamePadDpadLeft";
    case VirtualKey::GamePadDpadRight:
      return "GamePadDpadRight";
    case VirtualKey::GamePadDpadCenter:
      return "GamePadDpadCenter";

    case VirtualKey::UpArrow:
      return "UpArrow";
    case VirtualKey::DownArrow:
      return "DownArrow";
    case VirtualKey::LeftArrow:
      return "LeftArrow";
    case VirtualKey::RightArrow:
      return "RightArrow";

    case VirtualKey::F1:
      return "F1";
    case VirtualKey::F2:
      return "F2";
    case VirtualKey::F3:
      return "F3";
    case VirtualKey::F4:
      return "F4";
    case VirtualKey::F5:
      return "F5";
    case VirtualKey::F6:
      return "F6";
    case VirtualKey::F7:
      return "F7";
    case VirtualKey::F8:
      return "F8";
    case VirtualKey::F9:
      return "F9";
    case VirtualKey::F10:
      return "F10";
    case VirtualKey::F11:
      return "F11";
    case VirtualKey::F12:
      return "F12";

    case VirtualKey::Insert:
      return "Insert";
    case VirtualKey::Delete:
      return "Delete";
    case VirtualKey::Home:
      return "Home";
    case VirtualKey::End:
      return "End";
    case VirtualKey::PageUp:
      return "PageUp";
    case VirtualKey::PageDown:
      return "PageDown";

    case VirtualKey::PrintScreen:
      return "PrintScreen";
    case VirtualKey::ScrollLock:
      return "ScrollLock";
    case VirtualKey::Pause:
      return "Pause";
    case VirtualKey::Break:
      return "Break";

    case VirtualKey::Add:
      return "Add";
    case VirtualKey::Subtract:
      return "Subtract";

    case VirtualKey::Unknown:
      return "Unknown";
    }
    return nullptr;
  }

  inline std::string ToString(const VirtualKey::Enum key)
  {
    const char* const psz = TryToString(key);
    if (psz != nullptr)
    {
      return fmt::format("{} (0x{:x})", psz, static_cast<int64_t>(key));
    }
    return fmt::format("0x{:x}", static_cast<int64_t>(key));
  }
}

#endif
