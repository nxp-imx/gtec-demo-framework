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

#include <FslGraphics/Font/TextureAtlasBitmapFont.hpp>
#include <FslGraphics/Font/BitmapFontConverter.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>

namespace Fsl
{
  namespace
  {
    inline int32_t ScaledBaseLine(const int32_t baseLinePx, const float fontScale)
    {
      return int32_t(std::round(static_cast<float>(baseLinePx) * std::max(fontScale, 0.0f)));
    }

    inline PxSize2D DoMeasureString(const BitmapFontFastLookup& lookup, const BitmapFontChar& unknownChar, const StringViewLite& strView)
    {
      int32_t renderRightPx = 0;
      int32_t renderBottomPx = 0;
      if (!strView.empty())
      {
        // extract information about the characters we are rendering
        int32_t layoutXOffsetPx = 0;
        for (std::size_t index = 0; index < strView.size(); ++index)
        {
          const BitmapFontChar& charInfo = lookup.GetChar(strView[index], unknownChar);
          if (charInfo.SrcTextureRectPx.Width > 0u && charInfo.SrcTextureRectPx.Height > 0u)
          {
            const int32_t currentRightPx = layoutXOffsetPx + charInfo.OffsetPx.X + charInfo.SrcTextureRectPx.Width;
            const int32_t currentBottomPx = charInfo.OffsetPx.Y + charInfo.SrcTextureRectPx.Height;
            renderRightPx = (currentRightPx > renderRightPx ? currentRightPx : renderRightPx);
            renderBottomPx = (currentBottomPx > renderBottomPx ? currentBottomPx : renderBottomPx);
          }
          layoutXOffsetPx += charInfo.XAdvancePx;
        }
      }
      return {renderRightPx, renderBottomPx};
    }

    bool DoExtractRenderRules(const BitmapFontFastLookup& lookup, const BitmapFontChar& unknownChar, std::vector<FontGlyphPosition>& rDst,
                              const StringViewLite& strView)
    {
      if (strView.empty())
      {
        return false;
      }
      if (strView.length() > std::numeric_limits<uint32_t>::max())
      {
        throw std::invalid_argument("length can not be larger than a uint32");
      }
      if (rDst.size() < strView.length())
      {
        throw std::invalid_argument("rDst is too small");
      }

      // extract information about the characters we are rendering
      int32_t layoutXOffsetPx = 0;
      const auto viewLength = strView.length();
      for (uint32_t index = 0; index < viewLength; ++index)
      {
        const BitmapFontChar& charInfo = lookup.GetChar(strView[index], unknownChar);

        const PxRectangle2D dstRectPx(layoutXOffsetPx + charInfo.OffsetPx.X, charInfo.OffsetPx.Y, charInfo.SrcTextureRectPx.Width,
                                      charInfo.SrcTextureRectPx.Height);
        rDst[index] = FontGlyphPosition(dstRectPx, charInfo.SrcTextureRectPx);
        layoutXOffsetPx += charInfo.XAdvancePx;
      }
      return true;
    }


    inline PxSize2D DoMeasureStringWithKerning(const BitmapFontFastLookup& lookup, const BitmapFontChar& unknownChar, const StringViewLite& strView)
    {
      int32_t renderRightPx = 0;
      int32_t renderBottomPx = 0;
      if (!strView.empty())
      {
        // extract information about the characters we are rendering
        int32_t layoutXOffsetPx = 0;
        uint32_t previousChar = 0;
        for (std::size_t index = 0; index < strView.size(); ++index)
        {
          const uint32_t currentChar = static_cast<unsigned char>(strView[index]);
          const BitmapFontChar& charInfo = lookup.GetChar(currentChar, unknownChar);
          // apply kerning
          layoutXOffsetPx += lookup.GetKerning(previousChar, currentChar);
          if (charInfo.SrcTextureRectPx.Width > 0u && charInfo.SrcTextureRectPx.Height > 0u)
          {
            const int32_t currentRightPx = layoutXOffsetPx + charInfo.OffsetPx.X + charInfo.SrcTextureRectPx.Width;
            const int32_t currentBottomPx = charInfo.OffsetPx.Y + charInfo.SrcTextureRectPx.Height;
            renderRightPx = (currentRightPx > renderRightPx ? currentRightPx : renderRightPx);
            renderBottomPx = (currentBottomPx > renderBottomPx ? currentBottomPx : renderBottomPx);
          }
          layoutXOffsetPx += charInfo.XAdvancePx;
          previousChar = currentChar;
        }
      }
      return {renderRightPx, renderBottomPx};
    }

