/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using Test_PixelFormatUtil = TestFixtureFslGraphics;
}


TEST(Test_PixelFormatUtil, GetBytesPerPixel)
{
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::Undefined), 0u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R4G4_UNORM_PACK8), 1u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R4G4B4A4_UNORM_PACK16), 2u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B4G4R4A4_UNORM_PACK16), 2u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R5G6B5_UNORM_PACK16), 2u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B5G6R5_UNORM_PACK16), 2u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R5G5B5A1_UNORM_PACK16), 2u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B5G5R5A1_UNORM_PACK16), 2u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A1R5G5B5_UNORM_PACK16), 2u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8_UNORM), 1u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8_SNORM), 1u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8_USCALED), 1u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8_SSCALED), 1u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8_UINT), 1u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8_SINT), 1u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8_SRGB), 1u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8_UNORM), 2u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8_SNORM), 2u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8_USCALED), 2u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8_SSCALED), 2u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8_UINT), 2u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8_SINT), 2u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8_SRGB), 2u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8B8_UNORM), 3u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8B8_SNORM), 3u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8B8_USCALED), 3u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8B8_SSCALED), 3u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8B8_UINT), 3u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8B8_SINT), 3u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8B8_SRGB), 3u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B8G8R8_UNORM), 3u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B8G8R8_SNORM), 3u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B8G8R8_USCALED), 3u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B8G8R8_SSCALED), 3u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B8G8R8_UINT), 3u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B8G8R8_SINT), 3u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B8G8R8_SRGB), 3u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8B8A8_UNORM), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8B8A8_SNORM), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8B8A8_USCALED), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8B8A8_SSCALED), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8B8A8_UINT), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8B8A8_SINT), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R8G8B8A8_SRGB), 4u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B8G8R8A8_UNORM), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B8G8R8A8_SNORM), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B8G8R8A8_USCALED), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B8G8R8A8_SSCALED), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B8G8R8A8_UINT), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B8G8R8A8_SINT), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B8G8R8A8_SRGB), 4u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A8B8G8R8_UNORM_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A8B8G8R8_SNORM_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A8B8G8R8_USCALED_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A8B8G8R8_SSCALED_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A8B8G8R8_UINT_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A8B8G8R8_SINT_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A8B8G8R8_SRGB_PACK32), 4u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A2R10G10B10_UNORM_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A2R10G10B10_SNORM_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A2R10G10B10_USCALED_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A2R10G10B10_SSCALED_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A2R10G10B10_UINT_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A2R10G10B10_SINT_PACK32), 4u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A2B10G10R10_UNORM_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A2B10G10R10_SNORM_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A2B10G10R10_USCALED_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A2B10G10R10_SSCALED_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A2B10G10R10_UINT_PACK32), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::A2B10G10R10_SINT_PACK32), 4u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16_UNORM), 2u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16_SNORM), 2u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16_USCALED), 2u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16_SSCALED), 2u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16_UINT), 2u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16_SINT), 2u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16_SFLOAT), 2u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16_UNORM), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16_SNORM), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16_USCALED), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16_SSCALED), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16_UINT), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16_SINT), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16_SFLOAT), 4u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16B16_UNORM), 6u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16B16_SNORM), 6u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16B16_USCALED), 6u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16B16_SSCALED), 6u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16B16_UINT), 6u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16B16_SINT), 6u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16B16_SFLOAT), 6u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16B16A16_UNORM), 8u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16B16A16_SNORM), 8u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16B16A16_USCALED), 8u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16B16A16_SSCALED), 8u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16B16A16_UINT), 8u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16B16A16_SINT), 8u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R16G16B16A16_SFLOAT), 8u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R32_UINT), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R32_SINT), 4u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R32_SFLOAT), 4u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R32G32_UINT), 8u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R32G32_SINT), 8u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R32G32_SFLOAT), 8u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R32G32B32_UINT), 12u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R32G32B32_SINT), 12u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R32G32B32_SFLOAT), 12u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R32G32B32A32_UINT), 16u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R32G32B32A32_SINT), 16u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R32G32B32A32_SFLOAT), 16u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R64_UINT), 8u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R64_SINT), 8u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R64_SFLOAT), 8u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R64G64_UINT), 16u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R64G64_SINT), 16u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R64G64_SFLOAT), 16u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R64G64B64_UINT), 24u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R64G64B64_SINT), 24u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R64G64B64_SFLOAT), 24u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R64G64B64A64_UINT), 32u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R64G64B64A64_SINT), 32u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::R64G64B64A64_SFLOAT), 32u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::B10G11R11_UFLOAT_PACK32), 4u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::E5B9G9R9_UFLOAT_PACK32), 4u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::D16_UNORM), 2u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::X8_D24_UNORM_PACK32), 4u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::D32_SFLOAT), 4u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::S8_UINT), 1u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::D16_UNORM_S8_UINT), 3u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::D24_UNORM_S8_UINT), 4u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::D32_SFLOAT_S8_UINT), 5u);
}

