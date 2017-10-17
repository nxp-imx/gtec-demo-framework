/****************************************************************************************************************************************************
* Copyright 2017 NXP
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

#include <FslUtil/Vulkan1_0/Batch/QuadBatchCommandBuffers.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <limits>

using namespace RapidVulkan;

namespace Fsl
{
  namespace Vulkan
  {
    namespace
    {
      const uint32_t BUCKET_SIZE = 256;
      const uint32_t ARRAY_START_SIZE = BUCKET_SIZE;
      const uint32_t ARRAY_EXPAND_ENTRIES = BUCKET_SIZE;
    }


    QuadBatchCommandBuffers::QuadBatchCommandBuffers(const VkDevice device, const uint32_t queueFamilyIndex)
      : m_commandPool(device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueFamilyIndex)
      , m_commandBufferBuckets()
      , m_activeBuffers(ARRAY_START_SIZE)
      , m_activeCount(0)
    {
      // Allocate the initial bucket
      m_commandBufferBuckets.push_back(CommandBuffers(device, m_commandPool.Get(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, BUCKET_SIZE));
    }


    void QuadBatchCommandBuffers::Clear()
    {
      m_activeCount = 0;
    }


    VkCommandBuffer QuadBatchCommandBuffers::NextFree()
    {
      if (m_activeCount >= std::numeric_limits<uint32_t>::max())
        throw NotSupportedException("Maximum number of supported buffers reached");

      const uint32_t currentIndex = m_activeCount;

      const uint32_t currentBucketIndex = m_activeCount / BUCKET_SIZE;
      const uint32_t indexInsideBucket = m_activeCount % BUCKET_SIZE;

      // Check if we need to allocate another bucket
      assert(currentBucketIndex <= m_commandBufferBuckets.size());
      if (currentBucketIndex >= m_commandBufferBuckets.size())
        m_commandBufferBuckets.push_back(CommandBuffers(m_commandPool.GetDevice(), m_commandPool.Get(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, BUCKET_SIZE));

      // take the next available command buffer
      const VkCommandBuffer commandBuffer = m_commandBufferBuckets[currentBucketIndex][indexInsideBucket];

      ++m_activeCount;
      // ensure we have enough capacity to hold all active buffers in one array
      if (m_activeCount >= m_activeBuffers.size())
        m_activeBuffers.resize(m_activeBuffers.size() + ARRAY_EXPAND_ENTRIES);

      m_activeBuffers[currentIndex] = commandBuffer;
      return commandBuffer;
    }
  }
}
