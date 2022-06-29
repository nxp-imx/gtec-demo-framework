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
#include <FslBase/Log/IO/FmtPathView.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslGraphics/Sprite/BasicNineSliceSprite.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslGraphics/Sprite/Font/SpriteFontConfig.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslGraphics/Sprite/OptimizedBasicNineSliceSprite.hpp>
#include <FslGraphics/Sprite/OptimizedNineSliceSprite.hpp>
#include <FslGraphics/Sprite/SpriteManager.hpp>
#include <FslGraphics/TextureAtlas/AtlasNineSlicePatchInfo.hpp>
#include <FslGraphics/TextureAtlas/AtlasTextureInfo.hpp>
#include <algorithm>
#include <utility>

namespace Fsl
{
  SpriteManager::SpriteManager(const uint32_t densityDpi, const bool useYFlipTextureCoordinates)
    : m_spriteNativeAreaCalc(useYFlipTextureCoordinates)
    , m_densityDpi(densityDpi)
  {
    if (densityDpi <= 0)
    {
      throw std::invalid_argument("densityDpi must be >= 0");
    }
  }


  std::size_t SpriteManager::Count() const
  {
    return m_sprites.size();
  }


  void SpriteManager::Clear()
  {
    m_sprites.clear();
  }

  void SpriteManager::Add(std::shared_ptr<ISprite> sprite)
  {
    if (Contains(sprite))
    {
      throw std::invalid_argument("sprite already added");
    }

    m_sprites.emplace_back(std::move(sprite));
  }

  void SpriteManager::Remove(const std::shared_ptr<ISprite>& sprite)
  {
    m_sprites.remove(sprite);
  }


  bool SpriteManager::Contains(const std::shared_ptr<ISprite>& sprite) const
  {
    return std::find(m_sprites.begin(), m_sprites.end(), sprite) != m_sprites.end();
  }


  void SpriteManager::Resize(const uint32_t densityDpi)
  {
    if (densityDpi <= 0)
    {
      throw std::invalid_argument("densityDpi must be >= 0");
    }
    for (const auto& entry : m_sprites)
    {
      entry->Resize(densityDpi);
    }
    m_densityDpi = densityDpi;
  }


  std::shared_ptr<BasicImageSprite> SpriteManager::AddBasicImageSprite(const SpriteMaterialInfo& spriteMaterialInfo,
                                                                       const AtlasTextureInfo& textureInfo, const IO::PathView& debugName)
  {
    if (!spriteMaterialInfo.IsValid())
    {
      throw std::invalid_argument("spriteMaterialInfo must be valid");
    }

    if (textureInfo.TrimMarginPx.SumX() != 0u || textureInfo.TrimMarginPx.SumY() != 0u)
    {
      throw UsageErrorException("BasicImageSprite image can not contain trim");
    }

    auto sprite =
      std::make_shared<BasicImageSprite>(m_spriteNativeAreaCalc, spriteMaterialInfo, TypeConverter::To<PxRectangleU16>(textureInfo.TrimmedRectPx),
                                         textureInfo.Dpi, debugName, m_densityDpi);

    Add(sprite);
    return sprite;
  }


  std::shared_ptr<ImageSprite> SpriteManager::AddImageSprite(const SpriteMaterialInfo& spriteMaterialInfo, const AtlasTextureInfo& textureInfo,
                                                             const IO::PathView& debugName)
  {
    if (!spriteMaterialInfo.IsValid())
    {
      throw std::invalid_argument("spriteMaterialInfo must be valid");
    }

    auto sprite =
      std::make_shared<ImageSprite>(m_spriteNativeAreaCalc, spriteMaterialInfo, textureInfo.TrimMarginPx,
                                    TypeConverter::To<PxRectangleU16>(textureInfo.TrimmedRectPx), textureInfo.Dpi, debugName, m_densityDpi);

    Add(sprite);
    return sprite;
  }

