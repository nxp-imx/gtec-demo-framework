#ifndef SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANTEXTURE_HPP
#define SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANTEXTURE_HPP
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

#include <FslBase/Math/Pixel/PxExtent3D.hpp>
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <RapidVulkan/CheckError.hpp>
#include <RapidVulkan/Image.hpp>
#include <RapidVulkan/ImageView.hpp>
#include <RapidVulkan/Memory.hpp>
#include <RapidVulkan/Sampler.hpp>
#include <vulkan/vulkan.h>
#include <cassert>

namespace Fsl::Willems
{
  //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
  class VulkanTexture
  {
    RapidVulkan::Sampler m_sampler;
    RapidVulkan::Image m_image;
    VkImageLayout m_imageLayout;
    RapidVulkan::Memory m_deviceMemory;
    RapidVulkan::ImageView m_view;
    PxExtent3D m_extent;
    uint32_t m_mipLevels;
    uint32_t m_layerCount;
    VkDescriptorImageInfo m_descriptor;

  public:
    VulkanTexture(const VulkanTexture&) = delete;
    VulkanTexture& operator=(const VulkanTexture&) = delete;

    //! @brief Move assignment operator
    VulkanTexture& operator=(VulkanTexture&& other) noexcept;

    //! @brief Move constructor
    //! Transfer ownership from other to this
    VulkanTexture(VulkanTexture&& other) noexcept;

    //! @brief Move objects into this object
    VulkanTexture(RapidVulkan::Sampler&& sampler, RapidVulkan::Image&& image, const VkImageLayout& imageLayout, RapidVulkan::Memory&& deviceMemory,
                  RapidVulkan::ImageView&& view, const PxExtent3D& extent, const uint32_t mipLevels, const uint32_t layerCount,
                  const VkDescriptorImageInfo& descriptor);

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    VulkanTexture();

    ~VulkanTexture()
    {
      Reset();
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset() noexcept;

    //! @brief Move objects into this object
    void Reset(RapidVulkan::Sampler&& sampler, RapidVulkan::Image&& image, const VkImageLayout& imageLayout, RapidVulkan::Memory&& deviceMemory,
               RapidVulkan::ImageView&& view, const PxExtent3D& extent, const uint32_t mipLevels, const uint32_t layerCount,
               const VkDescriptorImageInfo& descriptor);

    //! @brief Change the associated sampler (destroys the old one)
    void SetSampler(RapidVulkan::Sampler&& sampler);


    //! @brief Get the associated 'Device'
    VkDevice GetDevice() const
    {
      return m_sampler.GetDevice();
    }

    VkSampler GetSampler() const
    {
      return m_sampler.Get();
    }

    VkImage GetImage() const
    {
      return m_image.Get();
    }

    VkImageLayout GetImageLayout() const
    {
      return m_imageLayout;
    }

    VkDeviceMemory GetMemory() const
    {
      return m_deviceMemory.Get();
    }

    VkImageView GetImageView() const
    {
      return m_view.Get();
    }

    PxExtent3D GetExent() const
    {
      return m_extent;
    }

    uint32_t GetLevels() const
    {
      return m_mipLevels;
    }

    uint32_t GetLayers() const
    {
      return m_layerCount;
    }

    VkDescriptorImageInfo GetImageDescriptor() const
    {
      return m_descriptor;
    }

    const VkDescriptorImageInfo* GetImageDescriptorPointer() const
    {
      return &m_descriptor;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_sampler.IsValid();
    }
  };
}

#endif
