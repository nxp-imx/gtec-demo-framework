#ifndef FSLGRAPHICS_FONT_BITMAPFONTKERNING_HPP
#define FSLGRAPHICS_FONT_BITMAPFONTKERNING_HPP
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

#include <cstdint>

namespace Fsl
{
  struct BitmapFontKerning
  {
    uint32_t First{};
    uint32_t Second{};
    int32_t AmountPx{};

    constexpr BitmapFontKerning() = default;

    constexpr BitmapFontKerning(const uint32_t first, const uint32_t second, const int32_t amountPx)
      : First(first)
      , Second(second)
      , AmountPx(amountPx)
    {
    }


    constexpr bool operator==(const BitmapFontKerning& rhs) const
    {
      return First == rhs.First && Second == rhs.Second && AmountPx == rhs.AmountPx;
    }

    constexpr bool operator!=(const BitmapFontKerning& rhs) const
    {
      return !(*this == rhs);
    }

    constexpr uint64_t GetId() const
    {
      return (uint64_t(First) << 32) | uint64_t(Second);
    }

    constexpr void SetId(const uint64_t value)
    {
      First = uint32_t((value >> 32) & 0xFFFFFFFF);
      Second = uint32_t(value & 0xFFFFFFFF);
    }

    static inline constexpr uint64_t ToId(const uint32_t first, const uint32_t second)
    {
      return (uint64_t(first) << 32) | uint64_t(second);
    }
  };
}

#endif
