/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include "GridRenderNativeBatchCRSpline3.hpp"
#include <FslBase/Math/VectorHelper.hpp>
#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <cassert>
#include "../TextureUtil.hpp"

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    inline void DrawLine(NativeBatch2D* pBatch, const GLBatch2D::texture_type& texFill, const PxRectangleU32 nativeTexRect, const Vector2& start,
                         const Vector2& end, const Color& color, const float thickness)
    {
      Vector2 delta = end - start;
      pBatch->Draw(texFill, start, nativeTexRect, color, VectorHelper::VectorToAngle(delta), Vector2(0, 0), Vector2(delta.Length(), thickness));
    }
  }


  //! The CatmullRom spline smoothing is now done in 3D in a initial line generation pass
  //! then the lines are converted to 2d points and finally rendered.
  //! This method produces a better looking grid and has the benefit of being slightly faster to render multiple times as the line generation code
  //! isn't run on each draw call.
  GridRenderNativeBatchCRSpline3::GridRenderNativeBatchCRSpline3(const Point2& gridSize)
    : m_gridSize(gridSize)
    , m_gridFinalSize((2 * gridSize.X) - 1, (2 * gridSize.Y) - 1)
    , m_coordinates3D((m_gridFinalSize.X * m_gridSize.Y) + (m_gridFinalSize.Y * m_gridSize.X))
    , m_coordinates2D((m_gridFinalSize.X * m_gridSize.Y) + (m_gridFinalSize.Y * m_gridSize.X))
  {
  }


  const char* GridRenderNativeBatchCRSpline3::GetName() const
  {
    return "NativeBatch Catmull-Rom spline3";
  }


  void GridRenderNativeBatchCRSpline3::Update(const DemoTime& /*demoTime*/, const Vector2& areaSize, const std::vector<PointMass>& points)
  {
    Calc3DCoordinates(m_coordinates3D, points);
    CalcFinalCoordinates(m_coordinates2D, m_coordinates3D, areaSize);
  }


  void GridRenderNativeBatchCRSpline3::Draw(const GridRenderDrawContext& drawContext, const std::vector<PointMass>& /*points*/)
  {
    GLBatch2D::texture_type texFillNative = TextureUtil::ToNative(drawContext.RenderSystem, drawContext.TexFill);

    const PxRectangleU32 texTrimmedRect(drawContext.TexFill.GetInfo().TrimmedRectPx);
    const PxRectangleU32 rectFillTex(texTrimmedRect.X + (texTrimmedRect.Width / 2), texTrimmedRect.Y + (texTrimmedRect.Height / 2), 1, 1);

    auto* pBatch = drawContext.pBatch;

    int width = m_gridFinalSize.X;
    int height = m_gridFinalSize.Y;
    Color color(0.12f, 0.12f, 0.55f, 0.33f);

    auto* pSrc = m_coordinates2D.data();
    auto* const pSrcArrayEnd = pSrc + m_coordinates2D.size();

    {    // Draw horizontal lines
      auto* const pSrcEnd = pSrc + (m_gridFinalSize.X * m_gridSize.Y);
      assert(pSrcEnd < pSrcArrayEnd);
      int lineCount = 0;
      while (pSrc < pSrcEnd)
      {
        float thickness = (lineCount % 5 == 0) ? 3.0f : 1.0f;
        for (int x = 0; x < (width - 1); ++x)
        {
          assert((pSrc + x) < pSrcArrayEnd);
          assert((pSrc + x + 1) < pSrcArrayEnd);
          DrawLine(pBatch, texFillNative, rectFillTex, pSrc[x], pSrc[x + 1], color, thickness);
        }
        ++lineCount;
        pSrc += m_gridFinalSize.X;
      }
    }
    {    // Draw vertical lines
      int lineCount = 0;
      while (pSrc < pSrcArrayEnd)
      {
        float thickness = (lineCount % 5 == 0) ? 3.0f : 1.0f;
        for (int y = 0; y < (height - 1); ++y)
        {
          assert((pSrc + y) < pSrcArrayEnd);
          assert((pSrc + y + 1) < pSrcArrayEnd);
          DrawLine(pBatch, texFillNative, rectFillTex, pSrc[y], pSrc[y + 1], color, thickness);
        }
        ++lineCount;
        pSrc += height;
      }
    }
  }


  void GridRenderNativeBatchCRSpline3::Calc3DCoordinates(std::vector<Vector3>& rDst, const std::vector<PointMass>& points)
  {
    int32_t pointsWritten = CreateLinesHorizontal(rDst, points, 0);
    CreateLinesVertical(rDst, points, pointsWritten);
  }


  int32_t GridRenderNativeBatchCRSpline3::CreateLinesHorizontal(std::vector<Vector3>& rDst, const std::vector<PointMass>& points,
                                                                const std::ptrdiff_t dstOffset)
  {
    const int srcGridMaxX = m_gridSize.X;
    const int srcGridMaxY = m_gridSize.Y;
    const int dstGridMaxX = m_gridFinalSize.X;
    // const int dstGridMaxY = m_gridFinalSize.Y;
    const std::ptrdiff_t srcStride = srcGridMaxX;
    const std::ptrdiff_t dstStride = dstGridMaxX;

#ifndef NDEBUG
    const auto* const pDstEnd = rDst.data() + rDst.size();
#endif

    // Create the horizontal border points as they are a special case
    {
      const PointMass* pSrcLeft = points.data();
      const PointMass* pSrcRight = pSrcLeft + srcGridMaxX - 3;
      const PointMass* const pSrcEnd = pSrcLeft + (srcStride * srcGridMaxY);
      Vector3* pDstLeft = rDst.data() + dstOffset;
      Vector3* pDstRight = pDstLeft + dstGridMaxX - 2;
      while (pSrcLeft < pSrcEnd)
      {
        assert(pDstLeft < pDstEnd);
        assert(pDstRight < pDstEnd);
        assert((pDstLeft + 2) < pDstEnd);
        assert((pDstRight + 1) < pDstEnd);
        pDstLeft[0] = pSrcLeft[0].m_position;
        pDstLeft[1] = VectorHelper::CatmullRom(pSrcLeft[0].m_position, pSrcLeft[0].m_position, pSrcLeft[1].m_position, pSrcLeft[2].m_position, 0.5f);
        pDstLeft[2] = pSrcLeft[1].m_position;

        pDstRight[0] =
          VectorHelper::CatmullRom(pSrcRight[0].m_position, pSrcRight[1].m_position, pSrcRight[2].m_position, pSrcRight[2].m_position, 0.5f);
        pDstRight[1] = pSrcRight[2].m_position;

        pSrcLeft += srcStride;
        pSrcRight += srcStride;
        pDstLeft += dstStride;
        pDstRight += dstStride;
      }
    }

    {    // horizontal pass - transfer all existing coordinates and spawn the new horizontal coordinates
      const PointMass* pSrc = points.data();
      const PointMass* const pSrcEnd = pSrc + (srcStride * srcGridMaxY);
      // +3 to skip the three points written in the border handling code
      Vector3* pDst = rDst.data() + dstOffset + 3;
      const int constrainedGridMaxX = srcGridMaxX - 3;
      while (pSrc < pSrcEnd)
      {
        assert(pDst < pDstEnd);
        for (int x = 0; x < constrainedGridMaxX; ++x)
        {
          assert((pDst + (x * 2)) < pDstEnd);
          assert((pDst + (x * 2) + 1) < pDstEnd);
          pDst[x * 2] = VectorHelper::CatmullRom(pSrc[x].m_position, pSrc[x + 1].m_position, pSrc[x + 2].m_position, pSrc[x + 3].m_position, 0.5f);
          pDst[(x * 2) + 1] = pSrc[x + 2].m_position;
        }
        pSrc += srcStride;
        pDst += dstStride;
      }
    }
    return (m_gridFinalSize.X * m_gridSize.Y);
  }

  int32_t GridRenderNativeBatchCRSpline3::CreateLinesVertical(std::vector<Vector3>& rDst, const std::vector<PointMass>& points,
                                                              const std::ptrdiff_t dstOffset)
  {
    const int srcGridMaxX = m_gridSize.X;
    const int srcGridMaxY = m_gridSize.Y;
    // const int dstGridMaxX = m_gridFinalSize.X;
    const int dstGridMaxY = m_gridFinalSize.Y;
    const std::ptrdiff_t srcStride = srcGridMaxX;
    const std::ptrdiff_t dstStride = dstGridMaxY;

#ifndef NDEBUG
    const auto* const pDstEnd = rDst.data() + rDst.size();
#endif

    // Create the vertical border points as they are a special case
    {
      const PointMass* pSrcTop = points.data();
      const PointMass* pSrcBottom = pSrcTop + (srcStride * (srcGridMaxY - 3));

      Vector3* pDstTop = rDst.data() + dstOffset;
      Vector3* pDstBottom = pDstTop + (dstGridMaxY - 2);
      for (int x = 0; x < srcGridMaxX; ++x)
      {
        assert(pDstTop < pDstEnd);
        assert((pDstTop + 2) < pDstEnd);
        assert(pDstBottom < pDstEnd);
        assert(pDstBottom < (pDstTop + dstStride));
        assert((pDstBottom + 1) < (pDstTop + dstStride));

        pDstTop[0] = pSrcTop[x].m_position;
        pDstTop[1] = VectorHelper::CatmullRom(pSrcTop[x].m_position, pSrcTop[x].m_position, pSrcTop[x + srcStride].m_position,
                                              pSrcTop[x + (2 * srcStride)].m_position, 0.5f);
        pDstTop[2] = pSrcTop[x + srcStride].m_position;

        pDstBottom[0] = VectorHelper::CatmullRom(pSrcBottom[x].m_position, pSrcBottom[x + srcStride].m_position,
                                                 pSrcBottom[x + (srcStride * 2)].m_position, pSrcBottom[x + (srcStride * 2)].m_position, 0.5f);
        pDstBottom[1] = pSrcBottom[x + (srcStride * 2)].m_position;

        pDstTop += dstStride;
        pDstBottom += dstStride;
      }
    }

    {    // vertical pass - transfer all existing coordinates and spawn the new vertical coordinates
      // +3 to skip the three points written in the border handling code
      const PointMass* const pSrcStart = points.data();
      const PointMass* const pSrcConstrainedEnd = pSrcStart + (srcStride * (srcGridMaxY - 3));
      Vector3* pDstStart = rDst.data() + dstOffset + 3;

      for (int x = 0; x < srcGridMaxX; ++x)
      {
        const PointMass* pSrc = pSrcStart;
        Vector3* pDst = pDstStart;
        while (pSrc < pSrcConstrainedEnd)
        {
          assert(pDst < pDstEnd);
          assert((pDst + 1) < pDstEnd);
          pDst[0] = VectorHelper::CatmullRom(pSrc[x].m_position, pSrc[x + srcStride].m_position, pSrc[x + (2 * srcStride)].m_position,
                                             pSrc[x + (3 * srcStride)].m_position, 0.5f);
          pDst[1] = pSrc[x + (2 * srcStride)].m_position;

          pSrc += srcStride;
          pDst += 2;
        }
        pDstStart += dstStride;
      }
    }
    return (m_gridFinalSize.Y * m_gridSize.X);
  }


  void GridRenderNativeBatchCRSpline3::CalcFinalCoordinates(std::vector<Vector2>& rDst, const std::vector<Vector3>& src, const Vector2& finalSize)
  {
    assert(rDst.size() == src.size());

    const Vector2 halfSize(finalSize * 0.5f);

    const std::size_t count = src.size();
    const auto* pSrc = src.data();
    const auto* const pSrcEnd = pSrc + count;
    auto* pDst = rDst.data();

    // Convert to 2D
    while (pSrc < pSrcEnd)
    {
      float factor = (pSrc->Z + 2000.0f) * 0.0005f;
      pDst->X = ((pSrc->X - halfSize.X) * factor) + halfSize.X;
      pDst->Y = ((pSrc->Y - halfSize.Y) * factor) + halfSize.Y;
      ++pSrc;
      ++pDst;
    }
  }
}
