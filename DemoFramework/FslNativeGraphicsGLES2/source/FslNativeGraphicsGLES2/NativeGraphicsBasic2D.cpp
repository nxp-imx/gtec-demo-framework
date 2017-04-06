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

#include "NativeGraphicsBasic2D.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Font/EmbeddedFont8x8.hpp>
#include <FslGraphicsGLES2/GLUtil.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    const char MIN_VALUE = 33;
    const char MAX_VALUE = 127;

    inline bool IsValidChar(const int ch)
    {
      return (ch >= int(MIN_VALUE) && ch <= int(MAX_VALUE));
    }
  }


  namespace GLES2
  {
    NativeGraphicsBasic2D::NativeGraphicsBasic2D(const std::shared_ptr<GLBatch2DQuadRenderer>& batchQuadRender, const Point2& currentResolution)
      : m_batch2D(batchQuadRender, currentResolution)
      , m_fontTexture()
      , m_currentResolution(currentResolution)
      , m_fontSize(EmbeddedFont8x8::CharacterSize())
      , m_fillPixelRect()
      , m_inBegin(false)
    {
      Bitmap fontBitmap;
      EmbeddedFont8x8::CreateFontBitmap(fontBitmap, PixelFormat::R8G8B8A8_UNORM, Point2(2, 2), RectangleSizeRestrictionFlag::Power2);

      // Create child images for each glyph and assign them to the valid chars
      {
        const uint8_t firstChar = MIN_VALUE;
        const uint8_t lastChar = MAX_VALUE;
        const uint8_t numChars = (lastChar - firstChar) + 1;

        assert(EmbeddedFont8x8::MinCharacter() <= firstChar);
        assert(EmbeddedFont8x8::MaxCharacter() >= lastChar);

        const int32_t imageWidth = fontBitmap.Width();
        Point2 fontSize = EmbeddedFont8x8::CharacterSize();
        fontSize.X += 2;
        fontSize.Y += 2;
        int32_t srcX = 0;
        int32_t srcY = 0;
        for (std::size_t i = 0; i < numChars; ++i)
        {
          m_charRects[i] = Rectangle(srcX, srcY, fontSize.X, fontSize.Y);
          srcX += fontSize.X;
          if ((srcX + fontSize.X) > imageWidth)
          {
            srcX = 0;
            srcY += fontSize.Y;
          }
        }
      }

      assert(fontBitmap.Width() >= 9);
      assert(fontBitmap.Height() >= 9);
      for (uint32_t y = fontBitmap.Height() - 9; y < fontBitmap.Height(); ++y)
      {
        for (uint32_t x = fontBitmap.Width() - 9; x < fontBitmap.Width(); ++x)
        {
          fontBitmap.SetNativePixel(x, y, 0xFFFFFFFF);
        }
      }
      m_fillPixelRect = Rectangle(fontBitmap.Width() - 4, fontBitmap.Height() - 4, 1, 1);

      // Because GLES requires upside down textures.
      BitmapUtil::FlipHorizontal(fontBitmap);

      GLTextureParameters texParams(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
      m_fontTexture.SetData(fontBitmap, texParams);
    }


    NativeGraphicsBasic2D::~NativeGraphicsBasic2D()
    {
    }


    void NativeGraphicsBasic2D::SetScreenResolution(const Point2& currentResolution)
    {
      assert(!m_inBegin);
      m_currentResolution = currentResolution;
    }


    void NativeGraphicsBasic2D::Begin()
    {
      assert(!m_inBegin);
      m_inBegin = true;

      // Query the old state. This is unfortunately necessary since we have to restore it.
      m_batch2D.Begin(BlendState::Opaque, true);
    }


    void NativeGraphicsBasic2D::End()
    {
      assert(m_inBegin);
      m_inBegin = false;

      // Restore the state we modified to ensure that we don't modify the state too much behind the app's back
      m_batch2D.End();
    }


    void NativeGraphicsBasic2D::DrawPoints(const Vector2*const pDst, const uint32_t length, const Color& color)
    {
      assert(m_inBegin);
      assert(pDst != nullptr);

      m_batch2D.Draw(m_fontTexture, pDst, length, m_fillPixelRect, color);
    }


    void NativeGraphicsBasic2D::DrawString(const char*const characters, const uint32_t length, const Vector2& dstPosition)
    {
      assert(m_inBegin);
      assert(characters != nullptr);

      if (length == 0)
        return;

      const Color colorWhite = Color::White();
      Vector2 dstPos = dstPosition;


      // build the arrays needed to render
      const char* pSrc = characters;
      const char*const pSrcEnd = pSrc + length;

      const int32_t charWidth = m_fontSize.X;

      // Handle leading 'non drawable chars' by skipping them
      while (pSrc < pSrcEnd && ! IsValidChar(int(*pSrc)))
      {
        dstPos.X += charWidth;
        ++pSrc;
      }

      // Build the 'glyph' list and x-adjust
      while (pSrc < pSrcEnd)
      {
        if (IsValidChar(int(*pSrc)))
        {
          m_batch2D.Draw(m_fontTexture, dstPos, m_charRects[*pSrc - MIN_VALUE], colorWhite);
        }
        FSLLOG_WARNING_IF(*pSrc == 0, "Zero is not a valid character in a string!");
        dstPos.X += charWidth;
        ++pSrc;
      }
    }


    Point2 NativeGraphicsBasic2D::FontSize() const
    {
      return m_fontSize;
    }
  }
}
