/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

#include <FslGraphics/Debug/DebugPixelFormat.hpp>

namespace Fsl::Debug
{
  const char* TryToString(const PixelFormat& value)
  {
    switch (value)
    {
    case PixelFormat::Undefined:
      return "PixelFormat::Undefined";
    case PixelFormat::R4G4_UNORM_PACK8:
      return "PixelFormat::R4G4_UNORM_PACK8";
    case PixelFormat::R4G4B4A4_UNORM_PACK16:
      return "PixelFormat::R4G4B4A4_UNORM_PACK16";
    case PixelFormat::B4G4R4A4_UNORM_PACK16:
      return "PixelFormat::B4G4R4A4_UNORM_PACK16";
    case PixelFormat::R5G6B5_UNORM_PACK16:
      return "PixelFormat::R5G6B5_UNORM_PACK16";
    case PixelFormat::B5G6R5_UNORM_PACK16:
      return "PixelFormat::B5G6R5_UNORM_PACK16";
    case PixelFormat::R5G5B5A1_UNORM_PACK16:
      return "PixelFormat::R5G5B5A1_UNORM_PACK16";
    case PixelFormat::B5G5R5A1_UNORM_PACK16:
      return "PixelFormat::B5G5R5A1_UNORM_PACK16";
    case PixelFormat::A1R5G5B5_UNORM_PACK16:
      return "PixelFormat::A1R5G5B5_UNORM_PACK16";
    case PixelFormat::R8_UNORM:
      return "PixelFormat::R8_UNORM";
    case PixelFormat::R8_SNORM:
      return "PixelFormat::R8_SNORM";
    case PixelFormat::R8_USCALED:
      return "PixelFormat::R8_USCALED";
    case PixelFormat::R8_SSCALED:
      return "PixelFormat::R8_SSCALED";
    case PixelFormat::R8_UINT:
      return "PixelFormat::R8_UINT";
    case PixelFormat::R8_SINT:
      return "PixelFormat::R8_SINT";
    case PixelFormat::R8_SRGB:
      return "PixelFormat::R8_SRGB";
    case PixelFormat::R8G8_UNORM:
      return "PixelFormat::R8G8_UNORM";
    case PixelFormat::R8G8_SNORM:
      return "PixelFormat::R8G8_SNORM";
    case PixelFormat::R8G8_USCALED:
      return "PixelFormat::R8G8_USCALED";
    case PixelFormat::R8G8_SSCALED:
      return "PixelFormat::R8G8_SSCALED";
    case PixelFormat::R8G8_UINT:
      return "PixelFormat::R8G8_UINT";
    case PixelFormat::R8G8_SINT:
      return "PixelFormat::R8G8_SINT";
    case PixelFormat::R8G8_SRGB:
      return "PixelFormat::R8G8_SRGB";
    case PixelFormat::R8G8B8_UNORM:
      return "PixelFormat::R8G8B8_UNORM";
    case PixelFormat::R8G8B8_SNORM:
      return "PixelFormat::R8G8B8_SNORM";
    case PixelFormat::R8G8B8_USCALED:
      return "PixelFormat::R8G8B8_USCALED";
    case PixelFormat::R8G8B8_SSCALED:
      return "PixelFormat::R8G8B8_SSCALED";
    case PixelFormat::R8G8B8_UINT:
      return "PixelFormat::R8G8B8_UINT";
    case PixelFormat::R8G8B8_SINT:
      return "PixelFormat::R8G8B8_SINT";
    case PixelFormat::R8G8B8_SRGB:
      return "PixelFormat::R8G8B8_SRGB";
    case PixelFormat::B8G8R8_UNORM:
      return "PixelFormat::B8G8R8_UNORM";
    case PixelFormat::B8G8R8_SNORM:
      return "PixelFormat::B8G8R8_SNORM";
    case PixelFormat::B8G8R8_USCALED:
      return "PixelFormat::B8G8R8_USCALED";
    case PixelFormat::B8G8R8_SSCALED:
      return "PixelFormat::B8G8R8_SSCALED";
    case PixelFormat::B8G8R8_UINT:
      return "PixelFormat::B8G8R8_UINT";
    case PixelFormat::B8G8R8_SINT:
      return "PixelFormat::B8G8R8_SINT";
    case PixelFormat::B8G8R8_SRGB:
      return "PixelFormat::B8G8R8_SRGB";
    case PixelFormat::R8G8B8A8_UNORM:
      return "PixelFormat::R8G8B8A8_UNORM";
    case PixelFormat::R8G8B8A8_SNORM:
      return "PixelFormat::R8G8B8A8_SNORM";
    case PixelFormat::R8G8B8A8_USCALED:
      return "PixelFormat::R8G8B8A8_USCALED";
    case PixelFormat::R8G8B8A8_SSCALED:
      return "PixelFormat::R8G8B8A8_SSCALED";
    case PixelFormat::R8G8B8A8_UINT:
      return "PixelFormat::R8G8B8A8_UINT";
    case PixelFormat::R8G8B8A8_SINT:
      return "PixelFormat::R8G8B8A8_SINT";
    case PixelFormat::R8G8B8A8_SRGB:
      return "PixelFormat::R8G8B8A8_SRGB";
    case PixelFormat::B8G8R8A8_UNORM:
      return "PixelFormat::B8G8R8A8_UNORM";
    case PixelFormat::B8G8R8A8_SNORM:
      return "PixelFormat::B8G8R8A8_SNORM";
    case PixelFormat::B8G8R8A8_USCALED:
      return "PixelFormat::B8G8R8A8_USCALED";
    case PixelFormat::B8G8R8A8_SSCALED:
      return "PixelFormat::B8G8R8A8_SSCALED";
    case PixelFormat::B8G8R8A8_UINT:
      return "PixelFormat::B8G8R8A8_UINT";
    case PixelFormat::B8G8R8A8_SINT:
      return "PixelFormat::B8G8R8A8_SINT";
    case PixelFormat::B8G8R8A8_SRGB:
      return "PixelFormat::B8G8R8A8_SRGB";
    case PixelFormat::A8B8G8R8_UNORM_PACK32:
      return "PixelFormat::A8B8G8R8_UNORM_PACK32";
    case PixelFormat::A8B8G8R8_SNORM_PACK32:
      return "PixelFormat::A8B8G8R8_SNORM_PACK32";
    case PixelFormat::A8B8G8R8_USCALED_PACK32:
      return "PixelFormat::A8B8G8R8_USCALED_PACK32";
    case PixelFormat::A8B8G8R8_SSCALED_PACK32:
      return "PixelFormat::A8B8G8R8_SSCALED_PACK32";
    case PixelFormat::A8B8G8R8_UINT_PACK32:
      return "PixelFormat::A8B8G8R8_UINT_PACK32";
    case PixelFormat::A8B8G8R8_SINT_PACK32:
      return "PixelFormat::A8B8G8R8_SINT_PACK32";
    case PixelFormat::A8B8G8R8_SRGB_PACK32:
      return "PixelFormat::A8B8G8R8_SRGB_PACK32";
    case PixelFormat::A2R10G10B10_UNORM_PACK32:
      return "PixelFormat::A2R10G10B10_UNORM_PACK32";
    case PixelFormat::A2R10G10B10_SNORM_PACK32:
      return "PixelFormat::A2R10G10B10_SNORM_PACK32";
    case PixelFormat::A2R10G10B10_USCALED_PACK32:
      return "PixelFormat::A2R10G10B10_USCALED_PACK32";
    case PixelFormat::A2R10G10B10_SSCALED_PACK32:
      return "PixelFormat::A2R10G10B10_SSCALED_PACK32";
    case PixelFormat::A2R10G10B10_UINT_PACK32:
      return "PixelFormat::A2R10G10B10_UINT_PACK32";
    case PixelFormat::A2R10G10B10_SINT_PACK32:
      return "PixelFormat::A2R10G10B10_SINT_PACK32";
    case PixelFormat::A2B10G10R10_UNORM_PACK32:
      return "PixelFormat::A2B10G10R10_UNORM_PACK32";
    case PixelFormat::A2B10G10R10_SNORM_PACK32:
      return "PixelFormat::A2B10G10R10_SNORM_PACK32";
    case PixelFormat::A2B10G10R10_USCALED_PACK32:
      return "PixelFormat::A2B10G10R10_USCALED_PACK32";
    case PixelFormat::A2B10G10R10_SSCALED_PACK32:
      return "PixelFormat::A2B10G10R10_SSCALED_PACK32";
    case PixelFormat::A2B10G10R10_UINT_PACK32:
      return "PixelFormat::A2B10G10R10_UINT_PACK32";
    case PixelFormat::A2B10G10R10_SINT_PACK32:
      return "PixelFormat::A2B10G10R10_SINT_PACK32";
    case PixelFormat::R16_UNORM:
      return "PixelFormat::R16_UNORM";
    case PixelFormat::R16_SNORM:
      return "PixelFormat::R16_SNORM";
    case PixelFormat::R16_USCALED:
      return "PixelFormat::R16_USCALED";
    case PixelFormat::R16_SSCALED:
      return "PixelFormat::R16_SSCALED";
    case PixelFormat::R16_UINT:
      return "PixelFormat::R16_UINT";
    case PixelFormat::R16_SINT:
      return "PixelFormat::R16_SINT";
    case PixelFormat::R16_SFLOAT:
      return "PixelFormat::R16_SFLOAT";
    case PixelFormat::R16G16_UNORM:
      return "PixelFormat::R16G16_UNORM";
    case PixelFormat::R16G16_SNORM:
      return "PixelFormat::R16G16_SNORM";
    case PixelFormat::R16G16_USCALED:
      return "PixelFormat::R16G16_USCALED";
    case PixelFormat::R16G16_SSCALED:
      return "PixelFormat::R16G16_SSCALED";
    case PixelFormat::R16G16_UINT:
      return "PixelFormat::R16G16_UINT";
    case PixelFormat::R16G16_SINT:
      return "PixelFormat::R16G16_SINT";
    case PixelFormat::R16G16_SFLOAT:
      return "PixelFormat::R16G16_SFLOAT";
    case PixelFormat::R16G16B16_UNORM:
      return "PixelFormat::R16G16B16_UNORM";
    case PixelFormat::R16G16B16_SNORM:
      return "PixelFormat::R16G16B16_SNORM";
    case PixelFormat::R16G16B16_USCALED:
      return "PixelFormat::R16G16B16_USCALED";
    case PixelFormat::R16G16B16_SSCALED:
      return "PixelFormat::R16G16B16_SSCALED";
    case PixelFormat::R16G16B16_UINT:
      return "PixelFormat::R16G16B16_UINT";
    case PixelFormat::R16G16B16_SINT:
      return "PixelFormat::R16G16B16_SINT";
    case PixelFormat::R16G16B16_SFLOAT:
      return "PixelFormat::R16G16B16_SFLOAT";
    case PixelFormat::R16G16B16A16_UNORM:
      return "PixelFormat::R16G16B16A16_UNORM";
    case PixelFormat::R16G16B16A16_SNORM:
      return "PixelFormat::R16G16B16A16_SNORM";
    case PixelFormat::R16G16B16A16_USCALED:
      return "PixelFormat::R16G16B16A16_USCALED";
    case PixelFormat::R16G16B16A16_SSCALED:
      return "PixelFormat::R16G16B16A16_SSCALED";
    case PixelFormat::R16G16B16A16_UINT:
      return "PixelFormat::R16G16B16A16_UINT";
    case PixelFormat::R16G16B16A16_SINT:
      return "PixelFormat::R16G16B16A16_SINT";
    case PixelFormat::R16G16B16A16_SFLOAT:
      return "PixelFormat::R16G16B16A16_SFLOAT";
    case PixelFormat::R32_UINT:
      return "PixelFormat::R32_UINT";
    case PixelFormat::R32_SINT:
      return "PixelFormat::R32_SINT";
    case PixelFormat::R32_SFLOAT:
      return "PixelFormat::R32_SFLOAT";
    case PixelFormat::R32G32_UINT:
      return "PixelFormat::R32G32_UINT";
    case PixelFormat::R32G32_SINT:
      return "PixelFormat::R32G32_SINT";
    case PixelFormat::R32G32_SFLOAT:
      return "PixelFormat::R32G32_SFLOAT";
    case PixelFormat::R32G32B32_UINT:
      return "PixelFormat::R32G32B32_UINT";
    case PixelFormat::R32G32B32_SINT:
      return "PixelFormat::R32G32B32_SINT";
    case PixelFormat::R32G32B32_SFLOAT:
      return "PixelFormat::R32G32B32_SFLOAT";
    case PixelFormat::R32G32B32A32_UINT:
      return "PixelFormat::R32G32B32A32_UINT";
    case PixelFormat::R32G32B32A32_SINT:
      return "PixelFormat::R32G32B32A32_SINT";
    case PixelFormat::R32G32B32A32_SFLOAT:
      return "PixelFormat::R32G32B32A32_SFLOAT";
    case PixelFormat::R64_UINT:
      return "PixelFormat::R64_UINT";
    case PixelFormat::R64_SINT:
      return "PixelFormat::R64_SINT";
    case PixelFormat::R64_SFLOAT:
      return "PixelFormat::R64_SFLOAT";
    case PixelFormat::R64G64_UINT:
      return "PixelFormat::R64G64_UINT";
    case PixelFormat::R64G64_SINT:
      return "PixelFormat::R64G64_SINT";
    case PixelFormat::R64G64_SFLOAT:
      return "PixelFormat::R64G64_SFLOAT";
    case PixelFormat::R64G64B64_UINT:
      return "PixelFormat::R64G64B64_UINT";
    case PixelFormat::R64G64B64_SINT:
      return "PixelFormat::R64G64B64_SINT";
    case PixelFormat::R64G64B64_SFLOAT:
      return "PixelFormat::R64G64B64_SFLOAT";
    case PixelFormat::R64G64B64A64_UINT:
      return "PixelFormat::R64G64B64A64_UINT";
    case PixelFormat::R64G64B64A64_SINT:
      return "PixelFormat::R64G64B64A64_SINT";
    case PixelFormat::R64G64B64A64_SFLOAT:
      return "PixelFormat::R64G64B64A64_SFLOAT";
    case PixelFormat::B10G11R11_UFLOAT_PACK32:
      return "PixelFormat::B10G11R11_UFLOAT_PACK32";
    case PixelFormat::E5B9G9R9_UFLOAT_PACK32:
      return "PixelFormat::E5B9G9R9_UFLOAT_PACK32";
    case PixelFormat::D16_UNORM:
      return "PixelFormat::D16_UNORM";
    case PixelFormat::X8_D24_UNORM_PACK32:
      return "PixelFormat::X8_D24_UNORM_PACK32";
    case PixelFormat::D32_SFLOAT:
      return "PixelFormat::D32_SFLOAT";
    case PixelFormat::S8_UINT:
      return "PixelFormat::S8_UINT";
    case PixelFormat::D16_UNORM_S8_UINT:
      return "PixelFormat::D16_UNORM_S8_UINT";
    case PixelFormat::D24_UNORM_S8_UINT:
      return "PixelFormat::D24_UNORM_S8_UINT";
    case PixelFormat::D32_SFLOAT_S8_UINT:
      return "PixelFormat::D32_SFLOAT_S8_UINT";
    case PixelFormat::BC1_RGB_UNORM_BLOCK:
      return "PixelFormat::BC1_RGB_UNORM_BLOCK";
    case PixelFormat::BC1_RGB_SRGB_BLOCK:
      return "PixelFormat::BC1_RGB_SRGB_BLOCK";
    case PixelFormat::BC1_RGBA_UNORM_BLOCK:
      return "PixelFormat::BC1_RGBA_UNORM_BLOCK";
    case PixelFormat::BC1_RGBA_SRGB_BLOCK:
      return "PixelFormat::BC1_RGBA_SRGB_BLOCK";
    case PixelFormat::BC2_UNORM_BLOCK:
      return "PixelFormat::BC2_UNORM_BLOCK";
    case PixelFormat::BC2_SRGB_BLOCK:
      return "PixelFormat::BC2_SRGB_BLOCK";
    case PixelFormat::BC3_UNORM_BLOCK:
      return "PixelFormat::BC3_UNORM_BLOCK";
    case PixelFormat::BC3_SRGB_BLOCK:
      return "PixelFormat::BC3_SRGB_BLOCK";
    case PixelFormat::BC4_UNORM_BLOCK:
      return "PixelFormat::BC4_UNORM_BLOCK";
    case PixelFormat::BC4_SNORM_BLOCK:
      return "PixelFormat::BC4_SNORM_BLOCK";
    case PixelFormat::BC5_UNORM_BLOCK:
      return "PixelFormat::BC5_UNORM_BLOCK";
    case PixelFormat::BC5_SNORM_BLOCK:
      return "PixelFormat::BC5_SNORM_BLOCK";
    case PixelFormat::BC6H_UFLOAT_BLOCK:
      return "PixelFormat::BC6H_UFLOAT_BLOCK";
    case PixelFormat::BC6H_SFLOAT_BLOCK:
      return "PixelFormat::BC6H_SFLOAT_BLOCK";
    case PixelFormat::BC7_UNORM_BLOCK:
      return "PixelFormat::BC7_UNORM_BLOCK";
    case PixelFormat::BC7_SRGB_BLOCK:
      return "PixelFormat::BC7_SRGB_BLOCK";
    case PixelFormat::ETC2_R8G8B8_UNORM_BLOCK:
      return "PixelFormat::ETC2_R8G8B8_UNORM_BLOCK";
    case PixelFormat::ETC2_R8G8B8_SRGB_BLOCK:
      return "PixelFormat::ETC2_R8G8B8_SRGB_BLOCK";
    case PixelFormat::ETC2_R8G8B8A1_UNORM_BLOCK:
      return "PixelFormat::ETC2_R8G8B8A1_UNORM_BLOCK";
    case PixelFormat::ETC2_R8G8B8A1_SRGB_BLOCK:
      return "PixelFormat::ETC2_R8G8B8A1_SRGB_BLOCK";
    case PixelFormat::ETC2_R8G8B8A8_UNORM_BLOCK:
      return "PixelFormat::ETC2_R8G8B8A8_UNORM_BLOCK";
    case PixelFormat::ETC2_R8G8B8A8_SRGB_BLOCK:
      return "PixelFormat::ETC2_R8G8B8A8_SRGB_BLOCK";
    case PixelFormat::EAC_R11_UNORM_BLOCK:
      return "PixelFormat::EAC_R11_UNORM_BLOCK";
    case PixelFormat::EAC_R11_SNORM_BLOCK:
      return "PixelFormat::EAC_R11_SNORM_BLOCK";
    case PixelFormat::EAC_R11G11_UNORM_BLOCK:
      return "PixelFormat::EAC_R11G11_UNORM_BLOCK";
    case PixelFormat::EAC_R11G11_SNORM_BLOCK:
      return "PixelFormat::EAC_R11G11_SNORM_BLOCK";
    case PixelFormat::ASTC_4x4_UNORM_BLOCK:
      return "PixelFormat::ASTC_4x4_UNORM_BLOCK";
    case PixelFormat::ASTC_4x4_SRGB_BLOCK:
      return "PixelFormat::ASTC_4x4_SRGB_BLOCK";
    case PixelFormat::ASTC_5x4_UNORM_BLOCK:
      return "PixelFormat::ASTC_5x4_UNORM_BLOCK";
    case PixelFormat::ASTC_5x4_SRGB_BLOCK:
      return "PixelFormat::ASTC_5x4_SRGB_BLOCK";
    case PixelFormat::ASTC_5x5_UNORM_BLOCK:
      return "PixelFormat::ASTC_5x5_UNORM_BLOCK";
    case PixelFormat::ASTC_5x5_SRGB_BLOCK:
      return "PixelFormat::ASTC_5x5_SRGB_BLOCK";
    case PixelFormat::ASTC_6x5_UNORM_BLOCK:
      return "PixelFormat::ASTC_6x5_UNORM_BLOCK";
    case PixelFormat::ASTC_6x5_SRGB_BLOCK:
      return "PixelFormat::ASTC_6x5_SRGB_BLOCK";
    case PixelFormat::ASTC_6x6_UNORM_BLOCK:
      return "PixelFormat::ASTC_6x6_UNORM_BLOCK";
    case PixelFormat::ASTC_6x6_SRGB_BLOCK:
      return "PixelFormat::ASTC_6x6_SRGB_BLOCK";
    case PixelFormat::ASTC_8x5_UNORM_BLOCK:
      return "PixelFormat::ASTC_8x5_UNORM_BLOCK";
    case PixelFormat::ASTC_8x5_SRGB_BLOCK:
      return "PixelFormat::ASTC_8x5_SRGB_BLOCK";
    case PixelFormat::ASTC_8x6_UNORM_BLOCK:
      return "PixelFormat::ASTC_8x6_UNORM_BLOCK";
    case PixelFormat::ASTC_8x6_SRGB_BLOCK:
      return "PixelFormat::ASTC_8x6_SRGB_BLOCK";
    case PixelFormat::ASTC_8x8_UNORM_BLOCK:
      return "PixelFormat::ASTC_8x8_UNORM_BLOCK";
    case PixelFormat::ASTC_8x8_SRGB_BLOCK:
      return "PixelFormat::ASTC_8x8_SRGB_BLOCK";
    case PixelFormat::ASTC_10x5_UNORM_BLOCK:
      return "PixelFormat::ASTC_10x5_UNORM_BLOCK";
    case PixelFormat::ASTC_10x5_SRGB_BLOCK:
      return "PixelFormat::ASTC_10x5_SRGB_BLOCK";
    case PixelFormat::ASTC_10x6_UNORM_BLOCK:
      return "PixelFormat::ASTC_10x6_UNORM_BLOCK";
    case PixelFormat::ASTC_10x6_SRGB_BLOCK:
      return "PixelFormat::ASTC_10x6_SRGB_BLOCK";
    case PixelFormat::ASTC_10x8_UNORM_BLOCK:
      return "PixelFormat::ASTC_10x8_UNORM_BLOCK";
    case PixelFormat::ASTC_10x8_SRGB_BLOCK:
      return "PixelFormat::ASTC_10x8_SRGB_BLOCK";
    case PixelFormat::ASTC_10x10_UNORM_BLOCK:
      return "PixelFormat::ASTC_10x10_UNORM_BLOCK";
    case PixelFormat::ASTC_10x10_SRGB_BLOCK:
      return "PixelFormat::ASTC_10x10_SRGB_BLOCK";
    case PixelFormat::ASTC_12x10_UNORM_BLOCK:
      return "PixelFormat::ASTC_12x10_UNORM_BLOCK";
    case PixelFormat::ASTC_12x10_SRGB_BLOCK:
      return "PixelFormat::ASTC_12x10_SRGB_BLOCK";
    case PixelFormat::ASTC_12x12_UNORM_BLOCK:
      return "PixelFormat::ASTC_12x12_UNORM_BLOCK";
    case PixelFormat::ASTC_12x12_SRGB_BLOCK:
      return "PixelFormat::ASTC_12x12_SRGB_BLOCK";
    default:
      return nullptr;
    }
  }

  const char* ToString(const PixelFormat& value)
  {
    const auto* const psz = TryToString(value);
    return psz != nullptr ? psz : "Unknown";
  }
}