TEST(Test_PixelFormatUtil, GetBytesPerPixel_Compressed)
{
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::Undefined), 0u);

  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC1_RGB_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC1_RGB_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC1_RGBA_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC1_RGBA_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC2_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC2_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC3_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC3_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC4_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC4_SNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC5_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC5_SNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC6H_UFLOAT_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC6H_SFLOAT_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC7_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::BC7_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ETC2_R8G8B8_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ETC2_R8G8B8_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ETC2_R8G8B8A1_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ETC2_R8G8B8A1_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ETC2_R8G8B8A8_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ETC2_R8G8B8A8_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::EAC_R11_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::EAC_R11_SNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::EAC_R11G11_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::EAC_R11G11_SNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_4x4_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_4x4_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_5x4_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_5x4_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_5x5_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_5x5_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_6x5_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_6x5_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_6x6_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_6x6_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_8x5_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_8x5_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_8x6_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_8x6_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_8x8_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_8x8_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_10x5_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_10x5_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_10x6_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_10x6_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_10x8_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_10x8_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_10x10_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_10x10_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_12x10_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_12x10_SRGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_12x12_UNORM_BLOCK), 0u);
  EXPECT_EQ(PixelFormatUtil::GetBytesPerPixel(PixelFormat::ASTC_12x12_SRGB_BLOCK), 0u);
}


// PixelFormat::R4G4_UNORM_PACK8

// PixelFormat::R4G4B4A4_UNORM_PACK16

// PixelFormat::B4G4R4A4_UNORM_PACK16

// PixelFormat::R5G6B5_UNORM_PACK16

// PixelFormat::B5G6R5_UNORM_PACK16

// PixelFormat::R5G5B5A1_UNORM_PACK16

// PixelFormat::B5G5R5A1_UNORM_PACK16

// PixelFormat::A1R5G5B5_UNORM_PACK16

// PixelFormat::R8_UNORM
// PixelFormat::R8_SNORM
// PixelFormat::R8_USCALED
// PixelFormat::R8_SSCALED
// PixelFormat::R8_UINT
// PixelFormat::R8_SINT
// PixelFormat::R8_SRGB

// PixelFormat::R8G8_UNORM
// PixelFormat::R8G8_SNORM
// PixelFormat::R8G8_USCALED
// PixelFormat::R8G8_SSCALED
// PixelFormat::R8G8_UINT
// PixelFormat::R8G8_SINT
// PixelFormat::R8G8_SRGB

// PixelFormat::R8G8B8_UNORM
// PixelFormat::R8G8B8_SNORM
// PixelFormat::R8G8B8_USCALED
// PixelFormat::R8G8B8_SSCALED
// PixelFormat::R8G8B8_UINT
// PixelFormat::R8G8B8_SINT
// PixelFormat::R8G8B8_SRGB

// PixelFormat::B8G8R8_UNORM
// PixelFormat::B8G8R8_SNORM
// PixelFormat::B8G8R8_USCALED
// PixelFormat::B8G8R8_SSCALED
// PixelFormat::B8G8R8_UINT
// PixelFormat::B8G8R8_SINT
// PixelFormat::B8G8R8_SRGB

// PixelFormat::R8G8B8A8_UNORM
// PixelFormat::R8G8B8A8_SNORM
// PixelFormat::R8G8B8A8_USCALED
// PixelFormat::R8G8B8A8_SSCALED
// PixelFormat::R8G8B8A8_UINT
// PixelFormat::R8G8B8A8_SINT
// PixelFormat::R8G8B8A8_SRGB

