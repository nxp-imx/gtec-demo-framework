#ifndef FSLNATIVEWINDOW_PLATFORM_ADAPTER_WIN32_PLATFORMNATIVEWINDOWADAPTERWIN32_HPP
#define FSLNATIVEWINDOW_PLATFORM_ADAPTER_WIN32_PLATFORMNATIVEWINDOWADAPTERWIN32_HPP
#ifdef FSL_WINDOWSYSTEM_WIN32
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

#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslNativeWindow/Base/VirtualMouseButtonFlags.hpp>
#include <FslNativeWindow/Platform/Adapter/PlatformNativeWindowAdapter.hpp>
#include <vector>

namespace Fsl
{
  class PlatformNativeWindowAdapterWin32 : public PlatformNativeWindowAdapter
  {
    std::shared_ptr<DPIHelperWin32> m_dpiHelper;
    std::vector<uint8_t> m_rawInputScratchpad;
    VirtualMouseButtonFlags m_rawMouseButtonFlags;
    Point2 m_cachedDPIValue;
    bool m_mouseCaptureEnabled;
    bool m_mouseInternalCaptureEnabled;
    bool m_mouseIsCaptured;
    bool m_mouseCursorIsClipped;
    bool m_mouseHideCursorEnabled;
    bool m_mouseIsCursorHidden;

  public:
    PlatformNativeWindowAdapterWin32(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& platformWindowParams,
                                     const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams);
    ~PlatformNativeWindowAdapterWin32() override;


    bool TryCaptureMouse(const bool enableCapture) override;

    void OnDPIChanged(const Point2& value);

    void OnRawInput(const std::shared_ptr<INativeWindowEventQueue>& eventQueue, const LPARAM lParam);
    void OnMouseMove(const std::shared_ptr<INativeWindowEventQueue>& eventQueue, const PxPoint2& position);
    void OnWindowCaptureChanged(const HWND newCaptureHwnd);

    void SYS_SetMouseCapture(const bool enableCapture);

    void ResolveMouseCapture();
    void ResolveMouseCursorClip();
    void ResolveMouseCursorVisible();

  protected:
    bool TryGetNativeSize(PxPoint2& rSize) const override;
    bool TryGetNativeDpi(Vector2& rDPI) const override;
  };
}    // namespace Fsl

#endif
#endif
