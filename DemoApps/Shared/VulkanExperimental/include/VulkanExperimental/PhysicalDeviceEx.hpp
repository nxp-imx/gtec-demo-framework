#ifndef VULKANEXPERIMENTAL_PHYSICALDEVICEEX_HPP
#define VULKANEXPERIMENTAL_PHYSICALDEVICEEX_HPP
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
#include <FslGraphicsVulkan1_0/Util.hpp>
#include <FslGraphicsVulkan1_0/VulkanHelper.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  struct PhysicalDeviceEx
  {
    VkInstance Instance;
    VkPhysicalDevice Device;
    uint32_t Index;

    PhysicalDeviceEx()
      : Instance(VK_NULL_HANDLE)
      , Device{}
      , Index(0)
    {
    }

    PhysicalDeviceEx(const VkInstance instance, const VkPhysicalDevice& device, const uint32_t index)
      : Instance(instance)
      , Device(device)
      , Index(index)
    {
    }


    VkFormatProperties GetPhysicalDeviceFormatProperties(const VkFormat format) const
    {
      VkFormatProperties properties;
      vkGetPhysicalDeviceFormatProperties(Device, format, &properties);
      return properties;
    }


    void GetPhysicalDeviceFormatProperties(VkFormatProperties& rProperties, const VkFormat format) const
    {
      vkGetPhysicalDeviceFormatProperties(Device, format, &rProperties);
    }


    VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() const
    {
      VkPhysicalDeviceMemoryProperties properties;
      vkGetPhysicalDeviceMemoryProperties(Device, &properties);
      return properties;
    }

    void GetPhysicalDeviceMemoryProperties(VkPhysicalDeviceMemoryProperties& rProperties) const
    {
      vkGetPhysicalDeviceMemoryProperties(Device, &rProperties);
    }


    VkImageFormatProperties GetPhysicalDeviceImageFormatProperties(const VkFormat format, const VkImageType type, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageCreateFlags flags) const
    {
      VkImageFormatProperties properties;
      Vulkan::Util::Check(vkGetPhysicalDeviceImageFormatProperties(Device, format, type, tiling, usage, flags, &properties), "vkGetPhysicalDeviceImageFormatProperties", __FILE__, __LINE__);
      return properties;
    }


    bool TryGetPhysicalDeviceImageFormatProperties(const VkFormat format, const VkImageType type, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageCreateFlags flags, VkImageFormatProperties& rImageFormatProperties) const
    {
      return (vkGetPhysicalDeviceImageFormatProperties(Device, format, type, tiling, usage, flags, &rImageFormatProperties) == VK_SUCCESS);
    }


    bool IsImageTilingAvailable(const VkImageTiling imageTiling, const VkFormat format, const VkImageType type,
      const VkImageCreateFlags flags, const VkExtent3D& extent, const uint32_t mipLevels,
      const uint32_t arrayLayers, const VkSampleCountFlags sampleCounts, const VkDeviceSize resourceSize)
    {
      return Vulkan::VulkanHelper::IsImageTilingAvailable(Device, imageTiling, format, type, flags, extent, mipLevels, arrayLayers, sampleCounts, resourceSize);
    }

    bool IsValid() const
    {
      return Instance != VK_NULL_HANDLE && Device != VK_NULL_HANDLE;
    }
  };

}

#endif
