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

#include <FslGraphicsVulkan1_0/Check.hpp>
#include <FslGraphicsVulkan1_0/CommandBuffer.hpp>
#include <FslGraphicsVulkan1_0/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <cassert>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    void CommandBuffer::Begin(const VkCommandBufferUsageFlags flags, const VkRenderPass renderPass, const uint32_t subpass,
                              const VkFramebuffer framebuffer, const VkBool32 occlusionQueryEnable, const VkQueryControlFlags queryFlags,
                              const VkQueryPipelineStatisticFlags pipelineStatistics, const uint32_t bufferIndex)
    {
      VkCommandBufferInheritanceInfo commandBufferInheritanceInfo{};
      commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
      commandBufferInheritanceInfo.renderPass = renderPass;
      commandBufferInheritanceInfo.subpass = subpass;
      commandBufferInheritanceInfo.framebuffer = framebuffer;
      commandBufferInheritanceInfo.occlusionQueryEnable = occlusionQueryEnable;
      commandBufferInheritanceInfo.queryFlags = queryFlags;
      commandBufferInheritanceInfo.pipelineStatistics = pipelineStatistics;

      VkCommandBufferBeginInfo commandBufferBeginInfo{};
      commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      commandBufferBeginInfo.flags = flags;
      commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

      Begin(commandBufferBeginInfo);
    }


    void CommandBuffer::Begin(const VkCommandBufferBeginInfo& commandBufferBeginInfo)
    {
      if (m_commandBuffers == VK_NULL_HANDLE)
        throw UsageErrorException("Can not call Begin on a NULL handle");

      FSLGRAPHICSVULKAN_CHECK(vkBeginCommandBuffer(m_commandBuffers, &commandBufferBeginInfo));
    }


    void CommandBuffer::End()
    {
      if (m_commandBuffers == VK_NULL_HANDLE)
        throw UsageErrorException("Can not call End on a NULL handle");

      FSLGRAPHICSVULKAN_CHECK(vkEndCommandBuffer(m_commandBuffers));
    }

  }
}
