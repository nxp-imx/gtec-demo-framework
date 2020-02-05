#ifndef FSLDEMOAPP_VULKAN_BASIC_BUILDRESOURCESCONTEXT_HPP
#define FSLDEMOAPP_VULKAN_BASIC_BUILDRESOURCESCONTEXT_HPP
/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  namespace VulkanBasic
  {
    struct BuildResourcesContext
    {
      VkExtent2D SwapchainImageExtent{};
      VkFormat SwapchainImageFormat{VK_FORMAT_UNDEFINED};
      uint32_t SwapchainImagesCount{0};
      uint32_t MaxFramesInFlight{0};

      //! If there is no depth buffer this will be set to VK_NULL_HANDLE
      VkImageView DepthImageView{VK_NULL_HANDLE};
      //! If there is no depth buffer this will be set to VK_FORMAT_UNDEFINED
      VkFormat DepthBufferImageFormat{VK_FORMAT_UNDEFINED};
      //! If there is no depth buffer this will be 0,0, if there is a depth buffer it will always be >= SwapchainImageExtent
      VkExtent2D DepthBufferImageExtent{};
      VkCommandPool MainCommandPool{VK_NULL_HANDLE};

      BuildResourcesContext() = default;

      BuildResourcesContext(const VkExtent2D& swapchainImageExtent, const VkFormat swapchainImageFormat, const uint32_t swapchainImagesCount,
                            const uint32_t maxFramesInFlight, const VkImageView depthImageView, const VkFormat depthBufferImageFormat,
                            VkExtent2D depthBufferImageExtent, const VkCommandPool mainCommandPool)
        : SwapchainImageExtent(swapchainImageExtent)
        , SwapchainImageFormat(swapchainImageFormat)
        , SwapchainImagesCount(swapchainImagesCount)
        , MaxFramesInFlight(maxFramesInFlight)
        , DepthImageView(depthImageView)
        , DepthBufferImageFormat(depthBufferImageFormat)
        , DepthBufferImageExtent(depthBufferImageExtent)
        , MainCommandPool(mainCommandPool)
      {
      }
    };
  }
}

#endif
