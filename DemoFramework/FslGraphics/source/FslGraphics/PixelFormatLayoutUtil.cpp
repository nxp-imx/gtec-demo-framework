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

#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/PixelFormatLayoutUtil.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    enum class SwizzleCompatibilityId
    {
      None,
      R4G4B4A4_PACK16,
      R5G6B5_PACK16,
      R5G5B5A1_PACK16,
      R8G8B8,
      R8G8B8A8,
    };


    struct PixelFormatLayoutRecord
    {
      PixelFormatLayout Layout;
      SwizzleCompatibilityId SwizzleCompatibility;

      PixelFormatLayoutRecord()
        : Layout(PixelFormatLayout::Undefined)
        , SwizzleCompatibility(SwizzleCompatibilityId::None)
      {
      }

      PixelFormatLayoutRecord(const PixelFormatLayout layout, SwizzleCompatibilityId swizzleCompatibilityId = SwizzleCompatibilityId::None)
        : Layout(layout)
        , SwizzleCompatibility(swizzleCompatibilityId)
      {
      }
    };

    PixelFormatLayoutRecord g_pixelFormatLayouts[] =
    {
      PixelFormatLayoutRecord(PixelFormatLayout::Undefined),
      PixelFormatLayoutRecord(PixelFormatLayout::R4G4),
      PixelFormatLayoutRecord(PixelFormatLayout::R4G4B4A4_PACK16, SwizzleCompatibilityId::R4G4B4A4_PACK16),
      PixelFormatLayoutRecord(PixelFormatLayout::B4G4R4A4_PACK16, SwizzleCompatibilityId::R4G4B4A4_PACK16),
      PixelFormatLayoutRecord(PixelFormatLayout::R5G6B5_PACK16, SwizzleCompatibilityId::R5G6B5_PACK16),
      PixelFormatLayoutRecord(PixelFormatLayout::B5G6R5_PACK16, SwizzleCompatibilityId::R5G6B5_PACK16),
      PixelFormatLayoutRecord(PixelFormatLayout::R5G5B5A1_PACK16, SwizzleCompatibilityId::R5G6B5_PACK16),
      PixelFormatLayoutRecord(PixelFormatLayout::B5G5R5A1_PACK16, SwizzleCompatibilityId::R5G6B5_PACK16),
      PixelFormatLayoutRecord(PixelFormatLayout::A1R5G5B5_PACK16),
      PixelFormatLayoutRecord(PixelFormatLayout::R8),
      PixelFormatLayoutRecord(PixelFormatLayout::R8G8),
      PixelFormatLayoutRecord(PixelFormatLayout::R8G8B8, SwizzleCompatibilityId::R8G8B8),
      PixelFormatLayoutRecord(PixelFormatLayout::B8G8R8, SwizzleCompatibilityId::R8G8B8),
      PixelFormatLayoutRecord(PixelFormatLayout::R8G8B8A8, SwizzleCompatibilityId::R8G8B8A8),
      PixelFormatLayoutRecord(PixelFormatLayout::B8G8R8A8, SwizzleCompatibilityId::R8G8B8A8),
      PixelFormatLayoutRecord(PixelFormatLayout::A8B8G8R8_PACK32),
      PixelFormatLayoutRecord(PixelFormatLayout::A2R10G10B10_PACK32),
      PixelFormatLayoutRecord(PixelFormatLayout::A2B10G10R10_PACK32),
      PixelFormatLayoutRecord(PixelFormatLayout::R16),
      PixelFormatLayoutRecord(PixelFormatLayout::R16G16),
      PixelFormatLayoutRecord(PixelFormatLayout::R16G16B16),
      PixelFormatLayoutRecord(PixelFormatLayout::R16G16B16A16),
      PixelFormatLayoutRecord(PixelFormatLayout::R32),
      PixelFormatLayoutRecord(PixelFormatLayout::R32G32),
      PixelFormatLayoutRecord(PixelFormatLayout::R32G32B32),
      PixelFormatLayoutRecord(PixelFormatLayout::R32G32B32A32),
      PixelFormatLayoutRecord(PixelFormatLayout::R64),
      PixelFormatLayoutRecord(PixelFormatLayout::R64G64),
      PixelFormatLayoutRecord(PixelFormatLayout::R64G64B64),
      PixelFormatLayoutRecord(PixelFormatLayout::R64G64B64A64),
      PixelFormatLayoutRecord(PixelFormatLayout::B10G11R11_PACK32),
      PixelFormatLayoutRecord(PixelFormatLayout::E5B9G9R9_PACK32),
      PixelFormatLayoutRecord(PixelFormatLayout::D16),
      PixelFormatLayoutRecord(PixelFormatLayout::X8_D24_PACK32),
      PixelFormatLayoutRecord(PixelFormatLayout::D32),
      PixelFormatLayoutRecord(PixelFormatLayout::S8),
      PixelFormatLayoutRecord(PixelFormatLayout::D16_S8),
      PixelFormatLayoutRecord(PixelFormatLayout::D24_S8),
      PixelFormatLayoutRecord(PixelFormatLayout::D32_S8),
      PixelFormatLayoutRecord(PixelFormatLayout::BC1_RGB_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::BC1_RGBA_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::BC2_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::BC3_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::BC4_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::BC5_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::BC6H_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::BC7_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ETC2_R8G8B8_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ETC2_R8G8B8A1_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ETC2_R8G8B8A8_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::EAC_R11_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::EAC_R11G11_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ASTC_4x4_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ASTC_5x4_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ASTC_5x5_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ASTC_6x5_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ASTC_6x6_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ASTC_8x5_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ASTC_8x6_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ASTC_8x8_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ASTC_10x5_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ASTC_10x6_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ASTC_10x8_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ASTC_10x10_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ASTC_12x10_BLOCK),
      PixelFormatLayoutRecord(PixelFormatLayout::ASTC_12x12_BLOCK),
    };

    static_assert(static_cast<uint32_t>(sizeof(g_pixelFormatLayouts) / sizeof(PixelFormatLayoutRecord)) == static_cast<uint32_t>(PixelFormatLayout::ENUM_ID_RANGE_SIZE), "g_pixelFormatLayouts needs to match the size of the enum range");

  }


  uint32_t PixelFormatLayoutUtil::CalcMinimumStride(const uint32_t width, const PixelFormatLayout pixelFormatLayout)
  {
    return CalcMinimumStride(width, GetBytesPerPixel(pixelFormatLayout));
  }




  uint32_t PixelFormatLayoutUtil::CalcMinimumStride(const uint32_t width, const PixelFormatLayout pixelFormatLayout, const StrideRequirement strideRequirement)
  {
    return CalcMinimumStride(width, GetBytesPerPixel(pixelFormatLayout), strideRequirement);
  }


  uint32_t PixelFormatLayoutUtil::CalcMinimumStride(const uint32_t width, const uint32_t bytesPerPixel, const StrideRequirement strideRequirement)
  {
    uint32_t minStride = width * bytesPerPixel;
    switch (strideRequirement)
    {
    case StrideRequirement::Minimum:
    case StrideRequirement::Any:
      return minStride;
    case StrideRequirement::MinimumAlign16Bit:
    case StrideRequirement::Align16Bit:
      return minStride + (minStride & 1);
    case StrideRequirement::MinimumAlign32Bit:
    case StrideRequirement::Align32Bit:
      return minStride + ((minStride & 3) == 0 ? 0 : 4 - (minStride & 3));
    default:
      throw NotSupportedException("Unknown StrideRequirement");
    }
  }


  bool PixelFormatLayoutUtil::IsValidStride(const uint32_t width, const PixelFormatLayout pixelFormatLayout, const StrideRequirement strideRequirement, const uint32_t desiredStride)
  {
    return IsValidStride(width, GetBytesPerPixel(pixelFormatLayout), strideRequirement, desiredStride);
  }


  bool PixelFormatLayoutUtil::IsValidStride(const uint32_t width, const uint32_t bytesPerPixel, const StrideRequirement strideRequirement, const uint32_t desiredStride)
  {
    const uint32_t minStride = width * bytesPerPixel;
    switch (strideRequirement)
    {
    case StrideRequirement::Any:
      return desiredStride >= minStride;
    case StrideRequirement::Minimum:
      return desiredStride == minStride;
    case StrideRequirement::MinimumAlign16Bit:
      return desiredStride == (minStride + (minStride & 1));
    case StrideRequirement::MinimumAlign32Bit:
      return desiredStride == (minStride + ((minStride & 3) == 0 ? 0 : 4 - (minStride & 3)));
    case StrideRequirement::Align16Bit:
      return (desiredStride & 1) == 0;
    case StrideRequirement::Align32Bit:
      return (desiredStride & 3) == 0;
    default:
      throw NotSupportedException("Unknown StrideRequirement");
    }
  }


  bool PixelFormatLayoutUtil::IsSwizzleCompatible(const PixelFormatLayout lhs, const PixelFormatLayout rhs)
  {
    const auto srcId = GetId(lhs);
    const auto dstId = GetId(rhs);
    if (srcId >= static_cast<uint32_t>(PixelFormatLayout::ENUM_ID_RANGE_SIZE) || dstId >= static_cast<uint32_t>(PixelFormatLayout::ENUM_ID_RANGE_SIZE))
      return false;

    return g_pixelFormatLayouts[srcId].SwizzleCompatibility == g_pixelFormatLayouts[dstId].SwizzleCompatibility;
  }


  PixelFormatLayout PixelFormatLayoutUtil::TryGetPixelFormatLayoutById(const uint32_t formatId)
  {
    return (formatId < static_cast<uint32_t>(PixelFormatLayout::ENUM_ID_RANGE_SIZE)) ? g_pixelFormatLayouts[formatId].Layout : PixelFormatLayout::Undefined;
  }

  // Due to lack of constexpr we use this nasty macro
  // So because of the macros be sure to keep all real code above this so you dont get any interference from them
