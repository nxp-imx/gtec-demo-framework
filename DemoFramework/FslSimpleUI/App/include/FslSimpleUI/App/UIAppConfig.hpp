#ifndef FSLSIMPLEUI_APP_UIAPPCONFIG_HPP
#define FSLSIMPLEUI_APP_UIAPPCONFIG_HPP
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

#include <FslGraphics/Sprite/Material/SpriteMaterialId.hpp>

namespace Fsl
{
  namespace UIAppConfig
  {
    namespace MaterialId
    {
      constexpr SpriteMaterialId Default(0x00u);

      constexpr SpriteMaterialId DefaultUI_Opaque(0x01u);
      constexpr SpriteMaterialId DefaultUI_AlphaBlend(0x02u);

      constexpr SpriteMaterialId DefaultUI_SdfFont(0x03u);

      //! The custom Sprite id's are in the range: CustomSpriteOffset >= custin-id < DynamicOffset
      constexpr SpriteMaterialId CustomSpriteOffset(0x10000000u);

      // All sprite id's >= DynamicOffset are reserved for dynamic id's
      constexpr SpriteMaterialId DynamicOffset(0x80000000u);
    }
  }
}

#endif
