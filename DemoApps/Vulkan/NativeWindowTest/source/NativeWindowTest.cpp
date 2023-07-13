/****************************************************************************************************************************************************
 * Copyright 2019, 2023 NXP
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

#include "NativeWindowTest.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxPoint2.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslNativeWindow/Vulkan/IVulkanNativeWindow.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Log/All.hpp>
#include <FslUtil/Vulkan1_0/Log/FmtAll.hpp>
#include <FslUtil/Vulkan1_0/SurfaceFormatInfo.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Util/SwapchainKHRUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Debug/Strings/VkCompositeAlphaFlagBitsKHR.hpp>
#include <RapidVulkan/Debug/Strings/VkImageUsageFlagBits.hpp>
#include <RapidVulkan/Debug/Strings/VkSurfaceTransformFlagBitsKHR.hpp>
#include <vulkan/vulkan.h>
// Included last as a workaround to ensure all types are found
#include <FslUtil/Vulkan1_0/Debug/BitFlags.hpp>
#include <fmt/ostream.h>
#include <sstream>


namespace Fsl
{
  namespace
  {
    // The desired minimum image count for the swap buffers
    const uint32_t DESIRED_MIN_SWAP_BUFFER_COUNT = 2;

#ifdef FSL_WINDOWSYSTEM_WAYLAND
    const bool SWAPCHAIN_DEPENDENT_SURFACE = true;
#else
    const bool SWAPCHAIN_DEPENDENT_SURFACE = false;
#endif

    void LogSurfaceCapabilities(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
    {
      FSLLOG3_INFO("- surfaceCapabilities.minImageCount: {}", surfaceCapabilities.minImageCount);
      FSLLOG3_INFO("- surfaceCapabilities.maxImageCount: {}", surfaceCapabilities.maxImageCount);
      FSLLOG3_INFO("- surfaceCapabilities.currentExtent: {}", surfaceCapabilities.currentExtent);
      FSLLOG3_INFO("- surfaceCapabilities.minImageExtent: {}", surfaceCapabilities.minImageExtent);
      FSLLOG3_INFO("- surfaceCapabilities.maxImageExtent: {}", surfaceCapabilities.maxImageExtent);
      FSLLOG3_INFO("- surfaceCapabilities.maxImageArrayLayers: {}", surfaceCapabilities.maxImageArrayLayers);
      FSLLOG3_INFO("- surfaceCapabilities.supportedTransforms: {}",
                   Vulkan::Debug::GetBitflagsString(static_cast<VkSurfaceTransformFlagBitsKHR>(surfaceCapabilities.supportedTransforms)));
      FSLLOG3_INFO("- surfaceCapabilities.currentTransform: {}", Vulkan::Debug::GetBitflagsString(surfaceCapabilities.currentTransform));
      FSLLOG3_INFO("- surfaceCapabilities.supportedCompositeAlpha: {}",
                   Vulkan::Debug::GetBitflagsString(static_cast<VkCompositeAlphaFlagBitsKHR>(surfaceCapabilities.supportedCompositeAlpha)));
      FSLLOG3_INFO("- surfaceCapabilities.supportedUsageFlags: {}",
                   Vulkan::Debug::GetBitflagsString(static_cast<VkImageUsageFlagBits>(surfaceCapabilities.supportedUsageFlags)));
    }

    void CheckPhysicalDeviceSurfaceCapabilitiesPreSwapchain(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface,
                                                            const VkExtent2D windowExtent, const bool swapchainDependentSurface)
    {
      VkSurfaceCapabilitiesKHR surfaceCapabilities{};
      RAPIDVULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));
      LogSurfaceCapabilities(surfaceCapabilities);

      if (swapchainDependentSurface)
      {
        if (surfaceCapabilities.currentExtent.width != 0xFFFFFFFF || surfaceCapabilities.currentExtent.height != 0xFFFFFFFF)
        {
          std::stringstream errStream;
          errStream << "VulkanDriverError: driver did not report surfaceCapabilities.currentExtent.width == 0xFFFFFFFF && "
                    << "surfaceCapabilities.currentExtent.height == 0xFFFFFFFF as required by the standard for this window system. "
                    << "It reported surfaceCapabilities.currentExtent " << surfaceCapabilities.currentExtent;

          throw RapidVulkan::VulkanException(errStream.str(), __FILE__, __LINE__);
        }
      }
      else if (surfaceCapabilities.currentExtent.width != windowExtent.width || surfaceCapabilities.currentExtent.height != windowExtent.height)
      {
        std::stringstream errStream;
        errStream << "VulkanDriverError: surfaceCapabilities.currentExtent (" << surfaceCapabilities.currentExtent
                  << ") did not match the window extent (" << windowExtent << ")";
        throw RapidVulkan::VulkanException(errStream.str(), __FILE__, __LINE__);
      }
    }


    void CheckPhysicalDeviceSurfaceCapabilitiesPostSwapchain(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface,
                                                             const VkExtent2D windowExtent, const bool swapchainDependentSurface)
    {
      VkSurfaceCapabilitiesKHR surfaceCapabilities{};
      RAPIDVULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));
      LogSurfaceCapabilities(surfaceCapabilities);

      if (swapchainDependentSurface)
      {
        if (surfaceCapabilities.currentExtent.width != 0xFFFFFFFF || surfaceCapabilities.currentExtent.height != 0xFFFFFFFF)
        {
          std::stringstream errStream;
          errStream << "VulkanDriverError: driver did not report surfaceCapabilities.currentExtent.width == 0xFFFFFFFF && "
                    << "surfaceCapabilities.currentExtent.height == 0xFFFFFFFF as required by the standard for this window system. "
                    << "It reported surfaceCapabilities.currentExtent " << surfaceCapabilities.currentExtent;

          throw RapidVulkan::VulkanException(errStream.str(), __FILE__, __LINE__);
        }
      }
      else
      {
        if (surfaceCapabilities.currentExtent.width != windowExtent.width || surfaceCapabilities.currentExtent.height != windowExtent.height)
        {
          std::stringstream errStream;
          errStream << "VulkanDriverError: surfaceCapabilities.currentExtent (" << surfaceCapabilities.currentExtent
                    << ") did not match the window extent (" << windowExtent << ") as expected after the swapchain has been created";
          throw RapidVulkan::VulkanException(errStream.str(), __FILE__, __LINE__);
        }
      }
    }
  }


  NativeWindowTest::NativeWindowTest(const DemoAppConfig& config)
    : VulkanWindowDemoApp(config)
  {
    auto windowExtent = TypeConverter::UncheckedTo<VkExtent2D>(GetScreenExtent());
    PxPoint2 actualSize;
    if (GetNativeWindow()->TryGetActualSize(actualSize))
    {
      if (UncheckedNumericCast<uint32_t>(actualSize.X.Value) != windowExtent.width)
      {
        FSLLOG3_WARNING("actual native window width does not match reported width: {} != {}", actualSize.X, windowExtent.width);
      }
      if (UncheckedNumericCast<uint32_t>(actualSize.Y.Value) != windowExtent.height)
      {
        FSLLOG3_WARNING("actual native window height does not match reported height: {} != {}", actualSize.Y, windowExtent.height);
      }
    }
    else
    {
      FSLLOG3_WARNING("TryGetActualSize not supported");
    }

    FSLLOG3_INFO("WindowExtent: {}", windowExtent);

    FSLLOG3_INFO("Checking surface capabilities pre-swapchain creation");
    CheckPhysicalDeviceSurfaceCapabilitiesPreSwapchain(m_physicalDevice.Device, m_surface, windowExtent, SWAPCHAIN_DEPENDENT_SURFACE);

    FSLLOG3_INFO("Creating swapchain");

    const VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    const VkImageUsageFlags desiredImageUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto swapchain = Vulkan::SwapchainKHRUtil::CreateSwapchain(
      m_physicalDevice.Device, m_device.Get(), 0, m_surface, DESIRED_MIN_SWAP_BUFFER_COUNT, 1, desiredImageUsageFlags, VK_SHARING_MODE_EXCLUSIVE, 0,
      nullptr, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, presentMode, VK_TRUE, VK_NULL_HANDLE, windowExtent, Vulkan::SurfaceFormatInfo());

    FSLLOG3_INFO("Checking surface capabilities post-swapchain creation");
    CheckPhysicalDeviceSurfaceCapabilitiesPostSwapchain(m_physicalDevice.Device, m_surface, windowExtent, SWAPCHAIN_DEPENDENT_SURFACE);

    FSLLOG3_INFO("\n*** Checks completed successfully ***\n");
    GetDemoAppControl()->RequestExit();
  }
}
