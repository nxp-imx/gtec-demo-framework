#ifndef FSLUTIL_VULKAN1_0_BATCH_QUADBATCHDESCRIPTORSETS_HPP
#define FSLUTIL_VULKAN1_0_BATCH_QUADBATCHDESCRIPTORSETS_HPP
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

#include <RapidVulkan/CommandBuffers.hpp>
#include <RapidVulkan/CommandPool.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <deque>
#include <vector>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    //! Simple command buffer allocator
    //! It uses a basic bucket allocation scheme.
    class QuadBatchDescriptorSets
    {
      class Bucket
      {
        VkDescriptorSetLayout m_descriptorSetLayout;
        RapidVulkan::DescriptorPool m_descriptorPool;
        std::vector<VkDescriptorSet> m_descriptorSets;
        uint32_t m_entries;
        uint32_t m_index;

      public:
        Bucket(const VkDevice device, const VkDescriptorSetLayout descriptorSetLayout, const uint32_t size);
        void Clear();
        VkDescriptorSet NextFree();
      };

      VkDevice m_device;
      VkDescriptorSetLayout m_descriptorSetLayoutTexture;

      std::deque<Bucket> m_buckets;

      std::vector<VkDescriptorSet> m_activeSets;
      uint32_t m_activeCount;

    public:
      QuadBatchDescriptorSets(const QuadBatchDescriptorSets&) = delete;
      QuadBatchDescriptorSets& operator=(const QuadBatchDescriptorSets&) = delete;

      //! @brief Move assignment operator
      QuadBatchDescriptorSets& operator=(QuadBatchDescriptorSets&& other) noexcept
      {
        if (this != &other)
        {
          // Free existing resources then transfer the content of other to this one and fill other with default values
          if (IsValid())
          {
            Reset();
          }

          // Claim ownership here
          m_device = other.m_device;
          m_descriptorSetLayoutTexture = other.m_descriptorSetLayoutTexture;
          m_buckets = std::move(other.m_buckets);
          m_activeSets = std::move(other.m_activeSets);
          m_activeCount = other.m_activeCount;

          // Remove the data from other
          other.m_device = VK_NULL_HANDLE;
          other.m_descriptorSetLayoutTexture = VK_NULL_HANDLE;
          other.m_activeCount = 0;
        }
        return *this;
      }

      //! @brief Move constructor
      //! Transfer ownership from other to this
      QuadBatchDescriptorSets(QuadBatchDescriptorSets&& other) noexcept
        : m_device(other.m_device)
        , m_descriptorSetLayoutTexture(other.m_descriptorSetLayoutTexture)
        , m_buckets(std::move(other.m_buckets))
        , m_activeSets(std::move(other.m_activeSets))
        , m_activeCount(other.m_activeCount)
      {
        // Remove the data from other
        other.m_device = VK_NULL_HANDLE;
        other.m_descriptorSetLayoutTexture = VK_NULL_HANDLE;
        other.m_activeCount = 0;
      }

      QuadBatchDescriptorSets();
      QuadBatchDescriptorSets(const VkDevice device, const VkDescriptorSetLayout descriptorSetLayout);

      void Reset() noexcept;
      void Reset(const VkDevice device, const VkDescriptorSetLayout descriptorSetLayout);

      bool IsValid() const
      {
        return m_device != VK_NULL_HANDLE;
      }


      VkDescriptorSetLayout GetLayout() const
      {
        return m_descriptorSetLayoutTexture;
      }

      void Clear();

      //! @brief Get the next free
      VkDescriptorSet NextFree();

      uint32_t GetActiveCount() const
      {
        return m_activeCount;
      }

      const VkDescriptorSet* GetActivePointer() const
      {
        return m_activeSets.data();
      }
    };
  }
}

#endif
