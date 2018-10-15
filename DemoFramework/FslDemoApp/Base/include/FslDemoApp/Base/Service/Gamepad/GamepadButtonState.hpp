#ifndef FSLDEMOAPP_BASE_SERVICE_GAMEPAD_GAMEPADBUTTONSTATE_HPP
#define FSLDEMOAPP_BASE_SERVICE_GAMEPAD_GAMEPADBUTTONSTATE_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <FslNativeWindow/Base/VirtualGamepadButton.hpp>

namespace Fsl
{
  struct GamepadButtonState
  {
    uint32_t State{0};

    GamepadButtonState() = default;


    bool IsPressed(const VirtualGamepadButton button) const
    {
      return (State & static_cast<uint32_t>(button)) == static_cast<uint32_t>(button);
    }


    void SetButtonState(const VirtualGamepadButton button, const bool pressed)
    {
      if (pressed)
      {
        State |= static_cast<uint32_t>(button);
      }
      else
      {
        State &= ~static_cast<uint32_t>(button);
      }
    }


    bool operator==(const GamepadButtonState& rhs) const
    {
      return State == rhs.State;
    }

    bool operator!=(const GamepadButtonState& rhs) const
    {
      return State != rhs.State;
    }
  };
}

#endif