  void SpriteManager::Patch(const std::shared_ptr<IImageSprite>& sprite, const SpriteMaterialInfo& spriteMaterialInfo,
                            const AtlasTextureInfo& textureInfo, const IO::PathView& debugName)
  {
    IImageSprite* const pSprite = sprite.get();
    if (pSprite == nullptr)
    {
      throw std::invalid_argument("sprite can not be null");
    }
    if (!spriteMaterialInfo.IsValid())
    {
      throw std::invalid_argument("spriteMaterialInfo must be valid");
    }

    {    // Try BasicImageSprite
      auto* pImageSprite = dynamic_cast<BasicImageSprite*>(pSprite);
      if (pImageSprite != nullptr)
      {
        if (textureInfo.TrimMarginPx.SumX() != 0u || textureInfo.TrimMarginPx.SumY() != 0u)
        {
          throw UsageErrorException("BasicImageSprite image can not contain trim");
        }

        pImageSprite->SetContent(m_spriteNativeAreaCalc, spriteMaterialInfo, TypeConverter::To<PxRectangleU16>(textureInfo.TrimmedRectPx),
                                 textureInfo.Dpi, debugName, m_densityDpi);
        return;
      }
    }
    {    // Try ImageSprite
      auto* pImageSprite = dynamic_cast<ImageSprite*>(pSprite);
      if (pImageSprite != nullptr)
      {
        pImageSprite->SetContent(m_spriteNativeAreaCalc, spriteMaterialInfo, textureInfo.TrimMarginPx,
                                 TypeConverter::To<PxRectangleU16>(textureInfo.TrimmedRectPx), textureInfo.Dpi, debugName, m_densityDpi);
        return;
      }
    }
    throw NotSupportedException("Unsupported sprite type");
  }

  void SpriteManager::Patch(const std::shared_ptr<IImageSprite>& sprite, const SpriteMaterialInfo& spriteMaterialInfo0,
                            const SpriteMaterialInfo& spriteMaterialInfo1, const AtlasTextureInfo& textureInfo, const IO::PathView& debugName)
  {
    FSL_PARAM_NOT_USED(textureInfo);
    FSL_PARAM_NOT_USED(debugName);
    IImageSprite* const pSprite = sprite.get();
    if (pSprite == nullptr)
    {
      throw std::invalid_argument("sprite can not be null");
    }
    if (!spriteMaterialInfo0.IsValid())
    {
      throw std::invalid_argument("spriteMaterialInfo0 must be valid");
    }
    if (!spriteMaterialInfo1.IsValid())
    {
      throw std::invalid_argument("spriteMaterialInfo1 must be valid");
    }

    //{    // Try BasicImageSprite
    //  auto* pImageSprite = dynamic_cast<BasicImageSprite*>(pSprite);
    //  if (pImageSprite != nullptr)
    //  {
    //    if (textureInfo.TrimMarginPx.SumX() != 0u || textureInfo.TrimMarginPx.SumY() != 0u)
    //    {
    //      throw UsageErrorException("BasicImageSprite image can not contain trim");
    //    }

    //    pImageSprite->SetContent(m_spriteNativeAreaCalc, spriteMaterialInfo, textureInfo.TrimmedRectPx, textureInfo.Dpi, debugName, m_densityDpi);
    //    return;
    //  }
    //}
    //{    // Try ImageSprite
    //  auto* pImageSprite = dynamic_cast<ImageSprite*>(pSprite);
    //  if (pImageSprite != nullptr)
    //  {
    //    pImageSprite->SetContent(m_spriteNativeAreaCalc, spriteMaterialInfo, textureInfo.TrimMarginPx, textureInfo.TrimmedRectPx, textureInfo.Dpi,
    //                             debugName, m_densityDpi);
    //    return;
    //  }
    //}
    throw NotSupportedException("Unsupported sprite type");
  }


  std::shared_ptr<BasicNineSliceSprite> SpriteManager::AddBasicNineSliceSprite(const SpriteMaterialInfo& spriteMaterialInfo,
                                                                               const AtlasTextureInfo& textureInfo,
                                                                               const AtlasNineSlicePatchInfo& patchInfo,
                                                                               const IO::PathView& debugName)
  {
    if (!spriteMaterialInfo.IsValid())
    {
      throw std::invalid_argument("spriteMaterialInfo must be valid");
    }

    if (textureInfo.TrimMarginPx.SumX() != 0u || textureInfo.TrimMarginPx.SumY() != 0u)
    {
      throw UsageErrorException("BasicNineSliceSprite image can not contain trim");
    }

    auto sprite =
      std::make_shared<BasicNineSliceSprite>(m_spriteNativeAreaCalc, spriteMaterialInfo, TypeConverter::To<PxRectangleU16>(textureInfo.TrimmedRectPx),
                                             patchInfo.NineSlicePx, patchInfo.ContentMarginPx, textureInfo.Dpi, debugName, m_densityDpi);

    Add(sprite);
    return sprite;
  }


