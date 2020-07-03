#ifndef FSLGRAPHICS_SPRITE_ISPRITERESOURCEMANAGER_HPP
#define FSLGRAPHICS_SPRITE_ISPRITERESOURCEMANAGER_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialInfo.hpp>

namespace Fsl
{
  namespace IO
  {
    struct PathView;
    class Path;
  }

  class BasicNineSliceSprite;
  class BasicImageSprite;
  class NineSliceSprite;
  struct SpriteMaterialId;
  class SpriteFont;
  struct SpriteFontConfig;
  class ImageSprite;


  //! @brief Platform independent interface for creating sprite based resources
  class ISpriteResourceManager
  {
  public:
    ISpriteResourceManager() = default;
    virtual ~ISpriteResourceManager() = default;

    //! Get the material based on the material id (platform independent)
    virtual SpriteMaterialInfo GetSpriteMaterialInfo(const SpriteMaterialId& spriteMaterialId) = 0;

    //! Create a sprite image based on the given material id (platform independent)
    virtual std::shared_ptr<BasicImageSprite> CreateBasicImageSprite(const SpriteMaterialId& spriteMaterialId, const IO::PathView& atlasPathName) = 0;

    //! Create a sprite image based on the given material id (platform independent)
    virtual std::shared_ptr<BasicImageSprite> CreateBasicImageSprite(const SpriteMaterialId& spriteMaterialId, const IO::Path& atlasPathName) = 0;

    //! Create a sprite image based on the given material id (platform independent)
    virtual std::shared_ptr<ImageSprite> CreateImageSprite(const SpriteMaterialId& spriteMaterialId, const IO::PathView& atlasPathName) = 0;

    //! Create a sprite image based on the given material id (platform independent)
    virtual std::shared_ptr<ImageSprite> CreateImageSprite(const SpriteMaterialId& spriteMaterialId, const IO::Path& atlasPathName) = 0;

    //! Create a basic nine slice sprite based on the given material id (platform independent)
    virtual std::shared_ptr<BasicNineSliceSprite> CreateBasicNineSliceSprite(const SpriteMaterialId& spriteMaterialId,
                                                                             const IO::PathView& atlasPathName) = 0;

    //! Create a basic nine slice sprite based on the given material id (platform independent)
    virtual std::shared_ptr<BasicNineSliceSprite> CreateBasicNineSliceSprite(const SpriteMaterialId& spriteMaterialId,
                                                                             const IO::Path& atlasPathName) = 0;

    //! Create a nine slice sprite based on the given material id (platform independent)
    virtual std::shared_ptr<NineSliceSprite> CreateNineSliceSprite(const SpriteMaterialId& spriteMaterialId, const IO::PathView& atlasPathName) = 0;

    //! Create a nine slice sprite based on the given material id (platform independent)
    virtual std::shared_ptr<NineSliceSprite> CreateNineSliceSprite(const SpriteMaterialId& spriteMaterialId, const IO::Path& atlasPathName) = 0;


    //! Create a sprite font based on the given material id (platform independent)
    virtual std::shared_ptr<SpriteFont> CreateLegacySpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::PathView& fontName,
                                                               const SpriteFontConfig& spriteFontConfig) = 0;
    //! Create a sprite font based on the given material id (platform independent)
    virtual std::shared_ptr<SpriteFont> CreateLegacySpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::Path& fontName,
                                                               const SpriteFontConfig& spriteFontConfig) = 0;

    //! Create a sprite font based on the given material id (platform independent)
    virtual std::shared_ptr<SpriteFont> CreateSpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::PathView& fontName,
                                                         const SpriteFontConfig& spriteFontConfig) = 0;
    //! Create a sprite font based on the given material id (platform independent)
    virtual std::shared_ptr<SpriteFont> CreateSpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::Path& fontName,
                                                         const SpriteFontConfig& spriteFontConfig) = 0;
  };
}

#endif
