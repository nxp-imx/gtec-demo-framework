#ifndef FSLGRAPHICS_BITMAP_CONVERTER_BITMAPCONVERTERFUNCTIONS_HPP
#define FSLGRAPHICS_BITMAP_CONVERTER_BITMAPCONVERTERFUNCTIONS_HPP
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

#include <FslGraphics/Bitmap/Bitmap.hpp>

namespace Fsl::BitmapConverterFunctions
{
  //! @brief Convert SRGB R8G8B8 to a single 8bit gray-scale channel using the luminance NTSC method (The resulting luminance is also a non linear
  //! format).
  //! @param srcBitmap The raw bitmap to convert. Must be PixelFormat::R8G8B8_UNORM
  //! @param dstOneChannel8BitPixelFormat. Must be PixelFormat::R8_UNORM, EX_LUMINANCE8_UNORN or EX_ALPHA8_UNORM.
  Bitmap R8G8B8ToGrayscaleLuminanceNTSC(const Bitmap& srcBitmap, const PixelFormat dstOneChannel8BitPixelFormat = PixelFormat::R8_UNORM);
}

#endif
