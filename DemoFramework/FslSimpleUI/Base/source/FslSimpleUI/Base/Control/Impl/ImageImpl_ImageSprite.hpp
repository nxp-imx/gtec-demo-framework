#ifndef FSLSIMPLEUI_BASE_CONTROL_IMPL_IMAGEIMPL_IMAGESPRITE_HPP
#define FSLSIMPLEUI_BASE_CONTROL_IMPL_IMAGEIMPL_IMAGESPRITE_HPP
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

#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/Adapter/INativeTexture2D.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
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
    //! @note  Be careful to not interleave Scoped fast draw calls that could change the material!
    namespace ImageImpl
    {
      // class ScopedImageSpriteFastDraw
      //{
      //  INativeBatch2D& m_batch2D;
      //  const ImageSpriteInfo& m_info;
      //  const INativeTexture2D& m_texture;
      //  const bool m_isEmpty;
      // public:
      //  static const ISpriteMaterial& ToReference(const ISpriteMaterial*const p)
      //  {
      //    if (p == nullptr)
      //      throw std::invalid_argument("p can not be null");
      //    return *p;
      //  }

      //  ScopedImageSpriteFastDraw(INativeBatch2D& batch2D, const ImageSprite& sprite)
      //    : ScopedImageSpriteFastDraw(batch2D, sprite.GetInfo())
      //  {
      //  }

      //  ScopedImageSpriteFastDraw(INativeBatch2D& batch2D, const ImageSpriteInfo& info)
      //    : m_batch2D(batch2D)
      //    , m_info(info)
      //    , m_texture(dynamic_cast<const INativeTexture2D&>(ToReference(info.MaterialInfo.Material.get())))
      //    , m_isEmpty(info.ScaledTrimmedSizePx.Width() <= 0 || info.ScaledTrimmedSizePx.Height() <= 0)
      //  {
      //    if (!m_isEmpty)
      //    {
      //      // BlendState is stored in the NativeMaterialFlags
      //      batch2D.ChangeTo(static_cast<BlendState>(info.MaterialInfo.NativeMaterialFlags));
      //    }
      //  }

      //  //! @brief Draw the SpriteImage
      //  void Draw(const Vector2& dstPositionPxf, const Color& color)
      //  {
      //    if (!m_isEmpty)
      //    {
      //      // Create a unscaled destination rectangle (applying the trim to the offset)
      //      PxAreaRectangleF dstRectanglePxf(dstPositionPxf.X + static_cast<float>(m_info.ScaledTrimMarginPx.Left()),
      //                                       dstPositionPxf.Y + static_cast<float>(m_info.ScaledTrimMarginPx.Top()),
      //                                       static_cast<float>(m_info.ScaledTrimmedSizePx.Width()),
      //                                       static_cast<float>(m_info.ScaledTrimmedSizePx.Height()));

      //      // No scaling necessary
      //      m_batch2D.Draw(m_texture, m_info.ImageNativeTextureArea, dstRectanglePxf, color);
      //    }
      //  }
      //};


      inline static void Draw(INativeBatch2D& batch2D, const ImageSprite* const pSprite, const PxVector2 dstPositionPxf, const Color& color)
      {
        if (pSprite != nullptr)
        {
          const auto& info = pSprite->GetInfo();
          if (info.RenderInfo.ScaledTrimmedSizePxf.Width() > 0.0f && info.RenderInfo.ScaledTrimmedSizePxf.Height() > 0.0f)
          {
            const auto* const pNativeTexture = dynamic_cast<const INativeTexture2D*>(info.MaterialInfo.Material.get());
            if (pNativeTexture != nullptr)
            {
              // BlendState is stored in the NativeMaterialFlags
              batch2D.ChangeTo(static_cast<BlendState>(info.MaterialInfo.NativeMaterialFlags));

              // Create a unscaled destination rectangle (applying the trim to the offset)
              PxAreaRectangleF dstRectanglePxf(dstPositionPxf.X + info.RenderInfo.ScaledTrimMarginPxf.Left(),
                                               dstPositionPxf.Y + info.RenderInfo.ScaledTrimMarginPxf.Top(),
                                               info.RenderInfo.ScaledTrimmedSizePxf.Width(), info.RenderInfo.ScaledTrimmedSizePxf.Height());
              batch2D.Draw(*pNativeTexture, info.RenderInfo.TextureArea, dstRectanglePxf, color);
            }
          }
        }
      }


      inline static void Draw(INativeBatch2D& batch2D, const ImageSprite* const pSprite, const PxVector2 dstPositionPxf, const Color& color,
                              const PxPoint2& origin)
      {
        if (pSprite != nullptr)
        {
          const auto& info = pSprite->GetInfo();
          if (info.RenderInfo.ScaledTrimmedSizePxf.Width() > 0.0f && info.RenderInfo.ScaledTrimmedSizePxf.Height() > 0.0f)
          {
            const auto* const pNativeTexture = dynamic_cast<const INativeTexture2D*>(info.MaterialInfo.Material.get());
            if (pNativeTexture != nullptr)
            {
              // BlendState is stored in the NativeMaterialFlags
              batch2D.ChangeTo(static_cast<BlendState>(info.MaterialInfo.NativeMaterialFlags));

              // Create a unscaled destination rectangle (applying the trim to the offset)
              PxAreaRectangleF dstRectanglePxf(dstPositionPxf.X + info.RenderInfo.ScaledTrimMarginPxf.Left() - origin.X,
                                               dstPositionPxf.Y + info.RenderInfo.ScaledTrimMarginPxf.Top() - origin.Y,
                                               info.RenderInfo.ScaledTrimmedSizePxf.Width(), info.RenderInfo.ScaledTrimmedSizePxf.Height());
              batch2D.Draw(*pNativeTexture, info.RenderInfo.TextureArea, dstRectanglePxf, color);
            }
          }
        }
      }

      inline static void Draw(INativeBatch2D& batch2D, const ImageSprite* const pSprite, const PxVector2 dstPositionPxf, const PxSize2D& dstSizePx,
                              const Color& color)
      {
        if (pSprite != nullptr && dstSizePx.Width() > 0 && dstSizePx.Height() > 0)
        {
          const auto& info = pSprite->GetInfo();
          const auto* const pNativeTexture = dynamic_cast<const INativeTexture2D*>(info.MaterialInfo.Material.get());
          if (pNativeTexture != nullptr)
          {
            // BlendState is stored in the NativeMaterialFlags
            batch2D.ChangeTo(static_cast<BlendState>(info.MaterialInfo.NativeMaterialFlags));

            const float scaledTrimSumXPxf = info.RenderInfo.ScaledTrimMarginPxf.SumX();
            const float scaledTrimSumYPxf = info.RenderInfo.ScaledTrimMarginPxf.SumY();
            if (dstSizePx == info.RenderInfo.ScaledSizePx)
            {
              // Create a unscaled destination rectangle (applying the trim to the offset)
              PxAreaRectangleF dstRectanglePxf(dstPositionPxf.X + info.RenderInfo.ScaledTrimMarginPxf.Left(),
                                               dstPositionPxf.Y + info.RenderInfo.ScaledTrimMarginPxf.Top(),
                                               info.RenderInfo.ScaledTrimmedSizePxf.Width(), info.RenderInfo.ScaledTrimmedSizePxf.Height());
              batch2D.Draw(*pNativeTexture, info.RenderInfo.TextureArea, dstRectanglePxf, color);
            }
            else
            {
              // We need to apply the scaling and trim
              const float finalScalingX = static_cast<float>(dstSizePx.Width()) / static_cast<float>(info.RenderInfo.ScaledSizePx.Width());
              const float finalScalingY = static_cast<float>(dstSizePx.Height()) / static_cast<float>(info.RenderInfo.ScaledSizePx.Height());

              // We scale and apply round to the result since in this scaling scenario we are still interested in a pixel perfect result
              PxAreaRectangleF dstRectanglePxf(dstPositionPxf.X + std::round(info.RenderInfo.ScaledTrimMarginPxf.Left() * finalScalingX),
                                               dstPositionPxf.Y + std::round(info.RenderInfo.ScaledTrimMarginPxf.Top() * finalScalingY),
                                               static_cast<float>(dstSizePx.Width()) - std::round(scaledTrimSumXPxf * finalScalingX),
                                               static_cast<float>(dstSizePx.Height()) - std::round(scaledTrimSumYPxf * finalScalingY));

              batch2D.Draw(*pNativeTexture, info.RenderInfo.TextureArea, dstRectanglePxf, color);
            }
          }
        }
      }

      inline static PxSize2D ArrangeOverride(const PxSize2D& finalSizePx, const ImageSprite* const pSprite, const ItemScalePolicy scalepolicy)
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

      inline static PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx, const ImageSprite* const pSprite)
      {
        FSL_PARAM_NOT_USED(availableSizePx);
        return pSprite != nullptr ? pSprite->GetRenderInfo().ScaledSizePx : PxSize2D();
      }
    }
  }
}

#endif
