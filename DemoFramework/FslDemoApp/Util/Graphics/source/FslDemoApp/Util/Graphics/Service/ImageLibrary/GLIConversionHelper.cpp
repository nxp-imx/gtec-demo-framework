#ifdef FSL_FEATURE_GLI
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

#include "GLIConversionHelper.hpp"
#include <FslGraphics/Exceptions.hpp>

namespace Fsl
{
  gli::format GLIConversionHelper::TryConvert(const PixelFormat format)
  {
    switch (format)
    {
    case PixelFormat::R4G4_UNORM_PACK8:
      return gli::FORMAT_RG4_UNORM_PACK8;
    case PixelFormat::R4G4B4A4_UNORM_PACK16:
      return gli::FORMAT_RGBA4_UNORM_PACK16;
    case PixelFormat::B4G4R4A4_UNORM_PACK16:
      return gli::FORMAT_BGRA4_UNORM_PACK16;
    case PixelFormat::R5G6B5_UNORM_PACK16:
      return gli::FORMAT_R5G6B5_UNORM_PACK16;
    case PixelFormat::B5G6R5_UNORM_PACK16:
      return gli::FORMAT_B5G6R5_UNORM_PACK16;
    case PixelFormat::R5G5B5A1_UNORM_PACK16:
      return gli::FORMAT_RGB5A1_UNORM_PACK16;
    case PixelFormat::B5G5R5A1_UNORM_PACK16:
      return gli::FORMAT_BGR5A1_UNORM_PACK16;
    case PixelFormat::A1R5G5B5_UNORM_PACK16:
      return gli::FORMAT_A1RGB5_UNORM_PACK16;
    case PixelFormat::R8_UNORM:
      return gli::FORMAT_R8_UNORM_PACK8;
    case PixelFormat::R8_SNORM:
      return gli::FORMAT_R8_SNORM_PACK8;
    case PixelFormat::R8_USCALED:
      return gli::FORMAT_R8_USCALED_PACK8;
    case PixelFormat::R8_SSCALED:
      return gli::FORMAT_R8_SSCALED_PACK8;
    case PixelFormat::R8_UINT:
      return gli::FORMAT_R8_UINT_PACK8;
    case PixelFormat::R8_SINT:
      return gli::FORMAT_R8_SINT_PACK8;
    case PixelFormat::R8_SRGB:
      return gli::FORMAT_R8_SRGB_PACK8;
    case PixelFormat::R8G8_UNORM:
      return gli::FORMAT_RG8_UNORM_PACK8;
    case PixelFormat::R8G8_SNORM:
      return gli::FORMAT_RG8_SNORM_PACK8;
    case PixelFormat::R8G8_USCALED:
      return gli::FORMAT_RG8_USCALED_PACK8;
    case PixelFormat::R8G8_SSCALED:
      return gli::FORMAT_RG8_SSCALED_PACK8;
    case PixelFormat::R8G8_UINT:
      return gli::FORMAT_RG8_UINT_PACK8;
    case PixelFormat::R8G8_SINT:
      return gli::FORMAT_RG8_SINT_PACK8;
    case PixelFormat::R8G8_SRGB:
      return gli::FORMAT_RG8_SRGB_PACK8;
    case PixelFormat::R8G8B8_UNORM:
      return gli::FORMAT_RGB8_UNORM_PACK8;
    case PixelFormat::R8G8B8_SNORM:
      return gli::FORMAT_RGB8_SNORM_PACK8;
    case PixelFormat::R8G8B8_USCALED:
      return gli::FORMAT_RGB8_USCALED_PACK8;
    case PixelFormat::R8G8B8_SSCALED:
      return gli::FORMAT_RGB8_SSCALED_PACK8;
    case PixelFormat::R8G8B8_UINT:
      return gli::FORMAT_RGB8_UINT_PACK8;
    case PixelFormat::R8G8B8_SINT:
      return gli::FORMAT_RGB8_SINT_PACK8;
    case PixelFormat::R8G8B8_SRGB:
      return gli::FORMAT_RGB8_SRGB_PACK8;
    case PixelFormat::B8G8R8_UNORM:
      return gli::FORMAT_BGR8_UNORM_PACK8;
    case PixelFormat::B8G8R8_SNORM:
      return gli::FORMAT_BGR8_SNORM_PACK8;
    case PixelFormat::B8G8R8_USCALED:
      return gli::FORMAT_BGR8_USCALED_PACK8;
    case PixelFormat::B8G8R8_SSCALED:
      return gli::FORMAT_BGR8_SSCALED_PACK8;
    case PixelFormat::B8G8R8_UINT:
      return gli::FORMAT_BGR8_UINT_PACK8;
    case PixelFormat::B8G8R8_SINT:
      return gli::FORMAT_BGR8_SINT_PACK8;
    case PixelFormat::B8G8R8_SRGB:
      return gli::FORMAT_BGR8_SRGB_PACK8;
    case PixelFormat::R8G8B8A8_UNORM:
      return gli::FORMAT_RGBA8_UNORM_PACK8;
    case PixelFormat::R8G8B8A8_SNORM:
      return gli::FORMAT_RGBA8_SNORM_PACK8;
    case PixelFormat::R8G8B8A8_USCALED:
      return gli::FORMAT_RGBA8_USCALED_PACK8;
    case PixelFormat::R8G8B8A8_SSCALED:
      return gli::FORMAT_RGBA8_SSCALED_PACK8;
    case PixelFormat::R8G8B8A8_UINT:
      return gli::FORMAT_RGBA8_UINT_PACK8;
    case PixelFormat::R8G8B8A8_SINT:
      return gli::FORMAT_RGBA8_SINT_PACK8;
    case PixelFormat::R8G8B8A8_SRGB:
      return gli::FORMAT_RGBA8_SRGB_PACK8;
    case PixelFormat::B8G8R8A8_UNORM:
      return gli::FORMAT_BGRA8_UNORM_PACK8;
    case PixelFormat::B8G8R8A8_SNORM:
      return gli::FORMAT_BGRA8_SNORM_PACK8;
    case PixelFormat::B8G8R8A8_USCALED:
      return gli::FORMAT_BGRA8_USCALED_PACK8;
    case PixelFormat::B8G8R8A8_SSCALED:
      return gli::FORMAT_BGRA8_SSCALED_PACK8;
    case PixelFormat::B8G8R8A8_UINT:
      return gli::FORMAT_BGRA8_UINT_PACK8;
    case PixelFormat::B8G8R8A8_SINT:
      return gli::FORMAT_BGRA8_SINT_PACK8;
    case PixelFormat::B8G8R8A8_SRGB:
      return gli::FORMAT_BGRA8_SRGB_PACK8;
    // case PixelFormat::A8B8G8R8_UNORM_PACK32:
    // case PixelFormat::A8B8G8R8_SNORM_PACK32:
    // case PixelFormat::A8B8G8R8_USCALED_PACK32:
    // case PixelFormat::A8B8G8R8_SSCALED_PACK32:
    // case PixelFormat::A8B8G8R8_UINT_PACK32:
    // case PixelFormat::A8B8G8R8_SINT_PACK32:
    // case PixelFormat::A8B8G8R8_SRGB_PACK32:
    // case PixelFormat::A2R10G10B10_UNORM_PACK32:
    // case PixelFormat::A2R10G10B10_SNORM_PACK32:
    // case PixelFormat::A2R10G10B10_USCALED_PACK32:
    // case PixelFormat::A2R10G10B10_SSCALED_PACK32:
    // case PixelFormat::A2R10G10B10_UINT_PACK32:
    // case PixelFormat::A2R10G10B10_SINT_PACK32:
    // case PixelFormat::A2B10G10R10_UNORM_PACK32:
    // case PixelFormat::A2B10G10R10_SNORM_PACK32:
    // case PixelFormat::A2B10G10R10_USCALED_PACK32:
    // case PixelFormat::A2B10G10R10_SSCALED_PACK32:
    // case PixelFormat::A2B10G10R10_UINT_PACK32:
    // case PixelFormat::A2B10G10R10_SINT_PACK32:
    case PixelFormat::R16_UNORM:
      return gli::FORMAT_R16_UNORM_PACK16;
    case PixelFormat::R16_SNORM:
      return gli::FORMAT_R16_SNORM_PACK16;
    case PixelFormat::R16_USCALED:
      return gli::FORMAT_R16_USCALED_PACK16;
    case PixelFormat::R16_SSCALED:
      return gli::FORMAT_R16_SSCALED_PACK16;
    case PixelFormat::R16_UINT:
      return gli::FORMAT_R16_UINT_PACK16;
    case PixelFormat::R16_SINT:
      return gli::FORMAT_R16_SINT_PACK16;
    case PixelFormat::R16_SFLOAT:
      return gli::FORMAT_R16_SFLOAT_PACK16;
    case PixelFormat::R16G16_UNORM:
      return gli::FORMAT_RG16_UNORM_PACK16;
    case PixelFormat::R16G16_SNORM:
      return gli::FORMAT_RG16_SNORM_PACK16;
    case PixelFormat::R16G16_USCALED:
      return gli::FORMAT_RG16_USCALED_PACK16;
    case PixelFormat::R16G16_SSCALED:
      return gli::FORMAT_RG16_SSCALED_PACK16;
    case PixelFormat::R16G16_UINT:
      return gli::FORMAT_RG16_UINT_PACK16;
    case PixelFormat::R16G16_SINT:
      return gli::FORMAT_RG16_SINT_PACK16;
    case PixelFormat::R16G16_SFLOAT:
      return gli::FORMAT_RG16_SFLOAT_PACK16;
    case PixelFormat::R16G16B16_UNORM:
      return gli::FORMAT_RGB16_UNORM_PACK16;
    case PixelFormat::R16G16B16_SNORM:
      return gli::FORMAT_RGB16_SNORM_PACK16;
    case PixelFormat::R16G16B16_USCALED:
      return gli::FORMAT_RGB16_USCALED_PACK16;
    case PixelFormat::R16G16B16_SSCALED:
      return gli::FORMAT_RGB16_SSCALED_PACK16;
    case PixelFormat::R16G16B16_UINT:
      return gli::FORMAT_RGB16_UINT_PACK16;
    case PixelFormat::R16G16B16_SINT:
      return gli::FORMAT_RGB16_SINT_PACK16;
    case PixelFormat::R16G16B16_SFLOAT:
      return gli::FORMAT_RGB16_SFLOAT_PACK16;
    case PixelFormat::R16G16B16A16_UNORM:
      return gli::FORMAT_RGBA16_UNORM_PACK16;
    case PixelFormat::R16G16B16A16_SNORM:
      return gli::FORMAT_RGBA16_SNORM_PACK16;
    case PixelFormat::R16G16B16A16_USCALED:
      return gli::FORMAT_RGBA16_USCALED_PACK16;
    case PixelFormat::R16G16B16A16_SSCALED:
      return gli::FORMAT_RGBA16_SSCALED_PACK16;
    case PixelFormat::R16G16B16A16_UINT:
      return gli::FORMAT_RGBA16_UINT_PACK16;
    case PixelFormat::R16G16B16A16_SINT:
      return gli::FORMAT_RGBA16_SINT_PACK16;
    case PixelFormat::R16G16B16A16_SFLOAT:
      return gli::FORMAT_RGBA16_SFLOAT_PACK16;
    case PixelFormat::R32_UINT:
      return gli::FORMAT_R32_UINT_PACK32;
    case PixelFormat::R32_SINT:
      return gli::FORMAT_R32_SINT_PACK32;
    case PixelFormat::R32_SFLOAT:
      return gli::FORMAT_R32_SFLOAT_PACK32;
    case PixelFormat::R32G32_UINT:
      return gli::FORMAT_RG32_UINT_PACK32;
    case PixelFormat::R32G32_SINT:
      return gli::FORMAT_RG32_SINT_PACK32;
    case PixelFormat::R32G32_SFLOAT:
      return gli::FORMAT_RG32_SFLOAT_PACK32;
    case PixelFormat::R32G32B32_UINT:
      return gli::FORMAT_RGB32_UINT_PACK32;
    case PixelFormat::R32G32B32_SINT:
      return gli::FORMAT_RGB32_SINT_PACK32;
    case PixelFormat::R32G32B32_SFLOAT:
      return gli::FORMAT_RGB32_SFLOAT_PACK32;
    case PixelFormat::R32G32B32A32_UINT:
      return gli::FORMAT_RGBA32_UINT_PACK32;
    case PixelFormat::R32G32B32A32_SINT:
      return gli::FORMAT_RGBA32_SINT_PACK32;
    case PixelFormat::R32G32B32A32_SFLOAT:
      return gli::FORMAT_RGBA32_SFLOAT_PACK32;
    case PixelFormat::R64_UINT:
      return gli::FORMAT_R64_UINT_PACK64;
    case PixelFormat::R64_SINT:
      return gli::FORMAT_R64_SINT_PACK64;
    case PixelFormat::R64_SFLOAT:
      return gli::FORMAT_R64_SFLOAT_PACK64;
    case PixelFormat::R64G64_UINT:
      return gli::FORMAT_RG64_UINT_PACK64;
    case PixelFormat::R64G64_SINT:
      return gli::FORMAT_RG64_SINT_PACK64;
    case PixelFormat::R64G64_SFLOAT:
      return gli::FORMAT_RG64_SFLOAT_PACK64;
    case PixelFormat::R64G64B64_UINT:
      return gli::FORMAT_RGB64_UINT_PACK64;
    case PixelFormat::R64G64B64_SINT:
      return gli::FORMAT_RGB64_SINT_PACK64;
    case PixelFormat::R64G64B64_SFLOAT:
      return gli::FORMAT_RGB64_SFLOAT_PACK64;
    case PixelFormat::R64G64B64A64_UINT:
      return gli::FORMAT_RGBA64_UINT_PACK64;
    case PixelFormat::R64G64B64A64_SINT:
      return gli::FORMAT_RGBA64_SINT_PACK64;
    case PixelFormat::R64G64B64A64_SFLOAT:
      return gli::FORMAT_RGBA64_SFLOAT_PACK64;
    // case PixelFormat::B10G11R11_UFLOAT_PACK32:
    // case PixelFormat::E5B9G9R9_UFLOAT_PACK32:
    case PixelFormat::D16_UNORM:
      return gli::FORMAT_D16_UNORM_PACK16;
    // case PixelFormat::X8_D24_UNORM_PACK32:
    case PixelFormat::D32_SFLOAT:
      return gli::FORMAT_D32_SFLOAT_PACK32;
    case PixelFormat::S8_UINT:
      return gli::FORMAT_S8_UINT_PACK8;
    // case PixelFormat::D16_UNORM_S8_UINT:
    // case PixelFormat::D24_UNORM_S8_UINT:
    // case PixelFormat::D32_SFLOAT_S8_UINT:
    case PixelFormat::BC1_RGB_UNORM_BLOCK:
      return gli::FORMAT_RGB_DXT1_UNORM_BLOCK8;
    case PixelFormat::BC1_RGB_SRGB_BLOCK:
      return gli::FORMAT_RGB_DXT1_SRGB_BLOCK8;
    case PixelFormat::BC1_RGBA_UNORM_BLOCK:
      return gli::FORMAT_RGBA_DXT1_UNORM_BLOCK8;
    case PixelFormat::BC1_RGBA_SRGB_BLOCK:
      return gli::FORMAT_RGBA_DXT1_SRGB_BLOCK8;
    case PixelFormat::BC2_UNORM_BLOCK:
      return gli::FORMAT_RGBA_DXT3_UNORM_BLOCK16;
    case PixelFormat::BC2_SRGB_BLOCK:
      return gli::FORMAT_RGBA_DXT3_SRGB_BLOCK16;
    case PixelFormat::BC3_UNORM_BLOCK:
      return gli::FORMAT_RGBA_DXT5_UNORM_BLOCK16;
    case PixelFormat::BC3_SRGB_BLOCK:
      return gli::FORMAT_RGBA_DXT5_SRGB_BLOCK16;
    case PixelFormat::BC4_UNORM_BLOCK:
      return gli::FORMAT_R_ATI1N_UNORM_BLOCK8;
    case PixelFormat::BC4_SNORM_BLOCK:
      return gli::FORMAT_R_ATI1N_SNORM_BLOCK8;
    case PixelFormat::BC5_UNORM_BLOCK:
      return gli::FORMAT_RG_ATI2N_UNORM_BLOCK16;
    case PixelFormat::BC5_SNORM_BLOCK:
      return gli::FORMAT_RG_ATI2N_SNORM_BLOCK16;
    case PixelFormat::BC6H_UFLOAT_BLOCK:
      return gli::FORMAT_RGB_BP_UFLOAT_BLOCK16;
    case PixelFormat::BC6H_SFLOAT_BLOCK:
      return gli::FORMAT_RGB_BP_SFLOAT_BLOCK16;
    case PixelFormat::BC7_UNORM_BLOCK:
      return gli::FORMAT_RGBA_BP_UNORM_BLOCK16;
    case PixelFormat::BC7_SRGB_BLOCK:
      return gli::FORMAT_RGBA_BP_SRGB_BLOCK16;
    case PixelFormat::ETC2_R8G8B8_UNORM_BLOCK:
      return gli::FORMAT_RGB_ETC2_UNORM_BLOCK8;
    case PixelFormat::ETC2_R8G8B8_SRGB_BLOCK:
      return gli::FORMAT_RGB_ETC2_SRGB_BLOCK8;
    case PixelFormat::ETC2_R8G8B8A1_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ETC2_UNORM_BLOCK8;
    case PixelFormat::ETC2_R8G8B8A1_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ETC2_SRGB_BLOCK8;
    case PixelFormat::ETC2_R8G8B8A8_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ETC2_UNORM_BLOCK16;
    case PixelFormat::ETC2_R8G8B8A8_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ETC2_SRGB_BLOCK16;
    case PixelFormat::EAC_R11_UNORM_BLOCK:
      return gli::FORMAT_R_EAC_UNORM_BLOCK8;
    case PixelFormat::EAC_R11_SNORM_BLOCK:
      return gli::FORMAT_R_EAC_SNORM_BLOCK8;
    case PixelFormat::EAC_R11G11_UNORM_BLOCK:
      return gli::FORMAT_RG_EAC_UNORM_BLOCK16;
    case PixelFormat::EAC_R11G11_SNORM_BLOCK:
      return gli::FORMAT_RG_EAC_SNORM_BLOCK16;
    case PixelFormat::ASTC_4x4_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ASTC_4X4_UNORM_BLOCK16;
    case PixelFormat::ASTC_4x4_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ASTC_4X4_SRGB_BLOCK16;
    case PixelFormat::ASTC_5x4_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ASTC_5X4_UNORM_BLOCK16;
    case PixelFormat::ASTC_5x4_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ASTC_5X4_SRGB_BLOCK16;
    case PixelFormat::ASTC_5x5_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ASTC_5X5_UNORM_BLOCK16;
    case PixelFormat::ASTC_5x5_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ASTC_5X5_SRGB_BLOCK16;
    case PixelFormat::ASTC_6x5_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ASTC_6X5_UNORM_BLOCK16;
    case PixelFormat::ASTC_6x5_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ASTC_6X5_SRGB_BLOCK16;
    case PixelFormat::ASTC_6x6_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ASTC_6X6_UNORM_BLOCK16;
    case PixelFormat::ASTC_6x6_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ASTC_6X6_SRGB_BLOCK16;
    case PixelFormat::ASTC_8x5_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ASTC_8X5_UNORM_BLOCK16;
    case PixelFormat::ASTC_8x5_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ASTC_8X5_SRGB_BLOCK16;
    case PixelFormat::ASTC_8x6_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ASTC_8X6_UNORM_BLOCK16;
    case PixelFormat::ASTC_8x6_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ASTC_8X6_SRGB_BLOCK16;
    case PixelFormat::ASTC_8x8_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ASTC_8X8_UNORM_BLOCK16;
    case PixelFormat::ASTC_8x8_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ASTC_8X8_SRGB_BLOCK16;
    case PixelFormat::ASTC_10x5_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ASTC_10X5_UNORM_BLOCK16;
    case PixelFormat::ASTC_10x5_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ASTC_10X5_SRGB_BLOCK16;
    case PixelFormat::ASTC_10x6_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ASTC_10X6_UNORM_BLOCK16;
    case PixelFormat::ASTC_10x6_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ASTC_10X6_SRGB_BLOCK16;
    case PixelFormat::ASTC_10x8_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ASTC_10X8_UNORM_BLOCK16;
    case PixelFormat::ASTC_10x8_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ASTC_10X8_SRGB_BLOCK16;
    case PixelFormat::ASTC_10x10_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ASTC_10X10_UNORM_BLOCK16;
    case PixelFormat::ASTC_10x10_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ASTC_10X10_SRGB_BLOCK16;
    case PixelFormat::ASTC_12x10_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ASTC_12X10_UNORM_BLOCK16;
    case PixelFormat::ASTC_12x10_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ASTC_12X10_SRGB_BLOCK16;
    case PixelFormat::ASTC_12x12_UNORM_BLOCK:
      return gli::FORMAT_RGBA_ASTC_12X12_UNORM_BLOCK16;
    case PixelFormat::ASTC_12x12_SRGB_BLOCK:
      return gli::FORMAT_RGBA_ASTC_12X12_SRGB_BLOCK16;
    default:
      return gli::FORMAT_UNDEFINED;
    }
  }


