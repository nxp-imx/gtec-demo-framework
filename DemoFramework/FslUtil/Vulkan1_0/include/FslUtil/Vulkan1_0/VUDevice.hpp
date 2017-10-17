#ifndef FSLUTIL_VULKAN1_0_VUDEVICE_HPP
#define FSLUTIL_VULKAN1_0_VUDEVICE_HPP
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

#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslUtil/Vulkan1_0/VUPhysicalDeviceRecord.hpp>
#include <RapidVulkan/Device.hpp>

namespace Fsl
{
  namespace Vulkan
  {
    //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
    class VUDevice
    {
      VUPhysicalDeviceRecord m_physicalDevice;
      RapidVulkan::Device m_device;
    public:
      VUDevice(const VUDevice&) = delete;
      VUDevice& operator=(const VUDevice&) = delete;

      //! @brief Move assignment operator
      VUDevice& operator=(VUDevice&& other);

      //! @brief Move constructor
      //! Transfer ownership from other to this
      VUDevice(VUDevice&& other);

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      VUDevice();
      VUDevice(const VkPhysicalDevice physicalDevice, RapidVulkan::Device&& device);
      VUDevice(const VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo& createInfo);

      ~VUDevice()
      {
        Reset();
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset();
      void Reset(const VkPhysicalDevice physicalDevice, RapidVulkan::Device&& device);

      //! @brief Destroys any owned resources and then creates the requested one
      //! @note  Function: vkCreateDevice
      void Reset(const VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo& createInfo);

      //! @brief Destroys any owned resources and then creates the requested one
      //! @note  Function: vkCreateDevice
      void Reset(const VkPhysicalDevice physicalDevice, const VkDeviceCreateFlags flags, const uint32_t queueCreateInfoCount, VkDeviceQueueCreateInfo*const pQueueCreateInfos, const uint32_t enabledLayerCount, const char *const * ppEnabledLayerNames, const uint32_t enabledExtensionCount, const char *const * ppEnabledExtensionNames, VkPhysicalDeviceFeatures*const pEnabledFeatures);

      //! @brief Get the physical device
      const VUPhysicalDeviceRecord& GetPhysicalDevice() const
      {
        return m_physicalDevice;
      }

      //! @brief Get the associated 'Device'
      VkDevice Get() const
      {
        return m_device.Get();
      }

      //! @brief Get a pointer to the associated resource handle
      const VkDevice* GetPointer() const
      {
        return m_device.GetPointer();
      }

      bool IsValid() const
      {
        return m_device.IsValid();
      }

      //! @note  Function: vkGetDeviceProcAddr
      PFN_vkVoidFunction GetDeviceProcAddr(const char * pName)
      {
        return m_device.GetDeviceProcAddr(pName);
      }

      //! @note  Function: vkGetDeviceQueue
      void GetDeviceQueue(const uint32_t queueFamilyIndex, const uint32_t queueIndex, VkQueue * pQueue)
      {
        return m_device.GetDeviceQueue(queueFamilyIndex, queueIndex, pQueue);
      }

      //! @note  Function: vkDeviceWaitIdle
      void DeviceWaitIdle()
      {
        m_device.DeviceWaitIdle();
      }

      //! @note  Function: vkFlushMappedMemoryRanges
      void FlushMappedMemoryRanges(const uint32_t memoryRangeCount, const VkMappedMemoryRange * pMemoryRanges)
      {
        m_device.FlushMappedMemoryRanges(memoryRangeCount, pMemoryRanges);
      }

      //! @note  Function: vkInvalidateMappedMemoryRanges
      void InvalidateMappedMemoryRanges(const uint32_t memoryRangeCount, const VkMappedMemoryRange * pMemoryRanges)
      {
        m_device.InvalidateMappedMemoryRanges(memoryRangeCount, pMemoryRanges);
      }

      //! @note  Function: vkResetFences
      void ResetFences(const uint32_t fenceCount, const VkFence * pFences)
      {
        m_device.ResetFences(fenceCount, pFences);
      }

      //! @note  Function: vkWaitForFences
      void WaitForFences(const uint32_t fenceCount, const VkFence * pFences, const VkBool32 waitAll, const uint64_t timeout)
      {
        m_device.WaitForFences(fenceCount, pFences, waitAll, timeout);
      }

      //! @note  Function: vkUpdateDescriptorSets
      void UpdateDescriptorSets(const uint32_t descriptorWriteCount, const VkWriteDescriptorSet * pDescriptorWrites, const uint32_t descriptorCopyCount, const VkCopyDescriptorSet * pDescriptorCopies)
      {
        m_device.UpdateDescriptorSets(descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
      }

      //! @note  Function: vkDebugMarkerSetObjectTagEXT
      // void DebugMarkerSetObjectTagEXT(VkDebugMarkerObjectTagInfoEXT * pTagInfo)
      // {
      // m_device.DebugMarkerSetObjectTagEXT(pTagInfo);
      // }

      //! @note  Function: vkDebugMarkerSetObjectNameEXT
      // void DebugMarkerSetObjectNameEXT(VkDebugMarkerObjectNameInfoEXT * pNameInfo)
      // {
      // m_device.DebugMarkerSetObjectNameEXT(pNameInfo);
      // }

    };
  }
}

#endif
