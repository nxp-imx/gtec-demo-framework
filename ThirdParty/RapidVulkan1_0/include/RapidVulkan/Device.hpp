#ifndef RAPIDVULKAN_DEVICE_HPP
#define RAPIDVULKAN_DEVICE_HPP
//***************************************************************************************************************************************************
//* BSD 3-Clause License
//*
//* Copyright (c) 2016, Rene Thrane
//* All rights reserved.
//*
//* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//*
//* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
//*    documentation and/or other materials provided with the distribution.
//* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this
//*    software without specific prior written permission.
//*
//* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//***************************************************************************************************************************************************

// Auto-generated Vulkan 1.0 C++11 RAII classes by RAIIGen (https://github.com/Unarmed1000/RAIIGen)

#include <RapidVulkan/ClaimMode.hpp>
#include <RapidVulkan/CheckError.hpp>
#include <RapidVulkan/System/Macro.hpp>
#include <vulkan/vulkan.h>
#include <cassert>

namespace RapidVulkan
{
  //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
  class Device
  {
    VkDevice m_device;
  public:
    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    //! @brief Move assignment operator
    Device& operator=(Device&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_device = other.m_device;

        // Remove the data from other
        other.m_device = VK_NULL_HANDLE;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    Device(Device&& other)
      : m_device(other.m_device)
    {
      // Remove the data from other
      other.m_device = VK_NULL_HANDLE;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    Device()
      : m_device(VK_NULL_HANDLE)
    {
    }

    //! @brief Assume control of the Device (this object becomes responsible for releasing it)
    explicit Device(const ClaimMode claimMode, const VkDevice device)
      : Device()
    {
      Reset(claimMode, device);
    }

    //! @brief Create the requested resource
    //! @note  Function: vkCreateDevice
    Device(const VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo& createInfo)
      : Device()
    {
      Reset(physicalDevice, createInfo);
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Create the requested resource
    //! @note  Function: vkCreateDevice
    Device(const VkPhysicalDevice physicalDevice, const VkDeviceCreateFlags flags, const uint32_t queueCreateInfoCount, VkDeviceQueueCreateInfo*const pQueueCreateInfos, const uint32_t enabledLayerCount, const char *const * ppEnabledLayerNames, const uint32_t enabledExtensionCount, const char *const * ppEnabledExtensionNames, VkPhysicalDeviceFeatures*const pEnabledFeatures)
      : Device()
    {
      Reset(physicalDevice, flags, queueCreateInfoCount, pQueueCreateInfos, enabledLayerCount, ppEnabledLayerNames, enabledExtensionCount, ppEnabledExtensionNames, pEnabledFeatures);
    }
#endif

    ~Device()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    VkDevice Release() RAPIDVULKAN_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_device;
      m_device = VK_NULL_HANDLE;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_device != VK_NULL_HANDLE);

      vkDestroyDevice(m_device, nullptr);
      m_device = VK_NULL_HANDLE;
    }

    //! @brief Destroys any owned resources and assume control of the Device (this object becomes responsible for releasing it)
    void Reset(const ClaimMode claimMode, const VkDevice device)
    {
      if (IsValid())
        Reset();


      m_device = device;
    }

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateDevice
    void Reset(const VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo& createInfo)
    {
#ifndef RAPIDVULKAN_DISABLE_PARAM_VALIDATION
#else
#endif

      // Free any currently allocated resource
      if (IsValid())
        Reset();

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      VkDevice device;
      CheckError(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), "vkCreateDevice", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_device = device;
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateDevice
    void Reset(const VkPhysicalDevice physicalDevice, const VkDeviceCreateFlags flags, const uint32_t queueCreateInfoCount, VkDeviceQueueCreateInfo*const pQueueCreateInfos, const uint32_t enabledLayerCount, const char *const * ppEnabledLayerNames, const uint32_t enabledExtensionCount, const char *const * ppEnabledExtensionNames, VkPhysicalDeviceFeatures*const pEnabledFeatures)
    {
      VkDeviceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      createInfo.pNext = nullptr;
      createInfo.flags = flags;
      createInfo.queueCreateInfoCount = queueCreateInfoCount;
      createInfo.pQueueCreateInfos = pQueueCreateInfos;
      createInfo.enabledLayerCount = enabledLayerCount;
      createInfo.ppEnabledLayerNames = ppEnabledLayerNames;
      createInfo.enabledExtensionCount = enabledExtensionCount;
      createInfo.ppEnabledExtensionNames = ppEnabledExtensionNames;
      createInfo.pEnabledFeatures = pEnabledFeatures;

      Reset(physicalDevice, createInfo);
    }
#endif

    //! @brief Get the associated resource handle
    VkDevice Get() const
    {
      return m_device;
    }

    //! @brief Get a pointer to the associated resource handle
    const VkDevice* GetPointer() const
    {
      return &m_device;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_device != VK_NULL_HANDLE;
    }

    //! @note  Function: vkGetDeviceProcAddr
    PFN_vkVoidFunction GetDeviceProcAddr(const char * pName)
    {
      return vkGetDeviceProcAddr(m_device, pName);
    }

    //! @note  Function: vkGetDeviceQueue
    void GetDeviceQueue(const uint32_t queueFamilyIndex, const uint32_t queueIndex, VkQueue * pQueue)
    {
      vkGetDeviceQueue(m_device, queueFamilyIndex, queueIndex, pQueue);
    }

    //! @note  Function: vkDeviceWaitIdle
    void DeviceWaitIdle()
    {
      CheckError(vkDeviceWaitIdle(m_device), "vkDeviceWaitIdle", __FILE__, __LINE__);
    }

    //! @note  Function: vkFlushMappedMemoryRanges
    void FlushMappedMemoryRanges(const uint32_t memoryRangeCount, const VkMappedMemoryRange * pMemoryRanges)
    {
      CheckError(vkFlushMappedMemoryRanges(m_device, memoryRangeCount, pMemoryRanges), "vkFlushMappedMemoryRanges", __FILE__, __LINE__);
    }

    //! @note  Function: vkInvalidateMappedMemoryRanges
    void InvalidateMappedMemoryRanges(const uint32_t memoryRangeCount, const VkMappedMemoryRange * pMemoryRanges)
    {
      CheckError(vkInvalidateMappedMemoryRanges(m_device, memoryRangeCount, pMemoryRanges), "vkInvalidateMappedMemoryRanges", __FILE__, __LINE__);
    }

    //! @note  Function: vkResetFences
    void ResetFences(const uint32_t fenceCount, const VkFence * pFences)
    {
      CheckError(vkResetFences(m_device, fenceCount, pFences), "vkResetFences", __FILE__, __LINE__);
    }

    //! @note  Function: vkWaitForFences
    void WaitForFences(const uint32_t fenceCount, const VkFence * pFences, const VkBool32 waitAll, const uint64_t timeout)
    {
      CheckError(vkWaitForFences(m_device, fenceCount, pFences, waitAll, timeout), "vkWaitForFences", __FILE__, __LINE__);
    }

    //! @note  Function: vkUpdateDescriptorSets
    void UpdateDescriptorSets(const uint32_t descriptorWriteCount, const VkWriteDescriptorSet * pDescriptorWrites, const uint32_t descriptorCopyCount, const VkCopyDescriptorSet * pDescriptorCopies)
    {
      vkUpdateDescriptorSets(m_device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
    }
  };
}

#endif
