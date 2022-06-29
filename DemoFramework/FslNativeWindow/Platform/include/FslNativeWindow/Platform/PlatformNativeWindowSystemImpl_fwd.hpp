#ifndef FSLNATIVEWINDOW_PLATFORM_PLATFORMNATIVEWINDOWSYSTEMIMPL_FWD_HPP
#define FSLNATIVEWINDOW_PLATFORM_PLATFORMNATIVEWINDOWSYSTEMIMPL_FWD_HPP
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

// This file forward declares the platform dependent types:
// This allows you to use the types without pulling in nasty platform header files!

// Forward declares
// - PlatformNativeWindowSystemImpl
// / PlatformNativeWindowImpl

#if defined(FSL_PLATFORM_EMSCRIPTEN)
namespace Fsl
{
  class PlatformNativeWindowSDL;
  class PlatformNativeWindowSystemSDL;
  using PlatformNativeWindowImpl = PlatformNativeWindowSDL;
  using PlatformNativeWindowSystemImpl = PlatformNativeWindowSystemSDL;
}    // namespace Fsl
#elif defined(FSL_PLATFORM_WINDOWS)
namespace Fsl
{
  class PlatformNativeWindowWin32;
  class PlatformNativeWindowSystemWin32;
  using PlatformNativeWindowImpl = PlatformNativeWindowWin32;
  using PlatformNativeWindowSystemImpl = PlatformNativeWindowSystemWin32;
}    // namespace Fsl
#elif defined(__ANDROID__)
namespace Fsl
{
  class PlatformNativeWindowAndroid;
  class PlatformNativeWindowSystemAndroid;
  typedef PlatformNativeWindowAndroid PlatformNativeWindowImpl;
  typedef PlatformNativeWindowSystemAndroid PlatformNativeWindowSystemImpl;
}    // namespace Fsl
#elif defined(__QNXNTO__)
namespace Fsl
{
  class PlatformNativeWindowQNX;
  class PlatformNativeWindowSystemQNX;
  typedef PlatformNativeWindowQNX PlatformNativeWindowImpl;
  typedef PlatformNativeWindowSystemQNX PlatformNativeWindowSystemImpl;
}    // namespace Fsl
#elif defined(__linux__)
#if defined(FSL_WINDOWSYSTEM_X11)
namespace Fsl
{
  class PlatformNativeWindowX11;
  class PlatformNativeWindowSystemX11;
  using PlatformNativeWindowImpl = PlatformNativeWindowX11;
  using PlatformNativeWindowSystemImpl = PlatformNativeWindowSystemX11;
}    // namespace Fsl
#elif defined(FSL_WINDOWSYSTEM_WAYLAND)
namespace Fsl
{
  class PlatformNativeWindowWayland;
  class PlatformNativeWindowSystemWayland;
  typedef PlatformNativeWindowWayland PlatformNativeWindowImpl;
  typedef PlatformNativeWindowSystemWayland PlatformNativeWindowSystemImpl;
}    // namespace Fsl
#elif defined(FSL_WINDOWSYSTEM_FRAMEBUFFER)
namespace Fsl
{
  class PlatformNativeWindowFB;
  class PlatformNativeWindowSystemFB;
  using PlatformNativeWindowImpl = PlatformNativeWindowFB;
  using PlatformNativeWindowSystemImpl = PlatformNativeWindowSystemFB;
}    // namespace Fsl
#else
#error No PlatformNativeWindowSystemImpl available for this platform
#endif
#else
#error Unsupported platform
#endif

#endif
