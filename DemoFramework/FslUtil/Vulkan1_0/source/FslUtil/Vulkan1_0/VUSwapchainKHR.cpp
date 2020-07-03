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

#include <FslUtil/Vulkan1_0/VUSwapchainKHR.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <RapidVulkan/Check.hpp>
#include <cassert>
#include <cstring>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    VUSwapchainKHR& VUSwapchainKHR::operator=(VUSwapchainKHR&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
        {
          Reset();
        }

        // Claim ownership here
        m_swapchain = std::move(other.m_swapchain);
        // m_createInfo = std::move(other.m_createInfo);
        m_imageUsageFlags = other.m_imageUsageFlags;
        m_imageFormat = other.m_imageFormat;
        m_imageExtent = other.m_imageExtent;
        m_images = std::move(other.m_images);
        m_info = std::move(other.m_info);

        // Remove the data from other
        other.m_imageUsageFlags = 0u;
        other.m_imageFormat = VK_FORMAT_UNDEFINED;
        other.m_imageExtent = {};
      }
      return *this;
    }


    VUSwapchainKHR::VUSwapchainKHR(VUSwapchainKHR&& other) noexcept
      : m_swapchain(std::move(other.m_swapchain))
      //, m_createInfo(std::move(other.m_createInfo))
      , m_imageUsageFlags(other.m_imageUsageFlags)
      , m_imageFormat(other.m_imageFormat)
      , m_imageExtent(other.m_imageExtent)
      , m_images(std::move(other.m_images))
      , m_info(std::move(other.m_info))
    {
      // Remove the data from other
      other.m_imageUsageFlags = 0u;
      other.m_imageFormat = VK_FORMAT_UNDEFINED;
      other.m_imageExtent = {};
    }


    VUSwapchainKHR::VUSwapchainKHR()
      : m_imageUsageFlags(0u)
      , m_imageFormat(VK_FORMAT_UNDEFINED)
      , m_imageExtent{}
    {
    }


    VUSwapchainKHR::VUSwapchainKHR(const RapidVulkan::ClaimMode claimMode, const VkDevice device, const VkSwapchainKHR swapchain,
                                   const VkSwapchainCreateInfoKHR& createInfo)
      : VUSwapchainKHR()
    {
      Reset(claimMode, device, swapchain, createInfo);
    }


    VUSwapchainKHR::VUSwapchainKHR(const VkDevice device, const VkSwapchainCreateInfoKHR& createInfo)
      : VUSwapchainKHR()
    {
      Reset(device, createInfo);
    }


    VUSwapchainKHR::VUSwapchainKHR(const VkDevice device, const VkSwapchainCreateFlagsKHR flags, const VkSurfaceKHR surface,
                                   const uint32_t minImageCount, const VkFormat imageFormat, const VkColorSpaceKHR imageColorSpace,
                                   const VkExtent2D imageExtent, const uint32_t imageArrayLayers, const VkImageUsageFlags imageUsage,
                                   const VkSharingMode imageSharingMode, const uint32_t queueFamilyIndexCount, const uint32_t* pQueueFamilyIndices,
                                   const VkSurfaceTransformFlagBitsKHR preTransform, const VkCompositeAlphaFlagBitsKHR compositeAlpha,
                                   const VkPresentModeKHR presentMode, const VkBool32 clipped, const VkSwapchainKHR oldSwapchain)
      : VUSwapchainKHR()
    {
      Reset(device, flags, surface, minImageCount, imageFormat, imageColorSpace, imageExtent, imageArrayLayers, imageUsage, imageSharingMode,
            queueFamilyIndexCount, pQueueFamilyIndices, preTransform, compositeAlpha, presentMode, clipped, oldSwapchain);
    }


    VUSwapchainKHR::~VUSwapchainKHR()
    {
      Reset();
    }


    void VUSwapchainKHR::Reset() noexcept
    {
      if (!m_swapchain.IsValid())
      {
        return;
      }

      // if you add things to Reset then remember to add it to all the move places
      // and also the place marked "manual reset"

      m_swapchain.Reset();
      // m_createInfo.Reset();
      m_imageUsageFlags = 0u;
      m_imageFormat = VK_FORMAT_UNDEFINED;
      m_imageExtent = {};
      m_images.clear();
      m_info.clear();
    }


    void VUSwapchainKHR::Reset(const RapidVulkan::ClaimMode claimMode, const VkDevice device, const VkSwapchainKHR swapchain,
                               const VkSwapchainCreateInfoKHR& createInfo)
    {
      if (IsValid())
      {
        Reset();
      }

      try
      {
        m_swapchain.Reset(claimMode, device, swapchain);
        // Clear the old swapchain handle because it will most likely be invalid now
        // m_createInfo.Reset(createInfo, VK_NULL_HANDLE);
        m_imageUsageFlags = createInfo.imageUsage;
        m_imageFormat = createInfo.imageFormat;
        m_imageExtent = createInfo.imageExtent;
        RefreshImages();
      }
      catch (const std::exception&)
      {
        // Keep internal object state consistent
        Reset();
        throw;
      }
    }


    void VUSwapchainKHR::Reset(const VkDevice device, const VkSwapchainCreateInfoKHR& createInfo)
    {
      // Due to the 'oldSwapchain' thing we can now call reset like we normally would
      // so instead we rely on this resetting everything itself as needed
      // if (IsValid())
      //  Reset();

      try
      {
        m_swapchain.Reset(device, createInfo);
        // "manual reset" (since the VkSwapchainCreateInfoKHR might contain our old handle, meaning it needs to be valid for the create)
        // Clear the old swapchain handle because it will most likely be invalid now
        // m_createInfo.Reset(createInfo, VK_NULL_HANDLE);
        m_imageUsageFlags = createInfo.imageUsage;
        m_imageFormat = createInfo.imageFormat;
        m_imageExtent = createInfo.imageExtent;
        m_images.clear();
        m_info.clear();

        RefreshImages();
      }
      catch (const std::exception&)
      {
        // Keep internal object state consistent
        Reset();
        throw;
      }
    }


    void VUSwapchainKHR::Reset(const VkDevice device, const VkSwapchainCreateFlagsKHR flags, const VkSurfaceKHR surface, const uint32_t minImageCount,
                               const VkFormat imageFormat, const VkColorSpaceKHR imageColorSpace, const VkExtent2D imageExtent,
                               const uint32_t imageArrayLayers, const VkImageUsageFlags imageUsage, const VkSharingMode imageSharingMode,
                               const uint32_t queueFamilyIndexCount, const uint32_t* pQueueFamilyIndices,
                               const VkSurfaceTransformFlagBitsKHR preTransform, const VkCompositeAlphaFlagBitsKHR compositeAlpha,
                               const VkPresentModeKHR presentMode, const VkBool32 clipped, const VkSwapchainKHR oldSwapchain)
    {
      VkSwapchainCreateInfoKHR createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
      createInfo.pNext = nullptr;
      createInfo.flags = flags;
      createInfo.surface = surface;
      createInfo.minImageCount = minImageCount;
      createInfo.imageFormat = imageFormat;
      createInfo.imageColorSpace = imageColorSpace;
      createInfo.imageExtent = imageExtent;
      createInfo.imageArrayLayers = imageArrayLayers;
      createInfo.imageUsage = imageUsage;
      createInfo.imageSharingMode = imageSharingMode;
      createInfo.queueFamilyIndexCount = queueFamilyIndexCount;
      createInfo.pQueueFamilyIndices = pQueueFamilyIndices;
      createInfo.preTransform = preTransform;
      createInfo.compositeAlpha = compositeAlpha;
      createInfo.presentMode = presentMode;
      createInfo.clipped = clipped;
      createInfo.oldSwapchain = oldSwapchain;

      Reset(device, createInfo);
    }


    void VUSwapchainKHR::RefreshImages()
    {
      // We dont use the VulkanHelper::GetSwapchainImagesKHR because we want to reuse our existing vector
      uint32_t swapchainImagesCount = 0;
      RAPIDVULKAN_CHECK(vkGetSwapchainImagesKHR(m_swapchain.GetDevice(), m_swapchain.Get(), &swapchainImagesCount, nullptr));

      if (swapchainImagesCount != m_images.size())
      {
        m_images.resize(swapchainImagesCount);
        m_info.resize(swapchainImagesCount);
      }

      RAPIDVULKAN_CHECK(vkGetSwapchainImagesKHR(m_swapchain.GetDevice(), m_swapchain.Get(), &swapchainImagesCount, m_images.data()));
    }


    uint32_t VUSwapchainKHR::GetImageCount() const
    {
      FSLLOG3_DEBUG_WARNING_IF(!IsValid(), "GetImageCount() swapchain is VK_NULL_HANDLE returning 0");
      return static_cast<uint32_t>(m_images.size());
    }


    VkImage VUSwapchainKHR::operator[](const std::size_t arrayIndex) const
    {
      if (arrayIndex > m_images.size())
      {
        FSLLOG3_DEBUG_WARNING_IF(!IsValid(), "[] swapchain is VK_NULL_HANDLE so image count will be zero");
        throw std::invalid_argument("arrayIndex is out of bounds");
      }
      return m_images[arrayIndex];
    }


    VkFormat VUSwapchainKHR::GetImageFormat() const
    {
      // return m_createInfo.Get().imageFormat;
      return m_imageFormat;
    }


    VkExtent2D VUSwapchainKHR::GetImageExtent() const
    {
      // return m_createInfo.Get().imageExtent;
      return m_imageExtent;
    }


    void VUSwapchainKHR::CmdPipelineBarrier(const VkCommandBuffer cmdBuffer, const VkAccessFlags dstAccessMask, const VkImageLayout newLayout,
                                            const VkPipelineStageFlags srcStageMask, const VkPipelineStageFlags dstStageMask,
                                            const uint32_t imageIndex)
    {
      if (imageIndex >= GetImageCount())
      {
        throw std::invalid_argument("Index out of bounds");
      }

      VkImageMemoryBarrier imageMemoryBarrier{};
      imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      imageMemoryBarrier.srcAccessMask = m_info[imageIndex].AccessFlags;
      imageMemoryBarrier.dstAccessMask = dstAccessMask;
      imageMemoryBarrier.oldLayout = m_info[imageIndex].ImageLayout;
      imageMemoryBarrier.newLayout = newLayout;
      imageMemoryBarrier.srcQueueFamilyIndex = 0;
      imageMemoryBarrier.dstQueueFamilyIndex = 0;
      imageMemoryBarrier.image = m_images[imageIndex];
      imageMemoryBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

      vkCmdPipelineBarrier(cmdBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

      m_info[imageIndex].AccessFlags = dstAccessMask;
      m_info[imageIndex].ImageLayout = newLayout;
    }


    VkResult VUSwapchainKHR::TryQueuePresent(const VkQueue queue, const uint32_t waitSemaphoreCount, const VkSemaphore* pWaitSemaphores,
                                             const uint32_t* pImageIndices, VkResult* pResults) const
    {
      VkSwapchainKHR swapchain = m_swapchain.Get();

      VkPresentInfoKHR presentInfo{};
      presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
      presentInfo.waitSemaphoreCount = waitSemaphoreCount;
      presentInfo.pWaitSemaphores = pWaitSemaphores;
      presentInfo.swapchainCount = 1;
      presentInfo.pSwapchains = &swapchain;
      presentInfo.pImageIndices = pImageIndices;
      presentInfo.pResults = pResults;
      return vkQueuePresentKHR(queue, &presentInfo);
    }
  }
}
