/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslUtil/Vulkan1_0/VUFramebuffer.hpp>
#include <array>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    VUFramebuffer& VUFramebuffer::operator=(VUFramebuffer&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
        {
          Reset();
        }

        // Claim ownership here
        m_texture = std::move(other.m_texture);
        m_framebuffer = std::move(other.m_framebuffer);

        // Remove the data from other
      }
      return *this;
    }


    VUFramebuffer::VUFramebuffer(VUFramebuffer&& other) noexcept
      : m_texture(std::move(other.m_texture))
      , m_framebuffer(std::move(other.m_framebuffer))
    {
      // Remove the data from other
    }


    VUFramebuffer::VUFramebuffer() = default;


    VUFramebuffer::VUFramebuffer(VUTexture&& texture, RapidVulkan::Framebuffer&& framebuffer)
      : VUFramebuffer()
    {
      Reset(std::move(texture), std::move(framebuffer));
    }

    VUFramebuffer::VUFramebuffer(const VUDevice& device, const VkExtent2D extent, const VkFormat format, const VkRenderPass renderPass,
                                 const std::string& name)
      : VUFramebuffer()
    {
      Reset(device, extent, format, renderPass, name);
    }

    VUFramebuffer::VUFramebuffer(const VUDevice& device, const VkExtent2D extent, const VkFormat format, const VkRenderPass renderPass,
                                 const VkImageView depthImageView, const std::string& name)
      : VUFramebuffer()
    {
      Reset(device, extent, format, renderPass, depthImageView, name);
    }


    void VUFramebuffer::Reset() noexcept
    {
      if (!IsValid())
      {
        return;
      }

      assert(m_texture.IsValid());
      assert(m_framebuffer.IsValid());

      DoReset();
    }


    void VUFramebuffer::Reset(VUTexture&& texture, RapidVulkan::Framebuffer&& framebuffer)
    {
      if (IsValid())
      {
        Reset();
      }
      if (!texture.IsValid())
      {
        throw std::invalid_argument("texture");
      }
      if (!framebuffer.IsValid())
      {
        throw std::invalid_argument("framebuffer");
      }

      try
      {
        m_texture = std::move(texture);
        m_framebuffer = std::move(framebuffer);
      }
      catch (const std::exception&)
      {
        DoReset();
        throw;
      }
    }

    void VUFramebuffer::Reset(const VUDevice& device, const VkExtent2D extent, const VkFormat format, const VkRenderPass renderPass,
                              const std::string& name)
    {
      Reset(device, extent, format, renderPass, VK_NULL_HANDLE, name);
    }

    void VUFramebuffer::Reset(const VUDevice& device, const VkExtent2D extent, const VkFormat format, const VkRenderPass renderPass,
                              const VkImageView depthImageView, const std::string& name)
    {
      if (IsValid())
      {
        Reset();
      }
      try
      {
        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = format;
        imageCreateInfo.extent = {extent.width, extent.height, 1};
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImageSubresourceRange subresourceRange{};
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = 1;
        subresourceRange.baseArrayLayer = 0;
        subresourceRange.layerCount = 1;

        Vulkan::VUImageMemoryView imageMemoryView(device, imageCreateInfo, subresourceRange, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, name);

        VkSamplerCreateInfo samplerCreateInfo{};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.anisotropyEnable = VK_FALSE;
        samplerCreateInfo.maxAnisotropy = 1.0f;
        samplerCreateInfo.compareEnable = VK_FALSE;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerCreateInfo.minLod = 0.0f;
        samplerCreateInfo.maxLod = 1.0f;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

        m_texture.Reset(std::move(imageMemoryView), samplerCreateInfo);
        // We know the renderPass is configured to transform the image to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL layout before we need to sample it
        // So we store that in the image for now (even though it will only be true at the point in time the attachment is used via a sampler)
        m_texture.SetImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // finally setup the framebuffer
        std::array<VkImageView, 2> imageViews = {m_texture.ImageView().Get(), depthImageView};
        const uint32_t attachmentCount = depthImageView != VK_NULL_HANDLE ? 2 : 1;
        m_framebuffer.Reset(device.Get(), 0, renderPass, attachmentCount, imageViews.data(), extent.width, extent.height, 1);
      }
      catch (const std::exception&)
      {
        DoReset();
        throw;
      }
    }

    void VUFramebuffer::DoReset() noexcept
    {
      m_texture.Reset();
      m_framebuffer.Reset();
    }
  }
}
