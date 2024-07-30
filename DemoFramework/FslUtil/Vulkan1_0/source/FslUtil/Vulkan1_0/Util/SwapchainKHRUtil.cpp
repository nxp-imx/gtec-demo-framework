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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Log/All.hpp>
#include <FslUtil/Vulkan1_0/Log/FmtAll.hpp>
#include <FslUtil/Vulkan1_0/SurfaceFormatInfo.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceKHRUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/SwapchainKHRUtil.hpp>
#include <RapidVulkan/Check.hpp>
#ifdef RAPIDVULKAN_VULKAN_VERSION_MAJOR
// Be sure to include "RapidVulkan/Check.hpp" before this check since it defines it if present.
// Versions of RapidVulkan before this define is set has a bug in this header
#include <RapidVulkan/Debug/Strings/VkColorSpaceKHR.hpp>
#endif
#include <RapidVulkan/Debug/Strings/VkCompositeAlphaFlagBitsKHR.hpp>
#include <RapidVulkan/Debug/Strings/VkFormat.hpp>
#include <RapidVulkan/Debug/Strings/VkImageUsageFlagBits.hpp>
#include <RapidVulkan/Debug/Strings/VkPresentModeKHR.hpp>
#include <RapidVulkan/Debug/Strings/VkSharingMode.hpp>
#include <RapidVulkan/Debug/Strings/VkSurfaceTransformFlagBitsKHR.hpp>
#include <fmt/ostream.h>
#include <algorithm>
#include <cassert>
// Included last as a workaround to ensure all types are found
#include <FslUtil/Vulkan1_0/Debug/BitFlags.hpp>

namespace Fsl::Vulkan::SwapchainKHRUtil
{
  namespace
  {
    inline void LogCreate(const VkSwapchainCreateInfoKHR& swapchainCreateInfo)
    {
      if (Fsl::LogConfig::GetLogLevel() < LogType::Verbose2)
      {
        return;
      }

      FSLLOG3_INFO("Creating swapchain with");
      FSLLOG3_INFO("- swapchainCreateInfo.minImageCount: {}", swapchainCreateInfo.minImageCount);
      FSLLOG3_INFO("- swapchainCreateInfo.imageFormat: {} ({})", static_cast<uint32_t>(swapchainCreateInfo.imageFormat),
                   RapidVulkan::Debug::ToString(swapchainCreateInfo.imageFormat));
#ifdef RAPIDVULKAN_VULKAN_VERSION_MAJOR
      FSLLOG3_INFO("- swapchainCreateInfo.imageColorSpace: {} ({})", static_cast<uint32_t>(swapchainCreateInfo.imageColorSpace),
                   RapidVulkan::Debug::ToString(swapchainCreateInfo.imageColorSpace));
#else
      FSLLOG3_INFO("- swapchainCreateInfo.imageColorSpace: {}", static_cast<uint32_t>(swapchainCreateInfo.imageColorSpace));
#endif
      FSLLOG3_INFO("- swapchainCreateInfo.imageExtent: {}", swapchainCreateInfo.imageExtent);
      FSLLOG3_INFO("- swapchainCreateInfo.imageUsage: {}",
                   Debug::GetBitflagsString(static_cast<VkImageUsageFlagBits>(swapchainCreateInfo.imageUsage)));

      FSLLOG3_INFO("- swapchainCreateInfo.imageSharingMode: {} ({})", static_cast<uint32_t>(swapchainCreateInfo.imageSharingMode),
                   RapidVulkan::Debug::ToString(swapchainCreateInfo.imageSharingMode));

      FSLLOG3_INFO("- swapchainCreateInfo.queueFamilyIndexCount: {}", swapchainCreateInfo.queueFamilyIndexCount);
      FSLLOG3_INFO("- swapchainCreateInfo.preTransform: {}", Debug::GetBitflagsString(swapchainCreateInfo.preTransform));
      FSLLOG3_INFO("- swapchainCreateInfo.compositeAlpha: {}", Debug::GetBitflagsString(swapchainCreateInfo.compositeAlpha));
      FSLLOG3_INFO("- swapchainCreateInfo.presentMode: {} ({})", static_cast<uint32_t>(swapchainCreateInfo.presentMode),
                   RapidVulkan::Debug::ToString(swapchainCreateInfo.presentMode));
      FSLLOG3_INFO("- swapchainCreateInfo.clipped: {}", (swapchainCreateInfo.clipped != VK_FALSE));
    }
  }

  VUSwapchainKHR CreateSwapchain(const VkPhysicalDevice physicalDevice, const VkDevice device, const VkSwapchainCreateFlagsKHR flags,
                                 const VkSurfaceKHR surface, const uint32_t desiredMinImageCount, const uint32_t imageArrayLayers,
                                 const VkImageUsageFlags imageUsage, const VkSharingMode imageSharingMode, const uint32_t queueFamilyIndexCount,
                                 const uint32_t* queueFamilyIndices, const VkCompositeAlphaFlagBitsKHR compositeAlpha, const VkBool32 clipped,
                                 const VkSwapchainKHR oldSwapchain, const VkExtent2D& fallbackExtent, const SurfaceFormatInfo& surfaceFormatInfo)
  {
    return CreateSwapchain(physicalDevice, device, flags, surface, desiredMinImageCount, imageArrayLayers, imageUsage, imageSharingMode,
                           queueFamilyIndexCount, queueFamilyIndices, compositeAlpha, VK_PRESENT_MODE_FIFO_KHR, clipped, oldSwapchain, fallbackExtent,
                           surfaceFormatInfo);
  }


