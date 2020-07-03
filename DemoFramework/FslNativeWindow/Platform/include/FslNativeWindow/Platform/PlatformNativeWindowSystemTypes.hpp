#ifndef FSLNATIVEWINDOW_PLATFORM_PLATFORMNATIVEWINDOWSYSTEMTYPES_HPP
#define FSLNATIVEWINDOW_PLATFORM_PLATFORMNATIVEWINDOWSYSTEMTYPES_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

// This file defines the platform dependent window types:

// Forward declares
// - PlatformNativeDisplayType
// - PlatformNativeWindowType

#if defined(_WIN32)
#include <windows.h>
#include <memory>
#include <utility>
namespace Fsl
{
  using PlatformNativeDisplayType = HINSTANCE;
  using PlatformNativeWindowType = HWND;

  struct PlatformNativeWindowSystemParams
  {
    // Its important that the default constructor exist as this is what we use for as a default parameter value
    PlatformNativeWindowSystemParams() = default;
  };

  class DPIHelperWin32;
  struct PlatformNativeWindowParams
  {
    PlatformNativeDisplayType PlatformDisplay;
    std::shared_ptr<DPIHelperWin32> DpiHelper;

    explicit PlatformNativeWindowParams(const PlatformNativeDisplayType& platformDisplay, std::shared_ptr<DPIHelperWin32> dpiHelper)
      : PlatformDisplay(platformDisplay)
      , DpiHelper(std::move(dpiHelper))
    {
    }
  };
}    // namespace Fsl
#elif defined(__ANDROID__)
#include <android_native_app_glue.h>
#include <FslNativeWindow/Platform/Android/PlatformNativeWindowAndroidCallbacks.hpp>

namespace Fsl
{
  typedef void* PlatformNativeDisplayType;
  typedef ANativeWindow* PlatformNativeWindowType;

  struct PlatformNativeWindowSystemParams
  {
    // Its important that the default constructor exist as this is what we use for as a default parameter value
    PlatformNativeWindowSystemParams()
    {
    }
  };

  struct PlatformNativeWindowParams
  {
    PlatformNativeDisplayType PlatformDisplay;
    android_app* AppState;
    PlatformCallbackNativeWindowAndroidOnWindowCreate OnWindowCreated;

    explicit PlatformNativeWindowParams(const PlatformNativeDisplayType& platformDisplay, android_app* pAppState,
                                        const PlatformCallbackNativeWindowAndroidOnWindowCreate& onWindowCreated)
      : PlatformDisplay(platformDisplay)
      , AppState(pAppState)
      , OnWindowCreated(onWindowCreated)
    {
    }
  };
}    // namespace Fsl
#elif defined(__QNXNTO__)
#include <screen/screen.h>

namespace Fsl
{
  typedef int PlatformNativeDisplayType;
  typedef screen_window_t PlatformNativeWindowType;

  struct PlatformNativeWindowSystemParams
  {
    // Its important that the default constructor exist as this is what we use for as a default parameter value
    PlatformNativeWindowSystemParams()
    {
    }
  };

  struct PlatformNativeWindowParams
  {
    PlatformNativeDisplayType PlatformDisplay;

    explicit PlatformNativeWindowParams(const PlatformNativeDisplayType& platformDisplay)
      : PlatformDisplay(platformDisplay)
    {
    }
  };
}    // namespace Fsl

#elif defined(__linux__)
#if defined(FSL_WINDOWSYSTEM_X11)
#include <X11/Xlib.h>
namespace Fsl
{
  using PlatformNativeDisplayType = Display*;
  using PlatformNativeWindowType = Window;

  struct PlatformNativeWindowSystemParams
  {
    // Its important that the default constructor exist as this is what we use for as a default parameter value
    PlatformNativeWindowSystemParams() = default;
  };

  struct PlatformNativeWindowParams
  {
    PlatformNativeDisplayType PlatformDisplay;
    int32_t VisualId;
    bool ExtensionRREnabled;

