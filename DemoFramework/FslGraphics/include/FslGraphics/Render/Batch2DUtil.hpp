#ifndef FSLGRAPHICS_RENDER_BATCH2DUTIL_HPP
#define FSLGRAPHICS_RENDER_BATCH2DUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2023 NXP
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

#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslBase/Math/Pixel/PxClipRectangle.hpp>
#include <FslGraphics/NativeQuadTextureCoords.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <cassert>

namespace Fsl::Batch2DUtil
{
  //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
  inline constexpr bool Clip(PxAreaRectangleF& rToClipRect, PxAreaRectangleF& rSrcTexRect, const PxAreaRectangleF& clipRect)
  {
    if (rToClipRect.Left() < clipRect.Right() && clipRect.Left() < rToClipRect.Right() && rToClipRect.Top() < clipRect.Bottom() &&
        clipRect.Top() < rToClipRect.Bottom())
    {
      auto dx = rToClipRect.Right() - rToClipRect.Left();
      auto dy = rToClipRect.Bottom() - rToClipRect.Top();
      auto dsx = rSrcTexRect.Right() - rSrcTexRect.Left();
      auto dsy = rSrcTexRect.Bottom() - rSrcTexRect.Top();
      if (dx.Value <= 0.0f || dy.Value <= 0.0f)
      {
        // The rect was clipped to zero in one direction
        return false;
      }

      auto clippedLeft = rToClipRect.Left();
      auto clippedRight = rToClipRect.Right();
      auto clippedTop = rToClipRect.Top();
      auto clippedBottom = rToClipRect.Bottom();

      auto clippedSrcLeft = rSrcTexRect.Left();
      auto clippedSrcRight = rSrcTexRect.Right();
      auto clippedSrcTop = rSrcTexRect.Top();
      auto clippedSrcBottom = rSrcTexRect.Bottom();

      if (clippedLeft < clipRect.Left())
      {
        auto dxClip = clipRect.Left() - clippedLeft;
        auto len = dxClip / dx;

        clippedSrcLeft += (dsx * len);
        clippedLeft = clipRect.Left();
      }
      if (clippedRight > clipRect.Right())
      {
        auto dxClip = clippedRight - clipRect.Right();
        auto len = dxClip / dx;
        clippedSrcRight -= (dsx * len);
        clippedRight = clipRect.Right();
      }

      if (clippedTop < clipRect.Top())
      {
        auto dyClip = clipRect.Top() - clippedTop;
        auto len = dyClip / dy;

        clippedSrcTop += (dsy * len);
        clippedTop = clipRect.Top();
      }
      if (clippedBottom > clipRect.Bottom())
      {
        auto dyClip = clippedBottom - clipRect.Bottom();
        auto len = dyClip / dy;
        clippedSrcBottom -= (dsy * len);
        clippedBottom = clipRect.Bottom();
      }

      assert(clippedLeft >= rToClipRect.Left());
      assert(clippedTop >= rToClipRect.Top());
      assert(clippedRight <= rToClipRect.Right());
      assert(clippedBottom <= rToClipRect.Bottom());
      assert(clippedSrcLeft >= rSrcTexRect.Left());
      assert(clippedSrcTop >= rSrcTexRect.Top());
      assert(clippedSrcRight <= rSrcTexRect.Right());
      assert(clippedSrcBottom <= rSrcTexRect.Bottom());

      rToClipRect = PxAreaRectangleF::FromLeftTopRightBottom(clippedLeft, clippedTop, clippedRight, clippedBottom);
      rSrcTexRect = PxAreaRectangleF::FromLeftTopRightBottom(clippedSrcLeft, clippedSrcTop, clippedSrcRight, clippedSrcBottom);
      return true;
    }
    return false;
  }

