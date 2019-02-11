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

#include <FslUtil/Vulkan1_0/VUImageMemoryView.hpp>
#include <FslUtil/Vulkan1_0/VUBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslBase/Log/Log.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Fence.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <limits>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    VUImageMemoryView& VUImageMemoryView::operator=(VUImageMemoryView&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
        {
          Reset();
        }

        // Claim ownership here
        m_image = std::move(other.m_image);
        m_memory = std::move(other.m_memory);
        m_imageView = std::move(other.m_imageView);

        // Remove the data from other
      }
      return *this;
    }


    VUImageMemoryView::VUImageMemoryView(VUImageMemoryView&& other) noexcept
      : m_image(std::move(other.m_image))
      , m_memory(std::move(other.m_memory))
      , m_imageView(std::move(other.m_imageView))
    {
      // Remove the data from other
    }


    VUImageMemoryView::VUImageMemoryView() = default;


    VUImageMemoryView::VUImageMemoryView(const VUDevice& device, const VkImageCreateInfo& imageCreateInfo,
                                         const VkImageSubresourceRange& subresourceRange, const VkMemoryPropertyFlags memoryPropertyFlags,
                                         const std::string& name)
      : VUImageMemoryView()
    {
      Reset(device, imageCreateInfo, subresourceRange, memoryPropertyFlags, name);
    }


    VUImageMemoryView::VUImageMemoryView(VUImage&& image, RapidVulkan::Memory&& memory, RapidVulkan::ImageView&& imageView, const std::string& name)
      : VUImageMemoryView()
    {
      Reset(std::move(image), std::move(memory), std::move(imageView), name);
    }


    void VUImageMemoryView::Reset() noexcept
    {
      if (!IsValid())
      {
        return;
      }

      assert(m_image.IsValid());
      assert(m_memory.IsValid());
      assert(m_imageView.IsValid());

      DoReset();
    }


    void VUImageMemoryView::Reset(const VUDevice& device, const VkImageCreateInfo& imageCreateInfo, const VkImageSubresourceRange& subresourceRange,
                                  const VkMemoryPropertyFlags memoryPropertyFlags, const std::string& name)
    {
      if (IsValid())
      {
        Reset();
      }

      try
      {
        m_image.Reset(device.Get(), imageCreateInfo);

        const auto imageMemoryRequirements = m_image.GetImageMemoryRequirements();

        VkMemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = imageMemoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex =
          device.GetPhysicalDevice().GetMemoryTypeIndex(imageMemoryRequirements.memoryTypeBits, memoryPropertyFlags);

        m_memory.Reset(device.Get(), memoryAllocateInfo);

        RAPIDVULKAN_CHECK(vkBindImageMemory(device.Get(), m_image.Get(), m_memory.Get(), 0));

        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = m_image.Get();
        imageViewCreateInfo.viewType =
          (m_image.GetFlags() & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT) != 0u ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = m_image.GetFormat();
        imageViewCreateInfo.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                                          VK_COMPONENT_SWIZZLE_IDENTITY};
        imageViewCreateInfo.subresourceRange = subresourceRange;

        m_imageView.Reset(device.Get(), imageViewCreateInfo);
      }
      catch (const std::exception&)
      {
        DoReset();
        throw;
      }
    }


    void VUImageMemoryView::Reset(VUImage&& image, RapidVulkan::Memory&& memory, RapidVulkan::ImageView&& imageView, const std::string& name)
    {
      if (IsValid())
      {
        Reset();
      }

      try
      {
        m_image = std::move(image);
        m_memory = std::move(memory);
        m_imageView = std::move(imageView);
      }
      catch (const std::exception&)
      {
        DoReset();
        throw;
      }
    }


    void VUImageMemoryView::DoReset() noexcept
    {
      m_imageView.Reset();
      m_memory.Reset();
      m_image.Reset();
    }
  }
}
