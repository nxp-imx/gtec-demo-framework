#ifndef FSLUTIL_VULKAN1_0_UTIL_COMMANDBUFFERUTIL_HPP
#define FSLUTIL_VULKAN1_0_UTIL_COMMANDBUFFERUTIL_HPP
/*
 * Assorted commonly used Vulkan helper functions
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

// Based on a code by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Extracted from base/VulkanTools and adapted for the DemoFramework.

#include <FslBase/BasicTypes.hpp>
#include <vulkan/vulkan.h>

namespace Fsl::Vulkan::CommandBufferUtil
{
  void SetImageLayout(const VkCommandBuffer cmdBuffer, const VkImage image, const VkImageAspectFlags aspectMask, const VkImageLayout oldImageLayout,
                      const VkImageLayout newImageLayout, const VkImageSubresourceRange& subresourceRange,
                      const VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                      const VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);


  void SetImageLayout(const VkCommandBuffer cmdBuffer, const VkImage image, const VkImageAspectFlags aspectMask, const VkImageLayout oldImageLayout,
                      const VkImageLayout newImageLayout, const VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                      const VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
}

#endif
