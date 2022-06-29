#if !defined(__ANDROID__) && defined(__linux__) && defined(FSL_WINDOWSYSTEM_X11)
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

#include "VulkanNativeWindowX11.hpp"
#include <FslNativeWindow/Vulkan/NativeVulkanSetup.hpp>
#include <RapidVulkan/Check.hpp>
#include "VulkanNativeWindowSystemX11.hpp"

namespace Fsl
{
  namespace
  {
    const auto PLATFORM_KHR_SURFACE_EXTENSION_NAME = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;


    std::shared_ptr<INativeWindow> AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                                                  const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<VulkanNativeWindowX11>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams);
    }
  }    // namespace


  VulkanNativeWindowSystemX11::VulkanNativeWindowSystemX11(const NativeWindowSystemSetup& setup)
    : VulkanNativeWindowSystemTemplate<PlatformNativeWindowSystemX11>(setup, PLATFORM_KHR_SURFACE_EXTENSION_NAME, AllocateWindow)
  {
  }


  VulkanNativeWindowX11::VulkanNativeWindowX11(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                                               const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    : VulkanNativeWindow<PlatformNativeWindowX11>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams)
  {
    VkXlibSurfaceCreateInfoKHR surfaceCreateInfo{};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = nullptr;
    surfaceCreateInfo.flags = 0;
    surfaceCreateInfo.dpy = GetPlatformDisplay();
    surfaceCreateInfo.window = GetPlatformWindow();

    RAPIDVULKAN_CHECK(vkCreateXlibSurfaceKHR(m_setup.Instance, &surfaceCreateInfo, nullptr, &m_surface));
  }


  VulkanNativeWindowX11::~VulkanNativeWindowX11() = default;


  PlatformNativeWindowType VulkanNativeWindowX11::GetWindowType() const
  {
    return GetPlatformWindow();
  }
}    // namespace Fsl
#endif
