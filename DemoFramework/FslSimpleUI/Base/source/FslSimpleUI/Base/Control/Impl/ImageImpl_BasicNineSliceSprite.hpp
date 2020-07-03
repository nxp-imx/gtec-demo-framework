#ifndef FSLSIMPLEUI_BASE_CONTROL_IMPL_IMAGEIMPL_BASICNINESLICESPRITE_HPP
#define FSLSIMPLEUI_BASE_CONTROL_IMPL_IMAGEIMPL_BASICNINESLICESPRITE_HPP
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
#include <FslGraphics/Sprite/BasicNineSliceSprite.hpp>
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
      inline static void WinDraw(const UIDrawContext& context, const BasicNineSliceSprite* const pSprite, const PxSize2D& renderSize,
                                 const Color& color, const std::shared_ptr<INativeBatch2D>& batch2D)
      {
        if (pSprite != nullptr)
        {
          const auto& info = pSprite->GetInfo();
          const auto* const pNativeTexture = dynamic_cast<const INativeTexture2D*>(info.MaterialInfo.Material.get());
          if (pNativeTexture != nullptr)
          {
            // BlendState is stored in the NativeMaterialFlags
            batch2D->ChangeTo(static_cast<BlendState>(info.MaterialInfo.NativeMaterialFlags));

            const auto& scaledNineSlicePx = info.RenderInfo.ScaledNineSlicePx;
            const float dstX0Pxf = context.TargetRect.Left();
            const float dstY0Pxf = context.TargetRect.Top();
            const float dstX1Pxf = dstX0Pxf + static_cast<float>(scaledNineSlicePx.Left());
            const float dstY1Pxf = dstY0Pxf + static_cast<float>(scaledNineSlicePx.Top());
            const float dstX2Pxf = dstX0Pxf + static_cast<float>(renderSize.Width() - scaledNineSlicePx.Right());
            const float dstY2Pxf = dstY0Pxf + static_cast<float>(renderSize.Height() - scaledNineSlicePx.Bottom());
            const float dstX3Pxf = dstX0Pxf + static_cast<float>(renderSize.Width());
            const float dstY3Pxf = dstY0Pxf + static_cast<float>(renderSize.Height());


            // top left
            batch2D->Draw(*pNativeTexture,
                          NativeTextureArea(info.RenderInfo.TextureArea.X0, info.RenderInfo.TextureArea.Y0, info.RenderInfo.TextureArea.X1,
                                            info.RenderInfo.TextureArea.Y1),
                          PxAreaRectangleF::FromLeftTopRightBottom(dstX0Pxf, dstY0Pxf, dstX1Pxf, dstY1Pxf), color);
            // top middle
            batch2D->Draw(*pNativeTexture,
                          NativeTextureArea(info.RenderInfo.TextureArea.X1, info.RenderInfo.TextureArea.Y0, info.RenderInfo.TextureArea.X2,
                                            info.RenderInfo.TextureArea.Y1),
                          PxAreaRectangleF::FromLeftTopRightBottom(dstX1Pxf, dstY0Pxf, dstX2Pxf, dstY1Pxf), color);
            // top right
            batch2D->Draw(*pNativeTexture,
                          NativeTextureArea(info.RenderInfo.TextureArea.X2, info.RenderInfo.TextureArea.Y0, info.RenderInfo.TextureArea.X3,
                                            info.RenderInfo.TextureArea.Y1),
                          PxAreaRectangleF::FromLeftTopRightBottom(dstX2Pxf, dstY0Pxf, dstX3Pxf, dstY1Pxf), color);

            // middle left
            batch2D->Draw(*pNativeTexture,
                          NativeTextureArea(info.RenderInfo.TextureArea.X0, info.RenderInfo.TextureArea.Y1, info.RenderInfo.TextureArea.X1,
                                            info.RenderInfo.TextureArea.Y2),
                          PxAreaRectangleF::FromLeftTopRightBottom(dstX0Pxf, dstY1Pxf, dstX1Pxf, dstY2Pxf), color);
            // middle middle
            batch2D->Draw(*pNativeTexture,
                          NativeTextureArea(info.RenderInfo.TextureArea.X1, info.RenderInfo.TextureArea.Y1, info.RenderInfo.TextureArea.X2,
                                            info.RenderInfo.TextureArea.Y2),
                          PxAreaRectangleF::FromLeftTopRightBottom(dstX1Pxf, dstY1Pxf, dstX2Pxf, dstY2Pxf), color);
            // middle right
            batch2D->Draw(*pNativeTexture,
                          NativeTextureArea(info.RenderInfo.TextureArea.X2, info.RenderInfo.TextureArea.Y1, info.RenderInfo.TextureArea.X3,
                                            info.RenderInfo.TextureArea.Y2),
                          PxAreaRectangleF::FromLeftTopRightBottom(dstX2Pxf, dstY1Pxf, dstX3Pxf, dstY2Pxf), color);

            // bottom left
            batch2D->Draw(*pNativeTexture,
                          NativeTextureArea(info.RenderInfo.TextureArea.X0, info.RenderInfo.TextureArea.Y2, info.RenderInfo.TextureArea.X1,
                                            info.RenderInfo.TextureArea.Y3),
                          PxAreaRectangleF::FromLeftTopRightBottom(dstX0Pxf, dstY2Pxf, dstX1Pxf, dstY3Pxf), color);
            // bottom middle
            batch2D->Draw(*pNativeTexture,
                          NativeTextureArea(info.RenderInfo.TextureArea.X1, info.RenderInfo.TextureArea.Y2, info.RenderInfo.TextureArea.X2,
                                            info.RenderInfo.TextureArea.Y3),
                          PxAreaRectangleF::FromLeftTopRightBottom(dstX1Pxf, dstY2Pxf, dstX2Pxf, dstY3Pxf), color);
            // bottom right
            batch2D->Draw(*pNativeTexture,
                          NativeTextureArea(info.RenderInfo.TextureArea.X2, info.RenderInfo.TextureArea.Y2, info.RenderInfo.TextureArea.X3,
                                            info.RenderInfo.TextureArea.Y3),
                          PxAreaRectangleF::FromLeftTopRightBottom(dstX2Pxf, dstY2Pxf, dstX3Pxf, dstY3Pxf), color);
          }
        }
      }

      inline static PxSize2D ArrangeOverride(const PxSize2D& finalSizePx, const BasicNineSliceSprite* const pSprite)
      {
        return pSprite != nullptr ? PxSize2D(std::max(finalSizePx.Width(), pSprite->GetRenderInfo().ScaledSizePx.Width()),
                                             std::max(finalSizePx.Height(), pSprite->GetRenderInfo().ScaledSizePx.Height()))
                                  : finalSizePx;
      }

      inline static PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx, const BasicNineSliceSprite* const pSprite)
      {
        FSL_PARAM_NOT_USED(availableSizePx);
        return pSprite != nullptr ? pSprite->GetRenderInfo().ScaledSizePx : PxSize2D();
      }
    }
  }
}

#endif