// PixelFormat::B8G8R8A8_UNORM
// PixelFormat::B8G8R8A8_SNORM
// PixelFormat::B8G8R8A8_USCALED
// PixelFormat::B8G8R8A8_SSCALED
// PixelFormat::B8G8R8A8_UINT
// PixelFormat::B8G8R8A8_SINT
// PixelFormat::B8G8R8A8_SRGB

// PixelFormat::A8B8G8R8_UNORM_PACK32
// PixelFormat::A8B8G8R8_SNORM_PACK32
// PixelFormat::A8B8G8R8_USCALED_PACK32
// PixelFormat::A8B8G8R8_SSCALED_PACK32
// PixelFormat::A8B8G8R8_UINT_PACK32
// PixelFormat::A8B8G8R8_SINT_PACK32
// PixelFormat::A8B8G8R8_SRGB_PACK32

// PixelFormat::A2R10G10B10_UNORM_PACK32
// PixelFormat::A2R10G10B10_SNORM_PACK32
// PixelFormat::A2R10G10B10_USCALED_PACK32
// PixelFormat::A2R10G10B10_SSCALED_PACK32
// PixelFormat::A2R10G10B10_UINT_PACK32
// PixelFormat::A2R10G10B10_SINT_PACK32

// PixelFormat::A2B10G10R10_UNORM_PACK32
// PixelFormat::A2B10G10R10_SNORM_PACK32
// PixelFormat::A2B10G10R10_USCALED_PACK32
// PixelFormat::A2B10G10R10_SSCALED_PACK32
// PixelFormat::A2B10G10R10_UINT_PACK32
// PixelFormat::A2B10G10R10_SINT_PACK32

// PixelFormat::R16_UNORM
// PixelFormat::R16_SNORM
// PixelFormat::R16_USCALED
// PixelFormat::R16_SSCALED
// PixelFormat::R16_UINT
// PixelFormat::R16_SINT
// PixelFormat::R16_SFLOAT

// PixelFormat::R16G16_UNORM
// PixelFormat::R16G16_SNORM
// PixelFormat::R16G16_USCALED
// PixelFormat::R16G16_SSCALED
// PixelFormat::R16G16_UINT
// PixelFormat::R16G16_SINT
// PixelFormat::R16G16_SFLOAT

// PixelFormat::R16G16B16_UNORM
// PixelFormat::R16G16B16_SNORM
// PixelFormat::R16G16B16_USCALED
// PixelFormat::R16G16B16_SSCALED
// PixelFormat::R16G16B16_UINT
// PixelFormat::R16G16B16_SINT
// PixelFormat::R16G16B16_SFLOAT

// PixelFormat::R16G16B16A16_UNORM
// PixelFormat::R16G16B16A16_SNORM
// PixelFormat::R16G16B16A16_USCALED
// PixelFormat::R16G16B16A16_SSCALED
// PixelFormat::R16G16B16A16_UINT
// PixelFormat::R16G16B16A16_SINT
// PixelFormat::R16G16B16A16_SFLOAT

// PixelFormat::R32_UINT
// PixelFormat::R32_SINT
// PixelFormat::R32_SFLOAT

// PixelFormat::R32G32_UINT
// PixelFormat::R32G32_SINT
// PixelFormat::R32G32_SFLOAT
// PixelFormat::R32G32B32_UINT
// PixelFormat::R32G32B32_SINT
// PixelFormat::R32G32B32_SFLOAT

// PixelFormat::R32G32B32A32_UINT
// PixelFormat::R32G32B32A32_SINT
// PixelFormat::R32G32B32A32_SFLOAT

// PixelFormat::R64_UINT
// PixelFormat::R64_SINT
// PixelFormat::R64_SFLOAT

// PixelFormat::R64G64_UINT
// PixelFormat::R64G64_SINT
// PixelFormat::R64G64_SFLOAT

// PixelFormat::R64G64B64_UINT
// PixelFormat::R64G64B64_SINT
// PixelFormat::R64G64B64_SFLOAT

// PixelFormat::R64G64B64A64_UINT
// PixelFormat::R64G64B64A64_SINT
// PixelFormat::R64G64B64A64_SFLOAT

