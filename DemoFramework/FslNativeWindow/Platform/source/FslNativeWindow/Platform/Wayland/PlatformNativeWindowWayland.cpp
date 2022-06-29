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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/LogRectangle.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslNativeWindow/Base/INativeWindowEventQueue.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/Base/NativeWindowSetup.hpp>
#include <FslNativeWindow/Base/NativeWindowSystemSetup.hpp>
#include <FslNativeWindow/Platform/Wayland/PlatformNativeWindowSystemWayland.hpp>
#include <FslNativeWindow/Platform/Wayland/PlatformNativeWindowWayland.hpp>
#include <linux/input.h>
#include <string.h>
#include <wayland-client.h>
#include <wayland-cursor.h>
#include <algorithm>
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
      int width{};
      int height{};
    };

    struct display;

    struct window
    {
      struct display* display{nullptr};
      PlatformNativeWindowType native{};
      struct geometry geometry
      {
      };
      struct geometry window_size
      {
      };
      wl_surface* surface{nullptr};
      wl_shell_surface* shell_surface{nullptr};
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
      struct ivi_surface* ivi_surface{nullptr};
#endif
      wl_callback* callback{nullptr};
      int fullscreen{0};
      int configured{0};
      std::function<void(void*, int, int, int, int)> resizeWindowCallback;
    };

    struct display
    {
      wl_display* display{nullptr};
      wl_registry* registry{nullptr};
      wl_compositor* compositor{nullptr};
      wl_shell* shell{nullptr};
      wl_seat* seat{nullptr};
      wl_pointer* pointer{nullptr};
      wl_keyboard* keyboard{nullptr};
      wl_shm* shm{nullptr};
      wl_cursor_theme* cursor_theme{nullptr};
      wl_cursor* default_cursor{nullptr};
      wl_surface* cursor_surface{nullptr};
      VirtualKey::Enum keycode{};
      bool isPressed{false};
      PxPoint2 mousePosition;
      int zDelta{};
      VirtualMouseButton::Enum mouseButton{};
      bool mouseIsPressed{};
      struct window* window{nullptr};
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
      struct ivi_application* ivi_application{nullptr};
#endif
      struct wl_list infos;
      bool roundtrip_needed;
      struct wl_list outputs;
    };

    struct display sdisplay
    {
    };
    struct window swindow
    {
    };

    typedef void (*print_info_t)(void* info);
    typedef void (*destroy_info_t)(void* info);

    struct global_info
    {
      struct wl_list link;

      uint32_t id;
      uint32_t version;
      char* interface;

      print_info_t print;
      destroy_info_t destroy;
    };

    struct output_mode
    {
      struct wl_list link;

      uint32_t flags;
      int32_t width, height;
      int32_t refresh;
    };

    struct output_info
    {
      struct global_info global;
      struct wl_list global_link;

      struct wl_output* output;

      uint32_t version;

      struct
      {
        int32_t x, y;
        int32_t scale;
        int32_t physical_width, physical_height;
        enum wl_output_subpixel subpixel;
        enum wl_output_transform output_transform;
        char* make;
        char* model;
      } geometry;

      struct wl_list modes;
    };

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

      {    // Let the framework know that we might have been resized
        std::shared_ptr<INativeWindowEventQueue> eventQueue = g_eventQueue.lock();
        if (eventQueue)
        {
          eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowResizedEvent());
        }
      }
    }


    void HandlePopUpDone(void* data, wl_shell_surface* shell_surface)
    {
    }


    const wl_shell_surface_listener shell_surface_listener = {HandlePing, HandleConfigure, HandlePopUpDone};
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
    static void HandleIVISurfaceConfigure(void* data, struct ivi_surface* ivi_surface, int32_t width, int32_t height)
    {
      struct window* window = (struct window*)data;

      if (window->resizeWindowCallback)
      {
        window->resizeWindowCallback(window->native, width, height, 0, 0);
      }
      window->geometry.width = width;
      window->geometry.height = height;
      if (!window->fullscreen)
        window->window_size = window->geometry;
    }
    const struct ivi_surface_listener iviSurfaceListener = {HandleIVISurfaceConfigure};
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
        if (display->shell)
        {
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
#ifndef FSL_WINDOWSYSTEM_WAYLAND_IVI
        if (display->shell)
        {
          if (nullptr == (window->shell_surface = wl_shell_get_shell_surface(display->shell, window->surface)))
            throw GraphicsException("wl_shell_get_shell_surface Failure");

          if (wl_shell_surface_add_listener(window->shell_surface, &shell_surface_listener, window))
            throw GraphicsException("wl_shell_surface_add_listener Failure");

          wl_shell_surface_set_title(window->shell_surface, "FSL Framework");

          ToggleFullScreen(window, window->fullscreen);
        }
#endif
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI

        if (display->ivi_application)
        {
          uint32_t id_ivisurf = IVI_SURFACE_ID + (uint32_t)getpid();
          window->ivi_surface = ivi_application_surface_create(display->ivi_application, id_ivisurf, window->surface);
          ivi_surface_add_listener(window->ivi_surface, &iviSurfaceListener, window);
        }
#endif
      }
      catch (const std::exception&)
      {
        if (window->shell_surface != nullptr)
          wl_shell_surface_destroy(window->shell_surface);
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
        if (window->ivi_surface != nullptr)
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

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

    const wl_pointer_listener pointer_listener = {
      PointerHandleEnter, PointerHandleLeave, PointerHandleMotion, PointerHandleButton, PointerHandleAxis,
    };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

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

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

    const wl_keyboard_listener keyboard_listener = {
      KeyboardHandleKeymap, KeyboardHandleEnter, KeyboardHandleLeave, KeyboardHandleKey, KeyboardHandleModifiers,
    };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

    static void print_global_info(void* data)
    {
      struct global_info* global = (global_info*)data;
      assert(global != nullptr);

      FSLLOG3_VERBOSE4("interface: {}, version: {}, name: {}", global->interface, global->version, global->id);
    }

    static void init_global_info(void* info, struct global_info* global, uint32_t id, const char* interface, uint32_t version)
    {
      struct display* display = (struct display*)info;
      assert(display != nullptr);
      global->id = id;
      global->version = version;
      global->interface = strdup(interface);

      wl_list_insert(display->infos.prev, &global->link);
    }

    static void print_output_info(void* data)
    {
      struct output_info* output = (output_info*)data;
      assert(output != nullptr);

      struct output_mode* mode;
      const char* subpixel_orientation;
      const char* transform;

      print_global_info(data);

      switch (output->geometry.subpixel)
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
        FSLLOG3_ERROR("unknown subpixel orientation {}", output->geometry.subpixel);
        subpixel_orientation = "unexpected value";
        break;
      }

      switch (output->geometry.output_transform)
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
        FSLLOG3_ERROR("unknown output transform {}", output->geometry.output_transform);
        transform = "unexpected value";
        break;
      }

      FSLLOG3_INFO("x: {}, y: {}", output->geometry.x, output->geometry.y);
      if (output->version >= 2)
        FSLLOG3_INFO("scale: {}", output->geometry.scale);

      FSLLOG3_INFO("physical_width: {} mm, physical_height: {} mm", output->geometry.physical_width, output->geometry.physical_height);
      FSLLOG3_INFO("make: {}, model: {}", output->geometry.make, output->geometry.model);
      FSLLOG3_INFO("subpixel_orientation: {}, output_transform: {}", subpixel_orientation, transform);

      wl_list_for_each(mode, &output->modes, link)
      {
        FSLLOG3_INFO("mode:");

        FSLLOG3_INFO("width: {} px, height: {} px, refresh: {} Hz", mode->width, mode->height, (float)mode->refresh / 1000);
        FSLLOG3_INFO("flags:");
        if (mode->flags & WL_OUTPUT_MODE_CURRENT)
          FSLLOG3_INFO(" current");
        if (mode->flags & WL_OUTPUT_MODE_PREFERRED)
          FSLLOG3_INFO(" preferred");
      }
    }

    static void output_handle_geometry(void* data, struct wl_output* wl_output, int32_t x, int32_t y, int32_t physical_width, int32_t physical_height,
                                       int32_t subpixel, const char* make, const char* model, int32_t output_transform)
    {
      struct output_info* output = (output_info*)data;
      assert(output != nullptr);

      output->geometry.x = x;
      output->geometry.y = y;
      output->geometry.physical_width = physical_width;
      output->geometry.physical_height = physical_height;
      output->geometry.subpixel = (wl_output_subpixel)subpixel;
      output->geometry.make = strdup(make);
      output->geometry.model = strdup(model);
      output->geometry.output_transform = (wl_output_transform)output_transform;
    }

    static void output_handle_mode(void* data, struct wl_output* wl_output, uint32_t flags, int32_t width, int32_t height, int32_t refresh)
    {
      struct output_info* output = (output_info*)data;
      assert(output != nullptr);

      struct output_mode* mode = (output_mode*)malloc(sizeof *mode);
      FSLLOG3_VERBOSE4("Allocated space for mode");

      mode->flags = flags;
      mode->width = width;
      mode->height = height;
      mode->refresh = refresh;

      wl_list_insert(output->modes.prev, &mode->link);
    }

    static void output_handle_done(void* data, struct wl_output* wl_output)
    {
      /* don't bother waiting for this; there's no good reason a
       * compositor will wait more than one roundtrip before sending
       * these initial events. */
    }

    static void output_handle_scale(void* data, struct wl_output* wl_output, int32_t scale)
    {
      struct output_info* output = (output_info*)data;
      assert(output != nullptr);

      output->geometry.scale = scale;
    }

    static const struct wl_output_listener output_listener = {
      output_handle_geometry,
      output_handle_mode,
      output_handle_done,
      output_handle_scale,
    };

    static void destroy_output_info(void* data)
    {
      struct output_info* output = (output_info*)data;
      assert(output != nullptr);
      struct output_mode *mode, *tmp;

      wl_output_destroy(output->output);

      if (output->geometry.make != NULL)
        free(output->geometry.make);
      if (output->geometry.model != NULL)
        free(output->geometry.model);

      wl_list_for_each_safe(mode, tmp, &output->modes, link)
      {
        wl_list_remove(&mode->link);
        FSLLOG3_VERBOSE4("Freed space for mode");
        free(mode);
      }
    }

    static void add_output_info(void* info, uint32_t id, uint32_t version)
    {
      struct display* display = (struct display*)info;
      assert(display != nullptr);

      struct output_info* output = (output_info*)malloc(sizeof *output);    // Hugo, was zalloc
      FSLLOG3_VERBOSE4("Allocated Space for output_info");

      init_global_info(info, &output->global, id, "wl_output", version);
      output->global.print = print_output_info;
      output->global.destroy = destroy_output_info;

      output->version = std::min(version, uint32_t{2});
      output->geometry.scale = 1;
      wl_list_init(&output->modes);

      output->output = (wl_output*)wl_registry_bind(display->registry, id, &wl_output_interface, output->version);
      wl_output_add_listener(output->output, &output_listener, output);

      display->roundtrip_needed = true;
      wl_list_insert(&display->outputs, &output->global_link);
    }

    // static void print_infos(struct wl_list* infos)
    // {
    //   struct global_info* info;
    //   assert(infos != nullptr);

    //   wl_list_for_each(info, infos, link) info->print(info);
    // }

    static void destroy_info(void* data)
    {
      struct global_info* global = (global_info*)data;
      assert(global != nullptr);

      global->destroy(data);
      wl_list_remove(&global->link);

      FSLLOG3_VERBOSE4("Freed space for interface at {}", global->interface);
      free(global->interface);
      FSLLOG3_VERBOSE4("Freed space for global_info at {}", data);
      free(data);
    }

    static void destroy_infos(struct wl_list* infos)
    {
      struct global_info *info, *tmp;
      wl_list_for_each_safe(info, tmp, infos, link) destroy_info(info);
    }


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

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

    const wl_seat_listener seat_listener = {
      SeatHandleCapabilities,
    };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

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
      else if (strcmp(interface, "wl_output") == 0)
      {
        add_output_info(data, name, version);
      }
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
      else if (strcmp(interface, "ivi_application") == 0)
      {
        d->ivi_application = (struct ivi_application*)wl_registry_bind(registry, name, &ivi_application_interface, 1);
      }
