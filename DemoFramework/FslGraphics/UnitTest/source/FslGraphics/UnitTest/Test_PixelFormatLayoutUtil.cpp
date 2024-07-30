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
#include <FslGraphics/PixelFormatLayoutUtil.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using Test_PixelFormatLayoutUtil = TestFixtureFslGraphics;
}


TEST(Test_PixelFormatLayoutUtil, GetBytesPerPixel)
{
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::Undefined), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R4G4), 1u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R4G4B4A4_PACK16), 2u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::B4G4R4A4_PACK16), 2u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R5G6B5_PACK16), 2u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::B5G6R5_PACK16), 2u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R5G5B5A1_PACK16), 2u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::B5G5R5A1_PACK16), 2u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::A1R5G5B5_PACK16), 2u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R8), 1u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R8G8), 2u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R8G8B8), 3u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::B8G8R8), 3u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R8G8B8A8), 4u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::B8G8R8A8), 4u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::A8B8G8R8_PACK32), 4u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::A2R10G10B10_PACK32), 4u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::A2B10G10R10_PACK32), 4u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R16), 2u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R16G16), 4u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R16G16B16), 6u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R16G16B16A16), 8u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R32), 4u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R32G32), 8u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R32G32B32), 12u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R32G32B32A32), 16u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R64), 8u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R64G64), 16u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R64G64B64), 24u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::R64G64B64A64), 32u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::B10G11R11_PACK32), 4u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::E5B9G9R9_PACK32), 4u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::D16), 2u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::X8_D24_PACK32), 4u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::D32), 4u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::S8), 1u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::D16_S8), 3u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::D24_S8), 4u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::D32_S8), 5u);
}

TEST(Test_PixelFormatLayoutUtil, GetBytesPerPixel_Compressed)
{
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::BC1_RGB_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::BC1_RGBA_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::BC2_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::BC3_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::BC4_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::BC5_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::BC6H_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::BC7_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ETC2_R8G8B8_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ETC2_R8G8B8A1_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ETC2_R8G8B8A8_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::EAC_R11_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::EAC_R11G11_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ASTC_4x4_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ASTC_5x4_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ASTC_5x5_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ASTC_6x5_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ASTC_6x6_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ASTC_8x5_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ASTC_8x6_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ASTC_8x8_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ASTC_10x5_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ASTC_10x6_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ASTC_10x8_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ASTC_10x10_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ASTC_12x10_BLOCK), 0u);
  EXPECT_EQ(PixelFormatLayoutUtil::GetBytesPerPixel(PixelFormatLayout::ASTC_12x12_BLOCK), 0u);
}

// PixelFormatLayout::Undefined
// PixelFormatLayout::R4G4
// PixelFormatLayout::R4G4B4A4_PACK16
// PixelFormatLayout::B4G4R4A4_PACK16
// PixelFormatLayout::R5G6B5_PACK16
// PixelFormatLayout::B5G6R5_PACK16
// PixelFormatLayout::R5G5B5A1_PACK16
// PixelFormatLayout::B5G5R5A1_PACK16
// PixelFormatLayout::A1R5G5B5_PACK16
// PixelFormatLayout::R8
// PixelFormatLayout::R8G8
// PixelFormatLayout::R8G8B8
// PixelFormatLayout::B8G8R8
// PixelFormatLayout::R8G8B8A8
// PixelFormatLayout::B8G8R8A8
// PixelFormatLayout::A8B8G8R8_PACK32
// PixelFormatLayout::A2R10G10B10_PACK32
// PixelFormatLayout::A2B10G10R10_PACK32
// PixelFormatLayout::R16
// PixelFormatLayout::R16G16
// PixelFormatLayout::R16G16B16
// PixelFormatLayout::R16G16B16A16
// PixelFormatLayout::R32
// PixelFormatLayout::R32G32
// PixelFormatLayout::R32G32B32
// PixelFormatLayout::R32G32B32A32
// PixelFormatLayout::R64
// PixelFormatLayout::R64G64
// PixelFormatLayout::R64G64B64
// PixelFormatLayout::R64G64B64A64
// PixelFormatLayout::B10G11R11_PACK32
// PixelFormatLayout::E5B9G9R9_PACK32
// PixelFormatLayout::D16
// PixelFormatLayout::X8_D24_PACK32
// PixelFormatLayout::D32
// PixelFormatLayout::S8
// PixelFormatLayout::D16_S8
// PixelFormatLayout::D24_S8
// PixelFormatLayout::D32_S8
// PixelFormatLayout::BC1_RGB_BLOCK
// PixelFormatLayout::BC1_RGBA_BLOCK
// PixelFormatLayout::BC2_BLOCK
// PixelFormatLayout::BC3_BLOCK
// PixelFormatLayout::BC4_BLOCK
// PixelFormatLayout::BC5_BLOCK
// PixelFormatLayout::BC6H_BLOCK
// PixelFormatLayout::BC7_BLOCK
// PixelFormatLayout::ETC2_R8G8B8_BLOCK
// PixelFormatLayout::ETC2_R8G8B8A1_BLOCK
// PixelFormatLayout::ETC2_R8G8B8A8_BLOCK
// PixelFormatLayout::EAC_R11_BLOCK
// PixelFormatLayout::EAC_R11G11_BLOCK
// PixelFormatLayout::ASTC_4x4_BLOCK
// PixelFormatLayout::ASTC_5x4_BLOCK
// PixelFormatLayout::ASTC_5x5_BLOCK
// PixelFormatLayout::ASTC_6x5_BLOCK
// PixelFormatLayout::ASTC_6x6_BLOCK
// PixelFormatLayout::ASTC_8x5_BLOCK
// PixelFormatLayout::ASTC_8x6_BLOCK
// PixelFormatLayout::ASTC_8x8_BLOCK
// PixelFormatLayout::ASTC_10x5_BLOCK
// PixelFormatLayout::ASTC_10x6_BLOCK
// PixelFormatLayout::ASTC_10x8_BLOCK
// PixelFormatLayout::ASTC_10x10_BLOCK
// PixelFormatLayout::ASTC_12x10_BLOCK
// PixelFormatLayout::ASTC_12x12_BLOCK