    bool DoExtractRenderRulesWithKerning(const BitmapFontFastLookup& lookup, const BitmapFontChar& unknownChar, std::vector<FontGlyphPosition>& rDst,
                                         const StringViewLite& strView)
    {
      if (strView.empty())
      {
        return false;
      }
      if (strView.length() > std::numeric_limits<uint32_t>::max())
      {
        throw std::invalid_argument("length can not be larger than a uint32");
      }
      if (rDst.size() < strView.length())
      {
        throw std::invalid_argument("rDst is too small");
      }

      // extract information about the characters we are rendering
      int32_t layoutXOffsetPx = 0;
      const auto viewLength = strView.length();
      uint32_t previousChar = 0;
      for (uint32_t index = 0; index < viewLength; ++index)
      {
        const uint32_t currentChar = static_cast<unsigned char>(strView[index]);
        const BitmapFontChar& charInfo = lookup.GetChar(currentChar, unknownChar);
        // apply kerning
        layoutXOffsetPx += lookup.GetKerning(previousChar, currentChar);

        const PxRectangle2D dstRectPx(layoutXOffsetPx + charInfo.OffsetPx.X, charInfo.OffsetPx.Y, charInfo.SrcTextureRectPx.Width,
                                      charInfo.SrcTextureRectPx.Height);
        rDst[index] = FontGlyphPosition(dstRectPx, charInfo.SrcTextureRectPx);
        layoutXOffsetPx += charInfo.XAdvancePx;
        previousChar = currentChar;
      }
      return true;
    }

    inline PxSize2D DoMeasureStringWithKerning(const BitmapFontFastLookup& lookup, const BitmapFontChar& unknownChar, const StringViewLite& strView,
                                               const float fontScale)
    {
      int32_t renderRightPx = 0;
      int32_t renderBottomPx = 0;
      if (!strView.empty() || fontScale <= 0.0f)
      {
        const auto baseLinePx = lookup.GetBaseLinePx();
        const auto scaledBaseLinePx = ScaledBaseLine(baseLinePx, fontScale);

        // extract information about the characters we are rendering

        float layoutXOffsetPxf = 0;
        uint32_t previousChar = 0;
        for (std::size_t index = 0; index < strView.size(); ++index)
        {
          const uint32_t currentChar = static_cast<unsigned char>(strView[index]);
          const BitmapFontChar& charInfo = lookup.GetChar(currentChar, unknownChar);

          // apply kerning
          layoutXOffsetPxf += float(lookup.GetKerning(previousChar, currentChar)) * fontScale;

          // This should match the algorithm in ExtractRenderRules
          if (charInfo.SrcTextureRectPx.Width > 0u && charInfo.SrcTextureRectPx.Height > 0u)
          {
            // calc distance from original baseline to the startY, then scale it
            // then add the distance to the scaled baseline and round it (this ensures we have high accuracy)
            // we store the kerning offset in a int32_t to ensure that the "-" operation doesn't underflow (due to unsigned subtraction)
            const int32_t glyphHeight = charInfo.SrcTextureRectPx.Height;
            auto scaledYStartPx = int32_t(std::round((scaledBaseLinePx + (float(charInfo.OffsetPx.Y - baseLinePx) * fontScale))));
            const auto scaledYEndPx = int32_t(std::round((scaledBaseLinePx + (float(charInfo.OffsetPx.Y + glyphHeight - baseLinePx) * fontScale))));

            const auto dstXPx = int32_t(std::round(layoutXOffsetPxf + (float(charInfo.OffsetPx.X) * fontScale)));
            auto dstXEndPx = int32_t(std::round(layoutXOffsetPxf + (float(charInfo.OffsetPx.X + charInfo.SrcTextureRectPx.Width) * fontScale)));

            dstXEndPx = dstXEndPx > dstXPx ? dstXEndPx : (dstXEndPx + 1);
            scaledYStartPx = scaledYStartPx < scaledYEndPx ? scaledYStartPx : (scaledYEndPx - 1);

            const int32_t currentRightPx = dstXEndPx;
            const int32_t currentBottomPx = scaledYEndPx;
            renderRightPx = (currentRightPx >= renderRightPx ? currentRightPx : renderRightPx);
            renderBottomPx = (currentBottomPx > renderBottomPx ? currentBottomPx : renderBottomPx);
          }

          layoutXOffsetPxf += float(charInfo.XAdvancePx) * fontScale;
          previousChar = currentChar;
        }
      }
      return {renderRightPx, renderBottomPx};
    }