    explicit PlatformNativeWindowParams(const PlatformNativeDisplayType& platformDisplay, const int32_t visualId, const bool extensionRREnabled)
      : PlatformDisplay(platformDisplay)
      , VisualId(visualId)
      , ExtensionRREnabled(extensionRREnabled)
    {
    }
  };
}    // namespace Fsl
#elif defined(FSL_WINDOWSYSTEM_WAYLAND)
#include <wayland-client.h>
#include <FslNativeWindow/Platform/Wayland/PlatformNativeWindowWaylandCallbacks.hpp>
namespace Fsl
{
  // wl_display
  typedef wl_display* PlatformNativeDisplayType;
  // This needs to be defined, without EGL we do not support windows but would require stand alone buffers to work with.
  typedef void* PlatformNativeWindowType;

  struct PlatformNativeWindowSystemParams
  {
    // Its important that the default constructor exist as this is what we use for as a default parameter value
    PlatformNativeWindowSystemParams()
    {
    }
  };

  struct PlatformNativeWindowParams
  {
    PlatformNativeDisplayType PlatformDisplay;
    PlatformCallbackNativeWindowWaylandCreate CreateWaylandWindow;
    PlatformCallbackNativeWindowWaylandDestroy DestroyWaylandWindow;
    PlatformCallbackNativeWindowWaylandResize ResizeWaylandWindow;
    explicit PlatformNativeWindowParams(const PlatformNativeDisplayType& platformDisplay,
                                        const PlatformCallbackNativeWindowWaylandCreate& createWaylandWindow,
                                        const PlatformCallbackNativeWindowWaylandDestroy& destroyWaylandWindow,
                                        const PlatformCallbackNativeWindowWaylandResize& resizeWaylandWindow)
      : PlatformDisplay(platformDisplay)
      , CreateWaylandWindow(createWaylandWindow)
      , DestroyWaylandWindow(destroyWaylandWindow)
      , ResizeWaylandWindow(resizeWaylandWindow)
    {
    }
  };
}    // namespace Fsl
#elif defined(FSL_WINDOWSYSTEM_FRAMEBUFFER)
#include <FslNativeWindow/Platform/FB/PlatformNativeWindowFBCallbacks.hpp>
#include <FslNativeWindow/Platform/FB/PlatformNativeWindowSystemFBCallbacks.hpp>
namespace Fsl
{
  using PlatformNativeDisplayType = void*;
  using PlatformNativeWindowType = void*;

  struct PlatformNativeWindowSystemParams
  {
    PlatformNativeDisplayType Display;
    PlatformCallbackNativeWindowSystemFBDestroyDisplay DestroyDisplayFunc;

    // Its important that the default constructor exist as this is what we use for as a default parameter value
    PlatformNativeWindowSystemParams()
      : Display(nullptr)
      , DestroyDisplayFunc(nullptr)
    {
    }

    PlatformNativeWindowSystemParams(PlatformNativeDisplayType display,
                                     const PlatformCallbackNativeWindowSystemFBDestroyDisplay& destroyDisplayFunc = nullptr)
      : Display(display)
      , DestroyDisplayFunc(destroyDisplayFunc)
    {
    }
  };

  struct PlatformNativeWindowParams
  {
    PlatformNativeDisplayType PlatformDisplay;
    PlatformNativeWindowType PlatformWindow;
    PlatformCallbackNativeWindowFBDestroyWindow DestroyWindowFunc;

    explicit PlatformNativeWindowParams(const PlatformNativeDisplayType& platformDisplay, const PlatformNativeWindowType platformWindow = nullptr,
                                        const PlatformCallbackNativeWindowFBDestroyWindow& destroyWindowFunc = nullptr)
      : PlatformDisplay(platformDisplay)
      , PlatformWindow(platformWindow)
      , DestroyWindowFunc(destroyWindowFunc)
    {
    }
  };
}    // namespace Fsl
#else
#error NativeDisplayType, NativeWindowType and PlatformNativeWindowParams not defined for platform
#endif
#else
#error Unsupported platform
#endif


#endif
