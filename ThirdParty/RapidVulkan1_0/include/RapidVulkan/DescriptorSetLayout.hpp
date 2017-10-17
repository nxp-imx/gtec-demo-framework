#ifndef RAPIDVULKAN_DESCRIPTORSETLAYOUT_HPP
#define RAPIDVULKAN_DESCRIPTORSETLAYOUT_HPP
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
  class DescriptorSetLayout
  {
    VkDevice m_device;
    VkDescriptorSetLayout m_setLayout;
  public:
    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

    //! @brief Move assignment operator
    DescriptorSetLayout& operator=(DescriptorSetLayout&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_device = other.m_device;
        m_setLayout = other.m_setLayout;

        // Remove the data from other
        other.m_device = VK_NULL_HANDLE;
        other.m_setLayout = VK_NULL_HANDLE;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    DescriptorSetLayout(DescriptorSetLayout&& other)
      : m_device(other.m_device)
      , m_setLayout(other.m_setLayout)
    {
      // Remove the data from other
      other.m_device = VK_NULL_HANDLE;
      other.m_setLayout = VK_NULL_HANDLE;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    DescriptorSetLayout()
      : m_device(VK_NULL_HANDLE)
      , m_setLayout(VK_NULL_HANDLE)
    {
    }

    //! @brief Assume control of the DescriptorSetLayout (this object becomes responsible for releasing it)
    explicit DescriptorSetLayout(const ClaimMode claimMode, const VkDevice device, const VkDescriptorSetLayout setLayout)
      : DescriptorSetLayout()
    {
      Reset(claimMode, device, setLayout);
    }

    //! @brief Create the requested resource
    //! @note  Function: vkCreateDescriptorSetLayout
    DescriptorSetLayout(const VkDevice device, const VkDescriptorSetLayoutCreateInfo& createInfo)
      : DescriptorSetLayout()
    {
      Reset(device, createInfo);
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Create the requested resource
    //! @note  Function: vkCreateDescriptorSetLayout
    DescriptorSetLayout(const VkDevice device, const VkDescriptorSetLayoutCreateFlags flags, const uint32_t bindingCount, VkDescriptorSetLayoutBinding*const pBindings)
      : DescriptorSetLayout()
    {
      Reset(device, flags, bindingCount, pBindings);
    }
#endif

    ~DescriptorSetLayout()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    VkDescriptorSetLayout Release() RAPIDVULKAN_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_setLayout;
      m_device = VK_NULL_HANDLE;
      m_setLayout = VK_NULL_HANDLE;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_device != VK_NULL_HANDLE);
      assert(m_setLayout != VK_NULL_HANDLE);

      vkDestroyDescriptorSetLayout(m_device, m_setLayout, nullptr);
      m_device = VK_NULL_HANDLE;
      m_setLayout = VK_NULL_HANDLE;
    }

    //! @brief Destroys any owned resources and assume control of the DescriptorSetLayout (this object becomes responsible for releasing it)
    void Reset(const ClaimMode claimMode, const VkDevice device, const VkDescriptorSetLayout setLayout)
    {
      if (IsValid())
        Reset();


      m_device = device;
      m_setLayout = setLayout;
    }

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateDescriptorSetLayout
    void Reset(const VkDevice device, const VkDescriptorSetLayoutCreateInfo& createInfo)
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
      VkDescriptorSetLayout setLayout;
      CheckError(vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &setLayout), "vkCreateDescriptorSetLayout", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_device = device;
      m_setLayout = setLayout;
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateDescriptorSetLayout
    void Reset(const VkDevice device, const VkDescriptorSetLayoutCreateFlags flags, const uint32_t bindingCount, VkDescriptorSetLayoutBinding*const pBindings)
    {
      VkDescriptorSetLayoutCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      createInfo.pNext = nullptr;
      createInfo.flags = flags;
      createInfo.bindingCount = bindingCount;
      createInfo.pBindings = pBindings;

      Reset(device, createInfo);
    }
#endif

    //! @brief Get the associated 'Device'
    VkDevice GetDevice() const
    {
      return m_device;
    }

    //! @brief Get the associated resource handle
    VkDescriptorSetLayout Get() const
    {
      return m_setLayout;
    }

    //! @brief Get a pointer to the associated resource handle
    const VkDescriptorSetLayout* GetPointer() const
    {
      return &m_setLayout;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_setLayout != VK_NULL_HANDLE;
    }
  };
}

#endif
