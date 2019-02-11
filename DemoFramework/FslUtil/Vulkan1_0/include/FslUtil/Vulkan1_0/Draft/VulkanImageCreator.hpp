#ifndef FSLUTIL_VULKAN1_0_DRAFT_VULKANIMAGECREATOR_HPP
#define FSLUTIL_VULKAN1_0_DRAFT_VULKANIMAGECREATOR_HPP
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

#include <RapidVulkan/CommandBuffer.hpp>
#include <RapidVulkan/CommandPool.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslUtil/Vulkan1_0/VUImageMemoryView.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>

namespace Fsl
{
  class Bitmap;
  class RawBitmap;
  class Texture;
  class RawTexture;
  class RawCubeBitmap;

  namespace Vulkan
  {
    //! @brief A helper class for quickly creating VulkanImage's or VulkanTexture's from Bitmap's or Texture's
    class VulkanImageCreator
    {
      VUPhysicalDeviceRecord m_physicalDevice;
      RapidVulkan::CommandPool m_commandPool;
      RapidVulkan::CommandBuffer m_commandBuffer;
      VkQueue m_queue;

    public:
      VulkanImageCreator(const VulkanImageCreator&) = delete;
      VulkanImageCreator& operator=(const VulkanImageCreator&) = delete;

      VulkanImageCreator(const VUDevice& device, const VkQueue queue, const uint32_t queueFamilyIndex);
      ~VulkanImageCreator();

      VUImageMemoryView CreateImage(const Bitmap& src, const std::string& name = std::string(),
                                    const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);
      VUImageMemoryView CreateImage(const Texture& src, const std::string& name = std::string(),
                                    const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);
      VUImageMemoryView CreateImage(const RawBitmap& src, const std::string& name = std::string(),
                                    const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);
      VUImageMemoryView CreateImage(const RawCubeBitmap& src, const std::string& name = std::string(),
                                    const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);
      VUImageMemoryView CreateImage(const RawTexture& src, const std::string& name = std::string(),
                                    const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);

      VUImageMemoryView CreateImage(const Bitmap& srcBitmapPosX, const Bitmap& srcBitmapNegX, const Bitmap& srcBitmapPosY,
                                    const Bitmap& srcBitmapNegY, const Bitmap& srcBitmapPosZ, const Bitmap& srcBitmapNegZ,
                                    const std::string& name = std::string(), const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);

      VUImageMemoryView CreateImage(const RawBitmap& srcBitmapPosX, const RawBitmap& srcBitmapNegX, const RawBitmap& srcBitmapPosY,
                                    const RawBitmap& srcBitmapNegY, const RawBitmap& srcBitmapPosZ, const RawBitmap& srcBitmapNegZ,
                                    const std::string& name = std::string(), const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);


      VUTexture CreateTexture(const Bitmap& src, const VkSamplerCreateInfo& samplerCreateInfo, const std::string& name = std::string(),
                              const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);
      VUTexture CreateTexture(const Texture& src, const VkSamplerCreateInfo& samplerCreateInfo, const std::string& name = std::string(),
                              const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);

      VUTexture CreateTexture(const Bitmap& src, RapidVulkan::Sampler&& sampler, const std::string& name = std::string(),
                              const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);
      VUTexture CreateTexture(const Texture& src, RapidVulkan::Sampler&& sampler, const std::string& name = std::string(),
                              const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);

      VUTexture CreateTexture(const RawBitmap& src, const VkSamplerCreateInfo& samplerCreateInfo, const std::string& name = std::string(),
                              const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);
      VUTexture CreateTexture(const RawCubeBitmap& src, const VkSamplerCreateInfo& samplerCreateInfo, const std::string& name = std::string(),
                              const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);
      VUTexture CreateTexture(const RawTexture& src, const VkSamplerCreateInfo& samplerCreateInfo, const std::string& name = std::string(),
                              const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);

      VUTexture CreateTexture(const RawBitmap& src, RapidVulkan::Sampler&& sampler, const std::string& name = std::string(),
                              const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);
      VUTexture CreateTexture(const RawCubeBitmap& src, RapidVulkan::Sampler&& sampler, const std::string& name = std::string(),
                              const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);
      VUTexture CreateTexture(const RawTexture& src, RapidVulkan::Sampler&& sampler, const std::string& name = std::string(),
                              const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);


      VUTexture CreateTexture(const Bitmap& srcBitmapPosX, const Bitmap& srcBitmapNegX, const Bitmap& srcBitmapPosY, const Bitmap& srcBitmapNegY,
                              const Bitmap& srcBitmapPosZ, const Bitmap& srcBitmapNegZ, const VkSamplerCreateInfo& samplerCreateInfo,
                              const std::string& name = std::string(), const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);
      VUTexture CreateTexture(const RawBitmap& srcBitmapPosX, const RawBitmap& srcBitmapNegX, const RawBitmap& srcBitmapPosY,
                              const RawBitmap& srcBitmapNegY, const RawBitmap& srcBitmapPosZ, const RawBitmap& srcBitmapNegZ,
                              const VkSamplerCreateInfo& samplerCreateInfo, const std::string& name = std::string(),
                              const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);

      VUTexture CreateTexture(const Bitmap& srcBitmapPosX, const Bitmap& srcBitmapNegX, const Bitmap& srcBitmapPosY, const Bitmap& srcBitmapNegY,
                              const Bitmap& srcBitmapPosZ, const Bitmap& srcBitmapNegZ, RapidVulkan::Sampler&& sampler,
                              const std::string& name = std::string(), const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);
      VUTexture CreateTexture(const RawBitmap& srcBitmapPosX, const RawBitmap& srcBitmapNegX, const RawBitmap& srcBitmapPosY,
                              const RawBitmap& srcBitmapNegY, const RawBitmap& srcBitmapPosZ, const RawBitmap& srcBitmapNegZ,
                              RapidVulkan::Sampler&& sampler, const std::string& name = std::string(),
                              const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);
    };
  }
}

#endif
