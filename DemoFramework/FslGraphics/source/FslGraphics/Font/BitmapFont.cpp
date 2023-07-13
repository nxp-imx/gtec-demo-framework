#ifndef FSLGRAPHICS_FONT_BITMAPFONTDECODER_HPP
#define FSLGRAPHICS_FONT_BITMAPFONTDECODER_HPP
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

#include <FslGraphics/Font/BitmapFont.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <limits>
#include <utility>

namespace Fsl
{
  namespace
  {
    void Validate(const std::vector<BitmapFontChar>& chars)
    {
      if (!chars.empty())
      {
        uint32_t previousEntry = chars[0].Id;
        for (std::size_t i = 1; i < chars.size(); ++i)
        {
          if (chars[i].Id == previousEntry)
          {
            throw std::invalid_argument(fmt::format("chars can not contain duplicated id's. Found {} at least twice.", previousEntry));
          }
          previousEntry = chars[i].Id;
        }
      }
    }

    void Validate(const std::vector<BitmapFontKerning>& kernings)
    {
      if (!kernings.empty())
      {
        const BitmapFontKerning* pPreviousEntry = &kernings[0];
        for (std::size_t i = 1; i < kernings.size(); ++i)
        {
          assert(pPreviousEntry != nullptr);
          if (kernings[i].First == pPreviousEntry->First && kernings[i].Second == pPreviousEntry->Second)
          {
            throw std::invalid_argument(fmt::format("Kernings can not contain duplicated pairs. Found 'first {} second {}' at least twice.",
                                                    pPreviousEntry->First, pPreviousEntry->Second));
          }
          pPreviousEntry = &kernings[i];
        }
      }
    }

  }

  BitmapFont::BitmapFont(std::string name, const uint16_t dpi, const uint16_t size, const PxValueU16 lineSpacingPx, const PxValueU16 baseLinePx,
                         const PxThicknessU16& paddingPx, std::string textureName, const BitmapFontType fontType, const SdfParams& sdfParams,
                         std::vector<BitmapFontChar> chars, std::vector<BitmapFontKerning> kernings)
    : m_name(std::move(name))
    , m_textureName(std::move(textureName))
    , m_dpi(dpi)
    , m_size(size)
    , m_lineSpacingPx(lineSpacingPx)
    , m_baseLinePx(baseLinePx)
    , m_paddingPx(paddingPx)
    , m_fontType(fontType)
    , m_sdfParams(sdfParams)
    , m_chars(std::move(chars))
    , m_kernings(std::move(kernings))
  {
    if (m_dpi <= 0u)
    {
      throw std::invalid_argument("dpi can not be zero");
    }

    // Sort the chars
    std::sort(m_chars.begin(), m_chars.end(), [](const BitmapFontChar& lhs, const BitmapFontChar& rhs) { return lhs.Id < rhs.Id; });

    // Sort the kernings
    std::sort(m_kernings.begin(), m_kernings.end(),
              [](const BitmapFontKerning& lhs, const BitmapFontKerning& rhs)
              { return lhs.First < rhs.First || (lhs.First == rhs.First && lhs.Second < rhs.Second); });

    // These checks expect the vectors to be sorted
    Validate(m_chars);
    Validate(m_kernings);
  }


  BitmapFont::BitmapFont(const StringViewLite name, const uint16_t dpi, const uint16_t size, const PxValueU16 lineSpacingPx,
                         const PxValueU16 baseLinePx, const PxThicknessU16& paddingPx, const StringViewLite textureName,
                         const BitmapFontType fontType, const SdfParams& sdfParams, const ReadOnlySpan<BitmapFontChar> chars,
                         const ReadOnlySpan<BitmapFontKerning> kernings)
    : BitmapFont(StringViewLiteUtil::ToString(name), dpi, size, lineSpacingPx, baseLinePx, paddingPx, StringViewLiteUtil::ToString(textureName),
                 fontType, sdfParams, ReadOnlySpanUtil::ToVector(chars), ReadOnlySpanUtil::ToVector(kernings))
  {
  }


  BitmapFont::BitmapFont(const StringViewLite name, const uint16_t dpi, const uint16_t size, const PxValueU16 lineSpacingPx,
                         const PxValueU16 baseLinePx, const PxThicknessU16& paddingPx, const StringViewLite textureName,
                         const BitmapFontType fontType, const SdfParams& sdfParams, std::vector<BitmapFontChar> chars,
                         std::vector<BitmapFontKerning> kernings)
    : BitmapFont(StringViewLiteUtil::ToString(name), dpi, size, lineSpacingPx, baseLinePx, paddingPx, StringViewLiteUtil::ToString(textureName),
                 fontType, sdfParams, std::move(chars), std::move(kernings))
  {
  }


  // const BitmapFontChar* BitmapFont::TryGetChar(const uint32_t id) const
  //{
  //  std::size_t length = m_chars.size();
  //  const BitmapFontChar* pChar = m_chars.data();

  //  while (length > 0)
  //  {
  //    auto index = length / 2;
  //    pChar += index;
  //    if (id < pChar->Id)
  //    {
  //      length = index;
  //      pChar -= index;
  //    }
  //    else if (id > pChar->Id)
  //    {
  //      length -= index;
  //    }
  //    else
  //    {
  //      // we found it
  //      return pChar;
  //    }
  //  }
  //  return nullptr;
  //}


  // int32_t BitmapFont::GetKerning(const uint32_t first, const uint32_t second) const
  //{
  //  auto itrFind = std::lower_bound(m_kernings.begin(), m_kernings.end(), first,
  //                                  [](const BitmapFontKerning& current, const uint32_t& key) { return current.First < key; });

  //  while (itrFind != m_kernings.end() && itrFind->First == first && itrFind->Second < second)
  //  {
  //    if (itrFind->Second == second)
  //    {
  //      return itrFind->AmountPx;
  //    }
  //    ++itrFind;
  //  }
  //  return 0;
  //}
}

#endif