  //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
  inline constexpr bool Clip(PxAreaRectangleF& rToClipRect, NativeTextureArea& rSrcTexArea, const PxAreaRectangleF& clipRect)
  {
    if (rToClipRect.Left() < clipRect.Right() && clipRect.Left() < rToClipRect.Right() && rToClipRect.Top() < clipRect.Bottom() &&
        clipRect.Top() < rToClipRect.Bottom())
    {
      auto dx = rToClipRect.Right() - rToClipRect.Left();
      auto dy = rToClipRect.Bottom() - rToClipRect.Top();
      auto dsx = rSrcTexArea.X1 - rSrcTexArea.X0;
      auto dsy = rSrcTexArea.Y1 - rSrcTexArea.Y0;
      if (dx.Value <= 0.0f || dy.Value <= 0.0f)
      {
        // The rect was clipped to zero in one direction
        return false;
      }

      auto clippedLeft = rToClipRect.Left();
      auto clippedRight = rToClipRect.Right();
      auto clippedTop = rToClipRect.Top();
      auto clippedBottom = rToClipRect.Bottom();

      auto clippedSrcLeft = rSrcTexArea.X0;
      auto clippedSrcRight = rSrcTexArea.X1;
      auto clippedSrcTop = rSrcTexArea.Y0;
      auto clippedSrcBottom = rSrcTexArea.Y1;

      if (clippedLeft < clipRect.Left())
      {
        auto dxClip = clipRect.Left() - clippedLeft;
        auto len = dxClip / dx;

        clippedSrcLeft += (dsx * len.Value);
        clippedLeft = clipRect.Left();
      }
      if (clippedRight > clipRect.Right())
      {
        auto dxClip = clippedRight - clipRect.Right();
        auto len = dxClip / dx;
        clippedSrcRight -= (dsx * len.Value);
        clippedRight = clipRect.Right();
      }

      if (clippedTop < clipRect.Top())
      {
        auto dyClip = clipRect.Top() - clippedTop;
        auto len = dyClip / dy;

        clippedSrcTop += (dsy * len.Value);
        clippedTop = clipRect.Top();
      }
      if (clippedBottom > clipRect.Bottom())
      {
        auto dyClip = clippedBottom - clipRect.Bottom();
        auto len = dyClip / dy;
        clippedSrcBottom -= (dsy * len.Value);
        clippedBottom = clipRect.Bottom();
      }

      assert(clippedLeft >= rToClipRect.Left());
      assert(clippedTop >= rToClipRect.Top());
      assert(clippedRight <= rToClipRect.Right());
      assert(clippedBottom <= rToClipRect.Bottom());

      rToClipRect = PxAreaRectangleF::FromLeftTopRightBottom(clippedLeft, clippedTop, clippedRight, clippedBottom);
      rSrcTexArea = NativeTextureArea(clippedSrcLeft, clippedSrcTop, clippedSrcRight, clippedSrcBottom);
      return true;
    }
    return false;
  }

  //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
  inline constexpr bool Clip(PxAreaRectangleF& rToClipRect, PxAreaRectangleF& rSrcTexRect, const PxClipRectangle& clipRect)
  {
    const auto clipRectLeft = PxValueF(static_cast<float>(clipRect.RawLeft()));
    const auto clipRectTop = PxValueF(static_cast<float>(clipRect.RawTop()));
    const auto clipRectRight = PxValueF(static_cast<float>(clipRect.RawRight()));
    const auto clipRectBottom = PxValueF(static_cast<float>(clipRect.RawBottom()));
    if (rToClipRect.Left() < clipRectRight && clipRectLeft < rToClipRect.Right() && rToClipRect.Top() < clipRectBottom &&
        clipRectTop < rToClipRect.Bottom())
    {
      auto dx = rToClipRect.Right() - rToClipRect.Left();
      auto dy = rToClipRect.Bottom() - rToClipRect.Top();
      auto dsx = rSrcTexRect.Right() - rSrcTexRect.Left();
      auto dsy = rSrcTexRect.Bottom() - rSrcTexRect.Top();
      if (dx.Value <= 0.0f || dy.Value <= 0.0f)
      {
        // The rect was clipped to zero in one direction
        return false;
      }

      auto clippedLeft = rToClipRect.Left();
      auto clippedRight = rToClipRect.Right();
      auto clippedTop = rToClipRect.Top();
      auto clippedBottom = rToClipRect.Bottom();

      auto clippedSrcLeft = rSrcTexRect.Left();
      auto clippedSrcRight = rSrcTexRect.Right();
      auto clippedSrcTop = rSrcTexRect.Top();
      auto clippedSrcBottom = rSrcTexRect.Bottom();

      if (clippedLeft < clipRectLeft)
      {
        auto dxClip = clipRectLeft - clippedLeft;
        auto len = dxClip / dx;

        clippedSrcLeft += (dsx * len);
        clippedLeft = clipRectLeft;
      }
      if (clippedRight > clipRectRight)
      {
        auto dxClip = clippedRight - clipRectRight;
        auto len = dxClip / dx;
        clippedSrcRight -= (dsx * len);
        clippedRight = clipRectRight;
      }

      if (clippedTop < clipRectTop)
      {
        auto dyClip = clipRectTop - clippedTop;
        auto len = dyClip / dy;

        clippedSrcTop += (dsy * len);
        clippedTop = clipRectTop;
      }
      if (clippedBottom > clipRectBottom)
      {
        auto dyClip = clippedBottom - clipRectBottom;
        auto len = dyClip / dy;
        clippedSrcBottom -= (dsy * len);
        clippedBottom = clipRectBottom;
      }

      assert(clippedLeft >= rToClipRect.Left());
      assert(clippedTop >= rToClipRect.Top());
      assert(clippedRight <= rToClipRect.Right());
      assert(clippedBottom <= rToClipRect.Bottom());
      assert(clippedSrcLeft >= rSrcTexRect.Left());
      assert(clippedSrcTop >= rSrcTexRect.Top());
      assert(clippedSrcRight <= rSrcTexRect.Right());
      assert(clippedSrcBottom <= rSrcTexRect.Bottom());

      rToClipRect = PxAreaRectangleF::FromLeftTopRightBottom(clippedLeft, clippedTop, clippedRight, clippedBottom);
      rSrcTexRect = PxAreaRectangleF::FromLeftTopRightBottom(clippedSrcLeft, clippedSrcTop, clippedSrcRight, clippedSrcBottom);
      return true;
    }
    return false;
  }


