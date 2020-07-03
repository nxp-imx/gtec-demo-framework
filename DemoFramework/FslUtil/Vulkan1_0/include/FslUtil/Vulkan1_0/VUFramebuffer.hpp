#ifndef FSLUTIL_VULKAN1_0_VUFRAMEBUFFER_HPP
#define FSLUTIL_VULKAN1_0_VUFRAMEBUFFER_HPP
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

#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <RapidVulkan/Framebuffer.hpp>

namespace Fsl
{
  namespace Vulkan
  {
    //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
    class VUFramebuffer
    {
      Vulkan::VUTexture m_texture;
      RapidVulkan::Framebuffer m_framebuffer;

    public:
      VUFramebuffer(const VUFramebuffer&) = delete;
      VUFramebuffer& operator=(const VUFramebuffer&) = delete;

      //! @brief Move assignment operator
      VUFramebuffer& operator=(VUFramebuffer&& other) noexcept;

      //! @brief Move constructor
      //! Transfer ownership from other to this
      VUFramebuffer(VUFramebuffer&& other) noexcept;

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      VUFramebuffer();
      VUFramebuffer(VUTexture&& texture, RapidVulkan::Framebuffer&& framebuffer);
      VUFramebuffer(const VUDevice& device, const VkExtent2D extent, const VkFormat format, const VkRenderPass renderPass, const std::string& name);
      VUFramebuffer(const VUDevice& device, const VkExtent2D extent, const VkFormat format, const VkRenderPass renderPass,
                    const VkImageView depthImageView, const std::string& name);


      ~VUFramebuffer() noexcept
      {
        Reset();
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset() noexcept;
      void Reset(VUTexture&& texture, RapidVulkan::Framebuffer&& r);
      void Reset(const VUDevice& device, const VkExtent2D extent, const VkFormat format, const VkRenderPass renderPass, const std::string& name);
      void Reset(const VUDevice& device, const VkExtent2D extent, const VkFormat format, const VkRenderPass renderPass,
                 const VkImageView depthImageView, const std::string& name);

      //! @brief Check if this object contains a valid resource
      inline bool IsValid() const
      {
        return m_texture.IsValid();
      }

      VkDevice GetDevice() const
      {
        return m_texture.GetDevice();
      }

      VkImage GetImage() const
      {
        return m_texture.GetImage();
      }

      VkImageView GetImageView() const
      {
        return m_texture.GetImageView();
      }

      VkFramebuffer GetFramebuffer() const
      {
        return m_framebuffer.Get();
      }

      VkExtent2D GetExtent2D() const
      {
        return m_texture.GetExtent2D();
      }

      //! @brief Get the texture associated with this object
      const VUTexture& Texture() const
      {
        return m_texture;
      }


      //! @brief Get the ImageView associated with this object
      const RapidVulkan::Framebuffer& Framebuffer() const
      {
        return m_framebuffer;
      }

      //! @brief Extract information about this texture as a VUTextureInfo struct
      operator VUTextureInfo() const    // NOLINT(google-explicit-constructor)
      {
        return m_texture;
      }

    private:
      inline void DoReset() noexcept;
    };
  }
}

#endif
