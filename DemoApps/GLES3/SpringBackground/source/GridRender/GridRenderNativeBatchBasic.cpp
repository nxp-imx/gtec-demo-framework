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

#include "GridRenderNativeBatchBasic.hpp"
#include <FslBase/Math/VectorHelper.hpp>
#include <FslUtil/OpenGLES3/DynamicNativeTexture2D.hpp>
#include <cassert>

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    inline Vector2 ToVec2(const Vector3& v, const Vector2& screenSize)
    {
      float factor = (v.Z + 2000.0f) * 0.0005f;
      return (Vector2(v.X, v.Y) - screenSize * 0.5f) * factor + screenSize * 0.5f;
    }


    inline void DrawLine(NativeBatch2D* pBatch, const GLBatch2D::texture_type& texFill, const PxRectangleU& nativeTexRect, const Vector2& start,
                         const Vector2& end, const Color& color, const float thickness)
    {
      Vector2 delta = end - start;

      pBatch->Draw(texFill, start, nativeTexRect, color, VectorHelper::VectorToAngle(delta), Vector2(0, 0), Vector2(delta.Length(), thickness));
    }
  }


  GridRenderNativeBatchBasic::GridRenderNativeBatchBasic(const Point2& gridSize)
    : m_gridSize(gridSize)
  {
  }


  const char* GridRenderNativeBatchBasic::GetName() const
  {
    return "NativeBatch Basic";
  }


  void GridRenderNativeBatchBasic::Update(const DemoTime& /*demoTime*/, const Vector2& /*areaSize*/, const std::vector<PointMass>& /*points*/)
  {
  }


  void GridRenderNativeBatchBasic::Draw(const GridRenderDrawContext& drawContext, const std::vector<PointMass>& points)
  {
    auto nativeTex = std::dynamic_pointer_cast<DynamicNativeTexture2D>(drawContext.TexFill.TryGetNative());
    assert(nativeTex);
    GLBatch2D::texture_type texFillNative(nativeTex->Get(), drawContext.TexFill.GetAtlasSize());
    const auto texTrimmedRect = drawContext.TexFill.GetInfo().TrimmedRectPx;
    const PxRectangleU rectFillTex(texTrimmedRect.X + (texTrimmedRect.Width / 2), texTrimmedRect.Y + (texTrimmedRect.Height / 2), 1, 1);


    const int32_t width = m_gridSize.X;
    const int32_t height = m_gridSize.Y;
    Color color(0.12f, 0.12f, 0.55f, 0.33f);

    auto* pBatch = drawContext.pBatch;
    const auto* const pPoints = points.data();
    const auto gridStride = m_gridSize.X;
    const auto areaSize = drawContext.AreaSize;

    // Changed the basic implementation to do add some smoothing based on CatmullRom splines.
    for (int y = 1; y < height; ++y)
    {
      Vector2 previousPointX = ToVec2(pPoints[(y * gridStride)].m_position, areaSize);
      for (int x = 1; x < width - 1; x++)
      {
        Vector2 left;
        Vector2 up;
        Vector2 currentPoint = ToVec2(pPoints[x + (y * gridStride)].m_position, areaSize);
        if (x > 1)
        {
          // horizontal
          left = previousPointX;
          float thickness = (y % 3 == 1) ? 3.0f : 1.0f;
          DrawLine(pBatch, texFillNative, rectFillTex, left, currentPoint, color, thickness);
        }
        if (y > 1)
        {
          // vertical
          up = ToVec2(pPoints[x + ((y - 1) * gridStride)].m_position, areaSize);
          float thickness = (x % 3 == 1) ? 3.0f : 1.0f;
          DrawLine(pBatch, texFillNative, rectFillTex, up, currentPoint, color, thickness);
        }
        previousPointX = currentPoint;
      }
    }
  }
}
