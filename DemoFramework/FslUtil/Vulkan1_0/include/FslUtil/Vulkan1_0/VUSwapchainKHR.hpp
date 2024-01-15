#ifndef FSLUTIL_VULKAN1_0_VUSWAPCHAINKHR_HPP
#define FSLUTIL_VULKAN1_0_VUSWAPCHAINKHR_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <RapidVulkan/SwapchainKHR.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace Fsl::Vulkan
{
  //! @brief
  class VUSwapchainKHR
  {
    struct ImageInfo
    {
      VkAccessFlags AccessFlags{0};
      VkImageLayout ImageLayout{VK_IMAGE_LAYOUT_UNDEFINED};

      ImageInfo() = default;
    };

    RapidVulkan::SwapchainKHR m_swapchain;
    VkImageUsageFlags m_imageUsageFlags;
    VkFormat m_imageFormat;
    VkExtent2D m_imageExtent;
    std::vector<VkImage> m_images;
    std::vector<ImageInfo> m_info;

  public:
    VUSwapchainKHR(const VUSwapchainKHR&) = delete;
    VUSwapchainKHR& operator=(const VUSwapchainKHR&) = delete;

    //! @brief Move assignment operator
    VUSwapchainKHR& operator=(VUSwapchainKHR&& other) noexcept;

    //! @brief Move constructor
    //! Transfer ownership from other to this
    VUSwapchainKHR(VUSwapchainKHR&& other) noexcept;

    VUSwapchainKHR();
    VUSwapchainKHR(const RapidVulkan::ClaimMode claimMode, const VkDevice device, const VkSwapchainKHR swapchain,
                   const VkSwapchainCreateInfoKHR& createInfo);
    VUSwapchainKHR(const VkDevice device, const VkSwapchainCreateInfoKHR& createInfo);
    VUSwapchainKHR(const VkDevice device, const VkSwapchainCreateFlagsKHR flags, const VkSurfaceKHR surface, const uint32_t minImageCount,
                   const VkFormat imageFormat, const VkColorSpaceKHR imageColorSpace, const VkExtent2D imageExtent, const uint32_t imageArrayLayers,
                   const VkImageUsageFlags imageUsage, const VkSharingMode imageSharingMode, const uint32_t queueFamilyIndexCount,
                   const uint32_t* pQueueFamilyIndices, const VkSurfaceTransformFlagBitsKHR preTransform,
                   const VkCompositeAlphaFlagBitsKHR compositeAlpha, const VkPresentModeKHR presentMode, const VkBool32 clipped,
                   const VkSwapchainKHR oldSwapchain);

    ~VUSwapchainKHR() noexcept;

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset() noexcept;

    //! @brief Replaces the managed object with a new one (releasing the old)
    void Reset(const RapidVulkan::ClaimMode claimMode, const VkDevice device, const VkSwapchainKHR swapchain,
               const VkSwapchainCreateInfoKHR& createInfo);

    //! @brief Replaces the managed object with a new one (releasing the old)
    void Reset(const VkDevice device, const VkSwapchainCreateInfoKHR& createInfo);

    //! @brief Replaces the managed object with a new one (releasing the old)
    void Reset(const VkDevice device, const VkSwapchainCreateFlagsKHR flags, const VkSurfaceKHR surface, const uint32_t minImageCount,
               const VkFormat imageFormat, const VkColorSpaceKHR imageColorSpace, const VkExtent2D imageExtent, const uint32_t imageArrayLayers,
               const VkImageUsageFlags imageUsage, const VkSharingMode imageSharingMode, const uint32_t queueFamilyIndexCount,
               const uint32_t* pQueueFamilyIndices, const VkSurfaceTransformFlagBitsKHR preTransform,
               const VkCompositeAlphaFlagBitsKHR compositeAlpha, const VkPresentModeKHR presentMode, const VkBool32 clipped,
               const VkSwapchainKHR oldSwapchain);


    //! @brief Get the associated 'Device'
    VkDevice GetDevice() const noexcept
    {
      return m_swapchain.GetDevice();
    }

    //! @brief Get the associated resource handle
    VkSwapchainKHR Get() const noexcept
    {
      return m_swapchain.Get();
    }

    //! @brief Check if this object contains a valid resource
    bool IsValid() const noexcept
    {
      return m_swapchain.IsValid();
    }

    void RefreshImages();

    uint32_t GetImageCount() const noexcept;

    //! @brief Access the image at the given index
    VkImage operator[](const std::size_t arrayIndex) const;

    VkImageUsageFlags GetImageUsageFlags() const noexcept
    {
      return m_imageUsageFlags;
    }

    VkFormat GetImageFormat() const noexcept;

    VkExtent2D GetImageExtent() const noexcept;

    void CmdPipelineBarrier(const VkCommandBuffer cmdBuffer, const VkAccessFlags dstAccessMask, const VkImageLayout newLayout,
                            const VkPipelineStageFlags srcStageMask, const VkPipelineStageFlags dstStageMask, const uint32_t imageIndex);

    VkResult TryQueuePresent(const VkQueue queue, const uint32_t waitSemaphoreCount, const VkSemaphore* pWaitSemaphores,
                             const uint32_t* pImageIndices, VkResult* pResults) const;
  };
}

#endif
