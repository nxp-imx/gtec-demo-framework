#ifndef FSLUTIL_OPENGLES3_GLCOMPRESSEDFORMAT_HPP
#define FSLUTIL_OPENGLES3_GLCOMPRESSEDFORMAT_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

namespace Fsl
{
  namespace GLES3
  {
    // Custom formats supported by some GPU's under OpenGL ES3 (but they are from the 3.2 header).
    enum class GLCompressedFormat
    {
      PALETTE4_RGB8_OES = 0x8B90,
      PALETTE4_RGBA8_OES = 0x8B91,
      PALETTE4_R5_G6_B5_OES = 0x8B92,
      PALETTE4_RGBA4_OES = 0x8B93,
      PALETTE4_RGB5_A1_OES = 0x8B94,
      PALETTE8_RGB8_OES = 0x8B95,
      PALETTE8_RGBA8_OES = 0x8B96,
      PALETTE8_R5_G6_B5_OES = 0x8B97,
      PALETTE8_RGBA4_OES = 0x8B98,
      PALETTE8_RGB5_A1_OES = 0x8B99,

      ETC1_RGB8_OES = 0x8D64,

      RGBA_ASTC_4x4 = 0x93B0,
      RGBA_ASTC_5x4 = 0x93B1,
      RGBA_ASTC_5x5 = 0x93B2,
      RGBA_ASTC_6x5 = 0x93B3,
      RGBA_ASTC_6x6 = 0x93B4,
      RGBA_ASTC_8x5 = 0x93B5,
      RGBA_ASTC_8x6 = 0x93B6,
      RGBA_ASTC_8x8 = 0x93B7,
      RGBA_ASTC_10x5 = 0x93B8,
      RGBA_ASTC_10x6 = 0x93B9,
      RGBA_ASTC_10x8 = 0x93BA,
      RGBA_ASTC_10x10 = 0x93BB,
      RGBA_ASTC_12x10 = 0x93BC,
      RGBA_ASTC_12x12 = 0x93BD,
      SRGB8_ALPHA8_ASTC_4x4 = 0x93D0,
      SRGB8_ALPHA8_ASTC_5x4 = 0x93D1,
      SRGB8_ALPHA8_ASTC_5x5 = 0x93D2,
      SRGB8_ALPHA8_ASTC_6x5 = 0x93D3,
      SRGB8_ALPHA8_ASTC_6x6 = 0x93D4,
      SRGB8_ALPHA8_ASTC_8x5 = 0x93D5,
      SRGB8_ALPHA8_ASTC_8x6 = 0x93D6,
      SRGB8_ALPHA8_ASTC_8x8 = 0x93D7,
      SRGB8_ALPHA8_ASTC_10x5 = 0x93D8,
      SRGB8_ALPHA8_ASTC_10x6 = 0x93D9,
      SRGB8_ALPHA8_ASTC_10x8 = 0x93DA,
      SRGB8_ALPHA8_ASTC_10x10 = 0x93DB,
      SRGB8_ALPHA8_ASTC_12x10 = 0x93DC,
      SRGB8_ALPHA8_ASTC_12x12 = 0x93DD,
    };
  }
}

#endif
