/****************************************************************************************************************************************************
* Copyright (c) 2017 Freescale Semiconductor, Inc.
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

#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslBase/Log/Log.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    VUDevice& VUDevice::operator = (VUDevice&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_physicalDevice = other.m_physicalDevice;
        m_device = std::move(other.m_device);

        // Remove the data from other
        other.m_physicalDevice = VUPhysicalDeviceRecord();
      }
      return *this;
    }


    VUDevice::VUDevice(VUDevice&& other)
      : m_physicalDevice(other.m_physicalDevice)
      , m_device(std::move(other.m_device))
    {
      // Remove the data from other
      other.m_physicalDevice = VUPhysicalDeviceRecord();
    }


    VUDevice::VUDevice()
      : m_physicalDevice()
      , m_device()
    {
    }


    VUDevice::VUDevice(const VkPhysicalDevice physicalDevice, RapidVulkan::Device&& device)
      : VUDevice()
    {
      Reset(physicalDevice, std::move(device));
    }


    VUDevice::VUDevice(const VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo& createInfo)
      : VUDevice()
    {
      Reset(physicalDevice, createInfo);
    }


    void VUDevice::Reset()
    {
      if (!IsValid())
        return;

      assert(m_device.IsValid());

      m_physicalDevice = VUPhysicalDeviceRecord();
      m_device.Reset();
    }


    void VUDevice::Reset(const VkPhysicalDevice physicalDevice, RapidVulkan::Device&& device)
    {
      if (device.IsValid())
        throw std::invalid_argument("the device must be valid");

      if (IsValid())
        Reset();

      try
      {
        m_physicalDevice = VUPhysicalDeviceRecord(physicalDevice);
        m_device = std::move(device);
      }
      catch (const std::exception&)
      {
        m_physicalDevice.Reset();
        m_device.Reset();
        throw;
      }
    }


    void VUDevice::Reset(const VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo& createInfo)
    {
      // Free any currently allocated resource
      if (IsValid())
        Reset();

      try
      {
        m_physicalDevice = VUPhysicalDeviceRecord(physicalDevice);
        m_device.Reset(physicalDevice, createInfo);
      }
      catch (const std::exception&)
      {
        m_physicalDevice.Reset();
        m_device.Reset();
        throw;
      }
    }


    void VUDevice::Reset(const VkPhysicalDevice physicalDevice, const VkDeviceCreateFlags flags, const uint32_t queueCreateInfoCount, VkDeviceQueueCreateInfo*const pQueueCreateInfos, const uint32_t enabledLayerCount, const char *const * ppEnabledLayerNames, const uint32_t enabledExtensionCount, const char *const * ppEnabledExtensionNames, VkPhysicalDeviceFeatures*const pEnabledFeatures)
    {
      // Free any currently allocated resource
      if (IsValid())
        Reset();

      try
      {
        m_physicalDevice = VUPhysicalDeviceRecord(physicalDevice);
        m_device.Reset(physicalDevice, flags, queueCreateInfoCount, pQueueCreateInfos, enabledLayerCount, ppEnabledLayerNames, enabledExtensionCount, ppEnabledExtensionNames, pEnabledFeatures);
      }
      catch (const std::exception&)
      {
        m_physicalDevice.Reset();
        m_device.Reset();
        throw;
      }
    }
  }
}
