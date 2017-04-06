#ifndef VULKANWILLEMSDEMOAPPEXPERIMENTAL_SWAPCHAIN_HPP
#define VULKANWILLEMSDEMOAPPEXPERIMENTAL_SWAPCHAIN_HPP
/*
* Class wrapping access to the swap chain
*
* A swap chain is a collection of framebuffers used for rendering and presentation to the windowing system
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/


// Based on a code by Sascha Willems from https://github.com/SaschaWillems/Vulkan

// Recreated as a DemoFramework freestyle window sample by Freescale (2016)
// This class simulates the functionality found in vulkanswapchain to make it easier
// to port samples.
// It is not a straight port, but it has instead been converted to follow the RAII principle
// used in this framework

#include <FslBase/Math/Extent2D.hpp>
#include <FslGraphicsVulkan1_0/Check.hpp>
#include <FslGraphicsVulkan1_0/SwapchainKHR.hpp>
#include <VulkanExperimental/PhysicalDeviceEx.hpp>
#include <VulkanWillemsDemoAppExperimental/SwapchainBuffers.hpp>
#include <vector>
#include <vulkan/vulkan.h>

namespace Fsl
{
  namespace Willems
  {
    // TODO: consider using SwapchainKHREx
    class Swapchain
    {
      VkDevice m_device;
      Vulkan::SwapchainKHR m_swapchain;
      std::vector<SwapchainBuffers> m_buffers;
      VkSwapchainCreateInfoKHR m_createInfo;
    public:
      Swapchain(const Swapchain&) = delete;
      Swapchain& operator=(const Swapchain&) = delete;

      //! @brief Move assignment operator
      Swapchain& operator=(Swapchain&& other);

      //! @brief Move constructor
      //! Transfer ownership from other to this
      Swapchain(Swapchain&& other);

      //! @brief Create a 'invalid' Swapchain (use Reset to populate it)
      Swapchain();

      //! @brief Create the requested resource
      Swapchain(const PhysicalDeviceEx& physicalDevice, const VkDevice device, const VkSurfaceKHR surface, const Extent2D extent, const bool enableVSync);

      ~Swapchain();

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset();

      //! @brief Destroys any owned resources and then creates the requested one
      void Reset(const PhysicalDeviceEx& physicalDevice, const VkDevice device, const VkSurfaceKHR surface, const Extent2D extent, const bool enableVSync);

      //! @brief Get the associated resource handle
      VkSwapchainKHR Get() const
      {
        return m_swapchain.Get();
      }

      //! @brief Get a pointer to the associated resource handle
      const VkSwapchainKHR* GetPointer() const
      {
        return m_swapchain.GetPointer();
      }

      //! @brief Check if this object contains a valid resource
      inline bool IsValid() const
      {
        return m_swapchain.IsValid();
      }

      //! @brief Access the resource at a given index
      const SwapchainBuffers& operator[] (const std::size_t arrayIndex) const;


      const uint32_t GetImageCount() const
      {
        return static_cast<uint32_t>(m_buffers.size());
      }

      const VkFormat GetImageFormat() const
      {
        return m_createInfo.imageFormat;
      }

      uint32_t AcquireNextImage(const VkSemaphore presentCompleteSemaphore)
      {
        // By setting timeout to UINT64_MAX we will always wait until the next image has been acquired or an actual error is thrown
        // With that we don't have to handle VK_NOT_READY
        uint32_t imageIndex;
        FSLGRAPHICSVULKAN_CHECK(TryAcquireNextImage(presentCompleteSemaphore, imageIndex));
        return imageIndex;
      }

      VkResult TryAcquireNextImage(const VkSemaphore presentCompleteSemaphore, uint32_t& rImageIndex)
      {
        // By setting timeout to UINT64_MAX we will always wait until the next image has been acquired or an actual error is thrown
        // With that we don't have to handle VK_NOT_READY
        return vkAcquireNextImageKHR(m_device, m_swapchain.Get(), UINT64_MAX, presentCompleteSemaphore, VK_NULL_HANDLE, &rImageIndex);
      }


      //! @brief Queue an image for presentation
      //! @param queue Presentation queue for presenting the image
      //! @param imageIndex Index of the swapchain image to queue for presentation
      //! @param waitSemaphore (Optional) Semaphore that is waited on before the image is presented (only used if != VK_NULL_HANDLE)
      //! @return VkResult of the queue presentation
      VkResult TryQueuePresent(const VkQueue queue, const uint32_t imageIndex, const VkSemaphore waitSemaphore = VK_NULL_HANDLE);

    };
  }
}

#endif
