#ifndef FSLGRAPHICS2D_PIXELFORMATCONVERTER_BITMAP_RAWBITMAPTONEMAPPERFUNCTIONS_HPP
#define FSLGRAPHICS2D_PIXELFORMATCONVERTER_BITMAP_RAWBITMAPTONEMAPPERFUNCTIONS_HPP
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

namespace Fsl::FslGraphics2D::RawBitmapToneMapperFunctions
{
  //! @brief Apply a very simple tone mapping
  //! @param dstBitmap The raw bitmap to write to. Must be PixelFormat::R32G32B32A32_SFLOAT
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R32G32B32A32_SFLOAT
  //! @note As this is a low level unchecked function the assumptions are only validated by asserts.
  //!       The following must be obeyed
  //!       - The origin of srcBitmap and rDstBitmap must match.
  //!       - The size of srcBitmap and rDstBitmap must match.
  //!       - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  //!       - The in-memory ordering of bytes within a component is determined by the host endianness.
  void UncheckedHableR32G32B32A32FloatToRG32B32A32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap, const float exposure) noexcept;
  void UncheckedReinhardR32G32B32A32FloatToRG32B32A32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap, const float exposure) noexcept;
}

#endif
