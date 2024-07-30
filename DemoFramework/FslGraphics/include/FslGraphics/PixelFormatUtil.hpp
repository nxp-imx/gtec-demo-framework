#ifndef FSLGRAPHICS_PIXELFORMATUTIL_HPP
#define FSLGRAPHICS_PIXELFORMATUTIL_HPP
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

#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslGraphics/PixelChannelOrder.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/PixelFormatLayoutUtil.hpp>
#include <FslGraphics/StrideRequirement.hpp>

namespace Fsl::PixelFormatUtil
{
  //! @brief Get the format-id of the pixel format (it basically strips all the extra meta data)
  extern uint32_t GetId(const PixelFormat pixelFormat);

  //! @brief Get the zero based index of this pixel format type.
  //         Index zero means the first id defined by PixelFormat::ENUM_ID_BEGIN_RANGE
  extern uint32_t GetFormatRangeIndex(const PixelFormat pixelFormat);
  // FIX: switch the return type here to enums
  // extern uint32_t GetNumericFormat(const PixelFormat pixelFormat);
  // FIX: switch the return type here to enums
  // extern uint32_t GetCompressionScheme(const PixelFormat pixelFormat);

  inline constexpr PixelFormatLayout GetPixelFormatLayout(const PixelFormat pixelFormat) noexcept
  {
    return static_cast<PixelFormatLayout>((static_cast<int32_t>(pixelFormat) & PixelFormatFlags::BIT_MASK_LAYOUT) >>
                                          PixelFormatFlags::BIT_INDEX_LAYOUT);
  }

  //! @brief Check if this is considered a compressed format
  inline constexpr bool IsCompressed(const PixelFormat pixelFormat) noexcept
  {
    return PixelFormatLayoutUtil::IsCompressed(GetPixelFormatLayout(pixelFormat));
  }

  //! @brief Check if this is considered a packed format
  inline constexpr bool IsPacked(const PixelFormat pixelFormat) noexcept
  {
    return PixelFormatLayoutUtil::IsPacked(GetPixelFormatLayout(pixelFormat));
  }

  //! @brief Given a pixel format id try to lookup the pixel format enum value.
  //! @note  Returns the PixelFormat value or PixelFormat::Undefined if the lookup fails (or if asked to lookup the undefined value)
  extern PixelFormat TryGetPixelFormatById(const uint32_t formatId) noexcept;

  //! @brief Given a pixel format bits try to lookup the pixel (beware this is a slow operation)
  //! @note  Returns the PixelFormat value or PixelFormat::Undefined if the lookup fails (or if asked to lookup the undefined value)
  extern PixelFormat TryFindPixelFormat(const uint32_t formatBits) noexcept;

  //! @brief Check if a PixelFormatFlags is set in the pixel format
  inline constexpr bool IsPixelFormatFlagSet(const PixelFormat pixelFormat, const PixelFormatFlags::Enum flag) noexcept
  {
    return (static_cast<uint32_t>(pixelFormat) & static_cast<uint32_t>(flag)) == static_cast<uint32_t>(flag);
  }

