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

#include "GridRenderNativeBatchCRSpline2.hpp"
#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <FslBase/Math/VectorHelper.hpp>
#include <cassert>
#include "../TextureUtil.hpp"

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    inline void DrawLine(NativeBatch2D* pBatch, const GLBatch2D::texture_type& texFill, const PxRectangleU32& nativeTexRect, const Vector2& start,
                         const Vector2& end, const Color& color, const float thickness)
    {
      Vector2 delta = end - start;
      pBatch->Draw(texFill, start, nativeTexRect, color, VectorHelper::VectorToAngle(delta), Vector2(0, 0), Vector2(delta.Length(), thickness));
    }
  }


  //! Changed the v1 implementation to do a initial 3d -> 2d pass before we start doing the line rendering
  //! V1 this one the fly instead.
  GridRenderNativeBatchCRSpline2::GridRenderNativeBatchCRSpline2(const Point2& gridSize)
    : m_gridSize(gridSize)
    , m_coordinates2D(gridSize.X * gridSize.Y)
  {
  }


  const char* GridRenderNativeBatchCRSpline2::GetName() const
  {
    return "NativeBatch Catmull-Rom spline2";
  }


  void GridRenderNativeBatchCRSpline2::Update(const DemoTime& /*demoTime*/, const Vector2& areaSize, const std::vector<PointMass>& points)
  {
    CalcFinalCoordinates(points, areaSize);
  }


  void GridRenderNativeBatchCRSpline2::Draw(const GridRenderDrawContext& drawContext, const std::vector<PointMass>& /*points*/)
  {
    GLBatch2D::texture_type texFillNative = TextureUtil::ToNative(drawContext.RenderSystem, drawContext.TexFill);

    const auto texTrimmedRect = drawContext.TexFill.GetInfo().TrimmedRectPx;
    constexpr auto size1Px = PxValueU::Create(1);
    constexpr auto size2Px = PxValueU::Create(2);
    const PxRectangleU32 rectFillTex(texTrimmedRect.X + (texTrimmedRect.Width / size2Px), texTrimmedRect.Y + (texTrimmedRect.Height / size2Px),
                                     size1Px, size1Px);

    int width = m_gridSize.X;
    int height = m_gridSize.Y;
    Color color(0.12f, 0.12f, 0.55f, 0.33f);

    auto* pBatch = drawContext.pBatch;
    const auto gridStride = m_gridSize.X;
    // const auto areaSize = drawContext.AreaSize;

    for (int y = 1; y < height; ++y)
    {
      Vector2 previousPointX = m_coordinates2D[y * gridStride];
      for (int x = 1; x < width - 1; x++)
      {
        Vector2 left;
        Vector2 up;
        Vector2 currentPoint = m_coordinates2D[x + (y * gridStride)];
        if (x > 1)
        {
          // horizontal
          left = previousPointX;
          float thickness = (y % 3 == 1) ? 3.0f : 1.0f;

          int clampedX = static_cast<int>(std::min(x + 1, width - 1));
          Vector2 mid = VectorHelper::CatmullRom(m_coordinates2D[x - 2 + (y * gridStride)], left, currentPoint,
                                                 m_coordinates2D[clampedX + (y * gridStride)], 0.5f);

          DrawLine(pBatch, texFillNative, rectFillTex, left, mid, color, thickness);
          DrawLine(pBatch, texFillNative, rectFillTex, mid, currentPoint, color, thickness);
        }
        if (y > 1)
        {
          // vertical
          up = m_coordinates2D[x + ((y - 1) * gridStride)];
          float thickness = (x % 3 == 1) ? 3.0f : 1.0f;

          int clampedY = static_cast<int>(std::min(y + 1, height - 1));
          Vector2 mid = VectorHelper::CatmullRom(m_coordinates2D[x + ((y - 2) * gridStride)], up, currentPoint,
                                                 m_coordinates2D[x + (clampedY * gridStride)], 0.5f);

          DrawLine(pBatch, texFillNative, rectFillTex, up, mid, color, thickness);
          DrawLine(pBatch, texFillNative, rectFillTex, mid, currentPoint, color, thickness);
        }
        previousPointX = currentPoint;
      }
    }
  }


  void GridRenderNativeBatchCRSpline2::CalcFinalCoordinates(const std::vector<PointMass>& points, const Vector2& finalSize)
  {
    assert(m_coordinates2D.size() == points.size());
    const std::size_t count = points.size();
    const auto* pSrc = points.data();
    const auto* const pSrcEnd = pSrc + count;
    auto* pDst = m_coordinates2D.data();
    const Vector2 halfSize(finalSize * 0.5f);
    while (pSrc < pSrcEnd)
    {
      float factor = (pSrc->m_position.Z + 2000.0f) * 0.0005f;
      pDst->X = ((pSrc->m_position.X - halfSize.X) * factor) + halfSize.X;
      pDst->Y = ((pSrc->m_position.Y - halfSize.Y) * factor) + halfSize.Y;
      ++pSrc;
      ++pDst;
    }
  }
}
