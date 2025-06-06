/****************************************************************************************************************************************************
 * Copyright 2017, 2024 NXP
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
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslBase/Span/SpanUtil_Create.hpp>
#include <FslDemoApp/Base/Service/NativeWindowEvents/INativeWindowEvents.hpp>
#include <FslDemoHost/Base/Service/Gamepad/GamepadsState.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/Base/VirtualGamepadState.hpp>
#include <algorithm>
#include <cmath>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr float Deadzone = 0.25f;
    }


    // Deadzone code based on this article:
    // http://www.third-helix.com/2013/04/12/doing-thumbstick-dead-zones-right.html

    void ApplyAxialDeadZone(GamepadThumbStick& rStick)
    {
      if (std::abs(rStick.X) < LocalConfig::Deadzone)
      {
        rStick.X = 0.0f;
      }
      if (std::abs(rStick.Y) < LocalConfig::Deadzone)
      {
        rStick.Y = 0.0f;
      }
    }


    void ApplyRadialDeadZone(GamepadThumbStick& rStick)
    {
      const auto length = std::sqrt((rStick.X * rStick.X) + (rStick.Y * rStick.Y));
      if (length < LocalConfig::Deadzone)
      {
        rStick.X = 0.0f;
        rStick.Y = 0.0f;
      }
    }


    void ApplyScaledRadialDeadZone(GamepadThumbStick& rStick)
    {
      const auto length = std::sqrt((rStick.X * rStick.X) + (rStick.Y * rStick.Y));
      if (length < LocalConfig::Deadzone)
      {
        rStick.X = 0.0f;
        rStick.Y = 0.0f;
      }
      else
      {
        const float scaled = ((length - LocalConfig::Deadzone) / (1.0f - LocalConfig::Deadzone));
        rStick.X = std::max(std::min((rStick.X / length) * scaled, 1.0f), -1.0f);
        rStick.Y = std::max(std::min((rStick.Y / length) * scaled, 1.0f), -1.0f);
      }
    }


    uint32_t HandleGamepadConfigurationEvent(const NativeWindowEvent& event, std::vector<GamepadState>& rGamepads, const bool /*wasConfigured*/)
    {
      uint32_t maxDevices = 0;
      NativeWindowEventHelper::DecodeGamepadConfiguration(event, maxDevices);

      if (maxDevices >= rGamepads.size())
      {
        rGamepads.resize(maxDevices);
      }
      return maxDevices;
    }

    float Normalize(const int16_t value)
    {
      if (value > 0)
      {
        return std::min(static_cast<float>(value) / static_cast<float>(std::numeric_limits<int16_t>::max()), 1.0f);
      }

      return std::max(static_cast<float>(value) / (-static_cast<float>(std::numeric_limits<int16_t>::min())), -1.0f);
    }


    void HandleGamepadStateEvent(const NativeWindowEvent& event, Span<GamepadState> gamepads, const GamepadDeadZoneType deadZoneType,
                                 [[maybe_unused]] const bool isConfigured)
    {
      VirtualGamepadState gamepadState;
      NativeWindowEventHelper::DecodeVirtualGamepadStateEvent(event, gamepadState);

      if (gamepadState.DeviceId >= gamepads.size())
      {
        FSLLOG3_DEBUG_WARNING_IF(!isConfigured, "GamepadsService: event received before the service was configured, ignored");
        FSLLOG3_DEBUG_WARNING_IF(isConfigured, "GamepadsService: event from a invalid deviceId: {}, event ignored", gamepadState.DeviceId);
        return;
      }

      auto oldState = gamepads[gamepadState.DeviceId];
      auto& rNewState = gamepads[gamepadState.DeviceId];

      // FSLLOG3_INFO("R" << rNewState.IsConnected << ", " << (int)rNewState.Buttons << ", " << (int)rNewState.LeftTrigger << ", " <<
      // (int)rNewState.RightTrigger << ", " << rNewState.LeftThumbX << ", " << rNewState.LeftThumbY << ", " << rNewState.RightThumbX << ", " <<
      // rNewState.RightThumbY);

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
      {
        return;
      }

      // eventPoster->Post()
    }


    void CheckInputKeyStateEvent(const NativeWindowEvent& event, [[maybe_unused]] const uint32_t maxDevices, [[maybe_unused]] const bool isConfigured)
    {
      VirtualKey::Enum virtualKey = VirtualKey::Undefined;
      bool isPressed = false;
      uint32_t deviceId = 0;
      NativeWindowEventHelper::DecodeInputKeyEvent(event, virtualKey, isPressed, deviceId);

      if (virtualKey < VirtualKey::SYS_GAMEPAD_RANGE_FIRST || virtualKey > VirtualKey::SYS_GAMEPAD_RANGE_LAST)
      {
        return;
      }

      FSLLOG3_DEBUG_WARNING_IF(!isConfigured, "GamepadsService: gamepad key event received before the service was configured, ignored");
      FSLLOG3_DEBUG_WARNING_IF(isConfigured && deviceId >= maxDevices,
                               "GamepadsService: gamepad key event from a invalid deviceId: {}, event ignored", deviceId);
    }
  }

  GamepadsState::GamepadsState()
    : m_deadZoneType(GamepadDeadZoneType::ScaledRadial)
    , m_isConfigured(false)
  {
  }


  void GamepadsState::SetDeadZoneType(const GamepadDeadZoneType type)
  {
    m_deadZoneType = type;
  }


  uint32_t GamepadsState::GetCount() const
  {
    assert(m_gamepadCount <= m_gamepads.size());
    return m_gamepadCount;
  }


  GamepadState GamepadsState::GetState(const uint32_t gamepadIndex) const
  {
    if (gamepadIndex >= m_gamepadCount)
    {
      throw std::invalid_argument("gamepadIndex out of bounds");
    }
    return m_gamepads[gamepadIndex];
  }


  void GamepadsState::OnNativeWindowEvent(const NativeWindowEvent& event)
  {
    switch (event.Type)
    {
    case NativeWindowEventType::GamepadConfiguration:
      m_gamepadCount = HandleGamepadConfigurationEvent(event, m_gamepads, m_isConfigured);
      m_isConfigured = true;
      break;
    case NativeWindowEventType::GamepadState:
      HandleGamepadStateEvent(event, SpanUtil::Create(m_gamepads.data(), m_gamepadCount), m_deadZoneType, m_isConfigured);
      break;
    case NativeWindowEventType::InputKey:
      CheckInputKeyStateEvent(event, m_gamepadCount, m_isConfigured);
      break;
    default:
      break;
    }
  }
}