  std::shared_ptr<NineSliceSprite> SpriteManager::AddNineSliceSprite(const SpriteMaterialInfo& spriteMaterialInfo,
                                                                     const AtlasTextureInfo& textureInfo, const AtlasNineSlicePatchInfo& patchInfo,
                                                                     const IO::PathView& debugName)
  {
    if (!spriteMaterialInfo.IsValid())
    {
      throw std::invalid_argument("spriteMaterialInfo must be valid");
    }

    auto sprite = std::make_shared<NineSliceSprite>(m_spriteNativeAreaCalc, spriteMaterialInfo, textureInfo.TrimMarginPx,
                                                    TypeConverter::To<PxRectangleU16>(textureInfo.TrimmedRectPx), patchInfo.NineSlicePx,
                                                    patchInfo.ContentMarginPx, textureInfo.Dpi, debugName, m_densityDpi);

    Add(sprite);
    return sprite;
  }


  std::shared_ptr<OptimizedBasicNineSliceSprite>
    SpriteManager::AddOptimizedBasicNineSliceSprite(const SpriteMaterialInfo& opaqueSpriteMaterialInfo,
                                                    const SpriteMaterialInfo& transparentSpriteMaterialInfo, const AtlasTextureInfo& textureInfo,
                                                    const AtlasNineSlicePatchInfo& patchInfo, const IO::PathView& debugName)
  {
    if (!opaqueSpriteMaterialInfo.IsValid())
    {
      throw std::invalid_argument("opaqueSpriteMaterialInfo must be valid");
    }
    if (!transparentSpriteMaterialInfo.IsValid())
    {
      throw std::invalid_argument("transparentSpriteMaterialInfo must be valid");
    }

    if (textureInfo.TrimMarginPx.SumX() != 0u || textureInfo.TrimMarginPx.SumY() != 0u)
    {
      throw UsageErrorException("BasicNineSliceSprite image can not contain trim");
    }

    auto sprite = std::make_shared<OptimizedBasicNineSliceSprite>(
      m_spriteNativeAreaCalc, opaqueSpriteMaterialInfo, transparentSpriteMaterialInfo, TypeConverter::To<PxRectangleU16>(textureInfo.TrimmedRectPx),
      patchInfo.NineSlicePx, patchInfo.ContentMarginPx, patchInfo.Flags, textureInfo.Dpi, debugName, m_densityDpi);

    Add(sprite);
    return sprite;
  }


  std::shared_ptr<OptimizedNineSliceSprite> SpriteManager::AddOptimizedNineSliceSprite(const SpriteMaterialInfo& opaqueSpriteMaterialInfo,
                                                                                       const SpriteMaterialInfo& transparentSpriteMaterialInfo,
                                                                                       const AtlasTextureInfo& textureInfo,
                                                                                       const AtlasNineSlicePatchInfo& patchInfo,
                                                                                       const IO::PathView& debugName)
  {
    if (!opaqueSpriteMaterialInfo.IsValid())
    {
      throw std::invalid_argument("opaqueSpriteMaterialInfo must be valid");
    }
    if (!transparentSpriteMaterialInfo.IsValid())
    {
      throw std::invalid_argument("transparentSpriteMaterialInfo must be valid");
    }

    auto sprite = std::make_shared<OptimizedNineSliceSprite>(m_spriteNativeAreaCalc, opaqueSpriteMaterialInfo, transparentSpriteMaterialInfo,
                                                             textureInfo.TrimMarginPx, TypeConverter::To<PxRectangleU16>(textureInfo.TrimmedRectPx),
                                                             patchInfo.NineSlicePx, patchInfo.ContentMarginPx, patchInfo.Flags, textureInfo.Dpi,
                                                             debugName, m_densityDpi);

    Add(sprite);
    return sprite;
  }


  void SpriteManager::Patch(const std::shared_ptr<INineSliceSprite>& sprite, const SpriteMaterialInfo& spriteMaterialInfo,
                            const AtlasTextureInfo& textureInfo, const AtlasNineSlicePatchInfo& patchInfo, const IO::PathView& debugName)
  {
    INineSliceSprite* const pSprite = sprite.get();
    if (pSprite == nullptr)
    {
      throw std::invalid_argument("sprite can not be null");
    }
    if (!spriteMaterialInfo.IsValid())
    {
      throw std::invalid_argument("spriteMaterialInfo must be valid");
    }
    {    // Try BasicNineSliceSprite
      auto* pNineSliceSprite = dynamic_cast<BasicNineSliceSprite*>(pSprite);
      if (pNineSliceSprite != nullptr)
      {
        if (textureInfo.TrimMarginPx.SumX() != 0u || textureInfo.TrimMarginPx.SumY() != 0u)
        {
          throw UsageErrorException("BasicImageSprite image can not contain trim");
        }

        pNineSliceSprite->SetContent(m_spriteNativeAreaCalc, spriteMaterialInfo, TypeConverter::To<PxRectangleU16>(textureInfo.TrimmedRectPx),
                                     patchInfo.NineSlicePx, patchInfo.ContentMarginPx, textureInfo.Dpi, debugName, m_densityDpi);
        return;
      }
    }
    {    // Try NineSliceSprite
      auto* pNineSliceSprite = dynamic_cast<NineSliceSprite*>(pSprite);
      if (pNineSliceSprite != nullptr)
      {
        pNineSliceSprite->SetContent(m_spriteNativeAreaCalc, spriteMaterialInfo, textureInfo.TrimMarginPx,
                                     TypeConverter::To<PxRectangleU16>(textureInfo.TrimmedRectPx), patchInfo.NineSlicePx, patchInfo.ContentMarginPx,
                                     textureInfo.Dpi, debugName, m_densityDpi);
        return;
      }
    }
    throw NotSupportedException("Unsupported sprite type");
  }