#endif
    }

    void RegistryHandleRemove(void* data, struct wl_registry* registry, uint32_t name)
    {
    }

    const wl_registry_listener registry_listener = {RegistryHandleGlobal, RegistryHandleRemove};

    uint32_t CalcDPI(const uint32_t width, const uint32_t millimeterWidth)
    {
      assert(width > 0);
      assert(millimeterWidth > 0);
      // 1mm = 0.0393701f inches
      const auto w = static_cast<double>(width);
      const double inchesWidth = static_cast<double>(millimeterWidth) * 0.0393701;
      return static_cast<uint32_t>(std::round(w / inchesWidth));
    }

    void TryUpdateDPI(const uint32_t& width, const uint32_t& height, const uint32_t& physicalWidth, const uint32_t& physicalHeight,
                      Point2& rScreenDPI)
    {
      if (width > 0 && height > 0 && physicalWidth > 0 && physicalHeight > 0)
      {
        FSLLOG3_VERBOSE4("PlatformNativeWindowWayland| UpdateDPI: ");
        FSLLOG3_VERBOSE4("- screenSize.width: {}", width);
        FSLLOG3_VERBOSE4("- screenSize.height: {}", height);
        FSLLOG3_VERBOSE4("- screenSize.mwidth: {}", physicalWidth);
        FSLLOG3_VERBOSE4("- screenSize.mheight: {}", physicalHeight);
        rScreenDPI = Point2(CalcDPI(width, physicalWidth), CalcDPI(height, physicalHeight));
        FSLLOG3_VERBOSE4("- DPI: {}, {}", rScreenDPI.X, rScreenDPI.Y);
      }
      else
      {
        FSLLOG3_WARNING("Failed to acquire proper DPI information, using defaults");
      }
    }

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

      wl_list_init(&sdisplay.infos);
      wl_list_init(&sdisplay.outputs);

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
      FSLLOG3_WARNING("wl_display_flush Failure");
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
    : PlatformNativeWindow(nativeWindowSetup, platformWindowParams, pPlatformCustomWindowAllocationParams, NativeWindowCapabilityFlags::GetDpi)
  {
    const NativeWindowConfig nativeWindowConfig = nativeWindowSetup.GetConfig();
    g_eventQueue = nativeWindowSetup.GetEventQueue();

    FSLLOG3_WARNING_IF(nativeWindowSetup.GetConfig().GetDisplayId() != 0, "Wayland only supports the main display. Using DisplayId 0 instead of {}",
                       nativeWindowSetup.GetConfig().GetDisplayId());
    if (nativeWindowConfig.GetWindowMode() != WindowMode::Window)
    {
      FSLLOG3_INFO_IF(nativeWindowSetup.GetVerbosityLevel() > 0, "Window Size/Position not defined, setting them to MAX Display Resolution");
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
      swindow.window_size.width = 480;
      swindow.window_size.height = 504;
      swindow.fullscreen = 0;
#else
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

    FSLLOG3_INFO_IF(nativeWindowSetup.GetVerbosityLevel() > 0, "PlatformNativeWindowWayland: Creating window: (Width={} Height={} fullscreen: {})",
                    swindow.window_size.width, swindow.window_size.height, swindow.fullscreen);


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
      m_destroyWindowCallback = platformWindowParams.DestroyWaylandWindow;
    }
    if (platformWindowParams.ResizeWaylandWindow)
    {
      swindow.resizeWindowCallback = platformWindowParams.ResizeWaylandWindow;
    }

    do
    {
      sdisplay.roundtrip_needed = false;
      wl_display_roundtrip(sdisplay.display);
    } while (sdisplay.roundtrip_needed);

    GetOutputs(&sdisplay.infos);

    TryUpdateDPI(m_displayOutput[0].width, m_displayOutput[0].height, m_displayOutput[0].physicalWidth, m_displayOutput[0].physicalHeight,
                 m_cachedScreenDPI);

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
      if (m_destroyWindowCallback)
        m_destroyWindowCallback(window->native);
    }

    destroy_infos(&display->infos);