  VUSwapchainKHR CreateSwapchain(const VkPhysicalDevice physicalDevice, const VkDevice device, const VkSwapchainCreateFlagsKHR flags,
                                 const VkSurfaceKHR surface, const uint32_t desiredMinImageCount, const uint32_t imageArrayLayers,
                                 const VkImageUsageFlags imageUsage, const VkSharingMode imageSharingMode, const uint32_t queueFamilyIndexCount,
                                 const uint32_t* queueFamilyIndices, const VkCompositeAlphaFlagBitsKHR compositeAlpha,
                                 const VkPresentModeKHR presentMode, const VkBool32 clipped, const VkSwapchainKHR oldSwapchain,
                                 const VkExtent2D& fallbackExtent, const SurfaceFormatInfo& surfaceFormatInfo)
  {
    if (physicalDevice == VK_NULL_HANDLE || device == VK_NULL_HANDLE)
    {
      throw std::invalid_argument("physicalDevice and device can not be VK_NULL_HANDLE");
    }

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    RAPIDVULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));

    VkSurfaceTransformFlagBitsKHR preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    if ((surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) == 0u)
    {
      preTransform = surfaceCapabilities.currentTransform;
    }

    // currentExtent is the current width and height of the surface, or the special value (0xFFFFFFFF, 0xFFFFFFFF) indicating that the surface
    // size will be determined by the extent of a swapchain targeting the surface.
    if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF && surfaceCapabilities.currentExtent.height == 0xFFFFFFFF)
    {
      FSLLOG3_VERBOSE("Using fallback extent as surface will be determined by the extent of a swapchain targeting the surface");
      surfaceCapabilities.currentExtent = fallbackExtent;
    }

    VkFormat imageFormat = surfaceFormatInfo.Format;
    VkColorSpaceKHR imageColorSpace = surfaceFormatInfo.ColorSpace;

    if (imageFormat == VK_FORMAT_UNDEFINED)
    {
      const auto surfaceFormats = PhysicalDeviceKHRUtil::GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface);
      if (surfaceFormats.empty())
      {
        throw NotFoundException("No surface formats found");
      }
      // We default to the first available surface format.
      imageFormat = surfaceFormats[0].format;
      imageColorSpace = surfaceFormats[0].colorSpace;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.flags = flags;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount =
      std::max(std::min(desiredMinImageCount, surfaceCapabilities.maxImageCount), surfaceCapabilities.minImageCount);
    swapchainCreateInfo.imageFormat = imageFormat;
    swapchainCreateInfo.imageColorSpace = imageColorSpace;
    swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
    swapchainCreateInfo.imageArrayLayers = imageArrayLayers;
    swapchainCreateInfo.imageUsage = imageUsage;
    swapchainCreateInfo.imageSharingMode = imageSharingMode;
    swapchainCreateInfo.queueFamilyIndexCount = queueFamilyIndexCount;
    swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    swapchainCreateInfo.preTransform = preTransform;
    swapchainCreateInfo.compositeAlpha = compositeAlpha;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = clipped;
    swapchainCreateInfo.oldSwapchain = oldSwapchain;

    const auto surfacePresentModes = PhysicalDeviceKHRUtil::GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface);
    if (std::find(surfacePresentModes.begin(), surfacePresentModes.end(), swapchainCreateInfo.presentMode) == surfacePresentModes.end())
    {
      constexpr auto PresentModeFallback = VK_PRESENT_MODE_FIFO_KHR;    // This is the only value of presentMode that is required to be supported
      FSLLOG3_WARNING("PresentMode: {} not supported, using fallback: {}", RapidVulkan::Debug::ToString(swapchainCreateInfo.presentMode),
                      RapidVulkan::Debug::ToString(PresentModeFallback));
      swapchainCreateInfo.presentMode = PresentModeFallback;
    }

    //// Default to allow VK_IMAGE_USAGE_TRANSFER_SRC_BIT so we can do screenshots
    // if ((surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) != 0u)
    //{
    //  swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    //}
    //// Default ot allow VK_IMAGE_USAGE_TRANSFER_DST_BIT so we can do
    // if ((surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) != 0u)
    //{
    //  swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    //}

    FSLLOG3_WARNING_IF(desiredMinImageCount > swapchainCreateInfo.minImageCount,
                       "CreateSwapchain minImageCount was limited to {} instead of {} due to device limits", swapchainCreateInfo.minImageCount,
                       desiredMinImageCount);
    FSLLOG3_VERBOSE_IF(desiredMinImageCount < swapchainCreateInfo.minImageCount,
                       "CreateSwapchain minImageCount was upgraded to {} instead of {} due to device limits", swapchainCreateInfo.minImageCount,
                       desiredMinImageCount);

    LogCreate(swapchainCreateInfo);
    return {device, swapchainCreateInfo};
  }

  std::vector<VkImage> GetSwapchainImagesKHR(const VkDevice device, const VkSwapchainKHR swapchain)
  {
    uint32_t count = 0;
    RAPIDVULKAN_CHECK2(vkGetSwapchainImagesKHR(device, swapchain, &count, nullptr), "could not get swap chain images size");

    std::vector<VkImage> result(count);
    RAPIDVULKAN_CHECK2(vkGetSwapchainImagesKHR(device, swapchain, &count, result.data()), "could not get swap chain image info");
    return result;
  }
}
