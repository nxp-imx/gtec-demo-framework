#if !defined(__ANDROID__) && defined(__linux__) && !defined(FSL_WINDOWSYSTEM_X11) && defined(FSL_WINDOWSYSTEM_WAYLAND)
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

#include <FslNativeWindow/Platform/Wayland/PlatformNativeWindowSystemWayland.hpp>
#include <FslNativeWindow/Platform/Wayland/PlatformNativeWindowWayland.hpp>
#include <FslNativeWindow/Base/INativeWindowEventQueue.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/Base/NativeWindowSetup.hpp>
#include <FslNativeWindow/Base/NativeWindowSystemSetup.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Log/Math/LogRectangle.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <wayland-client.h>
#include <wayland-cursor.h>
#include <linux/input.h>
#include <string.h>
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
#include <ilm/ivi-application-client-protocol.h>
#include <sys/types.h>
#include <unistd.h>

#define IVI_SURFACE_ID 9000
#endif

// TODO: remove globals

namespace Fsl
{
  namespace
  {
    std::weak_ptr<INativeWindowEventQueue> g_eventQueue;

    struct MouseEvent
    {
      enum Enum
      {
        NoEvent = 0,
        Button = 1,
        Movement = 2,
        Wheel = 3
      };
    };

    struct geometry
    {
      int width;
      int height;
    };

    struct display;

    struct window
    {
      struct display* display;
      PlatformNativeWindowType native;
      struct geometry geometry, window_size;
      wl_surface* surface;
      wl_shell_surface* shell_surface;
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
      struct ivi_surface *ivi_surface;
#endif
      wl_callback* callback;
      int fullscreen, configured;
      std::function<void(void*, int, int, int, int)> resizeWindowCallback;
    };

    struct display
    {
      wl_display* display;
      wl_registry* registry;
      wl_compositor* compositor;
      wl_shell* shell;
      wl_seat* seat;
      wl_pointer* pointer;
      wl_keyboard* keyboard;
      wl_shm* shm;
      wl_cursor_theme* cursor_theme;
      wl_cursor* default_cursor;
      wl_surface* cursor_surface;
      VirtualKey::Enum keycode;
      bool isPressed;
      Point2 mousePosition;
      int zDelta;
      VirtualMouseButton::Enum mouseButton;
      bool mouseIsPressed;
      struct window* window;
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
      struct ivi_application *ivi_application;
#endif

    };

    struct display sdisplay = {nullptr};
    struct window swindow = {nullptr};
    bool dummyRunning = true;


    void CheckKey(struct display* d, int state, VirtualKey::Enum keyCode)
    {
      d->keycode = keyCode;
      if (state)
        d->isPressed = true;
      else
        d->isPressed = false;
    }


    void ConfigureCallback(void* data, wl_callback* callback, uint32_t time)
    {
      struct window* window = (struct window*)data;
      wl_callback_destroy(callback);
      if (!window->fullscreen)
        window->configured = 1;
      else
        dummyRunning = false;
    }


    wl_callback_listener configureCallBackListener = {
      ConfigureCallback,
    };


    void HandlePing(void* data, wl_shell_surface* shell_surface, uint32_t serial)
    {
      wl_shell_surface_pong(shell_surface, serial);
    }


    void HandleConfigure(void* data, wl_shell_surface* shell_surface, uint32_t edges, int32_t width, int32_t height)
    {
      struct window* window = (struct window*)data;
      if (window->native && !window->fullscreen)
      {
        if (window->resizeWindowCallback)
          window->resizeWindowCallback(window->native, width, height, 0, 0);
      }
      window->geometry.width = width;
      window->geometry.height = height;
      swindow.window_size.width = width;
      swindow.window_size.height = height;

      if (!window->fullscreen)
        window->window_size = window->geometry;
    }


    void HandlePopUpDone(void* data, wl_shell_surface* shell_surface)
    {
    }


