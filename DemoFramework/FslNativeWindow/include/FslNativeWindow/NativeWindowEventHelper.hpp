#ifndef FSLNATIVEWINDOW_NATIVEWINDOWEVENTHELPER_HPP
#define FSLNATIVEWINDOW_NATIVEWINDOWEVENTHELPER_HPP
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
#include <FslBase/Math/Point2.hpp>
#include <FslNativeWindow/NativeWindowEvent.hpp>
#include <FslNativeWindow/VirtualKey.hpp>
#include <FslNativeWindow/VirtualMouseButton.hpp>
#include <FslNativeWindow/VirtualMouseButtonFlags.hpp>

namespace Fsl
{
  class NativeWindowEventHelper
  {
  public:
    //! @brief Create a window activation event
    static NativeWindowEvent EncodeWindowActivationEvent(const bool activated);

    //! @brief Decode a window activation event
    static void DecodeWindowActivationEvent(const NativeWindowEvent& event, bool& rActivated);

    //! @brief Create a window suspend event
    static NativeWindowEvent EncodeWindowSuspendEvent(const bool suspend);

    //! @brief Decode a window suspend event
    static void DecodeWindowSuspendEvent(const NativeWindowEvent& event, bool& rSuspend);

    //! @brief Create window resize event
    static NativeWindowEvent EncodeWindowResizedEvent();

    //! @brief Create window DPI updated event
    static NativeWindowEvent EncodeWindowDPIChanged();

    //! @brief Create a low memory event
    static NativeWindowEvent EncodeLowMemoryEvent();

    //! @brief Create a input key event
    static NativeWindowEvent EncodeInputKeyEvent(const VirtualKey::Enum virtualKey, const bool isPressed);

    //! @brief Decode a input key event
    static void DecodeInputKeyEvent(const NativeWindowEvent& event, VirtualKey::Enum& rVirtualKey, bool& rIsPressed);

    //! @brief Create a input mouse button event
    static NativeWindowEvent EncodeInputMouseButtonEvent(const VirtualMouseButton::Enum button, const bool isPressed, const Point2& position);

    //! @brief Decode a input mouse button event
    static void DecodeInputMouseButtonEvent(const NativeWindowEvent& event, VirtualMouseButton::Enum& rButton, bool& rIsPressed, Point2& rPosition);

    //! @brief Create a input mouse move event
    static NativeWindowEvent EncodeInputMouseMoveEvent(const Point2& position, const VirtualMouseButtonFlags& buttonFlags = VirtualMouseButtonFlags(VirtualMouseButton::Undefined));

    //! @brief Decode a input mouse move event
    static void DecodeInputMouseMoveEvent(const NativeWindowEvent& event, Point2& rPosition, VirtualMouseButtonFlags& rFlags);

    //! @brief Create a input mouse wheel event
    static NativeWindowEvent EncodeInputMouseWheelEvent(const int32_t delta, const Point2& position);

    //! @brief Decode a input mouse wheel event
    static void DecodeInputMouseWheelEvent(const NativeWindowEvent& event, int32_t& rDelta, Point2& rPosition);

    //! @brief Encode a position in a int32 (the x,y coordinates are expected to fit in a int16_t each)
    static int32_t EncodePosition(const Point2& position);

    //! @brief Decode a position
    static Point2 DecodePosition(const int32_t position);

    //! @brief Encode the mouse-button flags
    static int32_t EncodeVirtualMouseButtonFlags(const VirtualMouseButtonFlags& flags);

    //! @brief Decode the mouse-button flags
    static VirtualMouseButtonFlags DecodeVirtualMouseButtonFlags(const int32_t encodedFlags);
  };

}

#endif
