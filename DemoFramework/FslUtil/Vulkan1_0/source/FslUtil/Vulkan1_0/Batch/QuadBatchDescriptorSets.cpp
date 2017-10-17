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

#include <FslUtil/Vulkan1_0/Batch/QuadBatchDescriptorSets.hpp>
#include <FslBase/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <cassert>
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

      DescriptorSetLayout CreateDescriptorSetLayout(const VkDevice device, const VkDescriptorType descriptorType, const VkShaderStageFlags stageFlags)
      {
        // Descriptor set layout
        VkDescriptorSetLayoutBinding setLayoutBindings{};
        setLayoutBindings.binding = 0;
        setLayoutBindings.descriptorType = descriptorType;
        setLayoutBindings.descriptorCount = 1;
        setLayoutBindings.stageFlags = stageFlags;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.pNext = nullptr;
        descriptorSetLayoutInfo.bindingCount = 1;
        descriptorSetLayoutInfo.pBindings = &setLayoutBindings;

        return DescriptorSetLayout(device, descriptorSetLayoutInfo);
      }
    }


    QuadBatchDescriptorSets::Bucket::Bucket(const VkDevice device, const VkDescriptorSetLayout descriptorSetLayout, const uint32_t size)
      : m_descriptorSetLayout(descriptorSetLayout)
      , m_descriptorPool()
      , m_descriptorSets(size)
      , m_entries(0)
      , m_index(0)
    {
      VkDescriptorPoolSize poolSize{};
      poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSize.descriptorCount = size;

      VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
      descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolCreateInfo.maxSets = size;
      descriptorPoolCreateInfo.poolSizeCount = 1;
      descriptorPoolCreateInfo.pPoolSizes = &poolSize;

      m_descriptorPool.Reset(device, descriptorPoolCreateInfo);
    }


    void QuadBatchDescriptorSets::Bucket::Clear()
    {
      m_index = 0;
    }


    VkDescriptorSet QuadBatchDescriptorSets::Bucket::NextFree()
    {
      if (m_index >= m_descriptorSets.size())
        throw VulkanUsageErrorException("Bucket capacity exceeded");

      if (m_index < m_entries)
      {
        // Reuse a existing set
        auto result = m_descriptorSets[m_index];
        ++m_index;
        return result;
      }
      else
      {
        // Allocate a new descriptor set
        VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
        descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocInfo.descriptorPool = m_descriptorPool.Get();
        descriptorSetAllocInfo.descriptorSetCount = 1;
        descriptorSetAllocInfo.pSetLayouts = &m_descriptorSetLayout;

        VkDescriptorSet descriptorSet{};
        RAPIDVULKAN_CHECK(vkAllocateDescriptorSets(m_descriptorPool.GetDevice(), &descriptorSetAllocInfo, &descriptorSet));

        m_descriptorSets[m_index] = descriptorSet;
        ++m_index;
        ++m_entries;
        return descriptorSet;
      }
    }


    QuadBatchDescriptorSets::QuadBatchDescriptorSets(const VkDevice device)
      : m_descriptorSetLayoutTexture(CreateDescriptorSetLayout(device, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT))
      , m_activeSets(ARRAY_START_SIZE)
      , m_activeCount(0)
    {
      // Allocate the initial bucket
      m_buckets.push_back(Bucket(m_descriptorSetLayoutTexture.GetDevice(), m_descriptorSetLayoutTexture.Get(), BUCKET_SIZE));
    }


    void QuadBatchDescriptorSets::Clear()
    {
      m_activeCount = 0;
      for (auto& rEntry : m_buckets)
        rEntry.Clear();
    }


    VkDescriptorSet QuadBatchDescriptorSets::NextFree()
    {
      if (m_activeCount >= std::numeric_limits<uint32_t>::max())
        throw NotSupportedException("Maximum number of supported buffers reached");

      const uint32_t currentIndex = m_activeCount;

      const uint32_t currentBucketIndex = m_activeCount / BUCKET_SIZE;

      // Check if we need to allocate another bucket
      assert(currentBucketIndex <= m_buckets.size());
      if (currentBucketIndex >= m_buckets.size())
        m_buckets.push_back(Bucket(m_descriptorSetLayoutTexture.GetDevice(), m_descriptorSetLayoutTexture.Get(), BUCKET_SIZE));

      // take the next available command buffer
      const VkDescriptorSet descriptorSet = m_buckets[currentBucketIndex].NextFree();

      ++m_activeCount;
      // ensure we have enough capacity to hold all active buffers in one array
      if (m_activeCount >= m_activeSets.size())
        m_activeSets.resize(m_activeSets.size() + ARRAY_EXPAND_ENTRIES);

      m_activeSets[currentIndex] = descriptorSet;
      return descriptorSet;
    }
  }
}