    const wl_shell_surface_listener shell_surface_listener = {HandlePing, HandleConfigure, HandlePopUpDone};
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
  static void HandleIVISurfaceConfigure(void *data, struct ivi_surface *ivi_surface,
                                     int32_t width, int32_t height)
  {
     struct window *window = (struct window *)data;

     if (window->resizeWindowCallback)
     {
          window->resizeWindowCallback(window->native, width, height, 0, 0);
     }
     window->geometry.width = width;
     window->geometry.height = height;
     if (!window->fullscreen)
          window->window_size = window->geometry;

   }
    const struct ivi_surface_listener iviSurfaceListener = {HandleIVISurfaceConfigure };
#endif

    void ToggleFullScreen(struct window* window, int fullscreen)
    {
      wl_callback* callback;
      window->fullscreen = fullscreen;
      window->configured = 0;

      if (fullscreen)
      {
        wl_shell_surface_set_fullscreen(window->shell_surface, WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT, 0, nullptr);
      }
      else
      {
        wl_shell_surface_set_toplevel(window->shell_surface);
        HandleConfigure(window, window->shell_surface, 0, window->window_size.width, window->window_size.height);
      }

      if (nullptr == (callback = wl_display_sync(window->display->display)))
        throw GraphicsException("wl_display_sync Failure");
      if (-1 == (wl_callback_add_listener(callback, &configureCallBackListener, window)))
        throw GraphicsException("wl_display_sync Failure");
    }


    void CreateWlDummySurface()
    {
      struct display* display = &sdisplay;
      struct window* window = &swindow;
      wl_callback* callback;
      try
      {
        if (nullptr == (window->surface = wl_compositor_create_surface(display->compositor)))
          throw GraphicsException("wl_compositor_create_surface Failure");
        if(display->shell){
        if (nullptr == (window->shell_surface = wl_shell_get_shell_surface(display->shell, window->surface)))
          throw GraphicsException("wl_shell_get_shell_surface Failure");

        if (wl_shell_surface_add_listener(window->shell_surface, &shell_surface_listener, window))
          throw GraphicsException("wl_shell_surface_add_listener Failure");

        wl_shell_surface_set_title(window->shell_surface, "FSL Framework");

        wl_shell_surface_set_fullscreen(window->shell_surface, WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT, 0, nullptr);
        }
        if (nullptr == (callback = wl_display_sync(window->display->display)))
          throw GraphicsException("wl_display_sync Failure");

        if (-1 == (wl_callback_add_listener(callback, &configureCallBackListener, window)))
          throw GraphicsException("wl_display_sync Failure");
      }
      catch (const std::exception&)
      {
        if (window->surface != nullptr)
          wl_surface_destroy(window->surface);
        if (window->shell_surface != nullptr)
          wl_shell_surface_destroy(window->shell_surface);
        throw;
      }

      while (dummyRunning)
        wl_display_dispatch(sdisplay.display);

      if (window->fullscreen && display->shell)
      {
        wl_shell_surface_destroy(window->shell_surface);
        wl_surface_destroy(window->surface);
        window->fullscreen = 0;
      }
    }


    void CreateWlSurface(void)
    {
      struct display* display = &sdisplay;
      struct window* window = &swindow;
      try
      {
        if (nullptr == (window->surface = wl_compositor_create_surface(display->compositor)))
          throw GraphicsException("wl_compositor_create_surface Failure");
        if(display->shell)
       {
        if (nullptr == (window->shell_surface = wl_shell_get_shell_surface(display->shell, window->surface)))
          throw GraphicsException("wl_shell_get_shell_surface Failure");

        if (wl_shell_surface_add_listener(window->shell_surface, &shell_surface_listener, window))
          throw GraphicsException("wl_shell_surface_add_listener Failure");

           wl_shell_surface_set_title(window->shell_surface, "FSL Framework");

           ToggleFullScreen(window, window->fullscreen);
        }
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI

        if (display->ivi_application)
        {
           uint32_t id_ivisurf = IVI_SURFACE_ID + (uint32_t)getpid();
           window->ivi_surface = ivi_application_surface_create(display->ivi_application,id_ivisurf, window->surface);
           ivi_surface_add_listener(window->ivi_surface,&iviSurfaceListener, window);
          }
#endif
      }
      catch (const std::exception&)
      {
        if (window->shell_surface != nullptr)
          wl_shell_surface_destroy(window->shell_surface);
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
        if (window->ivi_surface !=nullptr)
          ivi_surface_destroy(window->ivi_surface);
        if (display->ivi_application)
          ivi_application_destroy(display->ivi_application);
#endif
        if (window->surface != nullptr)
          wl_surface_destroy(window->surface);
        throw;
      }
    }


