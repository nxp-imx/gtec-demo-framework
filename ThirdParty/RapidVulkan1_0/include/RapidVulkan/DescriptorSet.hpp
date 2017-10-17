#ifndef RAPIDVULKAN_DESCRIPTORSET_HPP
#define RAPIDVULKAN_DESCRIPTORSET_HPP
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
  class DescriptorSet
  {
    VkDevice m_device;
    VkDescriptorPool m_descriptorPool;
    VkDescriptorSet m_descriptorSets;
  public:
    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;

    //! @brief Move assignment operator
    DescriptorSet& operator=(DescriptorSet&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_device = other.m_device;
        m_descriptorPool = other.m_descriptorPool;
        m_descriptorSets = other.m_descriptorSets;

        // Remove the data from other
        other.m_device = VK_NULL_HANDLE;
        other.m_descriptorPool = VK_NULL_HANDLE;
        other.m_descriptorSets = VK_NULL_HANDLE;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    DescriptorSet(DescriptorSet&& other)
      : m_device(other.m_device)
      , m_descriptorPool(other.m_descriptorPool)
      , m_descriptorSets(other.m_descriptorSets)
    {
      // Remove the data from other
      other.m_device = VK_NULL_HANDLE;
      other.m_descriptorPool = VK_NULL_HANDLE;
      other.m_descriptorSets = VK_NULL_HANDLE;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    DescriptorSet()
      : m_device(VK_NULL_HANDLE)
      , m_descriptorPool(VK_NULL_HANDLE)
      , m_descriptorSets(VK_NULL_HANDLE)
    {
    }

    //! @brief Assume control of the DescriptorSet (this object becomes responsible for releasing it)
    explicit DescriptorSet(const ClaimMode claimMode, const VkDevice device, const VkDescriptorPool descriptorPool, const VkDescriptorSet descriptorSets)
      : DescriptorSet()
    {
      Reset(claimMode, device, descriptorPool, descriptorSets);
    }

    //! @brief Create the requested resource
    //! @note  Function: vkAllocateDescriptorSets
    DescriptorSet(const VkDevice device, const VkDescriptorSetAllocateInfo& allocateInfo)
      : DescriptorSet()
    {
      Reset(device, allocateInfo);
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Create the requested resource
    //! @note  Function: vkAllocateDescriptorSets
    DescriptorSet(const VkDevice device, const VkDescriptorPool descriptorPool, const VkDescriptorSetLayout * pSetLayouts)
      : DescriptorSet()
    {
      Reset(device, descriptorPool, pSetLayouts);
    }
#endif

    ~DescriptorSet()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    VkDescriptorSet Release() RAPIDVULKAN_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_descriptorSets;
      m_device = VK_NULL_HANDLE;
      m_descriptorPool = VK_NULL_HANDLE;
      m_descriptorSets = VK_NULL_HANDLE;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_device != VK_NULL_HANDLE);
      assert(m_descriptorPool != VK_NULL_HANDLE);
      assert(m_descriptorSets != VK_NULL_HANDLE);

      vkFreeDescriptorSets(m_device, m_descriptorPool, 1, &m_descriptorSets);
      m_device = VK_NULL_HANDLE;
      m_descriptorPool = VK_NULL_HANDLE;
      m_descriptorSets = VK_NULL_HANDLE;
    }

    //! @brief Destroys any owned resources and assume control of the DescriptorSet (this object becomes responsible for releasing it)
    void Reset(const ClaimMode claimMode, const VkDevice device, const VkDescriptorPool descriptorPool, const VkDescriptorSet descriptorSets)
    {
      if (IsValid())
        Reset();


      m_device = device;
      m_descriptorPool = descriptorPool;
      m_descriptorSets = descriptorSets;
    }

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkAllocateDescriptorSets
    void Reset(const VkDevice device, const VkDescriptorSetAllocateInfo& allocateInfo)
    {
#ifndef RAPIDVULKAN_DISABLE_PARAM_VALIDATION
      if (device == VK_NULL_HANDLE)
        throw std::invalid_argument("device can not be VK_NULL_HANDLE");
      if (allocateInfo.descriptorPool == VK_NULL_HANDLE)
        throw std::invalid_argument("allocateInfo.descriptorPool can not be VK_NULL_HANDLE");
#else
      assert(device != VK_NULL_HANDLE);
      assert(allocateInfo.descriptorPool != VK_NULL_HANDLE);
      assert(allocateInfo.descriptorSetCount == 1);
#endif

      // Free any currently allocated resource
      if (IsValid())
        Reset();

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      VkDescriptorSet descriptorSets;
      CheckError(vkAllocateDescriptorSets(device, &allocateInfo, &descriptorSets), "vkAllocateDescriptorSets", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_device = device;
      m_descriptorPool = allocateInfo.descriptorPool;
      m_descriptorSets = descriptorSets;
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkAllocateDescriptorSets
    void Reset(const VkDevice device, const VkDescriptorPool descriptorPool, const VkDescriptorSetLayout * pSetLayouts)
    {
      VkDescriptorSetAllocateInfo allocateInfo{};
      allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      allocateInfo.pNext = nullptr;
      allocateInfo.descriptorPool = descriptorPool;
      allocateInfo.descriptorSetCount = 1;
      allocateInfo.pSetLayouts = pSetLayouts;

      Reset(device, allocateInfo);
    }
#endif

    //! @brief Get the associated 'Device'
    VkDevice GetDevice() const
    {
      return m_device;
    }

    //! @brief Get the associated 'DescriptorPool'
    VkDescriptorPool GetDescriptorPool() const
    {
      return m_descriptorPool;
    }

    //! @brief Get the associated resource handle
    VkDescriptorSet Get() const
    {
      return m_descriptorSets;
    }

    //! @brief Get a pointer to the associated resource handle
    const VkDescriptorSet* GetPointer() const
    {
      return &m_descriptorSets;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_descriptorSets != VK_NULL_HANDLE;
    }
  };
}

#endif
