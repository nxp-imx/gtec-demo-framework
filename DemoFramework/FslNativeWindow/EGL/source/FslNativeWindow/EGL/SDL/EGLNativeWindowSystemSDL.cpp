#ifdef FSL_PLATFORM_EMSCRIPTEN
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

#include "EGLNativeWindowSystemSDL.hpp"
#include <FslNativeWindow/Platform/SDL/PlatformNativeWindowSDL.hpp>
#include <FslNativeWindow/EGL/EGLNativeWindowTemplate.hpp>
#include <SDL2/SDL.h>

namespace Fsl
{
  namespace
  {
    PlatformNativeWindowParams Patch(const PlatformNativeWindowParams& initialParams)
    {
      PlatformNativeWindowParams params(initialParams);
      params.WindowFlags |= SDL_WINDOW_OPENGL;
      return params;
    }

    class EGLNativeWindowSDL : public EGLNativeWindowTemplate<PlatformNativeWindowSDL>
    {
      SDL_GLContext m_context{nullptr};

    public:
      EGLNativeWindowSDL(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& platformWindowParams,
                         const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
        : EGLNativeWindowTemplate<PlatformNativeWindowSDL>(nativeWindowSetup, Patch(platformWindowParams), pPlatformCustomWindowAllocationParams)
      {
        // SDL_WINDOW_OPENGL
        m_context = SDL_GL_CreateContext(GetSdlWindow());
      }

      ~EGLNativeWindowSDL()
      {
        if (m_context != nullptr)
        {
          SDL_GL_DeleteContext(m_context);
          m_context = nullptr;
        }
      }
    };

    std::shared_ptr<INativeWindow> AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                                                  const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<EGLNativeWindowTemplate<EGLNativeWindowSDL>>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams);
    }
  }


  EGLNativeWindowSystemSDL::EGLNativeWindowSystemSDL(const NativeWindowSystemSetup& setup)
    : EGLNativeWindowSystemTemplate<PlatformNativeWindowSystemSDL>(setup, AllocateWindow, EGL_DEFAULT_DISPLAY)
  {
  }
}
#endif
