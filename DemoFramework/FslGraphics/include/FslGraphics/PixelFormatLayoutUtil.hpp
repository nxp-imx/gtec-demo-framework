#ifndef FSLGRAPHICS_PIXELFORMATLAYOUTUTIL_HPP
#define FSLGRAPHICS_PIXELFORMATLAYOUTUTIL_HPP
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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslGraphics/PixelChannelOrder.hpp>
#include <FslGraphics/PixelFormatLayout.hpp>
#include <FslGraphics/PixelFormatLayoutMetaData.hpp>
#include <FslGraphics/StrideRequirement.hpp>
#include <cassert>

namespace Fsl::PixelFormatLayoutUtil
{
  constexpr int32_t GetBasicEncodedLayout(const PixelFormatLayout pixelFormatLayout) noexcept;
  constexpr int32_t GetEncodedLayout1CompressionScheme(const PixelFormatLayout pixelFormatLayout) noexcept;
  constexpr int32_t GetEncodedLayout0Packed(const PixelFormatLayout pixelFormatLayout) noexcept;

  //! @brief Get the format-id of the pixel format (it basically strips all the extra meta data)
  inline constexpr uint32_t GetId(const PixelFormatLayout pixelFormatLayout) noexcept
  {
    return (static_cast<int32_t>(pixelFormatLayout) & PixelFormatLayoutFlags::BIT_MASK_FORMAT_ID);
  }

  //! @brief Lookup the bytes per pixel of the pixelFormatLayout, beware that for compressed formats this return 0
  inline constexpr uint32_t GetBytesPerPixel(const PixelFormatLayout pixelFormatLayout) noexcept
  {
    if (GetBasicEncodedLayout(pixelFormatLayout) == PixelFormatLayoutFlags::ENCODED_LAYOUT0)
    {
      return (static_cast<uint32_t>(pixelFormatLayout) & PixelFormatLayoutFlags::BIT_MASK_ENCODED_LAYOUT0_BYTES_PER_PIXEL) >>
             PixelFormatLayoutFlags::BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL;
    }

    FSLLOG3_DEBUG_WARNING("PixelFormatLayoutUtil::GetBytesPerPixel called on compressed pixel format");
    return 0;
  }

  inline constexpr uint32_t CalcMinimumStride(const PxValueU widthPx, const uint32_t bytesPerPixel) noexcept
  {
    return widthPx.Value * bytesPerPixel;
  }

  inline constexpr uint32_t CalcMinimumStride(const PxValueU widthPx, const PixelFormatLayout pixelFormatLayout) noexcept
  {
    return CalcMinimumStride(widthPx, GetBytesPerPixel(pixelFormatLayout));
  }

  inline constexpr uint32_t CalcMinimumStride(const PxSize1D widthPx, const uint32_t bytesPerPixel) noexcept
  {
    return widthPx.RawUnsignedValue() * bytesPerPixel;
  }

  inline constexpr uint32_t CalcMinimumStride(const PxSize1D widthPx, const PixelFormatLayout pixelFormatLayout) noexcept
  {
    return CalcMinimumStride(widthPx, GetBytesPerPixel(pixelFormatLayout));
  }

  uint32_t CalcMinimumStride(const PxValueU widthPx, const PixelFormatLayout pixelFormatLayout, const StrideRequirement strideRequirement);
  uint32_t CalcMinimumStride(const PxValueU widthPx, const uint32_t bytesPerPixel, const StrideRequirement strideRequirement);

  inline uint32_t CalcMinimumStride(const PxSize1D widthPx, const PixelFormatLayout pixelFormatLayout, const StrideRequirement strideRequirement)
  {
    return CalcMinimumStride(widthPx.UnsignedValue(), pixelFormatLayout, strideRequirement);
  }

  inline uint32_t CalcMinimumStride(const PxSize1D widthPx, const uint32_t bytesPerPixel, const StrideRequirement strideRequirement)
  {
    return CalcMinimumStride(widthPx.UnsignedValue(), bytesPerPixel, strideRequirement);
  }

  bool IsValidStride(const uint32_t width, const PixelFormatLayout pixelFormatLayout, const StrideRequirement strideRequirement,
                     const uint32_t desiredStride);
  bool IsValidStride(const uint32_t width, const uint32_t bytesPerPixel, const StrideRequirement strideRequirement, const uint32_t desiredStride);