  PixelFormat GLIConversionHelper::TryConvert(const gli::format format)
  {
    switch (format)
    {
    case gli::FORMAT_RG4_UNORM_PACK8:
      return PixelFormat::R4G4_UNORM_PACK8;
    case gli::FORMAT_RGBA4_UNORM_PACK16:
      return PixelFormat::R4G4B4A4_UNORM_PACK16;
    case gli::FORMAT_BGRA4_UNORM_PACK16:
      return PixelFormat::B4G4R4A4_UNORM_PACK16;
    case gli::FORMAT_R5G6B5_UNORM_PACK16:
      return PixelFormat::R5G6B5_UNORM_PACK16;
    case gli::FORMAT_B5G6R5_UNORM_PACK16:
      return PixelFormat::B5G6R5_UNORM_PACK16;
    case gli::FORMAT_RGB5A1_UNORM_PACK16:
      return PixelFormat::R5G5B5A1_UNORM_PACK16;
    case gli::FORMAT_BGR5A1_UNORM_PACK16:
      return PixelFormat::B5G5R5A1_UNORM_PACK16;
    case gli::FORMAT_A1RGB5_UNORM_PACK16:
      return PixelFormat::A1R5G5B5_UNORM_PACK16;

    case gli::FORMAT_R8_UNORM_PACK8:
      return PixelFormat::R8_UNORM;
    case gli::FORMAT_R8_SNORM_PACK8:
      return PixelFormat::R8_SNORM;
    case gli::FORMAT_R8_USCALED_PACK8:
      return PixelFormat::R8_USCALED;
    case gli::FORMAT_R8_SSCALED_PACK8:
      return PixelFormat::R8_SSCALED;
    case gli::FORMAT_R8_UINT_PACK8:
      return PixelFormat::R8_UINT;
    case gli::FORMAT_R8_SINT_PACK8:
      return PixelFormat::R8_SINT;
    case gli::FORMAT_R8_SRGB_PACK8:
      return PixelFormat::R8_SRGB;

    case gli::FORMAT_RG8_UNORM_PACK8:
      return PixelFormat::R8G8_UNORM;
    case gli::FORMAT_RG8_SNORM_PACK8:
      return PixelFormat::R8G8_SNORM;
    case gli::FORMAT_RG8_USCALED_PACK8:
      return PixelFormat::R8G8_USCALED;
    case gli::FORMAT_RG8_SSCALED_PACK8:
      return PixelFormat::R8G8_SSCALED;
    case gli::FORMAT_RG8_UINT_PACK8:
      return PixelFormat::R8G8_UINT;
    case gli::FORMAT_RG8_SINT_PACK8:
      return PixelFormat::R8G8_SINT;
    case gli::FORMAT_RG8_SRGB_PACK8:
      return PixelFormat::R8G8_SRGB;

    case gli::FORMAT_RGB8_UNORM_PACK8:
      return PixelFormat::R8G8B8_UNORM;
    case gli::FORMAT_RGB8_SNORM_PACK8:
      return PixelFormat::R8G8B8_SNORM;
    case gli::FORMAT_RGB8_USCALED_PACK8:
      return PixelFormat::R8G8B8_USCALED;
    case gli::FORMAT_RGB8_SSCALED_PACK8:
      return PixelFormat::R8G8B8_SSCALED;
    case gli::FORMAT_RGB8_UINT_PACK8:
      return PixelFormat::R8G8B8_UINT;
    case gli::FORMAT_RGB8_SINT_PACK8:
      return PixelFormat::R8G8B8_SINT;
    case gli::FORMAT_RGB8_SRGB_PACK8:
      return PixelFormat::R8G8B8_SRGB;

    case gli::FORMAT_BGR8_UNORM_PACK8:
      return PixelFormat::B8G8R8_UNORM;
    case gli::FORMAT_BGR8_SNORM_PACK8:
      return PixelFormat::B8G8R8_SNORM;
    case gli::FORMAT_BGR8_USCALED_PACK8:
      return PixelFormat::B8G8R8_USCALED;
    case gli::FORMAT_BGR8_SSCALED_PACK8:
      return PixelFormat::B8G8R8_SSCALED;
    case gli::FORMAT_BGR8_UINT_PACK8:
      return PixelFormat::B8G8R8_UINT;
    case gli::FORMAT_BGR8_SINT_PACK8:
      return PixelFormat::B8G8R8_SINT;
    case gli::FORMAT_BGR8_SRGB_PACK8:
      return PixelFormat::B8G8R8_SRGB;

    case gli::FORMAT_RGBA8_UNORM_PACK8:
      return PixelFormat::R8G8B8A8_UNORM;
    case gli::FORMAT_RGBA8_SNORM_PACK8:
      return PixelFormat::R8G8B8A8_SNORM;
    case gli::FORMAT_RGBA8_USCALED_PACK8:
      return PixelFormat::R8G8B8A8_USCALED;
    case gli::FORMAT_RGBA8_SSCALED_PACK8:
      return PixelFormat::R8G8B8A8_SSCALED;
    case gli::FORMAT_RGBA8_UINT_PACK8:
      return PixelFormat::R8G8B8A8_UINT;
    case gli::FORMAT_RGBA8_SINT_PACK8:
      return PixelFormat::R8G8B8A8_SINT;
    case gli::FORMAT_RGBA8_SRGB_PACK8:
      return PixelFormat::R8G8B8A8_SRGB;

    case gli::FORMAT_BGRA8_UNORM_PACK8:
      return PixelFormat::B8G8R8A8_UNORM;
    case gli::FORMAT_BGRA8_SNORM_PACK8:
      return PixelFormat::B8G8R8A8_SNORM;
    case gli::FORMAT_BGRA8_USCALED_PACK8:
      return PixelFormat::B8G8R8A8_USCALED;
    case gli::FORMAT_BGRA8_SSCALED_PACK8:
      return PixelFormat::B8G8R8A8_SSCALED;
    case gli::FORMAT_BGRA8_UINT_PACK8:
      return PixelFormat::B8G8R8A8_UINT;
    case gli::FORMAT_BGRA8_SINT_PACK8:
      return PixelFormat::B8G8R8A8_SINT;
    case gli::FORMAT_BGRA8_SRGB_PACK8:
      return PixelFormat::B8G8R8A8_SRGB;

      // case gli::FORMAT_RGBA8_UNORM_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_RGBA8_SNORM_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_RGBA8_USCALED_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_RGBA8_SSCALED_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_RGBA8_UINT_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_RGBA8_SINT_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_RGBA8_SRGB_PACK32:
      //  return PixelFormat::Undefined;

      // case gli::FORMAT_RGB10A2_UNORM_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_RGB10A2_SNORM_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_RGB10A2_USCALED_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_RGB10A2_SSCALED_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_RGB10A2_UINT_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_RGB10A2_SINT_PACK32:
      //  return PixelFormat::Undefined;

      // case gli::FORMAT_BGR10A2_UNORM_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_BGR10A2_SNORM_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_BGR10A2_USCALED_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_BGR10A2_SSCALED_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_BGR10A2_UINT_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_BGR10A2_SINT_PACK32:
      //  return PixelFormat::Undefined;

    case gli::FORMAT_R16_UNORM_PACK16:
      return PixelFormat::R16_UNORM;
    case gli::FORMAT_R16_SNORM_PACK16:
      return PixelFormat::R16_SNORM;
    case gli::FORMAT_R16_USCALED_PACK16:
      return PixelFormat::R16_USCALED;
    case gli::FORMAT_R16_SSCALED_PACK16:
      return PixelFormat::R16_SSCALED;
    case gli::FORMAT_R16_UINT_PACK16:
      return PixelFormat::R16_UINT;
    case gli::FORMAT_R16_SINT_PACK16:
      return PixelFormat::R16_SINT;
    case gli::FORMAT_R16_SFLOAT_PACK16:
      return PixelFormat::R16_SFLOAT;

    case gli::FORMAT_RG16_UNORM_PACK16:
      return PixelFormat::R16G16_UNORM;
    case gli::FORMAT_RG16_SNORM_PACK16:
      return PixelFormat::R16G16_SNORM;
    case gli::FORMAT_RG16_USCALED_PACK16:
      return PixelFormat::R16G16_USCALED;
    case gli::FORMAT_RG16_SSCALED_PACK16:
      return PixelFormat::R16G16_SSCALED;
    case gli::FORMAT_RG16_UINT_PACK16:
      return PixelFormat::R16G16_UINT;
    case gli::FORMAT_RG16_SINT_PACK16:
      return PixelFormat::R16G16_SINT;
    case gli::FORMAT_RG16_SFLOAT_PACK16:
      return PixelFormat::R16G16_SFLOAT;

    case gli::FORMAT_RGB16_UNORM_PACK16:
      return PixelFormat::R16G16B16_UNORM;
    case gli::FORMAT_RGB16_SNORM_PACK16:
      return PixelFormat::R16G16B16_SNORM;
    case gli::FORMAT_RGB16_USCALED_PACK16:
      return PixelFormat::R16G16B16_USCALED;
    case gli::FORMAT_RGB16_SSCALED_PACK16:
      return PixelFormat::R16G16B16_SSCALED;
    case gli::FORMAT_RGB16_UINT_PACK16:
      return PixelFormat::R16G16B16_UINT;
    case gli::FORMAT_RGB16_SINT_PACK16:
      return PixelFormat::R16G16B16_SINT;
    case gli::FORMAT_RGB16_SFLOAT_PACK16:
      return PixelFormat::R16G16B16_SFLOAT;

    case gli::FORMAT_RGBA16_UNORM_PACK16:
      return PixelFormat::R16G16B16A16_UNORM;
    case gli::FORMAT_RGBA16_SNORM_PACK16:
      return PixelFormat::R16G16B16A16_SNORM;
    case gli::FORMAT_RGBA16_USCALED_PACK16:
      return PixelFormat::R16G16B16A16_USCALED;
    case gli::FORMAT_RGBA16_SSCALED_PACK16:
      return PixelFormat::R16G16B16A16_SSCALED;
    case gli::FORMAT_RGBA16_UINT_PACK16:
      return PixelFormat::R16G16B16A16_UINT;
    case gli::FORMAT_RGBA16_SINT_PACK16:
      return PixelFormat::R16G16B16A16_SINT;
    case gli::FORMAT_RGBA16_SFLOAT_PACK16:
      return PixelFormat::R16G16B16A16_SFLOAT;

    case gli::FORMAT_R32_UINT_PACK32:
      return PixelFormat::R32_UINT;
    case gli::FORMAT_R32_SINT_PACK32:
      return PixelFormat::R32_SINT;
    case gli::FORMAT_R32_SFLOAT_PACK32:
      return PixelFormat::R32_SFLOAT;

    case gli::FORMAT_RG32_UINT_PACK32:
      return PixelFormat::R32G32_UINT;
    case gli::FORMAT_RG32_SINT_PACK32:
      return PixelFormat::R32G32_SINT;
    case gli::FORMAT_RG32_SFLOAT_PACK32:
      return PixelFormat::R32G32_SFLOAT;

    case gli::FORMAT_RGB32_UINT_PACK32:
      return PixelFormat::R32G32B32_UINT;
    case gli::FORMAT_RGB32_SINT_PACK32:
      return PixelFormat::R32G32B32_SINT;
    case gli::FORMAT_RGB32_SFLOAT_PACK32:
      return PixelFormat::R32G32B32_SFLOAT;

    case gli::FORMAT_RGBA32_UINT_PACK32:
      return PixelFormat::R32G32B32A32_UINT;
    case gli::FORMAT_RGBA32_SINT_PACK32:
      return PixelFormat::R32G32B32A32_SINT;
    case gli::FORMAT_RGBA32_SFLOAT_PACK32:
      return PixelFormat::R32G32B32A32_SFLOAT;

    case gli::FORMAT_R64_UINT_PACK64:
      return PixelFormat::R64_UINT;
    case gli::FORMAT_R64_SINT_PACK64:
      return PixelFormat::R64_SINT;
    case gli::FORMAT_R64_SFLOAT_PACK64:
      return PixelFormat::R64_SFLOAT;

    case gli::FORMAT_RG64_UINT_PACK64:
      return PixelFormat::R64G64_UINT;
    case gli::FORMAT_RG64_SINT_PACK64:
      return PixelFormat::R64G64_SINT;
    case gli::FORMAT_RG64_SFLOAT_PACK64:
      return PixelFormat::R64G64_SFLOAT;

    case gli::FORMAT_RGB64_UINT_PACK64:
      return PixelFormat::R64G64B64_UINT;
    case gli::FORMAT_RGB64_SINT_PACK64:
      return PixelFormat::R64G64B64_SINT;
    case gli::FORMAT_RGB64_SFLOAT_PACK64:
      return PixelFormat::R64G64B64_SFLOAT;

    case gli::FORMAT_RGBA64_UINT_PACK64:
      return PixelFormat::R64G64B64A64_UINT;
    case gli::FORMAT_RGBA64_SINT_PACK64:
      return PixelFormat::R64G64B64A64_SINT;
    case gli::FORMAT_RGBA64_SFLOAT_PACK64:
      return PixelFormat::R64G64B64A64_SFLOAT;

      // case gli::FORMAT_RG11B10_UFLOAT_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_RGB9E5_UFLOAT_PACK32:
      //  return PixelFormat::Undefined;

    case gli::FORMAT_D16_UNORM_PACK16:
      return PixelFormat::D16_UNORM;
    // case gli::FORMAT_D24_UNORM_PACK32:
    //  return PixelFormat::Undefined;
    case gli::FORMAT_D32_SFLOAT_PACK32:
      return PixelFormat::D32_SFLOAT;
    case gli::FORMAT_S8_UINT_PACK8:
      return PixelFormat::S8_UINT;
      // case gli::FORMAT_D16_UNORM_S8_UINT_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_D24_UNORM_S8_UINT_PACK32:
      //  return PixelFormat::Undefined;
      // case gli::FORMAT_D32_SFLOAT_S8_UINT_PACK64:
      //  return PixelFormat::Undefined;

    case gli::FORMAT_RGB_DXT1_UNORM_BLOCK8:
      return PixelFormat::BC1_RGB_UNORM_BLOCK;
    case gli::FORMAT_RGB_DXT1_SRGB_BLOCK8:
      return PixelFormat::BC1_RGB_SRGB_BLOCK;
    case gli::FORMAT_RGBA_DXT1_UNORM_BLOCK8:
      return PixelFormat::BC1_RGBA_UNORM_BLOCK;
    case gli::FORMAT_RGBA_DXT1_SRGB_BLOCK8:
      return PixelFormat::BC1_RGBA_SRGB_BLOCK;
    case gli::FORMAT_RGBA_DXT3_UNORM_BLOCK16:
      return PixelFormat::BC2_UNORM_BLOCK;
    case gli::FORMAT_RGBA_DXT3_SRGB_BLOCK16:
      return PixelFormat::BC2_SRGB_BLOCK;
    case gli::FORMAT_RGBA_DXT5_UNORM_BLOCK16:
      return PixelFormat::BC3_UNORM_BLOCK;
    case gli::FORMAT_RGBA_DXT5_SRGB_BLOCK16:
      return PixelFormat::BC3_SRGB_BLOCK;
    case gli::FORMAT_R_ATI1N_UNORM_BLOCK8:
      return PixelFormat::BC4_UNORM_BLOCK;
    case gli::FORMAT_R_ATI1N_SNORM_BLOCK8:
      return PixelFormat::BC4_SNORM_BLOCK;
    case gli::FORMAT_RG_ATI2N_UNORM_BLOCK16:
      return PixelFormat::BC5_UNORM_BLOCK;
    case gli::FORMAT_RG_ATI2N_SNORM_BLOCK16:
      return PixelFormat::BC5_SNORM_BLOCK;
    case gli::FORMAT_RGB_BP_UFLOAT_BLOCK16:
      return PixelFormat::BC6H_UFLOAT_BLOCK;
    case gli::FORMAT_RGB_BP_SFLOAT_BLOCK16:
      return PixelFormat::BC6H_SFLOAT_BLOCK;
    case gli::FORMAT_RGBA_BP_UNORM_BLOCK16:
      return PixelFormat::BC7_UNORM_BLOCK;
    case gli::FORMAT_RGBA_BP_SRGB_BLOCK16:
      return PixelFormat::BC7_SRGB_BLOCK;

    case gli::FORMAT_RGB_ETC2_UNORM_BLOCK8:
      return PixelFormat::ETC2_R8G8B8_UNORM_BLOCK;    // ?
    case gli::FORMAT_RGB_ETC2_SRGB_BLOCK8:
      return PixelFormat::ETC2_R8G8B8_SRGB_BLOCK;    // ?
    case gli::FORMAT_RGBA_ETC2_UNORM_BLOCK8:
      return PixelFormat::ETC2_R8G8B8A1_UNORM_BLOCK;    // ?
    case gli::FORMAT_RGBA_ETC2_SRGB_BLOCK8:
      return PixelFormat::ETC2_R8G8B8A1_SRGB_BLOCK;    // ?
    case gli::FORMAT_RGBA_ETC2_UNORM_BLOCK16:
      return PixelFormat::ETC2_R8G8B8A8_UNORM_BLOCK;    // ?
    case gli::FORMAT_RGBA_ETC2_SRGB_BLOCK16:
      return PixelFormat::ETC2_R8G8B8A8_SRGB_BLOCK;    // ?
    case gli::FORMAT_R_EAC_UNORM_BLOCK8:
      return PixelFormat::EAC_R11_UNORM_BLOCK;    // ?
    case gli::FORMAT_R_EAC_SNORM_BLOCK8:
      return PixelFormat::EAC_R11_SNORM_BLOCK;    // ?
    case gli::FORMAT_RG_EAC_UNORM_BLOCK16:
      return PixelFormat::EAC_R11G11_UNORM_BLOCK;    // ?
    case gli::FORMAT_RG_EAC_SNORM_BLOCK16:
      return PixelFormat::EAC_R11G11_SNORM_BLOCK;    // ?

    case gli::FORMAT_RGBA_ASTC_4X4_UNORM_BLOCK16:
      return PixelFormat::ASTC_4x4_UNORM_BLOCK;
    case gli::FORMAT_RGBA_ASTC_4X4_SRGB_BLOCK16:
      return PixelFormat::ASTC_4x4_SRGB_BLOCK;
    case gli::FORMAT_RGBA_ASTC_5X4_UNORM_BLOCK16:
      return PixelFormat::ASTC_5x4_UNORM_BLOCK;
    case gli::FORMAT_RGBA_ASTC_5X4_SRGB_BLOCK16:
      return PixelFormat::ASTC_5x4_SRGB_BLOCK;
    case gli::FORMAT_RGBA_ASTC_5X5_UNORM_BLOCK16:
      return PixelFormat::ASTC_5x5_UNORM_BLOCK;
    case gli::FORMAT_RGBA_ASTC_5X5_SRGB_BLOCK16:
      return PixelFormat::ASTC_5x5_SRGB_BLOCK;
    case gli::FORMAT_RGBA_ASTC_6X5_UNORM_BLOCK16:
      return PixelFormat::ASTC_6x5_UNORM_BLOCK;
    case gli::FORMAT_RGBA_ASTC_6X5_SRGB_BLOCK16:
      return PixelFormat::ASTC_6x5_SRGB_BLOCK;
    case gli::FORMAT_RGBA_ASTC_6X6_UNORM_BLOCK16:
      return PixelFormat::ASTC_6x6_UNORM_BLOCK;
    case gli::FORMAT_RGBA_ASTC_6X6_SRGB_BLOCK16:
      return PixelFormat::ASTC_6x6_SRGB_BLOCK;
    case gli::FORMAT_RGBA_ASTC_8X5_UNORM_BLOCK16:
      return PixelFormat::ASTC_8x5_UNORM_BLOCK;
    case gli::FORMAT_RGBA_ASTC_8X5_SRGB_BLOCK16:
      return PixelFormat::ASTC_8x5_SRGB_BLOCK;
    case gli::FORMAT_RGBA_ASTC_8X6_UNORM_BLOCK16:
      return PixelFormat::ASTC_8x6_UNORM_BLOCK;
    case gli::FORMAT_RGBA_ASTC_8X6_SRGB_BLOCK16:
      return PixelFormat::ASTC_8x6_SRGB_BLOCK;
    case gli::FORMAT_RGBA_ASTC_8X8_UNORM_BLOCK16:
      return PixelFormat::ASTC_8x8_UNORM_BLOCK;
    case gli::FORMAT_RGBA_ASTC_8X8_SRGB_BLOCK16:
      return PixelFormat::ASTC_8x8_SRGB_BLOCK;
    case gli::FORMAT_RGBA_ASTC_10X5_UNORM_BLOCK16:
      return PixelFormat::ASTC_10x5_UNORM_BLOCK;
    case gli::FORMAT_RGBA_ASTC_10X5_SRGB_BLOCK16:
      return PixelFormat::ASTC_10x5_SRGB_BLOCK;
    case gli::FORMAT_RGBA_ASTC_10X6_UNORM_BLOCK16:
      return PixelFormat::ASTC_10x6_UNORM_BLOCK;
    case gli::FORMAT_RGBA_ASTC_10X6_SRGB_BLOCK16:
      return PixelFormat::ASTC_10x6_SRGB_BLOCK;
    case gli::FORMAT_RGBA_ASTC_10X8_UNORM_BLOCK16:
      return PixelFormat::ASTC_10x8_UNORM_BLOCK;
    case gli::FORMAT_RGBA_ASTC_10X8_SRGB_BLOCK16:
      return PixelFormat::ASTC_10x8_SRGB_BLOCK;
    case gli::FORMAT_RGBA_ASTC_10X10_UNORM_BLOCK16:
      return PixelFormat::ASTC_10x10_UNORM_BLOCK;
    case gli::FORMAT_RGBA_ASTC_10X10_SRGB_BLOCK16:
      return PixelFormat::ASTC_10x10_SRGB_BLOCK;
    case gli::FORMAT_RGBA_ASTC_12X10_UNORM_BLOCK16:
      return PixelFormat::ASTC_12x10_UNORM_BLOCK;
    case gli::FORMAT_RGBA_ASTC_12X10_SRGB_BLOCK16:
      return PixelFormat::ASTC_12x10_SRGB_BLOCK;
    case gli::FORMAT_RGBA_ASTC_12X12_UNORM_BLOCK16:
      return PixelFormat::ASTC_12x12_UNORM_BLOCK;
    case gli::FORMAT_RGBA_ASTC_12X12_SRGB_BLOCK16:
      return PixelFormat::ASTC_12x12_SRGB_BLOCK;

    // case gli::FORMAT_RGB_PVRTC1_8X8_UNORM_BLOCK32:
    //  return PixelFormat::Undefined;
    // case gli::FORMAT_RGB_PVRTC1_8X8_SRGB_BLOCK32:
    //  return PixelFormat::Undefined;
    // case gli::FORMAT_RGB_PVRTC1_16X8_UNORM_BLOCK32:
    //  return PixelFormat::Undefined;
    // case gli::FORMAT_RGB_PVRTC1_16X8_SRGB_BLOCK32:
    //  return PixelFormat::Undefined;
    // case gli::FORMAT_RGBA_PVRTC1_8X8_UNORM_BLOCK32:
    //  return PixelFormat::Undefined;
    // case gli::FORMAT_RGBA_PVRTC1_8X8_SRGB_BLOCK32:
    //  return PixelFormat::Undefined;
    // case gli::FORMAT_RGBA_PVRTC1_16X8_UNORM_BLOCK32:
    //  return PixelFormat::Undefined;
    // case gli::FORMAT_RGBA_PVRTC1_16X8_SRGB_BLOCK32:
    //  return PixelFormat::Undefined;
    // case gli::FORMAT_RGBA_PVRTC2_4X4_UNORM_BLOCK8:
    //  return PixelFormat::Undefined;
    // case gli::FORMAT_RGBA_PVRTC2_4X4_SRGB_BLOCK8:
    //  return PixelFormat::Undefined;
    // case gli::FORMAT_RGBA_PVRTC2_8X4_UNORM_BLOCK8:
    //  return PixelFormat::Undefined;
    // case gli::FORMAT_RGBA_PVRTC2_8X4_SRGB_BLOCK8:
    //  return PixelFormat::Undefined;

    // case gli::FORMAT_RGB_ETC_UNORM_BLOCK8:
    // case gli::FORMAT_RGB_ATC_UNORM_BLOCK8:
    // case gli::FORMAT_RGBA_ATCA_UNORM_BLOCK16:
    // case gli::FORMAT_RGBA_ATCI_UNORM_BLOCK16:

    // case gli::FORMAT_L8_UNORM_PACK8:
    // case gli::FORMAT_A8_UNORM_PACK8:
    // case gli::FORMAT_LA8_UNORM_PACK8:
    // case gli::FORMAT_L16_UNORM_PACK16:
    // case gli::FORMAT_A16_UNORM_PACK16:
    // case gli::FORMAT_LA16_UNORM_PACK16:
    // case gli::FORMAT_BGR8_UNORM_PACK32:
    // case gli::FORMAT_BGR8_SRGB_PACK32:
    // case gli::FORMAT_RG3B2_UNORM_PACK8:
    default:
      return PixelFormat::Undefined;
    }
  }


