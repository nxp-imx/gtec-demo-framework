#ifndef RAPIDVULKAN_SWAPCHAINKHR_HPP
#define RAPIDVULKAN_SWAPCHAINKHR_HPP
//***************************************************************************************************************************************************
//* BSD 3-Clause License
//*
//* Copyright (c) 2016, Rene Thrane
//* All rights reserved.
//*
//* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//*
//* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
//*    documentation and/or other materials provided with the distribution.
//* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this
//*    software without specific prior written permission.
//*
//* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//***************************************************************************************************************************************************

// Auto-generated Vulkan 1.0 C++11 RAII classes by RAIIGen (https://github.com/Unarmed1000/RAIIGen)

#include <RapidVulkan/ClaimMode.hpp>
#include <RapidVulkan/CheckError.hpp>
#include <RapidVulkan/System/Macro.hpp>
#include <vulkan/vulkan.h>
#include <cassert>

namespace RapidVulkan
{
  //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
  class SwapchainKHR
  {
    VkDevice m_device;
    VkSwapchainKHR m_swapchain;
  public:
    SwapchainKHR(const SwapchainKHR&) = delete;
    SwapchainKHR& operator=(const SwapchainKHR&) = delete;

    //! @brief Move assignment operator
    SwapchainKHR& operator=(SwapchainKHR&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_device = other.m_device;
        m_swapchain = other.m_swapchain;

        // Remove the data from other
        other.m_device = VK_NULL_HANDLE;
        other.m_swapchain = VK_NULL_HANDLE;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    SwapchainKHR(SwapchainKHR&& other)
      : m_device(other.m_device)
      , m_swapchain(other.m_swapchain)
    {
      // Remove the data from other
      other.m_device = VK_NULL_HANDLE;
      other.m_swapchain = VK_NULL_HANDLE;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    SwapchainKHR()
      : m_device(VK_NULL_HANDLE)
      , m_swapchain(VK_NULL_HANDLE)
    {
    }

    //! @brief Assume control of the SwapchainKHR (this object becomes responsible for releasing it)
    explicit SwapchainKHR(const ClaimMode claimMode, const VkDevice device, const VkSwapchainKHR swapchain)
      : SwapchainKHR()
    {
      Reset(claimMode, device, swapchain);
    }

    //! @brief Create the requested resource
    //! @note  Function: vkCreateSwapchainKHR
    SwapchainKHR(const VkDevice device, const VkSwapchainCreateInfoKHR& createInfo)
      : SwapchainKHR()
    {
      Reset(device, createInfo);
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Create the requested resource
    //! @note  Function: vkCreateSwapchainKHR
    SwapchainKHR(const VkDevice device, const VkSwapchainCreateFlagsKHR flags, const VkSurfaceKHR surface, const uint32_t minImageCount, const VkFormat imageFormat, const VkColorSpaceKHR imageColorSpace, const VkExtent2D imageExtent, const uint32_t imageArrayLayers, const VkImageUsageFlags imageUsage, const VkSharingMode imageSharingMode, const uint32_t queueFamilyIndexCount, const uint32_t * pQueueFamilyIndices, const VkSurfaceTransformFlagBitsKHR preTransform, const VkCompositeAlphaFlagBitsKHR compositeAlpha, const VkPresentModeKHR presentMode, const VkBool32 clipped, const VkSwapchainKHR oldSwapchain)
      : SwapchainKHR()
    {
      Reset(device, flags, surface, minImageCount, imageFormat, imageColorSpace, imageExtent, imageArrayLayers, imageUsage, imageSharingMode, queueFamilyIndexCount, pQueueFamilyIndices, preTransform, compositeAlpha, presentMode, clipped, oldSwapchain);
    }
#endif

    ~SwapchainKHR()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    VkSwapchainKHR Release() RAPIDVULKAN_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_swapchain;
      m_device = VK_NULL_HANDLE;
      m_swapchain = VK_NULL_HANDLE;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_device != VK_NULL_HANDLE);
      assert(m_swapchain != VK_NULL_HANDLE);

      vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
      m_device = VK_NULL_HANDLE;
      m_swapchain = VK_NULL_HANDLE;
    }

    //! @brief Destroys any owned resources and assume control of the SwapchainKHR (this object becomes responsible for releasing it)
    void Reset(const ClaimMode claimMode, const VkDevice device, const VkSwapchainKHR swapchain)
    {
      if (IsValid())
        Reset();


      m_device = device;
      m_swapchain = swapchain;
    }

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateSwapchainKHR
    //!        If we detect that createInfo.oldSwapchain == m_swapchain then we ensure that m_swapchain isn't destroyed before the new swapchain has been created
    void Reset(const VkDevice device, const VkSwapchainCreateInfoKHR& createInfo)
    {
#ifndef RAPIDVULKAN_DISABLE_PARAM_VALIDATION
      if (device == VK_NULL_HANDLE)
        throw std::invalid_argument("device can not be VK_NULL_HANDLE");
#else
      assert(device != VK_NULL_HANDLE);
#endif

      // Detect a 'special case' where we are referencing this objects resource and
      // need it to create the new resource
      bool delayedReset = false;
      if (IsValid() && createInfo.oldSwapchain != m_swapchain)
      {
        Reset();
        delayedReset = true;
      }

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      VkSwapchainKHR swapchain;
      CheckError(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain), "vkCreateSwapchainKHR", __FILE__, __LINE__);
      if (delayedReset)
        Reset();

      // Everything is ready, so assign the members
      m_device = device;
      m_swapchain = swapchain;
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateSwapchainKHR
    void Reset(const VkDevice device, const VkSwapchainCreateFlagsKHR flags, const VkSurfaceKHR surface, const uint32_t minImageCount, const VkFormat imageFormat, const VkColorSpaceKHR imageColorSpace, const VkExtent2D imageExtent, const uint32_t imageArrayLayers, const VkImageUsageFlags imageUsage, const VkSharingMode imageSharingMode, const uint32_t queueFamilyIndexCount, const uint32_t * pQueueFamilyIndices, const VkSurfaceTransformFlagBitsKHR preTransform, const VkCompositeAlphaFlagBitsKHR compositeAlpha, const VkPresentModeKHR presentMode, const VkBool32 clipped, const VkSwapchainKHR oldSwapchain)
    {
      VkSwapchainCreateInfoKHR createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
      createInfo.pNext = nullptr;
      createInfo.flags = flags;
      createInfo.surface = surface;
      createInfo.minImageCount = minImageCount;
      createInfo.imageFormat = imageFormat;
      createInfo.imageColorSpace = imageColorSpace;
      createInfo.imageExtent = imageExtent;
      createInfo.imageArrayLayers = imageArrayLayers;
      createInfo.imageUsage = imageUsage;
      createInfo.imageSharingMode = imageSharingMode;
      createInfo.queueFamilyIndexCount = queueFamilyIndexCount;
      createInfo.pQueueFamilyIndices = pQueueFamilyIndices;
      createInfo.preTransform = preTransform;
      createInfo.compositeAlpha = compositeAlpha;
      createInfo.presentMode = presentMode;
      createInfo.clipped = clipped;
      createInfo.oldSwapchain = oldSwapchain;

      Reset(device, createInfo);
    }
#endif

    //! @brief Get the associated 'Device'
    VkDevice GetDevice() const
    {
      return m_device;
    }

    //! @brief Get the associated resource handle
    VkSwapchainKHR Get() const
    {
      return m_swapchain;
    }

    //! @brief Get a pointer to the associated resource handle
    const VkSwapchainKHR* GetPointer() const
    {
      return &m_swapchain;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_swapchain != VK_NULL_HANDLE;
    }

    //! @note  Function: vkGetSwapchainImagesKHR
    void GetSwapchainImagesKHR(uint32_t * pSwapchainImageCount, VkImage * pSwapchainImages)
    {
      CheckError(vkGetSwapchainImagesKHR(m_device, m_swapchain, pSwapchainImageCount, pSwapchainImages), "vkGetSwapchainImagesKHR", __FILE__, __LINE__);
    }

    //! @note  Function: vkAcquireNextImageKHR
    void AcquireNextImageKHR(const uint64_t timeout, const VkSemaphore semaphore, const VkFence fence, uint32_t * pImageIndex)
    {
      CheckError(vkAcquireNextImageKHR(m_device, m_swapchain, timeout, semaphore, fence, pImageIndex), "vkAcquireNextImageKHR", __FILE__, __LINE__);
    }


#if VK_HEADER_VERSION >= 51
    //! @note  Function: vkGetSwapchainStatusKHR
    void GetSwapchainStatusKHR()
    {
      CheckError(vkGetSwapchainStatusKHR(m_device, m_swapchain), "vkGetSwapchainStatusKHR", __FILE__, __LINE__);
    }
#endif
  };
}

#endif
