#ifndef FSLNATIVEWINDOW_PLATFORM_ADAPTER_PLATFORMNATIVEWINDOWSYSTEMADAPTERIMPL_FWD_HPP
#define FSLNATIVEWINDOW_PLATFORM_ADAPTER_PLATFORMNATIVEWINDOWSYSTEMADAPTERIMPL_FWD_HPP
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
// - PlatformNativeWindowSystemAdapterImpl
// / PlatformNativeWindowAdapterImpl

#if defined(FSL_WINDOWSYSTEM_SDL2)
namespace Fsl
{
  class PlatformNativeWindowAdapterSDL;
  class PlatformNativeWindowSystemAdapterSDL;
  using PlatformNativeWindowAdapterImpl = PlatformNativeWindowAdapterSDL;
  using PlatformNativeWindowSystemAdapterImpl = PlatformNativeWindowSystemAdapterSDL;
}    // namespace Fsl
#elif defined(FSL_WINDOWSYSTEM_WIN32)
namespace Fsl
{
  class PlatformNativeWindowAdapterWin32;
  class PlatformNativeWindowSystemAdapterWin32;
  using PlatformNativeWindowAdapterImpl = PlatformNativeWindowAdapterWin32;
  using PlatformNativeWindowSystemAdapterImpl = PlatformNativeWindowSystemAdapterWin32;
}    // namespace Fsl
#elif defined(__ANDROID__)
namespace Fsl
{
  class PlatformNativeWindowAdapterAndroid;
  class PlatformNativeWindowSystemAdapterAndroid;
  using PlatformNativeWindowAdapterImpl = PlatformNativeWindowAdapterAndroid;
  using PlatformNativeWindowSystemAdapterImpl = PlatformNativeWindowSystemAdapterAndroid;
}    // namespace Fsl
#elif defined(__QNXNTO__)
namespace Fsl
{
  class PlatformNativeWindowAdapterQNX;
  class PlatformNativeWindowSystemAdapterQNX;
  using PlatformNativeWindowAdapterImpl = PlatformNativeWindowAdapterQNX;
  using PlatformNativeWindowSystemAdapterImpl = PlatformNativeWindowSystemAdapterQNX;
}    // namespace Fsl
#elif defined(__linux__)
#if defined(FSL_WINDOWSYSTEM_X11)
namespace Fsl
{
  class PlatformNativeWindowAdapterX11;
  class PlatformNativeWindowSystemAdapterX11;
  using PlatformNativeWindowAdapterImpl = PlatformNativeWindowAdapterX11;
  using PlatformNativeWindowSystemAdapterImpl = PlatformNativeWindowSystemAdapterX11;
}    // namespace Fsl
#elif defined(FSL_WINDOWSYSTEM_WAYLAND)
namespace Fsl
{
  class PlatformNativeWindowAdapterWayland;
  class PlatformNativeWindowSystemAdapterWayland;
  using PlatformNativeWindowAdapterImpl = PlatformNativeWindowAdapterWayland;
  using PlatformNativeWindowSystemAdapterImpl = PlatformNativeWindowSystemAdapterWayland;
}    // namespace Fsl
#elif defined(FSL_WINDOWSYSTEM_FRAMEBUFFER)
namespace Fsl
{
  class PlatformNativeWindowAdapterFB;
  class PlatformNativeWindowSystemAdapterFB;
  using PlatformNativeWindowAdapterImpl = PlatformNativeWindowAdapterFB;
  using PlatformNativeWindowSystemAdapterImpl = PlatformNativeWindowSystemAdapterFB;
}    // namespace Fsl
#else
#error No PlatformNativeWindowSystemAdapterImpl available for this platform
#endif
#else
#error Unsupported platform
#endif

#endif
