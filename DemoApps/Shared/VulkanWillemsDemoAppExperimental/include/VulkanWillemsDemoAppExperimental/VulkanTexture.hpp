#ifndef VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANTEXTURE_HPP
#define VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANTEXTURE_HPP
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

#include <FslGraphicsVulkan1_0/Common.hpp>
#include <FslGraphicsVulkan1_0/Memory.hpp>
#include <FslGraphicsVulkan1_0/Image.hpp>
#include <FslGraphicsVulkan1_0/ImageView.hpp>
#include <FslGraphicsVulkan1_0/Sampler.hpp>
#include <FslGraphicsVulkan1_0/Util.hpp>
#include <FslBase/Math/Extent3D.hpp>
#include <vulkan/vulkan.h>
#include <cassert>

namespace Fsl
{
  namespace Willems
  {
    //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
    class VulkanTexture
    {
      Vulkan::Sampler m_sampler;
      Vulkan::Image m_image;
      VkImageLayout m_imageLayout;
      Vulkan::Memory m_deviceMemory;
      Vulkan::ImageView m_view;
      Extent3D m_extent;
      uint32_t m_mipLevels;
      uint32_t m_layerCount;
      VkDescriptorImageInfo m_descriptor;
    public:
      VulkanTexture(const VulkanTexture&) = delete;
      VulkanTexture& operator=(const VulkanTexture&) = delete;

      //! @brief Move assignment operator
      VulkanTexture& operator=(VulkanTexture&& other);

      //! @brief Move constructor
      //! Transfer ownership from other to this
      VulkanTexture(VulkanTexture&& other);

      //! @brief Move objects into this object
      VulkanTexture(Vulkan::Sampler&& sampler, Vulkan::Image&& image, const VkImageLayout& imageLayout,
                    Vulkan::Memory&& deviceMemory, Vulkan::ImageView&& view, const Extent3D& extent,
                    const uint32_t mipLevels, const uint32_t layerCount, const VkDescriptorImageInfo& descriptor);

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      VulkanTexture();

      ~VulkanTexture()
      {
        Reset();
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset();

      //! @brief Move objects into this object
      void Reset(Vulkan::Sampler&& sampler, Vulkan::Image&& image, const VkImageLayout& imageLayout,
                 Vulkan::Memory&& deviceMemory, Vulkan::ImageView&& view, const Extent3D& extent,
                 const uint32_t mipLevels, const uint32_t layerCount, const VkDescriptorImageInfo& descriptor);

      //! @brief Change the associated sampler (destroys the old one)
      void SetSampler(Vulkan::Sampler&& sampler);


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

      Extent3D GetExent() const
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
}

#endif
