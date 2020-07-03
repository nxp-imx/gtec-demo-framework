#ifndef SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANTEXTURELOADER_HPP
#define SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANTEXTURELOADER_HPP
/*
 * Texture loader for Vulkan
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */


// Based on a code by Sascha Willems from https://github.com/SaschaWillems/Vulkan

// Recreated as a DemoFramework freestyle window sample by Freescale (2016)
// This class simulates the functionality found in VulkanTextureLoader to make it easier
// to port samples. It is not a straight port, but it has instead been converted to
// follow the RAII principle used in this framework

#include <RapidVulkan/CommandBuffer.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanTexture.hpp>
#include <memory>
#include <vulkan/vulkan.h>

namespace Fsl
{
  class IContentManager;

  namespace Willems
  {
    class VulkanTextureLoader
    {
      std::shared_ptr<IContentManager> m_contentManager;
      VkPhysicalDevice m_physicalDevice;
      VkDevice m_device;
      VkQueue m_queue;
      // VkCommandPool m_cmdPool;
      RapidVulkan::CommandBuffer m_cmdBuffer;
      float m_maxAnisotropy;

    public:
      VulkanTextureLoader(const std::shared_ptr<IContentManager>& contentManager, const VkPhysicalDevice physicalDevice, const VkDevice device,
                          const VkQueue queue, const VkCommandPool cmdPool, const VkPhysicalDeviceFeatures activeDeviceFeatures);
      ~VulkanTextureLoader();

      //! Load a 2D texture including all mip levels
      //! @param filename File to load
      //! @param format Vulkan format of the image data stored in the file
      //! @param texture Pointer to the texture object to load the image into
      //! @param (Optional) forceLinear Force linear tiling (not advised, defaults to false)
      //! @param (Optional) imageUsageFlags Usage flags for the texture's image (defaults to VK_IMAGE_USAGE_SAMPLED_BIT)
      VulkanTexture LoadTexture(const std::string& filename, const VkFormat format = VK_FORMAT_UNDEFINED, const bool forceLinear = false,
                                const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);

      //! Load a cubemap texture including all mip levels from a single file
      //! @param filename File to load
      //! @param format Vulkan format of the image data stored in the file
      //! @param texture Pointer to the texture object to load the image into
      VulkanTexture LoadCubemap(const std::string& filename, const VkFormat format = VK_FORMAT_UNDEFINED,
                                const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);

      //! Load a texture array including all mip levels from a single file
      //! @param filename File to load
      //! @param format Vulkan format of the image data stored in the file
      //! @param texture Pointer to the texture object to load the image into
      VulkanTexture LoadTextureArray(const std::string& filename, const VkFormat format = VK_FORMAT_UNDEFINED,
                                     const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);
    };
  }
}

#endif
