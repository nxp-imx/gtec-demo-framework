#ifndef FSLGRAPHICSVULKAN1_0_EXTEND_SWAPCHAINKHREX_HPP
#define FSLGRAPHICSVULKAN1_0_EXTEND_SWAPCHAINKHREX_HPP
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
#include <FslGraphicsVulkan1_0/Common.hpp>
#include <FslGraphicsVulkan1_0/SwapchainKHR.hpp>
#include <FslGraphicsVulkan1_0/SafeType/SwapchainCreateInfoKHRCopy.hpp>
#include <FslBase/Noncopyable.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace Fsl
{
  namespace Vulkan
  {

    //! @brief A semaphore with a Recreate method. This builds upon the existing semaphore code.
    class SwapchainKHREx
    {
      struct ImageInfo
      {
        VkAccessFlags AccessFlags;
        VkImageLayout ImageLayout;

        ImageInfo()
          : AccessFlags(0)
          , ImageLayout(VK_IMAGE_LAYOUT_UNDEFINED)
        {
        }
      };

      SwapchainKHR m_swapchain;
      SwapchainCreateInfoKHRCopy m_createInfo;
      std::vector<VkImage> m_images;
      std::vector<ImageInfo> m_info;
    public:
      SwapchainKHREx(const SwapchainKHREx&) = delete;
      SwapchainKHREx& operator=(const SwapchainKHREx&) = delete;

      //! @brief Move assignment operator
      SwapchainKHREx& operator=(SwapchainKHREx&& other);

        //! @brief Move constructor
      //! Transfer ownership from other to this
      SwapchainKHREx(SwapchainKHREx&& other);

      SwapchainKHREx();
      SwapchainKHREx(const ClaimMode claimMode, const VkDevice device, const VkSwapchainKHR swapchain, const VkSwapchainCreateInfoKHR& createInfo);
      SwapchainKHREx(const VkDevice device, const VkSwapchainCreateInfoKHR& createInfo);
      SwapchainKHREx(const VkDevice device, const VkSwapchainCreateFlagsKHR flags, const VkSurfaceKHR surface, const uint32_t minImageCount, const VkFormat imageFormat, const VkColorSpaceKHR imageColorSpace, const VkExtent2D imageExtent, const uint32_t imageArrayLayers, const VkImageUsageFlags imageUsage, const VkSharingMode imageSharingMode, const uint32_t queueFamilyIndexCount, const uint32_t * pQueueFamilyIndices, const VkSurfaceTransformFlagBitsKHR preTransform, const VkCompositeAlphaFlagBitsKHR compositeAlpha, const VkPresentModeKHR presentMode, const VkBool32 clipped, const VkSwapchainKHR oldSwapchain);

      ~SwapchainKHREx();

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset();

      //! @brief Replaces the managed object with a new one (releasing the old)
      void Reset(const ClaimMode claimMode, const VkDevice device, const VkSwapchainKHR swapchain, const VkSwapchainCreateInfoKHR& createInfo);

      //! @brief Replaces the managed object with a new one (releasing the old)
      void Reset(const VkDevice device, const VkSwapchainCreateInfoKHR& createInfo);

      //! @brief Replaces the managed object with a new one (releasing the old)
      void Reset(const VkDevice device, const VkSwapchainCreateFlagsKHR flags, const VkSurfaceKHR surface, const uint32_t minImageCount, const VkFormat imageFormat, const VkColorSpaceKHR imageColorSpace, const VkExtent2D imageExtent, const uint32_t imageArrayLayers, const VkImageUsageFlags imageUsage, const VkSharingMode imageSharingMode, const uint32_t queueFamilyIndexCount, const uint32_t * pQueueFamilyIndices, const VkSurfaceTransformFlagBitsKHR preTransform, const VkCompositeAlphaFlagBitsKHR compositeAlpha, const VkPresentModeKHR presentMode, const VkBool32 clipped, const VkSwapchainKHR oldSwapchain);


      //! @brief Get the associated 'Device'
      VkDevice GetDevice() const
      {
        return m_swapchain.GetDevice();
      }

      //! @brief Get the associated resource handle
      VkSwapchainKHR Get() const
      {
        return m_swapchain.Get();
      }

      //! @brief Check if this object contains a valid resource
      bool IsValid() const
      {
        return m_swapchain.IsValid();
      }

      void RefreshImages();

      uint32_t GetImageCount() const;

      //! @brief Access the image at the given index
      VkImage operator[] (const std::size_t arrayIndex) const;

      VkFormat GetImageFormat() const;

      VkExtent2D GetImageExtent() const;

      void CmdPipelineBarrier(const VkCommandBuffer cmdBuffer, const VkAccessFlags dstAccessMask, const VkImageLayout newLayout, const VkPipelineStageFlags srcStageMask, const VkPipelineStageFlags dstStageMask, const uint32_t imageIndex);

      VkResult TryQueuePresent(const VkQueue queue, const uint32_t waitSemaphoreCount, const VkSemaphore* pWaitSemaphores, const uint32_t* pImageIndices, VkResult* pResults) const;
    };
  }
}

#endif
