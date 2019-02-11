#ifndef FSLUTIL_VULKAN1_0_VUTEXTUREINFO_HPP
#define FSLUTIL_VULKAN1_0_VUTEXTUREINFO_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Extent2D.hpp>
#include <FslBase/Math/Extent3D.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  namespace Vulkan
  {
    //! @brief A simple container for texture information.
    //! @note This is not a RAII object so you are responsible for managing the lifetime of the texture object.
    struct VUTextureInfo
    {
      VkDescriptorImageInfo ImageInfo;
      Extent3D Extent;

      //! @brief Create a uninitialized texture (use SetData to add texture data to it)
      VUTextureInfo()
        : ImageInfo{}
      {
      }

      //! @brief Supply the object with information about a texture
      //! @param handle the GL handle to the texture (it's assumed the handle is a GL_TEXTURE_2D or GL_TEXTURE_3D)
      //! @param size the size of the texture
      VUTextureInfo(const VkDescriptorImageInfo& imageInfo, const Point2& size)
        : VUTextureInfo(imageInfo, Extent3D(size.X, size.Y, 1))
      {
      }

      //! @brief Supply the object with information about a texture
      //! @param handle the GL handle to the texture (it's assumed the handle is a GL_TEXTURE_2D or GL_TEXTURE_3D)
      //! @param size the size of the texture
      VUTextureInfo(const VkDescriptorImageInfo& imageInfo, const Extent2D& extent)
        : VUTextureInfo(imageInfo, Extent3D(extent.Width, extent.Height, 1u))
      {
      }

      //! @brief Supply the object with information about a texture
      //! @param handle the GL handle to the texture (it's assumed the handle is a GL_TEXTURE_2D or GL_TEXTURE_3D)
      //! @param size the size of the texture
      VUTextureInfo(const VkDescriptorImageInfo& imageInfo, const Extent3D& extent)
        : ImageInfo(imageInfo)
        , Extent(extent)
      {
      }

      //! @brief Supply the object with information about a texture
      //! @param handle the GL handle to the texture (it's assumed the handle is a GL_TEXTURE_2D or GL_TEXTURE_3D)
      //! @param size the size of the texture
      VUTextureInfo(const VkDescriptorImageInfo& imageInfo, const VkExtent2D& extent)
        : VUTextureInfo(imageInfo, Extent3D(extent.width, extent.height, 1u))
      {
      }

      //! @brief Supply the object with information about a texture
      //! @param handle the GL handle to the texture (it's assumed the handle is a GL_TEXTURE_2D or GL_TEXTURE_3D)
      //! @param size the size of the texture
      VUTextureInfo(const VkDescriptorImageInfo& imageInfo, const VkExtent3D& extent)
        : ImageInfo(imageInfo)
        , Extent(extent.width, extent.height, extent.depth)
      {
      }

      //! @brief Supply the object with information about a texture
      //! @param handle the GL handle to the texture (it's assumed the handle is a GL_TEXTURE_2D or GL_TEXTURE_3D)
      //! @param size the size of the texture
      VUTextureInfo(const VkSampler sampler, const VkImageView imageView, const VkImageLayout imageLayout, const VkExtent3D& extent)
        : ImageInfo{sampler, imageView, imageLayout}
        , Extent(extent.width, extent.height, extent.depth)
      {
      }


      void Reset()
      {
        ImageInfo = {};
        Extent = {};
      }


      bool operator==(const VUTextureInfo& rhs) const
      {
        return ImageInfo.sampler == rhs.ImageInfo.sampler && ImageInfo.imageView == rhs.ImageInfo.imageView &&
               ImageInfo.imageLayout == rhs.ImageInfo.imageLayout && Extent == rhs.Extent;
      }

      bool operator!=(const VUTextureInfo& rhs) const
      {
        return !(*this == rhs);
      }
    };
  }
}

#endif
