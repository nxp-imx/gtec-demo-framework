#ifdef FSL_WINDOWSYSTEM_SDL2
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include "EGLNativeWindowSystemAdapterSDL.hpp"
#include <FslNativeWindow/EGL/Adapter/EGLNativeWindowAdapterTemplate.hpp>
#include <FslNativeWindow/Platform/Adapter/SDL/PlatformNativeWindowAdapterSDL.hpp>
#include <FslUtil/EGL/Exceptions.hpp>
#include <SDL2/SDL.h>

#ifdef SDL_VIDEO_DRIVER_WAYLAND
// SDL backed by Wayland is not supported as Wayland requires calls to wl_egl_window_create, wl_egl_window_destroy, wl_egl_window_resize to handle
// EGL windows see EGLNativeWindowSystemAdapterWayland.cpp.
// For this to be transparent we need to call SDL_GL_CreateContext however that requires us to handoff all the EGL setup code to SDL.
#error SDL backed by Wayland is not supported as it requires custom wayland setup code when working with EGL (See code comment)
#endif
namespace Fsl
{
  namespace
  {
    PlatformNativeWindowParams Patch(const PlatformNativeWindowParams& initialParams)
    {
      PlatformNativeWindowParams params(initialParams);
      // params.WindowFlags |= SDL_WINDOW_OPENGL;
      return params;
    }

    class EGLNativeWindowSDL : public EGLNativeWindowAdapterTemplate<PlatformNativeWindowAdapterSDL>
    {
      // SDL_GLContext m_context{nullptr};

    public:
      EGLNativeWindowSDL(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& platformWindowParams,
                         const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
        : EGLNativeWindowAdapterTemplate<PlatformNativeWindowAdapterSDL>(nativeWindowSetup, Patch(platformWindowParams),
                                                                         pPlatformCustomWindowAllocationParams)
      {
        //// SDL_WINDOW_OPENGL
        // SDL_Window* pWindow = GetSdlWindow();
        // m_context = SDL_GL_CreateContext(pWindow);
        // if(m_context == nullptr)
        //{
        //   throw GraphicsException("Failed to create GL context");
        // }
        // try
        //{
        //   if(SDL_GL_MakeCurrent(pWindow, m_context) != 0u)
        //   {
        //     throw GraphicsException("Failed to set the context");
        //   }
        // }
        // catch(const std::exception& ex)
        //{
        //   if (m_context != nullptr)
        //   {
        //     SDL_GL_DeleteContext(m_context);
        //     m_context = nullptr;
        //   }
        //   throw;
        // }
      }

      ~EGLNativeWindowSDL() override = default;
      // {
      // if (m_context != nullptr)
      //{
      //   SDL_GL_DeleteContext(m_context);
      //   m_context = nullptr;
      // }
      // }
    };

    std::shared_ptr<IPlatformNativeWindowAdapter>
      AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                     const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<EGLNativeWindowAdapterTemplate<EGLNativeWindowSDL>>(nativeWindowSetup, windowParams,
                                                                                  pPlatformCustomWindowAllocationParams);
    }
  }


  EGLNativeWindowSystemAdapterSDL::EGLNativeWindowSystemAdapterSDL(const NativeWindowSystemSetup& setup)
    : EGLNativeWindowSystemAdapterTemplate<PlatformNativeWindowSystemAdapterSDL>(setup, AllocateWindow, EGL_DEFAULT_DISPLAY,
                                                                                 PlatformNativeWindowSystemParams(true))
  {
  }
}
#endif
