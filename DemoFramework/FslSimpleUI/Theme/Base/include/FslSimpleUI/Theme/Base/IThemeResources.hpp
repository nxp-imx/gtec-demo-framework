#ifndef FSLSIMPLEUI_THEME_BASE_ITHEMERESOURCES_HPP
#define FSLSIMPLEUI_THEME_BASE_ITHEMERESOURCES_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslSimpleUI/Theme/Base/IconType.hpp>
#include <FslSimpleUI/Theme/Base/WindowType.hpp>
#include <memory>

namespace Fsl
{
  class BasicImageSprite;
  class ImageSprite;
  class INineSliceSprite;
  class NineSliceSprite;
  class SpriteFont;

  namespace UI
  {
    namespace Theme
    {
      class IThemeResources
      {
      public:
        virtual ~IThemeResources() = default;

        virtual const std::shared_ptr<INineSliceSprite>& GetBackground(const WindowType windowType) const = 0;

        virtual const std::shared_ptr<BasicImageSprite>& GetBasicFillSprite(const bool opaque = true) const = 0;
        virtual const std::shared_ptr<BasicImageSprite>& GetBasicMiniFillSprite(const bool opaque) const = 0;
        virtual const std::shared_ptr<BasicImageSprite>& GetBasicTestSprite(const bool opaque = true) const = 0;

        virtual const std::shared_ptr<ImageSprite>& GetFillSprite() const = 0;
        virtual const std::shared_ptr<SpriteFont>& GetDefaultSpriteFont() const = 0;
        virtual const std::shared_ptr<NineSliceSprite>& GetToolTipNineSliceSprite() const = 0;
        virtual const std::shared_ptr<INineSliceSprite>& GetDialogNineSliceSprite() const = 0;
        virtual const std::shared_ptr<INineSliceSprite>& GetDividerNineSliceSprite() const = 0;
        virtual const std::shared_ptr<INineSliceSprite>& GetColorMarkerNineSliceSprite() const = 0;
        virtual const std::shared_ptr<ImageSprite>& GetIconSprite(const IconType iconType) const = 0;
      };
    }
  }
}

#endif