    bool DoExtractRenderRules(const BitmapFontFastLookup& lookup, const BitmapFontChar& unknownChar, std::vector<FontGlyphPosition>& rDst,
                              const StringViewLite& strView, const float fontScale)
    {
      if (strView.empty() || fontScale <= 0.0f)
      {
        // Fill(rDst, strView.length(), {});
        return false;
      }
      if (strView.length() > std::numeric_limits<uint32_t>::max())
      {
        throw std::invalid_argument("length can not be larger than a uint32");
      }
      if (rDst.size() < strView.length())
      {
        throw std::invalid_argument("rDst is too small");
      }

      const auto baseLinePx = lookup.GetBaseLinePx();
      const auto scaledBaseLinePx = ScaledBaseLine(baseLinePx, fontScale);
      // extract information about the characters we are rendering
      float layoutXOffsetPxf = 0.0f;
      const auto viewLength = strView.length();
      for (uint32_t index = 0; index < viewLength; ++index)
      {
        const BitmapFontChar& charInfo = lookup.GetChar(strView[index], unknownChar);

        // calc distance from original baseline to the startY, then scale it
        // then add the distance to the scaled baseline and round it (this ensures we have high accuracy)
        // we store the kerning offset in a int32_t to ensure that the "-" operation doesn't underflow (due to unsigned subtraction)
        const int32_t glyphYOffsetPx = charInfo.OffsetPx.Y;
        const auto glyphHeight = UncheckedNumericCast<int32_t>(charInfo.SrcTextureRectPx.Height);
        auto scaledYStartPx = int32_t(std::round((scaledBaseLinePx + (float(glyphYOffsetPx - baseLinePx) * fontScale))));
        const auto scaledYEndPx = int32_t(std::round((scaledBaseLinePx + (float(glyphYOffsetPx + glyphHeight - baseLinePx) * fontScale))));

        const auto dstXPx = int32_t(std::round(layoutXOffsetPxf + (float(charInfo.OffsetPx.X) * fontScale)));
        auto dstXEndPx = int32_t(std::round(layoutXOffsetPxf + (float(charInfo.OffsetPx.X + charInfo.SrcTextureRectPx.Width) * fontScale)));

        if (dstXPx >= dstXEndPx)
        {
          dstXEndPx = charInfo.SrcTextureRectPx.Width > 0u ? (dstXEndPx + 1) : dstXEndPx;
        }
        if (scaledYStartPx >= scaledYEndPx)
        {
          scaledYStartPx = glyphHeight > 0 ? (scaledYEndPx - 1) : scaledYEndPx;
        }

        const auto dstRectPx = PxRectangle2D::FromLeftTopRightBottom(dstXPx, scaledYStartPx, dstXEndPx, scaledYEndPx);
        rDst[index] = FontGlyphPosition(dstRectPx, charInfo.SrcTextureRectPx);
        layoutXOffsetPxf += float(charInfo.XAdvancePx) * fontScale;
      }
      return true;
    }

