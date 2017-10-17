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

#include <FslDemoHost/Base/Service/Gamepad/GamepadsState.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslDemoApp/Base/Service/NativeWindowEvents/INativeWindowEvents.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/Base/VirtualGamepadState.hpp>
#include <algorithm>
#include <cmath>

namespace Fsl
{
  namespace
  {
    const float DEADZONE = 0.25f;


    // Deadzone code based on this article:
    // http://www.third-helix.com/2013/04/12/doing-thumbstick-dead-zones-right.html

    void ApplyAxialDeadZone(GamepadThumbStick& rStick)
    {
      if (std::abs(rStick.X) < DEADZONE)
        rStick.X = 0.0f;
      if (std::abs(rStick.Y) < DEADZONE)
        rStick.Y = 0.0f;
    }


    void ApplyRadialDeadZone(GamepadThumbStick& rStick)
    {
      const auto length = std::sqrt((rStick.X*rStick.X) + (rStick.Y*rStick.Y));
      if (length < DEADZONE)
      {
        rStick.X = 0.0f;
        rStick.Y = 0.0f;
      }
    }


    void ApplyScaledRadialDeadZone(GamepadThumbStick& rStick)
    {
      const auto length = std::sqrt((rStick.X*rStick.X) + (rStick.Y*rStick.Y));
      if (length < DEADZONE)
      {
        rStick.X = 0.0f;
        rStick.Y = 0.0f;
      }
      else
      {
        const float scaled = ((length - DEADZONE) / (1.0f - DEADZONE));
        rStick.X = std::max(std::min((rStick.X / length) * scaled, 1.0f), -1.0f);
        rStick.Y = std::max(std::min((rStick.Y / length) * scaled, 1.0f), -1.0f);
      }
    }


    void HandleGamepadConfigurationEvent(const NativeWindowEvent& event, std::vector<GamepadState>& rGamepads)
    {
      uint32_t maxDevices;
      NativeWindowEventHelper::DecodeGamepadConfiguration(event, maxDevices);

      rGamepads.clear();
      rGamepads.resize(maxDevices);
    }

    float Normalize(const int16_t value)
    {
      if (value > 0)
        return std::min(static_cast<float>(value) / static_cast<float>(std::numeric_limits<int16_t>::max()), 1.0f);
      else
        return std::max(static_cast<float>(value) / (-static_cast<float>(std::numeric_limits<int16_t>::min())), -1.0f);
    }


    void HandleGamepadStateEvent(const NativeWindowEvent& event, std::vector<GamepadState>& rGamepads, const GamepadDeadZoneType deadZoneType, const bool isConfigured)
    {
      VirtualGamepadState gamepadState;
      NativeWindowEventHelper::DecodeVirtualGamepadStateEvent(event, gamepadState);

      if (gamepadState.DeviceId >= rGamepads.size())
      {
        FSLLOG_DEBUG_WARNING_IF(!isConfigured, "GamepadsService: event received before the service was configured, ignored");
        FSLLOG_DEBUG_WARNING_IF(isConfigured, "GamepadsService: event from a invalid deviceId: " << gamepadState.DeviceId << ", event ignored");
        return;
      }

      auto oldState = rGamepads[gamepadState.DeviceId];
      auto& rNewState = rGamepads[gamepadState.DeviceId];

      // FSLLOG("R" << rNewState.IsConnected << ", " << (int)rNewState.Buttons << ", " << (int)rNewState.LeftTrigger << ", " << (int)rNewState.RightTrigger << ", " << rNewState.LeftThumbX << ", " << rNewState.LeftThumbY << ", " << rNewState.RightThumbX << ", " << rNewState.RightThumbY);

      rNewState.IsConnected = gamepadState.IsConnected;
      rNewState.Buttons.State = gamepadState.Buttons;
      rNewState.LeftTrigger.Value = gamepadState.LeftTrigger;
      rNewState.RightTrigger.Value = gamepadState.RightTrigger;
      rNewState.LeftThumb.X = Normalize(gamepadState.LeftThumbX);
      rNewState.LeftThumb.Y = Normalize(gamepadState.LeftThumbY);
      rNewState.RightThumb.X = Normalize(gamepadState.RightThumbX);
      rNewState.RightThumb.Y = Normalize(gamepadState.RightThumbY);

      switch (deadZoneType)
      {
      case GamepadDeadZoneType::Disabled:
        break;
      case GamepadDeadZoneType::Axial:
        ApplyAxialDeadZone(rNewState.LeftThumb);
        ApplyAxialDeadZone(rNewState.RightThumb);
        break;
      case GamepadDeadZoneType::Radial:
        ApplyRadialDeadZone(rNewState.LeftThumb);
        ApplyRadialDeadZone(rNewState.RightThumb);
        break;
      case GamepadDeadZoneType::ScaledRadial:
        ApplyScaledRadialDeadZone(rNewState.LeftThumb);
        ApplyScaledRadialDeadZone(rNewState.RightThumb);
        break;
      default:
        break;
      }

      if (oldState == rNewState)
        return;

      //eventPoster->Post()
    }


    void CheckInputKeyStateEvent(const NativeWindowEvent& event, const std::size_t maxDevices, const bool isConfigured)
    {
      VirtualKey::Enum virtualKey;
      bool isPressed;
      uint32_t deviceId;
      NativeWindowEventHelper::DecodeInputKeyEvent(event, virtualKey, isPressed, deviceId);

      if (virtualKey < VirtualKey::SYS_GAMEPAD_RANGE_FIRST || virtualKey > VirtualKey::SYS_GAMEPAD_RANGE_LAST)
        return;

      FSLLOG_DEBUG_WARNING_IF(!isConfigured, "GamepadsService: gamepad key event received before the service was configured, ignored");
      FSLLOG_DEBUG_WARNING_IF(isConfigured && deviceId >= maxDevices, "GamepadsService: gamepad key event from a invalid deviceId: " << deviceId << ", event ignored");
    }
  }

  GamepadsState::GamepadsState()
    : m_gamepads()
    , m_deadZoneType(GamepadDeadZoneType::ScaledRadial)
    , m_isConfigured(false)
  {
  }


  void GamepadsState::SetDeadZoneType(const GamepadDeadZoneType type)
  {
    m_deadZoneType = type;
  }


  uint32_t GamepadsState::GetCount() const
  {
    return static_cast<uint32_t>(m_gamepads.size());
  }


  GamepadState GamepadsState::GetState(const uint32_t gamepadIndex) const
  {
    if (gamepadIndex >= m_gamepads.size())
      throw std::invalid_argument("gamepadIndex out of bounds");
    return m_gamepads[gamepadIndex];
  }


  void GamepadsState::OnNativeWindowEvent(const NativeWindowEvent& event)
  {
    switch (event.Type)
    {
    case NativeWindowEventType::GamepadConfiguration:
      if (m_isConfigured)
      {
        FSLLOG_DEBUG_WARNING("Gamepad service is already configured, new config ignored");
        return;
      }
      HandleGamepadConfigurationEvent(event, m_gamepads);
      m_isConfigured = true;
      break;
    case NativeWindowEventType::GamepadState:
      HandleGamepadStateEvent(event, m_gamepads, m_deadZoneType, m_isConfigured);
      break;
    case NativeWindowEventType::InputKey:
      CheckInputKeyStateEvent(event, m_gamepads.size(), m_isConfigured);
      break;
    default:
      break;
    }
  }
}
