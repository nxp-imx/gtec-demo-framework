/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/Base/VirtualGamepadState.hpp>
#include <FslNativeWindow/Base/VirtualKeyFlag.hpp>
#include <cassert>
#include <limits>

namespace Fsl
{
  NativeWindowEvent NativeWindowEventHelper::EncodeWindowActivationEvent(const bool activated) noexcept
  {
    return {UnknownTimestamp(), NativeWindowEventType::WindowActivation, activated ? 1 : 0};
  }


  void NativeWindowEventHelper::DecodeWindowActivationEvent(const NativeWindowEvent& event, bool& rActivated) noexcept
  {
    assert(event.Type == NativeWindowEventType::WindowActivation);
    rActivated = event.Arg1 != 0;
  }


  NativeWindowEvent NativeWindowEventHelper::EncodeWindowSuspendEvent(const bool suspend) noexcept
  {
    return {UnknownTimestamp(), NativeWindowEventType::WindowSuspend, suspend ? 1 : 0};
  }


  void NativeWindowEventHelper::DecodeWindowSuspendEvent(const NativeWindowEvent& event, bool& rSuspend) noexcept
  {
    assert(event.Type == NativeWindowEventType::WindowActivation);
    rSuspend = event.Arg1 != 0;
  }

  NativeWindowEvent NativeWindowEventHelper::EncodeWindowResizedEvent() noexcept
  {
    return {UnknownTimestamp(), NativeWindowEventType::WindowResized};
  }


  NativeWindowEvent NativeWindowEventHelper::EncodeWindowConfigChanged() noexcept
  {
    return {UnknownTimestamp(), NativeWindowEventType::WindowConfigChanged};
  }


  NativeWindowEvent NativeWindowEventHelper::EncodeLowMemoryEvent() noexcept
  {
    return {UnknownTimestamp(), NativeWindowEventType::LowMemory};
  }


  NativeWindowEvent NativeWindowEventHelper::EncodeInputKeyEvent(const VirtualKey::Enum virtualKey, const bool isPressed,
                                                                 const uint32_t deviceId) noexcept
  {
    const int32_t arg1 = virtualKey;
    const int32_t arg2 = (isPressed ? VirtualKeyFlag::IsPressed : 0);
    const auto arg3 = static_cast<int32_t>(deviceId);
    return {UnknownTimestamp(), NativeWindowEventType::InputKey, arg1, arg2, arg3};
  }


  void NativeWindowEventHelper::DecodeInputKeyEvent(const NativeWindowEvent& event, VirtualKey::Enum& rVirtualKey, bool& rIsPressed,
                                                    uint32_t& rDeviceId) noexcept
  {
    assert(event.Type == NativeWindowEventType::InputKey);
    rVirtualKey = static_cast<VirtualKey::Enum>(event.Arg1);
    rIsPressed = (event.Arg2 & VirtualKeyFlag::IsPressed) == VirtualKeyFlag::IsPressed;
    rDeviceId = static_cast<uint32_t>(event.Arg3);
  }


  NativeWindowEvent NativeWindowEventHelper::EncodeInputMouseButtonEvent(const MillisecondTickCount32 timestamp, const VirtualMouseButton button,
                                                                         const bool isPressed, const PxPoint2 position, const bool isTouch)
  {
    const auto arg1 = static_cast<int32_t>(button);
    const int32_t arg2 = (isPressed ? VirtualKeyFlag::IsPressed : 0);
    const int32_t arg3 = EncodePosition(position);
    const int32_t arg4 = isTouch ? 1 : 0;

    return {timestamp, NativeWindowEventType::InputMouseButton, arg1, arg2, arg3, arg4};
  }


  void NativeWindowEventHelper::DecodeInputMouseButtonEvent(const NativeWindowEvent& event, VirtualMouseButton& rButton, bool& rIsPressed,
                                                            PxPoint2& rPosition, bool& rIsTouch) noexcept
  {
    assert(event.Type == NativeWindowEventType::InputMouseButton);
    rButton = static_cast<VirtualMouseButton>(event.Arg1);
    rIsPressed = (event.Arg2 & VirtualKeyFlag::IsPressed) == VirtualKeyFlag::IsPressed;
    rPosition = DecodePosition(event.Arg3);
    rIsTouch = event.Arg4 != 0;
  }