    inline PxSize2D DoMeasureString(const BitmapFontFastLookup& lookup, const BitmapFontChar& unknownChar, const StringViewLite& strView,
                                    const float& fontScale)
    {
      int32_t renderRightPx = 0;
      int32_t renderBottomPx = 0;
      if (!strView.empty() || fontScale <= 0.0f)
      {
        const auto baseLinePx = lookup.GetBaseLinePx();
        const auto scaledBaseLinePx = ScaledBaseLine(baseLinePx, fontScale);

        // extract information about the characters we are rendering

        float layoutXOffsetPxf = 0;
        for (std::size_t index = 0; index < strView.size(); ++index)
        {
          const BitmapFontChar& charInfo = lookup.GetChar(strView[index], unknownChar);

          // This should match the algorithm in ExtractRenderRules
          if (charInfo.SrcTextureRectPx.Width > 0u && charInfo.SrcTextureRectPx.Height > 0u)
          {
            // calc distance from original baseline to the startY, then scale it
            // then add the distance to the scaled baseline and round it (this ensures we have high accuracy)
            // we store the kerning offset in a int32_t to ensure that the "-" operation doesn't underflow (due to unsigned subtraction)
            const int32_t glyphHeight = charInfo.SrcTextureRectPx.Height;
            auto scaledYStartPx = int32_t(std::round((scaledBaseLinePx + (float(charInfo.OffsetPx.Y - baseLinePx) * fontScale))));
            const auto scaledYEndPx = int32_t(std::round((scaledBaseLinePx + (float(charInfo.OffsetPx.Y + glyphHeight - baseLinePx) * fontScale))));

            const auto dstXPx = int32_t(std::round(layoutXOffsetPxf + (float(charInfo.OffsetPx.X) * fontScale)));
            auto dstXEndPx = int32_t(std::round(layoutXOffsetPxf + (float(charInfo.OffsetPx.X + charInfo.SrcTextureRectPx.Width) * fontScale)));

            dstXEndPx = dstXEndPx > dstXPx ? dstXEndPx : (dstXEndPx + 1);
            scaledYStartPx = scaledYStartPx < scaledYEndPx ? scaledYStartPx : (scaledYEndPx - 1);

            const int32_t currentRightPx = dstXEndPx;
            const int32_t currentBottomPx = scaledYEndPx;
            renderRightPx = (currentRightPx >= renderRightPx ? currentRightPx : renderRightPx);
            renderBottomPx = (currentBottomPx > renderBottomPx ? currentBottomPx : renderBottomPx);
          }

          layoutXOffsetPxf += float(charInfo.XAdvancePx) * fontScale;
        }
      }
      return {renderRightPx, renderBottomPx};
    }

    bool DoExtractRenderRulesWithKerning(const BitmapFontFastLookup& lookup, const BitmapFontChar& unknownChar, std::vector<FontGlyphPosition>& rDst,
                                         const StringViewLite& strView, const float fontScale)
    {
      if (strView.empty() || fontScale <= 0.0f)
      {
        // Fill(rDst, strView.length(), {});
        return false;
      }
      if (strView.length() > std::numeric_limits<uint32_t>::max())
      {
        throw std::invalid_argument("length can not be larger than a uint32");
      }
      if (rDst.size() < strView.length())
      {
        throw std::invalid_argument("rDst is too small");
      }

      const auto baseLinePx = lookup.GetBaseLinePx();
      const auto scaledBaseLinePx = ScaledBaseLine(baseLinePx, fontScale);
      // extract information about the characters we are rendering
      float layoutXOffsetPxf = 0.0f;
      const auto viewLength = strView.length();
      uint32_t previousChar = 0;
      for (uint32_t index = 0; index < viewLength; ++index)
      {
        const uint32_t currentChar = static_cast<unsigned char>(strView[index]);
        const BitmapFontChar& charInfo = lookup.GetChar(currentChar, unknownChar);
        // apply kerning
        layoutXOffsetPxf += float(lookup.GetKerning(previousChar, currentChar)) * fontScale;

        // calc distance from original baseline to the startY, then scale it
        // then add the distance to the scaled baseline and round it (this ensures we have high accuracy)
        // we store the kerning offset in a int32_t to ensure that the "-" operation doesn't underflow (due to unsigned subtraction)
        const int32_t glyphYOffsetPx = charInfo.OffsetPx.Y;
        const auto glyphHeight = UncheckedNumericCast<int32_t>(charInfo.SrcTextureRectPx.Height);
        auto scaledYStartPx = int32_t(std::round((scaledBaseLinePx + (float(glyphYOffsetPx - baseLinePx) * fontScale))));
        const auto scaledYEndPx = int32_t(std::round((scaledBaseLinePx + (float(glyphYOffsetPx + glyphHeight - baseLinePx) * fontScale))));

        const auto dstXPx = int32_t(std::round(layoutXOffsetPxf + (float(charInfo.OffsetPx.X) * fontScale)));
        auto dstXEndPx = int32_t(std::round(layoutXOffsetPxf + (float(charInfo.OffsetPx.X + charInfo.SrcTextureRectPx.Width) * fontScale)));

        if (dstXPx >= dstXEndPx)
        {
          dstXEndPx = charInfo.SrcTextureRectPx.Width > 0u ? (dstXEndPx + 1) : dstXEndPx;
        }
        if (scaledYStartPx >= scaledYEndPx)
        {
          scaledYStartPx = glyphHeight > 0 ? (scaledYEndPx - 1) : scaledYEndPx;
        }

        const auto dstRectPx = PxRectangle2D::FromLeftTopRightBottom(dstXPx, scaledYStartPx, dstXEndPx, scaledYEndPx);
        rDst[index] = FontGlyphPosition(dstRectPx, charInfo.SrcTextureRectPx);
        layoutXOffsetPxf += float(charInfo.XAdvancePx) * fontScale;
        previousChar = currentChar;
      }
      return true;
    }
  }

