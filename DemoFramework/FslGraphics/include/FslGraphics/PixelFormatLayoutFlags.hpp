#ifndef FSLGRAPHICS_PIXELFORMATLAYOUTFLAGS_HPP
#define FSLGRAPHICS_PIXELFORMATLAYOUTFLAGS_HPP
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

namespace Fsl
{
  // PixelFormat Layout = PixelFormatFlags | (PixelFormatLayoutFlags << 14)
  // So that basically means we have
  // 14 bits for the PixelFormatFlags
  // 17 bits for the PixelFormatLayoutFlags
  //  1 bit reserved

  // PixelFormatFlags
  // Bit fields
  // 00..09 Format id           - 10 bit =  1024 values
  // 10..13 Numeric format      -  4 bit =    16 values
  // = 14 bits as allocated

  // This would be the first choice for a layout (not the one used!)
  //   PixelFormatLayoutFlags
  //   Bit fields
  //   00..07 Format id           -  8 bit =  256 values
  //   08..15 Bytes per pixel     -  8 bit =   256 values -> So thats up to (255*8) / 4 = 510 bits per channel if using four channels
  //   16..19 Compression scheme  -  4 bit =    16 values
  //   20..20 PackedFlag          -  1 bit =     2 values
  //   But thats 21 bits :(
  //
  // So we utilize a slightly more convoluted format
  //   PixelFormatLayoutFlags
  //   Bit fields
  //   00..07 Format id              -  8 bit =  256 values
  //   08..16 EncodedLayoutInfo      -  9 bit =  512 values
  //   = 17 bits
  //
  // So the encoded compression info is the interesting part
  // If we look at the 9 bits of the encoded layout info then bit 7 defines the meaning of the rest of the bits
  // It's basically the encoding type
  //
  // Encoding type 0 bit fields
  // 00..00 = 1 bit 'encoding type' 0 (no compression)
  // 01..07 = 7 bits for bits per pixel = 127 values -> So thats up to (127*8) / 4 = 254 bits per channel if using four channels
  // 08..08 = 1 bit for packed flag
  //
  // Encoding type 1 bit fields
  // 00..01 = 2 bit 'encoding type' bit 0 == 1 (allows us more encoding schemes if need be)
  // 02..08 = 7 bits for Compression scheme


  //   Bit fields
  //   00..07 Format id              -  8 bit =  256 values
  //   08..16 EncodedLayoutInfo      -  9 bit =  512 values
  //   = 17 bits
  struct PixelFormatLayoutFlags
  {
    enum Enum
    {
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_COUNT_FORMAT_ID = 8,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_COUNT_ENCODED_LAYOUT_BLOCK = 9,

      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_INDEX_FORMAT_ID = 0,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_INDEX_ENCODED_LAYOUT_BLOCK = BIT_INDEX_FORMAT_ID + BIT_COUNT_FORMAT_ID,

      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_MASK_FORMAT_ID = ((1 << BIT_COUNT_FORMAT_ID) - 1),
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_MASK_ENCODED_LAYOUT_BLOCK = ((1 << BIT_COUNT_ENCODED_LAYOUT_BLOCK) - 1) << BIT_INDEX_ENCODED_LAYOUT_BLOCK,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_MASK_ENCODED_LAYOUT_BIT0 = ((1 << 1) - 1) << BIT_INDEX_ENCODED_LAYOUT_BLOCK,

      // Encoded layout 0 (9 bits)
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_COUNT_ENCODED_LAYOUT0_ENCODING = 1,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_COUNT_ENCODED_LAYOUT0_BYTES_PER_PIXEL = 7,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_COUNT_ENCODED_LAYOUT0_PACKED = 1,

      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_INDEX_ENCODED_LAYOUT0_ENCODING = BIT_INDEX_ENCODED_LAYOUT_BLOCK,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL = BIT_INDEX_ENCODED_LAYOUT0_ENCODING + BIT_COUNT_ENCODED_LAYOUT0_ENCODING,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_INDEX_ENCODED_LAYOUT0_PACKED = BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL + BIT_COUNT_ENCODED_LAYOUT0_BYTES_PER_PIXEL,

      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_MASK_ENCODED_LAYOUT0_ENCODING = ((1 << BIT_COUNT_ENCODED_LAYOUT0_ENCODING) - 1) << BIT_INDEX_ENCODED_LAYOUT0_ENCODING,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_MASK_ENCODED_LAYOUT0_BYTES_PER_PIXEL = ((1 << BIT_COUNT_ENCODED_LAYOUT0_BYTES_PER_PIXEL) - 1) << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_MASK_ENCODED_LAYOUT0_PACKED = ((1 << BIT_COUNT_ENCODED_LAYOUT0_PACKED) - 1) << BIT_INDEX_ENCODED_LAYOUT0_PACKED,

