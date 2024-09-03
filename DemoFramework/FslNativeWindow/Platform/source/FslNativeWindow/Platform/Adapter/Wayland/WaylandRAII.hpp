#ifndef FSLNATIVEWINDOW_PLATFORM_ADAPTER_WAYLAND_WAYLANDRAII_HPP
#define FSLNATIVEWINDOW_PLATFORM_ADAPTER_WAYLAND_WAYLANDRAII_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <df-xdg-decoration-client-protocol.h>    // XDG wayland-scanner created header
#include <df-xdg-shell-client-protocol.h>         // XDG wayland-scanner created header
#include <wayland-client.h>
#include <wayland-cursor.h>
#include <memory>

namespace Fsl
{
  struct CustomDeleter_wl_callback
  {
    void operator()(wl_callback* pVal)
    {
      if (pVal != nullptr)
      {
        wl_callback_destroy(pVal);
      }
    }
  };


  struct CustomDeleter_wl_compositor
  {
    void operator()(wl_compositor* pVal)
    {
      if (pVal != nullptr)
      {
        wl_compositor_destroy(pVal);
      }
    }
  };

  // struct CustomDeleter_wl_cursor
  // {
  //   void operator()(wl_cursor* pVal)
  //   {
  //     FSL_PARAM_NOT_USED(pVal);
  //     // Cursors do not need to be destroyed
  //     // if(pVal != nullptr)
  //     // {
  //     //   wl_cursor_destroy(pVal);
  //     // }
  //   }
  // };

  struct CustomDeleter_wl_cursor_theme
  {
    void operator()(wl_cursor_theme* pVal)
    {
      if (pVal != nullptr)
      {
        wl_cursor_theme_destroy(pVal);
      }
    }
  };

  struct CustomDeleter_wl_display
  {
    void operator()(wl_display* pVal)
    {
      if (pVal != nullptr)
      {
        wl_display_disconnect(pVal);
      }
    }
  };

  struct CustomDeleter_wl_keyboard
  {
    void operator()(wl_keyboard* pVal)
    {
      if (pVal != nullptr)
      {
        // wl_keyboard_release(pVal);
        wl_keyboard_destroy(pVal);
      }
    }
  };


  struct CustomDeleter_wl_output
  {
    void operator()(wl_output* pVal)
    {
      if (pVal != nullptr)
      {
        wl_output_destroy(pVal);
      }
    }
  };


  struct CustomDeleter_wl_pointer
  {
    void operator()(wl_pointer* pVal)
    {
      if (pVal != nullptr)
      {
        // wl_pointer_release(pVal);
        wl_pointer_destroy(pVal);
      }
    }
  };

  struct CustomDeleter_wl_seat
  {
    void operator()(wl_seat* pVal)
    {
      if (pVal != nullptr)
      {
        wl_seat_release(pVal);
      }
    }
  };


  struct CustomDeleter_wl_shm
  {
    void operator()(wl_shm* pVal)
    {
      if (pVal != nullptr)
      {
        wl_shm_destroy(pVal);
      }
    }
  };

  struct CustomDeleter_wl_surface
  {
    void operator()(wl_surface* pVal)
    {
      if (pVal != nullptr)
      {
        wl_surface_destroy(pVal);
      }
    }
  };

  struct CustomDeleter_xdg_wm_base
  {
    void operator()(xdg_wm_base* pVal)
    {
      if (pVal != nullptr)
      {
        xdg_wm_base_destroy(pVal);
      }
    }
  };


  struct CustomDeleter_xdg_surface
  {
    void operator()(xdg_surface* pVal)
    {
      if (pVal != nullptr)
      {
        xdg_surface_destroy(pVal);
      }
    }
  };


  struct CustomDeleter_xdg_toplevel
  {
    void operator()(xdg_toplevel* pVal)
    {
      if (pVal != nullptr)
      {
        xdg_toplevel_destroy(pVal);
      }
    }
  };


  struct CustomDeleter_zxdg_decoration_manager_v1
  {
    void operator()(zxdg_decoration_manager_v1* pVal)
    {
      if (pVal != nullptr)
      {
        zxdg_decoration_manager_v1_destroy(pVal);
      }
    }
  };

  using ScopedWaylandCallback = std::unique_ptr<wl_callback, CustomDeleter_wl_callback>;
  using ScopedWaylandCompositor = std::unique_ptr<wl_compositor, CustomDeleter_wl_compositor>;
  // using ScopedWaylandCursor = std::unique_ptr<wl_cursor, CustomDeleter_wl_cursor>;
  using ScopedWaylandCursorTheme = std::unique_ptr<wl_cursor_theme, CustomDeleter_wl_cursor_theme>;
  using ScopedWaylandDisplay = std::unique_ptr<wl_display, CustomDeleter_wl_display>;
  using ScopedWaylandKeyboard = std::unique_ptr<wl_keyboard, CustomDeleter_wl_keyboard>;
  using ScopedWaylandOutput = std::unique_ptr<wl_output, CustomDeleter_wl_output>;
  using ScopedWaylandPointer = std::unique_ptr<wl_pointer, CustomDeleter_wl_pointer>;
  using ScopedWaylandSeat = std::unique_ptr<wl_seat, CustomDeleter_wl_seat>;
  using ScopedWaylandShm = std::unique_ptr<wl_shm, CustomDeleter_wl_shm>;
  using ScopedWaylandSurface = std::unique_ptr<wl_surface, CustomDeleter_wl_surface>;

  using ScopedWaylandXdgWmBase = std::unique_ptr<xdg_wm_base, CustomDeleter_xdg_wm_base>;
  using ScopedWaylandXdgSurface = std::unique_ptr<xdg_surface, CustomDeleter_xdg_surface>;
  using ScopedWaylandXdgToplevel = std::unique_ptr<xdg_toplevel, CustomDeleter_xdg_toplevel>;
  using ScopedWaylandXdgDecorationManagerV1 = std::unique_ptr<zxdg_decoration_manager_v1, CustomDeleter_zxdg_decoration_manager_v1>;

}

#endif
#endif
