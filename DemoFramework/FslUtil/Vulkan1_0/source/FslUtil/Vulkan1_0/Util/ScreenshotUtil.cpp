/****************************************************************************************************************************************************
 * Copyright 2019, 2022 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Span/SpanUtil_Create.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/ScreenshotUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/VulkanConvert.hpp>
#include <FslUtil/Vulkan1_0/VUScopedMapMemory.hpp>
#include <RapidVulkan/CommandBuffer.hpp>
#include <RapidVulkan/CommandPool.hpp>
#include <RapidVulkan/Fence.hpp>
#include <RapidVulkan/Image.hpp>
#include <RapidVulkan/Memory.hpp>
#include <limits>

namespace Fsl::Vulkan::ScreenshotUtil
{
  namespace
  {
    struct ImageRecord
    {
      RapidVulkan::Image TheImage;
      RapidVulkan::Memory TheMemory;
      VkDeviceSize AllocationSize{0};
      VkExtent2D Extent{};
    };


    ImageRecord PrepareDstImage(const VkPhysicalDevice physicalDevice, const VkDevice device, const VkExtent2D& imageExtent,
                                const VkFormat imageFormat)
    {
      // Create the linear dst image
      VkImageCreateInfo dstImageCreateInfo{};
      dstImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      dstImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
      dstImageCreateInfo.format = imageFormat;
      dstImageCreateInfo.extent = {imageExtent.width, imageExtent.height, 1};
      dstImageCreateInfo.mipLevels = 1;
      dstImageCreateInfo.arrayLayers = 1;
      dstImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
      dstImageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
      dstImageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
      dstImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      RapidVulkan::Image dstImage(device, dstImageCreateInfo);

      // Get memory requirements
      VkMemoryRequirements memRequirements{};
      vkGetImageMemoryRequirements(device, dstImage.Get(), &memRequirements);

      VkPhysicalDeviceMemoryProperties memoryProperties{};
      vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

      // Allocate memory
      VkMemoryAllocateInfo dstImageMemoryAllocateInfo{};
      dstImageMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      dstImageMemoryAllocateInfo.allocationSize = memRequirements.size;

      dstImageMemoryAllocateInfo.memoryTypeIndex = MemoryTypeUtil::GetMemoryTypeIndex(
        memoryProperties, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

      RapidVulkan::Memory dstImageMemory(device, dstImageMemoryAllocateInfo);

      // Bind the memory and image
      RapidVulkan::CheckError(vkBindImageMemory(device, dstImage.Get(), dstImageMemory.Get(), 0), "vkBindImageMemory", __FILE__, __LINE__);

      ImageRecord result;
      result.TheImage = std::move(dstImage);
      result.TheMemory = std::move(dstImageMemory);
      result.AllocationSize = dstImageMemoryAllocateInfo.allocationSize;
      result.Extent = imageExtent;
      return result;
    }


    void TransitionImagesToTransferLayout(const VkCommandBuffer commandBuffer, const VkImage dstImage, const VkImage srcImage)
    {
      constexpr VkImageSubresourceRange ImageSubresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
      {
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageMemoryBarrier.image = dstImage;
        imageMemoryBarrier.subresourceRange = ImageSubresourceRange;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                             &imageMemoryBarrier);
      }

      // Transition source image to transfer source layout
      {
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        imageMemoryBarrier.image = srcImage;
        imageMemoryBarrier.subresourceRange = ImageSubresourceRange;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                             &imageMemoryBarrier);
      }
    }

    void TransitionFromTransferLayout(const VkCommandBuffer commandBuffer, const VkImage dstImage, const VkImage srcImage)
    {
      constexpr VkImageSubresourceRange ImageSubresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
      // Transition source image back to source format
      {
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        imageMemoryBarrier.image = srcImage;
        imageMemoryBarrier.subresourceRange = ImageSubresourceRange;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                             &imageMemoryBarrier);
      }

      // Transition destination image to a CPU readable layout
      {
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        imageMemoryBarrier.image = dstImage;
        imageMemoryBarrier.subresourceRange = ImageSubresourceRange;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                             &imageMemoryBarrier);
      }
    }

    Bitmap ExtractToBitmap(const ImageRecord& image, const PixelFormat imagePixelFormat)
    {
      const VkDevice device = image.TheImage.GetDevice();

      // Get information about the image layout
      VkImageSubresource subResource{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0};
      VkSubresourceLayout subResourceLayout{};
      vkGetImageSubresourceLayout(device, image.TheImage.Get(), &subResource, &subResourceLayout);

      if (subResourceLayout.offset > image.AllocationSize)
      {
        throw NotSupportedException("invalid sub resource layout detected");
      }
      if (subResourceLayout.rowPitch > std::numeric_limits<uint32_t>::max())
      {
        throw NotSupportedException("rowPitch size is unsupported");
      }

      // Do the extraction
      void* pImage = nullptr;
      // We use the scoped map class here since it will since its exception safe
      Vulkan::VUScopedMapMemory scopedMap(device, image.TheMemory.Get(), 0, VK_WHOLE_SIZE, 0, &pImage);

      assert(pImage != nullptr);
      assert(subResourceLayout.offset <= image.AllocationSize);

      const uint8_t* pImageMemory = static_cast<const uint8_t*>(pImage) + subResourceLayout.offset;
      const auto sizePx = PxSize2D::Create(NumericCast<int32_t>(image.Extent.width), NumericCast<int32_t>(image.Extent.height));
      assert(subResourceLayout.rowPitch <= std::numeric_limits<uint32_t>::max());
      const auto stride = static_cast<uint32_t>(subResourceLayout.rowPitch);

      // Extract the bitmap data
      return {SpanUtil::CreateReadOnly(pImageMemory, NumericCast<std::size_t>(subResourceLayout.size)), sizePx, imagePixelFormat, stride,
              BitmapOrigin::UpperLeft};
    }
  }


  Bitmap TryCaptureScreenshot(const VkPhysicalDevice physicalDevice, const VkDevice device, const VkQueue queue, const uint32_t queueFamilyIndex,
                              const VkImage srcImage, const VkFormat srcImageFormat, const VkImageUsageFlags srcImageUsageFormats,
                              const VkExtent2D& srcImageExtent, const uint64_t timeout)
  {
    if (physicalDevice == VK_NULL_HANDLE)
    {
      FSLLOG3_DEBUG_WARNING("Invalid physicalDevice, capture cancelled");
      return {};
    }
    if (device == VK_NULL_HANDLE)
    {
      FSLLOG3_DEBUG_WARNING("Invalid device, capture cancelled");
      return {};
    }
    if (queue == VK_NULL_HANDLE)
    {
      FSLLOG3_DEBUG_WARNING("Invalid queue, capture cancelled");
      return {};
    }
    if (srcImage == VK_NULL_HANDLE)
    {
      FSLLOG3_DEBUG_WARNING("Invalid srcImage, capture cancelled");
      return {};
    }
    if ((srcImageUsageFormats & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) == 0u)
    {
      FSLLOG3_DEBUG_WARNING("srcImageUsageFormats did not support VK_IMAGE_USAGE_TRANSFER_SRC_BIT, capture cancelled");
      return {};
    }
    if (srcImageFormat == VK_FORMAT_UNDEFINED)
    {
      FSLLOG3_DEBUG_WARNING("Invalid srcImageFormat, capture cancelled");
      return {};
    }

    auto pixelFormat = VulkanConvert::ToPixelFormat(srcImageFormat);
    if (PixelFormatUtil::IsCompressed(pixelFormat))
    {
      FSLLOG3_WARNING("srcPixelFormat is compressed, capture cancelled");
      return {};
    }
    // We use the same format as the source image as the copy command dont convert it for us
    const VkFormat dstImageFormat = srcImageFormat;

    // We wait for the device to be idle before we start capturing
    if (vkDeviceWaitIdle(device) != VK_SUCCESS)
    {
      FSLLOG3_DEBUG_WARNING("Failed to wait for device idle, capture cancelled");
      return {};
    }

    try
    {
      // Prepare the image that we will 'transfer' the screenshot to
      auto dstImage = PrepareDstImage(physicalDevice, device, srcImageExtent, dstImageFormat);

      VkCommandPoolCreateInfo commandPoolCreateInfo{};
      commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
      commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
      commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

      RapidVulkan::CommandPool commandPool(device, commandPoolCreateInfo);

      VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
      commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      commandBufferAllocateInfo.commandPool = commandPool.Get();
      commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      commandBufferAllocateInfo.commandBufferCount = 1;

      RapidVulkan::CommandBuffer commandBuffer(device, commandBufferAllocateInfo);
      VkCommandBufferBeginInfo commandBufferBeginInfo{};
      commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      commandBuffer.Begin(commandBufferBeginInfo);
      {
        TransitionImagesToTransferLayout(commandBuffer.Get(), dstImage.TheImage.Get(), srcImage);

        // Copy the current content of the swapchain image to the dst buffer
        {
          VkImageCopy imageCopyRegion{};
          imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
          imageCopyRegion.srcSubresource.layerCount = 1;
          // imageCopyRegion.srcOffset;
          imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
          imageCopyRegion.dstSubresource.layerCount = 1;
          // imageCopyRegion.dstOffset;
          imageCopyRegion.extent = {srcImageExtent.width, srcImageExtent.height, 1};

          // Schedule copy
          vkCmdCopyImage(commandBuffer.Get(), srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage.TheImage.Get(),
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);
        }

        TransitionFromTransferLayout(commandBuffer.Get(), dstImage.TheImage.Get(), srcImage);
      }
      commandBuffer.End();

      RapidVulkan::Fence fence;
      fence.Reset(device, 0);

      // Submit to the queue
      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = commandBuffer.GetPointer();

      RapidVulkan::CheckError(vkQueueSubmit(queue, 1, &submitInfo, fence.Get()), "vkQueueSubmit", __FILE__, __LINE__);

      // Wait for the fence to signal that command buffer has finished executing
      fence.WaitForFence(timeout);

      // The dst image now contains the content
      return ExtractToBitmap(dstImage, pixelFormat);
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_VERBOSE2(ex.what());
      return {};
    }
  }
}
