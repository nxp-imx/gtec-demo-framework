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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/VUBuffer.hpp>
#include <cassert>

namespace Fsl::Vulkan
{
  VMBufferManager::VMBufferManager(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkQueue queue,
                                   const uint32_t queueFamilyIndex)
    : m_physicalDevice(physicalDevice)
    , m_device(device)
    , m_queue(queue)
    //, m_queueFamilyIndex(queueFamilyIndex)
    , m_commandPool(device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueFamilyIndex)
    , m_commandBuffer(device, m_commandPool.Get(), VK_COMMAND_BUFFER_LEVEL_PRIMARY)
  {
    if (!physicalDevice.IsValid())
    {
      throw std::invalid_argument("physicalDevice must be valid");
    }
    if (device == VK_NULL_HANDLE)
    {
      throw std::invalid_argument("device must be valid");
    }
    if (queue == VK_NULL_HANDLE)
    {
      throw std::invalid_argument("queue must be valid");
    }
    assert(m_physicalDevice.IsValid());
    assert(m_device != VK_NULL_HANDLE);
    assert(m_queue != VK_NULL_HANDLE);
    assert(m_commandPool.IsValid());
    assert(m_commandBuffer.IsValid());
  }

  //! @param elementStride the size of one element in bytes
  VUBufferMemory VMBufferManager::CreateBuffer(ReadOnlyFlexSpan bufferSpan, const std::size_t elementCapacity,
                                               const VkBufferUsageFlags bufferUsageFlags, const VMBufferUsage usage)
  {
    switch (usage)
    {
    case VMBufferUsage::STATIC:
      return CreateStaticBuffer(bufferSpan, elementCapacity, bufferUsageFlags);
    case VMBufferUsage::DYNAMIC:
      return CreateDynamicBuffer(bufferSpan, elementCapacity, bufferUsageFlags);
    default:
      throw NotSupportedException("Unsupported VMBufferUsage");
    }
  }

  VUBufferMemory VMBufferManager::CreateDynamicBuffer(ReadOnlyFlexSpan bufferSpan, const std::size_t elementCapacity,
                                                      const VkBufferUsageFlags bufferUsageFlags)
  {
    if (bufferSpan.size() > elementCapacity)
    {
      throw std::invalid_argument("elementCount must be <= elementCapacity");
    }

    assert(m_physicalDevice.IsValid());
    assert(m_device != VK_NULL_HANDLE);

    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = elementCapacity * bufferSpan.stride();
    bufferCreateInfo.usage = bufferUsageFlags;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VUBufferMemory buffer(m_physicalDevice, m_device, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    buffer.Map();
    buffer.Upload(0, bufferSpan.data(), bufferSpan.byte_size());
    // We leave the buffer mapped as this is a dynamic buffer that will be changed often
    return buffer;
  }

  VUBufferMemory VMBufferManager::CreateDynamicBuffer(const std::size_t elementCapacity, const std::size_t elementStride,
                                                      const VkBufferUsageFlags bufferUsageFlags)
  {
    assert(m_physicalDevice.IsValid());
    assert(m_device != VK_NULL_HANDLE);

    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = elementCapacity * elementStride;
    bufferCreateInfo.usage = bufferUsageFlags;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VUBufferMemory buffer(m_physicalDevice, m_device, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    buffer.Map();
    // We leave the buffer mapped as this is a dynamic buffer that will be changed often
    return buffer;
  }

  VUBufferMemory VMBufferManager::CreateStaticBuffer(ReadOnlyFlexSpan bufferSpan, const std::size_t elementCapacity,
                                                     const VkBufferUsageFlags bufferUsageFlags)
  {
    if ((bufferUsageFlags & (VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)) != 0)
    {
      throw NotSupportedException("Transfer buffers can not created using this method");
    }
    if (bufferSpan.size() > elementCapacity)
    {
      throw std::invalid_argument("elementCount must be <= elementCapacity");
    }

    // IMPROVEMENT:
    // Make a optimization that skips the staging buffer if all VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT is set to VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    // Or something like that

    assert(m_physicalDevice.IsValid());
    assert(m_device != VK_NULL_HANDLE);

    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = elementCapacity * bufferSpan.stride();
    bufferCreateInfo.usage = bufferUsageFlags | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VUBufferMemory stagingBuffer(m_physicalDevice, m_device, bufferCreateInfo,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.Upload(0, bufferSpan.data(), bufferSpan.byte_size());

    bufferCreateInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VUBufferMemory finalBuffer(m_physicalDevice, m_device, bufferCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    CopyBuffer(finalBuffer, stagingBuffer);
    return finalBuffer;
  }


  void VMBufferManager::CopyBuffer(VUBufferMemory& rDst, const VUBufferMemory& src)
  {
    assert(m_commandBuffer.IsValid());
    assert(rDst.IsValid());
    assert(src.IsValid());
    assert(rDst.GetBufferSize() >= src.GetBufferSize());

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    m_commandBuffer.Begin(beginInfo);
    {
      VkBufferCopy copyRegion{};
      copyRegion.srcOffset = 0;
      copyRegion.dstOffset = 0;
      copyRegion.size = src.GetBufferSize();
      vkCmdCopyBuffer(m_commandBuffer.Get(), src.GetBuffer(), rDst.GetBuffer(), 1, &copyRegion);
    }
    m_commandBuffer.End();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = m_commandBuffer.GetPointer();

    vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_queue);
  }
}
