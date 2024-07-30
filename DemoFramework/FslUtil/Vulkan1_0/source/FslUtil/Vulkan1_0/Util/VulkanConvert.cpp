/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/VulkanConvert.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <cassert>

namespace
{
  namespace LocalConfig
  {
    // VK_FORMAT_BEGIN_RANGE has been removed from new vulkan headers, so default back to VK_FORMAT_UNDEFINED instead
    constexpr uint32_t FormatBeginRange = VK_FORMAT_UNDEFINED;
    constexpr uint32_t FormatEndRange = VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
    constexpr uint32_t FormatRangeSize = (FormatEndRange - FormatBeginRange) + 1;
  }
}

namespace Fsl
{
  namespace Vulkan::VulkanConvert
  {
    PixelFormat TryToPixelFormat(const VkFormat pixelFormat)
    {
      const auto formatId =
        static_cast<uint32_t>(pixelFormat - LocalConfig::FormatBeginRange) + static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE);
      return PixelFormatUtil::TryGetPixelFormatById(formatId);
    }


    PixelFormat ToPixelFormat(const VkFormat pixelFormat)
    {
      if (pixelFormat == VK_FORMAT_UNDEFINED)
      {
        return PixelFormat::Undefined;
      }

      const PixelFormat result = TryToPixelFormat(pixelFormat);
      if (result != PixelFormat::Undefined)
      {
        return result;
      }
      throw UnsupportedVulkanPixelFormatException(pixelFormat);
    }


    VkFormat TryToVkFormat(const PixelFormat pixelFormat)
    {
      const auto pixelFormatRangeIndex = PixelFormatUtil::GetFormatRangeIndex(pixelFormat) + static_cast<uint32_t>(LocalConfig::FormatBeginRange);
      static_assert(LocalConfig::FormatBeginRange == 0, "(LocalConfig::FormatBeginRange is expected to be zero");
      // if (pixelFormatRangeIndex >= static_cast<uint32_t>(LocalConfig::FormatBeginRange) &&  pixelFormatRangeIndex <=
      // static_cast<uint32_t>(VK_FORMAT_END_RANGE))
      if (pixelFormatRangeIndex <= static_cast<uint32_t>(LocalConfig::FormatEndRange))
      {
        return static_cast<VkFormat>(pixelFormatRangeIndex);
      }
      return VK_FORMAT_UNDEFINED;
    }


    VkFormat ToVkFormat(const PixelFormat pixelFormat)
    {
      if (pixelFormat == PixelFormat::Undefined)
      {
        return VK_FORMAT_UNDEFINED;
      }

      const VkFormat result = TryToVkFormat(pixelFormat);
      if (result != VK_FORMAT_UNDEFINED)
      {
        return result;
      }
      throw UnsupportedPixelFormatException(pixelFormat);
    }


