/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsDescriptorSetManager.hpp>

namespace Fsl::Vulkan
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr uint32_t MaxDescriptorCount = 64;
      constexpr auto LogType = Fsl::LogType::Verbose5;
    }


    RapidVulkan::DescriptorPool CreateDescriptorPool(const Vulkan::VUDevice& device, const uint32_t count)
    {
      // Example uses one image sampler
      std::array<VkDescriptorPoolSize, 1> poolSizes{};
      poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      poolSizes[0].descriptorCount = count;

      VkDescriptorPoolCreateInfo descriptorPoolInfo{};
      descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descriptorPoolInfo.maxSets = count;
      descriptorPoolInfo.poolSizeCount = UncheckedNumericCast<uint32_t>(poolSizes.size());
      descriptorPoolInfo.pPoolSizes = poolSizes.data();

      return {device.Get(), descriptorPoolInfo};
    }


    RapidVulkan::DescriptorSetLayout CreateDescriptorSetLayout(const VkDevice device, const VkDescriptorType descriptorType,
                                                               const VkShaderStageFlags stageFlags)
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

      return {device, descriptorSetLayoutInfo};
    }

    void UpdateDescriptorSet(const VkDevice device, const VkDescriptorSet descriptorSet, const VkDescriptorImageInfo& textureImageInfo)
    {
      std::array<VkWriteDescriptorSet, 1> writeDescriptorSets{};

      // Binding 0 : Fragment shader texture sampler
      writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writeDescriptorSets[0].pNext = nullptr;
      writeDescriptorSets[0].dstSet = descriptorSet;
      writeDescriptorSets[0].dstBinding = 0;
      writeDescriptorSets[0].descriptorCount = 1;
      writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writeDescriptorSets[0].pImageInfo = &textureImageInfo;

      vkUpdateDescriptorSets(device, UncheckedNumericCast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
    }


    VkDescriptorSet CreateDescriptorSet(const RapidVulkan::DescriptorPool& descriptorPool,
                                        const RapidVulkan::DescriptorSetLayout& descriptorSetLayout)
    {
      assert(descriptorPool.IsValid());
      assert(descriptorSetLayout.IsValid());

      // Allocate a new descriptor set from the global descriptor pool
      VkDescriptorSetAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      allocInfo.descriptorPool = descriptorPool.Get();
      allocInfo.descriptorSetCount = 1;
      allocInfo.pSetLayouts = descriptorSetLayout.GetPointer();

      VkDescriptorSet descriptorSet{VK_NULL_HANDLE};
      RapidVulkan::CheckError(vkAllocateDescriptorSets(descriptorPool.GetDevice(), &allocInfo, &descriptorSet), "vkAllocateDescriptorSets", __FILE__,
                              __LINE__);

      return descriptorSet;
    }
  }


  NativeGraphicsDescriptorSetManager::NativeGraphicsDescriptorSetManager(const VUDevice& device, const uint32_t maxFramesInFlight)
    : m_device(device.Get())
    , m_mainDescriptorPool(CreateDescriptorPool(device, std::max(maxFramesInFlight, 1u) * LocalConfig::MaxDescriptorCount))
    , m_mainDescriptorSetLayout(CreateDescriptorSetLayout(device.Get(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT))
  {
    FSLLOG3(LocalConfig::LogType, "NativeGraphicsDescriptorSetManager::Construct");
  }


  NativeGraphicsDescriptorSetManager ::~NativeGraphicsDescriptorSetManager()
  {
    FSLLOG3(LocalConfig::LogType, "NativeGraphicsDescriptorSetManager::Destruct");
  }


  VkDescriptorSet NativeGraphicsDescriptorSetManager::AcquireDescriptorSet(const VkDescriptorImageInfo& textureImageInfo)
  {
    VkDescriptorSet descriptorSet{VK_NULL_HANDLE};
    if (m_releasedDescriptors.empty())
    {
      descriptorSet = CreateDescriptorSet(m_mainDescriptorPool, m_mainDescriptorSetLayout);
      FSLLOG3(LocalConfig::LogType, "NativeGraphicsDescriptorSetManager::AcquireDescriptorSet: new descriptor");
    }
    else
    {
      descriptorSet = m_releasedDescriptors.back();
      m_releasedDescriptors.pop_back();
      FSLLOG3(LocalConfig::LogType, "NativeGraphicsDescriptorSetManager::AcquireDescriptorSet: reuse descriptor");
    }
    UpdateDescriptorSet(m_device, descriptorSet, textureImageInfo);
    return descriptorSet;
  }


  void NativeGraphicsDescriptorSetManager::ReleaseDescriptorSet(const VkDescriptorSet descriptorSet)
  {
    assert(descriptorSet != VK_NULL_HANDLE);
    m_releasedDescriptors.push_back(descriptorSet);
    FSLLOG3(LocalConfig::LogType, "NativeGraphicsDescriptorSetManager::ReleaseDescriptorSet");
  }
}
