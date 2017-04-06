#ifndef VULKANEXPERIMENTAL_VULKANUTIL_HPP
#define VULKANEXPERIMENTAL_VULKANUTIL_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslGraphicsVulkan1_0/Extend/SwapchainKHREx.hpp>
#include <FslGraphicsVulkan1_0/Instance.hpp>
#include <VulkanExperimental/DeviceQueue.hpp>
#include <VulkanExperimental/PhysicalDeviceEx.hpp>
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace Fsl
{
  namespace VulkanUtil
  {
    bool IsInstanceExtensionsAvailable(const uint32_t extensionCount, const char* const* enabledExtensionNames);

    Vulkan::Instance CreateInstance(const std::string& applicationName, const uint32_t applicationVersion,
                                    const uint32_t apiVersion, const VkInstanceCreateFlags flags, const uint32_t enabledLayerCount,
                                    const char* const* ppszEnabledLayerNames, const uint32_t enabledExtensionCount,
                                    const char* const* ppszEnabledExtensionNames);

    PhysicalDeviceEx GetPhysicalDevice(const VkInstance instance, const uint32_t index);
    bool TryGetQueueFamilyIndex(const std::vector<VkQueueFamilyProperties>& allQueueFamilyProperties, const VkQueueFlags queueFlags, const uint32_t queueIndex, const std::vector<bool>* pFilter, uint32_t& rQueueFamilyIndex);
    std::vector<bool> GetPhysicalDeviceSurfaceSupport(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, const std::size_t queueFamilyIndexCount);
    uint32_t GetFamilyIndex(const std::vector<VkQueueFamilyProperties>& allQueueFamilyProperties, const VkQueueFlags queueFlags, const uint32_t queueIndex, const std::vector<bool>* pFilter);
    DeviceQueue GetDeviceQueue(const VkDevice device, const uint32_t queueFamilyIndex, const uint32_t queueIndex);
    Vulkan::SwapchainKHREx CreateSwapchain(const VkPhysicalDevice physicalDevice, const VkDevice device, const VkSwapchainCreateFlagsKHR flags, const VkSurfaceKHR surface, const uint32_t minImageCount, const uint32_t imageArrayLayers, const VkImageUsageFlags imageUsage, const VkSharingMode imageSharingMode, const uint32_t queueFamilyIndexCount, const uint32_t* queueFamilyIndices, const VkCompositeAlphaFlagBitsKHR compositeAlpha, const VkBool32 clipped, const VkSwapchainKHR oldSwapchain);
    Vulkan::SwapchainKHREx CreateSwapchain(const VkPhysicalDevice physicalDevice, const VkDevice device, const VkSwapchainCreateFlagsKHR flags, const VkSurfaceKHR surface, const uint32_t minImageCount, const uint32_t imageArrayLayers, const VkImageUsageFlags imageUsage, const VkSharingMode imageSharingMode, const uint32_t queueFamilyIndexCount, const uint32_t* queueFamilyIndices, const VkCompositeAlphaFlagBitsKHR compositeAlpha, const VkPresentModeKHR presentMode, const VkBool32 clipped, const VkSwapchainKHR oldSwapchain);
  }
}

#endif
