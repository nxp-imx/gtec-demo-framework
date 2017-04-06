#ifndef FSLNATIVEWINDOWPLATFORM_PLATFORMNATIVEWINDOWSYSTEMIMPL_HPP
#define FSLNATIVEWINDOWPLATFORM_PLATFORMNATIVEWINDOWSYSTEMIMPL_HPP
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

#include <FslNativeWindowPlatform/PlatformNativeWindowSystemImpl_fwd.hpp>

// If you don't need access to the actual types then include the NativeWindowTypes_fwd.hpp file instead.
// It's only recommended to include this file from CPP files, if possible dont us it from header files!
// Pull in the actual class definitions.

#if defined(_WIN32)
  #include <FslNativeWindowPlatform/Win32/PlatformNativeWindowSystemWin32.hpp>
#elif defined(__ANDROID__)
  #include <FslNativeWindowPlatform/Android/PlatformNativeWindowSystemAndroid.hpp>
#elif defined(__QNXNTO__)
  #include <FslNativeWindowPlatform/QNX/PlatformNativeWindowSystemQNX.hpp>
#elif defined(__linux__)
  #if defined(FSL_WINDOWSYSTEM_X11)
    #include <FslNativeWindowPlatform/X11/PlatformNativeWindowSystemX11.hpp>
  #elif defined(FSL_WINDOWSYSTEM_WAYLAND)
    #include <FslNativeWindowPlatform/Wayland/PlatformNativeWindowSystemWayland.hpp>
  #elif defined(FSL_WINDOWSYSTEM_FRAMEBUFFER)
    #include <FslNativeWindowPlatform/FB/PlatformNativeWindowSystemFB.hpp>
  #else
    #error No window system set
  #endif
#else
  #error Unsupported platform
#endif


#endif
