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

#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreatorUtil.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Extend/ImageEx.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/CommandBufferUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Fence.hpp>
#include <FslUtil/Vulkan1_0/VUBuffer.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <cstring>
#include <limits>
#include <utility>

using namespace RapidVulkan;

namespace Fsl
{
  namespace Vulkan
  {
    namespace VulkanImageCreatorUtil
    {
      namespace
      {
        Bitmap ConvertToMinimumStride(const Bitmap& src)
        {
          FSLLOG_DEBUG_WARNING("(Performance) Bitmap did not use minimum stride, so a conversion is performed");
          throw NotSupportedException("Bitmap conversion not supported");
        }

        void CopyToBuffer(VUBuffer& rStagingBuffer, const Bitmap& src)
        {
          VUBuffer::ScopedMap scopedMap(rStagingBuffer);

          RawBitmap rawSrc;
          Bitmap::ScopedDirectAccess directAccess(src, rawSrc);
          std::memcpy(scopedMap.pMappedMemory, rawSrc.Content(), rawSrc.GetBufferLength());
        }


        void CopyToBuffer(VUBuffer& rStagingBuffer, const Texture& src)
        {
          VUBuffer::ScopedMap scopedMap(rStagingBuffer);

          RawTexture rawSrc;
          Texture::ScopedDirectAccess directAccess(src, rawSrc);
          std::memcpy(scopedMap.pMappedMemory, rawSrc.GetContent(), rawSrc.GetContentByteSize());
        }


        VkImageCreateInfo FillImageCreateInfo(const VkImageType imageType, const VkFormat format, const VkExtent3D& extent,
                                              const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags,
                                              const TextureInfo& textureInfo, const TextureType textureType)
        {
          // Now prepare a optimal tiled target image
          VkImageCreateInfo imageCreateInfo{};
          imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
          imageCreateInfo.imageType = imageType;
          imageCreateInfo.format = format;
          imageCreateInfo.extent = extent;
          imageCreateInfo.mipLevels = textureInfo.Levels;
          imageCreateInfo.arrayLayers = textureInfo.Layers;
          imageCreateInfo.samples = samples;
          imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
          imageCreateInfo.usage = imageUsageFlags | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
          imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
          imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

          if (textureType == TextureType::TexCube || textureType == TextureType::TexCubeArray)
          {
            imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
          }

          return imageCreateInfo;
        }
      }


      void Create(ImageEx& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkQueue queue,
                  const VkCommandBuffer commandBuffer, const Bitmap& src, const VkAccessFlags accessMask, const VkImageUsageFlags imageUsageFlags)
      {
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

        const auto minimumStride = PixelFormatUtil::CalcMinimumStride(src.Width(), src.GetPixelFormat());
        if (minimumStride != src.Stride())
        {
          Bitmap tempSrc = ConvertToMinimumStride(src);
          if (minimumStride == tempSrc.Stride())
          {
            Create(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, imageUsageFlags);
            return;
          }
          throw NotSupportedException("We only support minimum stride bitmaps");
        }

        const auto srcFormat = ConvertUtil::Convert(src.GetPixelFormat());
        const auto srcExtent = src.GetExtent();
        const VkExtent3D srcExtentEx = {srcExtent.Width, srcExtent.Height, 1};

        const VkDeviceSize resourceSize = src.GetByteSize();

        if (PhysicalDeviceUtil::IsImageTilingAvailable(physicalDevice.Device.Device, VK_IMAGE_TILING_OPTIMAL, srcFormat, VK_IMAGE_TYPE_2D, 0,
                                                       srcExtentEx, 1, 1, samples, resourceSize))
        {
          CreateUsingStaging(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, samples, imageUsageFlags);
        }
        else if (PhysicalDeviceUtil::IsImageTilingAvailable(physicalDevice.Device.Device, VK_IMAGE_TILING_LINEAR, srcFormat, VK_IMAGE_TYPE_2D, 0,
                                                            srcExtentEx, 1, 1, samples, resourceSize))
        {
          CreateLinear(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, samples, imageUsageFlags);
        }
        else
        {
          throw NotSupportedException("The image-resource format was not supported");
        }
      }


