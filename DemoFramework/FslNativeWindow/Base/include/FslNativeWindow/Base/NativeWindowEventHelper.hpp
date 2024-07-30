#ifndef FSLNATIVEWINDOW_BASE_NATIVEWINDOWEVENTHELPER_HPP
#define FSLNATIVEWINDOW_BASE_NATIVEWINDOWEVENTHELPER_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Time/MillisecondTickCount32.hpp>
#include <FslNativeWindow/Base/NativeWindowEvent.hpp>
#include <FslNativeWindow/Base/VirtualKey.hpp>
#include <FslNativeWindow/Base/VirtualMouseButton.hpp>
#include <FslNativeWindow/Base/VirtualMouseButtonFlags.hpp>

namespace Fsl
{
  struct VirtualGamepadState;

  class NativeWindowEventHelper
  {
  public:
    static constexpr MillisecondTickCount32 UnknownTimestamp() noexcept
    {
      return {};
    }

    //! @brief Create a window activation event
    static NativeWindowEvent EncodeWindowActivationEvent(const bool activated) noexcept;

    //! @brief Decode a window activation event
    static void DecodeWindowActivationEvent(const NativeWindowEvent& event, bool& rActivated) noexcept;

    //! @brief Create a window suspend event
    static NativeWindowEvent EncodeWindowSuspendEvent(const bool suspend) noexcept;

    //! @brief Decode a window suspend event
    static void DecodeWindowSuspendEvent(const NativeWindowEvent& event, bool& rSuspend) noexcept;

    //! @brief Create window resize event
    static NativeWindowEvent EncodeWindowResizedEvent() noexcept;

    //! @brief Create window config updated event (dpi, dp, etc)
    static NativeWindowEvent EncodeWindowConfigChanged() noexcept;

    //! @brief Create a low memory event
    static NativeWindowEvent EncodeLowMemoryEvent() noexcept;

    //! @brief Create a input key event
    static NativeWindowEvent EncodeInputKeyEvent(const VirtualKey::Enum virtualKey, const bool isPressed, const uint32_t deviceId = 0) noexcept;

    //! @brief Decode a input key event
    static void DecodeInputKeyEvent(const NativeWindowEvent& event, VirtualKey::Enum& rVirtualKey, bool& rIsPressed, uint32_t& rDeviceId) noexcept;

    //! @brief Create a input mouse button event
    static NativeWindowEvent EncodeInputMouseButtonEvent(const MillisecondTickCount32 timestamp, const VirtualMouseButton button,
                                                         const bool isPressed, const PxPoint2 position, const bool isTouch = false);

    //! @brief Decode a input mouse button event
    static void DecodeInputMouseButtonEvent(const NativeWindowEvent& event, VirtualMouseButton& rButton, bool& rIsPressed, PxPoint2& rPosition,
                                            bool& rIsTouch) noexcept;

    //! @brief Create a input mouse move event
    static NativeWindowEvent
      EncodeInputMouseMoveEvent(const MillisecondTickCount32 timestamp, const PxPoint2 position,
                                const VirtualMouseButtonFlags buttonFlags = VirtualMouseButtonFlags(VirtualMouseButton::Undefined),
                                const bool isTouch = false);

    //! @brief Decode a input mouse move event
    static void DecodeInputMouseMoveEvent(const NativeWindowEvent& event, PxPoint2& rPosition, VirtualMouseButtonFlags& rFlags,
                                          bool& rIsTouch) noexcept;

    //! @brief Create a input mouse wheel event
    static NativeWindowEvent EncodeInputMouseWheelEvent(const MillisecondTickCount32 timestamp, const int32_t delta, const PxPoint2 position);

    //! @brief Decode a input mouse wheel event
    static void DecodeInputMouseWheelEvent(const NativeWindowEvent& event, int32_t& rDelta, PxPoint2& rPosition) noexcept;

    //! @brief Create a raw input mouse move event
    static NativeWindowEvent
      EncodeInputRawMouseMoveEvent(const MillisecondTickCount32 timestamp, const PxPoint2 position,
                                   const VirtualMouseButtonFlags buttonFlags = VirtualMouseButtonFlags(VirtualMouseButton::Undefined)) noexcept;

    //! @brief Decode a raw input mouse move event
    static void DecodeInputRawMouseMoveEvent(const NativeWindowEvent& event, PxPoint2& rPosition, VirtualMouseButtonFlags& rFlags) noexcept;

    //! @brief Encode a position in a int32 (the x,y coordinates are expected to fit in a int16_t each)
    static int32_t EncodePosition(const PxPoint2 position);

    //! @brief Decode a position
    static PxPoint2 DecodePosition(const int32_t encodedPosition) noexcept;

    //! @brief Encode the mouse-button flags
    static int32_t EncodeVirtualMouseButtonFlags(const VirtualMouseButtonFlags flags) noexcept;

    //! @brief Decode the mouse-button flags
    static VirtualMouseButtonFlags DecodeVirtualMouseButtonFlags(const int32_t encodedFlags) noexcept;

    static NativeWindowEvent EncodeGamepadConfiguration(const uint32_t maxDevices) noexcept;
    static void DecodeGamepadConfiguration(const NativeWindowEvent& event, uint32_t& rMaxDevices) noexcept;

    static NativeWindowEvent EncodeVirtualGamepadStateEvent(const VirtualGamepadState& state) noexcept;
    static void DecodeVirtualGamepadStateEvent(const NativeWindowEvent& event, VirtualGamepadState& rState) noexcept;
  };
}

#endif