    VkFormat TryToVkFormat(const VertexElementFormat vertexFormat)
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
      case VertexElementFormat::X8_UNORM:
        return VK_FORMAT_R8_UNORM;
      case VertexElementFormat::X8_UINT:
        return VK_FORMAT_R8_UINT;
      case VertexElementFormat::X8Y8_UNORM:
        return VK_FORMAT_R8G8_UNORM;
      case VertexElementFormat::X8Y8_UINT:
        return VK_FORMAT_R8G8_UINT;
      case VertexElementFormat::X8Y8Z8_UNORM:
        return VK_FORMAT_R8G8B8_UNORM;
      case VertexElementFormat::X8Y8Z8_UINT:
        return VK_FORMAT_R8G8B8_UINT;
      case VertexElementFormat::X8Y8Z8W8_UNORM:
        return VK_FORMAT_R8G8B8A8_UNORM;
      case VertexElementFormat::X8Y8Z8W8_UINT:
        return VK_FORMAT_R8G8B8A8_UINT;
      case VertexElementFormat::X16Y16Z16W16_UNORM:
        return VK_FORMAT_R16G16B16A16_UNORM;
      case VertexElementFormat::X16Y16Z16W16_UINT:
        return VK_FORMAT_R16G16B16A16_UINT;
      case VertexElementFormat::Matrix4x4:
      case VertexElementFormat::Undefined:
      default:
        return VK_FORMAT_UNDEFINED;
      }
    }


    VkFormat ToVkFormat(const VertexElementFormat vertexFormat)
    {
      if (vertexFormat == VertexElementFormat::Undefined)
      {
        return VK_FORMAT_UNDEFINED;
      }

      const VkFormat result = TryToVkFormat(vertexFormat);
      if (result != VK_FORMAT_UNDEFINED)
      {
        return result;
      }
      throw UnsupportedVertexElementFormatException(vertexFormat);
    }


    VkImageType ToVkImageType(const TextureType textureType)
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


    VkImageViewType ToVkImageViewType(const TextureType textureType)
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

  // if this fires we need to update our PixelFormats to match
  static_assert(static_cast<uint32_t>(LocalConfig::FormatRangeSize) == static_cast<uint32_t>(PixelFormat::ENUM_ID_RANGE_COMMON_SIZE),
                "The number of vulkan formats did not match our pixel formats");

  static_assert(PixelFormat::ENUM_ID_BEGIN_RANGE == PixelFormat::ENUM_ID_BEGIN_COMMON_RANGE, "We expect that the common is the start of the ranges");

  // Some cut and pasted verification checks just to be 100% on the safe side and its a compile time check so its fine
  // Some auto generated checks
  static_assert(((static_cast<uint32_t>(PixelFormat::Undefined) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(0 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R4G4_UNORM_PACK8) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(1 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R4G4B4A4_UNORM_PACK16) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(2 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B4G4R4A4_UNORM_PACK16) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(3 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R5G6B5_UNORM_PACK16) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(4 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B5G6R5_UNORM_PACK16) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(5 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R5G5B5A1_UNORM_PACK16) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(6 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B5G5R5A1_UNORM_PACK16) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(7 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A1R5G5B5_UNORM_PACK16) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(8 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(9 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(10 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(11 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(12 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(13 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(14 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8_SRGB) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(15 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(16 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(17 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(18 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(19 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(20 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(21 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8_SRGB) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(22 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(23 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(24 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(25 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(26 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(27 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(28 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8_SRGB) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(29 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(30 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(31 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(32 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(33 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(34 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(35 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8_SRGB) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(36 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8A8_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(37 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8A8_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(38 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8A8_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(39 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8A8_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(40 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8A8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(41 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8A8_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(42 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R8G8B8A8_SRGB) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(43 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8A8_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(44 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8A8_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(45 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8A8_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(46 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8A8_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(47 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8A8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(48 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8A8_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(49 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B8G8R8A8_SRGB) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(50 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A8B8G8R8_UNORM_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(51 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A8B8G8R8_SNORM_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(52 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A8B8G8R8_USCALED_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(53 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A8B8G8R8_SSCALED_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(54 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A8B8G8R8_UINT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(55 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A8B8G8R8_SINT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(56 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A8B8G8R8_SRGB_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(57 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2R10G10B10_UNORM_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(58 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2R10G10B10_SNORM_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(59 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2R10G10B10_USCALED_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(60 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2R10G10B10_SSCALED_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(61 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2R10G10B10_UINT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(62 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2R10G10B10_SINT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(63 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2B10G10R10_UNORM_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(64 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2B10G10R10_SNORM_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(65 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2B10G10R10_USCALED_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(66 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2B10G10R10_SSCALED_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(67 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2B10G10R10_UINT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(68 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::A2B10G10R10_SINT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(69 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(70 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(71 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(72 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(73 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(74 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(75 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(76 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(77 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(78 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(79 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(80 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(81 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(82 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(83 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(84 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(85 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(86 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(87 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(88 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(89 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(90 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16A16_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(91 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16A16_SNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(92 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16A16_USCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(93 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16A16_SSCALED) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(94 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16A16_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(95 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16A16_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(96 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R16G16B16A16_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(97 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(98 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(99 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(100 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(101 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(102 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(103 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32B32_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(104 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32B32_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(105 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32B32_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(106 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32B32A32_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(107 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32B32A32_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(108 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R32G32B32A32_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(109 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(110 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(111 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(112 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(113 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(114 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(115 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64B64_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(116 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64B64_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(117 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64B64_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(118 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64B64A64_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(119 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64B64A64_SINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(120 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::R64G64B64A64_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(121 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::B10G11R11_UFLOAT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(122 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::E5B9G9R9_UFLOAT_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(123 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::D16_UNORM) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(124 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::X8_D24_UNORM_PACK32) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(125 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::D32_SFLOAT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(126 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::S8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(127 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::D16_UNORM_S8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(128 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::D24_UNORM_S8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(129 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::D32_SFLOAT_S8_UINT) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(130 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC1_RGB_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(131 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC1_RGB_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(132 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC1_RGBA_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(133 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC1_RGBA_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(134 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC2_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(135 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC2_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(136 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC3_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(137 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC3_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(138 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC4_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(139 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC4_SNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(140 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC5_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(141 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC5_SNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(142 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC6H_UFLOAT_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(143 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC6H_SFLOAT_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(144 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC7_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(145 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::BC7_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(146 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ETC2_R8G8B8_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(147 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ETC2_R8G8B8_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(148 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ETC2_R8G8B8A1_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(149 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ETC2_R8G8B8A1_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(150 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ETC2_R8G8B8A8_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(151 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ETC2_R8G8B8A8_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(152 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::EAC_R11_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(153 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::EAC_R11_SNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(154 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::EAC_R11G11_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(155 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::EAC_R11G11_SNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(156 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_4x4_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(157 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_4x4_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(158 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_5x4_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(159 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_5x4_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(160 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_5x5_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(161 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_5x5_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(162 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_6x5_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(163 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_6x5_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(164 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_6x6_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(165 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_6x6_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(166 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_8x5_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(167 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_8x5_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(168 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_8x6_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(169 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_8x6_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(170 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_8x8_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(171 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_8x8_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(172 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x5_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(173 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x5_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(174 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x6_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(175 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x6_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(176 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x8_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(177 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x8_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(178 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x10_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(179 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_10x10_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(180 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_12x10_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(181 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_12x10_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(182 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_12x12_UNORM_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(183 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
  static_assert(((static_cast<uint32_t>(PixelFormat::ASTC_12x12_SRGB_BLOCK) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_FORMAT_ID)) -
                 static_cast<uint32_t>(PixelFormat::ENUM_ID_BEGIN_RANGE)) == static_cast<uint32_t>(184 - LocalConfig::FormatBeginRange),
                "The formatId did not match the Vulkan format");
}
