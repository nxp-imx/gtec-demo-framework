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
#include <FslBase/UncheckedNumericCast.hpp>
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
    if (textureSize.RawWidth() < nineSlice.SumX() || textureSize.RawHeight() < nineSlice.SumY())
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

    const PxSize1D srcLeftWidth = PxSize1D::UncheckedCreate(nineSlice.SliceFromTopLeftX());
    const PxSize1D srcTopHeight = PxSize1D::UncheckedCreate(nineSlice.SliceFromTopLeftY());
    const PxSize1D srcCenterWidth = PxSize1D::UncheckedCreate(textureSize.RawWidth() - nineSlice.SumX());
    const PxSize1D srcCenterHeight = PxSize1D::UncheckedCreate(textureSize.RawHeight() - nineSlice.SumY());
    const PxSize1D srcRightWidth = PxSize1D::UncheckedCreate(nineSlice.SliceFromBottomRightX());
    const PxSize1D srcBottomHeight = PxSize1D::UncheckedCreate(nineSlice.SliceFromBottomRightY());

    const PxSize1DF rect0Width(srcLeftWidth);
    const PxSize1DF rect0Height(srcTopHeight);
    const PxSize1DF rect4Width(srcCenterWidth);
    const PxSize1DF rect4Height(srcCenterHeight);
    const PxSize1DF rect8Width(srcRightWidth);
    const PxSize1DF rect8Height(srcBottomHeight);

    PxSize1DF bgLeft = rect0Width;
    PxSize1DF bgTop = rect0Height;
    PxSize1DF bgRight = rect8Width;
    PxSize1DF bgBottom = rect8Height;
    PxSize1DF bgSizeX(bgLeft + bgRight + rect4Width);
    PxSize1DF bgSizeY(bgTop + bgBottom + rect4Height);
    {
      PxSize2DF bgScale;
      if (!UIScaleUtil::TryCalcScaling(bgScale, dstRectanglePxf.Size(), PxSize2DF(bgSizeX, bgSizeY), ItemScalePolicy::DownscaleKeepAR))
      {
        return;
      }

      bgLeft *= bgScale.Width();
      bgRight *= bgScale.Width();
      bgTop *= bgScale.Height();
      bgBottom *= bgScale.Height();
    }

    const PxValueF dstXA = dstRectanglePxf.Left();
    const PxValueF dstYA = dstRectanglePxf.Top();
    const PxValueF dstXB = dstXA + bgLeft;
    const PxValueF dstYB = dstYA + bgTop;
    const PxValueF dstXD = dstRectanglePxf.Right();
    const PxValueF dstYD = dstRectanglePxf.Bottom();
    const PxValueF dstXC = dstXD - bgRight;
    const PxValueF dstYC = dstYD - bgBottom;

    PxAreaRectangleF finalDstRectPxf;

    const PxSize1D srcRightStart = srcLeftWidth + srcCenterWidth;
    const PxSize1D srcBottomStart = srcTopHeight + srcCenterHeight;


    // TOP_LEFT CORNER: xa, ya, xb, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYA, dstXB, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(PxValue(0), PxValue(0), srcLeftWidth, srcTopHeight), color);

    // TOP: xb, ya, xc, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYA, dstXC, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcLeftWidth, PxValue(0), srcCenterWidth, srcTopHeight), color);

    // TOP_RIGHT CORNER: xc, ya, xd, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYA, dstXD, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcRightStart, PxValue(0), srcRightWidth, srcTopHeight), color);

    // LEFT SIDE: xa, yb, xb, yc
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYB, dstXB, dstYC);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(PxValue(0), srcTopHeight, srcLeftWidth, srcCenterHeight), color);

    // CENTER: xb, yb, xc, yc
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYB, dstXC, dstYC);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcLeftWidth, srcTopHeight, srcCenterWidth, srcCenterHeight), color);

    // RIGHT SIDE: xc, yb, xd, yc
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYB, dstXD, dstYC);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcRightStart, srcTopHeight, srcRightWidth, srcCenterHeight), color);

    // BOTTOM_LEFT CORNER: xa, yc, xb, yd
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYC, dstXB, dstYD);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(PxValue(0), srcBottomStart, srcLeftWidth, srcBottomHeight), color);

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
    if (textureSize.RawWidth() < nineSlice.SumX() || textureSize.RawHeight() < nineSlice.SumY())
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

    const PxSize1D srcLeftWidth = PxSize1D::UncheckedCreate(nineSlice.SliceFromTopLeftX());
    const PxSize1D srcTopHeight = PxSize1D::UncheckedCreate(nineSlice.SliceFromTopLeftY());
    const PxSize1D srcCenterWidth = PxSize1D::UncheckedCreate(textureSize.RawWidth() - nineSlice.SumX());
    const PxSize1D srcCenterHeight = PxSize1D::UncheckedCreate(textureSize.RawHeight() - nineSlice.SumY());
    const PxSize1D srcRightWidth = PxSize1D::UncheckedCreate(nineSlice.SliceFromBottomRightX());
    const PxSize1D srcBottomHeight = PxSize1D::UncheckedCreate(nineSlice.SliceFromBottomRightY());

    const PxSize1DF rect0Width(srcLeftWidth);
    const PxSize1DF rect0Height(srcTopHeight);
    const PxSize1DF rect4Width(srcCenterWidth);
    const PxSize1DF rect4Height(srcCenterHeight);
    const PxSize1DF rect8Width(srcRightWidth);
    const PxSize1DF rect8Height(srcBottomHeight);

    PxSize1DF bgLeft = std::max(rect0Width, PxSize1DF(minimumBackgroundMarginPx.Left()));
    PxSize1DF bgTop = std::max(rect0Height, PxSize1DF(minimumBackgroundMarginPx.Top()));
    PxSize1DF bgRight = std::max(rect8Width, PxSize1DF(minimumBackgroundMarginPx.Right()));
    PxSize1DF bgBottom = std::max(rect8Height, PxSize1DF(minimumBackgroundMarginPx.Bottom()));
    PxSize1DF bgSizeX = bgLeft + bgRight + rect4Width;
    PxSize1DF bgSizeY = bgTop + bgBottom + rect4Height;
    {
      PxSize2DF bgScale;
      if (!UIScaleUtil::TryCalcScaling(bgScale, dstRectanglePxf.Size(), PxSize2DF(bgSizeX, bgSizeY), ItemScalePolicy::DownscaleKeepAR))
      {
        return;
      }

      bgLeft *= bgScale.Width();
      bgRight *= bgScale.Width();
      bgTop *= bgScale.Height();
      bgBottom *= bgScale.Height();
    }

    const PxValueF dstXA = dstRectanglePxf.Left();
    const PxValueF dstYA = dstRectanglePxf.Top();
    const PxValueF dstXB = dstXA + bgLeft;
    const PxValueF dstYB = dstYA + bgTop;
    const PxValueF dstXD = dstRectanglePxf.Right();
    const PxValueF dstYD = dstRectanglePxf.Bottom();
    const PxValueF dstXC = dstXD - bgRight;
    const PxValueF dstYC = dstYD - bgBottom;

    PxAreaRectangleF finalDstRectPxf;

    const PxSize1D srcRightStart = srcLeftWidth + srcCenterWidth;
    const PxSize1D srcBottomStart = srcTopHeight + srcCenterHeight;


    // TOP_LEFT CORNER: xa, ya, xb, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYA, dstXB, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(PxValue(0), PxValue(0), srcLeftWidth, srcTopHeight), color);

    // TOP: xb, ya, xc, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYA, dstXC, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcLeftWidth, PxValue(0), srcCenterWidth, srcTopHeight), color);

    // TOP_RIGHT CORNER: xc, ya, xd, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYA, dstXD, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcRightStart, PxValue(0), srcRightWidth, srcTopHeight), color);

    // LEFT SIDE: xa, yb, xb, yc
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYB, dstXB, dstYC);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(PxValue(0), srcTopHeight, srcLeftWidth, srcCenterHeight), color);

    // CENTER: xb, yb, xc, yc
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYB, dstXC, dstYC);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcLeftWidth, srcTopHeight, srcCenterWidth, srcCenterHeight), color);

    // RIGHT SIDE: xc, yb, xd, yc
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYB, dstXD, dstYC);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(srcRightStart, srcTopHeight, srcRightWidth, srcCenterHeight), color);

    // BOTTOM_LEFT CORNER: xa, yc, xb, yd
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYC, dstXB, dstYD);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangle(PxValue(0), srcBottomStart, srcLeftWidth, srcBottomHeight), color);

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
    if (textureSize.RawWidth() < nineSlice.SumX() || textureSize.RawHeight() < nineSlice.SumY())
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

    const PxValueU srcLeftWidth(UncheckedNumericCast<PxValueU::raw_value_type>(nineSlice.SliceFromTopLeftX()));
    const PxValueU srcTopHeight(UncheckedNumericCast<PxValueU::raw_value_type>(nineSlice.SliceFromTopLeftY()));
    const PxValueU srcCenterWidth(UncheckedNumericCast<PxValueU::raw_value_type>(textureSize.RawWidth() - nineSlice.SumX()));
    const PxValueU srcCenterHeight(UncheckedNumericCast<PxValueU::raw_value_type>(textureSize.RawHeight() - nineSlice.SumY()));
    const PxValueU srcRightWidth(UncheckedNumericCast<PxValueU::raw_value_type>(nineSlice.SliceFromBottomRightX()));
    const PxValueU srcBottomHeight(UncheckedNumericCast<PxValueU::raw_value_type>(nineSlice.SliceFromBottomRightY()));

    const PxSize1DF rect0Width(srcTopHeight);
    const PxSize1DF rect0Height(srcLeftWidth);
    const PxSize1DF rect4Width(srcCenterHeight);
    const PxSize1DF rect4Height(srcCenterWidth);
    const PxSize1DF rect8Width(srcBottomHeight);
    const PxSize1DF rect8Height(srcRightWidth);

    PxSize1DF bgLeft = rect0Width;
    PxSize1DF bgTop = rect0Height;
    PxSize1DF bgRight = rect8Width;
    PxSize1DF bgBottom = rect8Height;
    PxSize1DF bgSizeX = bgLeft + bgRight + rect4Width;
    PxSize1DF bgSizeY = bgTop + bgBottom + rect4Height;
    {
      PxSize2DF bgScale;
      if (!UIScaleUtil::TryCalcScaling(bgScale, dstRectanglePxf.Size(), PxSize2DF(bgSizeX, bgSizeY), ItemScalePolicy::DownscaleKeepAR))
      {
        return;
      }

      bgLeft *= bgScale.Width();
      bgRight *= bgScale.Width();
      bgTop *= bgScale.Height();
      bgBottom *= bgScale.Height();
    }

    const PxValueF dstXA = dstRectanglePxf.Left();
    const PxValueF dstYA = dstRectanglePxf.Top();
    const PxValueF dstXB = dstXA + bgLeft;
    const PxValueF dstYB = dstYA + bgTop;
    const PxValueF dstXD = dstRectanglePxf.Right();
    const PxValueF dstYD = dstRectanglePxf.Bottom();
    const PxValueF dstXC = dstXD - bgRight;
    const PxValueF dstYC = dstYD - bgBottom;

    PxAreaRectangleF finalDstRect;

    const PxValueU srcRightStart(srcLeftWidth + srcCenterWidth);
    const PxValueU srcBottomStart(srcTopHeight + srcCenterHeight);

    // TOP_LEFT CORNER: xa, ya, xb, yb
    finalDstRect = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYA, dstXB, dstYB);
    batch2D->Draw(texture, finalDstRect, PxRectangleU32(PxValueU(0), srcBottomStart, srcLeftWidth, srcBottomHeight), color,
                  BatchEffect::Rotate90Clockwise);

    // TOP: xb, ya, xc, yb
    finalDstRect = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYA, dstXC, dstYB);
    batch2D->Draw(texture, finalDstRect, PxRectangleU32(PxValueU(0), srcTopHeight, srcLeftWidth, srcCenterHeight), color,
                  BatchEffect::Rotate90Clockwise);

    // TOP_RIGHT CORNER: xc, ya, xd, yb
    finalDstRect = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYA, dstXD, dstYB);
    batch2D->Draw(texture, finalDstRect, PxRectangleU32(PxValueU(0), PxValueU(0), srcLeftWidth, srcTopHeight), color, BatchEffect::Rotate90Clockwise);

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
    batch2D->Draw(texture, finalDstRect, PxRectangleU32(srcLeftWidth, PxValueU(0), srcCenterWidth, srcTopHeight), color,
                  BatchEffect::Rotate90Clockwise);

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
    batch2D->Draw(texture, finalDstRect, PxRectangleU32(srcRightStart, PxValueU(0), srcRightWidth, srcTopHeight), color,
                  BatchEffect::Rotate90Clockwise);
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
    if (textureSize.RawWidth() < nineSlice.SumX() || textureSize.RawHeight() < nineSlice.SumY())
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

    const PxValueU srcLeftWidth(UncheckedNumericCast<PxValueU::raw_value_type>(nineSlice.SliceFromTopLeftX()));
    const PxValueU srcTopHeight(UncheckedNumericCast<PxValueU::raw_value_type>(nineSlice.SliceFromTopLeftY()));
    const PxValueU srcCenterWidth(UncheckedNumericCast<PxValueU::raw_value_type>(textureSize.RawWidth() - nineSlice.SumX()));
    const PxValueU srcCenterHeight(UncheckedNumericCast<PxValueU::raw_value_type>(textureSize.RawHeight() - nineSlice.SumY()));
    const PxValueU srcRightWidth(UncheckedNumericCast<PxValueU::raw_value_type>(nineSlice.SliceFromBottomRightX()));
    const PxValueU srcBottomHeight(UncheckedNumericCast<PxValueU::raw_value_type>(nineSlice.SliceFromBottomRightY()));

    const PxSize1DF rect0Width(srcTopHeight);
    const PxSize1DF rect0Height(srcLeftWidth);
    const PxSize1DF rect4Width(srcCenterHeight);
    const PxSize1DF rect4Height(srcCenterWidth);
    const PxSize1DF rect8Width(srcBottomHeight);
    const PxSize1DF rect8Height(srcRightWidth);

    PxSize1DF bgLeft = std::max(rect0Width, PxSize1DF::UncheckedCreate(minimumBackgroundMargin.Top()));
    PxSize1DF bgTop = std::max(rect0Height, PxSize1DF::UncheckedCreate(minimumBackgroundMargin.Left()));
    PxSize1DF bgRight = std::max(rect8Width, PxSize1DF::UncheckedCreate(minimumBackgroundMargin.Bottom()));
    PxSize1DF bgBottom = std::max(rect8Height, PxSize1DF::UncheckedCreate(minimumBackgroundMargin.Right()));
    PxSize1DF bgSizeX = bgLeft + bgRight + rect4Width;
    PxSize1DF bgSizeY = bgTop + bgBottom + rect4Height;
    {
      PxSize2DF bgScale;
      if (!UIScaleUtil::TryCalcScaling(bgScale, dstRectanglePxf.Size(), PxSize2DF(bgSizeX, bgSizeY), ItemScalePolicy::DownscaleKeepAR))
      {
        return;
      }

      bgLeft *= bgScale.Width();
      bgRight *= bgScale.Width();
      bgTop *= bgScale.Height();
      bgBottom *= bgScale.Height();
    }

    const PxValueF dstXA = dstRectanglePxf.Left();
    const PxValueF dstYA = dstRectanglePxf.Top();
    const PxValueF dstXB = dstXA + bgLeft;
    const PxValueF dstYB = dstYA + bgTop;
    const PxValueF dstXD = dstRectanglePxf.Right();
    const PxValueF dstYD = dstRectanglePxf.Bottom();
    const PxValueF dstXC = dstXD - bgRight;
    const PxValueF dstYC = dstYD - bgBottom;

    PxAreaRectangleF finalDstRectPxf;

    const PxValueU srcRightStart = srcLeftWidth + srcCenterWidth;
    const PxValueU srcBottomStart = srcTopHeight + srcCenterHeight;

    // TOP_LEFT CORNER: xa, ya, xb, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXA, dstYA, dstXB, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangleU32(PxValueU(0), srcBottomStart, srcLeftWidth, srcBottomHeight), color,
                  BatchEffect::Rotate90Clockwise);

    // TOP: xb, ya, xc, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXB, dstYA, dstXC, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangleU32(PxValueU(0), srcTopHeight, srcLeftWidth, srcCenterHeight), color,
                  BatchEffect::Rotate90Clockwise);

    // TOP_RIGHT CORNER: xc, ya, xd, yb
    finalDstRectPxf = PxAreaRectangleF::FromLeftTopRightBottom(dstXC, dstYA, dstXD, dstYB);
    batch2D->Draw(texture, finalDstRectPxf, PxRectangleU32(PxValueU(0), PxValueU(0), srcLeftWidth, srcTopHeight), color,
                  BatchEffect::Rotate90Clockwise);

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
    batch2D->Draw(texture, finalDstRectPxf, PxRectangleU32(srcLeftWidth, PxValueU(0), srcCenterWidth, srcTopHeight), color,
                  BatchEffect::Rotate90Clockwise);

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
    batch2D->Draw(texture, finalDstRectPxf, PxRectangleU32(srcRightStart, PxValueU(0), srcRightWidth, srcTopHeight), color,
                  BatchEffect::Rotate90Clockwise);
  }
}

#endif
