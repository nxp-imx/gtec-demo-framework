#ifndef RAPIDVULKAN_INSTANCE_HPP
#define RAPIDVULKAN_INSTANCE_HPP
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
  class Instance
  {
    VkInstance m_instance;
  public:
    Instance(const Instance&) = delete;
    Instance& operator=(const Instance&) = delete;

    //! @brief Move assignment operator
    Instance& operator=(Instance&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_instance = other.m_instance;

        // Remove the data from other
        other.m_instance = VK_NULL_HANDLE;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    Instance(Instance&& other)
      : m_instance(other.m_instance)
    {
      // Remove the data from other
      other.m_instance = VK_NULL_HANDLE;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    Instance()
      : m_instance(VK_NULL_HANDLE)
    {
    }

    //! @brief Assume control of the Instance (this object becomes responsible for releasing it)
    explicit Instance(const ClaimMode claimMode, const VkInstance instance)
      : Instance()
    {
      Reset(claimMode, instance);
    }

    //! @brief Create the requested resource
    //! @note  Function: vkCreateInstance
    Instance(const VkInstanceCreateInfo& createInfo)
      : Instance()
    {
      Reset(createInfo);
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Create the requested resource
    //! @note  Function: vkCreateInstance
    Instance(const VkInstanceCreateFlags flags, VkApplicationInfo*const pApplicationInfo, const uint32_t enabledLayerCount, const char *const * ppEnabledLayerNames, const uint32_t enabledExtensionCount, const char *const * ppEnabledExtensionNames)
      : Instance()
    {
      Reset(flags, pApplicationInfo, enabledLayerCount, ppEnabledLayerNames, enabledExtensionCount, ppEnabledExtensionNames);
    }
#endif

    ~Instance()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    VkInstance Release() RAPIDVULKAN_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_instance;
      m_instance = VK_NULL_HANDLE;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_instance != VK_NULL_HANDLE);

      vkDestroyInstance(m_instance, nullptr);
      m_instance = VK_NULL_HANDLE;
    }

    //! @brief Destroys any owned resources and assume control of the Instance (this object becomes responsible for releasing it)
    void Reset(const ClaimMode claimMode, const VkInstance instance)
    {
      if (IsValid())
        Reset();


      m_instance = instance;
    }

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateInstance
    void Reset(const VkInstanceCreateInfo& createInfo)
    {
#ifndef RAPIDVULKAN_DISABLE_PARAM_VALIDATION
#else
#endif

      // Free any currently allocated resource
      if (IsValid())
        Reset();

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      VkInstance instance;
      CheckError(vkCreateInstance(&createInfo, nullptr, &instance), "vkCreateInstance", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_instance = instance;
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateInstance
    void Reset(const VkInstanceCreateFlags flags, VkApplicationInfo*const pApplicationInfo, const uint32_t enabledLayerCount, const char *const * ppEnabledLayerNames, const uint32_t enabledExtensionCount, const char *const * ppEnabledExtensionNames)
    {
      VkInstanceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      createInfo.pNext = nullptr;
      createInfo.flags = flags;
      createInfo.pApplicationInfo = pApplicationInfo;
      createInfo.enabledLayerCount = enabledLayerCount;
      createInfo.ppEnabledLayerNames = ppEnabledLayerNames;
      createInfo.enabledExtensionCount = enabledExtensionCount;
      createInfo.ppEnabledExtensionNames = ppEnabledExtensionNames;

      Reset(createInfo);
    }
#endif

    //! @brief Get the associated resource handle
    VkInstance Get() const
    {
      return m_instance;
    }

    //! @brief Get a pointer to the associated resource handle
    const VkInstance* GetPointer() const
    {
      return &m_instance;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_instance != VK_NULL_HANDLE;
    }

    //! @note  Function: vkEnumeratePhysicalDevices
    void EnumeratePhysicalDevices(uint32_t * pPhysicalDeviceCount, VkPhysicalDevice * pPhysicalDevices)
    {
      CheckError(vkEnumeratePhysicalDevices(m_instance, pPhysicalDeviceCount, pPhysicalDevices), "vkEnumeratePhysicalDevices", __FILE__, __LINE__);
    }

    //! @note  Function: vkGetInstanceProcAddr
    PFN_vkVoidFunction GetInstanceProcAddr(const char * pName)
    {
      return vkGetInstanceProcAddr(m_instance, pName);
    }
  };
}

#endif
