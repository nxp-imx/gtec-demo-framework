#ifndef FSLUTIL_VULKAN1_0_UTIL_SWAPCHAINKHRUTIL_HPP
#define FSLUTIL_VULKAN1_0_UTIL_SWAPCHAINKHRUTIL_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslUtil/Vulkan1_0/VUSwapchainKHR.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace Fsl
{
  namespace Vulkan
  {
    namespace SwapchainKHRUtil
    {
      VUSwapchainKHR CreateSwapchain(const VkPhysicalDevice physicalDevice, const VkDevice device, const VkSwapchainCreateFlagsKHR flags,
                                     const VkSurfaceKHR surface, const uint32_t minImageCount, const uint32_t imageArrayLayers,
                                     const VkImageUsageFlags imageUsage, const VkSharingMode imageSharingMode,
                                     const uint32_t queueFamilyIndexCount, const uint32_t* queueFamilyIndices,
                                     const VkCompositeAlphaFlagBitsKHR compositeAlpha, const VkBool32 clipped,
                                     const VkSwapchainKHR oldSwapchain);

      VUSwapchainKHR CreateSwapchain(const VkPhysicalDevice physicalDevice, const VkDevice device, const VkSwapchainCreateFlagsKHR flags,
                                     const VkSurfaceKHR surface, const uint32_t minImageCount, const uint32_t imageArrayLayers,
                                     const VkImageUsageFlags imageUsage, const VkSharingMode imageSharingMode,
                                     const uint32_t queueFamilyIndexCount, const uint32_t* queueFamilyIndices,
                                     const VkCompositeAlphaFlagBitsKHR compositeAlpha, const VkPresentModeKHR presentMode,
                                     const VkBool32 clipped, const VkSwapchainKHR oldSwapchain);

      //! @brief Get the swap chain images
      std::vector<VkImage> GetSwapchainImagesKHR(const VkDevice device, const VkSwapchainKHR swapchain);
    }
  }
}

#endif
