#ifndef FSLGRAPHICS_SPRITE_FONT_SPRITEFONTFASTLOOKUP_HPP
#define FSLGRAPHICS_SPRITE_FONT_SPRITEFONTFASTLOOKUP_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2023 NXP
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

#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxValueU16.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslGraphics/Font/BasicFlatHashTable.hpp>
#include <FslGraphics/Font/BitmapFontChar.hpp>
#include <FslGraphics/Font/BitmapFontKerningFastLookup.hpp>
#include <FslGraphics/Font/BitmapFontType.hpp>
#include <FslGraphics/Sprite/Info/SpriteFontCharInfo.hpp>
#include <cassert>
#include <utility>
#include <vector>

namespace Fsl
{
  class BitmapFont;
  class SpriteNativeAreaCalc;

  class SpriteFontFastLookup final
  {
    BasicFlatHashTable<SpriteFontCharInfo, uint32_t, 256u> m_charLookup;
    BitmapFontKerningFastLookup m_kerningLookup;

    //! This is the distance in pixels between each line of text.
    PxValueU16 m_lineSpacingPx;

    //! The number of pixels from the absolute top of the line to the base of the characters.
    PxValueU16 m_baseLinePx;

    //! The name of the font type
    BitmapFontType m_fontType{BitmapFontType::Bitmap};

  public:
    // move assignment operator
    SpriteFontFastLookup& operator=(SpriteFontFastLookup&& other) noexcept
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
        other.m_lineSpacingPx = {};
        other.m_baseLinePx = {};
        other.m_fontType = BitmapFontType::Bitmap;
      }
      return *this;
    }

    // move constructor
    SpriteFontFastLookup(SpriteFontFastLookup&& other) noexcept
      : m_charLookup(std::move(other.m_charLookup))
      , m_kerningLookup(std::move(other.m_kerningLookup))
      , m_lineSpacingPx(other.m_lineSpacingPx)
      , m_baseLinePx(other.m_baseLinePx)
      , m_fontType(other.m_fontType)
    {
      other.m_lineSpacingPx = {};
      other.m_baseLinePx = {};
      other.m_fontType = BitmapFontType::Bitmap;
    }

    SpriteFontFastLookup(const SpriteFontFastLookup&) = delete;
    SpriteFontFastLookup& operator=(const SpriteFontFastLookup&) = delete;

    SpriteFontFastLookup() = default;

    explicit SpriteFontFastLookup(const SpriteNativeAreaCalc& spriteNativeAreaCalc, const PxExtent2D textureExtentPx, const BitmapFont& bitmapFont,
                                  const uint32_t densityDpi);

    //! Lookup the char
    inline const SpriteFontCharInfo* TryGetChar(const uint32_t id) const
    {
      return m_charLookup.TryGet(id);
    }

    inline const SpriteFontCharInfo& GetChar(const uint32_t id, const SpriteFontCharInfo& defaultChar) const
    {
      return m_charLookup.Get(id, defaultChar);
    }

    //! Lookup the kerning adjustment for the second glyph when its following the first.
    inline PxValue GetKerning(const uint32_t first, const uint32_t second) const
    {
      const auto* pEntry = m_kerningLookup.TryGet(first, second);
      return pEntry != nullptr ? pEntry->AmountPx : PxValue();
    }

    inline PxValueU16 GetLineSpacingPx() const noexcept
    {
      return m_lineSpacingPx;
    }

    inline PxValueU16 GetBaseLinePx() const noexcept
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
      m_lineSpacingPx = {};
      m_baseLinePx = {};
      m_fontType = BitmapFontType::Bitmap;
    }

  private:
    static BasicFlatHashTable<SpriteFontCharInfo, uint32_t, 256u> Populate(const SpriteNativeAreaCalc& spriteNativeAreaCalc,
                                                                           const PxExtent2D textureExtentPx,
                                                                           const ReadOnlySpan<BitmapFontChar> srcSpan, const uint32_t imageDpi,
                                                                           const uint32_t densityDpi);
  };
}

#endif
