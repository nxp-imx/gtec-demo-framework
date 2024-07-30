#ifndef FSLGRAPHICS_FONT_BITMAPFONTCHAR_HPP
#define FSLGRAPHICS_FONT_BITMAPFONTCHAR_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <FslBase/Math/Pixel/PxValueU16.hpp>

namespace Fsl
{
  //! This describes on character in the font. There is one for each included character in the font.
  struct BitmapFontChar
  {
    //! The character id.
    uint32_t Id{};

    //! The rectangle of the character image in the texture.
    PxRectangleU32 SrcTextureRectPx;

    //! How much the current position should be offset when copying the image from the texture to the screen.
    PxPoint2 OffsetPx;

    /// How much the current position should be advanced after drawing the character.
    PxValueU16 XAdvancePx;

    constexpr BitmapFontChar() = default;

    constexpr BitmapFontChar(const uint32_t id, const PxRectangleU32& srcTextureRectPx, const PxPoint2 offsetPx, const PxValueU16 xAdvancePx)
      : Id(id)
      , SrcTextureRectPx(srcTextureRectPx)
      , OffsetPx(offsetPx)
      , XAdvancePx(xAdvancePx)
    {
    }

    constexpr bool operator==(const BitmapFontChar& rhs) const
    {
      return Id == rhs.Id && SrcTextureRectPx == rhs.SrcTextureRectPx && OffsetPx == rhs.OffsetPx && XAdvancePx == rhs.XAdvancePx;
    }

    constexpr bool operator!=(const BitmapFontChar& rhs) const
    {
      return !(*this == rhs);
    }


    constexpr uint32_t GetId() const
    {
      return Id;
    }

    constexpr void SetId(const uint32_t id)
    {
      Id = id;
    }
  };
}

#endif
