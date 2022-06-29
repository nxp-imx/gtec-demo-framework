#ifndef FSLUTIL_VULKAN1_0_VUIMAGEMEMORYVIEW_HPP
#define FSLUTIL_VULKAN1_0_VUIMAGEMEMORYVIEW_HPP
/****************************************************************************************************************************************************
 * Copyright 2017, 2022 NXP
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

#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslUtil/Vulkan1_0/VUImage.hpp>
#include <RapidVulkan/ImageView.hpp>
#include <RapidVulkan/Memory.hpp>
#include <string>

namespace Fsl::Vulkan
{
  class VUDevice;

  //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
  class VUImageMemoryView
  {
    VUImage m_image;
    RapidVulkan::Memory m_memory;
    RapidVulkan::ImageView m_imageView;

  public:
    VUImageMemoryView(const VUImageMemoryView&) = delete;
    VUImageMemoryView& operator=(const VUImageMemoryView&) = delete;

    //! @brief Move assignment operator
    VUImageMemoryView& operator=(VUImageMemoryView&& other) noexcept;

    //! @brief Move constructor
    //! Transfer ownership from other to this
    VUImageMemoryView(VUImageMemoryView&& other) noexcept;

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    VUImageMemoryView();

    VUImageMemoryView(const VUDevice& device, const VkImageCreateInfo& imageCreateInfo, const VkImageSubresourceRange& subresourceRange,
                      const VkMemoryPropertyFlags memoryPropertyFlags, const std::string& name);

    VUImageMemoryView(VUImage&& image, RapidVulkan::Memory&& memory, RapidVulkan::ImageView&& imageView, const std::string& name);

    ~VUImageMemoryView() noexcept
    {
      Reset();
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset() noexcept;

    void Reset(const VUDevice& device, const VkImageCreateInfo& imageCreateInfo, const VkImageSubresourceRange& subresourceRange,
               const VkMemoryPropertyFlags memoryPropertyFlags, const std::string& name);
    void Reset(VUImage&& image, RapidVulkan::Memory&& memory, RapidVulkan::ImageView&& imageView, const std::string& name);


    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_image.IsValid();
    }

    VkDevice GetDevice() const
    {
      return m_image.GetDevice();
    }

    //! @return the format or VK_FORMAT_UNDEFINED if the object is invalid
    VkFormat GetFormat() const
    {
      return m_image.GetFormat();
    }

    //! @return the image (or VK_NULL_HANDLE if the object is invalid)
    VkImage GetImage() const
    {
      return m_image.Get();
    }

    //! @return the imageview (or VK_NULL_HANDLE if the object is invalid)
    VkImageView GetImageView() const
    {
      return m_imageView.Get();
    }

    //! @brief
    //! @return the extent (or VkExtent3D{} if the object is invalid)
    VkExtent3D GetExtent() const
    {
      return m_image.GetExtent();
    }

    //! @return the extent (or VkExtent2D{} if the object is invalid)
    VkExtent2D GetExtent2D() const
    {
      return IsValid() ? VkExtent2D{m_image.GetExtent().width, m_image.GetExtent().height} : VkExtent2D{};
    }

    //! @brief Get the Image associated with this object
    const VUImage& Image() const
    {
      return m_image;
    }

    //! @brief Get the Image associated with this object
    VUImage& Image()
    {
      return m_image;
    }


    //! @brief Get the ImageView associated with this object
    const RapidVulkan::ImageView& ImageView() const
    {
      return m_imageView;
    }

    //! @brief Get the Memory associated with this object
    const RapidVulkan::Memory& Memory() const
    {
      return m_memory;
    }

    //! @brief Beware this does not actually do anything to the image, it just sets the stored layout variable
    void SetImageLayout(const VkImageLayout newLayout)
    {
      m_image.SetImageLayout(newLayout);
    }

    VkDescriptorImageInfo GetDescriptorImageInfo() const
    {
      VkDescriptorImageInfo descriptorImageInfo{};
      descriptorImageInfo.imageView = m_imageView.Get();
      descriptorImageInfo.imageLayout = m_image.GetImageLayout();
      return descriptorImageInfo;
    }

  private:
    inline void DoReset() noexcept;
  };
}

#endif
