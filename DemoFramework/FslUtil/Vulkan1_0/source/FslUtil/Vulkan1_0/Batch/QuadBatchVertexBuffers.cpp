/****************************************************************************************************************************************************
 * Copyright 2017, 2022 NXP
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

#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslUtil/Vulkan1_0/Batch/QuadBatchVertexBuffers.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <algorithm>
#include <limits>

namespace Fsl::Vulkan
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const uint32_t BucketSize = 4096;
      // const uint32_t ARRAY_START_SIZE = LocalConfig::BucketSize;
      // const uint32_t ARRAY_EXPAND_ENTRIES = LocalConfig::BucketSize;
    }


    inline VUBufferMemory CreateBuffer(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkDeviceSize bufferByteSize,
                                       const VkBufferUsageFlags usageFlags, const VkMemoryPropertyFlags memoryPropertyFlags)
    {
      VkBufferCreateInfo bufferCreateInfo{};
      bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferCreateInfo.size = bufferByteSize;
      bufferCreateInfo.usage = usageFlags;
      bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      return {physicalDevice, device, bufferCreateInfo, memoryPropertyFlags};
    }
  }

  QuadBatchVertexBuffers::QuadBatchVertexBuffers()
    : m_device(VK_NULL_HANDLE)
    , m_segmentVertexCount(0)
  {
  }


  QuadBatchVertexBuffers::QuadBatchVertexBuffers(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                                                 const uint32_t minimumVertexCountRequest)
    : QuadBatchVertexBuffers()
  {
    Reset(physicalDevice, device, minimumVertexCountRequest);
  }


  QuadBatchVertexBuffers::~QuadBatchVertexBuffers()
  {
    Reset();
  }


  void QuadBatchVertexBuffers::Reset() noexcept
  {
    if (!IsValid())
    {
      return;
    }

    for (auto& rEntry : m_buckets)
    {
      rEntry.Unmap();
    }

    m_physicalDevice = {};
    m_device = VK_NULL_HANDLE;
    m_segmentVertexCount = 0;
    m_buckets.clear();
    m_activeCount = 0;
  }


  void QuadBatchVertexBuffers::Reset(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const uint32_t minimumVertexCountRequest)
  {
    if (!physicalDevice.IsValid())
    {
      throw std::invalid_argument("physicalDevice must be valid");
    }
    if (device == VK_NULL_HANDLE)
    {
      throw std::invalid_argument("device must be valid");
    }
    if (minimumVertexCountRequest > LocalConfig::BucketSize)
    {
      throw std::invalid_argument("minimumVertexCountRequest must be '<=' LocalConfig::BucketSize");
    }

    Reset();

    try
    {
      m_physicalDevice = physicalDevice;
      m_device = device;
      m_segmentVertexCount = minimumVertexCountRequest;

      // Allocate the initial bucket
      m_buckets.push_back(CreateBuffer(m_physicalDevice, m_device, sizeof(VertexPositionColorTexture) * LocalConfig::BucketSize,
                                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
      m_buckets.back().Map();
    }
    catch (const std::exception&)
    {
      Reset();
      throw;
    }
  }


  void QuadBatchVertexBuffers::Clear()
  {
    m_activeCount = 0;
  }


  QuadBatchVertexBuffers::VertexBufferEntry QuadBatchVertexBuffers::NextFree(const uint32_t vertexCapacityHint)
  {
    if (m_activeCount >= std::numeric_limits<uint32_t>::max())
    {
      throw NotSupportedException("Maximum number of supported buffers reached");
    }

    assert((vertexCapacityHint % m_segmentVertexCount) == 0);

    uint32_t currentBucketIndex = m_activeCount / LocalConfig::BucketSize;
    uint32_t indexInsideBucket = m_activeCount % LocalConfig::BucketSize;
    uint32_t currentBucketCapacity = LocalConfig::BucketSize - indexInsideBucket;
    if (currentBucketCapacity < m_segmentVertexCount)
    {
      ++currentBucketIndex;
      indexInsideBucket = 0;
      currentBucketCapacity = LocalConfig::BucketSize;
      m_activeCount = currentBucketIndex * LocalConfig::BucketSize;
    }

    // Check if we need to allocate another bucket
    assert(currentBucketIndex <= m_buckets.size());
    if (currentBucketIndex >= m_buckets.size())
    {
      m_buckets.push_back(CreateBuffer(m_physicalDevice, m_device, sizeof(VertexPositionColorTexture) * LocalConfig::BucketSize,
                                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
      m_buckets.back().Map();
    }

    // take the next available command buffer
    const VkBuffer vertexBuffer = m_buckets[currentBucketIndex].GetBuffer();
    auto* pMapped = static_cast<VertexPositionColorTexture*>(m_buckets[currentBucketIndex].GetMappedPointer()) + indexInsideBucket;
    auto capacity = std::min(vertexCapacityHint, (currentBucketCapacity / m_segmentVertexCount) * m_segmentVertexCount);

    m_activeCount += capacity;

    return {vertexBuffer, pMapped, capacity, indexInsideBucket};
  }
}