  //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
  inline constexpr bool Clip(PxAreaRectangleF& rToClipRect, NativeTextureArea& rSrcTexArea, const PxClipRectangle& clipRect)
  {
    const auto clipRectLeft = PxValueF(static_cast<float>(clipRect.RawLeft()));
    const auto clipRectTop = PxValueF(static_cast<float>(clipRect.RawTop()));
    const auto clipRectRight = PxValueF(static_cast<float>(clipRect.RawRight()));
    const auto clipRectBottom = PxValueF(static_cast<float>(clipRect.RawBottom()));

    if (rToClipRect.Left() < clipRectRight && clipRectLeft < rToClipRect.Right() && rToClipRect.Top() < clipRectBottom &&
        clipRectTop < rToClipRect.Bottom())
    {
      auto dx = rToClipRect.Right() - rToClipRect.Left();
      auto dy = rToClipRect.Bottom() - rToClipRect.Top();
      auto dsx = rSrcTexArea.X1 - rSrcTexArea.X0;
      auto dsy = rSrcTexArea.Y1 - rSrcTexArea.Y0;
      if (dx.Value <= 0.0f || dy.Value <= 0.0f)
      {
        // The rect was clipped to zero in one direction
        return false;
      }

      auto clippedLeft = rToClipRect.Left();
      auto clippedRight = rToClipRect.Right();
      auto clippedTop = rToClipRect.Top();
      auto clippedBottom = rToClipRect.Bottom();

      auto clippedSrcX1 = rSrcTexArea.X0;
      auto clippedSrcX2 = rSrcTexArea.X1;
      auto clippedSrcY1 = rSrcTexArea.Y0;
      auto clippedSrcY2 = rSrcTexArea.Y1;

      if (clippedLeft < clipRectLeft)
      {
        auto dxClip = clipRectLeft - clippedLeft;
        auto len = dxClip / dx;

        clippedSrcX1 += (dsx * len.Value);
        clippedLeft = clipRectLeft;
      }
      if (clippedRight > clipRectRight)
      {
        auto dxClip = clippedRight - clipRectRight;
        auto len = dxClip / dx;
        clippedSrcX2 -= (dsx * len.Value);
        clippedRight = clipRectRight;
      }

      if (clippedTop < clipRectTop)
      {
        auto dyClip = clipRectTop - clippedTop;
        auto len = dyClip / dy;

        clippedSrcY1 += (dsy * len.Value);
        clippedTop = clipRectTop;
      }
      if (clippedBottom > clipRectBottom)
      {
        auto dyClip = clippedBottom - clipRectBottom;
        auto len = dyClip / dy;
        clippedSrcY2 -= (dsy * len.Value);
        clippedBottom = clipRectBottom;
      }

      assert(clippedLeft >= rToClipRect.Left());
      assert(clippedTop >= rToClipRect.Top());
      assert(clippedRight <= rToClipRect.Right());
      assert(clippedBottom <= rToClipRect.Bottom());

      rToClipRect = PxAreaRectangleF::FromLeftTopRightBottom(clippedLeft, clippedTop, clippedRight, clippedBottom);
      rSrcTexArea = NativeTextureArea(clippedSrcX1, clippedSrcY1, clippedSrcX2, clippedSrcY2);
      return true;
    }
    return false;
  }

