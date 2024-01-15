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


#if defined(FSL_WINDOWSYSTEM_SDL2)
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#ifdef SDL_VIDEO_DRIVER_WINDOWS
namespace Fsl
{
  using PlatformNativeDisplayType = HINSTANCE;
  using PlatformNativeWindowType = HWND;
}
#elif defined(SDL_VIDEO_DRIVER_EMSCRIPTEN)
namespace Fsl
{
  // FIX: use the correct types here
  using PlatformNativeDisplayType = void*;
  using PlatformNativeWindowType = void*;
}
#elif defined(SDL_VIDEO_DRIVER_X11)
namespace Fsl
{
  using PlatformNativeDisplayType = Display*;
  using PlatformNativeWindowType = Window;
}
#elif defined(SDL_VIDEO_DRIVER_WAYLAND)
#include <wayland-client.h>
namespace Fsl
{
  // wl_display
  using PlatformNativeDisplayType = wl_display*;
  // This needs to be defined, without EGL we do not support windows but would require stand alone buffers to work with.
  using PlatformNativeWindowType = void*;
}
#else
#error Unsupported SDL platform
#endif
namespace Fsl
{
  struct PlatformNativeWindowSystemParams
  {
    bool UseExternalContext{false};

    // Its important that the default constructor exist as this is what we use for as a default parameter value
    PlatformNativeWindowSystemParams() = default;

    explicit PlatformNativeWindowSystemParams(const bool useExternalContext) noexcept
      : UseExternalContext(useExternalContext)
    {
    }
  };

  struct PlatformNativeWindowParams
  {
    PlatformNativeDisplayType PlatformDisplay;
    int WindowFlags{0};

    explicit PlatformNativeWindowParams(const PlatformNativeDisplayType platformDisplay, const int windowFlags)
      : PlatformDisplay(platformDisplay)

    {
    }
  };
}    // namespace Fsl

#elif defined(FSL_WINDOWSYSTEM_WIN32)
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
#include <FslNativeWindow/Platform/Adapter/Android/PlatformNativeWindowAndroidCallbacks.hpp>
#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace Fsl
{
  using PlatformNativeDisplayType = void*;
  using PlatformNativeWindowType = ANativeWindow*;

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
  using PlatformNativeDisplayType = int;
  using PlatformNativeWindowType = screen_window_t;

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
#include <FslNativeWindow/Platform/Adapter/Wayland/PlatformNativeWindowWaylandCallbacks.hpp>
#include <wayland-client.h>
#include <memory>
#include <utility>
namespace Fsl
{
  // wl_display
  using PlatformNativeDisplayType = wl_display*;
  // This needs to be defined, without EGL we do not support windows but would require stand alone buffers to work with.
  using PlatformNativeWindowType = void*;

  struct PlatformNativeWindowSystemParams
  {
    // Its important that the default constructor exist as this is what we use for as a default parameter value
    PlatformNativeWindowSystemParams() = default;
  };

  class PlatformNativeWindowSystemContextWayland;

  struct PlatformNativeWindowParams
  {
    std::weak_ptr<PlatformNativeWindowSystemContextWayland> WindowSystemWaylandContext;
    PlatformNativeDisplayType PlatformDisplay;
    PlatformCallbackNativeWindowWaylandCreate CreateWaylandWindow;
    PlatformCallbackNativeWindowWaylandDestroy DestroyWaylandWindow;
    PlatformCallbackNativeWindowWaylandResize ResizeWaylandWindow;

    explicit PlatformNativeWindowParams(std::weak_ptr<PlatformNativeWindowSystemContextWayland> windowSystemWaylandContext,
                                        const PlatformNativeDisplayType& platformDisplay,
                                        PlatformCallbackNativeWindowWaylandCreate createWaylandWindow,
                                        PlatformCallbackNativeWindowWaylandDestroy destroyWaylandWindow,
                                        PlatformCallbackNativeWindowWaylandResize resizeWaylandWindow)
      : WindowSystemWaylandContext(std::move(windowSystemWaylandContext))
      , PlatformDisplay(platformDisplay)
      , CreateWaylandWindow(std::move(createWaylandWindow))
      , DestroyWaylandWindow(std::move(destroyWaylandWindow))
      , ResizeWaylandWindow(std::move(resizeWaylandWindow))
    {
    }
    ~PlatformNativeWindowParams();
  };
}    // namespace Fsl
#elif defined(FSL_WINDOWSYSTEM_FRAMEBUFFER)
#include <FslNativeWindow/Platform/Adapter/FB/PlatformNativeWindowFBCallbacks.hpp>
#include <FslNativeWindow/Platform/Adapter/FB/PlatformNativeWindowSystemFBCallbacks.hpp>
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
