#ifndef FSLUTIL_VULKAN1_0_UTIL_VULKANCONVERT_HPP
#define FSLUTIL_VULKAN1_0_UTIL_VULKANCONVERT_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxExtent3D.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/Vertices/VertexElementFormat.hpp>
#include <FslGraphics/Texture/TextureType.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxExtent3D.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  namespace Vulkan
  {
    namespace VulkanConvert
    {
      //! @brief Try to convert the Vulkan VKFormat to a PixelFormat enum
      //! @returns the pixel format or PixelFormat::Undefined if not successful
      PixelFormat TryToPixelFormat(const VkFormat pixelFormat);

      //! @brief to convert the Vulkan VKFormat to a PixelFormat enum
      //! @note Throws a UnsupportedVulkanPixelFormatException if the conversion fails
      PixelFormat ToPixelFormat(const VkFormat pixelFormat);

      //! @brief Convert the PixelFormat to a Vulkan VKFormat
      //! @returns the pixel format or VK_FORMAT_UNDEFINED if not successful
      VkFormat TryToVkFormat(const PixelFormat pixelFormat);

      //! @brief Convert the PixelFormat to a Vulkan VKFormat
      //! @note Throws a UnsupportedVulkanPixelFormatException if the conversion fails
      VkFormat ToVkFormat(const PixelFormat pixelFormat);

      //! @brief Convert the VertexElementFormat to a Vulkan VKFormat
      //! @returns the pixel format or VK_FORMAT_UNDEFINED if not successful
      VkFormat TryToVkFormat(const VertexElementFormat vertexFormat);

      //! @brief Convert the VertexElementFormat to a Vulkan VKFormat
      //! @note Throws a UnsupportedVulkanPixelFormatException if the conversion fails
      VkFormat ToVkFormat(const VertexElementFormat vertexFormat);

      //! @brief Convert the textureType to a VkImageType
      VkImageType ToVkImageType(const TextureType textureType);

      //! @brief Convert the textureType to a VkImageViewType
      VkImageViewType ToVkImageViewType(const TextureType textureType);
    };
  }
}

#endif