      void Create(ImageEx& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkQueue queue,
                  const VkCommandBuffer commandBuffer, const Texture& src, const VkAccessFlags accessMask, const VkImageUsageFlags imageUsageFlags)
      {
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

        const TextureInfo textureInfo = src.GetTextureInfo();
        const auto srcFormat = ConvertUtil::Convert(src.GetPixelFormat());
        const auto srcImageType = ConvertUtil::ToImageType(src.GetTextureType());
        const VkDeviceSize resourceSize = src.GetByteSize();
        const auto srcExtent = ConvertUtil::Convert(src.GetExtent());

        if (PhysicalDeviceUtil::IsImageTilingAvailable(physicalDevice.Device.Device, VK_IMAGE_TILING_OPTIMAL, srcFormat, srcImageType, 0, srcExtent,
                                                       textureInfo.Levels, textureInfo.Layers, samples, resourceSize))
        {
          CreateUsingStaging(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, samples, imageUsageFlags);
        }
        else if (PhysicalDeviceUtil::IsImageTilingAvailable(physicalDevice.Device.Device, VK_IMAGE_TILING_LINEAR, srcFormat, srcImageType, 0,
                                                            srcExtent, textureInfo.Levels, textureInfo.Layers, samples, resourceSize))
        {
          CreateLinear(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, samples, imageUsageFlags);
        }
        else
        {
          throw NotSupportedException("The image-resource format was not supported");
        }
      }


      void CreateLinear(Vulkan::ImageEx& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                        const VkQueue queue, const VkCommandBuffer commandBuffer, const Bitmap& src, const VkAccessFlags accessMask,
                        const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags)
      {
        throw NotImplementedException();
      }


      void CreateLinear(Vulkan::ImageEx& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                        const VkQueue queue, const VkCommandBuffer commandBuffer, const Texture& src, const VkAccessFlags accessMask,
                        const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags)
      {
        throw NotImplementedException();
      }


