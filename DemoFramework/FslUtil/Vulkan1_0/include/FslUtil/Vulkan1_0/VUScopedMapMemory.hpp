#ifndef FSLUTIL_VULKAN1_0_VUSCOPEDMAPMEMORY_HPP
#define FSLUTIL_VULKAN1_0_VUSCOPEDMAPMEMORY_HPP
/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <RapidVulkan/CheckError.hpp>
#include <RapidVulkan/Memory.hpp>
#include <vulkan/vulkan.h>
#include <cassert>

namespace Fsl::Vulkan
{
  // Exception safe wrapper for vkMapMemory, vkUnmapMemory
  class VUScopedMapMemory
  {
    VkDevice m_device{VK_NULL_HANDLE};
    VkDeviceMemory m_deviceMemory{VK_NULL_HANDLE};

  public:
    VUScopedMapMemory(const VUScopedMapMemory&) = delete;
    VUScopedMapMemory& operator=(const VUScopedMapMemory&) = delete;

    // move assignment operator
    VUScopedMapMemory& operator=(VUScopedMapMemory&& other) noexcept
    {
      if (this != &other)
      {
        if (IsValid())
        {
          Reset();
        }

        m_device = other.m_device;
        m_deviceMemory = other.m_deviceMemory;

        other.m_device = VK_NULL_HANDLE;
        other.m_deviceMemory = VK_NULL_HANDLE;
      }
      return *this;
    }

    // move constructor
    VUScopedMapMemory(VUScopedMapMemory&& other) noexcept
      : m_device(other.m_device)
      , m_deviceMemory(other.m_deviceMemory)
    {
      other.m_device = VK_NULL_HANDLE;
      other.m_deviceMemory = VK_NULL_HANDLE;
    }

    VUScopedMapMemory() = default;

    //! @brief Start a map memory scope
    VUScopedMapMemory(const VkDevice device, const VkDeviceMemory deviceMemory, const VkDeviceSize offset, const VkDeviceSize size,
                      const VkMemoryMapFlags flags, void** ppData)
      : VUScopedMapMemory()
    {
      Reset(device, deviceMemory, offset, size, flags, ppData);
    }

    ~VUScopedMapMemory() noexcept
    {
      Reset();
    }

    bool IsValid() const
    {
      return m_device != VK_NULL_HANDLE && m_deviceMemory != VK_NULL_HANDLE;
    }

    void Reset() noexcept
    {
      if (!IsValid())
      {
        assert(m_device == VK_NULL_HANDLE && m_deviceMemory == VK_NULL_HANDLE);
        return;
      }

      if (m_device != VK_NULL_HANDLE && m_deviceMemory != VK_NULL_HANDLE)
      {
        vkUnmapMemory(m_device, m_deviceMemory);
        m_device = VK_NULL_HANDLE;
        m_deviceMemory = VK_NULL_HANDLE;
      }
    }

    void Reset(const VkDevice device, const VkDeviceMemory deviceMemory, const VkDeviceSize offset, const VkDeviceSize size,
               const VkMemoryMapFlags flags, void** ppData)
    {
      if (device == VK_NULL_HANDLE)
      {
        throw std::invalid_argument("device can not be VK_NULL_HANDLE");
      }
      if (deviceMemory == VK_NULL_HANDLE)
      {
        throw std::invalid_argument("deviceMemory can not be VK_NULL_HANDLE");
      }
      if (ppData == nullptr)
      {
        throw std::invalid_argument("ppData can not be nullptr");
      }
      if (IsValid())
      {
        Reset();
      }

      RapidVulkan::CheckError(vkMapMemory(device, deviceMemory, offset, size, flags, ppData), "vkMapMemory", __FILE__, __LINE__);

      assert(ppData != nullptr);
      assert(*ppData != nullptr);
      m_device = device;
      m_deviceMemory = deviceMemory;
    }
  };
}

#endif
