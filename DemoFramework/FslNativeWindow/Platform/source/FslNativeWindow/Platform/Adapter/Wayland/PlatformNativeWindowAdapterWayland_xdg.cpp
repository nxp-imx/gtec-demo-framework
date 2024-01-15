#if !defined(__ANDROID__) && defined(__linux__) && !defined(FSL_WINDOWSYSTEM_X11) && defined(FSL_WINDOWSYSTEM_WAYLAND) && \
  defined(FSL_WINDOWSYSTEM_WAYLAND_XDG)
/*
 * Copyright (C) 2011 Benjamin Franzke
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission. The copyright holders make no representations
 * about the suitability of this software for any purpose. It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

// Wayland code adapted for the DemoFramework by Freescale 2014


#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/LogRectangle.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxPoint2.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxSize2D.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/Base/NativeWindowSetup.hpp>
#include <FslNativeWindow/Base/NativeWindowSystemSetup.hpp>
#include <FslNativeWindow/Platform/Adapter/Wayland/PlatformNativeWindowAdapterWayland.hpp>
#include <FslNativeWindow/Platform/Adapter/Wayland/PlatformNativeWindowSystemAdapterWayland.hpp>
#include <df-xdg-decoration-client-protocol.h>    // XDG wayland-scanner created header
#include <df-xdg-shell-client-protocol.h>         // XDG wayland-scanner created header
#include <linux/input.h>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "IVI/WaylandIVIHandler.hpp"
#include "PlatformNativeWindowSystemContextWayland.hpp"

// XDG shell decided to break C/C++ library support conventions and require us to
// run their wayland-scanner tool to generate non-app dependent headers and code.
// If they had gone for the normal semantic versioned headers the build process would
// be much simpler for all end users and much less error prone.

// TODO: remove globals

// https://gitlab.freedesktop.org/wayland/wayland/-/issues/34
// Workaround for "wl_array_for_each" not being C++ safe (or safe in general) and wayland failing to fix it for 3+ years
// at this point
#define LOCAL_WL_ARRAY_FOR_EACH(pos, array, type) \
  for (pos = (type)(array)->data; (const char*)pos < ((const char*)(array)->data + (array)->size); (pos)++)

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const char* const Title = "FSL Framework";
      constexpr int MinWindowWidth = 64;
      constexpr int MinWindowHeight = 64;
    }

    bool g_requestClose = false;


    // void ConfigureWindowGeometry(PlatformNativeWindowContextWayland& rWindow)
    // {
    //   // bool windowSizeChanged = true;
    //   // if(windowSizeChanged)
    //   // {

    //     // if (rWindow.Handles.Surface && rWindow.Handles.XdgSurface)
    //     // {
    //       // rWindow.WindowSize = rWindow.Geometry;
    //       // FSLLOG3_VERBOSE5("ConfigureWindowGeometry: New window size:{}", rWindow.WindowSize);
    //       // xdg_surface_set_window_geometry(rWindow.Handles.XdgSurface.get(), 0, 0, rWindow.WindowSize.RawWidth(),
    //       rWindow.WindowSize.RawHeight());
    //     // }
    //   // }
    // }

    void OnWaylandWindowContext_HandleSurfaceEnter(void* data, wl_surface* pSurface, wl_output* pOutput)
    {
      FSLLOG3_VERBOSE5("OnWaylandWindowContext_HandleSurfaceEnter");
    }

    void OnWaylandWindowContext_HandleSurfaceLeave(void* data, wl_surface* pSurface, wl_output* pOutput)
    {
      FSLLOG3_VERBOSE5("OnWaylandWindowContext_HandleSurfaceLeave");
    }

    // void OnWaylandWindowContext_HandlePreferredBufferScale(void *data, wl_surface *wl_surface, int32_t factor)
    // {
    // }
    // void OnWaylandWindowContext_HandlePreferredBufferTransform(void *data, wl_surface *wl_surface, uint32_t transform)
    // {
    // }

    const wl_surface_listener g_surfaceListener = {
      OnWaylandWindowContext_HandleSurfaceEnter, OnWaylandWindowContext_HandleSurfaceLeave,
      // OnWaylandWindowContext_HandlePreferredBufferScale,
      // OnWaylandWindowContext_HandlePreferredBufferTransform
    };


    void OnWaylandWindowContext_ConfigureCallback(void* data, wl_callback* callback, uint32_t time)
    {
      auto* window = static_cast<PlatformNativeWindowContextWayland*>(data);
      wl_callback_destroy(callback);
      window->Configured = true;
    }


    const wl_callback_listener g_configureCallBackListener = {
      OnWaylandWindowContext_ConfigureCallback,
    };


    void OnWaylandSystemContext_XdgWmBaseHandlePing(void* data, xdg_wm_base* shell, uint32_t serial)
    {
      xdg_wm_base_pong(shell, serial);
    }


    void OnWaylandWindowContext_XdgHandleToplevelConfigure(void* data, xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states)
    {
      auto* pWindow = static_cast<PlatformNativeWindowContextWayland*>(data);
      if (pWindow == nullptr)
      {
        return;
      }

      pWindow->Fullscreen = false;
      pWindow->Maximized = false;

      const uint32_t* p = nullptr;
      LOCAL_WL_ARRAY_FOR_EACH(p, states, uint32_t*)
      {
        uint32_t state = *p;
        switch (state)
        {
        case XDG_TOPLEVEL_STATE_FULLSCREEN:
          pWindow->Fullscreen = true;
          break;
        case XDG_TOPLEVEL_STATE_MAXIMIZED:
          pWindow->Maximized = true;
          break;
        // case XDG_TOPLEVEL_STATE_ACTIVATED:
        // case XDG_TOPLEVEL_STATE_SUSPENDED
        default:
          break;
        }
      }

      FSLLOG3_VERBOSE5(
        "OnWaylandWindowContext_XdgHandleToplevelConfigure width {} height {} Fullscreen {} Maximized {}, current active config geometry: {}", width,
        height, pWindow->Fullscreen, pWindow->Maximized, pWindow->Geometry);

      if (width <= 0)
      {
        width = pWindow->DesiredWindowSize.RawWidth();
        FSLLOG3_VERBOSE5("Setting width to desired width: {}", width)
      }
      if (height <= 0)
      {
        height = pWindow->DesiredWindowSize.RawHeight();
        FSLLOG3_VERBOSE5("Setting height to desired height {}", height)
      }

      const PxSize2D originalSizePx = pWindow->Geometry;

      // Apply window size constraints
      width = std::max(width, LocalConfig::MinWindowWidth);
      height = std::max(height, LocalConfig::MinWindowHeight);

      pWindow->Geometry = PxSize2D::Create(width, height);

      const bool resized = originalSizePx != pWindow->Geometry;
      FSLLOG3_VERBOSE5("OnWaylandWindowContext_XdgHandleToplevelConfigure result: resized: {} geometry {}", resized, pWindow->Geometry);

      if (resized && pWindow->Native != nullptr && pWindow->ResizeWindowCallback)
      {
        pWindow->ResizeWindowCallback(pWindow->Native, width, height, 0, 0);
      }

      if (resized)
      {    // Let the framework know that we might have been resized
        std::shared_ptr<INativeWindowEventQueue> eventQueue = pWindow->EventQueue.lock();
        if (eventQueue)
        {
          eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowResizedEvent());
        }
      }
    }

    void OnWaylandWindowContext_XdgSurfaceHandleSurfaceConfigure(void* data, xdg_surface* surface, uint32_t serial)
    {
      FSLLOG3_VERBOSE5("OnWaylandWindowContext_XdgSurfaceHandleSurfaceConfigure");
      auto* pWindow = static_cast<PlatformNativeWindowContextWayland*>(data);
      if (pWindow != nullptr)
      {
        // ConfigureWindowGeometry(*pWindow);
        pWindow->WaitForConfigure = false;
      }
      xdg_surface_ack_configure(surface, serial);
    }

    void OnWaylandWindowContext_XdgHandleToplevelClose(void* data, xdg_toplevel* xdgToplevel)
    {
      FSLLOG3_VERBOSE5("OnWaylandWindowContext_XdgHandleToplevelClose");
      // auto* pWindow = static_cast<PlatformNativeWindowContextWayland*>(data);
      // if(pWindow != nullptr)
      // {
      //   pWindow->RequestClose = true;
      // }
      g_requestClose = true;
    }

    const xdg_wm_base_listener g_wmBaseListener = {
      OnWaylandSystemContext_XdgWmBaseHandlePing,
    };

    const xdg_surface_listener g_xdgSurfaceListener = {OnWaylandWindowContext_XdgSurfaceHandleSurfaceConfigure};

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

    const xdg_toplevel_listener g_XdgToplevelListener = {
      OnWaylandWindowContext_XdgHandleToplevelConfigure,
      OnWaylandWindowContext_XdgHandleToplevelClose,
    };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

    void CreateWlSurface(const PlatformNativeWindowSystemContextWayland& context, PlatformNativeWindowContextWayland& rWindow)
    {
      try
      {
        // Ensure no pre-existing handles are valid
        rWindow.Handles.reset();

        rWindow.Handles.Surface.reset(wl_compositor_create_surface(context.Handles.Compositor.get()));
        if (!rWindow.Handles.Surface)
        {
          throw GraphicsException("wl_compositor_create_surface Failure");
        }
        wl_surface_add_listener(rWindow.Handles.Surface.get(), &g_surfaceListener, &rWindow);


        if (!context.Handles.Ivi.Enabled)
        {
          if (context.Handles.WmBase)
          {
            rWindow.Handles.XdgSurface.reset(xdg_wm_base_get_xdg_surface(context.Handles.WmBase.get(), rWindow.Handles.Surface.get()));
            if (!rWindow.Handles.XdgSurface)
            {
              throw GraphicsException("xdg_wm_base_get_shell_surface Failure");
            }

            if (xdg_surface_add_listener(rWindow.Handles.XdgSurface.get(), &g_xdgSurfaceListener, &rWindow) != 0)
            {
              throw GraphicsException("xdg_surface_add_listener Failure");
            }

            rWindow.Handles.XdgToplevel.reset(xdg_surface_get_toplevel(rWindow.Handles.XdgSurface.get()));
            if (!rWindow.Handles.XdgToplevel)
            {
              throw GraphicsException("xdg_surface_get_toplevel Failure");
            }

            if (xdg_toplevel_add_listener(rWindow.Handles.XdgToplevel.get(), &g_XdgToplevelListener, &rWindow) != 0)
            {
              throw GraphicsException("xdg_toplevel_add_listener Failure");
            };

            xdg_toplevel_set_title(rWindow.Handles.XdgToplevel.get(), LocalConfig::Title);

            if (context.Handles.DecorationManager)
            {
              FSLLOG3_VERBOSE5("Nice 'Wayland XDG Decorations' support found, requesting server side window decorations.");
              // Let the compositor do all the complicated window management
              zxdg_toplevel_decoration_v1* decoration =
                zxdg_decoration_manager_v1_get_toplevel_decoration(context.Handles.DecorationManager.get(), rWindow.Handles.XdgToplevel.get());
              if (decoration != nullptr)
              {
                zxdg_toplevel_decoration_v1_set_mode(decoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
              }
            }
            else
            {
              // Unfortunately wayland window managers are not required to support XDG Decorations.
              // This does not really respect cross platform app developers time and it makes little sense that each app has to provide a custom
              // window decoration implementation for all possible wayland backends that the app need to run on just to match the native look and feel
              // of the current wayland backend. It makes more sense solving it "once per wayland backend" instead of "once per app per wayland
              // backend" especially since there are millions of apps vs a few wayland backends.
              FSLLOG3_INFO_IF(!rWindow.Fullscreen,
                              "Your 'Wayland window manager' does not support 'XDG Decorations' so no window decorations will be shown.");
            }

            // xdg_surface_set_window_geometry(rWindow.Handles.XdgSurface.get(), 0, 0, rWindow.DesiredWindowSize.RawWidth(),
            // rWindow.DesiredWindowSize.RawHeight());

            rWindow.WaitForConfigure = true;
            wl_surface_commit(rWindow.Handles.Surface.get());
            if (rWindow.Fullscreen)
            {
              xdg_toplevel_set_fullscreen(rWindow.Handles.XdgToplevel.get(), nullptr);
            }
          }

          wl_callback* callback = wl_display_sync(context.Handles.Display.get());
          if (callback == nullptr)
          {
            throw GraphicsException("wl_display_sync Failure");
          }

          if (wl_callback_add_listener(callback, &g_configureCallBackListener, &rWindow) == -1)
          {
            throw GraphicsException("wl_display_sync Failure");
          }

          while (rWindow.WaitForConfigure)
          {
            wl_display_dispatch(context.Handles.Display.get());
          }
        }
        else
        {
          WaylandIVIHandler::Create(context, rWindow);
        }
      }
      catch (const std::exception&)
      {
        rWindow.Handles.reset();
        throw;
      }
    }


    //! Used to extract window dimensions
    // void CreateWlDummySurface(const PlatformNativeWindowSystemContextWayland& context, PlatformNativeWindowContextWayland& rWindow)
    // {
    //   CreateWlSurface(context, rWindow);

    //   while (!rWindow.Configured)
    //   {
    //     wl_display_dispatch(context.Handles.Display.get());
    //   }
    //   if (rWindow.Fullscreen && context.Handles.WmBase)
    //   {
    //     rWindow.Handles.reset();
    //     rWindow.Fullscreen = false;
    //     wl_display_dispatch(context.Handles.Display.get());
    //   }
    // }


    void OnWaylandSystemContext_PointerHandleEnter(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t sx,
                                                   wl_fixed_t sy)
    {
      auto* pContext = static_cast<PlatformNativeWindowSystemContextWayland*>(data);
      assert(pContext != nullptr);
      wl_buffer* buffer = nullptr;
      wl_cursor* cursor = pContext->Handles.DefaultCursor;
      wl_cursor_image* image = nullptr;


      if (pContext->Window->Fullscreen)
      {
        wl_pointer_set_cursor(pointer, serial, nullptr, 0, 0);
      }
      else if (cursor != nullptr)
      {
        image = pContext->Handles.DefaultCursor->images[0];
        buffer = wl_cursor_image_get_buffer(image);
        wl_pointer_set_cursor(pointer, serial, pContext->Handles.CursorSurface.get(), UncheckedNumericCast<int32_t>(image->hotspot_x),
                              UncheckedNumericCast<int32_t>(image->hotspot_y));
        wl_surface_attach(pContext->Handles.CursorSurface.get(), buffer, 0, 0);
        wl_surface_damage(pContext->Handles.CursorSurface.get(), 0, 0, UncheckedNumericCast<int32_t>(image->width),
                          UncheckedNumericCast<int32_t>(image->height));
        wl_surface_commit(pContext->Handles.CursorSurface.get());
      }

      pContext->MousePosition = PxPoint2::Create(sx / 256, sy / 256);
    }


    void OnWaylandSystemContext_PointerHandleLeave(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface)
    {
    }


    void OnWaylandSystemContext_PointerHandleMotion(void* data, wl_pointer* pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
    {
      auto* pContext = static_cast<PlatformNativeWindowSystemContextWayland*>(data);
      assert(pContext != nullptr);

      std::shared_ptr<INativeWindowEventQueue> eventQueue = pContext->EventQueue.lock();

      pContext->MousePosition = PxPoint2::Create(sx / 256, sy / 256);
      if (eventQueue)
      {
        eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputMouseMoveEvent(pContext->MousePosition));
      }
    }


    void OnWaylandSystemContext_PointerHandleButton(void* data, wl_pointer* wl_pointer, uint32_t serial, uint32_t time, uint32_t button,
                                                    uint32_t state)
    {
      auto* pContext = static_cast<PlatformNativeWindowSystemContextWayland*>(data);
      assert(pContext != nullptr);
      std::shared_ptr<INativeWindowEventQueue> eventQueue = pContext->EventQueue.lock();

      if (WL_POINTER_BUTTON_STATE_PRESSED == state)
      {
        pContext->MouseIsPressed = true;
        if (button == BTN_LEFT)
        {
          pContext->MouseButton = VirtualMouseButton::Left;
        }
        else if (button == BTN_RIGHT)
        {
          pContext->MouseButton = VirtualMouseButton::Right;
        }
        else if (button == BTN_MIDDLE)
        {
          pContext->MouseButton = VirtualMouseButton::Middle;
        }
      }
      else
      {
        pContext->MouseIsPressed = false;
      }
      if (eventQueue)
      {
        eventQueue->PostEvent(
          NativeWindowEventHelper::EncodeInputMouseButtonEvent(pContext->MouseButton, pContext->MouseIsPressed, pContext->MousePosition));
      }
    }


    void OnWaylandSystemContext_PointerHandleAxis(void* data, wl_pointer* wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
    {
      auto* pContext = static_cast<PlatformNativeWindowSystemContextWayland*>(data);
      assert(pContext != nullptr);
      std::shared_ptr<INativeWindowEventQueue> eventQueue = pContext->EventQueue.lock();

      pContext->ZDelta = static_cast<int>(value) / 2560;
      if (eventQueue)
      {
        eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputMouseWheelEvent(pContext->ZDelta, pContext->MousePosition));
      }
    }

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

    const wl_pointer_listener g_pointerListener = {
      OnWaylandSystemContext_PointerHandleEnter,  OnWaylandSystemContext_PointerHandleLeave, OnWaylandSystemContext_PointerHandleMotion,
      OnWaylandSystemContext_PointerHandleButton, OnWaylandSystemContext_PointerHandleAxis,
    };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


    void OnWaylandSystemContext_KeyboardHandleKeymap(void* data, wl_keyboard* keyboard, uint32_t format, int fd, uint32_t size)
    {
    }


    void OnWaylandSystemContext_KeyboardHandleEnter(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface, wl_array* keys)
    {
    }


    void OnWaylandSystemContext_KeyboardHandleLeave(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface)
    {
    }


    void OnWaylandSystemContext_KeyboardHandleKey(void* data, wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
    {
      auto* pContext = static_cast<PlatformNativeWindowSystemContextWayland*>(data);
      assert(pContext != nullptr);
      std::shared_ptr<INativeWindowEventQueue> eventQueue = pContext->EventQueue.lock();
      if (eventQueue)
      {
        // Try to translate the keykode and post it if successfull
        const VirtualKey::Enum virtualKey = WaylandUtil::TryToVirtualKey(key);
        if (virtualKey != VirtualKey::Undefined)
        {
          // Post the event
          const bool isPressed = state != 0;
          eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputKeyEvent(virtualKey, isPressed));
        }
      }
    }


    void OnWaylandSystemContext_KeyboardHandleModifiers(void* data, wl_keyboard* keyboard, uint32_t serial, uint32_t mods_depressed,
                                                        uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
    {
    }

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

    const wl_keyboard_listener g_keyboardListener = {
      OnWaylandSystemContext_KeyboardHandleKeymap, OnWaylandSystemContext_KeyboardHandleEnter,     OnWaylandSystemContext_KeyboardHandleLeave,
      OnWaylandSystemContext_KeyboardHandleKey,    OnWaylandSystemContext_KeyboardHandleModifiers,
    };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#if 0
    void PrintGlobalInfo(const GlobalInfo& global)
    {
      FSLLOG3_VERBOSE5("interface: {}, Version: {}, name: {}", global.Interface, global.Version, global.Id);
    }

    void PrintOutputInfo(const OutputInfo& output)
    {
      const char* subpixel_orientation = nullptr;
      const char* transform = nullptr;

      PrintGlobalInfo(output.Global);

      switch (output.Geometry.Subpixel)
      {
      case WL_OUTPUT_SUBPIXEL_UNKNOWN:
        subpixel_orientation = "unknown";
        break;
      case WL_OUTPUT_SUBPIXEL_NONE:
        subpixel_orientation = "none";
        break;
      case WL_OUTPUT_SUBPIXEL_HORIZONTAL_RGB:
        subpixel_orientation = "horizontal rgb";
        break;
      case WL_OUTPUT_SUBPIXEL_HORIZONTAL_BGR:
        subpixel_orientation = "horizontal bgr";
        break;
      case WL_OUTPUT_SUBPIXEL_VERTICAL_RGB:
        subpixel_orientation = "vertical rgb";
        break;
      case WL_OUTPUT_SUBPIXEL_VERTICAL_BGR:
        subpixel_orientation = "vertical bgr";
        break;
      default:
        FSLLOG3_WARNING("unknown subpixel orientation {}", static_cast<uint64_t>(output.Geometry.Subpixel));
        subpixel_orientation = "unexpected value";
        break;
      }

      switch (output.Geometry.OutputTransform)
      {
      case WL_OUTPUT_TRANSFORM_NORMAL:
        transform = "normal";
        break;
      case WL_OUTPUT_TRANSFORM_90:
        transform = "90";
        break;
      case WL_OUTPUT_TRANSFORM_180:
        transform = "180";
        break;
      case WL_OUTPUT_TRANSFORM_270:
        transform = "270";
        break;
      case WL_OUTPUT_TRANSFORM_FLIPPED:
        transform = "flipped";
        break;
      case WL_OUTPUT_TRANSFORM_FLIPPED_90:
        transform = "flipped 90";
        break;
      case WL_OUTPUT_TRANSFORM_FLIPPED_180:
        transform = "flipped 180";
        break;
      case WL_OUTPUT_TRANSFORM_FLIPPED_270:
        transform = "flipped 270";
        break;
      default:
        FSLLOG3_WARNING("unknown output transform {}", static_cast<uint64_t>(output.Geometry.OutputTransform));
        transform = "unexpected value";
        break;
      }

      FSLLOG3_INFO("x: {}, y: {}", output.Geometry.X, output.Geometry.Y);
      if (output.Version >= 2)
      {
        FSLLOG3_INFO("scale: {}", output.Geometry.Scale);
      }

      FSLLOG3_INFO("physical_width: {} mm, physical_height: {} mm", output.Geometry.PhysicalWidth, output.Geometry.PhysicalHeight);
      FSLLOG3_INFO("make: {}, model: {}", output.Geometry.Make, output.Geometry.Model);
      FSLLOG3_INFO("subpixel_orientation: {}, output_transform: {}", subpixel_orientation, transform);

      for (const auto& mode : output.Modes)
      {
        FSLLOG3_INFO("mode:");

        FSLLOG3_INFO("width: {} px, height: {} px, refresh: {} Hz", mode.Width, mode.Height, (float)mode.Refresh / 1000);
        FSLLOG3_INFO("flags:");
        if ((mode.Flags & WL_OUTPUT_MODE_CURRENT) != 0u)
          FSLLOG3_INFO(" current");
        if ((mode.Flags & WL_OUTPUT_MODE_PREFERRED) != 0u)
          FSLLOG3_INFO(" preferred");
      }
    }
#endif

    void OnWaylandOutputContext_HandleGeometry(void* data, wl_output* wl_output, int32_t x, int32_t y, int32_t physicalWidth, int32_t physicalHeight,
                                               int32_t subpixel, const char* make, const char* model, int32_t output_transform)
    {
      auto* output = static_cast<OutputInfo*>(data);
      assert(output != nullptr);

      FSLLOG3_VERBOSE5("OnWaylandOutputContext_HandleGeometry: x:{} y:{} physicalWidth:{} physicalHeight:{} subpixel:{} output_transform:{}", x, y,
                       physicalWidth, physicalHeight, subpixel, output_transform);

      output->Geometry.X = x;
      output->Geometry.Y = y;
      output->Geometry.PhysicalWidth = physicalWidth;
      output->Geometry.PhysicalHeight = physicalHeight;
      output->Geometry.Subpixel = static_cast<wl_output_subpixel>(subpixel);
      output->Geometry.Make = make;
      output->Geometry.Model = model;
      output->Geometry.OutputTransform = static_cast<wl_output_transform>(output_transform);
    }

    void OnWaylandOutputContext_HandleMode(void* data, wl_output* wl_output, uint32_t flags, int32_t width, int32_t height, int32_t refresh)
    {
      auto* output = static_cast<OutputInfo*>(data);
      assert(output != nullptr);

      FSLLOG3_VERBOSE5("OnWaylandOutputContext_HandleMode");

      OutputModeRecord mode;
      mode.Flags = flags;
      mode.Width = width;
      mode.Height = height;
      mode.Refresh = refresh;

      output->Modes.push_back(mode);
    }

    void OnWaylandOutputContext_HandleDone(void* data, wl_output* wl_output)
    {
      // don't bother waiting for this; there's no good reason a
      // compositor will wait more than one roundtrip before sending
      // these initial events.
    }

    void OnWaylandOutputContext_HandleScale(void* data, wl_output* wl_output, int32_t scale)
    {
      FSLLOG3_VERBOSE5("OnWaylandOutputContext_HandleScale: {}", scale);

      auto* output = static_cast<OutputInfo*>(data);
      if (output != nullptr)
      {
        output->Geometry.Scale = scale;
      }
    }

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

    const wl_output_listener g_outputListener = {
      OnWaylandOutputContext_HandleGeometry,
      OnWaylandOutputContext_HandleMode,
      OnWaylandOutputContext_HandleDone,
      OnWaylandOutputContext_HandleScale,
    };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


    void AddOutputInfo(PlatformNativeWindowSystemContextWayland& rContext, const uint32_t id, const uint32_t version)
    {
      auto output = std::make_unique<OutputInfo>(GlobalInfo(id, version, wl_output_interface.name), version);
      FSLLOG3_VERBOSE5("Allocated Space for OutputInfo");

      output->Output.reset(static_cast<wl_output*>(wl_registry_bind(rContext.Handles.Registry, id, &wl_output_interface, output->Version)));
      wl_output_add_listener(output->Output.get(), &g_outputListener, output.get());

      rContext.RoundtripNeeded = true;
      rContext.Outputs.push_back(std::move(output));
    }


    void OnWaylandSystemContext_SeatHandleCapabilities(void* data, wl_seat* seat, uint32_t caps)
    {
      auto* pContext = static_cast<PlatformNativeWindowSystemContextWayland*>(data);

      if (((caps & WL_SEAT_CAPABILITY_POINTER) != 0u) && !pContext->Handles.Pointer)
      {
        pContext->Handles.Pointer.reset(wl_seat_get_pointer(seat));
        wl_pointer_add_listener(pContext->Handles.Pointer.get(), &g_pointerListener, pContext);
      }
      else if (((caps & WL_SEAT_CAPABILITY_POINTER) == 0u) && pContext->Handles.Pointer)
      {
        pContext->Handles.Pointer.reset();
      }

      if (((caps & WL_SEAT_CAPABILITY_KEYBOARD) != 0u) && !pContext->Handles.Keyboard)
      {
        pContext->Handles.Keyboard.reset(wl_seat_get_keyboard(seat));
        wl_keyboard_add_listener(pContext->Handles.Keyboard.get(), &g_keyboardListener, pContext);
      }
      else if (((caps & WL_SEAT_CAPABILITY_KEYBOARD) == 0u) && !pContext->Handles.Keyboard)
      {
        pContext->Handles.Keyboard.reset();
      }
    }

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

    const wl_seat_listener g_seatListener = {
      OnWaylandSystemContext_SeatHandleCapabilities,
    };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

    void OnWaylandSystemContext_RegistryHandleGlobal(void* data, wl_registry* registry, uint32_t name, const char* interface, uint32_t version)
    {
      FSLLOG3_VERBOSE5("Wayland registry handle global '{}'", interface)

      auto* pContext = static_cast<PlatformNativeWindowSystemContextWayland*>(data);
      assert(pContext != nullptr);

      if (strcmp(interface, wl_compositor_interface.name) == 0)
      {
        pContext->Handles.Compositor.reset(static_cast<wl_compositor*>(wl_registry_bind(registry, name, &wl_compositor_interface, 1)));
      }
      else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
      {
        pContext->Handles.WmBase.reset(static_cast<xdg_wm_base*>(wl_registry_bind(registry, name, &xdg_wm_base_interface, 1)));
        xdg_wm_base_add_listener(pContext->Handles.WmBase.get(), &g_wmBaseListener, pContext);
      }
      else if (strcmp(interface, wl_seat_interface.name) == 0)
      {
        pContext->Handles.Seat.reset(static_cast<wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface, 1)));
        wl_seat_add_listener(pContext->Handles.Seat.get(), &g_seatListener, pContext);
      }
      else if (strcmp(interface, wl_shm_interface.name) == 0)
      {
        pContext->Handles.Shm.reset(static_cast<wl_shm*>(wl_registry_bind(registry, name, &wl_shm_interface, 1)));
        pContext->Handles.CursorTheme.reset(wl_cursor_theme_load(nullptr, 32, pContext->Handles.Shm.get()));
        pContext->Handles.DefaultCursor = wl_cursor_theme_get_cursor(pContext->Handles.CursorTheme.get(), "left_ptr");
      }
      else if (strcmp(interface, wl_output_interface.name) == 0)
      {
        AddOutputInfo(*pContext, name, version);
      }
      else if (strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0)
      {
        pContext->Handles.DecorationManager.reset(
          static_cast<zxdg_decoration_manager_v1*>(wl_registry_bind(registry, name, &zxdg_decoration_manager_v1_interface, 1)));
      }
      else if (WaylandIVIHandler::TryRegistryHandleGlobal(*pContext, registry, name, interface, version))
      {
      }
    }

    void OnWaylandSystemContext_RegistryHandleRemove(void* data, wl_registry* registry, uint32_t name)
    {
    }

    const wl_registry_listener g_registryListener = {OnWaylandSystemContext_RegistryHandleGlobal, OnWaylandSystemContext_RegistryHandleRemove};

    uint32_t CalcDPI(const uint32_t width, const uint32_t millimeterWidth)
    {
      assert(width > 0);
      assert(millimeterWidth > 0);
      // 1mm = 0.0393701f inches
      const auto w = static_cast<double>(width);
      const double inchesWidth = static_cast<double>(millimeterWidth) * 0.0393701;
      return static_cast<uint32_t>(std::round(w / inchesWidth));
    }

    void TryUpdateDPI(const uint32_t width, const uint32_t height, const uint32_t physicalWidth, const uint32_t physicalHeight, Point2& rScreenDPI)
    {
      if (width > 0 && height > 0 && physicalWidth > 0 && physicalHeight > 0)
      {
        FSLLOG3_VERBOSE4("PlatformNativeWindowAdapterWayland| UpdateDPI: ");
        FSLLOG3_VERBOSE4("- screenSize.width: {}", width);
        FSLLOG3_VERBOSE4("- screenSize.height: {}", height);
        FSLLOG3_VERBOSE4("- screenSize.mwidth: {}", physicalWidth);
        FSLLOG3_VERBOSE4("- screenSize.mheight: {}", physicalHeight);
        rScreenDPI =
          Point2(UncheckedNumericCast<int32_t>(CalcDPI(width, physicalWidth)), UncheckedNumericCast<int32_t>(CalcDPI(height, physicalHeight)));
        FSLLOG3_VERBOSE4("- DPI: {}, {}", rScreenDPI.X, rScreenDPI.Y);
      }
      else
      {
        FSLLOG3_WARNING("Failed to acquire proper DPI information, using defaults");
        FSLLOG3_DEBUG_VERBOSE4("- Width {}, height {}, physicalWidth {} physicalHeight {}", width, height, physicalWidth, physicalHeight);
      }
    }

    std::shared_ptr<IPlatformNativeWindowAdapter>
      AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                     const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<PlatformNativeWindowAdapterWayland>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams);
    }

    void ExtractOutputs(std::vector<PlatformNativeWindowAdapterWayland::WaylandDisplayGeometry>& rDst,
                        const std::vector<std::unique_ptr<OutputInfo>>& outputs)
    {
      for (const auto& info : outputs)
      {
        PlatformNativeWindowAdapterWayland::WaylandDisplayGeometry currentDisplayGeometry{};
        currentDisplayGeometry.PhysicalWidth = info->Geometry.PhysicalWidth;
        currentDisplayGeometry.PhysicalHeight = info->Geometry.PhysicalHeight;
        for (const auto& mode : info->Modes)
        {
          if ((mode.Flags & WL_OUTPUT_MODE_CURRENT) != 0u)
          {
            currentDisplayGeometry.Width = mode.Width;
            currentDisplayGeometry.Height = mode.Height;
          }
        }
        rDst.push_back(currentDisplayGeometry);
      }
    }
  }


  PlatformNativeWindowSystemAdapterWayland::PlatformNativeWindowSystemAdapterWayland(
    const NativeWindowSystemSetup& setup, const PlatformNativeWindowAllocationFunction& allocateWindowFunction,
    const PlatformNativeWindowSystemParams& systemParams)
    : PlatformNativeWindowSystemAdapter(setup, nullptr)
    , m_allocationFunction(allocateWindowFunction ? allocateWindowFunction : AllocateWindow)
    , m_windowSystemContext(std::make_shared<PlatformNativeWindowSystemContextWayland>(setup.GetEventQueue()))
  {
    g_requestClose = false;

    // TODO: handle proper shutdown in case of exception here
    m_windowSystemContext->Handles.Display.reset(wl_display_connect(nullptr));
    if (!m_windowSystemContext->Handles.Display)
    {
      throw GraphicsException("wl_display_connect Failure");
    }

    try
    {
      m_platformDisplay = m_windowSystemContext->Handles.Display.get();

      m_windowSystemContext->Handles.Registry = wl_display_get_registry(m_windowSystemContext->Handles.Display.get());
      if (m_windowSystemContext->Handles.Registry == nullptr)
      {
        throw GraphicsException("wl_display_get_registry Failure");
      }

      if (wl_registry_add_listener(m_windowSystemContext->Handles.Registry, &g_registryListener, m_windowSystemContext.get()) == -1)
      {
        throw GraphicsException("wl_registry_add_listener Failure");
      }

      if (wl_display_dispatch(m_windowSystemContext->Handles.Display.get()) == -1)
      {
        throw GraphicsException("wl_display_dispatch Failure");
      }

      auto eventQueue = m_windowSystemContext->EventQueue.lock();
      if (eventQueue)
      {
        const NativeWindowEvent event = NativeWindowEventHelper::EncodeGamepadConfiguration(0);
        eventQueue->PostEvent(event);
      }
    }
    catch (const std::exception&)
    {
      throw;
    }
  }


  PlatformNativeWindowSystemAdapterWayland::~PlatformNativeWindowSystemAdapterWayland()
  {
    m_windowSystemContext->Outputs.clear();

    m_windowSystemContext->Handles.CursorSurface.reset();

    m_windowSystemContext->Handles.Ivi.reset();
    m_windowSystemContext->Handles.DecorationManager.reset();
    m_windowSystemContext->Handles.Keyboard.reset();
    m_windowSystemContext->Handles.Pointer.reset();

    // m_windowSystemContext.DefaultCursor.reset();
    m_windowSystemContext->Handles.CursorTheme.reset();
    m_windowSystemContext->Handles.Shm.reset();

    m_windowSystemContext->Handles.Seat.reset();
    m_windowSystemContext->Handles.WmBase.reset();
    m_windowSystemContext->Handles.Compositor.reset();

    if (wl_display_flush(m_windowSystemContext->Handles.Display.get()) == -1)
    {
      FSLLOG3_WARNING("wl_display_flush Failure");
    }

    m_windowSystemContext->Handles.Display.reset();

    m_windowSystemContext->MarkAsShutdown();

    m_windowSystemContext.reset();
  }


  std::shared_ptr<IPlatformNativeWindowAdapter> PlatformNativeWindowSystemAdapterWayland::CreateNativeWindow(
    const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
  {
    return m_allocationFunction(nativeWindowSetup, PlatformNativeWindowParams(m_windowSystemContext, m_platformDisplay, nullptr, nullptr, nullptr),
                                pPlatformCustomWindowAllocationParams);
  }


  bool PlatformNativeWindowSystemAdapterWayland::ProcessMessages(const NativeWindowProcessMessagesArgs& args)
  {
    bool bContinue = !g_requestClose;

    // This will dispatch messages that occurred
    wl_display_dispatch_pending(m_windowSystemContext->Handles.Display.get());

    return bContinue;
  }


  PlatformNativeWindowAdapterWayland::PlatformNativeWindowAdapterWayland(
    const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& platformWindowParams,
    const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    : PlatformNativeWindowAdapter(nativeWindowSetup, platformWindowParams, pPlatformCustomWindowAllocationParams, NativeWindowCapabilityFlags::GetDpi)
  {
    const NativeWindowConfig nativeWindowConfig = nativeWindowSetup.GetConfig();
    auto windowSystemContext = platformWindowParams.WindowSystemWaylandContext.lock();
    if (!windowSystemContext)
    {
      throw std::invalid_argument("Can not be null");
    }
    m_windowContext = std::make_unique<PlatformNativeWindowContextWayland>(windowSystemContext->EventQueue);

    FSLLOG3_WARNING_IF(nativeWindowSetup.GetConfig().GetDisplayId() != 0, "Wayland only supports the main display. Using DisplayId 0 instead of {}",
                       nativeWindowSetup.GetConfig().GetDisplayId());
    if (nativeWindowConfig.GetWindowMode() != WindowMode::Window)
    {
      FSLLOG3_VERBOSE("Fullscreen: Window Size/Position not defined, setting them to MAX Display Resolution");
      if (windowSystemContext->Handles.Ivi.Enabled)
      {
        m_windowContext->DesiredWindowSize = PxSize2D::Create(480, 504);
        m_windowContext->Fullscreen = false;
        FSLLOG3_VERBOSE("Wayland IVI not configured to fullscreen access even though requested to do so");
      }
      else
      {
        m_windowContext->DesiredWindowSize = PxSize2D::Create(250, 250);
        m_windowContext->Fullscreen = true;
      }
    }
    else
    {
      FSLLOG3_WARNING("Wayland does not allow the app to control the window position, so position request was ignored");
      const Rectangle windowRectangle = nativeWindowConfig.GetWindowRectangle();
      m_windowContext->DesiredWindowSize = TypeConverter::UncheckedTo<PxSize2D>(windowRectangle.GetSize());
    }
    FSLLOG3_VERBOSE("Creating window of size:{} and fullscreen:{}", m_windowContext->DesiredWindowSize, m_windowContext->Fullscreen);

    assert(windowSystemContext->Window == nullptr);
    windowSystemContext->Window = m_windowContext.get();
    windowSystemContext->MousePosition = PxPoint2::Create(0, 0);

    // if (m_windowContext->Fullscreen && windowSystemContext->Handles.WmBase)
    // {
    //   CreateWlDummySurface(*windowSystemContext, *m_windowContext);
    //   m_windowContext->Fullscreen = false;
    // }

    FSLLOG3_INFO_IF(nativeWindowSetup.GetVerbosityLevel() > 0,
                    "PlatformNativeWindowAdapterWayland: Creating window: (DesiredWindowSize={} Fullscreen: {})", m_windowContext->DesiredWindowSize,
                    m_windowContext->Fullscreen);


    CreateWlSurface(*windowSystemContext, *m_windowContext);
    m_platformSurface = m_windowContext->Handles.Surface.get();

    void* nativeWindowHolder = nullptr;
    if (platformWindowParams.CreateWaylandWindow)
    {
      nativeWindowHolder = platformWindowParams.CreateWaylandWindow(m_windowContext->Handles.Surface.get(), m_windowContext->Geometry.RawWidth(),
                                                                    m_windowContext->Geometry.RawHeight());
    }
    m_windowContext->Native = static_cast<PlatformNativeWindowType>(nativeWindowHolder);
    m_platformWindow = m_windowContext->Native;

    // Assign the Destroy Callback, there must be a smarter way to call them to avoid using globals.
    if (platformWindowParams.DestroyWaylandWindow)
    {
      m_destroyWindowCallback = platformWindowParams.DestroyWaylandWindow;
    }
    if (platformWindowParams.ResizeWaylandWindow)
    {
      m_windowContext->ResizeWindowCallback = platformWindowParams.ResizeWaylandWindow;
    }

    do
    {
      windowSystemContext->RoundtripNeeded = false;
      wl_display_roundtrip(windowSystemContext->Handles.Display.get());
    } while (windowSystemContext->RoundtripNeeded);

    ExtractOutputs(m_displayOutput, windowSystemContext->Outputs);

    TryUpdateDPI(m_displayOutput[0].Width, m_displayOutput[0].Height, m_displayOutput[0].PhysicalWidth, m_displayOutput[0].PhysicalHeight,
                 m_cachedScreenDPI);

    windowSystemContext->Handles.CursorSurface.reset(wl_compositor_create_surface(windowSystemContext->Handles.Compositor.get()));
    if (!windowSystemContext->Handles.CursorSurface)
    {
      throw GraphicsException("wl_compositor_create_surface CursorSurface failure");
    }


    {    // Post the activation message to let the framework know we are ready
      std::shared_ptr<INativeWindowEventQueue> eventQueue = m_windowContext->EventQueue.lock();
      if (eventQueue)
      {
        eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowActivationEvent(true));
      }
    }
  }


  PlatformNativeWindowAdapterWayland::~PlatformNativeWindowAdapterWayland()
  {
    if (m_windowContext->Native != nullptr)
    {
      if (m_destroyWindowCallback)
      {
        m_destroyWindowCallback(m_windowContext->Native);
      }
    }

    m_windowContext->Handles.reset();
    // m_windowContext->Callback.reset();
  }


  bool PlatformNativeWindowAdapterWayland::TryGetNativeSize(PxPoint2& rSize) const
  {
    auto width = m_windowContext->Geometry.RawWidth();
    auto height = m_windowContext->Geometry.RawHeight();
    if (width <= 0 || height <= 0)
    {
      return false;
    }

    rSize = PxPoint2::Create(width, height);
    FSLLOG3_VERBOSE5("TryGetNativeSize: {}", rSize)
    return true;
  }


  bool PlatformNativeWindowAdapterWayland::TryGetNativeDpi(Vector2& rDPI) const
  {
    rDPI = Vector2(m_cachedScreenDPI.X, m_cachedScreenDPI.Y);
    return true;
  }
}
#endif
