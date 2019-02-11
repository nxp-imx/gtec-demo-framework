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

#include <FslBase/Log/Log.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <cassert>

namespace Fsl
{
  namespace Vulkan
  {
    namespace ConvertUtil
    {
      PixelFormat TryConvert(const VkFormat pixelFormat)
      {
        const auto formatId = static_cast<uint32_t>(pixelFormat - VK_FORMAT_BEGIN_RANGE) + static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE);
        return PixelFormatUtil::TryGetPixelFormatById(formatId);
      }


      PixelFormat Convert(const VkFormat pixelFormat)
      {
        if (pixelFormat == VK_FORMAT_UNDEFINED)
        {
          return PixelFormat::Undefined;
        }

        const PixelFormat result = TryConvert(pixelFormat);
        if (result != PixelFormat::Undefined)
        {
          return result;
        }
        throw UnsupportedVulkanPixelFormatException(pixelFormat);
      }


      VkFormat TryConvert(const PixelFormat pixelFormat)
      {
        const auto pixelFormatRangeIndex = PixelFormatUtil::GetFormatRangeIndex(pixelFormat) + static_cast<uint32_t>(VK_FORMAT_BEGIN_RANGE);
        static_assert(VK_FORMAT_BEGIN_RANGE == 0, "(VK_FORMAT_BEGIN_RANGE is expected to be zero");
        // if (pixelFormatRangeIndex >= static_cast<uint32_t>(VK_FORMAT_BEGIN_RANGE) &&  pixelFormatRangeIndex <=
        // static_cast<uint32_t>(VK_FORMAT_END_RANGE))
        if (pixelFormatRangeIndex <= static_cast<uint32_t>(VK_FORMAT_END_RANGE))
        {
          return static_cast<VkFormat>(pixelFormatRangeIndex);
        }
        return VK_FORMAT_UNDEFINED;
      }


      VkFormat Convert(const PixelFormat pixelFormat)
      {
        if (pixelFormat == PixelFormat::Undefined)
        {
          return VK_FORMAT_UNDEFINED;
        }

        const VkFormat result = TryConvert(pixelFormat);
        if (result != VK_FORMAT_UNDEFINED)
        {
          return result;
        }
        throw UnsupportedPixelFormatException(pixelFormat);
      }


      // FIX: implement all conversions
      VkFormat TryConvert(const VertexElementFormat vertexFormat)
      {
        switch (vertexFormat)
        {
        case VertexElementFormat::Single:
          return VK_FORMAT_R32_SFLOAT;
        case VertexElementFormat::Vector2:
          return VK_FORMAT_R32G32_SFLOAT;
        case VertexElementFormat::Vector3:
          return VK_FORMAT_R32G32B32_SFLOAT;
        case VertexElementFormat::Vector4:
          return VK_FORMAT_R32G32B32A32_SFLOAT;
        case VertexElementFormat::Matrix4x4:
        case VertexElementFormat::X8_UNORM:
        case VertexElementFormat::X8_UINT:
        case VertexElementFormat::X8Y8_UNORM:
        case VertexElementFormat::X8Y8_UINT:
        case VertexElementFormat::X8Y8Z8_UNORM:
        case VertexElementFormat::X8Y8Z8_UINT:
        case VertexElementFormat::X8Y8Z8W8_UNORM:
        case VertexElementFormat::X8Y8Z8W8_UINT:
        case VertexElementFormat::Undefined:
        default:
          return VK_FORMAT_UNDEFINED;
        }
      }


      VkFormat Convert(const VertexElementFormat vertexFormat)
      {
        if (vertexFormat == VertexElementFormat::Undefined)
        {
          return VK_FORMAT_UNDEFINED;
        }

        const VkFormat result = TryConvert(vertexFormat);
        if (result != VK_FORMAT_UNDEFINED)
        {
          return result;
        }
        throw UnsupportedVertexElementFormatException(vertexFormat);
      }


      VkImageType ToImageType(const TextureType textureType)
      {
        switch (textureType)
        {
        case TextureType::Tex1D:
        case TextureType::Tex1DArray:
          return VK_IMAGE_TYPE_1D;
        case TextureType::Tex2D:
        case TextureType::Tex2DArray:
        case TextureType::TexCube:
        case TextureType::TexCubeArray:
          return VK_IMAGE_TYPE_2D;
        case TextureType::Tex3D:
          return VK_IMAGE_TYPE_3D;
        default:
          throw NotSupportedException("Unsupported TextureType");
        }
      }


