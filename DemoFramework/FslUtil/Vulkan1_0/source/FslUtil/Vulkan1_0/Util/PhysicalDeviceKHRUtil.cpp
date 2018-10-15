/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceKHRUtil.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <RapidVulkan/Check.hpp>
#include <cassert>

namespace Fsl
{
  namespace Vulkan
  {
    namespace PhysicalDeviceKHRUtil
    {
      std::vector<VkPresentModeKHR> GetPhysicalDeviceSurfacePresentModesKHR(const VkPhysicalDevice device, const VkSurfaceKHR surface)
      {
        uint32_t count;
        RAPIDVULKAN_CHECK2(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, nullptr), "failed to acquire the count");

        std::vector<VkPresentModeKHR> result(count);
        RAPIDVULKAN_CHECK2(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, result.data()), "failed to get the present modes");
        return result;
      }


      std::vector<VkSurfaceFormatKHR> GetPhysicalDeviceSurfaceFormatsKHR(const VkPhysicalDevice device, const VkSurfaceKHR surface)
      {
        uint32_t count;
        RAPIDVULKAN_CHECK2(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr), "failed to acquire the device count");

        std::vector<VkSurfaceFormatKHR> result(count);
        RAPIDVULKAN_CHECK2(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, result.data()), "failed to get surface formats");

        return result;
      }


      std::vector<bool> GetPhysicalDeviceSurfaceSupportKHR(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface,
                                                           const std::size_t queueFamilyIndexCount)
      {
        std::vector<bool> supported;
        for (uint32_t currentQueueFamilyIndex = 0; currentQueueFamilyIndex < queueFamilyIndexCount; ++currentQueueFamilyIndex)
        {
          VkBool32 isSupported;
          RAPIDVULKAN_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, currentQueueFamilyIndex, surface, &isSupported));
          supported.push_back(isSupported != VK_FALSE);
        }
        return supported;
      }


#ifndef __ANDROID__
      std::vector<VkDisplayPropertiesKHR> GetPhysicalDeviceDisplayPropertiesKHR(const VkPhysicalDevice device)
      {
        uint32_t count;
        RAPIDVULKAN_CHECK2(vkGetPhysicalDeviceDisplayPropertiesKHR(device, &count, nullptr), "failed to acquire the count");

        std::vector<VkDisplayPropertiesKHR> result(count);
        RAPIDVULKAN_CHECK2(vkGetPhysicalDeviceDisplayPropertiesKHR(device, &count, result.data()), "failed to get the DeviceDisplayPropertiesKHR");
        return result;
      }


      std::vector<VkDisplayModePropertiesKHR> GetDisplayModePropertiesKHR(const VkPhysicalDevice device, const VkDisplayKHR display)
      {
        uint32_t count;
        RAPIDVULKAN_CHECK2(vkGetDisplayModePropertiesKHR(device, display, &count, nullptr), "failed to acquire the count");

        std::vector<VkDisplayModePropertiesKHR> result(count);
        RAPIDVULKAN_CHECK2(vkGetDisplayModePropertiesKHR(device, display, &count, result.data()), "failed to get the display mode properties");
        return result;
      }


      std::vector<VkDisplayPlanePropertiesKHR> GetPhysicalDeviceDisplayPlanePropertiesKHR(const VkPhysicalDevice device)
      {
        uint32_t count;
        RAPIDVULKAN_CHECK2(vkGetPhysicalDeviceDisplayPlanePropertiesKHR(device, &count, nullptr), "failed to acquire the plane count");

        std::vector<VkDisplayPlanePropertiesKHR> result(count);
        RAPIDVULKAN_CHECK2(vkGetPhysicalDeviceDisplayPlanePropertiesKHR(device, &count, result.data()), "failed to get plane properties");

        return result;
      }

      std::vector<VkDisplayKHR> GetDisplayPlaneSupportedDisplaysKHR(const VkPhysicalDevice device, const uint32_t planeIndex)
      {
        uint32_t count;
        RAPIDVULKAN_CHECK2(vkGetDisplayPlaneSupportedDisplaysKHR(device, planeIndex, &count, nullptr),
                           "failed to acquire the supported display count");

        std::vector<VkDisplayKHR> result(count);
        RAPIDVULKAN_CHECK2(vkGetDisplayPlaneSupportedDisplaysKHR(device, planeIndex, &count, result.data()),
                           "failed to get plane supported displays");
        return result;
      }
#endif
    }
  }
}
