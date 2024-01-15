#if !defined(__ANDROID__) && defined(__linux__) && !defined(FSL_WINDOWSYSTEM_X11) && defined(FSL_WINDOWSYSTEM_WAYLAND)
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

#include "WaylandUtil.hpp"
#include <linux/input.h>

namespace Fsl::WaylandUtil
{
  VirtualKey::Enum TryToVirtualKey(const uint32_t key) noexcept
  {
    switch (key)
    {
    case KEY_ESC:
      return VirtualKey::Escape;
    case KEY_F1:
      return VirtualKey::F1;
    case KEY_F2:
      return VirtualKey::F2;
    case KEY_F3:
      return VirtualKey::F3;
    case KEY_F4:
      return VirtualKey::F4;
    case KEY_F5:
      return VirtualKey::F5;
    case KEY_F6:
      return VirtualKey::F6;
    case KEY_F7:
      return VirtualKey::F7;
    case KEY_F8:
      return VirtualKey::F8;
    case KEY_F9:
      return VirtualKey::F9;
    case KEY_F10:
      return VirtualKey::F10;
    case KEY_F11:
      return VirtualKey::F11;
    case KEY_F12:
      return VirtualKey::F12;
    case KEY_0:
      return VirtualKey::Code0;
    case KEY_1:
      return VirtualKey::Code1;
    case KEY_2:
      return VirtualKey::Code2;
    case KEY_3:
      return VirtualKey::Code3;
    case KEY_4:
      return VirtualKey::Code4;
    case KEY_5:
      return VirtualKey::Code5;
    case KEY_6:
      return VirtualKey::Code6;
    case KEY_7:
      return VirtualKey::Code7;
    case KEY_8:
      return VirtualKey::Code8;
    case KEY_9:
      return VirtualKey::Code9;
    case KEY_A:
      return VirtualKey::A;
    case KEY_B:
      return VirtualKey::B;
    case KEY_C:
      return VirtualKey::C;
    case KEY_D:
      return VirtualKey::D;
    case KEY_E:
      return VirtualKey::E;
    case KEY_F:
      return VirtualKey::F;
    case KEY_G:
      return VirtualKey::G;
    case KEY_H:
      return VirtualKey::H;
    case KEY_I:
      return VirtualKey::I;
    case KEY_J:
      return VirtualKey::J;
    case KEY_K:
      return VirtualKey::K;
    case KEY_L:
      return VirtualKey::L;
    case KEY_M:
      return VirtualKey::M;
    case KEY_N:
      return VirtualKey::N;
    case KEY_O:
      return VirtualKey::O;
    case KEY_P:
      return VirtualKey::P;
    case KEY_Q:
      return VirtualKey::Q;
    case KEY_R:
      return VirtualKey::R;
    case KEY_S:
      return VirtualKey::S;
    case KEY_T:
      return VirtualKey::T;
    case KEY_U:
      return VirtualKey::U;
    case KEY_V:
      return VirtualKey::V;
    case KEY_W:
      return VirtualKey::W;
    case KEY_X:
      return VirtualKey::X;
    case KEY_Y:
      return VirtualKey::Y;
    case KEY_Z:
      return VirtualKey::Z;
    case KEY_TAB:
      return VirtualKey::Tab;
    case KEY_ENTER:
      return VirtualKey::Return;
    case KEY_SPACE:
      return VirtualKey::Space;
    case KEY_BACKSPACE:
      return VirtualKey::Backspace;
    case KEY_UP:
      return VirtualKey::UpArrow;
    case KEY_DOWN:
      return VirtualKey::DownArrow;
    case KEY_LEFT:
      return VirtualKey::LeftArrow;
    case KEY_RIGHT:
      return VirtualKey::RightArrow;
    case KEY_INSERT:
      return VirtualKey::Insert;
    case KEY_DELETE:
      return VirtualKey::Delete;
    case KEY_HOME:
      return VirtualKey::Home;
    case KEY_END:
      return VirtualKey::End;
    case KEY_PAGEUP:
      return VirtualKey::PageUp;
    case KEY_PAGEDOWN:
      return VirtualKey::PageDown;
    case KEY_SCROLLLOCK:
      return VirtualKey::ScrollLock;
    case KEY_PAUSE:
      return VirtualKey::Pause;
    case KEY_BREAK:
      return VirtualKey::Break;
    default:
      return VirtualKey::Undefined;
    }
  }
}

#endif
