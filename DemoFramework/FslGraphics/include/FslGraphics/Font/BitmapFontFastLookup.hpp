#ifndef FSLGRAPHICS_FONT_BITMAPFONTFASTLOOKUP_HPP
#define FSLGRAPHICS_FONT_BITMAPFONTFASTLOOKUP_HPP
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

#include <FslGraphics/Font/BasicFlatHashTable.hpp>
#include <FslGraphics/Font/BitmapFont.hpp>
#include <FslGraphics/Font/BitmapFontKerningFastLookup.hpp>
#include <cassert>
#include <utility>
#include <vector>

namespace Fsl
{
  class BitmapFont;

  class BitmapFontFastLookup final
  {
    BasicFlatHashTable<BitmapFontChar, uint32_t, 256u> m_charLookup;
    BitmapFontKerningFastLookup m_kerningLookup;

    //! This is the distance in pixels between each line of text.
    uint16_t m_lineSpacingPx{};

    //! The number of pixels from the absolute top of the line to the base of the characters.
    uint16_t m_baseLinePx{};

    //! The name of the font type
    BitmapFontType m_fontType{BitmapFontType::Bitmap};

  public:
    // move assignment operator
    BitmapFontFastLookup& operator=(BitmapFontFastLookup&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        Clear();

        // Claim ownership here
        m_charLookup = std::move(other.m_charLookup);
        m_kerningLookup = std::move(other.m_kerningLookup);
        m_lineSpacingPx = other.m_lineSpacingPx;
        m_baseLinePx = other.m_baseLinePx;
        m_fontType = other.m_fontType;

        // Remove the data from other
        other.m_lineSpacingPx = 0;
        other.m_baseLinePx = 0;
        other.m_fontType = BitmapFontType::Bitmap;
      }
      return *this;
    }

    // move constructor
    BitmapFontFastLookup(BitmapFontFastLookup&& other) noexcept
      : m_charLookup(std::move(other.m_charLookup))
      , m_kerningLookup(std::move(other.m_kerningLookup))
      , m_lineSpacingPx(other.m_lineSpacingPx)
      , m_baseLinePx(other.m_baseLinePx)
      , m_fontType(other.m_fontType)
    {
      other.m_lineSpacingPx = 0;
      other.m_baseLinePx = 0;
      other.m_fontType = BitmapFontType::Bitmap;
    }

    BitmapFontFastLookup(const BitmapFontFastLookup&) = delete;
    BitmapFontFastLookup& operator=(const BitmapFontFastLookup&) = delete;

    BitmapFontFastLookup() = default;

    explicit BitmapFontFastLookup(const BitmapFont& bitmapFont)
      : m_charLookup(bitmapFont.GetChars())
      , m_kerningLookup(bitmapFont.GetKernings())
      , m_lineSpacingPx(bitmapFont.GetLineSpacingPx())
      , m_baseLinePx(bitmapFont.GetBaseLinePx())
      , m_fontType(bitmapFont.GetFontType())
    {
    }

    //! Lookup the char
    inline const BitmapFontChar* TryGetChar(const uint32_t id) const
    {
      return m_charLookup.TryGet(id);
    }

    inline const BitmapFontChar& GetChar(const uint32_t id, const BitmapFontChar& defaultChar) const
    {
      return m_charLookup.Get(id, defaultChar);
    }

    //! Lookup the kerning adjustment for the second glyph when its following the first.
    inline int32_t GetKerning(const uint32_t first, const uint32_t second) const
    {
      const auto* pEntry = m_kerningLookup.TryGet(first, second);
      return pEntry != nullptr ? pEntry->AmountPx : 0;
    }

    inline uint16_t GetLineSpacingPx() const
    {
      return m_lineSpacingPx;
    }

    inline uint16_t GetBaseLinePx() const
    {
      return m_baseLinePx;
    }

    inline bool HasKerning() const
    {
      return !m_kerningLookup.Empty();
    }

    void Clear() noexcept
    {
      m_charLookup = {};
      m_kerningLookup = {};
      m_lineSpacingPx = 0;
      m_baseLinePx = 0;
      m_fontType = BitmapFontType::Bitmap;
    }
  };
}

#endif