      void CreateUsingStaging(Vulkan::ImageEx& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                              const VkQueue queue, const VkCommandBuffer commandBuffer, const Bitmap& src, const VkAccessFlags accessMask,
                              const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags)
      {
        const auto minimumStride = PixelFormatUtil::CalcMinimumStride(src.Width(), src.GetPixelFormat());
        if (minimumStride != src.Stride())
        {
          Bitmap tempSrc = ConvertToMinimumStride(src);
          if (minimumStride == tempSrc.Stride())
          {
            CreateUsingStaging(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, samples, imageUsageFlags);
            return;
          }
          throw NotSupportedException("We only support minimum stride bitmaps");
        }

        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = src.GetByteSize();
        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VUBuffer fromStagingBuffer(physicalDevice, device, bufferCreateInfo,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        CopyToBuffer(fromStagingBuffer, src);

        const TextureInfo textureInfo(1, 1, 1);
        VkExtent3D srcExtent{};
        srcExtent.width = src.Width();
        srcExtent.height = src.Height();
        srcExtent.depth = 1;

        std::vector<VkBufferImageCopy> bufferImageCopy(1);
        bufferImageCopy[0].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferImageCopy[0].imageSubresource.mipLevel = 0;
        bufferImageCopy[0].imageSubresource.baseArrayLayer = 0;
        bufferImageCopy[0].imageSubresource.layerCount = 1;
        bufferImageCopy[0].imageExtent = srcExtent;
        bufferImageCopy[0].bufferOffset = 0;

        // Now prepare a optimal tiled target image
        auto imageCreateInfo = FillImageCreateInfo(VK_IMAGE_TYPE_2D, ConvertUtil::Convert(src.GetPixelFormat()), srcExtent, samples, imageUsageFlags,
                                                   textureInfo, TextureType::Tex2D);

        ImageEx toImage(device, imageCreateInfo, accessMask);

        auto memReqs = toImage.GetImageMemoryRequirements();
        VkMemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memReqs.size;
        memoryAllocateInfo.memoryTypeIndex = physicalDevice.GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        Memory toMemory(device, memoryAllocateInfo);

        RAPIDVULKAN_CHECK(vkBindImageMemory(device, toImage.Get(), toMemory.Get(), 0));

        UploadFromStagingToImage(device, commandBuffer, queue, textureInfo, fromStagingBuffer.GetBuffer(), toImage.Get(), bufferImageCopy,
                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // Transfer the result
        rDstImage = std::move(toImage);
        rDstMemory = std::move(toMemory);
      }


      void CreateUsingStaging(Vulkan::ImageEx& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                              const VkQueue queue, const VkCommandBuffer commandBuffer, const Texture& src, const VkAccessFlags accessMask,
                              const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags)
      {
        const TextureInfo textureInfo = src.GetTextureInfo();

        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = src.GetByteSize();
        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VUBuffer fromStagingBuffer(physicalDevice, device, bufferCreateInfo,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        CopyToBuffer(fromStagingBuffer, src);

        const auto fromImageSubresourceRanges = PrepareCopyRegions(src);

        const auto srcTextureType = src.GetTextureType();

        // Now prepare a optimal tiled target image
        auto imageCreateInfo =
          FillImageCreateInfo(ConvertUtil::ToImageType(srcTextureType), ConvertUtil::Convert(src.GetPixelFormat()),
                              ConvertUtil::Convert(src.GetExtent()), samples, imageUsageFlags, textureInfo, src.GetTextureType());

        ImageEx toImage(device, imageCreateInfo, accessMask);

        auto memReqs = toImage.GetImageMemoryRequirements();
        VkMemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memReqs.size;
        memoryAllocateInfo.memoryTypeIndex = physicalDevice.GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        Memory toMemory(device, memoryAllocateInfo);

        RAPIDVULKAN_CHECK(vkBindImageMemory(device, toImage.Get(), toMemory.Get(), 0));

        UploadFromStagingToImage(device, commandBuffer, queue, textureInfo, fromStagingBuffer.GetBuffer(), toImage.Get(), fromImageSubresourceRanges,
                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // Transfer the result
        rDstImage = std::move(toImage);
        rDstMemory = std::move(toMemory);
      }


      std::vector<VkBufferImageCopy> PrepareCopyRegions(const Texture& texture)
      {
        const auto textureInfo = texture.GetTextureInfo();

        std::vector<VkBufferImageCopy> imageCopyRegions(textureInfo.Levels * textureInfo.Faces * textureInfo.Layers);

        std::size_t dstOffset = 0;
        for (uint32_t layerIndex = 0; layerIndex < textureInfo.Layers; ++layerIndex)
        {
          for (uint32_t faceIndex = 0; faceIndex < textureInfo.Faces; ++faceIndex)
          {
            for (uint32_t levelIndex = 0; levelIndex < textureInfo.Levels; ++levelIndex)
            {
              const auto blobRecord = texture.GetTextureBlob(levelIndex, faceIndex, layerIndex);
              const auto blobExtent = texture.GetExtent(levelIndex);

              VkBufferImageCopy bufferImageCopy{};
              bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
              bufferImageCopy.imageSubresource.mipLevel = levelIndex;
              // FIX: handle proper layer calc when using both multiple faces and multiple layers (cube array)
              assert((textureInfo.Layers == 1 && textureInfo.Faces >= 1) || (textureInfo.Layers >= 1 && textureInfo.Faces == 1));
              bufferImageCopy.imageSubresource.baseArrayLayer = (textureInfo.Faces > 1 ? faceIndex : layerIndex);
              bufferImageCopy.imageSubresource.layerCount = 1;
              bufferImageCopy.imageExtent = ConvertUtil::Convert(blobExtent);
              bufferImageCopy.bufferOffset = blobRecord.Offset;

              imageCopyRegions[dstOffset] = bufferImageCopy;
              ++dstOffset;
            }
          }
        }
        return imageCopyRegions;
      }


      void UploadFromStagingToImage(const VkDevice device, const VkCommandBuffer commandBuffer, const VkQueue queue, const TextureInfo& textureInfo,
                                    const VkBuffer fromStagingBuffer, const VkImage toImage,
                                    const std::vector<VkBufferImageCopy>& fromImageSubresourceRanges, const VkImageLayout imageLayout)
      {
        {
          VkCommandBufferBeginInfo commandBufferBeginInfo{};
          commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
          RAPIDVULKAN_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

          VkImageSubresourceRange imageSubresourceRange{};
          imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
          imageSubresourceRange.baseMipLevel = 0;
          imageSubresourceRange.levelCount = textureInfo.Levels;
          imageSubresourceRange.baseArrayLayer = 0;
          imageSubresourceRange.layerCount = textureInfo.Faces * textureInfo.Layers;

          CommandBufferUtil::SetImageLayout(commandBuffer, toImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageSubresourceRange);

          vkCmdCopyBufferToImage(commandBuffer, fromStagingBuffer, toImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                 static_cast<uint32_t>(fromImageSubresourceRanges.size()), fromImageSubresourceRanges.data());

          // Change texture image layout to shader read after all mip levels have been copied
          CommandBufferUtil::SetImageLayout(commandBuffer, toImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageLayout,
                                            imageSubresourceRange);

          RAPIDVULKAN_CHECK(vkEndCommandBuffer(commandBuffer));
        }

        // Create a fence to make sure that the copies have finished before continuing
        Fence copyFence(device, 0);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        RAPIDVULKAN_CHECK(vkQueueSubmit(queue, 1, &submitInfo, copyFence.Get()));
        RAPIDVULKAN_CHECK(vkWaitForFences(device, 1, copyFence.GetPointer(), VK_TRUE, std::numeric_limits<uint64_t>::max()));
      }
    }
  }
}
