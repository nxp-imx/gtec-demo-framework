#ifndef FSLNATIVEWINDOW_VULKAN_VULKANNATIVEWINDOW_HPP
#define FSLNATIVEWINDOW_VULKAN_VULKANNATIVEWINDOW_HPP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslNativeWindow/Base/NativeWindowSetup.hpp>
#include <FslNativeWindow/Vulkan/IVulkanNativeWindow.hpp>
#include <FslNativeWindow/Vulkan/NativeVulkanSetup.hpp>
#include <limits>

#ifdef _WIN32
#pragma warning(push)
// Disable the warning about inheritance via dominance
#pragma warning(disable : 4250)
#endif

namespace Fsl
{
  template <typename TPlatformNativeWindow>
  class VulkanNativeWindow
    : public TPlatformNativeWindow
    , public virtual IVulkanNativeWindow
  {
  protected:
    NativeVulkanSetup m_setup;
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};

  public:
    VulkanNativeWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                       const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
      : TPlatformNativeWindow(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams)
      , m_setup(ToNativeVulkanSetup(pPlatformCustomWindowAllocationParams))

    {
      if (m_setup.Instance == VK_NULL_HANDLE || m_setup.PhysicalDevice == VK_NULL_HANDLE)
      {
        throw std::invalid_argument("VkInstance or physicialDevice not valid");
      }
    }

    ~VulkanNativeWindow() override
    {
      vkDestroySurfaceKHR(m_setup.Instance, m_surface, nullptr);
    }

    bool TryCaptureMouse(const bool enableCapture) final
    {
      return TPlatformNativeWindow::TryCaptureMouse(enableCapture);
    }

    VkSurfaceKHR GetVulkanSurface() const final
    {
      return m_surface;
    }

    //! Done this way instead to prevent warnings from some compilers
    bool TryGetActualSize(PxPoint2& rSize) const final
    {
      return TPlatformNativeWindow::TryGetNativeSize(rSize);
    }

  protected:
    //! Done this way instead to prevent warnings from some compilers
    bool TryGetNativeSize(PxPoint2& rSize) const final
    {
      if (TryGetVulkanSurfaceSize(rSize))
      {
        return true;
      }
      return TryGetActualSize(rSize);
    }

    bool TryGetVulkanSurfaceSize(PxPoint2& rSize) const
    {
      VkSurfaceCapabilitiesKHR surfaceCapabilities;
      if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_setup.PhysicalDevice, m_surface, &surfaceCapabilities) != VK_SUCCESS)
      {
        return false;
      }

      if (surfaceCapabilities.currentExtent.width >= static_cast<uint32_t>(std::numeric_limits<PxPoint2::raw_value_type>::max()) ||
          surfaceCapabilities.currentExtent.height >= static_cast<uint32_t>(std::numeric_limits<PxPoint2::raw_value_type>::max()))
      {
        return false;
      }

      rSize = PxPoint2::Create(static_cast<PxPoint2::raw_value_type>(surfaceCapabilities.currentExtent.width),
                               static_cast<PxPoint2::raw_value_type>(surfaceCapabilities.currentExtent.height));
      return true;
    }

  private:
    static NativeVulkanSetup ToNativeVulkanSetup(const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      const auto* const pNativeSetup = dynamic_cast<const NativeVulkanSetup*>(pPlatformCustomWindowAllocationParams);
      if (pNativeSetup == nullptr)
      {
        throw NotSupportedException("NativeVulkanSetup pointer expected");
      }
      return *pNativeSetup;
    }
  };
}    // namespace Fsl
#endif

#ifdef _WIN32
#pragma warning(pop)
#endif
