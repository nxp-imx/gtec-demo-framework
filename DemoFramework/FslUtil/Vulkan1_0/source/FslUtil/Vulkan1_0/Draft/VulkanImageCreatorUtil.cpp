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
#include <FslGraphics/Bitmap/RawBitmap.hpp>
#include <FslGraphics/Bitmap/RawCubeBitmap.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Texture/RawTexture.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/CommandBufferUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/DeviceMemoryUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <FslUtil/Vulkan1_0/VUImage.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/Check.hpp>
#include <RapidVulkan/Fence.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
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
        inline void CopyToBuffer(VUBufferMemory& rStagingBuffer, const RawTexture& rawSrc)
        {
          rStagingBuffer.Upload(0, rawSrc.GetContent(), rawSrc.GetByteSize());
        }


        inline void CopyToBuffer(VUBufferMemory& rStagingBuffer, const Texture& src)
        {
          RawTexture rawSrc;
          Texture::ScopedDirectAccess directAccess(src, rawSrc);
          CopyToBuffer(rStagingBuffer, rawSrc);
        }


        VkImageCreateInfo FillLinearImageCreateInfo(const VkImageType imageType, const VkFormat format, const VkExtent3D& extent,
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
          imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
          imageCreateInfo.usage = imageUsageFlags;
          imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
          imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

          if (textureType == TextureType::TexCube || textureType == TextureType::TexCubeArray)
          {
            imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
            imageCreateInfo.arrayLayers = textureInfo.Faces;
          }

          return imageCreateInfo;
        }


        VkImageCreateInfo FillTransferImageCreateInfo(const VkImageType imageType, const VkFormat format, const VkExtent3D& extent,
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
            imageCreateInfo.arrayLayers = textureInfo.Faces;
          }

          return imageCreateInfo;
        }

        //! @brief Create a image from a Bitmap or RawBitmap using staging
        template <typename TBitmap>
        void DoCreateBitmap(VUImage& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                            const VkQueue queue, const VkCommandBuffer commandBuffer, const TBitmap& src, const VkAccessFlags accessMask,
                            const VkImageUsageFlags imageUsageFlags)
        {
          VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

          const auto srcFormat = ConvertUtil::Convert(src.GetPixelFormat());
          const VkDeviceSize resourceSize = src.GetByteSize();
          const auto srcExtent = src.GetExtent();
          const VkExtent3D srcExtentEx = {srcExtent.Width, srcExtent.Height, 1};

          if (PhysicalDeviceUtil::IsImageTilingAvailable(physicalDevice.Device, VK_IMAGE_TILING_OPTIMAL, srcFormat, VK_IMAGE_TYPE_2D, 0, srcExtentEx,
                                                         1, 1, samples, resourceSize))
          {
            CreateUsingStaging(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, samples, imageUsageFlags);
          }
          else if (PhysicalDeviceUtil::IsImageTilingAvailable(physicalDevice.Device, VK_IMAGE_TILING_LINEAR, srcFormat, VK_IMAGE_TYPE_2D, 0,
                                                              srcExtentEx, 1, 1, samples, resourceSize))
          {
            CreateLinear(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, samples, imageUsageFlags);
          }
          else
          {
            throw NotSupportedException("The image-resource format was not supported");
          }
        }

        //! @brief Create a image from a Texture or RawTexture using staging
        template <typename TTexture>
        void DoCreateTexture(VUImage& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                             const VkQueue queue, const VkCommandBuffer commandBuffer, const TTexture& src, const VkAccessFlags accessMask,
                             const VkImageUsageFlags imageUsageFlags)
        {
          VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

          const TextureInfo textureInfo = src.GetTextureInfo();
          const auto srcFormat = ConvertUtil::Convert(src.GetPixelFormat());
          const auto srcImageType = ConvertUtil::ToImageType(src.GetTextureType());
          const VkDeviceSize resourceSize = src.GetByteSize();
          const auto srcExtent = ConvertUtil::Convert(src.GetExtent());

          if (PhysicalDeviceUtil::IsImageTilingAvailable(physicalDevice.Device, VK_IMAGE_TILING_OPTIMAL, srcFormat, srcImageType, 0, srcExtent,
                                                         textureInfo.Levels, textureInfo.Layers, samples, resourceSize))
          {
            CreateUsingStaging(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, samples, imageUsageFlags);
          }
          else if (PhysicalDeviceUtil::IsImageTilingAvailable(physicalDevice.Device, VK_IMAGE_TILING_LINEAR, srcFormat, srcImageType, 0, srcExtent,
                                                              textureInfo.Levels, textureInfo.Layers, samples, resourceSize))
          {
            CreateLinear(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, samples, imageUsageFlags);
          }
          else
          {
            throw NotSupportedException("The image-resource format was not supported");
          }
        }

        void DoPrepareCopyRegions(std::array<VkBufferImageCopy, 6>& rCopyRegions, const RawCubeBitmap& rawCubeBitmap)
        {
          const auto srcExtent = rawCubeBitmap.GetExtent();
          const auto srcByteSize = rawCubeBitmap.GetNegX().GetByteSize();
          uint32_t faceIndex = 0;
          VkDeviceSize bufferOffset = 0;
          VkExtent3D imageExtent{srcExtent.Width, srcExtent.Height, 1};
          for (auto& rCopyRegion : rCopyRegions)
          {
            rCopyRegion = {};
            rCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            rCopyRegion.imageSubresource.baseArrayLayer = faceIndex;
            rCopyRegion.imageSubresource.layerCount = 1;
            rCopyRegion.imageExtent = imageExtent;
            rCopyRegion.bufferOffset = bufferOffset;
            bufferOffset += srcByteSize;
            ++faceIndex;
          }
        }

        // TTexture can be Texture or RawTexture
        template <typename TTexture>
        std::vector<VkBufferImageCopy> DoPrepareCopyRegions(const TTexture& texture)
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

        //! @brief Create a image from a Texture or RawTexture using staging
        template <typename TTexture>
        void DoCreateUsingStaging(Vulkan::VUImage& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                                  const VkQueue queue, const VkCommandBuffer commandBuffer, const TTexture& src, const VkAccessFlags accessMask,
                                  const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags)
        {
          FSL_PARAM_NOT_USED(accessMask);

          const TextureInfo textureInfo = src.GetTextureInfo();

          VkBufferCreateInfo bufferCreateInfo{};
          bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
          bufferCreateInfo.size = src.GetByteSize();
          bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
          bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

          VUBufferMemory fromStagingBuffer(physicalDevice, device, bufferCreateInfo,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

          CopyToBuffer(fromStagingBuffer, src);

          const auto fromImageSubresourceRanges = DoPrepareCopyRegions(src);

          const auto srcTextureType = src.GetTextureType();

          // Now prepare a optimal tiled target image
          auto imageCreateInfo =
            FillTransferImageCreateInfo(ConvertUtil::ToImageType(srcTextureType), ConvertUtil::Convert(src.GetPixelFormat()),
                                        ConvertUtil::Convert(src.GetExtent()), samples, imageUsageFlags, textureInfo, srcTextureType);

          VUImage toImage(device, imageCreateInfo);

          auto memReqs = toImage.GetImageMemoryRequirements();
          VkMemoryAllocateInfo memoryAllocateInfo{};
          memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
          memoryAllocateInfo.allocationSize = memReqs.size;
          memoryAllocateInfo.memoryTypeIndex = physicalDevice.GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

          Memory toMemory(device, memoryAllocateInfo);

          RAPIDVULKAN_CHECK(vkBindImageMemory(device, toImage.Get(), toMemory.Get(), 0));

          auto finalImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
          UploadFromStagingToImage(device, commandBuffer, queue, textureInfo, fromStagingBuffer.GetBuffer(), toImage.Get(),
                                   fromImageSubresourceRanges, finalImageLayout);

          toImage.SetImageLayout(finalImageLayout);

          // Transfer the result
          rDstImage = std::move(toImage);
          rDstMemory = std::move(toMemory);
        }
      }


      void Create(VUImage& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkQueue queue,
                  const VkCommandBuffer commandBuffer, const Bitmap& src, const VkAccessFlags accessMask, const VkImageUsageFlags imageUsageFlags)
      {
        DoCreateBitmap(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, imageUsageFlags);
      }


      void Create(VUImage& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkQueue queue,
                  const VkCommandBuffer commandBuffer, const RawBitmap& src, const VkAccessFlags accessMask, const VkImageUsageFlags imageUsageFlags)
      {
        DoCreateBitmap(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, imageUsageFlags);
      }


      void Create(VUImage& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkQueue queue,
                  const VkCommandBuffer commandBuffer, const RawCubeBitmap& src, const VkAccessFlags accessMask,
                  const VkImageUsageFlags imageUsageFlags)
      {
        DoCreateBitmap(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, imageUsageFlags);
      }


      void Create(VUImage& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkQueue queue,
                  const VkCommandBuffer commandBuffer, const Texture& src, const VkAccessFlags accessMask, const VkImageUsageFlags imageUsageFlags)
      {
        DoCreateTexture(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, imageUsageFlags);
      }


      void Create(VUImage& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkQueue queue,
                  const VkCommandBuffer commandBuffer, const RawTexture& src, const VkAccessFlags accessMask, const VkImageUsageFlags imageUsageFlags)
      {
        DoCreateTexture(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, imageUsageFlags);
      }


      void CreateLinear(Vulkan::VUImage& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                        const VkQueue queue, const VkCommandBuffer commandBuffer, const Bitmap& src, const VkAccessFlags accessMask,
                        const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags)
      {
        RawBitmap srcRaw;
        Bitmap::ScopedDirectAccess directAccess(src, srcRaw);
        CreateLinear(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, srcRaw, accessMask, samples, imageUsageFlags);
      }

      //! @brief Linear allocation of image data
      //! @note  The memory is allocated so its host coherent and visible.
      //!        FIX: this is not optimal for static images!
      //!             This image type is optimal for textures that gets updated by the CPU each frame.
      //!             However we currently transition it to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
      void CreateLinear(Vulkan::VUImage& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                        const VkQueue queue, const VkCommandBuffer commandBuffer, const RawBitmap& src, const VkAccessFlags accessMask,
                        const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags)
      {
        FSL_PARAM_NOT_USED(accessMask);
        FSL_PARAM_NOT_USED(commandBuffer);
        FSL_PARAM_NOT_USED(queue);

        const auto minimumStride = PixelFormatUtil::CalcMinimumStride(src.Width(), src.GetPixelFormat());
        if (minimumStride != src.Stride())
        {
          throw NotSupportedException("We only support minimum stride bitmaps");
        }

        const TextureInfo textureInfo(1, 1, 1);
        VkExtent3D srcExtent{};
        srcExtent.width = src.Width();
        srcExtent.height = src.Height();
        srcExtent.depth = 1;

        auto imageCreateInfo = FillLinearImageCreateInfo(VK_IMAGE_TYPE_2D, ConvertUtil::Convert(src.GetPixelFormat()), srcExtent, samples,
                                                         imageUsageFlags, textureInfo, TextureType::Tex2D);
        VUImage toImage(device, imageCreateInfo);

        const VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        auto memReqs = toImage.GetImageMemoryRequirements();
        VkMemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memReqs.size;
        memoryAllocateInfo.memoryTypeIndex = physicalDevice.GetMemoryTypeIndex(memReqs.memoryTypeBits, memoryPropertyFlags);

        Memory toMemory(device, memoryAllocateInfo);

        RAPIDVULKAN_CHECK(vkBindImageMemory(device, toImage.Get(), toMemory.Get(), 0));

        DeviceMemoryUtil::FastUpload(device, physicalDevice.Properties.limits.nonCoherentAtomSize, toMemory.Get(), memReqs.size, memoryPropertyFlags,
                                     0, 0, src.Content(), src.GetByteSize());

        // Finally transition the image to a optimal layout
        toImage.SetImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // Transfer the result
        rDstImage = std::move(toImage);
        rDstMemory = std::move(toMemory);
      }


      void CreateLinear(Vulkan::VUImage& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                        const VkQueue queue, const VkCommandBuffer commandBuffer, const RawCubeBitmap& src, const VkAccessFlags accessMask,
                        const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags)
      {
        FSL_PARAM_NOT_USED(rDstImage);
        FSL_PARAM_NOT_USED(rDstMemory);
        FSL_PARAM_NOT_USED(physicalDevice);
        FSL_PARAM_NOT_USED(device);
        FSL_PARAM_NOT_USED(queue);
        FSL_PARAM_NOT_USED(commandBuffer);
        FSL_PARAM_NOT_USED(src);
        FSL_PARAM_NOT_USED(accessMask);
        FSL_PARAM_NOT_USED(samples);
        FSL_PARAM_NOT_USED(imageUsageFlags);

        // const auto minimumStride = PixelFormatUtil::CalcMinimumStride(src.Width(), src.GetPixelFormat());
        // if (minimumStride != src.Stride())
        //{
        //  throw NotSupportedException("We only support minimum stride bitmaps");
        //}

        // FIX: implement
        throw NotImplementedException("VulkanImageCreatorUtil::CreateLinear not implemented");
      }


      void CreateLinear(Vulkan::VUImage& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                        const VkQueue queue, const VkCommandBuffer commandBuffer, const Texture& src, const VkAccessFlags accessMask,
                        const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags)
      {
        RawTexture srcRaw;
        Texture::ScopedDirectAccess directAccess(src, srcRaw);
        CreateLinear(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, srcRaw, accessMask, samples, imageUsageFlags);
      }


      void CreateLinear(Vulkan::VUImage& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                        const VkQueue queue, const VkCommandBuffer commandBuffer, const RawTexture& src, const VkAccessFlags accessMask,
                        const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags)
      {
        FSL_PARAM_NOT_USED(rDstImage);
        FSL_PARAM_NOT_USED(rDstMemory);
        FSL_PARAM_NOT_USED(physicalDevice);
        FSL_PARAM_NOT_USED(device);
        FSL_PARAM_NOT_USED(queue);
        FSL_PARAM_NOT_USED(commandBuffer);
        FSL_PARAM_NOT_USED(src);
        FSL_PARAM_NOT_USED(accessMask);
        FSL_PARAM_NOT_USED(samples);
        FSL_PARAM_NOT_USED(imageUsageFlags);
        // FIX: implement
        throw NotImplementedException("VulkanImageCreatorUtil::CreateLinear not implemented");
      }


      void CreateUsingStaging(Vulkan::VUImage& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                              const VkQueue queue, const VkCommandBuffer commandBuffer, const Bitmap& src, const VkAccessFlags accessMask,
                              const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags)
      {
        const auto minimumStride = PixelFormatUtil::CalcMinimumStride(src.Width(), src.GetPixelFormat());
        if (minimumStride != src.Stride())
        {
          throw NotSupportedException("We only support minimum stride bitmaps");
        }

        RawBitmap srcRawBitmap;
        Bitmap::ScopedDirectAccess directAccess(src, srcRawBitmap);
        CreateUsingStaging(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, srcRawBitmap, accessMask, samples, imageUsageFlags);
      }


      void CreateUsingStaging(Vulkan::VUImage& rDstImage, Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device,
                              const VkQueue queue, const VkCommandBuffer commandBuffer, const RawBitmap& src, const VkAccessFlags accessMask,
                              const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags)
      {
        FSL_PARAM_NOT_USED(accessMask);

        const auto minimumStride = PixelFormatUtil::CalcMinimumStride(src.Width(), src.GetPixelFormat());
        if (minimumStride != src.Stride())
        {
          throw UsageErrorException("We only support minimum stride RawBitmap");
        }

        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = src.GetByteSize();
        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VUBufferMemory fromStagingBuffer(physicalDevice, device, bufferCreateInfo,
                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        fromStagingBuffer.Upload(0, src.Content(), src.GetByteSize());

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
        auto imageCreateInfo = FillTransferImageCreateInfo(VK_IMAGE_TYPE_2D, ConvertUtil::Convert(src.GetPixelFormat()), srcExtent, samples,
                                                           imageUsageFlags, textureInfo, TextureType::Tex2D);

        VUImage toImage(device, imageCreateInfo);

        auto memReqs = toImage.GetImageMemoryRequirements();
        VkMemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memReqs.size;
        memoryAllocateInfo.memoryTypeIndex = physicalDevice.GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        Memory toMemory(device, memoryAllocateInfo);

        RAPIDVULKAN_CHECK(vkBindImageMemory(device, toImage.Get(), toMemory.Get(), 0));

        auto finalImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        UploadFromStagingToImage(device, commandBuffer, queue, textureInfo, fromStagingBuffer.GetBuffer(), toImage.Get(), bufferImageCopy,
                                 finalImageLayout);

        toImage.SetImageLayout(finalImageLayout);

        // Transfer the result
        rDstImage = std::move(toImage);
        rDstMemory = std::move(toMemory);
      }


      void CreateUsingStaging(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice,
                              const VkDevice device, const VkQueue queue, const VkCommandBuffer commandBuffer, const RawCubeBitmap& src,
                              const VkAccessFlags accessMask, const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags)
      {
        FSL_PARAM_NOT_USED(accessMask);

        const auto minimumStride = PixelFormatUtil::CalcMinimumStride(src.Width(), src.GetPixelFormat());
        if (minimumStride != src.GetPosX().Stride() || minimumStride != src.GetNegX().Stride() || minimumStride != src.GetPosY().Stride() ||
            minimumStride != src.GetNegY().Stride() || minimumStride != src.GetPosZ().Stride() || minimumStride != src.GetNegZ().Stride())
        {
          throw UsageErrorException("We only support minimum stride RawCubeBitmap");
        }
        const auto srcByteSize = src.GetPosX().GetByteSize();
        assert(srcByteSize == src.GetNegX().GetByteSize());
        assert(srcByteSize == src.GetPosY().GetByteSize());
        assert(srcByteSize == src.GetNegY().GetByteSize());
        assert(srcByteSize == src.GetPosZ().GetByteSize());
        assert(srcByteSize == src.GetNegZ().GetByteSize());

        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = srcByteSize * 6;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VUBufferMemory fromStagingBuffer(physicalDevice, device, bufferCreateInfo,
                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        fromStagingBuffer.Map();
        fromStagingBuffer.Upload((srcByteSize * 0), src.GetPosX().Content(), srcByteSize);
        fromStagingBuffer.Upload((srcByteSize * 1), src.GetNegX().Content(), srcByteSize);
        fromStagingBuffer.Upload((srcByteSize * 2), src.GetPosY().Content(), srcByteSize);
        fromStagingBuffer.Upload((srcByteSize * 3), src.GetNegY().Content(), srcByteSize);
        fromStagingBuffer.Upload((srcByteSize * 4), src.GetPosZ().Content(), srcByteSize);
        fromStagingBuffer.Upload((srcByteSize * 5), src.GetNegZ().Content(), srcByteSize);
        fromStagingBuffer.Unmap();

        std::array<VkBufferImageCopy, 6> fromImageSubresourceRanges{};
        DoPrepareCopyRegions(fromImageSubresourceRanges, src);

        const auto srcExtent = src.GetExtent();
        const VkExtent3D srcExtentEx = {srcExtent.Width, srcExtent.Height, 1};
        TextureInfo textureInfo(1, 6, 1);

        // Now prepare a optimal tiled target image
        auto imageCreateInfo = FillTransferImageCreateInfo(VK_IMAGE_TYPE_2D, ConvertUtil::Convert(src.GetPixelFormat()), srcExtentEx, samples,
                                                           imageUsageFlags, textureInfo, TextureType::TexCube);

        VUImage toImage(device, imageCreateInfo);

        auto memReqs = toImage.GetImageMemoryRequirements();
        VkMemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memReqs.size;
        memoryAllocateInfo.memoryTypeIndex = physicalDevice.GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        Memory toMemory(device, memoryAllocateInfo);

        RAPIDVULKAN_CHECK(vkBindImageMemory(device, toImage.Get(), toMemory.Get(), 0));

        auto finalImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        UploadFromStagingToImage(device, commandBuffer, queue, textureInfo, fromStagingBuffer.GetBuffer(), toImage.Get(), fromImageSubresourceRanges,
                                 finalImageLayout);

        toImage.SetImageLayout(finalImageLayout);

        // Transfer the result
        rDstImage = std::move(toImage);
        rDstMemory = std::move(toMemory);
      }


      void CreateUsingStaging(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice,
                              const VkDevice device, const VkQueue queue, const VkCommandBuffer commandBuffer, const Texture& src,
                              const VkAccessFlags accessMask, const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags)
      {
        DoCreateUsingStaging(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, samples, imageUsageFlags);
      }


      void CreateUsingStaging(VUImage& rDstImage, RapidVulkan::Memory& rDstMemory, const VUPhysicalDeviceRecord& physicalDevice,
                              const VkDevice device, const VkQueue queue, const VkCommandBuffer commandBuffer, const RawTexture& src,
                              const VkAccessFlags accessMask, const VkSampleCountFlagBits samples, const VkImageUsageFlags imageUsageFlags)
      {
        DoCreateUsingStaging(rDstImage, rDstMemory, physicalDevice, device, queue, commandBuffer, src, accessMask, samples, imageUsageFlags);
      }


      std::vector<VkBufferImageCopy> PrepareCopyRegions(const Texture& texture)
      {
        return DoPrepareCopyRegions(texture);
      }


      std::vector<VkBufferImageCopy> PrepareCopyRegions(const RawTexture& texture)
      {
        return DoPrepareCopyRegions(texture);
      }

      void UploadFromStagingToImage(const VkDevice device, const VkCommandBuffer commandBuffer, const VkQueue queue, const TextureInfo& textureInfo,
                                    const VkBuffer fromStagingBuffer, const VkImage toImage, const VkBufferImageCopy* pFromImageSubresourceRanges,
                                    const std::size_t imageSubresourceRangeCount, const VkImageLayout imageLayout)
      {
        assert(device != VK_NULL_HANDLE);
        assert(commandBuffer != VK_NULL_HANDLE);
        assert(queue != VK_NULL_HANDLE);
        assert(fromStagingBuffer != VK_NULL_HANDLE);
        assert(toImage != VK_NULL_HANDLE);
        assert(pFromImageSubresourceRanges != nullptr);
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
                                 static_cast<uint32_t>(imageSubresourceRangeCount), pFromImageSubresourceRanges);

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
