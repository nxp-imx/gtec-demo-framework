#ifndef RAPIDVULKAN_EVENT_HPP
#define RAPIDVULKAN_EVENT_HPP
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
  class Event
  {
    VkDevice m_device;
    VkEvent m_event;
  public:
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    //! @brief Move assignment operator
    Event& operator=(Event&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_device = other.m_device;
        m_event = other.m_event;

        // Remove the data from other
        other.m_device = VK_NULL_HANDLE;
        other.m_event = VK_NULL_HANDLE;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    Event(Event&& other)
      : m_device(other.m_device)
      , m_event(other.m_event)
    {
      // Remove the data from other
      other.m_device = VK_NULL_HANDLE;
      other.m_event = VK_NULL_HANDLE;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    Event()
      : m_device(VK_NULL_HANDLE)
      , m_event(VK_NULL_HANDLE)
    {
    }

    //! @brief Assume control of the Event (this object becomes responsible for releasing it)
    explicit Event(const ClaimMode claimMode, const VkDevice device, const VkEvent event)
      : Event()
    {
      Reset(claimMode, device, event);
    }

    //! @brief Create the requested resource
    //! @note  Function: vkCreateEvent
    Event(const VkDevice device, const VkEventCreateInfo& createInfo)
      : Event()
    {
      Reset(device, createInfo);
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Create the requested resource
    //! @note  Function: vkCreateEvent
    Event(const VkDevice device, const VkEventCreateFlags flags)
      : Event()
    {
      Reset(device, flags);
    }
#endif

    ~Event()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    VkEvent Release() RAPIDVULKAN_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_event;
      m_device = VK_NULL_HANDLE;
      m_event = VK_NULL_HANDLE;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_device != VK_NULL_HANDLE);
      assert(m_event != VK_NULL_HANDLE);

      vkDestroyEvent(m_device, m_event, nullptr);
      m_device = VK_NULL_HANDLE;
      m_event = VK_NULL_HANDLE;
    }

    //! @brief Destroys any owned resources and assume control of the Event (this object becomes responsible for releasing it)
    void Reset(const ClaimMode claimMode, const VkDevice device, const VkEvent event)
    {
      if (IsValid())
        Reset();


      m_device = device;
      m_event = event;
    }

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateEvent
    void Reset(const VkDevice device, const VkEventCreateInfo& createInfo)
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
      VkEvent event;
      CheckError(vkCreateEvent(device, &createInfo, nullptr, &event), "vkCreateEvent", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_device = device;
      m_event = event;
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateEvent
    void Reset(const VkDevice device, const VkEventCreateFlags flags)
    {
      VkEventCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
      createInfo.pNext = nullptr;
      createInfo.flags = flags;

      Reset(device, createInfo);
    }
#endif

    //! @brief Get the associated 'Device'
    VkDevice GetDevice() const
    {
      return m_device;
    }

    //! @brief Get the associated resource handle
    VkEvent Get() const
    {
      return m_event;
    }

    //! @brief Get a pointer to the associated resource handle
    const VkEvent* GetPointer() const
    {
      return &m_event;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_event != VK_NULL_HANDLE;
    }

    //! @note  Function: vkGetEventStatus
    void GetEventStatus()
    {
      CheckError(vkGetEventStatus(m_device, m_event), "vkGetEventStatus", __FILE__, __LINE__);
    }

    //! @note  Function: vkSetEvent
    void SetEvent()
    {
      CheckError(vkSetEvent(m_device, m_event), "vkSetEvent", __FILE__, __LINE__);
    }

    //! @note  Function: vkResetEvent
    void ResetEvent()
    {
      CheckError(vkResetEvent(m_device, m_event), "vkResetEvent", __FILE__, __LINE__);
    }
  };
}

#endif