      // Encoded layout 1 (9 bits)
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_COUNT_ENCODED_LAYOUT1_ENCODING = 2,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_COUNT_ENCODED_LAYOUT1_COMPRESSION_SCHEME = 7,

      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_INDEX_ENCODED_LAYOUT1_ENCODING = BIT_INDEX_ENCODED_LAYOUT_BLOCK,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME = BIT_INDEX_ENCODED_LAYOUT1_ENCODING + BIT_COUNT_ENCODED_LAYOUT1_ENCODING,

      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_MASK_ENCODED_LAYOUT1_ENCODING = ((1 << BIT_COUNT_ENCODED_LAYOUT1_ENCODING) - 1) << BIT_INDEX_ENCODED_LAYOUT1_ENCODING,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_MASK_ENCODED_LAYOUT1_COMPRESSION_SCHEME = ((1 << BIT_COUNT_ENCODED_LAYOUT1_COMPRESSION_SCHEME) - 1)
                                                    << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME,

      // Layout 0 - Bytes per pixel
      // NOLINTNEXTLINE(readability-identifier-naming)
      BytesPerPixelUndefined = (0 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel1 = (1 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel2 = (2 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel3 = (3 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel4 = (4 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel5 = (5 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel6 = (6 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel7 = (7 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel8 = (8 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel9 = (9 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel10 = (10 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel11 = (11 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel12 = (12 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel13 = (13 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel14 = (14 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel15 = (15 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel16 = (16 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel17 = (17 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel18 = (18 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel19 = (19 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel20 = (20 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel21 = (21 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel22 = (22 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel23 = (23 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel24 = (24 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel25 = (25 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel26 = (26 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel27 = (27 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel28 = (28 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel29 = (29 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel30 = (30 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel31 = (31 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel32 = (32 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel33 = (33 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel34 = (34 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel35 = (35 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel36 = (36 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel37 = (37 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel38 = (38 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel39 = (39 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel40 = (40 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel41 = (41 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel42 = (42 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel43 = (43 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel44 = (44 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel45 = (45 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel46 = (46 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel47 = (47 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel48 = (48 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel49 = (49 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel50 = (50 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel51 = (51 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel52 = (52 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel53 = (53 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel54 = (54 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel55 = (55 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel56 = (56 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel57 = (57 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel58 = (58 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel59 = (59 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel60 = (60 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel61 = (61 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel62 = (62 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel63 = (63 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),
      BytesPerPixel64 = (64 << BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL),

      // NOLINTNEXTLINE(readability-identifier-naming)
      ENCODED_LAYOUT0 = 0,
      // NOLINTNEXTLINE(readability-identifier-naming)
      ENCODED_LAYOUT1 = BIT_MASK_ENCODED_LAYOUT_BIT0,

      // Layout 0 - Packing
      // Packed formats store multiple components within one underlying type.
      // The in-memory ordering of bytes comprising the underlying type is determined by the host endianness.
      Packed = (1 << BIT_INDEX_ENCODED_LAYOUT0_PACKED),

      // Layout 1 - Compression scheme

      //! No compression
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_NONE = ENCODED_LAYOUT1 | (0 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      //! Block Compression
      //! See the Vulkan documentation for details
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_BC1_RGB = ENCODED_LAYOUT1 | (1 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_BC1_RGBA = ENCODED_LAYOUT1 | (2 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_BC2 = ENCODED_LAYOUT1 | (3 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_BC3 = ENCODED_LAYOUT1 | (4 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_BC4 = ENCODED_LAYOUT1 | (5 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_BC5 = ENCODED_LAYOUT1 | (6 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_BC6H = ENCODED_LAYOUT1 | (7 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_BC7 = ENCODED_LAYOUT1 | (8 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      //! Ericsson Texture Compression
      //! See the Vulkan documentation for details
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ETC2_R8G8B8 = ENCODED_LAYOUT1 | (9 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ETC2_R8G8B8A1 = ENCODED_LAYOUT1 | (10 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ETC2_R8G8B8A8 = ENCODED_LAYOUT1 | (11 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      //! ETC2 Alpha Compression
      //! See the Vulkan documentation for details
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_EAC_R11 = ENCODED_LAYOUT1 | (12 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_EAC_R11G11 = ENCODED_LAYOUT1 | (13 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      //! Adaptive Scalable Texture Compression(LDR Profile)
      //! See the Vulkan documentation for details
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ASTC_4X4 = ENCODED_LAYOUT1 | (14 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ASTC_5X4 = ENCODED_LAYOUT1 | (15 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ASTC_5X5 = ENCODED_LAYOUT1 | (16 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ASTC_6X5 = ENCODED_LAYOUT1 | (17 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ASTC_6X6 = ENCODED_LAYOUT1 | (18 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ASTC_8X5 = ENCODED_LAYOUT1 | (19 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ASTC_8X6 = ENCODED_LAYOUT1 | (20 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ASTC_8X8 = ENCODED_LAYOUT1 | (21 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ASTC_10X5 = ENCODED_LAYOUT1 | (22 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ASTC_10X6 = ENCODED_LAYOUT1 | (23 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ASTC_10X8 = ENCODED_LAYOUT1 | (24 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ASTC_10X10 = ENCODED_LAYOUT1 | (25 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ASTC_12X10 = ENCODED_LAYOUT1 | (26 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
      // NOLINTNEXTLINE(readability-identifier-naming)
      CS_ASTC_12X12 = ENCODED_LAYOUT1 | (27 << BIT_INDEX_ENCODED_LAYOUT1_COMPRESSION_SCHEME),
    };
  };
}

#endif
