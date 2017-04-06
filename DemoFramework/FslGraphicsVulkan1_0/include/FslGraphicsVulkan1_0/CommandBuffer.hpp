#ifndef FSLGRAPHICSVULKAN1_0_COMMANDBUFFER_HPP
#define FSLGRAPHICSVULKAN1_0_COMMANDBUFFER_HPP
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
    class CommandBuffer
    {
      VkDevice m_device;
      VkCommandPool m_commandPool;
      VkCommandBuffer m_commandBuffers;
    public:
      CommandBuffer(const CommandBuffer&) = delete;
      CommandBuffer& operator=(const CommandBuffer&) = delete;

      //! @brief Move assignment operator
      CommandBuffer& operator=(CommandBuffer&& other)
      {
        if (this != &other)
        {
          // Free existing resources then transfer the content of other to this one and fill other with default values
          if (IsValid())
            Reset();

          // Claim ownership here
          m_device = other.m_device;
          m_commandPool = other.m_commandPool;
          m_commandBuffers = other.m_commandBuffers;

          // Remove the data from other
          other.m_device = VK_NULL_HANDLE;
          other.m_commandPool = VK_NULL_HANDLE;
          other.m_commandBuffers = VK_NULL_HANDLE;
        }
        return *this;
      }

      //! @brief Move constructor
      //! Transfer ownership from other to this
      CommandBuffer(CommandBuffer&& other)
        : m_device(other.m_device)
        , m_commandPool(other.m_commandPool)
        , m_commandBuffers(other.m_commandBuffers)
      {
        // Remove the data from other
        other.m_device = VK_NULL_HANDLE;
        other.m_commandPool = VK_NULL_HANDLE;
        other.m_commandBuffers = VK_NULL_HANDLE;
      }

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      CommandBuffer()
        : m_device(VK_NULL_HANDLE)
        , m_commandPool(VK_NULL_HANDLE)
        , m_commandBuffers(VK_NULL_HANDLE)
      {
      }

      //! @brief Assume control of the CommandBuffer (this object becomes responsible for releasing it)
      explicit CommandBuffer(const ClaimMode claimMode, const VkDevice device, const VkCommandPool commandPool, const VkCommandBuffer commandBuffers)
        : CommandBuffer()
      {
        Reset(claimMode, device, commandPool, commandBuffers);
      }

      //! @brief Create the requested resource
      //! @note  Function: vkAllocateCommandBuffers
      CommandBuffer(const VkDevice device, const VkCommandBufferAllocateInfo& allocateInfo)
        : CommandBuffer()
      {
        Reset(device, allocateInfo);
      }

      //! @brief Create the requested resource
      //! @note  Function: vkAllocateCommandBuffers
      CommandBuffer(const VkDevice device, const VkCommandPool commandPool, const VkCommandBufferLevel level)
        : CommandBuffer()
      {
        Reset(device, commandPool, level);
      }

      ~CommandBuffer()
      {
        Reset();
      }

      //! @brief returns the managed handle and releases the ownership.
      VkCommandBuffer Release() FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT
      {
        const auto resource = m_commandBuffers;
        m_device = VK_NULL_HANDLE;
        m_commandPool = VK_NULL_HANDLE;
        m_commandBuffers = VK_NULL_HANDLE;
        return resource;
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset()
      {
        if (! IsValid())
          return;

        assert(m_device != VK_NULL_HANDLE);
        assert(m_commandPool != VK_NULL_HANDLE);
        assert(m_commandBuffers != VK_NULL_HANDLE);

        vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_commandBuffers);
        m_device = VK_NULL_HANDLE;
        m_commandPool = VK_NULL_HANDLE;
        m_commandBuffers = VK_NULL_HANDLE;
      }

      //! @brief Destroys any owned resources and assume control of the CommandBuffer (this object becomes responsible for releasing it)
      void Reset(const ClaimMode claimMode, const VkDevice device, const VkCommandPool commandPool, const VkCommandBuffer commandBuffers)
      {
        if (IsValid())
          Reset();


        m_device = device;
        m_commandPool = commandPool;
        m_commandBuffers = commandBuffers;
      }

      //! @brief Destroys any owned resources and then creates the requested one
      //! @note  Function: vkAllocateCommandBuffers
      void Reset(const VkDevice device, const VkCommandBufferAllocateInfo& allocateInfo)
      {
#ifndef FSLGRAPHICSVULKAN_DISABLE_PARAM_VALIDATION
        if (device == VK_NULL_HANDLE)
          throw std::invalid_argument("device can not be VK_NULL_HANDLE");
        if (allocateInfo.commandPool == VK_NULL_HANDLE)
          throw std::invalid_argument("allocateInfo.commandPool can not be VK_NULL_HANDLE");
#else
        assert(device != VK_NULL_HANDLE);
        assert(allocateInfo.commandPool != VK_NULL_HANDLE);
        assert(allocateInfo.commandBufferCount == 1);
#endif

        // Free any currently allocated resource
        if (IsValid())
          Reset();

        // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
        VkCommandBuffer commandBuffers;
        Util::Check(vkAllocateCommandBuffers(device, &allocateInfo, &commandBuffers), "vkAllocateCommandBuffers", __FILE__, __LINE__);

        // Everything is ready, so assign the members
        m_device = device;
        m_commandPool = allocateInfo.commandPool;
        m_commandBuffers = commandBuffers;
      }

      //! @brief Destroys any owned resources and then creates the requested one
      //! @note  Function: vkAllocateCommandBuffers
      void Reset(const VkDevice device, const VkCommandPool commandPool, const VkCommandBufferLevel level)
      {
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.pNext = nullptr;
        allocateInfo.commandPool = commandPool;
        allocateInfo.level = level;
        allocateInfo.commandBufferCount = 1;

        Reset(device, allocateInfo);
      }

      //! @brief Get the associated 'Device'
      VkDevice GetDevice() const
      {
        return m_device;
      }

      //! @brief Get the associated 'CommandPool'
      VkCommandPool GetCommandPool() const
      {
        return m_commandPool;
      }

      //! @brief Get the associated resource handle
      VkCommandBuffer Get() const
      {
        return m_commandBuffers;
      }

      //! @brief Get a pointer to the associated resource handle
      const VkCommandBuffer* GetPointer() const
      {
        return &m_commandBuffers;
      }

      //! @brief Check if this object contains a valid resource
      inline bool IsValid() const
      {
        return m_commandBuffers != VK_NULL_HANDLE;
      }


      void Begin(const VkCommandBufferUsageFlags flags, const VkRenderPass renderPass, const uint32_t subpass,
                 const VkFramebuffer framebuffer, const VkBool32 occlusionQueryEnable, const VkQueryControlFlags queryFlags,
                 const VkQueryPipelineStatisticFlags pipelineStatistics, const uint32_t bufferIndex = 0);
      void Begin(const VkCommandBufferBeginInfo& commandBufferBeginInfo);
      void End();

      void CmdBeginRenderPass(const VkRenderPassBeginInfo* pRenderPassBeginInfo, const VkSubpassContents contents)
      {
        vkCmdBeginRenderPass(m_commandBuffers, pRenderPassBeginInfo, contents);
      }

      void CmdEndRenderPass()
      {
        vkCmdEndRenderPass(m_commandBuffers);
      }

    };

  }
}

#endif
