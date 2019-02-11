#ifndef FSLUTIL_VULKAN1_0_VUTEXTURE_HPP
#define FSLUTIL_VULKAN1_0_VUTEXTURE_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslUtil/Vulkan1_0/VUImageMemoryView.hpp>
#include <FslUtil/Vulkan1_0/VUTextureInfo.hpp>
#include <RapidVulkan/Sampler.hpp>

namespace Fsl
{
  namespace Vulkan
  {
    //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
    class VUTexture
    {
      VUImageMemoryView m_image;
      RapidVulkan::Sampler m_sampler;

    public:
      VUTexture(const VUTexture&) = delete;
      VUTexture& operator=(const VUTexture&) = delete;

      //! @brief Move assignment operator
      VUTexture& operator=(VUTexture&& other) noexcept;

      //! @brief Move constructor
      //! Transfer ownership from other to this
      VUTexture(VUTexture&& other) noexcept;

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      VUTexture();
      VUTexture(VUImageMemoryView&& image, const VkSamplerCreateInfo& createInfo);
      VUTexture(VUImageMemoryView&& image, RapidVulkan::Sampler&& sampler);

      ~VUTexture()
      {
        Reset();
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset() noexcept;
      void Reset(VUImageMemoryView&& image, const VkSamplerCreateInfo& createInfo);
      void Reset(VUImageMemoryView&& image, RapidVulkan::Sampler&& sampler);

      //! @brief Check if this object contains a valid resource
      inline bool IsValid() const
      {
        return m_image.IsValid();
      }

      VkDevice GetDevice() const
      {
        return m_sampler.GetDevice();
      }

      //! @brief Get the Image associated with this object
      const VUImage& Image() const
      {
        return m_image.Image();
      }


      //! @brief Get the ImageView associated with this object
      const RapidVulkan::ImageView& ImageView() const
      {
        return m_image.ImageView();
      }

      //! @brief Get the Memory associated with this object
      const RapidVulkan::Memory& Memory() const
      {
        return m_image.Memory();
      }

      //! @brief Get the Memory associated with this object
      const RapidVulkan::Sampler& Sampler() const
      {
        return m_sampler;
      }

      //! @brief Extract information about this texture as a GLTextureInfo struct
      operator VUTextureInfo() const
      {
        return VUTextureInfo(m_sampler.Get(), m_image.ImageView().Get(), m_image.Image().GetImageLayout(), m_image.Image().GetExtent());
      }

      VkExtent3D GetExtent() const
      {
        return m_image.Image().GetExtent();
      }


      VkDescriptorImageInfo GetDescriptorImageInfo() const
      {
        VkDescriptorImageInfo descriptorImageInfo{};
        descriptorImageInfo.sampler = m_sampler.Get();
        descriptorImageInfo.imageView = m_image.ImageView().Get();
        descriptorImageInfo.imageLayout = m_image.Image().GetImageLayout();
        return descriptorImageInfo;
      }

      //! @brief Beware this does not actually do anything to the image, it just sets the stored layout variable
      void SetImageLayout(const VkImageLayout newLayout)
      {
        m_image.SetImageLayout(newLayout);
      }

    private:
      inline void DoReset();
    };
  }
}

#endif
