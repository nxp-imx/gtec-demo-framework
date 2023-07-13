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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslGraphics/Font/BitmapFont.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslGraphics/Sprite/Font/SpriteFontConfig.hpp>
#include <FslGraphics/Sprite/SpriteUnitConverter.hpp>
#include <cassert>
#include <utility>

namespace Fsl
{
  SpriteFont::SpriteFont(const SpriteNativeAreaCalc& spriteNativeAreaCalc, const SpriteMaterialInfo& spriteMaterialInfo, const BitmapFont& bitmapFont,
                         const SpriteFontConfig& spriteFontConfig, const uint32_t densityDpi, const StringViewLite& debugName)
    : m_info(spriteMaterialInfo, bitmapFont.GetLineSpacingPx(), bitmapFont.GetBaseLinePx(), bitmapFont.GetDpi(), spriteFontConfig.EnableKerning,
             bitmapFont.GetFontType() == BitmapFontType::SDF, bitmapFont.GetSdfParams().Scale, debugName)
    , m_bitmapFontAtlas(spriteNativeAreaCalc, spriteMaterialInfo.ExtentPx, bitmapFont, densityDpi)
  {
    FSL_PARAM_NOT_USED(spriteNativeAreaCalc);

    Resize(densityDpi);
  }

  void SpriteFont::ClearContent()
  {
    m_info.MaterialInfo.Material.reset();
    m_bitmapFontAtlas.Reset();
  }


  void SpriteFont::SetContent(const SpriteNativeAreaCalc& spriteNativeAreaCalc, const SpriteMaterialInfo& spriteMaterialInfo,
                              const BitmapFont& bitmapFont, const SpriteFontConfig& spriteFontConfig, const uint32_t densityDpi,
                              const StringViewLite& debugName)
  {
    FSL_PARAM_NOT_USED(spriteNativeAreaCalc);

    m_info =
      SpriteFontInfo(spriteMaterialInfo, bitmapFont.GetLineSpacingPx(), bitmapFont.GetBaseLinePx(), bitmapFont.GetDpi(),
                     spriteFontConfig.EnableKerning, bitmapFont.GetFontType() == BitmapFontType::SDF, bitmapFont.GetSdfParams().Scale, debugName);
    m_bitmapFontAtlas.Reset(spriteNativeAreaCalc, spriteMaterialInfo.ExtentPx, bitmapFont, densityDpi);
    Resize(densityDpi);
  }


  const SpriteMaterialInfo& SpriteFont::GetMaterialInfo(const uint32_t index) const
  {
    FSL_PARAM_NOT_USED(index);
    assert(index == 0u);
    return m_info.MaterialInfo;
  }


  void SpriteFont::Resize(const uint32_t densityDpi)
  {
    SpriteUnitConverter unitConverter(densityDpi);
    auto scale = unitConverter.CalcImageDensityScale(m_info.ImageDpi);
    if (m_info.IsSdfBased && m_info.SdfScale != 1.0f)
    {
      scale *= m_info.SdfScale;
    }
    m_info.FontConfig.Scale = scale;
    m_info.ScaledLineSpacingPx = TypeConverter::ChangeTo<PxValueU16>(PxValueF(static_cast<float>(m_info.LineSpacingPx.Value) * scale));
    m_info.ScaledBaseLinePx = TypeConverter::ChangeTo<PxValueU16>(PxValueF(static_cast<float>(m_info.BaseLinePx.Value) * scale));
  }

  PxSize2D SpriteFont::MeasureString(const StringViewLite& strView) const
  {
    return m_bitmapFontAtlas.MeasureString(strView, m_info.FontConfig);
  }
}
