#ifndef FSLSIMPLEUI_BASE_CONTROL_IMPL_IMAGEIMPL_BASICIMAGESPRITE_HPP
#define FSLSIMPLEUI_BASE_CONTROL_IMPL_IMAGEIMPL_BASICIMAGESPRITE_HPP
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

#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/Adapter/INativeTexture2D.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslGraphics/Sprite/Material/ISpriteMaterial.hpp>
#include <FslSimpleUI/Base/PxAvailableSize.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/UIScaleUtil.hpp>
#include <memory>

namespace Fsl
{
  namespace UI
  {
    //! @brief Since we have multiple controls that need this code its been isolated here as inline methods
    namespace ImageImpl
    {
      inline static void WinDraw(const UIDrawContext& context, const BasicImageSprite* const pSprite, const Color& color,
                                 const std::shared_ptr<INativeBatch2D>& batch2D)
      {
        if (pSprite != nullptr)
        {
          const auto& info = pSprite->GetInfo();
          const auto* const pNativeTexture = dynamic_cast<const INativeTexture2D*>(info.MaterialInfo.Material.get());
          if (pNativeTexture != nullptr)
          {
            // BlendState is stored in the NativeMaterialFlags
            batch2D->ChangeTo(static_cast<BlendState>(info.MaterialInfo.NativeMaterialFlags));

            batch2D->Draw(*pNativeTexture, info.RenderInfo.TextureArea, context.TargetRect, color);
          }
        }
      }

      inline static PxSize2D ArrangeOverride(const PxSize2D& finalSizePx, const BasicImageSprite* const pSprite, const ItemScalePolicy scalepolicy)
      {
        PxSize2D calcedSizePx;
        if (pSprite != nullptr)
        {
          if (!UIScaleUtil::TryCalcSize(calcedSizePx, finalSizePx, pSprite->GetRenderInfo().ScaledSizePx, scalepolicy))
          {
            return finalSizePx;
          }
        }
        return calcedSizePx;
      }

      inline static PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx, const BasicImageSprite* const pSprite)
      {
        FSL_PARAM_NOT_USED(availableSizePx);
        return pSprite != nullptr ? pSprite->GetRenderInfo().ScaledSizePx : PxSize2D();
      }
    }
  }
}

#endif
