#ifndef FSLGRAPHICS2D_PIXELFORMATCONVERTER_BITMAP_RAWBITMAPCONVERTERFUNCTIONS_HPP
#define FSLGRAPHICS2D_PIXELFORMATCONVERTER_BITMAP_RAWBITMAPCONVERTERFUNCTIONS_HPP
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

#include <FslGraphics/Bitmap/RawBitmapEx.hpp>

namespace Fsl::FslGraphics2D::RawBitmapConverterFunctions
{
  //! Check if the two bitmaps are considered overlapping
  bool DoesMemoryRegionOverlap(const RawBitmapEx& dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept;

  //! Check if the two bitmaps memory are not overlapping or is considered safe for in-place modification
  //! Inplace modification requires
  //! - that the src and dst start at the same memory address
  //! - that a src stride is >= the dst stride
  //! - that a src pixel byte size is >= the dst pixel byte size
  //! This allow inplace modification to occur by processing from the first pixel in the bitmap moving towards the last.
  bool IsSafeInplaceModificationOrNoMemoryOverlap(const RawBitmapEx& dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept;

  //! @brief Convert SRGB R8G8B8 to linear R16G16B16  (stored as a uint16 per channel).
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R16G16B16_UNORM.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R8G8B8_SRGB
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR8G8B8SrgbToR16G16B16UNorm(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept;

  //! @brief Convert SRGB R8G8B8A8 to linear R16G16B16A16 (stored as a uint16 per channel).
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R16G16B16A16_UNORM.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R8G8B8A8_SRGB
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR8G8B8A8SrgbToR16G16B16A16UNorm(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept;

  //! @brief Convert SRGB R8G8B8 to linear R16G16B16 (stored as a fp16 per channel).
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R16G16B16_SFLOAT.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R8G8B8_SRGB
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR8G8B8SrgbToR16G16B16Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap);

  //! @brief Convert SRGB R8G8B8A8 to linear R16G16B16A16 (stored as a fp16 per channel).
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R16G16B16A16_SFLOAT.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R8G8B8A8_SRGB
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR8G8B8A8SrgbToR16G16B16A16Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap);

  //! @brief Convert SRGB R8G8B8 to linear R32G32B32 (stored as a float per channel).
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R32G32B32_SFLOAT.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R8G8B8_SRGB
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR8G8B8SrgbToR32G32B32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept;

  //! @brief Convert SRGB R8G8B8A8 to linear R32G32B32A32 (stored as a float per channel).
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R32G32B32A32_SFLOAT.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R8G8B8A8_SRGB
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR8G8B8A8SrgbToR32G32B32A32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept;

  //! @brief Convert linear R16G16B16 (stored as a uint16 per channel) to SRGB R8G8B8.
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R8G8B8_SRGB.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R16G16B16_UNORM.
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR16G16B16UNormToR8G8B8Srgb(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept;

  //! @brief Convert linear R16G16B16A16 (stored as a uint16 per channel) to SRGB R8G8B8A8.
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R8G8B8A8_SRGB.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R16G16B16A16_UNORM
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR16G16B16A16UNormToR8G8B8A8Srgb(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept;

  //! @brief Convert linear R16G16B16 (stored as a fp16 per channel) to SRGB R8G8B8
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R8G8B8_SRGB.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R16G16B16_SFLOAT.
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR16G16B16FloatToR8G8B8Srgb(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap);

  //! @brief Convert linear R16G16B16A16 (stored as a fp16 per channel) to SRGB R8G8B8A8.
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R8G8B8A8_SRGB.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R16G16B16A16_SFLOAT.
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR16G16B16A16FloatToR8G8B8A8Srgb(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap);

  //! @brief Convert linear R32G32B32 (stored as a float per channel) to SRGB R8G8B8.
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R8G8B8_SRGB.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R32G32B32_SFLOAT.
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR32G32B32FloatToR8G8B8Srgb(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept;

  //! @brief Convert linear R32G32B32A32 (stored as a float per channel) to SRGB R8G8B8A8.
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R8G8B8_SRGB.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R32G32B32A32_SFLOAT.
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR32G32B32A32FloatToR8G8B8A8Srgb(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept;

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief Convert linear R16G16B16 to linear R32G32B32 (stored as a fp16 per channel).
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R32G32B32_SFLOAT.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R16G16B16_SFLOAT
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR16G16B16FloatToR32G32B32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap);

  //! @brief Convert linear R16G16B16A16 to linear R32G32B32A32 (stored as a fp16 per channel).
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R16G16B16A16_SFLOAT
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R32G32B32A32_SFLOAT
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR16G16B16A16FloatToR32G32B32A32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap);

  //! @brief Convert linear R32G32B32 to linear R16G16B16 (stored as a fp16 per channel).
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R16G16B16_SFLOAT.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R32G32B32_SFLOAT
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR32G32B32FloatToR16G16B16Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap);

  //! @brief Convert linear R32G32B32A32 to linear R16G16B16A16 (stored as a fp16 per channel).
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R32G32B32A32_SFLOAT.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R16G16B16A16_SFLOAT
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR32G32B32A32FloatToR16G16B16A16Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap);


  // -------------------------------------------------------------------------------------------------------------------------------------------------

  //! @brief Convert linear R16G16B16 to linear R32G32B32 (stored as a fp16 per channel).
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R32G32B32_SFLOAT.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R16G16B16_UNORM
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR16G16B16UNormToR32G32B32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap);

  //! @brief Convert linear R16G16B16A16 to linear R32G32B32A32 (stored as a fp16 per channel).
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R16G16B16A16_UNORM
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R32G32B32A32_SFLOAT
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR16G16B16A16UNormToR32G32B32A32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap);

  //! @brief Convert linear R32G32B32 to linear R16G16B16 (stored as a fp16 per channel).
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R16G16B16_UNORM.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R32G32B32_SFLOAT
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR32G32B32FloatToR16G16B16UNorm(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap);

  //! @brief Convert linear R32G32B32A32 to linear R16G16B16A16 (stored as a fp16 per channel).
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R32G32B32A32_SFLOAT.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R16G16B16A16_UNORM
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR32G32B32A32FloatToR16G16B16A16UNorm(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap);

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // Special methods - Where the source and dst pixel format channel count does not match 100%
  // -------------------------------------------------------------------------------------------------------------------------------------------------


  //! @brief Convert linear R32G32B32 to linear R16G16B16A16 (stored as a fp16 per channel) The generated alpha will be set to 1.0f.
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R32G32B32_SFLOAT.
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R16G16B16A16_SFLOAT
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedR32G32B32FloatToR16G16B16A16Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap);
}

#endif
