#ifndef FSLNATIVEWINDOW_BASE_NATIVEWINDOWEVENTTYPE_HPP
#define FSLNATIVEWINDOW_BASE_NATIVEWINDOWEVENTTYPE_HPP
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

namespace Fsl
{
  enum class NativeWindowEventType
  {
    //! No operation (a empty message)
    NOP,
    //! The window is currently being activated or deactivated
    //! Use NativeWindowEventHelper::EncodeWindowActivationEvent to create a NativeWindowEvent.
    //! Use NativeWindowEventHelper::DecodeWindowActivationEvent to decode a NativeWindowEvent.
    WindowActivation,
    //! The window is currently being suspended or resumed
    //! Use NativeWindowEventHelper::EncodeWindowSuspendEvent to create a NativeWindowEvent.
    //! Use NativeWindowEventHelper::DecodeWindowSuspendEvent to decode a NativeWindowEvent.
    WindowSuspend,
    //! The window has been resized
    WindowResized,
    //! The window DPI has changed
    WindowDPIChanged,
    //! The system is experiencing a low memory situation.
    //! Use NativeWindowEventHelper::EncodeLowMemoryEvent to create a NativeWindowEvent.
    //! Use NativeWindowEventHelper::DecodeLowMemoryEvent to decode a NativeWindowEvent.
    LowMemory,
    //! A key event (its represents a very simple key event and is not intended to drive complex UI's)
    //! Use NativeWindowEventHelper::EncodeInputKeyEvent to create a NativeWindowEvent.
    //! Use NativeWindowEventHelper::DecodeInputKeyEvent to decode a NativeWindowEvent.
    InputKey,
    //! A mouse button event
    //! Use NativeWindowEventHelper::EncodeInputMouseButtonEvent to create a NativeWindowEvent.
    //! Use NativeWindowEventHelper::DecodeInputMouseButtonEvent to decode a NativeWindowEvent.
    InputMouseButton,
    //! A mouse move event
    //! Use NativeWindowEventHelper::EncodeInputMouseMoveEvent to create a NativeWindowEvent.
    //! Use NativeWindowEventHelper::DecodeInputMouseMoveEvent to decode a NativeWindowEvent.
    InputMouseMove,
    //! A mouse wheel event
    //! Use NativeWindowEventHelper::EncodeInputMouseWheelEvent to create a NativeWindowEvent.
    //! Use NativeWindowEventHelper::DecodeInputMouseWheelEvent to decode a NativeWindowEvent.
    InputMouseWheel,

    //! A raw mouse move event
    //! Use NativeWindowEventHelper::EncodeInputRawMouseMoveEvent to create a NativeWindowEvent.
    //! Use NativeWindowEventHelper::DecodeInputRawMouseMoveEvent to decode a NativeWindowEvent.
    InputRawMouseMove,
    //!
    //!
    // InputTouch,
    //! A gamepad service configuration event
    //! Use NativeWindowEventHelper::EncodeGamepadConfiguration to create a NativeWindowEvent.
    //! Use NativeWindowEventHelper::DecodeGamepadConfiguration to decode a NativeWindowEvent.
    GamepadConfiguration,
    //! The current state of a gamepad
    GamepadState,
  };
}

#endif