  //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
  inline constexpr bool Clip(PxAreaRectangleF& rToClipRect, NativeQuadTextureCoords& rSrcTexArea, const PxClipRectangle& clipRect)
  {
    const auto clipRectLeft = PxValueF(static_cast<float>(clipRect.RawLeft()));
    const auto clipRectTop = PxValueF(static_cast<float>(clipRect.RawTop()));
    const auto clipRectRight = PxValueF(static_cast<float>(clipRect.RawRight()));
    const auto clipRectBottom = PxValueF(static_cast<float>(clipRect.RawBottom()));

    if (rToClipRect.Left() < clipRectRight && clipRectLeft < rToClipRect.Right() && rToClipRect.Top() < clipRectBottom &&
        clipRectTop < rToClipRect.Bottom())
    {
      auto dx = rToClipRect.Right() - rToClipRect.Left();
      auto dy = rToClipRect.Bottom() - rToClipRect.Top();
      const Vector2 ds0 = rSrcTexArea.TopRight - rSrcTexArea.TopLeft;
      const Vector2 ds1 = rSrcTexArea.BottomRight - rSrcTexArea.BottomLeft;
      if (dx.Value <= 0.0f || dy.Value <= 0.0f)
      {
        // The rect was clipped to zero in one direction
        return false;
      }

      auto clippedLeft = rToClipRect.Left();
      auto clippedRight = rToClipRect.Right();
      auto clippedTop = rToClipRect.Top();
      auto clippedBottom = rToClipRect.Bottom();

      auto clippedSrc0 = rSrcTexArea.TopLeft;
      auto clippedSrc1 = rSrcTexArea.TopRight;
      auto clippedSrc2 = rSrcTexArea.BottomLeft;
      auto clippedSrc3 = rSrcTexArea.BottomRight;

      if (clippedLeft < clipRectLeft)
      {
        auto dxClip = clipRectLeft - clippedLeft;
        auto len = dxClip / dx;

        clippedSrc0.X += (ds0.X * len.Value);
        clippedSrc2.X += (ds1.X * len.Value);
        clippedLeft = clipRectLeft;
      }
      if (clippedRight > clipRectRight)
      {
        auto dxClip = clippedRight - clipRectRight;
        auto len = dxClip / dx;
        clippedSrc1.X -= (ds0.X * len.Value);
        clippedSrc3.X -= (ds1.X * len.Value);
        clippedRight = clipRectRight;
      }

      if (clippedTop < clipRectTop)
      {
        auto dyClip = clipRectTop - clippedTop;
        auto len = dyClip / dy;

        clippedSrc0.Y += (ds0.Y * len.Value);
        clippedSrc2.Y += (ds1.Y * len.Value);
        clippedTop = clipRectTop;
      }
      if (clippedBottom > clipRectBottom)
      {
        auto dyClip = clippedBottom - clipRectBottom;
        auto len = dyClip / dy;
        clippedSrc1.Y -= (ds0.Y * len.Value);
        clippedSrc3.Y -= (ds1.Y * len.Value);
        clippedBottom = clipRectBottom;
      }

      assert(clippedLeft >= rToClipRect.Left());
      assert(clippedTop >= rToClipRect.Top());
      assert(clippedRight <= rToClipRect.Right());
      assert(clippedBottom <= rToClipRect.Bottom());

      rToClipRect = PxAreaRectangleF::FromLeftTopRightBottom(clippedLeft, clippedTop, clippedRight, clippedBottom);
      rSrcTexArea = NativeQuadTextureCoords(clippedSrc0, clippedSrc1, clippedSrc2, clippedSrc3);
      return true;
    }
    return false;
  }

}

#endif
