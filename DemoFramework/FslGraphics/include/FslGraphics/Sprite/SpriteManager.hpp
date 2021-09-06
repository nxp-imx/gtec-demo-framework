#ifndef FSLGRAPHICS_SPRITE_SPRITEMANAGER_HPP
#define FSLGRAPHICS_SPRITE_SPRITEMANAGER_HPP
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

#include <FslBase/IO/PathView.hpp>
#include <FslGraphics/Font/BitmapFont.hpp>
#include <FslGraphics/Sprite/ISprite.hpp>
#include <FslGraphics/Sprite/SpriteNativeAreaCalc.hpp>
#include <list>
#include <memory>

namespace Fsl
{
  struct AtlasNineSlicePatchInfo;
  struct AtlasTextureInfo;
  class BasicImageSprite;
  class BasicNineSliceSprite;
  class NineSliceSprite;
  class IImageSprite;
  class ImageSprite;
  class INineSliceSprite;
  class OptimizedBasicNineSliceSprite;
  class OptimizedNineSliceSprite;
  class SpriteFont;
  struct SpriteFontConfig;
  struct SpriteMaterialInfo;

  //! @brief Very basic material manager
  class SpriteManager final
  {
    SpriteNativeAreaCalc m_spriteNativeAreaCalc;
    std::list<std::shared_ptr<ISprite>> m_sprites;
    uint32_t m_densityDpi;

  public:
    explicit SpriteManager(const uint32_t densityDpi, const bool useYFlipTextureCoordinates);
    ~SpriteManager() = default;

    const SpriteNativeAreaCalc& GetSpriteNativeAreaCalc() const
    {
      return m_spriteNativeAreaCalc;
    }

    uint32_t GetDensityDpi() const
    {
      return m_densityDpi;
    }

    std::size_t Count() const;
    void Clear();
    void Add(std::shared_ptr<ISprite> sprite);
    void Remove(const std::shared_ptr<ISprite>& sprite);
    bool Contains(const std::shared_ptr<ISprite>& sprite) const;

    void Resize(const uint32_t densityDpi);

    std::shared_ptr<BasicImageSprite> AddBasicImageSprite(const SpriteMaterialInfo& spriteMaterialInfo, const AtlasTextureInfo& textureInfo,
                                                          const IO::PathView& debugName);

    std::shared_ptr<ImageSprite> AddImageSprite(const SpriteMaterialInfo& spriteMaterialInfo, const AtlasTextureInfo& textureInfo,
                                                const IO::PathView& debugName);

    void Patch(const std::shared_ptr<IImageSprite>& sprite, const SpriteMaterialInfo& spriteMaterialInfo, const AtlasTextureInfo& textureInfo,
               const IO::PathView& debugName);
    void Patch(const std::shared_ptr<IImageSprite>& sprite, const SpriteMaterialInfo& spriteMaterialInfo0,
               const SpriteMaterialInfo& spriteMaterialInfo1, const AtlasTextureInfo& textureInfo, const IO::PathView& debugName);

    std::shared_ptr<BasicNineSliceSprite> AddBasicNineSliceSprite(const SpriteMaterialInfo& spriteMaterialInfo, const AtlasTextureInfo& textureInfo,
                                                                  const AtlasNineSlicePatchInfo& patchInfo, const IO::PathView& debugName);


    std::shared_ptr<NineSliceSprite> AddNineSliceSprite(const SpriteMaterialInfo& spriteMaterialInfo, const AtlasTextureInfo& textureInfo,
                                                        const AtlasNineSlicePatchInfo& patchInfo, const IO::PathView& debugName);

    std::shared_ptr<OptimizedBasicNineSliceSprite>
      AddOptimizedBasicNineSliceSprite(const SpriteMaterialInfo& opaqueSpriteMaterialInfo, const SpriteMaterialInfo& transparentSpriteMaterialInfo,
                                       const AtlasTextureInfo& textureInfo, const AtlasNineSlicePatchInfo& patchInfo, const IO::PathView& debugName);

    std::shared_ptr<OptimizedNineSliceSprite> AddOptimizedNineSliceSprite(const SpriteMaterialInfo& opaqueSpriteMaterialInfo,
                                                                          const SpriteMaterialInfo& transparentSpriteMaterialInfo,
                                                                          const AtlasTextureInfo& textureInfo,
                                                                          const AtlasNineSlicePatchInfo& patchInfo, const IO::PathView& debugName);

    void Patch(const std::shared_ptr<INineSliceSprite>& sprite, const SpriteMaterialInfo& spriteMaterialInfo, const AtlasTextureInfo& textureInfo,
               const AtlasNineSlicePatchInfo& patchInfo, const IO::PathView& debugName);
    void Patch(const std::shared_ptr<INineSliceSprite>& sprite, const SpriteMaterialInfo& spriteMaterialInfo0,
               const SpriteMaterialInfo& spriteMaterialInfo1, const AtlasTextureInfo& textureInfo, const AtlasNineSlicePatchInfo& patchInfo,
               const IO::PathView& debugName);

    std::shared_ptr<SpriteFont> AddSpriteFont(const SpriteMaterialInfo& spriteMaterialInfo, const BitmapFont& bitmapFont,
                                              const SpriteFontConfig& spriteFontConfig, const IO::PathView& debugName);

    void PatchSpriteFont(const std::shared_ptr<SpriteFont>& font, const SpriteMaterialInfo& spriteMaterialInfo, const BitmapFont& bitmapFont,
                         const IO::PathView& debugName);
  };
}

#endif
