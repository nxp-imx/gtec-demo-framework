#ifndef FSLGRAPHICS_TEXTURE_TEXTURETYPE_HPP
#define FSLGRAPHICS_TEXTURE_TEXTURETYPE_HPP
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

namespace Fsl
{
  struct TextureTypeFlags
  {
    // Bit fields
    // 00..09 Type id            - 10 bit =  1024 values
    // 10..14 Dimensions         -  5 bit =    32 values
    // 15..15 ArrayFlag          -  1 bit =     2 values
    // 16..31 Reserved           - 16 bit
    enum Enum
    {
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_COUNT_ID = 10,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_COUNT_DIMENSIONS = 5,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_COUNT_ARRAY_FLAG = 1,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_COUNT_RESERVED = 16,

      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_INDEX_ID = 0,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_INDEX_DIMENSIONS = BIT_INDEX_ID + BIT_COUNT_ID,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_INDEX_ARRAY_FLAG = BIT_INDEX_DIMENSIONS + BIT_COUNT_DIMENSIONS,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_INDEX_RESERVED = BIT_INDEX_ARRAY_FLAG + BIT_COUNT_ARRAY_FLAG,

      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_MASK_ID = ((1 << BIT_COUNT_ID) - 1) << BIT_INDEX_ID,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_MASK_DIMENSIONS = ((1 << BIT_COUNT_DIMENSIONS) - 1) << BIT_INDEX_DIMENSIONS,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_MASK_ARRAY_FLAG = ((1 << BIT_COUNT_ARRAY_FLAG) - 1) << BIT_INDEX_ARRAY_FLAG,
      // NOLINTNEXTLINE(readability-identifier-naming)
      BIT_MASK_RESERVED = ((1 << BIT_COUNT_RESERVED) - 1) << BIT_INDEX_RESERVED,

      // Dimensions
      Dim1D = (1 << BIT_INDEX_DIMENSIONS),
      Dim2D = (2 << BIT_INDEX_DIMENSIONS),
      Dim3D = (3 << BIT_INDEX_DIMENSIONS),
      DimCube = (4 << BIT_INDEX_DIMENSIONS),

      // ArrayFlag
      Array = (1 << BIT_INDEX_ARRAY_FLAG),
    };
  };


  enum class TextureType
  {
    Undefined = 0,
    Tex1D = 1 | TextureTypeFlags::Dim1D,
    Tex2D = 2 | TextureTypeFlags::Dim2D,
    Tex3D = 3 | TextureTypeFlags::Dim3D,
    TexCube = 4 | TextureTypeFlags::DimCube,
    Tex1DArray = 5 | TextureTypeFlags::Dim1D | TextureTypeFlags::Array,
    Tex2DArray = 6 | TextureTypeFlags::Dim2D | TextureTypeFlags::Array,
    TexCubeArray = 7 | TextureTypeFlags::DimCube | TextureTypeFlags::Array,

    //! This covers all the enum texture types
    //! The first valid value in the range
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_BEGIN_ID_RANGE = (Tex1D & TextureTypeFlags::BIT_MASK_ID),
    //! The last valid value in the range
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_END_ID_RANGE = (TexCubeArray & TextureTypeFlags::BIT_MASK_ID),
    //! The total number of entries in the range
    // NOLINTNEXTLINE(readability-identifier-naming)
    ENUM_ID_RANGE_SIZE = (ENUM_END_ID_RANGE - ENUM_BEGIN_ID_RANGE) + 1,
  };
}

#endif