#define LOCAL_GET_PIXELFORMATLAYOUT_ID(X) ((static_cast<uint32_t>((X)) & static_cast<uint32_t>(PixelFormatLayoutFlags::BIT_MASK_FORMAT_ID)) - static_cast<uint32_t>(PixelFormatLayout::ENUM_ID_BEGIN_RANGE))
#define LOCAL_ERROR_MESSAGE "The index did not match our assumption"

  // Some cut and pasted verification checks just to be 100% on the safe side and its a compile time check so its fine
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::Undefined) == 0, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R4G4) == 1, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R4G4B4A4_PACK16) == 2, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::B4G4R4A4_PACK16) == 3, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R5G6B5_PACK16) == 4, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::B5G6R5_PACK16) == 5, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R5G5B5A1_PACK16) == 6, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::B5G5R5A1_PACK16) == 7, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::A1R5G5B5_PACK16) == 8, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R8) == 9, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R8G8) == 10, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R8G8B8) == 11, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::B8G8R8) == 12, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R8G8B8A8) == 13, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::B8G8R8A8) == 14, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::A8B8G8R8_PACK32) == 15, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::A2R10G10B10_PACK32) == 16, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::A2B10G10R10_PACK32) == 17, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R16) == 18, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R16G16) == 19, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R16G16B16) == 20, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R16G16B16A16) == 21, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R32) == 22, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R32G32) == 23, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R32G32B32) == 24, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R32G32B32A32) == 25, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R64) == 26, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R64G64) == 27, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R64G64B64) == 28, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::R64G64B64A64) == 29, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::B10G11R11_PACK32) == 30, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::E5B9G9R9_PACK32) == 31, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::D16) == 32, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::X8_D24_PACK32) == 33, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::D32) == 34, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::S8) == 35, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::D16_S8) == 36, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::D24_S8) == 37, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::D32_S8) == 38, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::BC1_RGB_BLOCK) == 39, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::BC1_RGBA_BLOCK) == 40, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::BC2_BLOCK) == 41, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::BC3_BLOCK) == 42, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::BC4_BLOCK) == 43, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::BC5_BLOCK) == 44, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::BC6H_BLOCK) == 45, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::BC7_BLOCK) == 46, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ETC2_R8G8B8_BLOCK) == 47, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ETC2_R8G8B8A1_BLOCK) == 48, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ETC2_R8G8B8A8_BLOCK) == 49, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::EAC_R11_BLOCK) == 50, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::EAC_R11G11_BLOCK) == 51, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ASTC_4x4_BLOCK) == 52, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ASTC_5x4_BLOCK) == 53, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ASTC_5x5_BLOCK) == 54, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ASTC_6x5_BLOCK) == 55, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ASTC_6x6_BLOCK) == 56, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ASTC_8x5_BLOCK) == 57, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ASTC_8x6_BLOCK) == 58, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ASTC_8x8_BLOCK) == 59, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ASTC_10x5_BLOCK) == 60, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ASTC_10x6_BLOCK) == 61, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ASTC_10x8_BLOCK) == 62, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ASTC_10x10_BLOCK) == 63, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ASTC_12x10_BLOCK) == 64, LOCAL_ERROR_MESSAGE);
  static_assert(LOCAL_GET_PIXELFORMATLAYOUT_ID(PixelFormatLayout::ASTC_12x12_BLOCK) == 65, LOCAL_ERROR_MESSAGE);
}
