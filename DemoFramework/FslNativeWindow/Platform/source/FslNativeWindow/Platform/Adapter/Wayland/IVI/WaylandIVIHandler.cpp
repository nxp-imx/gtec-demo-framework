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

#include "WaylandIVIHandler.hpp"
#include "../PlatformNativeWindowSystemContextWayland.hpp"
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
#include <ilm/ivi-application-client-protocol.h>
#include <sys/types.h>
#include <unistd.h>

#define IVI_SURFACE_ID 9000
#endif


namespace Fsl
{
  namespace
  {
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
    static void OnWaylandWindowContext_HandleIVISurfaceConfigure(void* data, struct ivi_surface* iviSurface, int32_t width, int32_t height)
    {
      PlatformNativeWindowContextWayland* window = (PlatformNativeWindowContextWayland*)data;

      if (window->ResizeWindowCallback)
      {
        window->ResizeWindowCallback(window->Native, width, height, 0, 0);
      }
      window->Geometry.Width = width;
      window->Geometry.Height = height;
      if (!window->Fullscreen)
      {
        window->WindowSize = window->Geometry;
      }
    }

    const struct ivi_surface_listener g_iviSurfaceListener = {OnWaylandWindowContext_HandleIVISurfaceConfigure};
#endif
  }


  void WaylandIVIHandler::Create(const PlatformNativeWindowSystemContextWayland& context, const PlatformNativeWindowContextWayland& window)
  {
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
    if (context.Handles.IviApplication)
    {
      uint32_t id_ivisurf = IVI_SURFACE_ID + (uint32_t)getpid();
      rWindow.Handles.Ivi.Surface.reset(ivi_application_surface_create(context.Handles.IviApplication.get(), id_ivisurf, window.Surface.get()));
      ivi_surface_add_listener(rWindow.Handles.Ivi.Surface.get(), &g_iviSurfaceListener, &window);
    }
#endif
  }


  bool WaylandIVIHandler::TryRegistryHandleGlobal(const PlatformNativeWindowSystemContextWayland& context, wl_registry* pRegistry, uint32_t name,
                                                  const char* interface, uint32_t version)
  {
#ifdef FSL_WINDOWSYSTEM_WAYLAND_IVI
    if (strcmp(interface, ivi_application_interface.name) == 0)
    {
        pContext->Handles.IviApplication.reset(static_cast<ivi_application*>(wl_registry_bind(pRegistry, name, &ivi_application_interface, 1));
        return true;
    }

#endif
    return false;
  }

}

#endif
