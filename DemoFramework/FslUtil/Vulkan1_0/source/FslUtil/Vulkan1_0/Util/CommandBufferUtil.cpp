/*
 * Assorted commonly used Vulkan helper functions
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include <FslUtil/Vulkan1_0/Util/CommandBufferUtil.hpp>
#include <FslBase/Log/BasicLog.hpp>

namespace Fsl
{
  namespace Vulkan
  {
    namespace CommandBufferUtil
    {
      // Create an image memory barrier for changing the layout of an image and put it into an active command buffer
      void SetImageLayout(const VkCommandBuffer cmdBuffer, const VkImage image, const VkImageAspectFlags aspectMask,
                          const VkImageLayout oldImageLayout, const VkImageLayout newImageLayout, const VkImageSubresourceRange& subresourceRange,
                          const VkPipelineStageFlags srcStageMask, const VkPipelineStageFlags dstStageMask)
      {
        // Create an image barrier object
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.pNext = nullptr;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        imageMemoryBarrier.oldLayout = oldImageLayout;
        imageMemoryBarrier.newLayout = newImageLayout;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange = subresourceRange;

        // Source layouts (old)
        // Source access mask controls actions that have to be finished on the old layout
        // before it will be transitioned to the new layout
        switch (oldImageLayout)
        {
        case VK_IMAGE_LAYOUT_UNDEFINED:
          // Image layout is undefined (or does not matter)
          // Only valid as initial layout
          // No flags required, listed only for completeness
          imageMemoryBarrier.srcAccessMask = 0;
          break;
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
          // Image is preinitialized
          // Only valid as initial layout for linear images, preserves memory contents
          // Make sure host writes have been finished
          imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
          break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
          // Image is a color attachment
          // Make sure any writes to the color buffer have been finished
          imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
          break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
          // Image is a depth/stencil attachment
          // Make sure any writes to the depth/stencil buffer have been finished
          imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
          break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
          // Image is a transfer source
          // Make sure any reads from the image have been finished
          imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
          break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
          // Image is a transfer destination
          // Make sure any writes to the image have been finished
          imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
          break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
          // Image is read by a shader
          // Make sure any shader reads from the image have been finished
          imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
          break;
        default:
          FSLBASICLOG_WARNING("Unhandled oldImageLayout");
          break;
        }

        // Target layouts (new)
        // Destination access mask controls the dependency for the new image layout
        switch (newImageLayout)
        {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
          // Image will be used as a transfer destination
          // Make sure any writes to the image have been finished
          imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
          break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
          // Image will be used as a transfer source
          // Make sure any reads from the image have been finished
          imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
          break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
          // Image will be used as a color attachment
          // Make sure any writes to the color buffer have been finished
          imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
          break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
          // Image layout will be used as a depth/stencil attachment
          // Make sure any writes to depth/stencil buffer have been finished
          imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
          break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
          // Image will be read in a shader (sampler, input attachment)
          // Make sure any writes to the image have been finished
          if (imageMemoryBarrier.srcAccessMask == 0)
          {
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
          }
          imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
          break;
        default:
          FSLBASICLOG_WARNING("Unhandled newImageLayout");
          break;
        }

        // Put barrier inside setup command buffer
        vkCmdPipelineBarrier(cmdBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
      }


      void SetImageLayout(const VkCommandBuffer cmdBuffer, const VkImage image, const VkImageAspectFlags aspectMask,
                          const VkImageLayout oldImageLayout, const VkImageLayout newImageLayout, const VkPipelineStageFlags srcStageMask,
                          const VkPipelineStageFlags dstStageMask)
      {
        VkImageSubresourceRange subresourceRange{};
        subresourceRange.aspectMask = aspectMask;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = 1;
        subresourceRange.layerCount = 1;
        SetImageLayout(cmdBuffer, image, aspectMask, oldImageLayout, newImageLayout, subresourceRange, srcStageMask, dstStageMask);
      }
    }
  }
}