  void SpriteManager::Patch(const std::shared_ptr<INineSliceSprite>& sprite, const SpriteMaterialInfo& spriteMaterialInfo0,
                            const SpriteMaterialInfo& spriteMaterialInfo1, const AtlasTextureInfo& textureInfo,
                            const AtlasNineSlicePatchInfo& patchInfo, const IO::PathView& debugName)
  {
    INineSliceSprite* const pSprite = sprite.get();
    if (pSprite == nullptr)
    {
      throw std::invalid_argument("sprite can not be null");
    }
    if (!spriteMaterialInfo0.IsValid())
    {
      throw std::invalid_argument("spriteMaterialInfo0 must be valid");
    }
    if (!spriteMaterialInfo1.IsValid())
    {
      throw std::invalid_argument("spriteMaterialInfo1 must be valid");
    }
    {    // Try OptimizedBasicNineSliceSprite
      auto* pNineSliceSprite = dynamic_cast<OptimizedBasicNineSliceSprite*>(pSprite);
      if (pNineSliceSprite != nullptr)
      {
        if (textureInfo.TrimMarginPx.SumX() != 0u || textureInfo.TrimMarginPx.SumY() != 0u)
        {
          throw UsageErrorException("BasicImageSprite image can not contain trim");
        }

        pNineSliceSprite->SetContent(m_spriteNativeAreaCalc, spriteMaterialInfo0, spriteMaterialInfo1,
                                     TypeConverter::To<PxRectangleU16>(textureInfo.TrimmedRectPx), patchInfo.NineSlicePx, patchInfo.ContentMarginPx,
                                     patchInfo.Flags, textureInfo.Dpi, debugName, m_densityDpi);
        return;
      }
    }
    {    // Try NineSliceSprite
      auto* pNineSliceSprite = dynamic_cast<OptimizedNineSliceSprite*>(pSprite);
      if (pNineSliceSprite != nullptr)
      {
        pNineSliceSprite->SetContent(m_spriteNativeAreaCalc, spriteMaterialInfo0, spriteMaterialInfo1, textureInfo.TrimMarginPx,
                                     TypeConverter::To<PxRectangleU16>(textureInfo.TrimmedRectPx), patchInfo.NineSlicePx, patchInfo.ContentMarginPx,
                                     patchInfo.Flags, textureInfo.Dpi, debugName, m_densityDpi);
        return;
      }
    }
    throw NotSupportedException("Unsupported sprite type");
  }

  std::shared_ptr<SpriteFont> SpriteManager::AddSpriteFont(const SpriteMaterialInfo& spriteMaterialInfo, const BitmapFont& bitmapFont,
                                                           const SpriteFontConfig& spriteFontConfig, const IO::PathView& debugName)
  {
    if (!spriteMaterialInfo.IsValid())
    {
      throw std::invalid_argument("spriteMaterialInfo must be valid");
    }

    auto sprite = std::make_shared<SpriteFont>(m_spriteNativeAreaCalc, spriteMaterialInfo, bitmapFont, spriteFontConfig, m_densityDpi, debugName);

    Add(sprite);
    return sprite;
  }


  void SpriteManager::PatchSpriteFont(const std::shared_ptr<SpriteFont>& font, const SpriteMaterialInfo& spriteMaterialInfo,
                                      const BitmapFont& bitmapFont, const IO::PathView& debugName)
  {
    if (!font)
    {
      throw std::invalid_argument("font can not be null");
    }

    const auto fontConfig = font->GetInfo().FontConfig;
    const SpriteFontConfig spriteFontConfig(fontConfig.Kerning);

    font->SetContent(m_spriteNativeAreaCalc, spriteMaterialInfo, bitmapFont, spriteFontConfig, m_densityDpi, debugName);
  }

}
