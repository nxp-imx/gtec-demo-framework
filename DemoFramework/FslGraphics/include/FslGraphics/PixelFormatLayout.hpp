#ifndef FSLGRAPHICS_PIXELFORMATLAYOUT_HPP
#define FSLGRAPHICS_PIXELFORMATLAYOUT_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/PixelFormatLayoutFlags.hpp>

namespace Fsl
{
  //! @brief This represents a core pixel format memory layout without all the extra data for how to interpret it
  enum class PixelFormatLayout
  {
    // NOLINTNEXTLINE(readability-identifier-naming)
    Undefined = 0,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R4G4 = 1 | PixelFormatLayoutFlags::BytesPerPixel1,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R4G4B4A4_PACK16 = 2 | PixelFormatLayoutFlags::BytesPerPixel2 | PixelFormatLayoutFlags::Packed,
    // NOLINTNEXTLINE(readability-identifier-naming)
    B4G4R4A4_PACK16 = 3 | PixelFormatLayoutFlags::BytesPerPixel2 | PixelFormatLayoutFlags::Packed,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R5G6B5_PACK16 = 4 | PixelFormatLayoutFlags::BytesPerPixel2 | PixelFormatLayoutFlags::Packed,
    // NOLINTNEXTLINE(readability-identifier-naming)
    B5G6R5_PACK16 = 5 | PixelFormatLayoutFlags::BytesPerPixel2 | PixelFormatLayoutFlags::Packed,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R5G5B5A1_PACK16 = 6 | PixelFormatLayoutFlags::BytesPerPixel2 | PixelFormatLayoutFlags::Packed,
    // NOLINTNEXTLINE(readability-identifier-naming)
    B5G5R5A1_PACK16 = 7 | PixelFormatLayoutFlags::BytesPerPixel2 | PixelFormatLayoutFlags::Packed,
    // NOLINTNEXTLINE(readability-identifier-naming)
    A1R5G5B5_PACK16 = 8 | PixelFormatLayoutFlags::BytesPerPixel2 | PixelFormatLayoutFlags::Packed,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8 = 9 | PixelFormatLayoutFlags::BytesPerPixel1,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8 = 10 | PixelFormatLayoutFlags::BytesPerPixel2,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8 = 11 | PixelFormatLayoutFlags::BytesPerPixel3,
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8 = 12 | PixelFormatLayoutFlags::BytesPerPixel3,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8A8 = 13 | PixelFormatLayoutFlags::BytesPerPixel4,
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8A8 = 14 | PixelFormatLayoutFlags::BytesPerPixel4,
    // NOLINTNEXTLINE(readability-identifier-naming)
    A8B8G8R8_PACK32 = 15 | PixelFormatLayoutFlags::BytesPerPixel4 | PixelFormatLayoutFlags::Packed,
    // NOLINTNEXTLINE(readability-identifier-naming)
    A2R10G10B10_PACK32 = 16 | PixelFormatLayoutFlags::BytesPerPixel4 | PixelFormatLayoutFlags::Packed,
    // NOLINTNEXTLINE(readability-identifier-naming)
    A2B10G10R10_PACK32 = 17 | PixelFormatLayoutFlags::BytesPerPixel4 | PixelFormatLayoutFlags::Packed,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16 = 18 | PixelFormatLayoutFlags::BytesPerPixel2,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16 = 19 | PixelFormatLayoutFlags::BytesPerPixel4,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16 = 20 | PixelFormatLayoutFlags::BytesPerPixel6,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16A16 = 21 | PixelFormatLayoutFlags::BytesPerPixel8,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32 = 22 | PixelFormatLayoutFlags::BytesPerPixel4,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32G32 = 23 | PixelFormatLayoutFlags::BytesPerPixel8,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32G32B32 = 24 | PixelFormatLayoutFlags::BytesPerPixel12,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32G32B32A32 = 25 | PixelFormatLayoutFlags::BytesPerPixel16,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64 = 26 | PixelFormatLayoutFlags::BytesPerPixel8,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64G64 = 27 | PixelFormatLayoutFlags::BytesPerPixel16,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64G64B64 = 28 | PixelFormatLayoutFlags::BytesPerPixel24,
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64G64B64A64 = 29 | PixelFormatLayoutFlags::BytesPerPixel32,
    // NOLINTNEXTLINE(readability-identifier-naming)
    B10G11R11_PACK32 = 30 | PixelFormatLayoutFlags::BytesPerPixel4 | PixelFormatLayoutFlags::Packed,
    // NOLINTNEXTLINE(readability-identifier-naming)
    E5B9G9R9_PACK32 = 31 | PixelFormatLayoutFlags::BytesPerPixel4 | PixelFormatLayoutFlags::Packed,
    // NOLINTNEXTLINE(readability-identifier-naming)
    D16 = 32 | PixelFormatLayoutFlags::BytesPerPixel2,
    // NOLINTNEXTLINE(readability-identifier-naming)
    X8_D24_PACK32 = 33 | PixelFormatLayoutFlags::BytesPerPixel4 | PixelFormatLayoutFlags::Packed,
    // NOLINTNEXTLINE(readability-identifier-naming)
    D32 = 34 | PixelFormatLayoutFlags::BytesPerPixel4,
    // NOLINTNEXTLINE(readability-identifier-naming)
    S8 = 35 | PixelFormatLayoutFlags::BytesPerPixel1,
    // NOLINTNEXTLINE(readability-identifier-naming)
    D16_S8 = 36 | PixelFormatLayoutFlags::BytesPerPixel3,
    // NOLINTNEXTLINE(readability-identifier-naming)
    D24_S8 = 37 | PixelFormatLayoutFlags::BytesPerPixel4,
    // NOLINTNEXTLINE(readability-identifier-naming)
    D32_S8 = 38 | PixelFormatLayoutFlags::BytesPerPixel5,
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC1_RGB_BLOCK = 39 | PixelFormatLayoutFlags::CS_BC1_RGB,
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC1_RGBA_BLOCK = 40 | PixelFormatLayoutFlags::CS_BC1_RGBA,
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC2_BLOCK = 41 | PixelFormatLayoutFlags::CS_BC2,
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC3_BLOCK = 42 | PixelFormatLayoutFlags::CS_BC3,
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC4_BLOCK = 43 | PixelFormatLayoutFlags::CS_BC4,
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC5_BLOCK = 44 | PixelFormatLayoutFlags::CS_BC5,
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC6H_BLOCK = 45 | PixelFormatLayoutFlags::CS_BC6H,
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC7_BLOCK = 46 | PixelFormatLayoutFlags::CS_BC7,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ETC2_R8G8B8_BLOCK = 47 | PixelFormatLayoutFlags::CS_ETC2_R8G8B8,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ETC2_R8G8B8A1_BLOCK = 48 | PixelFormatLayoutFlags::CS_ETC2_R8G8B8A1,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ETC2_R8G8B8A8_BLOCK = 49 | PixelFormatLayoutFlags::CS_ETC2_R8G8B8A8,
    // NOLINTNEXTLINE(readability-identifier-naming)
    EAC_R11_BLOCK = 50 | PixelFormatLayoutFlags::CS_EAC_R11,
    // NOLINTNEXTLINE(readability-identifier-naming)
    EAC_R11G11_BLOCK = 51 | PixelFormatLayoutFlags::CS_EAC_R11G11,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_4x4_BLOCK = 52 | PixelFormatLayoutFlags::CS_ASTC_4X4,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_5x4_BLOCK = 53 | PixelFormatLayoutFlags::CS_ASTC_5X4,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_5x5_BLOCK = 54 | PixelFormatLayoutFlags::CS_ASTC_5X5,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_6x5_BLOCK = 55 | PixelFormatLayoutFlags::CS_ASTC_6X5,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_6x6_BLOCK = 56 | PixelFormatLayoutFlags::CS_ASTC_6X6,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_8x5_BLOCK = 57 | PixelFormatLayoutFlags::CS_ASTC_8X5,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_8x6_BLOCK = 58 | PixelFormatLayoutFlags::CS_ASTC_8X6,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_8x8_BLOCK = 59 | PixelFormatLayoutFlags::CS_ASTC_8X8,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_10x5_BLOCK = 60 | PixelFormatLayoutFlags::CS_ASTC_10X5,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_10x6_BLOCK = 61 | PixelFormatLayoutFlags::CS_ASTC_10X6,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_10x8_BLOCK = 62 | PixelFormatLayoutFlags::CS_ASTC_10X8,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_10x10_BLOCK = 63 | PixelFormatLayoutFlags::CS_ASTC_10X10,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_12x10_BLOCK = 64 | PixelFormatLayoutFlags::CS_ASTC_12X10,
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_12x12_BLOCK = 65 | PixelFormatLayoutFlags::CS_ASTC_12X12,

    //! -- Helper values --

    //! The first valid value in the common range
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_ID_BEGIN_RANGE = (Undefined & PixelFormatLayoutFlags::BIT_MASK_FORMAT_ID),
    //! The last valid value in the common range
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_ID_END_RANGE = (ASTC_12x12_BLOCK & PixelFormatLayoutFlags::BIT_MASK_FORMAT_ID),
    //! The total number of entries in the common range
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_ID_RANGE_SIZE = (ENUM_ID_END_RANGE - ENUM_ID_BEGIN_RANGE + 1),
  };
}

#endif
