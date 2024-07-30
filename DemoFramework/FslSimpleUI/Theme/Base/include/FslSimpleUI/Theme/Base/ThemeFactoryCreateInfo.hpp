#ifndef FSLSIMPLEUI_THEME_BASE_THEMEFACTORYCREATEINFO_HPP
#define FSLSIMPLEUI_THEME_BASE_THEMEFACTORYCREATEINFO_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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
#include <FslGraphics/Sprite/Material/SpriteMaterialId.hpp>
#include <FslSimpleUI/Base/UIColorSpace.hpp>
#include <memory>
#include <utility>

namespace Fsl::UI::Theme
{
  struct ThemeFactoryCreateInfo
  {
    const SpriteMaterialId DefaultOpaqueMaterialId;
    const SpriteMaterialId DefaultTransparentMaterialId;
    const SpriteMaterialId DefaultLineListOpaqueMaterialId;
    const SpriteMaterialId DefaultLineListTransparentMaterialId;
    const std::shared_ptr<SpriteFont> DefaultFont;
    const std::shared_ptr<SpriteFont> DefaultHeaderFont;
    const bool UsePrimaryPalette;
    const bool DisableOpaqueMaterials;
    const UI::UIColorSpace DefaultColorSpace;

    ThemeFactoryCreateInfo(const SpriteMaterialId defaultOpaqueMaterialId, const SpriteMaterialId defaultTransparentMaterialId,
                           const SpriteMaterialId defaultLineListOpaqueMaterialId, const SpriteMaterialId defaultLineListTransparentMaterialId,
                           std::shared_ptr<SpriteFont> defaultFont, std::shared_ptr<SpriteFont> defaultHeaderFont, const bool usePrimaryPalette,
                           const bool disableOpaqueMaterials, const UI::UIColorSpace defaultColorSpace)
      : DefaultOpaqueMaterialId(!disableOpaqueMaterials ? defaultOpaqueMaterialId : defaultTransparentMaterialId)
      , DefaultTransparentMaterialId(defaultTransparentMaterialId)
      , DefaultLineListOpaqueMaterialId(!disableOpaqueMaterials ? defaultLineListOpaqueMaterialId : defaultLineListTransparentMaterialId)
      , DefaultLineListTransparentMaterialId(defaultLineListTransparentMaterialId)
      , DefaultFont(std::move(defaultFont))
      , DefaultHeaderFont(std::move(defaultHeaderFont))
      , UsePrimaryPalette(usePrimaryPalette)
      , DisableOpaqueMaterials(disableOpaqueMaterials)
      , DefaultColorSpace(defaultColorSpace)
    {
    }
  };
}

#endif