  inline bool IsValidStride(const PxSize1D widthPx, const PixelFormatLayout pixelFormatLayout, const StrideRequirement strideRequirement,
                            const uint32_t desiredStride)
  {
    return IsValidStride(widthPx.RawUnsignedValue(), pixelFormatLayout, strideRequirement, desiredStride);
  }

  inline bool IsValidStride(const PxSize1D widthPx, const uint32_t bytesPerPixel, const StrideRequirement strideRequirement,
                            const uint32_t desiredStride)
  {
    return IsValidStride(widthPx.RawUnsignedValue(), bytesPerPixel, strideRequirement, desiredStride);
  }

  //! @brief Check if the formats can be converted to each other using a simple swizzle operation
  inline constexpr bool IsSwizzleCompatible(const PixelFormatLayout lhs, const PixelFormatLayout rhs) noexcept
  {
    const auto srcId = GetId(lhs);
    const auto dstId = GetId(rhs);
    if (srcId >= static_cast<uint32_t>(PixelFormatLayout::ENUM_ID_RANGE_SIZE) ||
        dstId >= static_cast<uint32_t>(PixelFormatLayout::ENUM_ID_RANGE_SIZE))
    {
      return false;
    }

    return PixelFormatLayoutMetaData::Get(srcId).SwizzleCompatibility == PixelFormatLayoutMetaData::Get(dstId).SwizzleCompatibility;
  }

  //! @brief Given a pixel format id try to lookup the pixel format enum value.
  //! @note  Returns the PixelFormat value or PixelFormat::Undefined if the lookup fails (or if asked to lookup the undefined value)
  inline constexpr PixelFormatLayout TryGetPixelFormatLayoutById(const uint32_t formatId) noexcept
  {
    return (formatId < static_cast<uint32_t>(PixelFormatLayout::ENUM_ID_RANGE_SIZE)) ? PixelFormatLayoutMetaData::Get(formatId).Layout
                                                                                     : PixelFormatLayout::Undefined;
  }

  //! @brief Check if this is considered a compressed format
  inline constexpr bool IsCompressed(const PixelFormatLayout pixelFormatLayout) noexcept
  {
    return (GetBasicEncodedLayout(pixelFormatLayout) == PixelFormatLayoutFlags::ENCODED_LAYOUT1 &&
            GetEncodedLayout1CompressionScheme(pixelFormatLayout) != PixelFormatLayoutFlags::CS_NONE);
  }

  //! @brief Check if this is considered a packed format
  inline constexpr bool IsPacked(const PixelFormatLayout pixelFormatLayout) noexcept
  {
    return (GetBasicEncodedLayout(pixelFormatLayout) == PixelFormatLayoutFlags::ENCODED_LAYOUT0 &&
            GetEncodedLayout0Packed(pixelFormatLayout) == PixelFormatLayoutFlags::Packed);
  }


  inline constexpr uint32_t GetChannelCount(const PixelFormatLayout pixelFormatLayout) noexcept
  {
    const auto srcId = GetId(pixelFormatLayout);
    return srcId < static_cast<uint32_t>(PixelFormatLayout::ENUM_ID_RANGE_SIZE) ? PixelFormatLayoutMetaData::Get(srcId).Info.GetChannelCount() : 0u;
  }


  //! @brief Check if the formats can be converted to each other using a simple swizzle operation
  inline constexpr PixelChannelOrder GetPixelChannelOrder(const PixelFormatLayout lhs) noexcept
  {
    const auto srcId = GetId(lhs);
    return srcId < static_cast<uint32_t>(PixelFormatLayout::ENUM_ID_RANGE_SIZE) ? PixelFormatLayoutMetaData::Get(srcId).ChannelOrder
                                                                                : PixelChannelOrder::Undefined;
  }

  //! @brief Check if the formats can be converted to each other using a simple swizzle operation
  inline constexpr bool HasAlphaChannel(const PixelFormatLayout lhs) noexcept
  {
    const auto srcId = GetId(lhs);
    return srcId < static_cast<uint32_t>(PixelFormatLayout::ENUM_ID_RANGE_SIZE) ? PixelFormatLayoutMetaData::Get(srcId).Info.HasAlphaChannel()
                                                                                : false;
  }

