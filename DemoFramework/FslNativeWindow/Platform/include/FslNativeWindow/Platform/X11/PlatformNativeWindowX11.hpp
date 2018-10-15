#ifndef FSLNATIVEWINDOW_PLATFORM_X11_PLATFORMNATIVEWINDOWX11_HPP
#define FSLNATIVEWINDOW_PLATFORM_X11_PLATFORMNATIVEWINDOWX11_HPP
#if !defined(__ANDROID__) && defined(__linux__) && defined(FSL_WINDOWSYSTEM_X11)
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

#include <FslNativeWindow/Platform/PlatformNativeWindow.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <memory>

namespace Fsl
{
  class NativeWindowSetup;
  class INativeWindowEventQueue;

  class PlatformNativeWindowX11 : public PlatformNativeWindow
  {
    XVisualInfo* m_pVisual;
    Colormap m_colormap;
    Point2 m_cachedWindowSize;
    Point2 m_cachedScreenDPI;

  public:
    Atom m_wm_delete_window;

    PlatformNativeWindowX11(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& platformWindowParams,
                            const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams);
    ~PlatformNativeWindowX11() override;

    bool TryGetDPI(Vector2& rDPI) const override;
    bool TryGetSize(Point2& rSize) const override;
    bool TryCaptureMouse(const bool enableCapture) override
    {
      return false;
    }

    void OnConfigureNotify(const XConfigureEvent& event, const std::shared_ptr<INativeWindowEventQueue>& eventQueue);
    void OnRRScreenChangeNotify(XEvent* pEvent, const std::shared_ptr<INativeWindowEventQueue>& eventQueue);
  };
}    // namespace Fsl

#endif
#endif
