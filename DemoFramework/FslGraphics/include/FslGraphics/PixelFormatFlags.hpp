#ifndef FSLGRAPHICS_PIXELFORMATFLAGS_HPP
#define FSLGRAPHICS_PIXELFORMATFLAGS_HPP
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

#include <FslGraphics/PixelFormatLayout.hpp>

namespace Fsl
{
  // PixelFormatFlags
  // Bit fields
  // 00..09 Format id         - 10 bit =  1024 values
  // 10..13 Numeric format    -  4 bit =    16 values
  // 14..30 PixelFormatLayout - 17 bit
  // 31..31 Reserved          -  1 bit

  struct PixelFormatFlags
  {
    enum Enum
    {
      BIT_COUNT_FORMAT_ID = 10,
      BIT_COUNT_NUMERIC_FORMAT = 4,
      BIT_COUNT_LAYOUT = 17,
      BIT_COUNT_RESERVED = 1,

      BIT_INDEX_FORMAT_ID = 0,
      BIT_INDEX_NUMERIC_FORMAT = BIT_INDEX_FORMAT_ID + BIT_COUNT_FORMAT_ID,
      BIT_INDEX_LAYOUT = BIT_INDEX_NUMERIC_FORMAT + BIT_COUNT_NUMERIC_FORMAT,
      BIT_INDEX_RESERVED = BIT_INDEX_LAYOUT + BIT_COUNT_LAYOUT,

      BIT_MASK_FORMAT_ID = ((1 << BIT_COUNT_FORMAT_ID) - 1) << BIT_INDEX_FORMAT_ID,
      BIT_MASK_NUMERIC_FORMAT = ((1 << BIT_COUNT_NUMERIC_FORMAT) - 1) << BIT_INDEX_NUMERIC_FORMAT,
      BIT_MASK_LAYOUT = ((1 << BIT_COUNT_LAYOUT) - 1) << BIT_INDEX_LAYOUT,
      BIT_MASK_RESERVED = ((1 << BIT_COUNT_RESERVED) - 1) << BIT_INDEX_RESERVED,

      // Numeric Formats (see the Vulkan for details)
      NF_Undefined = 0,

      //! The components are unsigned normalized values in the range [0,1]
      //! See the Vulkan documentation for details
      NF_UNorm = (1 << BIT_INDEX_NUMERIC_FORMAT),
      //! The components are signed normalized values in the range[-1, 1]
      //! See the Vulkan documentation for details
      NF_SNorm = (2 << BIT_INDEX_NUMERIC_FORMAT),
      //! The components are unsigned integer values that get converted to floating-point in the range [0, pow(2,n)-1]
      //! See the Vulkan documentation for details
      NF_UScaled = (3 << BIT_INDEX_NUMERIC_FORMAT),
      //! The components are signed integer values that get converted to floating-point in the range [-pow(2,n-1), pow(2,n-1)-1]
      //! See the Vulkan documentation for details
      NF_SScaled = (4 << BIT_INDEX_NUMERIC_FORMAT),
      //! The components are unsigned integer values in the range [0, pow(2,n)-1]
      //! See the Vulkan documentation for details
      NF_UInt = (5 << BIT_INDEX_NUMERIC_FORMAT),
      //! The components are signed integer values in the range [-pow(2,n-1), pow(2,n-1)-1]
      //! See the Vulkan documentation for details
      NF_SInt = (6 << BIT_INDEX_NUMERIC_FORMAT),
      //! The components are unsigned floating - point numbers(used by packed, shared exponent, and some compressed formats)
      //! See the Vulkan documentation for details
      NF_UFloat = (7 << BIT_INDEX_NUMERIC_FORMAT),
      //! The components are signed floating - point numbers
      //! See the Vulkan documentation for details
      NF_SFloat = (8 << BIT_INDEX_NUMERIC_FORMAT),
      //! The R, G, and B components are unsigned normalized values that represent values using sRGB nonlinear encoding, while the A component (if one
      //! exists) is a regular unsigned normalized value See the Vulkan documentation for details
      NF_Srgb = (9 << BIT_INDEX_NUMERIC_FORMAT),
    };

    // constexpr is not supported in VS2013
    // constexpr static int Encode(const PixelFormatLayout layout)
    //{
    //  return static_cast<int>(layout)& ((1 << BIT_COUNT_LAYOUT_FLAGS) - 1) << BIT_INDEX_LAYOUT_FLAGS;
    //}
  };


  inline constexpr int FSLGRAPHICS_PIXELFORMATFLAGS_ENCODE_LAYOUT(const PixelFormatLayout pfl)
  {
    return ((static_cast<int>((pfl)) & ((1 << PixelFormatFlags::BIT_COUNT_LAYOUT) - 1)) << PixelFormatFlags::BIT_INDEX_LAYOUT);
  }
}

// We keep the original macro around just in case we need it
// Due to missing const_expr we use this nasty macro
////#define FSLGRAPHICS_PIXELFORMATFLAGS_ENCODE_LAYOUT(X) ((static_cast<int>((X)) & ((1 << PixelFormatFlags::BIT_COUNT_LAYOUT) - 1)) <<
/// PixelFormatFlags::BIT_INDEX_LAYOUT)


#endif