  //! @brief Check if the formats can be converted to each other using a simple swizzle operation
  // inline constexpr PixelChannelOrder HasAlphaChannel(const PixelFormatLayout lhs) noexcept
  //{
  //   const auto srcId = GetId(lhs);
  //   return srcId < static_cast<uint32_t>(PixelFormatLayout::ENUM_ID_RANGE_SIZE) ? PixelFormatLayoutMetaData::Get(srcId).ChannelOrder
  //                                                                               : false;
  // }


  // *** bit extraction helper methods ***

  // @brief Extract the basic encoded layout (can only differentiate between layout0 and 1, not any extended encodings)
  // @return PixelFormatLayoutFlags::ENCODED_LAYOUT0 or PixelFormatLayoutFlags::ENCODED_LAYOUT1
  inline constexpr int32_t GetBasicEncodedLayout(const PixelFormatLayout pixelFormatLayout) noexcept
  {
    return (static_cast<int32_t>(pixelFormatLayout) & PixelFormatLayoutFlags::BIT_MASK_ENCODED_LAYOUT_BIT0);
  }

  // @brief Extract the encoded layout
  // @return PixelFormatLayoutFlags::ENCODED_LAYOUT0 or PixelFormatLayoutFlags::ENCODED_LAYOUT1
  inline constexpr int32_t GetEncodedLayout0Packed(const PixelFormatLayout pixelFormatLayout) noexcept
  {
    assert(GetBasicEncodedLayout(pixelFormatLayout) == PixelFormatLayoutFlags::ENCODED_LAYOUT0);
    return (static_cast<int32_t>(pixelFormatLayout) & PixelFormatLayoutFlags::BIT_MASK_ENCODED_LAYOUT0_PACKED);
  }

  // @brief Extract the encoded layout
  // @return PixelFormatLayoutFlags::ENCODED_LAYOUT0 or PixelFormatLayoutFlags::ENCODED_LAYOUT1
  inline constexpr int32_t GetEncodedLayout1CompressionScheme(const PixelFormatLayout pixelFormatLayout) noexcept
  {
    assert(GetBasicEncodedLayout(pixelFormatLayout) == PixelFormatLayoutFlags::ENCODED_LAYOUT1);
    return (static_cast<int32_t>(pixelFormatLayout) & PixelFormatLayoutFlags::BIT_MASK_ENCODED_LAYOUT1_COMPRESSION_SCHEME);
  }

  //! @brief Transform the given pixel format layout to the one that best matches the preferredChannelOrder.
  //! @param layout if layout == PixelFormatLayout::Undefined this returns PixelFormatLayout::Undefined
  //! @param preferredChannelOrder if preferredChannelOrder == PixelChannelOrder::Undefined this returns layout
  PixelFormatLayout Transform(const PixelFormatLayout layout, const PixelChannelOrder preferredChannelOrder) noexcept;

  [[deprecated("Utilize the PxSize1D or PxValueU variant instead")]] inline constexpr uint32_t
    CalcMinimumStride(const uint32_t width, const uint32_t bytesPerPixel) noexcept
  {
    return CalcMinimumStride(PxValueU::Create(width), bytesPerPixel);
  }

  [[deprecated("Utilize the PxSize1D or PxValueU variant instead")]] inline constexpr uint32_t
    CalcMinimumStride(const uint32_t width, const PixelFormatLayout pixelFormatLayout) noexcept
  {
    return CalcMinimumStride(PxValueU::Create(width), pixelFormatLayout);
  }

  [[deprecated("Utilize the PxSize1D or PxValueU variant instead")]] inline uint32_t
    CalcMinimumStride(const uint32_t width, const PixelFormatLayout pixelFormatLayout, const StrideRequirement strideRequirement)
  {
    return CalcMinimumStride(PxValueU::Create(width), pixelFormatLayout, strideRequirement);
  }

  [[deprecated("Utilize the PxSize1D or PxValueU variant instead")]] inline uint32_t
    CalcMinimumStride(const uint32_t width, const uint32_t bytesPerPixel, const StrideRequirement strideRequirement)
  {
    return CalcMinimumStride(PxValueU::Create(width), bytesPerPixel, strideRequirement);
  }
}

#endif
