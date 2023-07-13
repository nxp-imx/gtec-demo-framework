#ifndef FSLGRAPHICS_FONT_BITMAPFONT_HPP
#define FSLGRAPHICS_FONT_BITMAPFONT_HPP
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

#include <FslBase/Math/Pixel/PxThicknessU16.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslGraphics/Font/BitmapFontChar.hpp>
#include <FslGraphics/Font/BitmapFontKerning.hpp>
#include <FslGraphics/Font/BitmapFontType.hpp>
#include <string>
#include <vector>

namespace Fsl
{
  class BitmapFont final
  {
  public:
    struct SdfParams
    {
      // If this is zero it means its not defined
      uint16_t Spread{0};
      float Scale{1.0f};

      constexpr SdfParams() noexcept = default;
      constexpr SdfParams(const uint16_t spread, const float scale) noexcept
        : Spread(spread)
        , Scale(scale)
      {
      }

      constexpr bool operator==(const SdfParams& rhs) const noexcept
      {
        return Spread == rhs.Spread && Scale == rhs.Scale;
      }
      constexpr bool operator!=(const SdfParams& rhs) const noexcept
      {
        return !(*this == rhs);
      }
    };

  private:
    //! This is the name of the true type font
    std::string m_name;

    //! The name of the texture
    std::string m_textureName;

    uint16_t m_dpi{};

    //! The size of the true type font.
    uint16_t m_size{};

    //! This is the distance in pixels between each line of text.
    PxValueU16 m_lineSpacingPx{};

    //! The number of pixels from the absolute top of the line to the base of the characters.
    PxValueU16 m_baseLinePx{};

    PxThicknessU16 m_paddingPx;

    //! The name of the font type
    BitmapFontType m_fontType{BitmapFontType::Bitmap};

    SdfParams m_sdfParams;

    // A sorted vector of chars (sorted on Id low to high)
    std::vector<BitmapFontChar> m_chars;

    // A sorted vector of kernings (sorted on first,second low to high)
    std::vector<BitmapFontKerning> m_kernings;

  public:
    BitmapFont() = default;

    BitmapFont(std::string name, const uint16_t dpi, const uint16_t size, const PxValueU16 lineSpacingPx, const PxValueU16 baseLinePx,
               const PxThicknessU16& paddingPx, std::string textureName, const BitmapFontType fontType, const SdfParams& sdfParams,
               std::vector<BitmapFontChar> chars, std::vector<BitmapFontKerning> kernings);

    BitmapFont(const StringViewLite name, const uint16_t dpi, const uint16_t size, const PxValueU16 lineSpacingPx, const PxValueU16 baseLinePx,
               const PxThicknessU16& paddingPx, const StringViewLite textureName, const BitmapFontType fontType, const SdfParams& sdfParams,
               const ReadOnlySpan<BitmapFontChar> chars, const ReadOnlySpan<BitmapFontKerning> kernings);

    BitmapFont(const StringViewLite name, const uint16_t dpi, const uint16_t size, const PxValueU16 lineSpacingPx, const PxValueU16 baseLinePx,
               const PxThicknessU16& paddingPx, const StringViewLite textureName, const BitmapFontType fontType, const SdfParams& sdfParams,
               std::vector<BitmapFontChar> chars, std::vector<BitmapFontKerning> kernings);


    StringViewLite GetName() const
    {
      return StringViewLiteUtil::AsStringViewLite(m_name);
    }

    StringViewLite GetTextureName() const
    {
      return StringViewLiteUtil::AsStringViewLite(m_textureName);
    }

    uint16_t GetDpi() const
    {
      return m_dpi;
    }

    uint16_t GetSize() const
    {
      return m_size;
    }

    PxValueU16 GetLineSpacingPx() const
    {
      return m_lineSpacingPx;
    }

    PxValueU16 GetBaseLinePx() const
    {
      return m_baseLinePx;
    }

    PxThicknessU16 GetPaddingPx() const
    {
      return m_paddingPx;
    }

    BitmapFontType GetFontType() const
    {
      return m_fontType;
    }

    inline std::size_t GetCharCount() const
    {
      return m_chars.size();
    }

    inline ReadOnlySpan<BitmapFontChar> GetChars() const
    {
      return ReadOnlySpanUtil::AsSpan(m_chars);
    }

    inline std::size_t GetKerningsCount() const
    {
      return m_kernings.size();
    }

    inline ReadOnlySpan<BitmapFontKerning> GetKernings() const
    {
      return ReadOnlySpanUtil::AsSpan(m_kernings);
    }

    SdfParams GetSdfParams() const
    {
      return m_sdfParams;
    }

    void SetSdfParams(const SdfParams params)
    {
      m_sdfParams = params;
    }

    // const BitmapFontChar* TryGetChar(const uint32_t id) const;

    ////! Lookup the kerning adjustment for the second glyph when its following the first.
    // int32_t GetKerning(const uint32_t first, const uint32_t second) const;
  };
}

#endif
