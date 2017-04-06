#ifndef FSLSIMPLEUI_CONTROL_IMPL_DRAW9SLICEIMPL_HPP
#define FSLSIMPLEUI_CONTROL_IMPL_DRAW9SLICEIMPL_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2015 Freescale Semiconductor, Inc.
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
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslBase/Math/NineSlice.hpp>
#include <FslBase/Math/Rect.hpp>
#include <FslBase/Math/ThicknessF.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslSimpleUI/UIScaleUtil.hpp>
#include <memory>
#include <algorithm>

namespace Fsl
{
  namespace UI
  {

    //! @brief Since we have multiple controls that need this code its been isolated here as inline methods
    namespace Draw9SliceImpl
    {
      inline static void WinDraw(const std::shared_ptr<INativeBatch2D>& batch2D, const Rect& dstRect, const AtlasTexture2D& texture, const NineSlice& nineSlice, const ThicknessF& minimumBackgroundMargin, const Color& color)
      {
        if (!texture.IsValid())
          return;

        const Point2 textureSize = texture.GetSize();
        if (textureSize.X < nineSlice.SumX() || textureSize.Y < nineSlice.SumY())
        {
          FSLLOG_WARNING("The nine slice is incompatible with the image");
          return;
        }

        // Quick handling of a empty nine slice (this means we just draw the center piece)
        if (nineSlice.IsEmpty())
        {
          batch2D->Draw(texture, dstRect, Color::White());
          return;
        }

        //if (nineSlice.IsEmpty())
        //{

        //  return;
        //}


        // A  B              C  D
        //--------------------------------------------------------------------------------------------------------------------------------------------
        // |  |              |  |
        // |  |              |  |
        // |--------------------|   B
        // |  |              |  |
        // |  |              |  |
        // |  |              |  |
        // |--------------------|   C
        // |  |              |  |
        // |  |              |  |
        //--------------------------------------------------------------------------------------------------------------------------------------------

        const int32_t srcLeftWidth = nineSlice.SliceFromTopLeftX();
        const int32_t srcTopHeight = nineSlice.SliceFromTopLeftY();
        const int32_t srcCenterWidth = textureSize.X - nineSlice.SumX();
        const int32_t srcCenterHeight = textureSize.Y - nineSlice.SumY();
        const int32_t srcRightWidth = nineSlice.SliceFromBottomRightX();
        const int32_t srcBottomHeight = nineSlice.SliceFromBottomRightY();

        const float rect0Width = static_cast<float>(srcLeftWidth);
        const float rect0Height = static_cast<float>(srcTopHeight);
        const float rect4Width = static_cast<float>(srcCenterWidth);
        const float rect4Height = static_cast<float>(srcCenterHeight);
        const float rect8Width = static_cast<float>(srcRightWidth);
        const float rect8Height = static_cast<float>(srcBottomHeight);

        float bgLeft = std::max(rect0Width, minimumBackgroundMargin.Left());
        float bgTop = std::max(rect0Height, minimumBackgroundMargin.Top());
        float bgRight = std::max(rect8Width, minimumBackgroundMargin.Right());
        float bgBottom = std::max(rect8Height, minimumBackgroundMargin.Bottom());
        float bgSizeX = bgLeft + bgRight + rect4Width;
        float bgSizeY = bgTop + bgBottom + rect4Height;
        {
          Vector2 bgScale;
          if (!UIScaleUtil::TryCalcScaling(bgScale, dstRect.GetSize(), Vector2(bgSizeX, bgSizeY), ItemScalePolicy::DownscaleKeepAR))
            return;

          bgLeft *= bgScale.X;
          bgRight *= bgScale.X;
          bgTop *= bgScale.Y;
          bgBottom *= bgScale.Y;
        }

        const float dstXA = dstRect.Left();
        const float dstYA = dstRect.Top();
        const float dstXB = dstXA + bgLeft;
        const float dstYB = dstYA + bgTop;
        const float dstXD = dstRect.Right();
        const float dstYD = dstRect.Bottom();
        const float dstXC = dstXD - bgRight;
        const float dstYC = dstYD - bgBottom;

        Rect finalDstRect;

        batch2D->ChangeTo(BlendState::AlphaBlend);

        const int32_t srcRightStart = srcLeftWidth + srcCenterWidth;
        const int32_t srcBottomStart = srcTopHeight + srcCenterHeight;


        // TOP_LEFT CORNER: xa, ya, xb, yb
        finalDstRect = Rect(dstXA, dstYA, dstXB, dstYB, false);
        batch2D->Draw(texture, finalDstRect, Rectangle(0, 0, srcLeftWidth, srcTopHeight), color);

        // TOP: xb, ya, xc, yb
        finalDstRect = Rect(dstXB, dstYA, dstXC, dstYB, false);
        batch2D->Draw(texture, finalDstRect, Rectangle(srcLeftWidth, 0, srcCenterWidth, srcTopHeight), color);

        // TOP_RIGHT CORNER: xc, ya, xd, yb
        finalDstRect = Rect(dstXC, dstYA, dstXD, dstYB, false);
        batch2D->Draw(texture, finalDstRect, Rectangle(srcRightStart, 0, srcRightWidth, srcTopHeight), color);

        // LEFT SIDE: xa, yb, xb, yc
        finalDstRect = Rect(dstXA, dstYB, dstXB, dstYC, false);
        batch2D->Draw(texture, finalDstRect, Rectangle(0, srcTopHeight, srcLeftWidth, srcCenterHeight), color);

        // CENTER: xb, yb, xc, yc
        finalDstRect = Rect(dstXB, dstYB, dstXC, dstYC, false);
        batch2D->Draw(texture, finalDstRect, Rectangle(srcLeftWidth, srcTopHeight, srcCenterWidth, srcCenterHeight), color);

        // RIGHT SIDE: xc, yb, xd, yc
        finalDstRect = Rect(dstXC, dstYB, dstXD, dstYC, false);
        batch2D->Draw(texture, finalDstRect, Rectangle(srcRightStart, srcTopHeight, srcRightWidth, srcCenterHeight), color);

        // BOTTOM_LEFT CORNER: xa, yc, xb, yd
        finalDstRect = Rect(dstXA, dstYC, dstXB, dstYD, false);
        batch2D->Draw(texture, finalDstRect, Rectangle(0, srcBottomStart, srcLeftWidth, srcBottomHeight), color);

        // BOTTOM: xb, yc, xc, yd
        finalDstRect = Rect(dstXB, dstYC, dstXC, dstYD, false);
        batch2D->Draw(texture, finalDstRect, Rectangle(srcLeftWidth, srcBottomStart, srcCenterWidth, srcBottomHeight), color);

        // BOTTOM_RIGHT CORNER: xc, yc, xd, yd
        finalDstRect = Rect(dstXC, dstYC, dstXD, dstYD, false);
        batch2D->Draw(texture, finalDstRect, Rectangle(srcRightStart, srcBottomStart, srcRightWidth, srcBottomHeight), color);
      }
    }
  }
}

#endif