    void PointerHandleEnter(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy)
    {
      struct display* display = (struct display*)data;
      wl_buffer* buffer;
      wl_cursor* cursor = display->default_cursor;
      wl_cursor_image* image;


      if (display->window->fullscreen)
        wl_pointer_set_cursor(pointer, serial, nullptr, 0, 0);
      else if (cursor)
      {
        image = display->default_cursor->images[0];
        buffer = wl_cursor_image_get_buffer(image);
        wl_pointer_set_cursor(pointer, serial, display->cursor_surface, image->hotspot_x, image->hotspot_y);
        wl_surface_attach(display->cursor_surface, buffer, 0, 0);
        wl_surface_damage(display->cursor_surface, 0, 0, image->width, image->height);
        wl_surface_commit(display->cursor_surface);
      }

      display->mousePosition.X = sx / 256;
      display->mousePosition.Y = sy / 256;
    }


    void PointerHandleLeave(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface)
    {
    }


    void PointerHandleMotion(void* data, wl_pointer* pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
    {
      std::shared_ptr<INativeWindowEventQueue> eventQueue = g_eventQueue.lock();
      struct display* display = (struct display*)data;

      display->mousePosition.X = sx / 256;
      display->mousePosition.Y = sy / 256;
      if (eventQueue)
        eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputMouseMoveEvent(display->mousePosition));
    }


