#ifndef FSLGRAPHICS_SPRITE_INFO_SPRITEFONTCHARINFO_HPP
#define FSLGRAPHICS_SPRITE_INFO_SPRITEFONTCHARINFO_HPP
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

#include <FslGraphics/Sprite/Info/Core/CoreFontCharInfo.hpp>
#include <FslGraphics/Sprite/Info/Core/RenderFontCharInfo.hpp>

namespace Fsl
{
  //! This describes on character in the font. There is one for each included character in the font.
  struct SpriteFontCharInfo
  {
    CoreFontCharInfo CharInfo;
    RenderFontCharInfo RenderInfo;

    constexpr SpriteFontCharInfo() = default;

    constexpr SpriteFontCharInfo(const CoreFontCharInfo& charInfo, const RenderFontCharInfo& renderInfo)
      : CharInfo(charInfo)
      , RenderInfo(renderInfo)
    {
    }

    constexpr bool operator==(const SpriteFontCharInfo& rhs) const
    {
      return CharInfo == rhs.CharInfo && RenderInfo == rhs.RenderInfo;
    }

    constexpr bool operator!=(const SpriteFontCharInfo& rhs) const
    {
      return !(*this == rhs);
    }

    constexpr inline uint32_t GetId() const noexcept
    {
      return CharInfo.Id;
    }

    constexpr inline void SetId(const uint32_t id) noexcept
    {
      CharInfo.Id = id;
    }
  };
}

#endif
