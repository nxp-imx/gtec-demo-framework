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

#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslGraphics/Font/BitmapFont.hpp>
#include <FslGraphics/Sprite/Font/SpriteFontConfig.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <utility>

namespace Fsl
{
  SpriteFont::SpriteFont(const SpriteMaterialInfo& spriteMaterialInfo, const uint32_t imageDpi, const BitmapFont& bitmapFont,
                         const SpriteFontConfig& spriteFontConfig, const uint32_t densityDpi, const StringViewLite& debugName)
    : m_info(spriteMaterialInfo, bitmapFont.GetLineSpacingPx(), bitmapFont.GetBaseLinePx(), imageDpi, spriteFontConfig.EnableKerning, debugName)
    , m_bitmapFontAtlas(bitmapFont)
  {
    Resize(densityDpi);
  }

  void SpriteFont::ClearContent()
  {
    m_info.MaterialInfo.Material.reset();
    m_bitmapFontAtlas.Reset();
  }


  void SpriteFont::SetContent(const SpriteMaterialInfo& spriteMaterialInfo, const uint32_t imageDpi, const BitmapFont& bitmapFont,
                              const SpriteFontConfig& spriteFontConfig, const uint32_t densityDpi, const StringViewLite& debugName)
  {
    m_info = SpriteFontInfo(spriteMaterialInfo, bitmapFont.GetLineSpacingPx(), bitmapFont.GetBaseLinePx(), imageDpi, spriteFontConfig.EnableKerning,
                            debugName);
    m_bitmapFontAtlas.Reset(bitmapFont);
    Resize(densityDpi);
  }


  const SpriteMaterialInfo& SpriteFont::GetMaterialInfo() const
  {
    return m_info.MaterialInfo;
  }


  void SpriteFont::Resize(const uint32_t densityDpi)
  {
    SpriteUnitConverter unitConverter(densityDpi);
    const auto scale = unitConverter.CalcImageDensityScale(m_info.ImageDpi);
    m_info.FontConfig.Scale = scale;
    m_info.ScaledLineSpacingPx = TypeConverter::ChangeTo<uint16_t>(static_cast<float>(m_info.LineSpacingPx) * scale);
    m_info.ScaledBaseLinePx = TypeConverter::ChangeTo<uint16_t>(static_cast<float>(m_info.BaseLinePx) * scale);
  }

  PxSize2D SpriteFont::MeasureString(const StringViewLite& strView) const
  {
    return m_bitmapFontAtlas.MeasureString(strView, m_info.FontConfig);
  }
}
