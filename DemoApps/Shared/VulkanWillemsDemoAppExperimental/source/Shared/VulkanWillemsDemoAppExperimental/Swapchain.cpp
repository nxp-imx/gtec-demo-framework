/*
 * Vulkan Example base class
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */


// Based on a code by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)
// This class simulates the functionality found in VulkanExampleBase to make it easier
// to port samples.

#include <Shared/VulkanWillemsDemoAppExperimental/Swapchain.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/Vulkan1_0/DebugStrings.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceKHRUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/SwapchainKHRUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <utility>

using namespace RapidVulkan;

namespace Fsl
{
  using namespace Vulkan;
  using namespace Vulkan::ConvertUtil;
  using namespace Vulkan::PhysicalDeviceKHRUtil;

  namespace Willems
  {
    //! @brief Move assignment operator
    Swapchain& Swapchain::operator=(Swapchain&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
        {
          Reset();
        }

        // Claim ownership here
        m_device = other.m_device;
        m_swapchain = std::move(other.m_swapchain);
        m_buffers = std::move(other.m_buffers);
        m_createInfo = other.m_createInfo;

        // Remove the data from other
        other.m_device = VK_NULL_HANDLE;
        other.m_createInfo = VkSwapchainCreateInfoKHR{};
      }
      return *this;
    }


    Swapchain::Swapchain(Swapchain&& other) noexcept
      : m_device(other.m_device)
      , m_swapchain(std::move(other.m_swapchain))
      , m_buffers(std::move(other.m_buffers))
      , m_createInfo(other.m_createInfo)
    {
      // Remove the data from other
      other.m_device = VK_NULL_HANDLE;
      other.m_createInfo = VkSwapchainCreateInfoKHR{};
    }

    Swapchain::Swapchain()
      : m_device(VK_NULL_HANDLE)
      , m_createInfo{}
    {
    }


    Swapchain::Swapchain(const VkPhysicalDevice physicalDevice, const VkDevice device, const VkSurfaceKHR surface, const Extent2D extent,
                         const bool enableVSync)
      : Swapchain()
    {
      Reset(physicalDevice, device, surface, extent, enableVSync);
    }


    Swapchain::~Swapchain()
    {
      Reset();
    }


    void Swapchain::Reset() noexcept
    {
      if (!IsValid())
      {
        return;
      }

      m_device = VK_NULL_HANDLE;
      m_swapchain.Reset();
      m_buffers.clear();
      m_createInfo = VkSwapchainCreateInfoKHR{};
    }


    void Swapchain::Reset(const VkPhysicalDevice physicalDevice, const VkDevice device, const VkSurfaceKHR surface, const Extent2D extent,
                          const bool enableVSync)
    {
      try
      {
        // Due to the 'oldSwapchain' thing we can now call reset like we normally would
        // so instead we rely on this resetting everything itself as needed


        // Get physical device surface properties and formats
        VkSurfaceCapabilitiesKHR surfCaps{};
        RAPIDVULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCaps));

        const auto presentModes = GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface);

        // Select a present mode for the swapchain

        // The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
        // This mode waits for the vertical blank ("v-sync")
        VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

        // If v-sync is not requested, try to find a mailbox mode
        // It's the lowest latency non-tearing present mode available
        if (!enableVSync)
        {
          for (std::size_t i = 0; i < presentModes.size(); ++i)
          {
            if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            {
              swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
              break;
            }
            if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
            {
              swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            }
          }
        }

        // Determine the number of images
        uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
        if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
        {
          desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
        }

        // Find the transformation of the surface
        VkSurfaceTransformFlagBitsKHR preTransform;
        if ((surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) != 0u)
        {
          // We prefer a non-rotated transform
          preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        }
        else
        {
          preTransform = surfCaps.currentTransform;
        }

        const auto surfaceFormats = GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface);
        VkFormat colorFormat;
        // If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
        // there is no preferred format, so we assume VK_FORMAT_B8G8R8A8_UNORM
        if ((surfaceFormats.size() == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
        {
          colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
        }
        else
        {
          // For now always select the first available color format
          // If you need a specific format (e.g. SRGB) you'd need to
          // iterate over the list of available surface format and
          // check for it's presence
          colorFormat = surfaceFormats[0].format;
        }
        const auto colorSpace = surfaceFormats[0].colorSpace;

        FSLLOG3_INFO("Swapchain present mode: {}", Vulkan::Debug::ToString(swapchainPresentMode));

        m_device = device;
        m_createInfo = VkSwapchainCreateInfoKHR{};
        m_createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        m_createInfo.pNext = nullptr;
        m_createInfo.surface = surface;
        m_createInfo.minImageCount = desiredNumberOfSwapchainImages;
        m_createInfo.imageFormat = colorFormat;
        m_createInfo.imageColorSpace = colorSpace;
        m_createInfo.imageExtent = Convert(extent);
        m_createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        m_createInfo.preTransform = preTransform;
        m_createInfo.imageArrayLayers = 1;
        m_createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        m_createInfo.queueFamilyIndexCount = 0;
        m_createInfo.pQueueFamilyIndices = nullptr;
        m_createInfo.presentMode = swapchainPresentMode;
        m_createInfo.oldSwapchain = m_swapchain.Get();
        // Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
        m_createInfo.clipped = VK_TRUE;
        m_createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        m_swapchain.Reset(device, m_createInfo);

        // Clear the handle since it is now invalid
        m_createInfo.oldSwapchain = VK_NULL_HANDLE;
        m_buffers.clear();

        const auto images = SwapchainKHRUtil::GetSwapchainImagesKHR(device, m_swapchain.Get());

        // Get the swap chain buffers containing the image and imageview
        m_buffers.resize(images.size());
        for (std::size_t i = 0; i < m_buffers.size(); ++i)
        {
          VkImageViewCreateInfo colorAttachmentView{};
          colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
          colorAttachmentView.pNext = nullptr;
          colorAttachmentView.format = colorFormat;
          colorAttachmentView.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
          colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
          colorAttachmentView.subresourceRange.baseMipLevel = 0;
          colorAttachmentView.subresourceRange.levelCount = 1;
          colorAttachmentView.subresourceRange.baseArrayLayer = 0;
          colorAttachmentView.subresourceRange.layerCount = 1;
          colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
          colorAttachmentView.flags = 0;
          colorAttachmentView.image = images[i];

          // Store the data
          m_buffers[i].Image = images[i];
          m_buffers[i].View.Reset(device, colorAttachmentView);
        }
      }
      catch (const std::exception&)
      {
        Reset();
        throw;
      }
    }

    const SwapchainBuffers& Swapchain::operator[](const std::size_t arrayIndex) const
    {
      if (arrayIndex >= m_buffers.size())
      {
        throw std::invalid_argument("index is out of bounds");
      }
      return m_buffers[arrayIndex];
    }


    VkResult Swapchain::TryQueuePresent(const VkQueue queue, const uint32_t imageIndex, const VkSemaphore waitSemaphore)
    {
      VkPresentInfoKHR presentInfo{};
      presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
      presentInfo.pNext = nullptr;
      presentInfo.swapchainCount = 1;
      presentInfo.pSwapchains = m_swapchain.GetPointer();
      presentInfo.pImageIndices = &imageIndex;
      // Check if a wait semaphore has been specified to wait for before presenting the image
      if (waitSemaphore != VK_NULL_HANDLE)
      {
        presentInfo.pWaitSemaphores = &waitSemaphore;
        presentInfo.waitSemaphoreCount = 1;
      }
      return vkQueuePresentKHR(queue, &presentInfo);
    }
  }
}
