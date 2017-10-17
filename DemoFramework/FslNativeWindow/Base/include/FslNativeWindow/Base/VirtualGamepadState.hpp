#ifndef FSLNATIVEWINDOW_BASE_VIRTUALGAMEPADSTATE_HPP
#define FSLNATIVEWINDOW_BASE_VIRTUALGAMEPADSTATE_HPP
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
  struct VirtualGamepadState
  {
    bool IsConnected;
    uint8_t DeviceId;

    uint32_t Buttons;
    uint8_t LeftTrigger;
    uint8_t RightTrigger;
    int16_t LeftThumbX;
    int16_t LeftThumbY;
    int16_t RightThumbX;
    int16_t RightThumbY;

    VirtualGamepadState()
      : IsConnected(false)
      , DeviceId(0)
      , Buttons(0)
      , LeftTrigger(0)
      , RightTrigger(0)
      , LeftThumbX(0)
      , LeftThumbY(0)
      , RightThumbX(0)
      , RightThumbY(0)
    {
    }


    bool IsPressed(const VirtualGamepadButton button) const
    {
      return (Buttons & static_cast<uint32_t>(button)) == static_cast<uint32_t>(button);
    }


    void SetButtonState(const VirtualGamepadButton button, const bool pressed)
    {
      if (pressed)
        Buttons |= static_cast<uint32_t>(button);
      else
        Buttons &= ~static_cast<uint32_t>(button);
    }

    //! @brief Tests for equality.
    bool operator==(const VirtualGamepadState& rhs) const
    {
      return IsConnected == rhs.IsConnected &&
        DeviceId == rhs.DeviceId &&
        Buttons == rhs.Buttons &&
        LeftTrigger == rhs.LeftTrigger &&
        RightTrigger == rhs.RightTrigger &&
        LeftThumbX == rhs.LeftThumbX &&
        LeftThumbY == rhs.LeftThumbY &&
        RightThumbX == rhs.RightThumbX &&
        RightThumbY == rhs.RightThumbY;
    }

    //! @brief Tests for inequality.
    bool operator!=(const VirtualGamepadState& rhs) const
    {
      return !(*this == rhs);
    }

  };
}

#endif