#ifndef FSL_WINDOWSYSTEM_WAYLAND_IVI
    if (display->shell)
      wl_shell_surface_destroy(window->shell_surface);
#endif
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


  bool PlatformNativeWindowWayland::TryGetNativeSize(PxPoint2& rSize) const
  {
    auto width = swindow.window_size.width;
    auto height = swindow.window_size.height;
    if (width <= 0 || height <= 0)
    {
      return false;
    }

    rSize = PxPoint2(width, height);
    return true;
  }


  bool PlatformNativeWindowWayland::TryGetNativeDpi(Vector2& rDPI) const
  {
    rDPI = Vector2(m_cachedScreenDPI.X, m_cachedScreenDPI.Y);
    return true;
  }


  void PlatformNativeWindowWayland::GetOutputs(struct wl_list* infos)
  {
    struct global_info* info;
    wl_list_for_each(info, infos, link)
    {
      struct output_info* output = (output_info*)info;
      struct output_mode* mode;
      waylandDisplayGeometry currentDisplayGeometry{};
      currentDisplayGeometry.physicalWidth = output->geometry.physical_width;
      currentDisplayGeometry.physicalHeight = output->geometry.physical_height;
      wl_list_for_each(mode, &output->modes, link)
      {
        if (mode->flags & WL_OUTPUT_MODE_CURRENT)
        {
          currentDisplayGeometry.width = mode->width;
          currentDisplayGeometry.height = mode->height;
        }
      }
      m_displayOutput.push_back(currentDisplayGeometry);
    }
  }

}
#endif