  bool GLIConversionHelper::TryConvert(const TextureType textureType, gli::target& rResult)
  {
    switch (textureType)
    {
    case TextureType::Tex1D:
      rResult = gli::TARGET_1D;
      return true;
    case TextureType::Tex2D:
      rResult = gli::TARGET_2D;
      return true;
    case TextureType::Tex3D:
      rResult = gli::TARGET_3D;
      return true;
    case TextureType::TexCube:
      rResult = gli::TARGET_CUBE;
      return true;
    case TextureType::Tex1DArray:
      rResult = gli::TARGET_1D_ARRAY;
      return true;
    case TextureType::Tex2DArray:
      rResult = gli::TARGET_2D_ARRAY;
      return true;
    case TextureType::TexCubeArray:
      rResult = gli::TARGET_CUBE_ARRAY;
      return true;
    default:
      rResult = gli::TARGET_1D;
      return false;
    }
  }


  TextureType GLIConversionHelper::TryConvert(const gli::target target)
  {
    switch (target)
    {
    case gli::TARGET_1D:
      return TextureType::Tex1D;
    case gli::TARGET_2D:
      return TextureType::Tex2D;
    case gli::TARGET_3D:
      return TextureType::Tex3D;
    case gli::TARGET_CUBE:
      return TextureType::TexCube;
    case gli::TARGET_1D_ARRAY:
      return TextureType::Tex1DArray;
    case gli::TARGET_2D_ARRAY:
      return TextureType::Tex2DArray;
    case gli::TARGET_CUBE_ARRAY:
      return TextureType::TexCubeArray;
    default:
      return TextureType::Undefined;
    }
  }
}
#endif
