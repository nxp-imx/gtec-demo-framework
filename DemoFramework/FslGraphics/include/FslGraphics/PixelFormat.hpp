#ifndef FSLGRAPHICS_PIXELFORMAT_HPP
#define FSLGRAPHICS_PIXELFORMAT_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
#include <FslGraphics/PixelFormatFlags.hpp>

namespace Fsl
{
  //! Custom pixel format enum
  //! Based on the Vulkan formats but not 100% value compatible as some extra meta data has been embedded in the "format value"
  //! This extra data is only embedded as we never intend this PixelFormat type to be used on a public API
  //! If we had to do so the extra data should be stripped and getter methods should be created instead.
  //! For now this meta data embedding is experimental.
  //!
  //! Never try to access this meta data directly!!
  //! Use the PixelFormatUtil and PixelFormatLayoutUtil helper methods
  //!
  //! See the Vulkan documentation for details
  //! Please utilize PixelFormatUtil to extract that information as it is subject to change!!!
  //! WARNING: Do not modify the order here, it is expected that the FORMAT_ID of each format matches the Vulkan values
  enum class PixelFormat
  {
    Undefined = 0,

    // NOLINTNEXTLINE(readability-identifier-naming)
    R4G4_UNORM_PACK8 = 1 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R4G4),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R4G4B4A4_UNORM_PACK16 = 2 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R4G4B4A4_PACK16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B4G4R4A4_UNORM_PACK16 = 3 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B4G4R4A4_PACK16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R5G6B5_UNORM_PACK16 = 4 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R5G6B5_PACK16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B5G6R5_UNORM_PACK16 = 5 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B5G6R5_PACK16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R5G5B5A1_UNORM_PACK16 = 6 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R5G5B5A1_PACK16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B5G5R5A1_UNORM_PACK16 = 7 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R5G5B5A1_PACK16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A1R5G5B5_UNORM_PACK16 = 8 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A1R5G5B5_PACK16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8_UNORM = 9 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8_SNORM = 10 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8_USCALED = 11 | PixelFormatFlags::NF_UScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8_SSCALED = 12 | PixelFormatFlags::NF_SScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8_UINT = 13 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8_SINT = 14 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8_SRGB = 15 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8_UNORM = 16 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8_SNORM = 17 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8_USCALED = 18 | PixelFormatFlags::NF_UScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8_SSCALED = 19 | PixelFormatFlags::NF_SScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8_UINT = 20 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8_SINT = 21 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8_SRGB = 22 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8_UNORM = 23 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8B8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8_SNORM = 24 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8B8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8_USCALED = 25 | PixelFormatFlags::NF_UScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8B8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8_SSCALED = 26 | PixelFormatFlags::NF_SScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8B8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8_UINT = 27 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8B8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8_SINT = 28 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8B8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8_SRGB = 29 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8B8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8_UNORM = 30 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B8G8R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8_SNORM = 31 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B8G8R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8_USCALED = 32 | PixelFormatFlags::NF_UScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B8G8R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8_SSCALED = 33 | PixelFormatFlags::NF_SScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B8G8R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8_UINT = 34 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B8G8R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8_SINT = 35 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B8G8R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8_SRGB = 36 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B8G8R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8A8_UNORM = 37 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8B8A8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8A8_SNORM = 38 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8B8A8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8A8_USCALED = 39 | PixelFormatFlags::NF_UScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8B8A8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8A8_SSCALED = 40 | PixelFormatFlags::NF_SScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8B8A8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8A8_UINT = 41 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8B8A8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8A8_SINT = 42 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8B8A8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R8G8B8A8_SRGB = 43 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8B8A8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8A8_UNORM = 44 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B8G8R8A8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8A8_SNORM = 45 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B8G8R8A8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8A8_USCALED = 46 | PixelFormatFlags::NF_UScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B8G8R8A8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8A8_SSCALED = 47 | PixelFormatFlags::NF_SScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B8G8R8A8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8A8_UINT = 48 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B8G8R8A8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8A8_SINT = 49 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B8G8R8A8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B8G8R8A8_SRGB = 50 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B8G8R8A8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A8B8G8R8_UNORM_PACK32 = 51 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A8B8G8R8_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A8B8G8R8_SNORM_PACK32 = 52 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A8B8G8R8_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A8B8G8R8_USCALED_PACK32 = 53 | PixelFormatFlags::NF_UScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A8B8G8R8_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A8B8G8R8_SSCALED_PACK32 = 54 | PixelFormatFlags::NF_SScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A8B8G8R8_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A8B8G8R8_UINT_PACK32 = 55 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A8B8G8R8_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A8B8G8R8_SINT_PACK32 = 56 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A8B8G8R8_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A8B8G8R8_SRGB_PACK32 = 57 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A8B8G8R8_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A2R10G10B10_UNORM_PACK32 = 58 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A2R10G10B10_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A2R10G10B10_SNORM_PACK32 = 59 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A2R10G10B10_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A2R10G10B10_USCALED_PACK32 = 60 | PixelFormatFlags::NF_UScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A2R10G10B10_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A2R10G10B10_SSCALED_PACK32 = 61 | PixelFormatFlags::NF_SScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A2R10G10B10_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A2R10G10B10_UINT_PACK32 = 62 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A2R10G10B10_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A2R10G10B10_SINT_PACK32 = 63 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A2R10G10B10_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A2B10G10R10_UNORM_PACK32 = 64 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A2B10G10R10_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A2B10G10R10_SNORM_PACK32 = 65 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A2B10G10R10_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A2B10G10R10_USCALED_PACK32 = 66 | PixelFormatFlags::NF_UScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A2B10G10R10_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A2B10G10R10_SSCALED_PACK32 = 67 | PixelFormatFlags::NF_SScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A2B10G10R10_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A2B10G10R10_UINT_PACK32 = 68 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A2B10G10R10_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    A2B10G10R10_SINT_PACK32 = 69 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::A2B10G10R10_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16_UNORM = 70 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16_SNORM = 71 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16_USCALED = 72 | PixelFormatFlags::NF_UScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16_SSCALED = 73 | PixelFormatFlags::NF_SScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16_UINT = 74 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16_SINT = 75 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16_SFLOAT = 76 | PixelFormatFlags::NF_SFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16_UNORM = 77 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16_SNORM = 78 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16_USCALED = 79 | PixelFormatFlags::NF_UScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16_SSCALED = 80 | PixelFormatFlags::NF_SScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16_UINT = 81 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16_SINT = 82 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16_SFLOAT = 83 | PixelFormatFlags::NF_SFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16_UNORM = 84 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16B16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16_SNORM = 85 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16B16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16_USCALED = 86 | PixelFormatFlags::NF_UScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16B16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16_SSCALED = 87 | PixelFormatFlags::NF_SScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16B16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16_UINT = 88 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16B16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16_SINT = 89 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16B16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16_SFLOAT = 90 | PixelFormatFlags::NF_SFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16B16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16A16_UNORM = 91 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16B16A16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16A16_SNORM = 92 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16B16A16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16A16_USCALED = 93 | PixelFormatFlags::NF_UScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16B16A16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16A16_SSCALED = 94 | PixelFormatFlags::NF_SScaled | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16B16A16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16A16_UINT = 95 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16B16A16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16A16_SINT = 96 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16B16A16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R16G16B16A16_SFLOAT = 97 | PixelFormatFlags::NF_SFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R16G16B16A16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32_UINT = 98 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32_SINT = 99 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32_SFLOAT = 100 | PixelFormatFlags::NF_SFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32G32_UINT = 101 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R32G32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32G32_SINT = 102 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R32G32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32G32_SFLOAT = 103 | PixelFormatFlags::NF_SFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R32G32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32G32B32_UINT = 104 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R32G32B32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32G32B32_SINT = 105 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R32G32B32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32G32B32_SFLOAT = 106 | PixelFormatFlags::NF_SFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R32G32B32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32G32B32A32_UINT = 107 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R32G32B32A32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32G32B32A32_SINT = 108 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R32G32B32A32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R32G32B32A32_SFLOAT = 109 | PixelFormatFlags::NF_SFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R32G32B32A32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64_UINT = 110 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R64),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64_SINT = 111 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R64),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64_SFLOAT = 112 | PixelFormatFlags::NF_SFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R64),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64G64_UINT = 113 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R64G64),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64G64_SINT = 114 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R64G64),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64G64_SFLOAT = 115 | PixelFormatFlags::NF_SFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R64G64),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64G64B64_UINT = 116 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R64G64B64),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64G64B64_SINT = 117 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R64G64B64),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64G64B64_SFLOAT = 118 | PixelFormatFlags::NF_SFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R64G64B64),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64G64B64A64_UINT = 119 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R64G64B64A64),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64G64B64A64_SINT = 120 | PixelFormatFlags::NF_SInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R64G64B64A64),
    // NOLINTNEXTLINE(readability-identifier-naming)
    R64G64B64A64_SFLOAT = 121 | PixelFormatFlags::NF_SFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R64G64B64A64),
    // NOLINTNEXTLINE(readability-identifier-naming)
    B10G11R11_UFLOAT_PACK32 = 122 | PixelFormatFlags::NF_UFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::B10G11R11_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    E5B9G9R9_UFLOAT_PACK32 = 123 | PixelFormatFlags::NF_UFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::E5B9G9R9_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    D16_UNORM = 124 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::D16),
    // NOLINTNEXTLINE(readability-identifier-naming)
    X8_D24_UNORM_PACK32 = 125 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::X8_D24_PACK32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    D32_SFLOAT = 126 | PixelFormatFlags::NF_SFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::D32),
    // NOLINTNEXTLINE(readability-identifier-naming)
    S8_UINT = 127 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::S8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    D16_UNORM_S8_UINT = 128 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::D16_S8),
    // This is a packed format even though its not tagged as such in vulkan
    // "data copied to or from the depth aspect of a VK_FORMAT_X8_D24_UNORM_PACK32 or VK_FORMAT_D24_UNORM_S8_UINT format is packed with one 32-bit
    // word per texel with the D24 value in the LSBs of the word, and undefined values in the eight MSBs.
    // NOLINTNEXTLINE(readability-identifier-naming)
    D24_UNORM_S8_UINT = 129 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::D24_S8),    //,
    // NOLINTNEXTLINE(readability-identifier-naming)
    D32_SFLOAT_S8_UINT = 130 | PixelFormatFlags::NF_UInt | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::D32_S8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC1_RGB_UNORM_BLOCK = 131 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC1_RGB_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC1_RGB_SRGB_BLOCK = 132 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC1_RGB_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC1_RGBA_UNORM_BLOCK = 133 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC1_RGBA_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC1_RGBA_SRGB_BLOCK = 134 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC1_RGBA_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC2_UNORM_BLOCK = 135 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC2_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC2_SRGB_BLOCK = 136 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC2_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC3_UNORM_BLOCK = 137 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC3_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC3_SRGB_BLOCK = 138 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC3_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC4_UNORM_BLOCK = 139 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC4_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC4_SNORM_BLOCK = 140 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC4_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC5_UNORM_BLOCK = 141 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC5_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC5_SNORM_BLOCK = 142 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC5_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC6H_UFLOAT_BLOCK = 143 | PixelFormatFlags::NF_UFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC6H_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC6H_SFLOAT_BLOCK = 144 | PixelFormatFlags::NF_SFloat | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC6H_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC7_UNORM_BLOCK = 145 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC7_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    BC7_SRGB_BLOCK = 146 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::BC7_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ETC2_R8G8B8_UNORM_BLOCK = 147 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ETC2_R8G8B8_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ETC2_R8G8B8_SRGB_BLOCK = 148 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ETC2_R8G8B8_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ETC2_R8G8B8A1_UNORM_BLOCK = 149 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ETC2_R8G8B8A1_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ETC2_R8G8B8A1_SRGB_BLOCK = 150 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ETC2_R8G8B8A1_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ETC2_R8G8B8A8_UNORM_BLOCK = 151 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ETC2_R8G8B8A8_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ETC2_R8G8B8A8_SRGB_BLOCK = 152 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ETC2_R8G8B8A8_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    EAC_R11_UNORM_BLOCK = 153 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::EAC_R11_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    EAC_R11_SNORM_BLOCK = 154 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::EAC_R11_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    EAC_R11G11_UNORM_BLOCK = 155 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::EAC_R11G11_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    EAC_R11G11_SNORM_BLOCK = 156 | PixelFormatFlags::NF_SNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::EAC_R11G11_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_4x4_UNORM_BLOCK = 157 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_4x4_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_4x4_SRGB_BLOCK = 158 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_4x4_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_5x4_UNORM_BLOCK = 159 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_5x4_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_5x4_SRGB_BLOCK = 160 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_5x4_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_5x5_UNORM_BLOCK = 161 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_5x5_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_5x5_SRGB_BLOCK = 162 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_5x5_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_6x5_UNORM_BLOCK = 163 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_6x5_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_6x5_SRGB_BLOCK = 164 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_6x5_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_6x6_UNORM_BLOCK = 165 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_6x6_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_6x6_SRGB_BLOCK = 166 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_6x6_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_8x5_UNORM_BLOCK = 167 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_8x5_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_8x5_SRGB_BLOCK = 168 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_8x5_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_8x6_UNORM_BLOCK = 169 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_8x6_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_8x6_SRGB_BLOCK = 170 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_8x6_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_8x8_UNORM_BLOCK = 171 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_8x8_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_8x8_SRGB_BLOCK = 172 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_8x8_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_10x5_UNORM_BLOCK = 173 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_10x5_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_10x5_SRGB_BLOCK = 174 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_10x5_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_10x6_UNORM_BLOCK = 175 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_10x6_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_10x6_SRGB_BLOCK = 176 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_10x6_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_10x8_UNORM_BLOCK = 177 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_10x8_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_10x8_SRGB_BLOCK = 178 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_10x8_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_10x10_UNORM_BLOCK = 179 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_10x10_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_10x10_SRGB_BLOCK = 180 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_10x10_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_12x10_UNORM_BLOCK = 181 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_12x10_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_12x10_SRGB_BLOCK = 182 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_12x10_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_12x12_UNORM_BLOCK = 183 | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_12x12_BLOCK),
    // NOLINTNEXTLINE(readability-identifier-naming)
    ASTC_12x12_SRGB_BLOCK = 184 | PixelFormatFlags::NF_Srgb | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::ASTC_12x12_BLOCK),

    //! -- Helper values --
    //! The first valid value in the common range
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_ID_BEGIN_COMMON_RANGE = (Undefined & PixelFormatFlags::BIT_MASK_FORMAT_ID),
    //! The last valid value in the common range
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_ID_END_COMMON_RANGE = (ASTC_12x12_SRGB_BLOCK & PixelFormatFlags::BIT_MASK_FORMAT_ID),
    //! The total number of entries in the common range
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_ID_RANGE_COMMON_SIZE = (ENUM_ID_END_COMMON_RANGE - ENUM_ID_BEGIN_COMMON_RANGE + 1),

    //! The first valid value in the extended range
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_ID_BEGIN_EXTENDED_RANGE = (ENUM_ID_END_COMMON_RANGE & PixelFormatFlags::BIT_MASK_FORMAT_ID) + 1,
    //! -------------------

    //! These are custom formats not supported by Vulkan, but they are useful for OpenGL ES
    //! Unlike the Vulkan formats these commonly gets converted to another format by the GPU
    // NOLINTNEXTLINE(readability-identifier-naming)
    EX_ALPHA8_UNORM =
      (ENUM_ID_BEGIN_EXTENDED_RANGE + 0) | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    EX_LUMINANCE8_UNORM =
      (ENUM_ID_BEGIN_EXTENDED_RANGE + 1) | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8),
    // NOLINTNEXTLINE(readability-identifier-naming)
    EX_LUMINANCE8_ALPHA8_UNORM =
      (ENUM_ID_BEGIN_EXTENDED_RANGE + 2) | PixelFormatFlags::NF_UNorm | FslGraphicsPixelFormatFlagsEncodeLayout(PixelFormatLayout::R8G8),


    //! -- Helper values --
    //! The last valid value in the range
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_ID_END_EXTENDED_RANGE = (EX_LUMINANCE8_ALPHA8_UNORM & PixelFormatFlags::BIT_MASK_FORMAT_ID),
    //! The total number of entries in the range
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_ID_EXTENDED_RANGE_SIZE = (ENUM_ID_END_EXTENDED_RANGE - ENUM_ID_BEGIN_EXTENDED_RANGE + 1),

    //! This covers all the enum pixel formats
    //! The first valid value in the range
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_ID_BEGIN_RANGE = ENUM_ID_BEGIN_COMMON_RANGE,
    //! The last valid value in the range
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_ID_END_RANGE = ENUM_ID_END_EXTENDED_RANGE,
    //! The total number of entries in the range
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_ID_RANGE_SIZE = (ENUM_ID_END_RANGE - ENUM_ID_BEGIN_RANGE) + 1,

    //! The max value for the enum
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_MAX = 0x7FFFFFFF
  };
}

#endif
