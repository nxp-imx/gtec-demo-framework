/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include "VulkanHostService.hpp"
#include <FslDemoHost/Vulkan/VulkanDemoHostOptionParser.hpp>

namespace Fsl
{
  VulkanHostService::VulkanHostService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
  {
  }


  VulkanHostService::~VulkanHostService() = default;


  void VulkanHostService::SetLaunchOptions(const VulkanLaunchOptions& value)
  {
    m_launchOptions = value;
  }

  void VulkanHostService::Clear()
  {
    m_instance = VK_NULL_HANDLE;
    m_physicalDevice = {};
    m_surfaceKHR = VK_NULL_HANDLE;
  }


  void VulkanHostService::SetInstance(VkInstance value)
  {
    m_instance = value;
  }


  void VulkanHostService::SetPhysicalDevice(const Vulkan::VUPhysicalDeviceRecord& value)
  {
    m_physicalDevice = value;
  }


  void VulkanHostService::SetSurfaceKHR(VkSurfaceKHR value)
  {
    m_surfaceKHR = value;
  }


  VkInstance VulkanHostService::GetInstance() const
  {
    return m_instance;
  }

  Vulkan::VUPhysicalDeviceRecord VulkanHostService::GetPhysicalDevice() const
  {
    return m_physicalDevice;
  }

  VkSurfaceKHR VulkanHostService::GetSurfaceKHR() const
  {
    return m_surfaceKHR;
  }


  VulkanLaunchOptions VulkanHostService::GetLaunchOptions() const
  {
    return m_launchOptions;
  }
}
