#ifndef FSLSIMPLEUI_BASE_CONTROL_IMPL_IMAGEIMPL_NINESLICESPRITE_HPP
#define FSLSIMPLEUI_BASE_CONTROL_IMPL_IMAGEIMPL_NINESLICESPRITE_HPP
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
#include <FslGraphics/NativeQuadTextureCoords.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/Adapter/INativeTexture2D.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
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
      inline void Draw(INativeBatch2D& batch2D, const NineSliceSprite* const pSprite, const Vector2 dstPositionPxf, const PxSize2D& dstSizePx,
                       const Color& color)
      {
        if (pSprite != nullptr)
        {
          const auto& info = pSprite->GetInfo();
          const auto* const pNativeTexture = dynamic_cast<const INativeTexture2D*>(info.MaterialInfo.Material.get());
          if (pNativeTexture != nullptr)
          {
            // BlendState is stored in the NativeMaterialFlags
            batch2D.ChangeTo(static_cast<BlendState>(info.MaterialInfo.NativeMaterialFlags));

            const auto& scaledImageTrimMarginPx = info.RenderInfo.ScaledTrimMarginPx;
            auto renderWidthPx = dstSizePx.Width() - scaledImageTrimMarginPx.Right();
            auto renderHeightPx = dstSizePx.Height() - scaledImageTrimMarginPx.Bottom();

            const auto& scaledNineSlicePx = info.RenderInfo.ScaledTrimmedNineSlicePx;
            const float dstX0Pxf = dstPositionPxf.X + static_cast<float>(scaledImageTrimMarginPx.Left());
            const float dstY0Pxf = dstPositionPxf.Y + static_cast<float>(scaledImageTrimMarginPx.Top());
            const float dstX1Pxf = dstPositionPxf.X + static_cast<float>(scaledImageTrimMarginPx.Left() + scaledNineSlicePx.Left());
            const float dstY1Pxf = dstPositionPxf.Y + static_cast<float>(scaledImageTrimMarginPx.Top() + scaledNineSlicePx.Top());
            const float dstX2Pxf = dstPositionPxf.X + static_cast<float>(renderWidthPx - scaledNineSlicePx.Right());
            const float dstY2Pxf = dstPositionPxf.Y + static_cast<float>(renderHeightPx - scaledNineSlicePx.Bottom());
            const float dstX3Pxf = dstPositionPxf.X + static_cast<float>(renderWidthPx);
            const float dstY3Pxf = dstPositionPxf.Y + static_cast<float>(renderHeightPx);

            // top left
            batch2D.Draw(*pNativeTexture,
                         NativeTextureArea(info.RenderInfo.TextureArea.X0, info.RenderInfo.TextureArea.Y0, info.RenderInfo.TextureArea.X1,
                                           info.RenderInfo.TextureArea.Y1),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX0Pxf, dstY0Pxf, dstX1Pxf, dstY1Pxf), color);
            // top middle
            batch2D.Draw(*pNativeTexture,
                         NativeTextureArea(info.RenderInfo.TextureArea.X1, info.RenderInfo.TextureArea.Y0, info.RenderInfo.TextureArea.X2,
                                           info.RenderInfo.TextureArea.Y1),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX1Pxf, dstY0Pxf, dstX2Pxf, dstY1Pxf), color);
            // top right
            batch2D.Draw(*pNativeTexture,
                         NativeTextureArea(info.RenderInfo.TextureArea.X2, info.RenderInfo.TextureArea.Y0, info.RenderInfo.TextureArea.X3,
                                           info.RenderInfo.TextureArea.Y1),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX2Pxf, dstY0Pxf, dstX3Pxf, dstY1Pxf), color);

            // middle left
            batch2D.Draw(*pNativeTexture,
                         NativeTextureArea(info.RenderInfo.TextureArea.X0, info.RenderInfo.TextureArea.Y1, info.RenderInfo.TextureArea.X1,
                                           info.RenderInfo.TextureArea.Y2),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX0Pxf, dstY1Pxf, dstX1Pxf, dstY2Pxf), color);
            // middle middle
            batch2D.Draw(*pNativeTexture,
                         NativeTextureArea(info.RenderInfo.TextureArea.X1, info.RenderInfo.TextureArea.Y1, info.RenderInfo.TextureArea.X2,
                                           info.RenderInfo.TextureArea.Y2),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX1Pxf, dstY1Pxf, dstX2Pxf, dstY2Pxf), color);
            // middle right
            batch2D.Draw(*pNativeTexture,
                         NativeTextureArea(info.RenderInfo.TextureArea.X2, info.RenderInfo.TextureArea.Y1, info.RenderInfo.TextureArea.X3,
                                           info.RenderInfo.TextureArea.Y2),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX2Pxf, dstY1Pxf, dstX3Pxf, dstY2Pxf), color);

            // bottom left
            batch2D.Draw(*pNativeTexture,
                         NativeTextureArea(info.RenderInfo.TextureArea.X0, info.RenderInfo.TextureArea.Y2, info.RenderInfo.TextureArea.X1,
                                           info.RenderInfo.TextureArea.Y3),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX0Pxf, dstY2Pxf, dstX1Pxf, dstY3Pxf), color);
            // bottom middle
            batch2D.Draw(*pNativeTexture,
                         NativeTextureArea(info.RenderInfo.TextureArea.X1, info.RenderInfo.TextureArea.Y2, info.RenderInfo.TextureArea.X2,
                                           info.RenderInfo.TextureArea.Y3),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX1Pxf, dstY2Pxf, dstX2Pxf, dstY3Pxf), color);
            // bottom right
            batch2D.Draw(*pNativeTexture,
                         NativeTextureArea(info.RenderInfo.TextureArea.X2, info.RenderInfo.TextureArea.Y2, info.RenderInfo.TextureArea.X3,
                                           info.RenderInfo.TextureArea.Y3),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX2Pxf, dstY2Pxf, dstX3Pxf, dstY3Pxf), color);
          }
        }
      }

      inline static void DrawRotated90CW(INativeBatch2D& batch2D, const NineSliceSprite* const pSprite, const Vector2 dstPositionPxf,
                                         const PxSize2D& dstSizePx, const Color& color)
      {
        if (pSprite != nullptr)
        {
          const auto& info = pSprite->GetInfo();
          const auto* const pNativeTexture = dynamic_cast<const INativeTexture2D*>(info.MaterialInfo.Material.get());
          if (pNativeTexture != nullptr)
          {
            // BlendState is stored in the NativeMaterialFlags
            batch2D.ChangeTo(static_cast<BlendState>(info.MaterialInfo.NativeMaterialFlags));

            const auto& scaledImageTrimMarginPx = info.RenderInfo.ScaledTrimMarginPx;
            auto renderWidthPx = dstSizePx.Width() - scaledImageTrimMarginPx.Bottom();
            auto renderHeightPx = dstSizePx.Height() - scaledImageTrimMarginPx.Right();

            const auto& scaledNineSlicePx = info.RenderInfo.ScaledTrimmedNineSlicePx;
            const float dstX0Pxf = dstPositionPxf.X + static_cast<float>(scaledImageTrimMarginPx.Top());
            const float dstY0Pxf = dstPositionPxf.Y + static_cast<float>(scaledImageTrimMarginPx.Left());
            const float dstX1Pxf = dstPositionPxf.X + static_cast<float>(scaledImageTrimMarginPx.Top() + scaledNineSlicePx.Top());
            const float dstY1Pxf = dstPositionPxf.Y + static_cast<float>(scaledImageTrimMarginPx.Left() + scaledNineSlicePx.Left());
            const float dstX2Pxf = dstPositionPxf.X + static_cast<float>(renderWidthPx - scaledNineSlicePx.Bottom());
            const float dstY2Pxf = dstPositionPxf.Y + static_cast<float>(renderHeightPx - scaledNineSlicePx.Right());
            const float dstX3Pxf = dstPositionPxf.X + static_cast<float>(renderWidthPx);
            const float dstY3Pxf = dstPositionPxf.Y + static_cast<float>(renderHeightPx);

            // ABC | GDA
            // DEF | HEB
            // GHI | IFC
            // Basic quad vertex format
            //  u1 u2
            // v 0--1    None     = 0=u0,v0 1=u1,v0 2=u0,v1 3=u2,v1
            // 1 |  |    Rot90CW  = 0=u0,v1 1=u0,v0 2=u1,v1 3=u2,v0
            // v |  |    Rot180CW = 0=u1,v1 1=u0,v1 2=u1,v0 3=u0,v0
            // 2 2--3    Rot270CW = 0=u1,v0 1=u1,v1 2=u0,v0 3=u0,v1

            // top left
            batch2D.Draw(*pNativeTexture,
                         NativeQuadTextureCoords(info.RenderInfo.TextureArea.X0, info.RenderInfo.TextureArea.Y3,    // G
                                                 info.RenderInfo.TextureArea.X0, info.RenderInfo.TextureArea.Y2, info.RenderInfo.TextureArea.X1,
                                                 info.RenderInfo.TextureArea.Y3, info.RenderInfo.TextureArea.X1, info.RenderInfo.TextureArea.Y2),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX0Pxf, dstY0Pxf, dstX1Pxf, dstY1Pxf), color);
            // top middle
            batch2D.Draw(*pNativeTexture,
                         NativeQuadTextureCoords(info.RenderInfo.TextureArea.X0, info.RenderInfo.TextureArea.Y2,    // D
                                                 info.RenderInfo.TextureArea.X0, info.RenderInfo.TextureArea.Y1, info.RenderInfo.TextureArea.X1,
                                                 info.RenderInfo.TextureArea.Y2, info.RenderInfo.TextureArea.X1, info.RenderInfo.TextureArea.Y1),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX1Pxf, dstY0Pxf, dstX2Pxf, dstY1Pxf), color);
            // top right
            batch2D.Draw(*pNativeTexture,
                         NativeQuadTextureCoords(info.RenderInfo.TextureArea.X0, info.RenderInfo.TextureArea.Y1,    // A
                                                 info.RenderInfo.TextureArea.X0, info.RenderInfo.TextureArea.Y0, info.RenderInfo.TextureArea.X1,
                                                 info.RenderInfo.TextureArea.Y1, info.RenderInfo.TextureArea.X1, info.RenderInfo.TextureArea.Y0),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX2Pxf, dstY0Pxf, dstX3Pxf, dstY1Pxf), color);

            // middle left
            batch2D.Draw(*pNativeTexture,
                         NativeQuadTextureCoords(info.RenderInfo.TextureArea.X1, info.RenderInfo.TextureArea.Y3,    // H
                                                 info.RenderInfo.TextureArea.X1, info.RenderInfo.TextureArea.Y2, info.RenderInfo.TextureArea.X2,
                                                 info.RenderInfo.TextureArea.Y3, info.RenderInfo.TextureArea.X2, info.RenderInfo.TextureArea.Y2),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX0Pxf, dstY1Pxf, dstX1Pxf, dstY2Pxf), color);
            // middle middle
            batch2D.Draw(*pNativeTexture,
                         NativeQuadTextureCoords(info.RenderInfo.TextureArea.X1, info.RenderInfo.TextureArea.Y2,    // E
                                                 info.RenderInfo.TextureArea.X1, info.RenderInfo.TextureArea.Y1, info.RenderInfo.TextureArea.X2,
                                                 info.RenderInfo.TextureArea.Y2, info.RenderInfo.TextureArea.X2, info.RenderInfo.TextureArea.Y1),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX1Pxf, dstY1Pxf, dstX2Pxf, dstY2Pxf), color);
            // middle right
            batch2D.Draw(*pNativeTexture,
                         NativeQuadTextureCoords(info.RenderInfo.TextureArea.X1, info.RenderInfo.TextureArea.Y1,    // B
                                                 info.RenderInfo.TextureArea.X1, info.RenderInfo.TextureArea.Y0, info.RenderInfo.TextureArea.X2,
                                                 info.RenderInfo.TextureArea.Y1, info.RenderInfo.TextureArea.X2, info.RenderInfo.TextureArea.Y0),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX2Pxf, dstY1Pxf, dstX3Pxf, dstY2Pxf), color);

            // bottom left
            batch2D.Draw(*pNativeTexture,
                         NativeQuadTextureCoords(info.RenderInfo.TextureArea.X2, info.RenderInfo.TextureArea.Y3,    // I
                                                 info.RenderInfo.TextureArea.X2, info.RenderInfo.TextureArea.Y2, info.RenderInfo.TextureArea.X3,
                                                 info.RenderInfo.TextureArea.Y3, info.RenderInfo.TextureArea.X3, info.RenderInfo.TextureArea.Y2),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX0Pxf, dstY2Pxf, dstX1Pxf, dstY3Pxf), color);
            // bottom middle
            batch2D.Draw(*pNativeTexture,
                         NativeQuadTextureCoords(info.RenderInfo.TextureArea.X2, info.RenderInfo.TextureArea.Y2,    // F
                                                 info.RenderInfo.TextureArea.X2, info.RenderInfo.TextureArea.Y1, info.RenderInfo.TextureArea.X3,
                                                 info.RenderInfo.TextureArea.Y2, info.RenderInfo.TextureArea.X3, info.RenderInfo.TextureArea.Y1),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX1Pxf, dstY2Pxf, dstX2Pxf, dstY3Pxf), color);
            // bottom right
            batch2D.Draw(*pNativeTexture,
                         NativeQuadTextureCoords(info.RenderInfo.TextureArea.X2, info.RenderInfo.TextureArea.Y1,    // C
                                                 info.RenderInfo.TextureArea.X2, info.RenderInfo.TextureArea.Y0, info.RenderInfo.TextureArea.X3,
                                                 info.RenderInfo.TextureArea.Y1, info.RenderInfo.TextureArea.X3, info.RenderInfo.TextureArea.Y0),
                         PxAreaRectangleF::FromLeftTopRightBottom(dstX2Pxf, dstY2Pxf, dstX3Pxf, dstY3Pxf), color);
          }
        }
      }

      inline PxSize2D ArrangeOverride(const PxSize2D& finalSizePx, const NineSliceSprite* const pSprite)
      {
        return pSprite != nullptr ? PxSize2D::Max(finalSizePx, pSprite->GetRenderInfo().ScaledSizePx) : finalSizePx;
      }

      inline PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx, const NineSliceSprite* const pSprite)
      {
        FSL_PARAM_NOT_USED(availableSizePx);
        return pSprite != nullptr ? pSprite->GetRenderInfo().ScaledSizePx : PxSize2D();
      }

      inline PxSize2D ArrangeOverride(const PxSize2D& finalSizePx, const NineSliceSprite* const pSprite, const bool rotate90)
      {
        PxSize2D spriteSizePx;
        if (pSprite != nullptr)
        {
          const NineSliceSpriteInfo& info = pSprite->GetInfo();
          // coverity[swapped_arguments]
          spriteSizePx =
            !rotate90 ? info.RenderInfo.ScaledSizePx : PxSize2D(info.RenderInfo.ScaledSizePx.Height(), info.RenderInfo.ScaledSizePx.Width());
        }
        return PxSize2D::Max(finalSizePx, spriteSizePx);
      }


      inline PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx, const NineSliceSprite* const pSprite, const bool rotate90)
      {
        PxSize2D sizePx = MeasureOverride(availableSizePx, pSprite);
        // coverity[swapped_arguments]
        return !rotate90 ? sizePx : PxSize2D(sizePx.Height(), sizePx.Width());
      }

      inline PxThickness GetContentMargin(const NineSliceSprite* const pSprite0, const NineSliceSprite* const pSprite1)
      {
        if (pSprite0 != nullptr)
        {
          if (pSprite1 != nullptr)
          {
            // Apply the nine-slice padding, then measure the minimum size of the nine slice
            return PxThickness::Max(pSprite0->GetRenderInfo().ScaledContentMarginPx, pSprite1->GetRenderInfo().ScaledContentMarginPx);
          }
          return pSprite0->GetRenderInfo().ScaledContentMarginPx;
        }
        if (pSprite1 != nullptr)

        {
          return pSprite1->GetRenderInfo().ScaledContentMarginPx;
        }
        return {};
      }

      inline void ApplyMax(PxSize2D& rMaxPadding, PxSize2D& rMaxSize, const NineSliceSprite* const pSprite)
      {
        if (pSprite != nullptr)
        {
          const auto& spriteInfo = pSprite->GetInfo();
          rMaxPadding.SetMax(spriteInfo.RenderInfo.ScaledContentMarginPx.Sum());
          rMaxSize.SetMax(spriteInfo.RenderInfo.ScaledSizePx);
        }
      }

      inline void ApplyMeasure(PxSize2D& rDesiredSizePx, const NineSliceSprite* const pSprite0, const NineSliceSprite* const pSprite1,
                               const NineSliceSprite* const pSprite2)
      {
        PxSize2D maxPadding;
        PxSize2D maxSize;
        ApplyMax(maxPadding, maxSize, pSprite0);
        ApplyMax(maxPadding, maxSize, pSprite1);
        ApplyMax(maxPadding, maxSize, pSprite2);

        rDesiredSizePx += maxPadding;
        rDesiredSizePx.SetMax(maxSize);
      }

    }
  }
}

#endif