// PixelFormat::B10G11R11_UFLOAT_PACK32

// PixelFormat::E5B9G9R9_UFLOAT_PACK32

// PixelFormat::D16_UNORM

// PixelFormat::X8_D24_UNORM_PACK32

// PixelFormat::D32_SFLOAT

// PixelFormat::S8_UINT

// PixelFormat::D16_UNORM_S8_UINT

// PixelFormat::D24_UNORM_S8_UINT

// PixelFormat::D32_SFLOAT_S8_UINT

// PixelFormat::BC1_RGB_UNORM_BLOCK
// PixelFormat::BC1_RGB_SRGB_BLOCK
// PixelFormat::BC1_RGBA_UNORM_BLOCK
// PixelFormat::BC1_RGBA_SRGB_BLOCK
// PixelFormat::BC2_UNORM_BLOCK
// PixelFormat::BC2_SRGB_BLOCK
// PixelFormat::BC3_UNORM_BLOCK
// PixelFormat::BC3_SRGB_BLOCK
// PixelFormat::BC4_UNORM_BLOCK
// PixelFormat::BC4_SNORM_BLOCK
// PixelFormat::BC5_UNORM_BLOCK
// PixelFormat::BC5_SNORM_BLOCK
// PixelFormat::BC6H_UFLOAT_BLOCK
// PixelFormat::BC6H_SFLOAT_BLOCK
// PixelFormat::BC7_UNORM_BLOCK
// PixelFormat::BC7_SRGB_BLOCK
// PixelFormat::ETC2_R8G8B8_UNORM_BLOCK
// PixelFormat::ETC2_R8G8B8_SRGB_BLOCK
// PixelFormat::ETC2_R8G8B8A1_UNORM_BLOCK
// PixelFormat::ETC2_R8G8B8A1_SRGB_BLOCK
// PixelFormat::ETC2_R8G8B8A8_UNORM_BLOCK
// PixelFormat::ETC2_R8G8B8A8_SRGB_BLOCK
// PixelFormat::EAC_R11_UNORM_BLOCK
// PixelFormat::EAC_R11_SNORM_BLOCK
// PixelFormat::EAC_R11G11_UNORM_BLOCK
// PixelFormat::EAC_R11G11_SNORM_BLOCK
// PixelFormat::ASTC_4x4_UNORM_BLOCK
// PixelFormat::ASTC_4x4_SRGB_BLOCK
// PixelFormat::ASTC_5x4_UNORM_BLOCK
// PixelFormat::ASTC_5x4_SRGB_BLOCK
// PixelFormat::ASTC_5x5_UNORM_BLOCK
// PixelFormat::ASTC_5x5_SRGB_BLOCK
// PixelFormat::ASTC_6x5_UNORM_BLOCK
// PixelFormat::ASTC_6x5_SRGB_BLOCK
// PixelFormat::ASTC_6x6_UNORM_BLOCK
// PixelFormat::ASTC_6x6_SRGB_BLOCK
// PixelFormat::ASTC_8x5_UNORM_BLOCK
// PixelFormat::ASTC_8x5_SRGB_BLOCK
// PixelFormat::ASTC_8x6_UNORM_BLOCK
// PixelFormat::ASTC_8x6_SRGB_BLOCK
// PixelFormat::ASTC_8x8_UNORM_BLOCK
// PixelFormat::ASTC_8x8_SRGB_BLOCK
// PixelFormat::ASTC_10x5_UNORM_BLOCK
// PixelFormat::ASTC_10x5_SRGB_BLOCK
// PixelFormat::ASTC_10x6_UNORM_BLOCK
// PixelFormat::ASTC_10x6_SRGB_BLOCK
// PixelFormat::ASTC_10x8_UNORM_BLOCK
// PixelFormat::ASTC_10x8_SRGB_BLOCK
// PixelFormat::ASTC_10x10_UNORM_BLOCK
// PixelFormat::ASTC_10x10_SRGB_BLOCK
// PixelFormat::ASTC_12x10_UNORM_BLOCK
// PixelFormat::ASTC_12x10_SRGB_BLOCK
// PixelFormat::ASTC_12x12_UNORM_BLOCK
// PixelFormat::ASTC_12x12_SRGB_BLOCK
