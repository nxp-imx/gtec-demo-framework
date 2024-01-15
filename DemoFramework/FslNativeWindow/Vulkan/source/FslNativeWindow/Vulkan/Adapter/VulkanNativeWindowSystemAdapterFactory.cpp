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

#include <FslNativeWindow/Vulkan/Adapter/IVulkanNativeWindowSystemAdapter.hpp>
#include <FslNativeWindow/Vulkan/Adapter/VulkanNativeWindowSystemAdapterFactory.hpp>

#if defined(FSL_WINDOWSYSTEM_WIN32)
#include "Win32/VulkanNativeWindowSystemAdapterWin32.hpp"
namespace Fsl
{
  using VULKANNATIVEWINDOWSYSTEM = VulkanNativeWindowSystemAdapterWin32;
}
#elif defined(FSL_WINDOWSYSTEM_SDL2)
#include "SDL/VulkanNativeWindowSystemAdapterSDL.hpp"
namespace Fsl
{
  using VULKANNATIVEWINDOWSYSTEM = VulkanNativeWindowSystemAdapterSDL;
}
#elif defined(__ANDROID__)
#include "Android/VulkanNativeWindowSystemAdapterAndroid.hpp"
namespace Fsl
{
  typedef VulkanNativeWindowSystemAdapterAndroid VULKANNATIVEWINDOWSYSTEM;
}
#elif defined(__linux__)
#if defined(FSL_WINDOWSYSTEM_X11)
#include "X11/VulkanNativeWindowSystemAdapterX11.hpp"
namespace Fsl
{
  using VULKANNATIVEWINDOWSYSTEM = VulkanNativeWindowSystemAdapterX11;
}
#elif defined(FSL_WINDOWSYSTEM_WAYLAND)
#include "Wayland/VulkanNativeWindowSystemAdapterWayland.hpp"
namespace Fsl
{
  using VULKANNATIVEWINDOWSYSTEM = VulkanNativeWindowSystemAdapterWayland;
}
#elif defined(FSL_WINDOWSYSTEM_FRAMEBUFFER)
#include "FB/VulkanNativeWindowSystemAdapterFB.hpp"
namespace Fsl
{
  typedef VulkanNativeWindowSystemAdapterFB VULKANNATIVEWINDOWSYSTEM;
}
#else
#error No window system set
#endif
#else
// Unsupported window platform!
#error Unsupported window platform detected!
#endif

namespace Fsl
{
  std::unique_ptr<IVulkanNativeWindowSystemAdapter> VulkanNativeWindowSystemAdapterFactory::Allocate(const NativeWindowSystemSetup& setup)
  {
    return std::make_unique<VULKANNATIVEWINDOWSYSTEM>(setup);
  }
}