      VkImageViewType ToImageViewType(const TextureType textureType)
      {
        switch (textureType)
        {
        case TextureType::Tex1D:
          return VK_IMAGE_VIEW_TYPE_1D;
        case TextureType::Tex2D:
          return VK_IMAGE_VIEW_TYPE_2D;
        case TextureType::Tex3D:
          return VK_IMAGE_VIEW_TYPE_3D;
        case TextureType::TexCube:
          return VK_IMAGE_VIEW_TYPE_CUBE;
        case TextureType::Tex1DArray:
          return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
        case TextureType::Tex2DArray:
          return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        case TextureType::TexCubeArray:
          return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
        default:
          throw NotSupportedException("Unsupported TextureType");
        }
      }
    }
  }

  // if this fires we need to update our PixelFormats to match
  static_assert(static_cast<uint32_t>(VK_FORMAT_RANGE_SIZE) == static_cast<uint32_t>(PixelFormat::ENUM_ID_RANGE_COMMON_SIZE),
                "The number of vulkan formats did not match our pixel formats");

  static_assert(PixelFormat::ENUM_ID_BEGIN_RANGE == PixelFormat::ENUM_ID_BEGIN_COMMON_RANGE, "We expect that the common is the start of the ranges");

  // Some cut and pasted verification checks just to be 100% on the safe side and its a compile time check so its fine
  // Some auto generated checks
  static_assert(((static_cast<uint32_t>(PixelFormat::Undefined) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(0 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R4G4_UNORM_PACK8) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(1 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R4G4B4A4_UNORM_PACK16) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(2 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B4G4R4A4_UNORM_PACK16) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(3 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R5G6B5_UNORM_PACK16) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(4 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B5G6R5_UNORM_PACK16) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(5 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R5G5B5A1_UNORM_PACK16) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(6 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B5G5R5A1_UNORM_PACK16) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(7 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A1R5G5B5_UNORM_PACK16) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(8 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(9 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(10 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(11 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(12 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(13 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(14 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8_SRGB) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(15 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(16 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(17 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(18 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(19 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(20 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(21 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8_SRGB) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(22 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(23 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(24 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(25 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(26 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(27 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(28 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8_SRGB) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(29 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(30 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(31 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(32 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(33 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(34 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(35 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8_SRGB) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(36 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8A8_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(37 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8A8_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(38 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8A8_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(39 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8A8_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(40 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8A8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(41 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8A8_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(42 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8A8_SRGB) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(43 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8A8_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(44 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8A8_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(45 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8A8_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(46 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8A8_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(47 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8A8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(48 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8A8_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(49 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8A8_SRGB) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(50 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A8B8G8R8_UNORM_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(51 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A8B8G8R8_SNORM_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(52 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A8B8G8R8_USCALED_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(53 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A8B8G8R8_SSCALED_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(54 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A8B8G8R8_UINT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(55 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A8B8G8R8_SINT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(56 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A8B8G8R8_SRGB_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(57 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2R10G10B10_UNORM_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(58 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2R10G10B10_SNORM_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(59 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2R10G10B10_USCALED_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(60 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2R10G10B10_SSCALED_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(61 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2R10G10B10_UINT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(62 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2R10G10B10_SINT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(63 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2B10G10R10_UNORM_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(64 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2B10G10R10_SNORM_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(65 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2B10G10R10_USCALED_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(66 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2B10G10R10_SSCALED_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(67 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2B10G10R10_UINT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(68 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2B10G10R10_SINT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(69 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(70 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(71 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(72 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(73 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(74 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(75 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(76 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(77 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(78 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(79 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(80 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(81 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(82 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(83 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(84 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(85 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(86 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(87 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(88 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(89 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(90 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16A16_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(91 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16A16_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(92 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16A16_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(93 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16A16_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(94 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16A16_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(95 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16A16_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(96 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16A16_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(97 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(98 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(99 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(100 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(101 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(102 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(103 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32B32_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(104 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32B32_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(105 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32B32_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(106 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32B32A32_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(107 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32B32A32_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(108 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32B32A32_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(109 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(110 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(111 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(112 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(113 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(114 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(115 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64B64_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(116 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64B64_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(117 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64B64_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(118 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64B64A64_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(119 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64B64A64_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(120 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64B64A64_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(121 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B10G11R11_UFLOAT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(122 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::E5B9G9R9_UFLOAT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(123 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::D16_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(124 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::X8_D24_UNORM_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(125 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::D32_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(126 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::S8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(127 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::D16_UNORM_S8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(128 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::D24_UNORM_S8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(129 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::D32_SFLOAT_S8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(130 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC1_RGB_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(131 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC1_RGB_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(132 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC1_RGBA_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(133 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC1_RGBA_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(134 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC2_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(135 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC2_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(136 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC3_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(137 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC3_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(138 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC4_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(139 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC4_SNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(140 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC5_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(141 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC5_SNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(142 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC6H_UFLOAT_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(143 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC6H_SFLOAT_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(144 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC7_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(145 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC7_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(146 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ETC2_R8G8B8_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(147 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ETC2_R8G8B8_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(148 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ETC2_R8G8B8A1_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(149 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ETC2_R8G8B8A1_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(150 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ETC2_R8G8B8A8_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(151 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ETC2_R8G8B8A8_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(152 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::EAC_R11_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(153 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::EAC_R11_SNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(154 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::EAC_R11G11_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(155 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::EAC_R11G11_SNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(156 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_4x4_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(157 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_4x4_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(158 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_5x4_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(159 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_5x4_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(160 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_5x5_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(161 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_5x5_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(162 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_6x5_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(163 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_6x5_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(164 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_6x6_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(165 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_6x6_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(166 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_8x5_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(167 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_8x5_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(168 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_8x6_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(169 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_8x6_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(170 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_8x8_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(171 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_8x8_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(172 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x5_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(173 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x5_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(174 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x6_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(175 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x6_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(176 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x8_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(177 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x8_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(178 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x10_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(179 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x10_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(180 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_12x10_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(181 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_12x10_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(182 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_12x12_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(183 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_12x12_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(184 - VK_FORMAT_BEGIN_RANGE),
                "The formatId did not match the Vulkan format");
}
