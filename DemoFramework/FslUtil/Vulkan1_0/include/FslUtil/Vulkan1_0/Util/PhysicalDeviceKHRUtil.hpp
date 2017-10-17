#ifndef FSLUTIL_VULKAN1_0_UTIL_PHYSICALDEVICEKHRUTIL_HPP
#define FSLUTIL_VULKAN1_0_UTIL_PHYSICALDEVICEKHRUTIL_HPP
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
#include <vulkan/vulkan.h>
#include <vector>

namespace Fsl
{
  namespace Vulkan
  {
    namespace PhysicalDeviceKHRUtil
    {
      //! @brief Get the physical devices surface present modes
      std::vector<VkPresentModeKHR> GetPhysicalDeviceSurfacePresentModesKHR(const VkPhysicalDevice device, const VkSurfaceKHR surface);

      //! @brief Get the physical devices surface formats
      std::vector<VkSurfaceFormatKHR> GetPhysicalDeviceSurfaceFormatsKHR(const VkPhysicalDevice device, const VkSurfaceKHR surface);

      std::vector<bool> GetPhysicalDeviceSurfaceSupportKHR(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, const std::size_t queueFamilyIndexCount);


#ifndef __ANDROID__
      //! @brief Get the physical devices display properties
      std::vector<VkDisplayPropertiesKHR> GetPhysicalDeviceDisplayPropertiesKHR(const VkPhysicalDevice device);

      //! @brief Get the display mode properties.
      std::vector<VkDisplayModePropertiesKHR> GetDisplayModePropertiesKHR(const VkPhysicalDevice device, const VkDisplayKHR display);

      //! @brief Get the physical devices display plane properties
      std::vector<VkDisplayPlanePropertiesKHR> GetPhysicalDeviceDisplayPlanePropertiesKHR(const VkPhysicalDevice device);

      //! @brief Get the supported displays for the plane
      std::vector<VkDisplayKHR> GetDisplayPlaneSupportedDisplaysKHR(const VkPhysicalDevice device, const uint32_t planeIndex);
#endif
    }
  }
}

#endif
