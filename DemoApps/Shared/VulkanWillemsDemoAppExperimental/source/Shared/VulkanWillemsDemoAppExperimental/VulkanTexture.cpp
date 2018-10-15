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

#include <Shared/VulkanWillemsDemoAppExperimental/VulkanTexture.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <utility>

namespace Fsl
{
  namespace Willems
  {
    VulkanTexture& VulkanTexture::operator=(VulkanTexture&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
        {
          Reset();
        }

        // Claim ownership here
        m_sampler = std::move(other.m_sampler);
        m_image = std::move(other.m_image);
        m_imageLayout = other.m_imageLayout;
        m_deviceMemory = std::move(other.m_deviceMemory);
        m_view = std::move(other.m_view);
        m_extent = other.m_extent;
        m_mipLevels = other.m_mipLevels;
        m_layerCount = other.m_layerCount;
        m_descriptor = other.m_descriptor;

        // Remove the data from other
        other.m_imageLayout = VkImageLayout{};
        other.m_extent = Extent3D();
        other.m_mipLevels = 0;
        other.m_layerCount = 0;
        other.m_descriptor = VkDescriptorImageInfo{};
      }
      return *this;
    }


    VulkanTexture::VulkanTexture(VulkanTexture&& other) noexcept
      : m_sampler(std::move(other.m_sampler))
      , m_image(std::move(other.m_image))
      , m_imageLayout(other.m_imageLayout)
      , m_deviceMemory(std::move(other.m_deviceMemory))
      , m_view(std::move(other.m_view))
      , m_extent(other.m_extent)
      , m_mipLevels(other.m_mipLevels)
      , m_layerCount(other.m_layerCount)
      , m_descriptor(other.m_descriptor)
    {
      // Remove the data from other
      other.m_imageLayout = VkImageLayout{};
      other.m_extent = Extent3D();
      other.m_mipLevels = 0;
      other.m_layerCount = 0;
      other.m_descriptor = VkDescriptorImageInfo{};
    }


    VulkanTexture::VulkanTexture(RapidVulkan::Sampler&& sampler, RapidVulkan::Image&& image, const VkImageLayout& imageLayout,
                                 RapidVulkan::Memory&& deviceMemory, RapidVulkan::ImageView&& view, const Extent3D& extent, const uint32_t mipLevels,
                                 const uint32_t layerCount, const VkDescriptorImageInfo& descriptor)
      : VulkanTexture()
    {
      Reset(std::move(sampler), std::move(image), imageLayout, std::move(deviceMemory), std::move(view), extent, mipLevels, layerCount, descriptor);
    }


    VulkanTexture::VulkanTexture()
      : m_imageLayout{}
      , m_mipLevels(0)
      , m_layerCount(0)
      , m_descriptor{}
    {
    }


    void VulkanTexture::Reset() noexcept
    {
      if (!IsValid())
      {
        return;
      }

      assert(m_sampler.IsValid());
      assert(m_image.IsValid());
      assert(m_deviceMemory.IsValid());
      assert(m_view.IsValid());

      m_sampler.Reset();
      m_image.Reset();
      m_imageLayout = VkImageLayout{};
      m_deviceMemory.Reset();
      m_view.Reset();
      m_extent = Extent3D();
      m_mipLevels = 0;
      m_layerCount = 0;
      m_descriptor = VkDescriptorImageInfo{};
    }


    void VulkanTexture::Reset(RapidVulkan::Sampler&& sampler, RapidVulkan::Image&& image, const VkImageLayout& imageLayout,
                              RapidVulkan::Memory&& deviceMemory, RapidVulkan::ImageView&& view, const Extent3D& extent, const uint32_t mipLevels,
                              const uint32_t layerCount, const VkDescriptorImageInfo& descriptor)
    {
      if (IsValid())
      {
        Reset();
      }

      const bool hasOneValid = sampler.IsValid() || image.IsValid() || deviceMemory.IsValid() || view.IsValid();
      if (sampler.IsValid() != hasOneValid || image.IsValid() != hasOneValid || deviceMemory.IsValid() != hasOneValid ||
          view.IsValid() != hasOneValid)
      {
        throw std::invalid_argument("Either all objects has to be valid or none must be");
      }

      // Everything must belong to the same device
      assert(sampler.GetDevice() == image.GetDevice());
      assert(image.GetDevice() == deviceMemory.GetDevice());
      assert(deviceMemory.GetDevice() == view.GetDevice());

      m_sampler = std::move(sampler);
      m_image = std::move(image);
      m_imageLayout = imageLayout;
      m_deviceMemory = std::move(deviceMemory);
      m_view = std::move(view);
      m_extent = extent;
      m_mipLevels = mipLevels;
      m_layerCount = layerCount;
      m_descriptor = descriptor;
    }


    void VulkanTexture::SetSampler(RapidVulkan::Sampler&& sampler)
    {
      if (!IsValid())
      {
        throw UsageErrorException("Can not set a sampler on a invalid texture");
      }
      if (!sampler.IsValid())
      {
        throw std::invalid_argument("The sampler must be valid");
      }
      if (sampler.GetDevice() != m_sampler.GetDevice())
      {
        throw UsageErrorException("Sampler must belong to the same device as the texture");
      }

      m_sampler = std::move(sampler);
      m_descriptor.sampler = m_sampler.Get();
    }
  }
}
