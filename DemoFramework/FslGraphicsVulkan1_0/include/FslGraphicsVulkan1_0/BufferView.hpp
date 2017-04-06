#ifndef FSLGRAPHICSVULKAN1_0_BUFFERVIEW_HPP
#define FSLGRAPHICSVULKAN1_0_BUFFERVIEW_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2016 Freescale Semiconductor, Inc.
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
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

// Auto-generated Vulkan 1.0 C++11 RAII classes by RAIIGen (https://github.com/Unarmed1000)
// Auto generation template based on RapidVulkan https://github.com/Unarmed1000/RapidVulkan with permission.

#include <FslGraphicsVulkan1_0/ClaimMode.hpp>
#include <FslGraphicsVulkan1_0/Common.hpp>
#include <FslGraphicsVulkan1_0/Util.hpp>
#include <FslBase/Attributes.hpp>
#include <vulkan/vulkan.h>
#include <cassert>

namespace Fsl
{
  namespace Vulkan
  {
    //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
    class BufferView
    {
      VkDevice m_device;
      VkBufferView m_view;
    public:
      BufferView(const BufferView&) = delete;
      BufferView& operator=(const BufferView&) = delete;

      //! @brief Move assignment operator
      BufferView& operator=(BufferView&& other)
      {
        if (this != &other)
        {
          // Free existing resources then transfer the content of other to this one and fill other with default values
          if (IsValid())
            Reset();

          // Claim ownership here
          m_device = other.m_device;
          m_view = other.m_view;

          // Remove the data from other
          other.m_device = VK_NULL_HANDLE;
          other.m_view = VK_NULL_HANDLE;
        }
        return *this;
      }

      //! @brief Move constructor
      //! Transfer ownership from other to this
      BufferView(BufferView&& other)
        : m_device(other.m_device)
        , m_view(other.m_view)
      {
        // Remove the data from other
        other.m_device = VK_NULL_HANDLE;
        other.m_view = VK_NULL_HANDLE;
      }

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      BufferView()
        : m_device(VK_NULL_HANDLE)
        , m_view(VK_NULL_HANDLE)
      {
      }

      //! @brief Assume control of the BufferView (this object becomes responsible for releasing it)
      explicit BufferView(const ClaimMode claimMode, const VkDevice device, const VkBufferView view)
        : BufferView()
      {
        Reset(claimMode, device, view);
      }

      //! @brief Create the requested resource
      //! @note  Function: vkCreateBufferView
      BufferView(const VkDevice device, const VkBufferViewCreateInfo& createInfo)
        : BufferView()
      {
        Reset(device, createInfo);
      }

      //! @brief Create the requested resource
      //! @note  Function: vkCreateBufferView
      BufferView(const VkDevice device, const VkBufferViewCreateFlags flags, const VkBuffer buffer, const VkFormat format, const VkDeviceSize offset, const VkDeviceSize range)
        : BufferView()
      {
        Reset(device, flags, buffer, format, offset, range);
      }

      ~BufferView()
      {
        Reset();
      }

      //! @brief returns the managed handle and releases the ownership.
      VkBufferView Release() FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT
      {
        const auto resource = m_view;
        m_device = VK_NULL_HANDLE;
        m_view = VK_NULL_HANDLE;
        return resource;
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset()
      {
        if (! IsValid())
          return;

        assert(m_device != VK_NULL_HANDLE);
        assert(m_view != VK_NULL_HANDLE);

        vkDestroyBufferView(m_device, m_view, nullptr);
        m_device = VK_NULL_HANDLE;
        m_view = VK_NULL_HANDLE;
      }

      //! @brief Destroys any owned resources and assume control of the BufferView (this object becomes responsible for releasing it)
      void Reset(const ClaimMode claimMode, const VkDevice device, const VkBufferView view)
      {
        if (IsValid())
          Reset();


        m_device = device;
        m_view = view;
      }

      //! @brief Destroys any owned resources and then creates the requested one
      //! @note  Function: vkCreateBufferView
      void Reset(const VkDevice device, const VkBufferViewCreateInfo& createInfo)
      {
#ifndef FSLGRAPHICSVULKAN_DISABLE_PARAM_VALIDATION
        if (device == VK_NULL_HANDLE)
          throw std::invalid_argument("device can not be VK_NULL_HANDLE");
#else
        assert(device != VK_NULL_HANDLE);
#endif

        // Free any currently allocated resource
        if (IsValid())
          Reset();

        // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
        VkBufferView view;
        Util::Check(vkCreateBufferView(device, &createInfo, nullptr, &view), "vkCreateBufferView", __FILE__, __LINE__);

        // Everything is ready, so assign the members
        m_device = device;
        m_view = view;
      }

      //! @brief Destroys any owned resources and then creates the requested one
      //! @note  Function: vkCreateBufferView
      void Reset(const VkDevice device, const VkBufferViewCreateFlags flags, const VkBuffer buffer, const VkFormat format, const VkDeviceSize offset, const VkDeviceSize range)
      {
        VkBufferViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = flags;
        createInfo.buffer = buffer;
        createInfo.format = format;
        createInfo.offset = offset;
        createInfo.range = range;

        Reset(device, createInfo);
      }

      //! @brief Get the associated 'Device'
      VkDevice GetDevice() const
      {
        return m_device;
      }

      //! @brief Get the associated resource handle
      VkBufferView Get() const
      {
        return m_view;
      }

      //! @brief Get a pointer to the associated resource handle
      const VkBufferView* GetPointer() const
      {
        return &m_view;
      }

      //! @brief Check if this object contains a valid resource
      inline bool IsValid() const
      {
        return m_view != VK_NULL_HANDLE;
      }
    };

  }
}

#endif
