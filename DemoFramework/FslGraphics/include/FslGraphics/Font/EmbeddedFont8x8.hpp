#ifndef FSLGRAPHICS_FONT_EMBEDDEDFONT8X8_HPP
#define FSLGRAPHICS_FONT_EMBEDDEDFONT8X8_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/RectangleSizeRestrictionFlag.hpp>
#include <FslGraphics/PixelFormat.hpp>

namespace Fsl
{
  class Bitmap;
  class RawBitmapEx;

  //! This is a very simple monochrome bitmap font for most ASCII characters (Contain at least 33-127)
  class EmbeddedFont8x8
  {
  public:
    EmbeddedFont8x8(const EmbeddedFont8x8&) = delete;
    EmbeddedFont8x8& operator=(const EmbeddedFont8x8&) = delete;

    static uint8_t MinCharacter();
    static uint8_t MaxCharacter();
    static uint8_t CharacterCount();
    static PxSize2D CharacterSize();
    static void ExtractCharacter(const uint8_t theCharacter, uint8_t* pDst, const int32_t dstLength);

    //! @brief Write a character to the bitmap (convenience method)
    //! @note We currently support B8G8R8_UINT, R8G8B8_UINT, B8G8R8A8_UINT, R8G8B8A8_UINT and EX_ALPHA8_UINT
    static void ExtractCharacter(const uint8_t theCharacter, Bitmap& rDstBitmap, const PxPoint2& dstPos);

    //! @brief Write a character to the bitmap (convenience method)
    //! @note We currently support B8G8R8_UINT, R8G8B8_UINT, B8G8R8A8_UINT, R8G8B8A8_UINT and EX_ALPHA8_UINT
    static void ExtractCharacter(const uint8_t theCharacter, RawBitmapEx& rDstBitmap, const PxPoint2& dstPos);

    //! @brief Extract all characters in the font (convenience method)
    static void CreateFontBitmap(Bitmap& rDstBitmap, const PixelFormat dstPixelFormat, const PxPoint2& padding,
                                 const RectangleSizeRestrictionFlag restrictionFlags);

    //! @brief Extract the supplied range of characters (convenience method)
    static void CreateFontBitmap(Bitmap& rDstBitmap, const PixelFormat dstPixelFormat, const uint8_t startCharacter, const uint8_t length,
                                 const PxPoint2& padding, const RectangleSizeRestrictionFlag restrictionFlags);
  };
}

#endif
