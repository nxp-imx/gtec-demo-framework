/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Font/EmbeddedFont8x8.hpp>
#include <cassert>

namespace Fsl
{
  namespace Vulkan
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

    NativeGraphicsBasic2D::NativeGraphicsBasic2D(const std::shared_ptr<QuadBatch>& quadBatch, const Point2& currentResolution)
      : m_batch2D(quadBatch, currentResolution)
      , m_currentResolution(currentResolution)
      , m_fontSize(EmbeddedFont8x8::CharacterSize())
      , m_inBegin(false)
    {
    }


    NativeGraphicsBasic2D::~NativeGraphicsBasic2D() = default;


    void NativeGraphicsBasic2D::SetScreenResolution(const Point2& currentResolution)
    {
      assert(!m_inBegin);
      m_currentResolution = currentResolution;
      m_batch2D.SetScreenResolution(currentResolution);
    }


    void NativeGraphicsBasic2D::Begin()
    {
      assert(!m_inBegin);
      m_inBegin = true;

      // Query the old state. This is unfortunately necessary since we have to restore it.
      if (m_resources.IsValid)
      {
        m_batch2D.Begin(BlendState::Opaque, true);
      }
    }


    void NativeGraphicsBasic2D::End()
    {
      assert(m_inBegin);
      m_inBegin = false;

      if (m_resources.IsValid)
      {
        // Restore the state we modified to ensure that we don't modify the state too much behind the app's back
        m_batch2D.End();
      }
    }


    void NativeGraphicsBasic2D::DrawPoints(const Vector2* const pDst, const uint32_t length, const Color& color)
    {
      assert(m_inBegin);
      assert(pDst != nullptr);

      if (m_resources.IsValid)
      {
        m_batch2D.Draw(m_resources.FontTexture, pDst, length, m_fillPixelRect, color);
      }
    }


    void NativeGraphicsBasic2D::DrawString(const StringViewLite& strView, const Vector2& dstPosition)
    {
      assert(m_inBegin);

      if (strView.empty() || !m_resources.IsValid)
      {
        return;
      }

      const Color colorWhite = Color::White();
      Vector2 dstPos = dstPosition;

      // build the arrays needed to render
      const char* pSrc = strView.data();
      const char* const pSrcEnd = pSrc + strView.size();

      const int32_t charWidth = m_fontSize.X;

      // Handle leading 'non drawable chars' by skipping them
      while (pSrc < pSrcEnd && !IsValidChar(int(*pSrc)))
      {
        dstPos.X += charWidth;
        ++pSrc;
      }

      // Build the 'glyph' list and x-adjust
      while (pSrc < pSrcEnd)
      {
        if (IsValidChar(int(*pSrc)))
        {
          m_batch2D.Draw(m_resources.FontTexture, dstPos, m_charRects[*pSrc - MIN_VALUE], colorWhite);
        }
        FSLLOG3_WARNING_IF(*pSrc == 0, "Zero is not a valid character in a string!");
        dstPos.X += charWidth;
        ++pSrc;
      }
    }


    Point2 NativeGraphicsBasic2D::FontSize() const
    {
      return m_fontSize;
    }

    void NativeGraphicsBasic2D::VulkanDeviceInit(const std::shared_ptr<VulkanImageCreator>& imageCreator)
    {
      if (!imageCreator)
      {
        throw std::invalid_argument("imageCreator can not be null");
      }
      try
      {
        m_resources.IsValid = true;
        m_resources.ImageCreator = imageCreator;
        m_resources.FontTexture = CreateFontTexture(*imageCreator);
      }
      catch (const std::exception&)
      {
        VulkanDeviceShutdown();
        throw;
      }
    }

    void NativeGraphicsBasic2D::VulkanDeviceShutdown() noexcept
    {
      m_resources = {};
    }


    VUTexture NativeGraphicsBasic2D::CreateFontTexture(VulkanImageCreator& rImageCreator)
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

      VkSamplerCreateInfo samplerCreateInfo{};
      samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
      samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
      samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerCreateInfo.mipLodBias = 0.0f;
      samplerCreateInfo.anisotropyEnable = VK_FALSE;
      samplerCreateInfo.maxAnisotropy = 1.0f;
      samplerCreateInfo.compareEnable = VK_FALSE;
      samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerCreateInfo.minLod = 0.0f;
      samplerCreateInfo.maxLod = 0.0f;
      samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
      return rImageCreator.CreateTexture(fontBitmap, samplerCreateInfo, "NativeGraphicsBasic2DFont");
    }
  }
}