  TextureAtlasBitmapFont::TextureAtlasBitmapFont(const ITextureAtlas& textureAtlas, const IFontBasicKerning& basicFontKerning)
    : TextureAtlasBitmapFont(BitmapFontConverter::ToBitmapFont(textureAtlas, basicFontKerning))
  {
  }


  TextureAtlasBitmapFont::TextureAtlasBitmapFont(const BitmapFont& bitmapFont)
    : m_lookup(bitmapFont)
  {
    // dump kerning info
    // auto kerningSpan = bitmapFont.GetKernings();
    // for (std::size_t i = 0; i < kerningSpan.size(); ++i)
    //{
    //  FSLLOG3_INFO("{} {}", (char)kerningSpan[i].First, (char)kerningSpan[i].Second);
    //}
  }

  void TextureAtlasBitmapFont::Reset(const BitmapFont& bitmapFont)
  {
    Reset();
    m_lookup = BitmapFontFastLookup(bitmapFont);
  }

  void TextureAtlasBitmapFont::Reset(const ITextureAtlas& textureAtlas, const IFontBasicKerning& basicFontKerning)
  {
    Reset();
    m_lookup = BitmapFontFastLookup(BitmapFontConverter::ToBitmapFont(textureAtlas, basicFontKerning));
  }


  int32_t TextureAtlasBitmapFont::BaseLinePx(const BitmapFontConfig& fontConfig) const
  {
    return ScaledBaseLine(m_lookup.GetBaseLinePx(), fontConfig.Scale);
  }


  int32_t TextureAtlasBitmapFont::LineSpacingPx(const BitmapFontConfig& fontConfig) const
  {
    return int32_t(std::round(static_cast<float>(m_lookup.GetLineSpacingPx()) * std::max(fontConfig.Scale, 0.0f)));
  }


  PxSize2D TextureAtlasBitmapFont::MeasureString(const StringViewLite& strView) const
  {
    return m_lookup.HasKerning() ? DoMeasureStringWithKerning(m_lookup, m_unknownChar, strView) : DoMeasureString(m_lookup, m_unknownChar, strView);
  }


  PxSize2D TextureAtlasBitmapFont::MeasureString(const StringViewLite& strView, const BitmapFontConfig& fontConfig) const
  {
    PxSize2D result;
    if (fontConfig.Kerning && m_lookup.HasKerning())
    {
      result = fontConfig.Scale == 1.0f ? DoMeasureStringWithKerning(m_lookup, m_unknownChar, strView)
                                        : DoMeasureStringWithKerning(m_lookup, m_unknownChar, strView, fontConfig.Scale);
    }
    else
    {
      result = fontConfig.Scale == 1.0f ? DoMeasureString(m_lookup, m_unknownChar, strView)
                                        : DoMeasureString(m_lookup, m_unknownChar, strView, fontConfig.Scale);
    }
    return result;
  }

  bool TextureAtlasBitmapFont::ExtractRenderRules(std::vector<FontGlyphPosition>& rDst, const StringViewLite& strView) const
  {
    return m_lookup.HasKerning() ? DoExtractRenderRulesWithKerning(m_lookup, m_unknownChar, rDst, strView)
                                 : DoExtractRenderRules(m_lookup, m_unknownChar, rDst, strView);
  }

  bool TextureAtlasBitmapFont::ExtractRenderRules(std::vector<FontGlyphPosition>& rDst, const StringViewLite& strView,
                                                  const BitmapFontConfig& fontConfig) const
  {
    bool result = false;
    if (fontConfig.Kerning && m_lookup.HasKerning())
    {
      result = fontConfig.Scale == 1.0f ? DoExtractRenderRulesWithKerning(m_lookup, m_unknownChar, rDst, strView)
                                        : DoExtractRenderRulesWithKerning(m_lookup, m_unknownChar, rDst, strView, fontConfig.Scale);
    }
    else
    {
      result = fontConfig.Scale == 1.0f ? DoExtractRenderRules(m_lookup, m_unknownChar, rDst, strView)
                                        : DoExtractRenderRules(m_lookup, m_unknownChar, rDst, strView, fontConfig.Scale);
    }
    return result;
  }
}
