/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslUtil/Vulkan1_0/UnitTest/Helper/Common.hpp>
#include <FslUtil/Vulkan1_0/Util/DeviceMemoryUtil.hpp>
#include <FslUtil/Vulkan1_0/UnitTest/Helper/TestFixtureFslUtil_Vulkan1_0.hpp>
#include <FslUtil/Vulkan1_0/Util/InstanceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/QueueUtil.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslBase/Exceptions.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/Device.hpp>
#include <RapidVulkan/Memory.hpp>
#include <RapidVulkan/Instance.hpp>
#include <array>
#include "TestFixtureFslUtil_Vulkan1_0_Compute.hpp"

using namespace Fsl;
using namespace Fsl::Vulkan;

namespace
{
  using TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil = TestFixtureFslUtil_Vulkan1_0_Compute;
}


TEST_P(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil, FastUploadCoherent)
{
  if (!m_instance.IsValid())
  {
    SkipTest("FastUploadCoherent");
    return;
  }

  std::array<float, 4> vertices = {0.0f, 1.0f, 2.0f, 3.0f};

  const VkDeviceSize cbVertexData = vertices.size() * sizeof(float);

  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = cbVertexData;
  bufferCreateInfo.usage = GetParam();
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  RapidVulkan::Buffer buffer(m_device.Get(), bufferCreateInfo);
  auto memoryRequirements = buffer.GetBufferMemoryRequirements();

  uint32_t memoryTypeIndex;
  if (MemoryTypeUtil::TryGetMemoryTypeIndex(memoryTypeIndex, m_device.GetPhysicalDevice().MemoryProperties, memoryRequirements.memoryTypeBits,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
  {
    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = memoryTypeIndex;
    RapidVulkan::Memory memory(m_device.Get(), allocateInfo);

    const auto selectedMemory = m_device.GetPhysicalDevice().MemoryProperties.memoryTypes[memoryTypeIndex];

    DeviceMemoryUtil::FastUploadCoherent(m_device.Get(), memory.Get(), memoryRequirements.size, selectedMemory.propertyFlags, 0, 0, vertices.data(),
                                         cbVertexData);
  }
  else
  {
    SkipTest("FastUploadCoherent");
  }
}

TEST_P(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil, FastUploadNonCoherent)
{
  if (!m_instance.IsValid())
  {
    SkipTest("FastUploadNonCoherent");
    return;
  }

  const auto nonCoherentAtomSize = m_device.GetPhysicalDevice().Properties.limits.nonCoherentAtomSize;

  std::array<float, 4> vertices = {0.0f, 1.0f, 2.0f, 3.0f};

  const VkDeviceSize cbVertexData = vertices.size() * sizeof(float);

  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = cbVertexData;
  bufferCreateInfo.usage = GetParam();
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  RapidVulkan::Buffer buffer(m_device.Get(), bufferCreateInfo);
  auto memoryRequirements = buffer.GetBufferMemoryRequirements();

  uint32_t memoryTypeIndex;
  if (MemoryTypeUtil::TryGetMemoryTypeIndex(memoryTypeIndex, m_device.GetPhysicalDevice().MemoryProperties, memoryRequirements.memoryTypeBits,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
  {
    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = memoryTypeIndex;
    RapidVulkan::Memory memory(m_device.Get(), allocateInfo);


    auto dstDeviceMemorySpan = DeviceMemoryUtil::CalcMemorySpan(memoryRequirements.size, nonCoherentAtomSize, 0, cbVertexData);
    const auto selectedMemory = m_device.GetPhysicalDevice().MemoryProperties.memoryTypes[memoryTypeIndex];

    DeviceMemoryUtil::FastUploadNonCoherent(m_device.Get(), nonCoherentAtomSize, memory.Get(), memoryRequirements.size, selectedMemory.propertyFlags,
                                            dstDeviceMemorySpan, 0, vertices.data(), cbVertexData);
  }
  else
  {
    SkipTest("FastUploadNonCoherent");
  }
}


TEST_P(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil, FastUpload_Coherent)
{
  if (!m_instance.IsValid())
  {
    SkipTest("FastUpload_Coherent");
    return;
  }

  const auto nonCoherentAtomSize = m_device.GetPhysicalDevice().Properties.limits.nonCoherentAtomSize;

  std::array<float, 4> vertices = {0.0f, 1.0f, 2.0f, 3.0f};

  const VkDeviceSize cbVertexData = vertices.size() * sizeof(float);

  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = cbVertexData;
  bufferCreateInfo.usage = GetParam();
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  RapidVulkan::Buffer buffer(m_device.Get(), bufferCreateInfo);
  auto memoryRequirements = buffer.GetBufferMemoryRequirements();

  uint32_t memoryTypeIndex;
  if (MemoryTypeUtil::TryGetMemoryTypeIndex(memoryTypeIndex, m_device.GetPhysicalDevice().MemoryProperties, memoryRequirements.memoryTypeBits,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
  {
    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = memoryTypeIndex;
    RapidVulkan::Memory memory(m_device.Get(), allocateInfo);

    const auto selectedMemory = m_device.GetPhysicalDevice().MemoryProperties.memoryTypes[memoryTypeIndex];

    DeviceMemoryUtil::FastUpload(m_device.Get(), nonCoherentAtomSize, memory.Get(), memoryRequirements.size, selectedMemory.propertyFlags, 0, 0,
                                 vertices.data(), cbVertexData);
  }
  else
  {
    SkipTest("FastUpload_Coherent");
  }
}


TEST_P(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil, FastUpload_NonCoherent)
{
  if (!m_instance.IsValid())
  {
    SkipTest("FastUpload_NonCoherent");
    return;
  }

  const auto nonCoherentAtomSize = m_device.GetPhysicalDevice().Properties.limits.nonCoherentAtomSize;

  std::array<float, 4> vertices = {0.0f, 1.0f, 2.0f, 3.0f};

  const VkDeviceSize cbVertexData = vertices.size() * sizeof(float);

  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = cbVertexData;
  bufferCreateInfo.usage = GetParam();
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  RapidVulkan::Buffer buffer(m_device.Get(), bufferCreateInfo);
  auto memoryRequirements = buffer.GetBufferMemoryRequirements();

  uint32_t memoryTypeIndex;
  if (MemoryTypeUtil::TryGetMemoryTypeIndex(memoryTypeIndex, m_device.GetPhysicalDevice().MemoryProperties, memoryRequirements.memoryTypeBits,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
  {
    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = memoryTypeIndex;
    RapidVulkan::Memory memory(m_device.Get(), allocateInfo);

    const auto selectedMemory = m_device.GetPhysicalDevice().MemoryProperties.memoryTypes[memoryTypeIndex];

    DeviceMemoryUtil::FastUpload(m_device.Get(), nonCoherentAtomSize, memory.Get(), memoryRequirements.size, selectedMemory.propertyFlags, 0, 0,
                                 vertices.data(), cbVertexData);
  }
  else
  {
    SkipTest("FastUpload_NonCoherent");
  }
}


INSTANTIATE_TEST_CASE_P(BufferTypes, TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil,
                        ::testing::Values(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT));
