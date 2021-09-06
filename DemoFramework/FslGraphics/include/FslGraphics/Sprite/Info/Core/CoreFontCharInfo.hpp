#ifndef FSLGRAPHICS_SPRITE_INFO_CORE_COREFONTCHARINFO_HPP
#define FSLGRAPHICS_SPRITE_INFO_CORE_COREFONTCHARINFO_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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
#include <FslBase/Math/Pixel/PxRectangleU16.hpp>

namespace Fsl
{
  struct CoreFontCharInfo
  {
    //! The character id.
    uint32_t Id{};

    //! The rectangle on the source atlas texture
    PxRectangleU16 SrcTextureRectPx;

    //! How much the current position should be offset when copying the image from the texture to the screen.
    PxPoint2 OffsetPx;

    /// How much the current position should be advanced after drawing the character.
    uint16_t XAdvancePx{};

    constexpr CoreFontCharInfo() = default;
    constexpr CoreFontCharInfo(const uint32_t id, const PxRectangleU16 srcTextureRectPx, const PxPoint2& offsetPx, const uint16_t xAdvancePx)
      : Id(id)
      , SrcTextureRectPx(srcTextureRectPx)
      , OffsetPx(offsetPx)
      , XAdvancePx(xAdvancePx)
    {
    }

    constexpr bool operator==(const CoreFontCharInfo& rhs) const
    {
      return Id == rhs.Id && SrcTextureRectPx == rhs.SrcTextureRectPx && OffsetPx == rhs.OffsetPx && XAdvancePx == rhs.XAdvancePx;
    }

    constexpr bool operator!=(const CoreFontCharInfo& rhs) const
    {
      return !(*this == rhs);
    }
  };
}

#endif
