#ifndef RAPIDVULKAN_DEBUGREPORTCALLBACKEXT_HPP
#define RAPIDVULKAN_DEBUGREPORTCALLBACKEXT_HPP
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
  class DebugReportCallbackEXT
  {
    VkInstance m_instance;
    VkDebugReportCallbackEXT m_callback;
  public:
    DebugReportCallbackEXT(const DebugReportCallbackEXT&) = delete;
    DebugReportCallbackEXT& operator=(const DebugReportCallbackEXT&) = delete;

    //! @brief Move assignment operator
    DebugReportCallbackEXT& operator=(DebugReportCallbackEXT&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_instance = other.m_instance;
        m_callback = other.m_callback;

        // Remove the data from other
        other.m_instance = VK_NULL_HANDLE;
        other.m_callback = VK_NULL_HANDLE;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    DebugReportCallbackEXT(DebugReportCallbackEXT&& other)
      : m_instance(other.m_instance)
      , m_callback(other.m_callback)
    {
      // Remove the data from other
      other.m_instance = VK_NULL_HANDLE;
      other.m_callback = VK_NULL_HANDLE;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    DebugReportCallbackEXT()
      : m_instance(VK_NULL_HANDLE)
      , m_callback(VK_NULL_HANDLE)
    {
    }

    //! @brief Assume control of the DebugReportCallbackEXT (this object becomes responsible for releasing it)
    explicit DebugReportCallbackEXT(const ClaimMode claimMode, const VkInstance instance, const VkDebugReportCallbackEXT callback)
      : DebugReportCallbackEXT()
    {
      Reset(claimMode, instance, callback);
    }

    //! @brief Create the requested resource
    //! @note  Function: vkCreateDebugReportCallbackEXT
    DebugReportCallbackEXT(const VkInstance instance, const VkDebugReportCallbackCreateInfoEXT& createInfo)
      : DebugReportCallbackEXT()
    {
      Reset(instance, createInfo);
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Create the requested resource
    //! @note  Function: vkCreateDebugReportCallbackEXT
    DebugReportCallbackEXT(const VkInstance instance, const VkDebugReportFlagsEXT flags, const PFN_vkDebugReportCallbackEXT pfnCallback, void * pUserData)
      : DebugReportCallbackEXT()
    {
      Reset(instance, flags, pfnCallback, pUserData);
    }
#endif

    ~DebugReportCallbackEXT()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    VkDebugReportCallbackEXT Release() RAPIDVULKAN_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_callback;
      m_instance = VK_NULL_HANDLE;
      m_callback = VK_NULL_HANDLE;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_instance != VK_NULL_HANDLE);
      assert(m_callback != VK_NULL_HANDLE);

      vkDestroyDebugReportCallbackEXT(m_instance, m_callback, nullptr);
      m_instance = VK_NULL_HANDLE;
      m_callback = VK_NULL_HANDLE;
    }

    //! @brief Destroys any owned resources and assume control of the DebugReportCallbackEXT (this object becomes responsible for releasing it)
    void Reset(const ClaimMode claimMode, const VkInstance instance, const VkDebugReportCallbackEXT callback)
    {
      if (IsValid())
        Reset();


      m_instance = instance;
      m_callback = callback;
    }

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateDebugReportCallbackEXT
    void Reset(const VkInstance instance, const VkDebugReportCallbackCreateInfoEXT& createInfo)
    {
#ifndef RAPIDVULKAN_DISABLE_PARAM_VALIDATION
      if (instance == VK_NULL_HANDLE)
        throw std::invalid_argument("instance can not be VK_NULL_HANDLE");
#else
      assert(instance != VK_NULL_HANDLE);
#endif

      // Free any currently allocated resource
      if (IsValid())
        Reset();

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      VkDebugReportCallbackEXT callback;
      CheckError(vkCreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback), "vkCreateDebugReportCallbackEXT", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_instance = instance;
      m_callback = callback;
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateDebugReportCallbackEXT
    void Reset(const VkInstance instance, const VkDebugReportFlagsEXT flags, const PFN_vkDebugReportCallbackEXT pfnCallback, void * pUserData)
    {
      VkDebugReportCallbackCreateInfoEXT createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
      createInfo.pNext = nullptr;
      createInfo.flags = flags;
      createInfo.pfnCallback = pfnCallback;
      createInfo.pUserData = pUserData;

      Reset(instance, createInfo);
    }
#endif

    //! @brief Get the associated 'Instance'
    VkInstance GetInstance() const
    {
      return m_instance;
    }

    //! @brief Get the associated resource handle
    VkDebugReportCallbackEXT Get() const
    {
      return m_callback;
    }

    //! @brief Get a pointer to the associated resource handle
    const VkDebugReportCallbackEXT* GetPointer() const
    {
      return &m_callback;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_callback != VK_NULL_HANDLE;
    }
  };
}

#endif