  NativeWindowEvent NativeWindowEventHelper::EncodeInputMouseMoveEvent(const MillisecondTickCount32 timestamp, const PxPoint2 position,
                                                                       const VirtualMouseButtonFlags buttonFlags, const bool isTouch)
  {
    const int32_t arg1 = EncodePosition(position);
    const int32_t arg2 = EncodeVirtualMouseButtonFlags(buttonFlags);
    const int32_t arg3 = isTouch ? 1 : 0;
    return {timestamp, NativeWindowEventType::InputMouseMove, arg1, arg2, arg3};
  }


  void NativeWindowEventHelper::DecodeInputMouseMoveEvent(const NativeWindowEvent& event, PxPoint2& rPosition, VirtualMouseButtonFlags& rFlags,
                                                          bool& rIsTouch) noexcept
  {
    assert(event.Type == NativeWindowEventType::InputMouseMove);
    rPosition = DecodePosition(event.Arg1);
    rFlags = DecodeVirtualMouseButtonFlags(event.Arg2);
    rIsTouch = event.Arg3 != 0;
  }


  NativeWindowEvent NativeWindowEventHelper::EncodeInputMouseWheelEvent(const MillisecondTickCount32 timestamp, const int32_t delta,
                                                                        const PxPoint2 position)
  {
    const int32_t arg1 = delta;
    const int32_t arg2 = EncodePosition(position);

    return {timestamp, NativeWindowEventType::InputMouseWheel, arg1, arg2};
  }


  void NativeWindowEventHelper::DecodeInputMouseWheelEvent(const NativeWindowEvent& event, int32_t& rDelta, PxPoint2& rPosition) noexcept
  {
    assert(event.Type == NativeWindowEventType::InputMouseWheel);
    rDelta = event.Arg1;
    rPosition = DecodePosition(event.Arg2);
  }


  NativeWindowEvent NativeWindowEventHelper::EncodeInputRawMouseMoveEvent(const MillisecondTickCount32 timestamp, const PxPoint2 position,
                                                                          const VirtualMouseButtonFlags buttonFlags) noexcept
  {
    const int32_t arg1 = position.X.Value;
    const int32_t arg2 = position.Y.Value;
    const int32_t arg3 = EncodeVirtualMouseButtonFlags(buttonFlags);
    return {timestamp, NativeWindowEventType::InputRawMouseMove, arg1, arg2, arg3};
  }


  void NativeWindowEventHelper::DecodeInputRawMouseMoveEvent(const NativeWindowEvent& event, PxPoint2& rPosition,
                                                             VirtualMouseButtonFlags& rFlags) noexcept
  {
    assert(event.Type == NativeWindowEventType::InputRawMouseMove);
    rPosition = PxPoint2::Create(event.Arg1, event.Arg2);
    rFlags = DecodeVirtualMouseButtonFlags(event.Arg3);
  }


  int32_t NativeWindowEventHelper::EncodePosition(const PxPoint2 position)
  {
    if (position.X.Value < std::numeric_limits<int16_t>::min() || position.X.Value > std::numeric_limits<int16_t>::max())
    {
      throw UsageErrorException("the x-position is expected to fit inside a int16");
    }
    if (position.Y.Value < std::numeric_limits<int16_t>::min() || position.Y.Value > std::numeric_limits<int16_t>::max())
    {
      throw UsageErrorException("the y-position is expected to fit inside a int16");
    }
    return ((position.Y.Value & 0xFFFF) << 16) | (position.X.Value & 0xFFFF);
  }


  PxPoint2 NativeWindowEventHelper::DecodePosition(const int32_t encodedPosition) noexcept
  {
    const auto x = static_cast<int16_t>(encodedPosition & 0xFFFF);
    const auto y = static_cast<int16_t>((encodedPosition >> 16) & 0xFFFF);
    return PxPoint2::Create(x, y);
  }


