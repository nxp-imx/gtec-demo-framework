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

#include "BitmapFont.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Font/EmbeddedFont8x8.hpp>
#include <VG/openvg.h>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>

namespace Fsl
{
  BitmapFont::BitmapFont(const DemoAppConfig& config)
    : DemoAppVG(config)
    , m_font(VG_INVALID_HANDLE)
    , m_angle(0)
    , m_scale(0)
  {
    // Create the parent image
    Bitmap bitmap;
    EmbeddedFont8x8::CreateFontBitmap(bitmap, PixelFormat::R8G8B8A8_UNORM, PxSize2D(), RectangleSizeRestrictionFlag::Power2);

    // Other filtering possibilities:
    // VG_IMAGE_QUALITY_NONANTIALIASED
    // VG_IMAGE_QUALITY_FASTER
    // VG_IMAGE_QUALITY_BETTER
    m_fontImage.Reset(bitmap, VG_IMAGE_QUALITY_FASTER);

    const uint8_t firstChar = 33;
    const uint8_t lastChar = 127;
    const uint8_t numChars = (lastChar - firstChar) + 1;

    assert(EmbeddedFont8x8::MinCharacter() <= firstChar);
    assert(EmbeddedFont8x8::MaxCharacter() >= lastChar);

    m_font = vgCreateFont(numChars);

    // Create child images for each glyph and assign them to the valid chars
    const VGFont parentImage = m_fontImage.GetHandle();
    const int32_t imageWidth = bitmap.RawWidth();
    const int32_t imageHeight = bitmap.RawHeight();
    const PxSize2D fontSize = EmbeddedFont8x8::CharacterSize();
    std::array<VGfloat, 2> origin = {0.0f, 0.0f};
    std::array<VGfloat, 2> escapement = {static_cast<VGfloat>(fontSize.RawWidth()), 0.0f};
    int32_t srcX = 0;
    int32_t srcY = 0;
    for (uint16_t i = 0; i < numChars; ++i)
    {
      const VGFont childImage = vgChildImage(parentImage, srcX, imageHeight - srcY - fontSize.RawHeight(), fontSize.RawWidth(), fontSize.RawHeight());
      vgSetGlyphToImage(m_font, firstChar + i, childImage, origin.data(), escapement.data());
      m_fontImages[i].Reset(childImage, fontSize);

      srcX += fontSize.RawWidth();
      if ((srcX + fontSize.RawWidth()) > imageWidth)
      {
        srcX = 0;
        srcY += fontSize.RawHeight();
      }
    }
  }


  BitmapFont::~BitmapFont()
  {
    if (m_font != VG_INVALID_HANDLE)
    {
      vgDestroyFont(m_font);
    }
  }


  void BitmapFont::Update(const DemoTime& demoTime)
  {
    m_scale = std::sin(m_angle);
    m_angle += 0.5f * demoTime.DeltaTime;
  }


  void BitmapFont::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    const PxSize2D currentSizePx = GetWindowSizePx();
    vgClear(0, 0, currentSizePx.RawWidth(), currentSizePx.RawHeight());

    // Draw one glyph
    {
      const std::array<float, 2> origin = {0, 0};
      vgSetfv(VG_GLYPH_ORIGIN, UncheckedNumericCast<VGint>(origin.size()), origin.data());
      vgSeti(VG_MATRIX_MODE, VG_MATRIX_GLYPH_USER_TO_SURFACE);
      vgLoadIdentity();
      vgTranslate(0, 10);
      vgDrawGlyph(m_font, '1', VG_FILL_PATH, VG_TRUE);
    }

    // Draw multiple glyphs
    {
      const std::array<float, 2> origin = {0, 0};
      vgSetfv(VG_GLYPH_ORIGIN, UncheckedNumericCast<VGint>(origin.size()), origin.data());
      vgSeti(VG_MATRIX_MODE, VG_MATRIX_GLYPH_USER_TO_SURFACE);
      vgLoadIdentity();

      std::array<VGuint, 11> glyphs = {'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd', '!'};
      std::array<VGfloat, 11> xadjust = {0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0};
      vgDrawGlyphs(m_font, UncheckedNumericCast<VGint>(glyphs.size()), glyphs.data(), xadjust.data(), nullptr, VG_FILL_PATH, VG_TRUE);
    }

    {    // Draw scaling 'Hello World!'
      const std::array<float, 2> origin = {0, 0};
      vgSetfv(VG_GLYPH_ORIGIN, UncheckedNumericCast<VGint>(origin.size()), origin.data());
      vgSeti(VG_MATRIX_MODE, VG_MATRIX_GLYPH_USER_TO_SURFACE);
      vgLoadIdentity();
      vgTranslate(100, 50);
      vgScale(m_scale, m_scale);

      std::array<VGuint, 11> glyphs = {'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd', '!'};
      std::array<VGfloat, 11> xadjust = {0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0};
      vgDrawGlyphs(m_font, UncheckedNumericCast<VGint>(glyphs.size()), glyphs.data(), xadjust.data(), nullptr, VG_FILL_PATH, VG_TRUE);
    }

    {    // Draw rotating 'Hello World!'
      const std::array<float, 2> origin = {0, 0};
      vgSetfv(VG_GLYPH_ORIGIN, UncheckedNumericCast<VGint>(origin.size()), origin.data());
      vgSeti(VG_MATRIX_MODE, VG_MATRIX_GLYPH_USER_TO_SURFACE);
      vgLoadIdentity();
      vgTranslate(100, 100);
      vgRotate(m_angle * 360);

      std::array<VGuint, 11> glyphs = {'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd', '!'};
      std::array<VGfloat, 11> xadjust = {0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0};
      vgDrawGlyphs(m_font, UncheckedNumericCast<VGint>(glyphs.size()), glyphs.data(), xadjust.data(), nullptr, VG_FILL_PATH, VG_TRUE);
    }

    {    // Draw the parent font image
      vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
      vgLoadIdentity();
      vgTranslate(200, 200);
      vgDrawImage(m_fontImage.GetHandle());
    }
  }
}
