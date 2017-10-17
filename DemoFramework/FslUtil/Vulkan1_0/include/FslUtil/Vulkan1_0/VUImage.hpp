#ifndef FSLUTIL_VULKAN1_0_VUIMAGE_HPP
#define FSLUTIL_VULKAN1_0_VUIMAGE_HPP
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

#include <FslUtil/Vulkan1_0/Common.hpp>
//#include <FslUtil/Vulkan1_0/Extend/DeviceMemoryEx.hpp>
#include <FslUtil/Vulkan1_0/Extend/ImageEx.hpp>
#include <RapidVulkan/CommandBuffer.hpp>
#include <RapidVulkan/ImageView.hpp>
#include <RapidVulkan/Memory.hpp>
#include <string>

namespace Fsl
{
  namespace Vulkan
  {
    class VUDevice;

    //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
    class VUImage
    {
      ImageEx m_image;
      RapidVulkan::Memory m_memory;
      RapidVulkan::ImageView m_imageView;
    public:
      VUImage(const VUImage&) = delete;
      VUImage& operator=(const VUImage&) = delete;

      //! @brief Move assignment operator
      VUImage& operator=(VUImage&& other);

      //! @brief Move constructor
      //! Transfer ownership from other to this
      VUImage(VUImage&& other);

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      VUImage();

      VUImage(const VUDevice& device, const VkCommandBuffer commandBuffer, const VkImageCreateInfo& imageCreateInfo, const VkAccessFlags srcAccessMask,
        const VkAccessFlags dstAccessMask, const VkImageLayout newLayout, const VkImageSubresourceRange& subresourceRange,
        const VkMemoryPropertyFlags memoryPropertyFlags, const std::string& name);

      VUImage(ImageEx&& image, RapidVulkan::Memory&& memory, RapidVulkan::ImageView&& imageView, const std::string& name);

      ~VUImage()
      {
        Reset();
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset();

      void Reset(const VUDevice& device, const VkCommandBuffer commandBuffer, const VkImageCreateInfo& imageCreateInfo, const VkAccessFlags srcAccessMask,
        const VkAccessFlags dstAccessMask, const VkImageLayout newLayout, const VkImageSubresourceRange& subresourceRange,
        const VkMemoryPropertyFlags memoryPropertyFlags, const std::string& name);
      void Reset(ImageEx&& image, RapidVulkan::Memory&& memory, RapidVulkan::ImageView&& imageView, const std::string& name);


      //! @brief Check if this object contains a valid resource
      inline bool IsValid() const
      {
        return m_image.IsValid();
      }

      VkDevice GetDevice() const
      {
        return m_image.GetDevice();
      }

      //! @brief Get the Image associated with this object
      const ImageEx& Image() const
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

    private:
      inline void DoReset();
    };
  }
}

#endif
