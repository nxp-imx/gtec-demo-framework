#ifndef FSLUTIL_VULKAN1_0_DRAFT_VULKANIMAGECREATORUTIL_HPP
#define FSLUTIL_VULKAN1_0_DRAFT_VULKANIMAGECREATORUTIL_HPP
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

#include <FslGraphics/Texture/TextureInfo.hpp>
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace RapidVulkan
{
  class Memory;
}

namespace Fsl
{
  class Bitmap;
  class ReadOnlyRawBitmap;
  class RawCubeBitmap;
  class ReadOnlyRawTexture;
  class Texture;

  namespace Vulkan
  {
    class VUImage;
    class VUDevice;

    namespace VulkanImageCreatorUtil
    {
      //! @brief Create a rDstImage and rDstMemory object based on the supplied texture using a optimal strategy.
      void Create(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                  const VkQueue queue, const VkCommandBuffer commandBuffer, const Bitmap& src, const VkAccessFlags accessMask,
                  const VkImageUsageFlags imageUsageFlags);

      //! @brief Create a rDstImage and rDstMemory object based on the supplied texture using a optimal strategy.
      void Create(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                  const VkQueue queue, const VkCommandBuffer commandBuffer, const ReadOnlyRawBitmap& src, const VkAccessFlags accessMask,
                  const VkImageUsageFlags imageUsageFlags);

      //! @brief Create a rDstImage and rDstMemory object based on the supplied texture using a optimal strategy.
      void Create(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                  const VkQueue queue, const VkCommandBuffer commandBuffer, const RawCubeBitmap& src, const VkAccessFlags accessMask,
                  const VkImageUsageFlags imageUsageFlags);

      //! @brief Create a rDstImage and rDstMemory object based on the supplied texture using a optimal strategy.
      void Create(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                  const VkQueue queue, const VkCommandBuffer commandBuffer, const Texture& src, const VkAccessFlags accessMask,
                  const VkImageUsageFlags imageUsageFlags);

      //! @brief Create a rDstImage and rDstMemory object based on the supplied texture using a optimal strategy.
      void Create(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                  const VkQueue queue, const VkCommandBuffer commandBuffer, const ReadOnlyRawTexture& src, const VkAccessFlags accessMask,
                  const VkImageUsageFlags imageUsageFlags);


      //! @brief Create a rDstImage and rDstMemory object based on the supplied bitmap using linear.
      void CreateLinear(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                        const VkQueue queue, const VkCommandBuffer commandBuffer, const Bitmap& src, const VkAccessFlags accessMask,
                        const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags);

      //! @brief Create a rDstImage and rDstMemory object based on the supplied bitmap using linear.
      void CreateLinear(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                        const VkQueue queue, const VkCommandBuffer commandBuffer, const ReadOnlyRawBitmap& src, const VkAccessFlags accessMask,
                        const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags);

      //! @brief Create a rDstImage and rDstMemory object based on the supplied bitmap using linear.
      void CreateLinear(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                        const VkQueue queue, const VkCommandBuffer commandBuffer, const RawCubeBitmap& src, const VkAccessFlags accessMask,
                        const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags);

      //! @brief Create a rDstImage and rDstMemory object based on the supplied texture using linear.
      void CreateLinear(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                        const VkQueue queue, const VkCommandBuffer commandBuffer, const Texture& src, const VkAccessFlags accessMask,
                        const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags);

      //! @brief Create a rDstImage and rDstMemory object based on the supplied texture using linear.
      void CreateLinear(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                        const VkQueue queue, const VkCommandBuffer commandBuffer, const ReadOnlyRawTexture& src, const VkAccessFlags accessMask,
                        const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags);


      //! @brief Create a rDstImage and rDstMemory object based on the supplied bitmap using staging.
      void CreateUsingStaging(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice,
                              const VkDevice device, const VkQueue queue, const VkCommandBuffer commandBuffer, const Bitmap& src,
                              const VkAccessFlags accessMask, const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags);

      //! @brief Create a rDstImage and rDstMemory object based on the supplied bitmap using staging.
      void CreateUsingStaging(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice,
                              const VkDevice device, const VkQueue queue, const VkCommandBuffer commandBuffer, const ReadOnlyRawBitmap& src,
                              const VkAccessFlags accessMask, const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags);

      //! @brief Create a rDstImage and rDstMemory object based on the supplied bitmap using staging.
      void CreateUsingStaging(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice,
                              const VkDevice device, const VkQueue queue, const VkCommandBuffer commandBuffer, const RawCubeBitmap& src,
                              const VkAccessFlags accessMask, const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags);

      //! @brief Create a rDstImage and rDstMemory object based on the supplied texture using staging.
      void CreateUsingStaging(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice,
                              const VkDevice device, const VkQueue queue, const VkCommandBuffer commandBuffer, const Texture& src,
                              const VkAccessFlags accessMask, const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags);

      //! @brief Create a rDstImage and rDstMemory object based on the supplied texture using staging.
      void CreateUsingStaging(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice,
                              const VkDevice device, const VkQueue queue, const VkCommandBuffer commandBuffer, const ReadOnlyRawTexture& src,
                              const VkAccessFlags accessMask, const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags);

      //! @brief Extract the regions from the texture
      std::vector<VkBufferImageCopy> PrepareCopyRegions(const Texture& texture);

      //! @brief Extract the regions from the texture
      std::vector<VkBufferImageCopy> PrepareCopyRegions(const ReadOnlyRawTexture& texture);


      //! @brief Do the upload from the staging buffer to the image
      void UploadFromStagingToImage(const VkDevice device, const VkCommandBuffer commandBuffer, const VkQueue queue, const TextureInfo& textureInfo,
                                    const VkBuffer fromStagingBuffer, const VkImage toImage, const VkBufferImageCopy* pFromImageSubresourceRanges,
                                    const std::size_t imageSubresourceRangeCount, const VkImageLayout imageLayout);

      template <std::size_t TSize>
      inline void UploadFromStagingToImage(const VkDevice device, const VkCommandBuffer commandBuffer, const VkQueue queue,
                                           const TextureInfo& textureInfo, const VkBuffer fromStagingBuffer, const VkImage toImage,
                                           const std::array<VkBufferImageCopy, TSize>& fromImageSubresourceRanges, const VkImageLayout imageLayout)
      {
        UploadFromStagingToImage(device, commandBuffer, queue, textureInfo, fromStagingBuffer, toImage, fromImageSubresourceRanges.data(),
                                 fromImageSubresourceRanges.size(), imageLayout);
      }


      //! @brief Do the upload from the staging buffer to the image
      inline void UploadFromStagingToImage(const VkDevice device, const VkCommandBuffer commandBuffer, const VkQueue queue,
                                           const TextureInfo& textureInfo, const VkBuffer fromStagingBuffer, const VkImage toImage,
                                           const std::vector<VkBufferImageCopy>& fromImageSubresourceRanges, const VkImageLayout imageLayout)
      {
        UploadFromStagingToImage(device, commandBuffer, queue, textureInfo, fromStagingBuffer, toImage, fromImageSubresourceRanges.data(),
                                 fromImageSubresourceRanges.size(), imageLayout);
      }
    }
  }
}

#endif
