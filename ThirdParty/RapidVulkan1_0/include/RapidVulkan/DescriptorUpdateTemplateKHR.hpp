#ifndef RAPIDVULKAN_DESCRIPTORUPDATETEMPLATEKHR_HPP
#define RAPIDVULKAN_DESCRIPTORUPDATETEMPLATEKHR_HPP
#if VK_HEADER_VERSION >= 42
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
  class DescriptorUpdateTemplateKHR
  {
    VkDevice m_device;
    VkDescriptorUpdateTemplateKHR m_descriptorUpdateTemplate;
  public:
    DescriptorUpdateTemplateKHR(const DescriptorUpdateTemplateKHR&) = delete;
    DescriptorUpdateTemplateKHR& operator=(const DescriptorUpdateTemplateKHR&) = delete;

    //! @brief Move assignment operator
    DescriptorUpdateTemplateKHR& operator=(DescriptorUpdateTemplateKHR&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_device = other.m_device;
        m_descriptorUpdateTemplate = other.m_descriptorUpdateTemplate;

        // Remove the data from other
        other.m_device = VK_NULL_HANDLE;
        other.m_descriptorUpdateTemplate = VK_NULL_HANDLE;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    DescriptorUpdateTemplateKHR(DescriptorUpdateTemplateKHR&& other)
      : m_device(other.m_device)
      , m_descriptorUpdateTemplate(other.m_descriptorUpdateTemplate)
    {
      // Remove the data from other
      other.m_device = VK_NULL_HANDLE;
      other.m_descriptorUpdateTemplate = VK_NULL_HANDLE;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    DescriptorUpdateTemplateKHR()
      : m_device(VK_NULL_HANDLE)
      , m_descriptorUpdateTemplate(VK_NULL_HANDLE)
    {
    }

    //! @brief Assume control of the DescriptorUpdateTemplateKHR (this object becomes responsible for releasing it)
    explicit DescriptorUpdateTemplateKHR(const ClaimMode claimMode, const VkDevice device, const VkDescriptorUpdateTemplateKHR descriptorUpdateTemplate)
      : DescriptorUpdateTemplateKHR()
    {
      Reset(claimMode, device, descriptorUpdateTemplate);
    }

#if VK_HEADER_VERSION >= 42
    //! @brief Create the requested resource
    //! @note  Function: vkCreateDescriptorUpdateTemplateKHR
    DescriptorUpdateTemplateKHR(const VkDevice device, const VkDescriptorUpdateTemplateCreateInfoKHR& createInfo)
      : DescriptorUpdateTemplateKHR()
    {
      Reset(device, createInfo);
    }
#endif

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Create the requested resource
    //! @note  Function: vkCreateDescriptorUpdateTemplateKHR
    DescriptorUpdateTemplateKHR(const VkDevice device, const VkDescriptorUpdateTemplateCreateFlagsKHR flags, const uint32_t descriptorUpdateEntryCount, VkDescriptorUpdateTemplateEntryKHR*const pDescriptorUpdateEntries, const VkDescriptorUpdateTemplateTypeKHR templateType, const VkDescriptorSetLayout descriptorSetLayout, const VkPipelineBindPoint pipelineBindPoint, const VkPipelineLayout pipelineLayout, const uint32_t set)
      : DescriptorUpdateTemplateKHR()
    {
      Reset(device, flags, descriptorUpdateEntryCount, pDescriptorUpdateEntries, templateType, descriptorSetLayout, pipelineBindPoint, pipelineLayout, set);
    }
#endif

    ~DescriptorUpdateTemplateKHR()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    VkDescriptorUpdateTemplateKHR Release() RAPIDVULKAN_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_descriptorUpdateTemplate;
      m_device = VK_NULL_HANDLE;
      m_descriptorUpdateTemplate = VK_NULL_HANDLE;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_device != VK_NULL_HANDLE);
      assert(m_descriptorUpdateTemplate != VK_NULL_HANDLE);

      vkDestroyDescriptorUpdateTemplateKHR(m_device, m_descriptorUpdateTemplate, nullptr);
      m_device = VK_NULL_HANDLE;
      m_descriptorUpdateTemplate = VK_NULL_HANDLE;
    }

    //! @brief Destroys any owned resources and assume control of the DescriptorUpdateTemplateKHR (this object becomes responsible for releasing it)
    void Reset(const ClaimMode claimMode, const VkDevice device, const VkDescriptorUpdateTemplateKHR descriptorUpdateTemplate)
    {
      if (IsValid())
        Reset();


      m_device = device;
      m_descriptorUpdateTemplate = descriptorUpdateTemplate;
    }

#if VK_HEADER_VERSION >= 42
    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateDescriptorUpdateTemplateKHR
    void Reset(const VkDevice device, const VkDescriptorUpdateTemplateCreateInfoKHR& createInfo)
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
      VkDescriptorUpdateTemplateKHR descriptorUpdateTemplate;
      CheckError(vkCreateDescriptorUpdateTemplateKHR(device, &createInfo, nullptr, &descriptorUpdateTemplate), "vkCreateDescriptorUpdateTemplateKHR", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_device = device;
      m_descriptorUpdateTemplate = descriptorUpdateTemplate;
    }
#endif

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateDescriptorUpdateTemplateKHR
    void Reset(const VkDevice device, const VkDescriptorUpdateTemplateCreateFlagsKHR flags, const uint32_t descriptorUpdateEntryCount, VkDescriptorUpdateTemplateEntryKHR*const pDescriptorUpdateEntries, const VkDescriptorUpdateTemplateTypeKHR templateType, const VkDescriptorSetLayout descriptorSetLayout, const VkPipelineBindPoint pipelineBindPoint, const VkPipelineLayout pipelineLayout, const uint32_t set)
    {
      VkDescriptorUpdateTemplateCreateInfoKHR createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO_KHR;
      createInfo.pNext = nullptr;
      createInfo.flags = flags;
      createInfo.descriptorUpdateEntryCount = descriptorUpdateEntryCount;
      createInfo.pDescriptorUpdateEntries = pDescriptorUpdateEntries;
      createInfo.templateType = templateType;
      createInfo.descriptorSetLayout = descriptorSetLayout;
      createInfo.pipelineBindPoint = pipelineBindPoint;
      createInfo.pipelineLayout = pipelineLayout;
      createInfo.set = set;

      Reset(device, createInfo);
    }
#endif

    //! @brief Get the associated 'Device'
    VkDevice GetDevice() const
    {
      return m_device;
    }

    //! @brief Get the associated resource handle
    VkDescriptorUpdateTemplateKHR Get() const
    {
      return m_descriptorUpdateTemplate;
    }

    //! @brief Get a pointer to the associated resource handle
    const VkDescriptorUpdateTemplateKHR* GetPointer() const
    {
      return &m_descriptorUpdateTemplate;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_descriptorUpdateTemplate != VK_NULL_HANDLE;
    }
  };
}

#endif
#endif
