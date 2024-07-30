#ifdef FSL_WINDOWSYSTEM_WIN32
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

#include "VulkanNativeWindowAdapterWin32.hpp"
#include <FslNativeWindow/Vulkan/NativeVulkanSetup.hpp>
#include <RapidVulkan/Check.hpp>
#include "VulkanNativeWindowSystemAdapterWin32.hpp"

namespace Fsl
{
  namespace
  {
    const auto g_platformKhrSurfaceExtensionName = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;


    std::shared_ptr<IPlatformNativeWindowAdapter>
      AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                     const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<VulkanNativeWindowAdapterWin32>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams);
    }
  }    // namespace


  VulkanNativeWindowSystemAdapterWin32::VulkanNativeWindowSystemAdapterWin32(const NativeWindowSystemSetup& setup)
    : VulkanNativeWindowSystemAdapterTemplate<PlatformNativeWindowSystemAdapterWin32>(setup, g_platformKhrSurfaceExtensionName, AllocateWindow)
  {
  }


  VulkanNativeWindowAdapterWin32::VulkanNativeWindowAdapterWin32(
    const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
    const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    : VulkanNativeWindowAdapter<PlatformNativeWindowAdapterWin32>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams)
  {
    VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfoKHR{};
    win32SurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    win32SurfaceCreateInfoKHR.flags = 0;
    win32SurfaceCreateInfoKHR.hinstance = GetPlatformDisplay();
    win32SurfaceCreateInfoKHR.hwnd = GetPlatformWindow();

    RAPIDVULKAN_CHECK(vkCreateWin32SurfaceKHR(m_setup.Instance, &win32SurfaceCreateInfoKHR, nullptr, &m_surface));
  }


  VulkanNativeWindowAdapterWin32::~VulkanNativeWindowAdapterWin32() = default;


  PlatformNativeWindowType VulkanNativeWindowAdapterWin32::GetWindowType() const
  {
    return GetPlatformWindow();
  }
}
#endif