    void PointerHandleButton(void* data, wl_pointer* wl_pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
    {
      std::shared_ptr<INativeWindowEventQueue> eventQueue = g_eventQueue.lock();
      struct display* display = (struct display*)data;

      if (WL_POINTER_BUTTON_STATE_PRESSED == state)
      {
        display->mouseIsPressed = true;
        if (button == BTN_LEFT)
        {
          display->mouseButton = VirtualMouseButton::Left;
        }
        else if (button == BTN_RIGHT)
        {
          display->mouseButton = VirtualMouseButton::Right;
        }
        else if (button == BTN_MIDDLE)
        {
          display->mouseButton = VirtualMouseButton::Middle;
        }
      }
      else
      {
        display->mouseIsPressed = false;
      }
      if (eventQueue)
        eventQueue->PostEvent(
          NativeWindowEventHelper::EncodeInputMouseButtonEvent(display->mouseButton, display->mouseIsPressed, display->mousePosition));
    }


    void PointerHandleAxis(void* data, wl_pointer* wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
    {
      std::shared_ptr<INativeWindowEventQueue> eventQueue = g_eventQueue.lock();
      struct display* display = (struct display*)data;

      display->zDelta = (int)value / 2560;
      if (eventQueue)
        eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputMouseWheelEvent(display->zDelta, display->mousePosition));
    }


    const wl_pointer_listener pointer_listener = {
      PointerHandleEnter, PointerHandleLeave, PointerHandleMotion, PointerHandleButton, PointerHandleAxis,
    };


    void KeyboardHandleKeymap(void* data, wl_keyboard* keyboard, uint32_t format, int fd, uint32_t size)
    {
    }


    void KeyboardHandleEnter(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface, wl_array* keys)
    {
    }


    void KeyboardHandleLeave(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface)
    {
    }


    void KeyboardHandleKey(void* data, wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
    {
      std::shared_ptr<INativeWindowEventQueue> eventQueue = g_eventQueue.lock();
      struct display* d = (struct display*)data;

      if (key == KEY_F11)
        CheckKey(d, state, VirtualKey::F11);
      else if (key == KEY_ESC)
        CheckKey(d, state, VirtualKey::Escape);
      else if (key == KEY_F1)
        CheckKey(d, state, VirtualKey::F1);
      else if (key == KEY_F2)
        CheckKey(d, state, VirtualKey::F2);
      else if (key == KEY_F3)
        CheckKey(d, state, VirtualKey::F3);
      else if (key == KEY_F4)
        CheckKey(d, state, VirtualKey::F4);
      else if (key == KEY_F5)
        CheckKey(d, state, VirtualKey::F5);
      else if (key == KEY_F6)
        CheckKey(d, state, VirtualKey::F6);
      else if (key == KEY_F7)
        CheckKey(d, state, VirtualKey::F7);
      else if (key == KEY_F8)
        CheckKey(d, state, VirtualKey::F8);
      else if (key == KEY_F9)
        CheckKey(d, state, VirtualKey::F9);
      else if (key == KEY_F10)
        CheckKey(d, state, VirtualKey::F10);
      else if (key == KEY_F11)
        CheckKey(d, state, VirtualKey::F11);
      else if (key == KEY_F12)
        CheckKey(d, state, VirtualKey::F12);
      else if (key == KEY_0)
        CheckKey(d, state, VirtualKey::Code0);
      else if (key == KEY_1)
        CheckKey(d, state, VirtualKey::Code1);
      else if (key == KEY_2)
        CheckKey(d, state, VirtualKey::Code2);
      else if (key == KEY_3)
        CheckKey(d, state, VirtualKey::Code3);
      else if (key == KEY_4)
        CheckKey(d, state, VirtualKey::Code4);
      else if (key == KEY_5)
        CheckKey(d, state, VirtualKey::Code5);
      else if (key == KEY_6)
        CheckKey(d, state, VirtualKey::Code6);
      else if (key == KEY_7)
        CheckKey(d, state, VirtualKey::Code7);
      else if (key == KEY_8)
        CheckKey(d, state, VirtualKey::Code8);
      else if (key == KEY_9)
        CheckKey(d, state, VirtualKey::Code9);
      else if (key == KEY_A)
        CheckKey(d, state, VirtualKey::A);
      else if (key == KEY_B)
        CheckKey(d, state, VirtualKey::B);
      else if (key == KEY_C)
        CheckKey(d, state, VirtualKey::C);
      else if (key == KEY_D)
        CheckKey(d, state, VirtualKey::D);
      else if (key == KEY_E)
        CheckKey(d, state, VirtualKey::E);
      else if (key == KEY_F)
        CheckKey(d, state, VirtualKey::F);
      else if (key == KEY_G)
        CheckKey(d, state, VirtualKey::G);
      else if (key == KEY_H)
        CheckKey(d, state, VirtualKey::H);
      else if (key == KEY_I)
        CheckKey(d, state, VirtualKey::I);
      else if (key == KEY_J)
        CheckKey(d, state, VirtualKey::J);
      else if (key == KEY_K)
        CheckKey(d, state, VirtualKey::K);
      else if (key == KEY_L)
        CheckKey(d, state, VirtualKey::L);
      else if (key == KEY_M)
        CheckKey(d, state, VirtualKey::M);
      else if (key == KEY_N)
        CheckKey(d, state, VirtualKey::N);
      else if (key == KEY_O)
        CheckKey(d, state, VirtualKey::O);
      else if (key == KEY_P)
        CheckKey(d, state, VirtualKey::P);
      else if (key == KEY_Q)
        CheckKey(d, state, VirtualKey::Q);
      else if (key == KEY_R)
        CheckKey(d, state, VirtualKey::R);
      else if (key == KEY_S)
        CheckKey(d, state, VirtualKey::S);
      else if (key == KEY_T)
        CheckKey(d, state, VirtualKey::T);
      else if (key == KEY_U)
        CheckKey(d, state, VirtualKey::U);
      else if (key == KEY_V)
        CheckKey(d, state, VirtualKey::V);
      else if (key == KEY_W)
        CheckKey(d, state, VirtualKey::W);
      else if (key == KEY_X)
        CheckKey(d, state, VirtualKey::X);
      else if (key == KEY_Y)
        CheckKey(d, state, VirtualKey::Y);
      else if (key == KEY_Z)
        CheckKey(d, state, VirtualKey::Z);
      else if (key == KEY_TAB)
        CheckKey(d, state, VirtualKey::Tab);
      else if (key == KEY_ENTER)
        CheckKey(d, state, VirtualKey::Return);
      else if (key == KEY_SPACE)
        CheckKey(d, state, VirtualKey::Space);
      else if (key == KEY_BACKSPACE)
        CheckKey(d, state, VirtualKey::Backspace);
      else if (key == KEY_UP)
        CheckKey(d, state, VirtualKey::UpArrow);
      else if (key == KEY_DOWN)
        CheckKey(d, state, VirtualKey::DownArrow);
      else if (key == KEY_LEFT)
        CheckKey(d, state, VirtualKey::LeftArrow);
      else if (key == KEY_RIGHT)
        CheckKey(d, state, VirtualKey::RightArrow);
      else if (key == KEY_INSERT)
        CheckKey(d, state, VirtualKey::Insert);
      else if (key == KEY_DELETE)
        CheckKey(d, state, VirtualKey::Delete);
      else if (key == KEY_HOME)
        CheckKey(d, state, VirtualKey::Home);
      else if (key == KEY_END)
        CheckKey(d, state, VirtualKey::End);
      else if (key == KEY_PAGEUP)
        CheckKey(d, state, VirtualKey::PageUp);
      else if (key == KEY_PAGEDOWN)
        CheckKey(d, state, VirtualKey::PageDown);
      else if (key == KEY_SCROLLLOCK)
        CheckKey(d, state, VirtualKey::ScrollLock);
      else if (key == KEY_PAUSE)
        CheckKey(d, state, VirtualKey::Pause);
      else if (key == KEY_BREAK)
        CheckKey(d, state, VirtualKey::Break);

      if (eventQueue)
        eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputKeyEvent(d->keycode, d->isPressed));
    }


    void KeyboardHandleModifiers(void* data, wl_keyboard* keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched,
                                 uint32_t mods_locked, uint32_t group)
    {
    }


    const wl_keyboard_listener keyboard_listener = {
      KeyboardHandleKeymap, KeyboardHandleEnter, KeyboardHandleLeave, KeyboardHandleKey, KeyboardHandleModifiers,
    };


    void SeatHandleCapabilities(void* data, wl_seat* seat, uint32_t caps)
    {
      struct display* d = (struct display*)data;

      if ((caps & WL_SEAT_CAPABILITY_POINTER) && !d->pointer)
      {
        d->pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(d->pointer, &pointer_listener, d);
      }
      else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && d->pointer)
      {
        wl_pointer_destroy(d->pointer);
        d->pointer = nullptr;
      }

      if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !d->keyboard)
      {
        d->keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(d->keyboard, &keyboard_listener, d);
      }
      else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && d->keyboard)
      {
        wl_keyboard_destroy(d->keyboard);
        d->keyboard = nullptr;
      }
    }

    const wl_seat_listener seat_listener = {
      SeatHandleCapabilities,
    };


    void RegistryHandleGlobal(void* data, wl_registry* registry, uint32_t name, const char* interface, uint32_t version)
    {
      struct display* d = (struct display*)data;

      if (strcmp(interface, "wl_compositor") == 0)
      {
        d->compositor = (struct wl_compositor*)wl_registry_bind(registry, name, &wl_compositor_interface, 1);
      }
      else if (strcmp(interface, "wl_shell") == 0)
      {
        d->shell = (struct wl_shell*)wl_registry_bind(registry, name, &wl_shell_interface, 1);
      }
      else if (strcmp(interface, "wl_seat") == 0)
      {
        d->seat = (struct wl_seat*)wl_registry_bind(registry, name, &wl_seat_interface, 1);
        wl_seat_add_listener(d->seat, &seat_listener, d);
      }
      else if (strcmp(interface, "wl_shm") == 0)
      {
        d->shm = (struct wl_shm*)wl_registry_bind(registry, name, &wl_shm_interface, 1);
        d->cursor_theme = wl_cursor_theme_load(nullptr, 32, d->shm);
        d->default_cursor = wl_cursor_theme_get_cursor(d->cursor_theme, "left_ptr");
      }
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
      else if (strcmp(interface, "ivi_application") == 0)
      {
        d->ivi_application =(struct ivi_application *)wl_registry_bind(registry, name,&ivi_application_interface, 1);
      }
#endif
    }

    const wl_registry_listener registry_listener = {RegistryHandleGlobal};


    std::shared_ptr<INativeWindow> AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                                                  const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<PlatformNativeWindowWayland>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams);
    }
  }    // namespace


  PlatformNativeWindowSystemWayland::PlatformNativeWindowSystemWayland(const NativeWindowSystemSetup& setup,
                                                                       const PlatformNativeWindowAllocationFunction& allocateWindowFunction,
                                                                       const PlatformNativeWindowSystemParams& systemParams)
    : PlatformNativeWindowSystem(setup, nullptr)
    , m_allocationFunction(allocateWindowFunction ? allocateWindowFunction : AllocateWindow)
  {
    // TODO: handle proper shutdown in case of exception here
    if (nullptr == (sdisplay.display = wl_display_connect(nullptr)))
      throw GraphicsException("wl_display_connect Failure");

    try
    {
      m_platformDisplay = sdisplay.display;

      if (nullptr == (sdisplay.registry = wl_display_get_registry(sdisplay.display)))
        throw GraphicsException("wl_display_get_registry Failure");

      if (-1 == (wl_registry_add_listener(sdisplay.registry, &registry_listener, &sdisplay)))
        throw GraphicsException("wl_registry_add_listener Failure");

      if (-1 == (wl_display_dispatch(sdisplay.display)))
        throw GraphicsException("wl_display_dispatch Failure");

      auto eventQueue = setup.GetEventQueue().lock();
      if (eventQueue)
      {
        const NativeWindowEvent event = NativeWindowEventHelper::EncodeGamepadConfiguration(0);
        eventQueue->PostEvent(event);
      }
    }
    catch (const std::exception&)
    {
      if (sdisplay.display != nullptr)
      {
        wl_display_disconnect(sdisplay.display);
        sdisplay.display = nullptr;
      }
      throw;
    }
  }


  PlatformNativeWindowSystemWayland::~PlatformNativeWindowSystemWayland()
  {
    struct display* display = &sdisplay;

    if (display->compositor)
      wl_compositor_destroy(display->compositor);

    if (-1 == (wl_display_flush(display->display)))
    {
      FSLLOG_WARNING("wl_display_flush Failure");
    }

    wl_display_disconnect(display->display);
  }


  std::shared_ptr<INativeWindow>
    PlatformNativeWindowSystemWayland::CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                                                          const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
  {
    return m_allocationFunction(nativeWindowSetup, PlatformNativeWindowParams(m_platformDisplay, nullptr, nullptr, nullptr),
                                pPlatformCustomWindowAllocationParams);
  }


  bool PlatformNativeWindowSystemWayland::ProcessMessages(const NativeWindowProcessMessagesArgs& args)
  {
    bool bContinue = true;

    struct display* display = &sdisplay;
    // This will dispatch messages that occurred
    wl_display_dispatch_pending(display->display);

    return bContinue;
  }


  PlatformNativeWindowWayland::PlatformNativeWindowWayland(const NativeWindowSetup& nativeWindowSetup,
                                                           const PlatformNativeWindowParams& platformWindowParams,
                                                           const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    : PlatformNativeWindow(nativeWindowSetup, platformWindowParams, pPlatformCustomWindowAllocationParams)
  {
    const NativeWindowConfig nativeWindowConfig = nativeWindowSetup.GetConfig();
    g_eventQueue = nativeWindowSetup.GetEventQueue();

    FSLLOG_WARNING_IF(nativeWindowSetup.GetConfig().GetDisplayId() != 0,
                      "Wayland only supports the main display. Using DisplayId 0 instead of " << nativeWindowSetup.GetConfig().GetDisplayId());
    if (nativeWindowConfig.GetWindowMode() != WindowMode::Window)
    {
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
           swindow.window_size.width = 480;
           swindow.window_size.height = 504;
           swindow.fullscreen = 0;
#else
      FSLLOG("WARNING: Window Size/Position not defined, setting them to MAX Display Resolution");
      swindow.window_size.width = 250;
      swindow.window_size.height = 250;
      swindow.fullscreen = 1;
#endif
    }
    else
    {
      const Rectangle windowRectangle = nativeWindowConfig.GetWindowRectangle();
      swindow.window_size.width = windowRectangle.Width();
      swindow.window_size.height = windowRectangle.Height();
    }

    swindow.display = &sdisplay;
    sdisplay.window = &swindow;

    sdisplay.mousePosition.X = 0;
    sdisplay.mousePosition.Y = 0;

    if (swindow.fullscreen && sdisplay.shell)
    {
      CreateWlDummySurface();
      swindow.fullscreen = 0;
    }

    FSLLOG_IF(nativeWindowSetup.GetVerbosityLevel() > 0,
              "PlatformNativeWindowX11: Creating window: {Width = " << swindow.window_size.width << " Height = " << swindow.window_size.height
                                                                    << " fullscreen: " << swindow.fullscreen << " }");


    CreateWlSurface();
    m_platformSurface = swindow.surface;

    void* nativeWindowHolder = nullptr;
    if (platformWindowParams.CreateWaylandWindow)
    {
      nativeWindowHolder = platformWindowParams.CreateWaylandWindow((void*)swindow.surface, swindow.window_size.width, swindow.window_size.height);
    }
    swindow.native = (PlatformNativeWindowType)nativeWindowHolder;
    m_platformWindow = swindow.native;

    // Assign the Destroy Callback, there must be a smarter way to call them to avoid using globals.
    if (platformWindowParams.DestroyWaylandWindow)
    {
      g_destroyWindowCallback = platformWindowParams.DestroyWaylandWindow;
    }
    if (platformWindowParams.ResizeWaylandWindow)
    {
      swindow.resizeWindowCallback = platformWindowParams.ResizeWaylandWindow;
    }

    if (nullptr == (sdisplay.cursor_surface = wl_compositor_create_surface(sdisplay.compositor)))
      throw GraphicsException("wl_compositor_create_surface Failure");

    {    // Post the activation message to let the framework know we are ready
      std::shared_ptr<INativeWindowEventQueue> eventQueue = g_eventQueue.lock();
      if (eventQueue)
        eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowActivationEvent(true));
    }
  }


  PlatformNativeWindowWayland::~PlatformNativeWindowWayland()
  {
    struct display* display = &sdisplay;
    struct window* window = &swindow;
    if (nullptr != window->native)
    {
      if (g_destroyWindowCallback)
        g_destroyWindowCallback(window->native);
    }
    if (display->shell)
       wl_shell_surface_destroy(window->shell_surface);
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
    if (window->ivi_surface)
       ivi_surface_destroy(window->ivi_surface);
#endif
    wl_surface_destroy(window->surface);

    if (window->callback)
      wl_callback_destroy(window->callback);

    wl_surface_destroy(display->cursor_surface);

    if (display->cursor_theme)
      wl_cursor_theme_destroy(display->cursor_theme);

    if (display->shell)
      wl_shell_destroy(display->shell);
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
    if (display->ivi_application)
       ivi_application_destroy(display->ivi_application);
#endif
  }


  bool PlatformNativeWindowWayland::TryGetDPI(Vector2& rDPI) const
  {
    {    // Remove this once its implemented
      static bool warnedNotImplementedOnce = false;
      if (!warnedNotImplementedOnce)
      {
        FSLLOG("PlatformNativeWindowWayland: TryGetDPI is not implemented on this backend.");
        warnedNotImplementedOnce = true;
      }
    }

    rDPI = Vector2();
    return false;
  }


  bool PlatformNativeWindowWayland::TryGetSize(Point2& rSize) const
  {
    {    // Remove this once its implemented
      static bool warnedNotImplementedOnce = false;
      if (!warnedNotImplementedOnce)
      {
        FSLLOG("PlatformNativeWindowWayland: TryGetSize is not implemented on this backend.");
        warnedNotImplementedOnce = true;
      }
    }

    rSize = Point2();
    return false;
  }
}    // namespace Fsl
#endif
