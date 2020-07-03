#ifndef FSLGRAPHICS_RENDER_BATCH2DUTIL_HPP
#define FSLGRAPHICS_RENDER_BATCH2DUTIL_HPP
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

#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslBase/Math/Pixel/PxClipRectangle.hpp>
#include <FslGraphics/NativeQuadTextureCoords.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <cassert>

namespace Fsl
{
  namespace Batch2DUtil
  {
    //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
    inline constexpr bool Clip(PxAreaRectangleF& rToClipRect, PxAreaRectangleF& rSrcTexRect, const PxAreaRectangleF& clipRect)
    {
      if (rToClipRect.Left() < clipRect.Right() && clipRect.Left() < rToClipRect.Right() && rToClipRect.Top() < clipRect.Bottom() &&
          clipRect.Top() < rToClipRect.Bottom())
      {
        float dx = rToClipRect.Right() - rToClipRect.Left();
        float dy = rToClipRect.Bottom() - rToClipRect.Top();
        float dsx = rSrcTexRect.Right() - rSrcTexRect.Left();
        float dsy = rSrcTexRect.Bottom() - rSrcTexRect.Top();
        if (dx <= 0.0f || dy <= 0.0f)
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
          float dxClip = clipRect.Left() - clippedLeft;
          float len = dxClip / dx;

          clippedSrcLeft += (dsx * len);
          clippedLeft = clipRect.Left();
        }
        if (clippedRight > clipRect.Right())
        {
          float dxClip = clippedRight - clipRect.Right();
          float len = dxClip / dx;
          clippedSrcRight -= (dsx * len);
          clippedRight = clipRect.Right();
        }

        if (clippedTop < clipRect.Top())
        {
          float dyClip = clipRect.Top() - clippedTop;
          float len = dyClip / dy;

          clippedSrcTop += (dsy * len);
          clippedTop = clipRect.Top();
        }
        if (clippedBottom > clipRect.Bottom())
        {
          float dyClip = clippedBottom - clipRect.Bottom();
          float len = dyClip / dy;
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
    inline constexpr bool Clip(PxAreaRectangleF& rToClipRect, PxAreaRectangleF& rSrcTexRect, const PxClipRectangle& clipRect)
    {
      if (rToClipRect.Left() < clipRect.Right() && clipRect.Left() < rToClipRect.Right() && rToClipRect.Top() < clipRect.Bottom() &&
          clipRect.Top() < rToClipRect.Bottom())
      {
        float dx = rToClipRect.Right() - rToClipRect.Left();
        float dy = rToClipRect.Bottom() - rToClipRect.Top();
        float dsx = rSrcTexRect.Right() - rSrcTexRect.Left();
        float dsy = rSrcTexRect.Bottom() - rSrcTexRect.Top();
        if (dx <= 0.0f || dy <= 0.0f)
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
          float dxClip = clipRect.Left() - clippedLeft;
          float len = dxClip / dx;

          clippedSrcLeft += (dsx * len);
          clippedLeft = float(clipRect.Left());
        }
        if (clippedRight > clipRect.Right())
        {
          float dxClip = clippedRight - clipRect.Right();
          float len = dxClip / dx;
          clippedSrcRight -= (dsx * len);
          clippedRight = float(clipRect.Right());
        }

        if (clippedTop < clipRect.Top())
        {
          float dyClip = clipRect.Top() - clippedTop;
          float len = dyClip / dy;

          clippedSrcTop += (dsy * len);
          clippedTop = float(clipRect.Top());
        }
        if (clippedBottom > clipRect.Bottom())
        {
          float dyClip = clippedBottom - clipRect.Bottom();
          float len = dyClip / dy;
          clippedSrcBottom -= (dsy * len);
          clippedBottom = float(clipRect.Bottom());
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
      if (rToClipRect.Left() < clipRect.Right() && clipRect.Left() < rToClipRect.Right() && rToClipRect.Top() < clipRect.Bottom() &&
          clipRect.Top() < rToClipRect.Bottom())
      {
        float dx = rToClipRect.Right() - rToClipRect.Left();
        float dy = rToClipRect.Bottom() - rToClipRect.Top();
        float dsx = rSrcTexArea.X1 - rSrcTexArea.X0;
        float dsy = rSrcTexArea.Y1 - rSrcTexArea.Y0;
        if (dx <= 0.0f || dy <= 0.0f)
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

        if (clippedLeft < clipRect.Left())
        {
          float dxClip = clipRect.Left() - clippedLeft;
          float len = dxClip / dx;

          clippedSrcX1 += (dsx * len);
          clippedLeft = float(clipRect.Left());
        }
        if (clippedRight > clipRect.Right())
        {
          float dxClip = clippedRight - clipRect.Right();
          float len = dxClip / dx;
          clippedSrcX2 -= (dsx * len);
          clippedRight = float(clipRect.Right());
        }

        if (clippedTop < clipRect.Top())
        {
          float dyClip = clipRect.Top() - clippedTop;
          float len = dyClip / dy;

          clippedSrcY1 += (dsy * len);
          clippedTop = float(clipRect.Top());
        }
        if (clippedBottom > clipRect.Bottom())
        {
          float dyClip = clippedBottom - clipRect.Bottom();
          float len = dyClip / dy;
          clippedSrcY2 -= (dsy * len);
          clippedBottom = float(clipRect.Bottom());
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
      if (rToClipRect.Left() < clipRect.Right() && clipRect.Left() < rToClipRect.Right() && rToClipRect.Top() < clipRect.Bottom() &&
          clipRect.Top() < rToClipRect.Bottom())
      {
        float dx = rToClipRect.Right() - rToClipRect.Left();
        float dy = rToClipRect.Bottom() - rToClipRect.Top();
        const Vector2 ds0 = rSrcTexArea.TopRight - rSrcTexArea.TopLeft;
        const Vector2 ds1 = rSrcTexArea.BottomRight - rSrcTexArea.BottomLeft;
        if (dx <= 0.0f || dy <= 0.0f)
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

        if (clippedLeft < clipRect.Left())
        {
          float dxClip = clipRect.Left() - clippedLeft;
          float len = dxClip / dx;

          clippedSrc0.X += (ds0.X * len);
          clippedSrc2.X += (ds1.X * len);
          clippedLeft = float(clipRect.Left());
        }
        if (clippedRight > clipRect.Right())
        {
          float dxClip = clippedRight - clipRect.Right();
          float len = dxClip / dx;
          clippedSrc1.X -= (ds0.X * len);
          clippedSrc3.X -= (ds1.X * len);
          clippedRight = float(clipRect.Right());
        }

        if (clippedTop < clipRect.Top())
        {
          float dyClip = clipRect.Top() - clippedTop;
          float len = dyClip / dy;

          clippedSrc0.Y += (ds0.Y * len);
          clippedSrc2.Y += (ds1.Y * len);
          clippedTop = float(clipRect.Top());
        }
        if (clippedBottom > clipRect.Bottom())
        {
          float dyClip = clippedBottom - clipRect.Bottom();
          float len = dyClip / dy;
          clippedSrc1.Y -= (ds0.Y * len);
          clippedSrc3.Y -= (ds1.Y * len);
          clippedBottom = float(clipRect.Bottom());
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
}

#endif
