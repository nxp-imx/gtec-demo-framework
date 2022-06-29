#ifndef FSLSIMPLEUI_BASE_CONTROL_UTIL_DRAW9SLICEUTIL_HPP
#define FSLSIMPLEUI_BASE_CONTROL_UTIL_DRAW9SLICEUTIL_HPP
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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Math/NineSlice.hpp>
#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Math/ThicknessF.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <FslSimpleUI/Base/UIScaleUtil.hpp>
#include <algorithm>
#include <memory>

namespace Fsl::UI::Draw9SliceUtil
{
  inline static void WinDraw(const std::shared_ptr<INativeBatch2D>& batch2D, const PxAreaRectangleF& dstRectanglePxf, const AtlasTexture2D& texture,
                             const NineSlice& nineSlice, const Color& color)
  {
    if (!texture.IsValid())
    {
      return;
    }

    const PxSize2D textureSize = texture.GetSize();
    if (textureSize.Width() < nineSlice.SumX() || textureSize.Height() < nineSlice.SumY())
    {
      FSLLOG3_WARNING("The nine slice is incompatible with the image");
      return;
    }

    // Quick handling of a empty nine slice (this means we just draw the center piece)
    if (nineSlice.IsEmpty())
    {
      batch2D->Draw(texture, dstRectanglePxf, color);
      return;
    }

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
    const int32_t srcCenterWidth = textureSize.Width() - nineSlice.SumX();
    const int32_t srcCenterHeight = textureSize.Height() - nineSlice.SumY();
    const int32_t srcRightWidth = nineSlice.SliceFromBottomRightX();
    const int32_t srcBottomHeight = nineSlice.SliceFromBottomRightY();

    const auto rect0Width = static_cast<float>(srcLeftWidth);
    const auto rect0Height = static_cast<float>(srcTopHeight);
    const auto rect4Width = static_cast<float>(srcCenterWidth);
    const auto rect4Height = static_cast<float>(srcCenterHeight);
    const auto rect8Width = static_cast<float>(srcRightWidth);
    const auto rect8Height = static_cast<float>(srcBottomHeight);

    float bgLeft = rect0Width;
    float bgTop = rect0Height;
    float bgRight = rect8Width;
    float bgBottom = rect8Height;
    float bgSizeX = bgLeft + bgRight + rect4Width;
    float bgSizeY = bgTop + bgBottom + rect4Height;
    {
      PxVector2 bgScale;
      if (!UIScaleUtil::TryCalcScaling(bgScale, dstRectanglePxf.GetSize(), PxVector2(bgSizeX, bgSizeY), ItemScalePolicy::DownscaleKeepAR))
      {
        return;
      }

      bgLeft *= bgScale.X;
      bgRight *= bgScale.X;
      bgTop *= bgScale.Y;
      bgBottom *= bgScale.Y;
    }

    const float dstXA = dstRectanglePxf.Left();
    const float dstYA = dstRectanglePxf.Top();
    const float dstXB = dstXA + bgLeft;
    const float dstYB = dstYA + bgTop;
    const float dstXD = dstRectanglePxf.Right();
    const float dstYD = dstRectanglePxf.Bottom();
    const float dstXC = dstXD - bgRight;
    const float dstYC = dstYD - bgBottom;

    PxAreaRectangleF finalDstRectPxf;

    const int32_t srcRightStart = srcLeftWidth + srcCenterWidth;
    const int32_t srcBottomStart = srcTopHeight + srcCenterHeight;


    // TOP_LEFT CORNER: xa, ya, xb, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYA, dstXB, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(0, 0, srcLeftWidth, srcTopHeight), color);

    // TOP: xb, ya, xc, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYA, dstXC, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcLeftWidth, 0, srcCenterWidth, srcTopHeight), color);

    // TOP_RIGHT CORNER: xc, ya, xd, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYA, dstXD, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcRightStart, 0, srcRightWidth, srcTopHeight), color);

    // LEFT SIDE: xa, yb, xb, yc
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYB, dstXB, dstYC);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(0, srcTopHeight, srcLeftWidth, srcCenterHeight), color);

    // CENTER: xb, yb, xc, yc
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYB, dstXC, dstYC);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcLeftWidth, srcTopHeight, srcCenterWidth, srcCenterHeight), color);

    // RIGHT SIDE: xc, yb, xd, yc
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYB, dstXD, dstYC);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcRightStart, srcTopHeight, srcRightWidth, srcCenterHeight), color);

    // BOTTOM_LEFT CORNER: xa, yc, xb, yd
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYC, dstXB, dstYD);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(0, srcBottomStart, srcLeftWidth, srcBottomHeight), color);

    // BOTTOM: xb, yc, xc, yd
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYC, dstXC, dstYD);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcLeftWidth, srcBottomStart, srcCenterWidth, srcBottomHeight), color);

    // BOTTOM_RIGHT CORNER: xc, yc, xd, yd
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYC, dstXD, dstYD);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcRightStart, srcBottomStart, srcRightWidth, srcBottomHeight), color);
  }

  inline static void WinDraw(const std::shared_ptr<INativeBatch2D>& batch2D, const PxAreaRectangleF& dstRectanglePxf, const AtlasTexture2D& texture,
                             const NineSlice& nineSlice, const PxThickness& minimumBackgroundMarginPx, const Color& color)
  {
    if (!texture.IsValid())
    {
      return;
    }

    const PxSize2D textureSize = texture.GetSize();
    if (textureSize.Width() < nineSlice.SumX() || textureSize.Height() < nineSlice.SumY())
    {
      FSLLOG3_WARNING("The nine slice is incompatible with the image");
      return;
    }

    // Quick handling of a empty nine slice (this means we just draw the center piece)
    if (nineSlice.IsEmpty())
    {
      batch2D->Draw(texture, dstRectanglePxf, color);
      return;
    }

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
    const int32_t srcCenterWidth = textureSize.Width() - nineSlice.SumX();
    const int32_t srcCenterHeight = textureSize.Height() - nineSlice.SumY();
    const int32_t srcRightWidth = nineSlice.SliceFromBottomRightX();
    const int32_t srcBottomHeight = nineSlice.SliceFromBottomRightY();

    const auto rect0Width = static_cast<float>(srcLeftWidth);
    const auto rect0Height = static_cast<float>(srcTopHeight);
    const auto rect4Width = static_cast<float>(srcCenterWidth);
    const auto rect4Height = static_cast<float>(srcCenterHeight);
    const auto rect8Width = static_cast<float>(srcRightWidth);
    const auto rect8Height = static_cast<float>(srcBottomHeight);

    float bgLeft = std::max(rect0Width, static_cast<float>(minimumBackgroundMarginPx.Left()));
    float bgTop = std::max(rect0Height, static_cast<float>(minimumBackgroundMarginPx.Top()));
    float bgRight = std::max(rect8Width, static_cast<float>(minimumBackgroundMarginPx.Right()));
    float bgBottom = std::max(rect8Height, static_cast<float>(minimumBackgroundMarginPx.Bottom()));
    float bgSizeX = bgLeft + bgRight + rect4Width;
    float bgSizeY = bgTop + bgBottom + rect4Height;
    {
      PxVector2 bgScale;
      if (!UIScaleUtil::TryCalcScaling(bgScale, dstRectanglePxf.GetSize(), PxVector2(bgSizeX, bgSizeY), ItemScalePolicy::DownscaleKeepAR))
      {
        return;
      }

      bgLeft *= bgScale.X;
      bgRight *= bgScale.X;
      bgTop *= bgScale.Y;
      bgBottom *= bgScale.Y;
    }

    const float dstXA = dstRectanglePxf.Left();
    const float dstYA = dstRectanglePxf.Top();
    const float dstXB = dstXA + bgLeft;
    const float dstYB = dstYA + bgTop;
    const float dstXD = dstRectanglePxf.Right();
    const float dstYD = dstRectanglePxf.Bottom();
    const float dstXC = dstXD - bgRight;
    const float dstYC = dstYD - bgBottom;

    PxAreaRectangleF finalDstRectPxf;

    const int32_t srcRightStart = srcLeftWidth + srcCenterWidth;
    const int32_t srcBottomStart = srcTopHeight + srcCenterHeight;


    // TOP_LEFT CORNER: xa, ya, xb, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYA, dstXB, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(0, 0, srcLeftWidth, srcTopHeight), color);

    // TOP: xb, ya, xc, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYA, dstXC, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcLeftWidth, 0, srcCenterWidth, srcTopHeight), color);

    // TOP_RIGHT CORNER: xc, ya, xd, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYA, dstXD, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcRightStart, 0, srcRightWidth, srcTopHeight), color);

    // LEFT SIDE: xa, yb, xb, yc
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYB, dstXB, dstYC);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(0, srcTopHeight, srcLeftWidth, srcCenterHeight), color);

    // CENTER: xb, yb, xc, yc
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYB, dstXC, dstYC);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcLeftWidth, srcTopHeight, srcCenterWidth, srcCenterHeight), color);

    // RIGHT SIDE: xc, yb, xd, yc
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYB, dstXD, dstYC);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcRightStart, srcTopHeight, srcRightWidth, srcCenterHeight), color);

    // BOTTOM_LEFT CORNER: xa, yc, xb, yd
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYC, dstXB, dstYD);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(0, srcBottomStart, srcLeftWidth, srcBottomHeight), color);

    // BOTTOM: xb, yc, xc, yd
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYC, dstXC, dstYD);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcLeftWidth, srcBottomStart, srcCenterWidth, srcBottomHeight), color);

    // BOTTOM_RIGHT CORNER: xc, yc, xd, yd
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYC, dstXD, dstYD);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcRightStart, srcBottomStart, srcRightWidth, srcBottomHeight), color);
  }

  inline static void WinDrawRotated90CW(const std::shared_ptr<INativeBatch2D>& batch2D, const PxAreaRectangleF& dstRectanglePxf,
                                        const AtlasTexture2D& texture, const NineSlice& nineSlice, const Color& color)
  {
    if (!texture.IsValid())
    {
      return;
    }

    const PxSize2D textureSize = texture.GetSize();
    if (textureSize.Width() < nineSlice.SumX() || textureSize.Height() < nineSlice.SumY())
    {
      FSLLOG3_WARNING("The nine slice is incompatible with the image");
      return;
    }

    batch2D->ChangeTo(BlendState::AlphaBlend);

    // Quick handling of a empty nine slice (this means we just draw the center piece)
    if (nineSlice.IsEmpty())
    {
      batch2D->Draw(texture, dstRectanglePxf, color, BatchEffect::Rotate90Clockwise);
      return;
    }

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
    const int32_t srcCenterWidth = textureSize.Width() - nineSlice.SumX();
    const int32_t srcCenterHeight = textureSize.Height() - nineSlice.SumY();
    const int32_t srcRightWidth = nineSlice.SliceFromBottomRightX();
    const int32_t srcBottomHeight = nineSlice.SliceFromBottomRightY();

    const auto rect0Width = static_cast<float>(srcTopHeight);
    const auto rect0Height = static_cast<float>(srcLeftWidth);
    const auto rect4Width = static_cast<float>(srcCenterHeight);
    const auto rect4Height = static_cast<float>(srcCenterWidth);
    const auto rect8Width = static_cast<float>(srcBottomHeight);
    const auto rect8Height = static_cast<float>(srcRightWidth);

    float bgLeft = rect0Width;
    float bgTop = rect0Height;
    float bgRight = rect8Width;
    float bgBottom = rect8Height;
    float bgSizeX = bgLeft + bgRight + rect4Width;
    float bgSizeY = bgTop + bgBottom + rect4Height;
    {
      PxVector2 bgScale;
      if (!UIScaleUtil::TryCalcScaling(bgScale, dstRectanglePxf.GetSize(), PxVector2(bgSizeX, bgSizeY), ItemScalePolicy::DownscaleKeepAR))
      {
        return;
      }

      bgLeft *= bgScale.X;
      bgRight *= bgScale.X;
      bgTop *= bgScale.Y;
      bgBottom *= bgScale.Y;
    }

    const float dstXA = dstRectanglePxf.Left();
    const float dstYA = dstRectanglePxf.Top();
    const float dstXB = dstXA + bgLeft;
    const float dstYB = dstYA + bgTop;
    const float dstXD = dstRectanglePxf.Right();
    const float dstYD = dstRectanglePxf.Bottom();
    const float dstXC = dstXD - bgRight;
    const float dstYC = dstYD - bgBottom;

    PxAreaRectangleF finalDstRect;

    const int32_t srcRightStart = srcLeftWidth + srcCenterWidth;
    const int32_t srcBottomStart = srcTopHeight + srcCenterHeight;

    // TOP_LEFT CORNER: xa, ya, xb, yb
    finalDstRect = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYA, dstXB, dstYB);
    batch2D->Draw(texture, finalDstRect, PxRectangleU32(0, srcBottomStart, srcLeftWidth, srcBottomHeight), color, BatchEffect::Rotate90Clockwise);

    // TOP: xb, ya, xc, yb
    finalDstRect = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYA, dstXC, dstYB);
    batch2D->Draw(texture, finalDstRect, PxRectangleU32(0, srcTopHeight, srcLeftWidth, srcCenterHeight), color, BatchEffect::Rotate90Clockwise);

    // TOP_RIGHT CORNER: xc, ya, xd, yb
    finalDstRect = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYA, dstXD, dstYB);
    batch2D->Draw(texture, finalDstRect, PxRectangleU32(0, 0, srcLeftWidth, srcTopHeight), color, BatchEffect::Rotate90Clockwise);

    // LEFT SIDE: xa, yb, xb, yc
    finalDstRect = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYB, dstXB, dstYC);
    batch2D->Draw(texture, finalDstRect, PxRectangleU32(srcLeftWidth, srcBottomStart, srcCenterWidth, srcBottomHeight), color,
                  BatchEffect::Rotate90Clockwise);

    // CENTER: xb, yb, xc, yc
    finalDstRect = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYB, dstXC, dstYC);
    batch2D->Draw(texture, finalDstRect, PxRectangleU32(srcLeftWidth, srcTopHeight, srcCenterWidth, srcCenterHeight), color,
                  BatchEffect::Rotate90Clockwise);

    // RIGHT SIDE: xc, yb, xd, yc
    finalDstRect = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYB, dstXD, dstYC);
    batch2D->Draw(texture, finalDstRect, PxRectangleU32(srcLeftWidth, 0, srcCenterWidth, srcTopHeight), color, BatchEffect::Rotate90Clockwise);

    // BOTTOM_LEFT CORNER: xa, yc, xb, yd
    finalDstRect = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYC, dstXB, dstYD);
    batch2D->Draw(texture, finalDstRect, PxRectangleU32(srcRightStart, srcBottomStart, srcRightWidth, srcBottomHeight), color,
                  BatchEffect::Rotate90Clockwise);

    // BOTTOM: xb, yc, xc, yd
    finalDstRect = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYC, dstXC, dstYD);
    batch2D->Draw(texture, finalDstRect, PxRectangleU32(srcRightStart, srcTopHeight, srcRightWidth, srcCenterHeight), color,
                  BatchEffect::Rotate90Clockwise);

    // BOTTOM_RIGHT CORNER: xc, yc, xd, yd
    finalDstRect = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYC, dstXD, dstYD);
    batch2D->Draw(texture, finalDstRect, PxRectangleU32(srcRightStart, 0, srcRightWidth, srcTopHeight), color, BatchEffect::Rotate90Clockwise);
  }

  inline static void WinDrawRotated90CW(const std::shared_ptr<INativeBatch2D>& batch2D, const PxAreaRectangleF& dstRectanglePxf,
                                        const AtlasTexture2D& texture, const NineSlice& nineSlice, const ThicknessF& minimumBackgroundMargin,
                                        const Color& color)
  {
    if (!texture.IsValid())
    {
      return;
    }

    const PxSize2D textureSize = texture.GetSize();
    if (textureSize.Width() < nineSlice.SumX() || textureSize.Height() < nineSlice.SumY())
    {
      FSLLOG3_WARNING("The nine slice is incompatible with the image");
      return;
    }

    // Quick handling of a empty nine slice (this means we just draw the center piece)
    if (nineSlice.IsEmpty())
    {
      batch2D->Draw(texture, dstRectanglePxf, color, BatchEffect::Rotate90Clockwise);
      return;
    }

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
    const int32_t srcCenterWidth = textureSize.Width() - nineSlice.SumX();
    const int32_t srcCenterHeight = textureSize.Height() - nineSlice.SumY();
    const int32_t srcRightWidth = nineSlice.SliceFromBottomRightX();
    const int32_t srcBottomHeight = nineSlice.SliceFromBottomRightY();

    const auto rect0Width = static_cast<float>(srcTopHeight);
    const auto rect0Height = static_cast<float>(srcLeftWidth);
    const auto rect4Width = static_cast<float>(srcCenterHeight);
    const auto rect4Height = static_cast<float>(srcCenterWidth);
    const auto rect8Width = static_cast<float>(srcBottomHeight);
    const auto rect8Height = static_cast<float>(srcRightWidth);

    float bgLeft = std::max(rect0Width, minimumBackgroundMargin.Top());
    float bgTop = std::max(rect0Height, minimumBackgroundMargin.Left());
    float bgRight = std::max(rect8Width, minimumBackgroundMargin.Bottom());
    float bgBottom = std::max(rect8Height, minimumBackgroundMargin.Right());
    float bgSizeX = bgLeft + bgRight + rect4Width;
    float bgSizeY = bgTop + bgBottom + rect4Height;
    {
      PxVector2 bgScale;
      if (!UIScaleUtil::TryCalcScaling(bgScale, dstRectanglePxf.GetSize(), PxVector2(bgSizeX, bgSizeY), ItemScalePolicy::DownscaleKeepAR))
      {
        return;
      }

      bgLeft *= bgScale.X;
      bgRight *= bgScale.X;
      bgTop *= bgScale.Y;
      bgBottom *= bgScale.Y;
    }

    const float dstXA = dstRectanglePxf.Left();
    const float dstYA = dstRectanglePxf.Top();
    const float dstXB = dstXA + bgLeft;
    const float dstYB = dstYA + bgTop;
    const float dstXD = dstRectanglePxf.Right();
    const float dstYD = dstRectanglePxf.Bottom();
    const float dstXC = dstXD - bgRight;
    const float dstYC = dstYD - bgBottom;

    PxAreaRectangleF finalDstRectPxf;

    const int32_t srcRightStart = srcLeftWidth + srcCenterWidth;
    const int32_t srcBottomStart = srcTopHeight + srcCenterHeight;

    // TOP_LEFT CORNER: xa, ya, xb, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYA, dstXB, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangleU32(0, srcBottomStart, srcLeftWidth, srcBottomHeight), color, BatchEffect::Rotate90Clockwise);

    // TOP: xb, ya, xc, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYA, dstXC, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangleU32(0, srcTopHeight, srcLeftWidth, srcCenterHeight), color, BatchEffect::Rotate90Clockwise);

    // TOP_RIGHT CORNER: xc, ya, xd, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYA, dstXD, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangleU32(0, 0, srcLeftWidth, srcTopHeight), color, BatchEffect::Rotate90Clockwise);

    // LEFT SIDE: xa, yb, xb, yc
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYB, dstXB, dstYC);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangleU32(srcLeftWidth, srcBottomStart, srcCenterWidth, srcBottomHeight), color,
                  BatchEffect::Rotate90Clockwise);

    // CENTER: xb, yb, xc, yc
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYB, dstXC, dstYC);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangleU32(srcLeftWidth, srcTopHeight, srcCenterWidth, srcCenterHeight), color,
                  BatchEffect::Rotate90Clockwise);

    // RIGHT SIDE: xc, yb, xd, yc
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYB, dstXD, dstYC);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangleU32(srcLeftWidth, 0, srcCenterWidth, srcTopHeight), color, BatchEffect::Rotate90Clockwise);

    // BOTTOM_LEFT CORNER: xa, yc, xb, yd
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYC, dstXB, dstYD);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangleU32(srcRightStart, srcBottomStart, srcRightWidth, srcBottomHeight), color,
                  BatchEffect::Rotate90Clockwise);

    // BOTTOM: xb, yc, xc, yd
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYC, dstXC, dstYD);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangleU32(srcRightStart, srcTopHeight, srcRightWidth, srcCenterHeight), color,
                  BatchEffect::Rotate90Clockwise);

    // BOTTOM_RIGHT CORNER: xc, yc, xd, yd
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYC, dstXD, dstYD);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangleU32(srcRightStart, 0, srcRightWidth, srcTopHeight), color, BatchEffect::Rotate90Clockwise);
  }
}

#endif
