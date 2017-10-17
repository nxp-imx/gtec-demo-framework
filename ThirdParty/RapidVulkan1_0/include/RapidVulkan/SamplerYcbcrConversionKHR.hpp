#ifndef RAPIDVULKAN_SAMPLERYCBCRCONVERSIONKHR_HPP
#define RAPIDVULKAN_SAMPLERYCBCRCONVERSIONKHR_HPP
#if VK_HEADER_VERSION >= 61
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
  class SamplerYcbcrConversionKHR
  {
    VkDevice m_device;
    VkSamplerYcbcrConversionKHR m_ycbcrConversion;
  public:
    SamplerYcbcrConversionKHR(const SamplerYcbcrConversionKHR&) = delete;
    SamplerYcbcrConversionKHR& operator=(const SamplerYcbcrConversionKHR&) = delete;

    //! @brief Move assignment operator
    SamplerYcbcrConversionKHR& operator=(SamplerYcbcrConversionKHR&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_device = other.m_device;
        m_ycbcrConversion = other.m_ycbcrConversion;

        // Remove the data from other
        other.m_device = VK_NULL_HANDLE;
        other.m_ycbcrConversion = VK_NULL_HANDLE;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    SamplerYcbcrConversionKHR(SamplerYcbcrConversionKHR&& other)
      : m_device(other.m_device)
      , m_ycbcrConversion(other.m_ycbcrConversion)
    {
      // Remove the data from other
      other.m_device = VK_NULL_HANDLE;
      other.m_ycbcrConversion = VK_NULL_HANDLE;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    SamplerYcbcrConversionKHR()
      : m_device(VK_NULL_HANDLE)
      , m_ycbcrConversion(VK_NULL_HANDLE)
    {
    }

    //! @brief Assume control of the SamplerYcbcrConversionKHR (this object becomes responsible for releasing it)
    explicit SamplerYcbcrConversionKHR(const ClaimMode claimMode, const VkDevice device, const VkSamplerYcbcrConversionKHR ycbcrConversion)
      : SamplerYcbcrConversionKHR()
    {
      Reset(claimMode, device, ycbcrConversion);
    }

#if VK_HEADER_VERSION >= 61
    //! @brief Create the requested resource
    //! @note  Function: vkCreateSamplerYcbcrConversionKHR
    SamplerYcbcrConversionKHR(const VkDevice device, const VkSamplerYcbcrConversionCreateInfoKHR& createInfo)
      : SamplerYcbcrConversionKHR()
    {
      Reset(device, createInfo);
    }
#endif

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Create the requested resource
    //! @note  Function: vkCreateSamplerYcbcrConversionKHR
    SamplerYcbcrConversionKHR(const VkDevice device, const VkFormat format, const VkSamplerYcbcrModelConversionKHR ycbcrModel, const VkSamplerYcbcrRangeKHR ycbcrRange, const VkComponentMapping components, const VkChromaLocationKHR xChromaOffset, const VkChromaLocationKHR yChromaOffset, const VkFilter chromaFilter, const VkBool32 forceExplicitReconstruction)
      : SamplerYcbcrConversionKHR()
    {
      Reset(device, format, ycbcrModel, ycbcrRange, components, xChromaOffset, yChromaOffset, chromaFilter, forceExplicitReconstruction);
    }
#endif

    ~SamplerYcbcrConversionKHR()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    VkSamplerYcbcrConversionKHR Release() RAPIDVULKAN_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_ycbcrConversion;
      m_device = VK_NULL_HANDLE;
      m_ycbcrConversion = VK_NULL_HANDLE;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_device != VK_NULL_HANDLE);
      assert(m_ycbcrConversion != VK_NULL_HANDLE);

      vkDestroySamplerYcbcrConversionKHR(m_device, m_ycbcrConversion, nullptr);
      m_device = VK_NULL_HANDLE;
      m_ycbcrConversion = VK_NULL_HANDLE;
    }

    //! @brief Destroys any owned resources and assume control of the SamplerYcbcrConversionKHR (this object becomes responsible for releasing it)
    void Reset(const ClaimMode claimMode, const VkDevice device, const VkSamplerYcbcrConversionKHR ycbcrConversion)
    {
      if (IsValid())
        Reset();


      m_device = device;
      m_ycbcrConversion = ycbcrConversion;
    }

#if VK_HEADER_VERSION >= 61
    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateSamplerYcbcrConversionKHR
    void Reset(const VkDevice device, const VkSamplerYcbcrConversionCreateInfoKHR& createInfo)
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
      VkSamplerYcbcrConversionKHR ycbcrConversion;
      CheckError(vkCreateSamplerYcbcrConversionKHR(device, &createInfo, nullptr, &ycbcrConversion), "vkCreateSamplerYcbcrConversionKHR", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_device = device;
      m_ycbcrConversion = ycbcrConversion;
    }
#endif

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateSamplerYcbcrConversionKHR
    void Reset(const VkDevice device, const VkFormat format, const VkSamplerYcbcrModelConversionKHR ycbcrModel, const VkSamplerYcbcrRangeKHR ycbcrRange, const VkComponentMapping components, const VkChromaLocationKHR xChromaOffset, const VkChromaLocationKHR yChromaOffset, const VkFilter chromaFilter, const VkBool32 forceExplicitReconstruction)
    {
      VkSamplerYcbcrConversionCreateInfoKHR createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO_KHR;
      createInfo.pNext = nullptr;
      createInfo.format = format;
      createInfo.ycbcrModel = ycbcrModel;
      createInfo.ycbcrRange = ycbcrRange;
      createInfo.components = components;
      createInfo.xChromaOffset = xChromaOffset;
      createInfo.yChromaOffset = yChromaOffset;
      createInfo.chromaFilter = chromaFilter;
      createInfo.forceExplicitReconstruction = forceExplicitReconstruction;

      Reset(device, createInfo);
    }
#endif

    //! @brief Get the associated 'Device'
    VkDevice GetDevice() const
    {
      return m_device;
    }

    //! @brief Get the associated resource handle
    VkSamplerYcbcrConversionKHR Get() const
    {
      return m_ycbcrConversion;
    }

    //! @brief Get a pointer to the associated resource handle
    const VkSamplerYcbcrConversionKHR* GetPointer() const
    {
      return &m_ycbcrConversion;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_ycbcrConversion != VK_NULL_HANDLE;
    }
  };
}

#endif
#endif
