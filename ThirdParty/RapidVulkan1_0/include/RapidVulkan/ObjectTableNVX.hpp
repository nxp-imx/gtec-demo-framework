#ifndef RAPIDVULKAN_OBJECTTABLENVX_HPP
#define RAPIDVULKAN_OBJECTTABLENVX_HPP
#if VK_HEADER_VERSION >= 37
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
  class ObjectTableNVX
  {
    VkDevice m_device;
    VkObjectTableNVX m_objectTable;
  public:
    ObjectTableNVX(const ObjectTableNVX&) = delete;
    ObjectTableNVX& operator=(const ObjectTableNVX&) = delete;

    //! @brief Move assignment operator
    ObjectTableNVX& operator=(ObjectTableNVX&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_device = other.m_device;
        m_objectTable = other.m_objectTable;

        // Remove the data from other
        other.m_device = VK_NULL_HANDLE;
        other.m_objectTable = VK_NULL_HANDLE;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    ObjectTableNVX(ObjectTableNVX&& other)
      : m_device(other.m_device)
      , m_objectTable(other.m_objectTable)
    {
      // Remove the data from other
      other.m_device = VK_NULL_HANDLE;
      other.m_objectTable = VK_NULL_HANDLE;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    ObjectTableNVX()
      : m_device(VK_NULL_HANDLE)
      , m_objectTable(VK_NULL_HANDLE)
    {
    }

    //! @brief Assume control of the ObjectTableNVX (this object becomes responsible for releasing it)
    explicit ObjectTableNVX(const ClaimMode claimMode, const VkDevice device, const VkObjectTableNVX objectTable)
      : ObjectTableNVX()
    {
      Reset(claimMode, device, objectTable);
    }

#if VK_HEADER_VERSION >= 37
    //! @brief Create the requested resource
    //! @note  Function: vkCreateObjectTableNVX
    ObjectTableNVX(const VkDevice device, const VkObjectTableCreateInfoNVX& createInfo)
      : ObjectTableNVX()
    {
      Reset(device, createInfo);
    }
#endif

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Create the requested resource
    //! @note  Function: vkCreateObjectTableNVX
    ObjectTableNVX(const VkDevice device, const uint32_t objectCount, const VkObjectEntryTypeNVX * pObjectEntryTypes, const uint32_t * pObjectEntryCounts, const VkObjectEntryUsageFlagsNVX * pObjectEntryUsageFlags, const uint32_t maxUniformBuffersPerDescriptor, const uint32_t maxStorageBuffersPerDescriptor, const uint32_t maxStorageImagesPerDescriptor, const uint32_t maxSampledImagesPerDescriptor, const uint32_t maxPipelineLayouts)
      : ObjectTableNVX()
    {
      Reset(device, objectCount, pObjectEntryTypes, pObjectEntryCounts, pObjectEntryUsageFlags, maxUniformBuffersPerDescriptor, maxStorageBuffersPerDescriptor, maxStorageImagesPerDescriptor, maxSampledImagesPerDescriptor, maxPipelineLayouts);
    }
#endif

    ~ObjectTableNVX()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    VkObjectTableNVX Release() RAPIDVULKAN_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_objectTable;
      m_device = VK_NULL_HANDLE;
      m_objectTable = VK_NULL_HANDLE;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_device != VK_NULL_HANDLE);
      assert(m_objectTable != VK_NULL_HANDLE);

      vkDestroyObjectTableNVX(m_device, m_objectTable, nullptr);
      m_device = VK_NULL_HANDLE;
      m_objectTable = VK_NULL_HANDLE;
    }

    //! @brief Destroys any owned resources and assume control of the ObjectTableNVX (this object becomes responsible for releasing it)
    void Reset(const ClaimMode claimMode, const VkDevice device, const VkObjectTableNVX objectTable)
    {
      if (IsValid())
        Reset();


      m_device = device;
      m_objectTable = objectTable;
    }

#if VK_HEADER_VERSION >= 37
    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateObjectTableNVX
    void Reset(const VkDevice device, const VkObjectTableCreateInfoNVX& createInfo)
    {
#ifndef RAPIDVULKAN_DISABLE_PARAM_VALIDATION
      if (device == VK_NULL_HANDLE)
        throw std::invalid_argument("device can not be VK_NULL_HANDLE");
#else
      assert(device != VK_NULL_HANDLE);
#endif

      // Free any currently allocated resource
      if (IsValid())
        Reset();

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      VkObjectTableNVX objectTable;
      CheckError(vkCreateObjectTableNVX(device, &createInfo, nullptr, &objectTable), "vkCreateObjectTableNVX", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_device = device;
      m_objectTable = objectTable;
    }
#endif

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateObjectTableNVX
    void Reset(const VkDevice device, const uint32_t objectCount, const VkObjectEntryTypeNVX * pObjectEntryTypes, const uint32_t * pObjectEntryCounts, const VkObjectEntryUsageFlagsNVX * pObjectEntryUsageFlags, const uint32_t maxUniformBuffersPerDescriptor, const uint32_t maxStorageBuffersPerDescriptor, const uint32_t maxStorageImagesPerDescriptor, const uint32_t maxSampledImagesPerDescriptor, const uint32_t maxPipelineLayouts)
    {
      VkObjectTableCreateInfoNVX createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_OBJECT_TABLE_CREATE_INFO_NVX;
      createInfo.pNext = nullptr;
      createInfo.objectCount = objectCount;
      createInfo.pObjectEntryTypes = pObjectEntryTypes;
      createInfo.pObjectEntryCounts = pObjectEntryCounts;
      createInfo.pObjectEntryUsageFlags = pObjectEntryUsageFlags;
      createInfo.maxUniformBuffersPerDescriptor = maxUniformBuffersPerDescriptor;
      createInfo.maxStorageBuffersPerDescriptor = maxStorageBuffersPerDescriptor;
      createInfo.maxStorageImagesPerDescriptor = maxStorageImagesPerDescriptor;
      createInfo.maxSampledImagesPerDescriptor = maxSampledImagesPerDescriptor;
      createInfo.maxPipelineLayouts = maxPipelineLayouts;

      Reset(device, createInfo);
    }
#endif

    //! @brief Get the associated 'Device'
    VkDevice GetDevice() const
    {
      return m_device;
    }

    //! @brief Get the associated resource handle
    VkObjectTableNVX Get() const
    {
      return m_objectTable;
    }

    //! @brief Get a pointer to the associated resource handle
    const VkObjectTableNVX* GetPointer() const
    {
      return &m_objectTable;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_objectTable != VK_NULL_HANDLE;
    }


#if VK_HEADER_VERSION >= 37
    //! @note  Function: vkRegisterObjectsNVX
    void RegisterObjectsNVX(const uint32_t objectCount, const VkObjectTableEntryNVX *const * ppObjectTableEntries, const uint32_t * pObjectIndices)
    {
      CheckError(vkRegisterObjectsNVX(m_device, m_objectTable, objectCount, ppObjectTableEntries, pObjectIndices), "vkRegisterObjectsNVX", __FILE__, __LINE__);
    }
#endif


#if VK_HEADER_VERSION >= 37
    //! @note  Function: vkUnregisterObjectsNVX
    void UnregisterObjectsNVX(const uint32_t objectCount, const VkObjectEntryTypeNVX * pObjectEntryTypes, const uint32_t * pObjectIndices)
    {
      CheckError(vkUnregisterObjectsNVX(m_device, m_objectTable, objectCount, pObjectEntryTypes, pObjectIndices), "vkUnregisterObjectsNVX", __FILE__, __LINE__);
    }
#endif
  };
}

#endif
#endif
