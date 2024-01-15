#ifndef FSLNATIVEWINDOW_PLATFORM_ADAPTER_WAYLAND_PLATFORMNATIVEWINDOWSYSTEMCONTEXTWAYLAND_HPP
#define FSLNATIVEWINDOW_PLATFORM_ADAPTER_WAYLAND_PLATFORMNATIVEWINDOWSYSTEMCONTEXTWAYLAND_HPP
#if !defined(__ANDROID__) && defined(__linux__) && !defined(FSL_WINDOWSYSTEM_X11) && defined(FSL_WINDOWSYSTEM_WAYLAND) && \
  defined(FSL_WINDOWSYSTEM_WAYLAND_XDG)
/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslNativeWindow/Base/INativeWindowEventQueue.hpp>
#include <FslNativeWindow/Base/VirtualMouseButton.hpp>
#include <FslNativeWindow/Platform/PlatformNativeWindowSystemTypes.hpp>
#include <wayland-client.h>
#include <wayland-cursor.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "IVI/PlatformNativeWindowWaylandHandlesIVI.hpp"
#include "WaylandRAII.hpp"
#include "WaylandUtil.hpp"

namespace Fsl
{
  struct PlatformNativeWindowWaylandHandles
  {
    PlatformNativeWindowWaylandHandles(const PlatformNativeWindowWaylandHandles&) = delete;
    PlatformNativeWindowWaylandHandles& operator=(const PlatformNativeWindowWaylandHandles&) = delete;
    PlatformNativeWindowWaylandHandles(PlatformNativeWindowWaylandHandles&& other) noexcept = delete;
    PlatformNativeWindowWaylandHandles& operator=(PlatformNativeWindowWaylandHandles&& other) noexcept = delete;

    ScopedWaylandSurface Surface;
    ScopedWaylandXdgSurface XdgSurface;
    ScopedWaylandXdgToplevel XdgToplevel;
    PlatformNativeWindowWaylandHandlesIVI Ivi;

    PlatformNativeWindowWaylandHandles() = default;

    void reset() noexcept
    {
      // Destroy the members in destruction order
      Ivi.reset();
      XdgToplevel.reset();
      XdgSurface.reset();
      Surface.reset();
    }
  };

  class PlatformNativeWindowContextWayland
  {
  public:
    std::weak_ptr<INativeWindowEventQueue> EventQueue;

    PxSize2D Geometry{};
    //! This is the originally requested window size
    PxSize2D DesiredWindowSize{};

    PlatformNativeWindowWaylandHandles Handles;

    PlatformNativeWindowType Native{};

    // ScopedWaylandCallback Callback;
    bool Fullscreen{false};
    bool Maximized{false};
    bool Configured{false};
    bool WaitForConfigure{true};
    // bool RequestClose{false};
    std::function<void(void*, int, int, int, int)> ResizeWindowCallback;


    explicit PlatformNativeWindowContextWayland(std::weak_ptr<INativeWindowEventQueue> eventQueue)
      : EventQueue(std::move(eventQueue))
    {
    }
  };


  struct GlobalInfo
  {
    uint32_t Id{0};
    uint32_t Version{0};
    std::string Interface;

    GlobalInfo() = default;
    GlobalInfo(const uint32_t id, const uint32_t version, std::string interface)
      : Id(id)
      , Version(version)
      , Interface(std::move(interface))
    {
    }
  };

  struct OutputModeRecord
  {
    uint32_t Flags{0};
    int32_t Width{0};
    int32_t Height{0};
    int32_t Refresh{0};
  };

  struct OutputGeometryInfo
  {
    int32_t X{0};
    int32_t Y{0};
    int32_t Scale{1};
    int32_t PhysicalWidth{0};
    int32_t PhysicalHeight{0};
    wl_output_subpixel Subpixel{WL_OUTPUT_SUBPIXEL_UNKNOWN};
    wl_output_transform OutputTransform{WL_OUTPUT_TRANSFORM_NORMAL};
    std::string Make;
    std::string Model;
  };


  struct OutputInfo
  {
    GlobalInfo Global;
    uint32_t Version{0};

    ScopedWaylandOutput Output;
    OutputGeometryInfo Geometry;

    std::vector<OutputModeRecord> Modes;

    OutputInfo() = default;
    OutputInfo(GlobalInfo global, const uint32_t version)
      : Global(std::move(global))
      , Version(std::min(version, uint32_t{2}))
    {
    }
  };

  struct PlatformNativeWindowSystemWaylandHandles
  {
    PlatformNativeWindowSystemWaylandHandles(const PlatformNativeWindowSystemWaylandHandles&) = delete;
    PlatformNativeWindowSystemWaylandHandles& operator=(const PlatformNativeWindowSystemWaylandHandles&) = delete;
    PlatformNativeWindowSystemWaylandHandles(PlatformNativeWindowSystemWaylandHandles&& other) noexcept = delete;
    PlatformNativeWindowSystemWaylandHandles& operator=(PlatformNativeWindowSystemWaylandHandles&& other) noexcept = delete;

    ScopedWaylandDisplay Display;
    wl_registry* Registry{nullptr};    // Does not need to be released
    ScopedWaylandCompositor Compositor;
    ScopedWaylandXdgWmBase WmBase;
    ScopedWaylandSeat Seat;
    ScopedWaylandShm Shm;
    ScopedWaylandCursorTheme CursorTheme;
    wl_cursor* DefaultCursor{nullptr};    // Does not need to be released
    ScopedWaylandPointer Pointer;
    ScopedWaylandKeyboard Keyboard;
    ScopedWaylandSurface CursorSurface;
    ScopedWaylandXdgDecorationManagerV1 DecorationManager;
    PlatformNativeWindowSystemWaylandHandlesIVI Ivi;

    PlatformNativeWindowSystemWaylandHandles() = default;

    // void reset()
    // {
    //   // Destroy the members in destructor order
    //     Ivi.reset();
    //     CursorSurface.reset();
    //     Keyboard.reset();
    //     Pointer.reset();
    //     DefaultCursor = nullptr;
    //     CursorTheme.reset();
    //     Shm.reset();
    //     Seat.reset();
    //     WmBase.reset();
    //     Compositor.reset();
    //     Registry = nullptr;
    //     Display.reset();
    // }
  };


  //! Beware the Scoped objects are listed in order for creation/destruction. So do not move Scoped members around unless you have a very good
  //! reason.
  class PlatformNativeWindowSystemContextWayland
  {
    bool m_isShutdown{false};

  public:
    std::weak_ptr<INativeWindowEventQueue> EventQueue;

    PlatformNativeWindowSystemWaylandHandles Handles;


    PxPoint2 MousePosition;
    int ZDelta{0};
    VirtualMouseButton::Enum MouseButton{};
    bool MouseIsPressed{false};

    bool RoundtripNeeded{false};

    PlatformNativeWindowContextWayland* Window{nullptr};
    std::vector<std::unique_ptr<OutputInfo>> Outputs;

    explicit PlatformNativeWindowSystemContextWayland(std::weak_ptr<INativeWindowEventQueue> eventQueue)
      : EventQueue(std::move(eventQueue))
    {
    }

    bool IsShutdown() const
    {
      return m_isShutdown;
    }

    void MarkAsShutdown()
    {
      m_isShutdown = true;
    }
  };
}

#endif
#endif