  // @brief Tries to change the numeric format of the pixelformat
  // @return PixelFormat::Undefined if the requested change is invalid
  inline PixelFormat TrySetCompatiblePixelFormatFlag(const PixelFormat sourcePixelFormat, const PixelFormatFlags::Enum flag) noexcept
  {
    auto pixelFormat = static_cast<uint32_t>(sourcePixelFormat);
    // Strip the numeric format
    pixelFormat &= ~static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_NUMERIC_FORMAT);
    pixelFormat |= static_cast<uint32_t>(flag);
    return TryFindPixelFormat(pixelFormat);
  }

  inline constexpr uint32_t GetBytesPerPixel(const PixelFormat pixelFormat) noexcept
  {
    return PixelFormatLayoutUtil::GetBytesPerPixel(GetPixelFormatLayout(pixelFormat));
  }

  inline constexpr uint32_t CalcMinimumStride(const PxValueU widthPx, const PixelFormat pixelFormat) noexcept
  {
    return PixelFormatLayoutUtil::CalcMinimumStride(widthPx, GetPixelFormatLayout(pixelFormat));
  }

  inline constexpr uint32_t CalcMinimumStride(const PxValueU widthPx, const uint32_t bytesPerPixel) noexcept
  {
    return PixelFormatLayoutUtil::CalcMinimumStride(widthPx, bytesPerPixel);
  }

  inline uint32_t CalcMinimumStride(const PxValueU widthPx, const PixelFormat pixelFormat, const StrideRequirement strideRequirement)
  {
    return PixelFormatLayoutUtil::CalcMinimumStride(widthPx, GetPixelFormatLayout(pixelFormat), strideRequirement);
  }

  inline uint32_t CalcMinimumStride(const PxValueU widthPx, const uint32_t bytesPerPixel, const StrideRequirement strideRequirement)
  {
    return PixelFormatLayoutUtil::CalcMinimumStride(widthPx, bytesPerPixel, strideRequirement);
  }

  inline constexpr uint32_t CalcMinimumStride(const PxSize1D widthPx, const PixelFormat pixelFormat) noexcept
  {
    return PixelFormatLayoutUtil::CalcMinimumStride(widthPx, GetPixelFormatLayout(pixelFormat));
  }

  inline constexpr uint32_t CalcMinimumStride(const PxSize1D widthPx, const uint32_t bytesPerPixel) noexcept
  {
    return PixelFormatLayoutUtil::CalcMinimumStride(widthPx, bytesPerPixel);
  }

  inline uint32_t CalcMinimumStride(const PxSize1D widthPx, const PixelFormat pixelFormat, const StrideRequirement strideRequirement)
  {
    return PixelFormatLayoutUtil::CalcMinimumStride(widthPx, GetPixelFormatLayout(pixelFormat), strideRequirement);
  }

  inline uint32_t CalcMinimumStride(const PxSize1D widthPx, const uint32_t bytesPerPixel, const StrideRequirement strideRequirement)
  {
    return PixelFormatLayoutUtil::CalcMinimumStride(widthPx, bytesPerPixel, strideRequirement);
  }


  [[deprecated("Utilize the PxSize1D or PxValueU variant instead")]] inline constexpr uint32_t
    CalcMinimumStride(const uint32_t width, const PixelFormat pixelFormat) noexcept
  {
    return PixelFormatLayoutUtil::CalcMinimumStride(PxValueU::Create(width), GetPixelFormatLayout(pixelFormat));
  }

  [[deprecated("Utilize the PxSize1D or PxValueU variant instead")]] inline constexpr uint32_t
    CalcMinimumStride(const uint32_t width, const uint32_t bytesPerPixel) noexcept
  {
    return PixelFormatLayoutUtil::CalcMinimumStride(PxValueU::Create(width), bytesPerPixel);
  }

  [[deprecated("Utilize the PxSize1D or PxValueU variant instead")]] inline uint32_t
    CalcMinimumStride(const uint32_t width, const PixelFormat pixelFormat, const StrideRequirement strideRequirement)
  {
    return PixelFormatLayoutUtil::CalcMinimumStride(PxValueU::Create(width), GetPixelFormatLayout(pixelFormat), strideRequirement);
  }

  [[deprecated("Utilize the PxSize1D or PxValueU variant instead")]] inline uint32_t
    CalcMinimumStride(const uint32_t width, const uint32_t bytesPerPixel, const StrideRequirement strideRequirement)
  {
    return PixelFormatLayoutUtil::CalcMinimumStride(PxValueU::Create(width), bytesPerPixel, strideRequirement);
  }


  inline bool IsValidStride(const uint32_t width, const PixelFormat pixelFormat, const StrideRequirement strideRequirement,
                            const uint32_t desiredStride)
  {
    return PixelFormatLayoutUtil::IsValidStride(width, GetPixelFormatLayout(pixelFormat), strideRequirement, desiredStride);
  }

  inline bool IsValidStride(const uint32_t width, const uint32_t bytesPerPixel, const StrideRequirement strideRequirement,
                            const uint32_t desiredStride)
  {
    return PixelFormatLayoutUtil::IsValidStride(width, bytesPerPixel, strideRequirement, desiredStride);
  }

  inline bool IsValidStride(const PxSize1D widthPx, const PixelFormat pixelFormat, const StrideRequirement strideRequirement,
                            const uint32_t desiredStride)
  {
    return PixelFormatLayoutUtil::IsValidStride(widthPx, GetPixelFormatLayout(pixelFormat), strideRequirement, desiredStride);
  }

  inline bool IsValidStride(const PxSize1D widthPx, const uint32_t bytesPerPixel, const StrideRequirement strideRequirement,
                            const uint32_t desiredStride)
  {
    return PixelFormatLayoutUtil::IsValidStride(widthPx, bytesPerPixel, strideRequirement, desiredStride);
  }

  //! @brief Extract the numeric format from the pixel format
  inline constexpr PixelFormatFlags::Enum GetNumericFormat(const PixelFormat pixelFormat) noexcept
  {
    return static_cast<PixelFormatFlags::Enum>(static_cast<uint32_t>(pixelFormat) & static_cast<uint32_t>(PixelFormatFlags::BIT_MASK_NUMERIC_FORMAT));
  }

  inline constexpr uint32_t GetChannelCount(const PixelFormat pixelFormat) noexcept
  {
    return PixelFormatLayoutUtil::GetChannelCount(GetPixelFormatLayout(pixelFormat));
  }

  inline constexpr bool HasAlphaChannel(const PixelFormat pixelFormat) noexcept
  {
    return PixelFormatLayoutUtil::HasAlphaChannel(GetPixelFormatLayout(pixelFormat));
  }

  //! @brief Try to transform the PixelFormatFlag of pixelFormat to the one supplied in pixelFormatFlags
  //! @param pixelFormat if pixelFormat == PixelFormat::Undefined this returns PixelFormat::Undefined
  //! @param pixelFormatFlags = the new pixelFormatFlags
  extern PixelFormat TryTransform(const PixelFormat pixelFormat, const PixelFormatFlags::Enum numericFormat) noexcept;

  //! @brief Transform the given pixel format to the one that best matches the preferredChannelOrder.
  //! @param pixelFormat if pixelFormat == PixelFormat::Undefined this returns PixelFormat::Undefined
  //! @param preferredChannelOrder if preferredChannelOrder == PixelChannelOrder::Undefined this returns pixelFormat
  extern PixelFormat Transform(const PixelFormat pixelFormat, const PixelChannelOrder preferredChannelOrder) noexcept;
}

#endif
