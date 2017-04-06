/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslNativeWindowEGL/EGLNativeWindowSystemFactory.hpp>
#include <FslNativeWindowEGL/IEGLNativeWindowSystem.hpp>

#if defined(_WIN32)
  #include "Win32/EGLNativeWindowSystemWin32.hpp"
  namespace Fsl
  {
    typedef EGLNativeWindowSystemWin32 EGLNATIVEWINDOWSYSTEM;
  }
#elif defined(__ANDROID__)
  #include "Android/EGLNativeWindowSystemAndroid.hpp"
  namespace Fsl
  {
    typedef EGLNativeWindowSystemAndroid EGLNATIVEWINDOWSYSTEM;
  }
#elif defined(__QNXNTO__)
  #include "QNX/EGLNativeWindowSystemQNX.hpp"
  namespace Fsl
  {
    typedef EGLNativeWindowSystemQNX EGLNATIVEWINDOWSYSTEM;
  }
#elif defined(__linux__)
  #if defined(FSL_WINDOWSYSTEM_X11)
    #include "X11/EGLNativeWindowSystemX11.hpp"
    namespace Fsl
    {
      typedef EGLNativeWindowSystemX11 EGLNATIVEWINDOWSYSTEM;
    }
  #elif defined(FSL_WINDOWSYSTEM_WAYLAND)
    #include "Wayland/EGLNativeWindowSystemWayland.hpp"
    namespace Fsl
    {
      typedef EGLNativeWindowSystemWayland EGLNATIVEWINDOWSYSTEM;
    }
  #elif defined(FSL_WINDOWSYSTEM_FRAMEBUFFER)
    #include "FB/EGLNativeWindowSystemFB.hpp"
    namespace Fsl
    {
      typedef EGLNativeWindowSystemFB EGLNATIVEWINDOWSYSTEM;
    }
  #else
    #error No window system set
  #endif
#else
  #error Unsupported platform
#endif

namespace Fsl
{
  const std::shared_ptr<IEGLNativeWindowSystem> EGLNativeWindowSystemFactory::Allocate(const NativeWindowSystemSetup& setup)
  {
    return std::make_shared<EGLNATIVEWINDOWSYSTEM>(setup);
  }

  const std::shared_ptr<INativeWindowSystem> EGLNativeWindowSystemFactory::AllocateNative(const NativeWindowSystemSetup& setup)
  {
    return Allocate(setup);
  }
}