  int32_t NativeWindowEventHelper::EncodeVirtualMouseButtonFlags(const VirtualMouseButtonFlags flags) noexcept
  {
    return static_cast<int32_t>(flags.Flags);
  }


  VirtualMouseButtonFlags NativeWindowEventHelper::DecodeVirtualMouseButtonFlags(const int32_t encodedFlags) noexcept
  {
    return VirtualMouseButtonFlags(static_cast<VirtualMouseButton>(encodedFlags));
  }


  NativeWindowEvent NativeWindowEventHelper::EncodeGamepadConfiguration(const uint32_t maxDevices) noexcept
  {
    const auto arg1 = static_cast<int32_t>(maxDevices);
    return {UnknownTimestamp(), NativeWindowEventType::GamepadConfiguration, arg1};
  }


  void NativeWindowEventHelper::DecodeGamepadConfiguration(const NativeWindowEvent& event, uint32_t& rMaxDevices) noexcept
  {
    assert(event.Type == NativeWindowEventType::GamepadConfiguration);
    rMaxDevices = static_cast<uint32_t>(event.Arg1);
  }


  NativeWindowEvent NativeWindowEventHelper::EncodeVirtualGamepadStateEvent(const VirtualGamepadState& state) noexcept
  {
    const auto deviceId = static_cast<uint32_t>(state.DeviceId);
    const auto leftTrigger = static_cast<uint32_t>(state.LeftTrigger) << 8;
    const auto rightTrigger = static_cast<uint32_t>(state.RightTrigger) << 16;
    const auto isConnected = static_cast<uint32_t>(state.IsConnected ? 1 : 0) << 24;

    const auto leftThumbX = (static_cast<uint32_t>(state.LeftThumbX) & 0xFFFF);
    const auto leftThumbY = (static_cast<uint32_t>(state.LeftThumbY) & 0xFFFF) << 16;

    const auto rightThumbX = (static_cast<uint32_t>(state.RightThumbX) & 0xFFFF);
    const auto rightThumbY = (static_cast<uint32_t>(state.RightThumbY) & 0XFFFF) << 16;

    const auto arg1 = static_cast<int32_t>(deviceId | leftTrigger | rightTrigger | isConnected);
    const auto arg2 = static_cast<int32_t>(state.Buttons);
    const auto arg3 = static_cast<int32_t>(leftThumbX | leftThumbY);
    const auto arg4 = static_cast<int32_t>(rightThumbX | rightThumbY);

    return {UnknownTimestamp(), NativeWindowEventType::GamepadState, arg1, arg2, arg3, arg4};
  }


  void NativeWindowEventHelper::DecodeVirtualGamepadStateEvent(const NativeWindowEvent& event, VirtualGamepadState& rState) noexcept
  {
    assert(event.Type == NativeWindowEventType::GamepadState);

    const auto arg1 = static_cast<uint32_t>(event.Arg1);
    const auto arg2 = static_cast<uint32_t>(event.Arg2);
    const auto arg3 = static_cast<uint32_t>(event.Arg3);
    const auto arg4 = static_cast<uint32_t>(event.Arg4);

    rState = VirtualGamepadState();
    rState.DeviceId = static_cast<uint8_t>(arg1 & 0xFF);
    rState.LeftTrigger = static_cast<uint8_t>((arg1 >> 8) & 0xFF);
    rState.RightTrigger = static_cast<uint8_t>((arg1 >> 16) & 0xFF);
    rState.IsConnected = ((arg1 >> 24) & 0x01) != 0;
    rState.Buttons = arg2;
    rState.LeftThumbX = static_cast<int16_t>(arg3 & 0xFFFF);
    rState.LeftThumbY = static_cast<int16_t>((arg3 >> 16) & 0xFFFF);
    rState.RightThumbX = static_cast<int16_t>(arg4 & 0xFFFF);
    rState.RightThumbY = static_cast<int16_t>((arg4 >> 16) & 0xFFFF);
  }
}
