#ifndef GLES3_SPRINGBACKGROUND_GRID_HPP
#define GLES3_SPRINGBACKGROUND_GRID_HPP
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

#include <FslBase/Math/Rect.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <vector>
#include "IGridRender.hpp"
#include "PointMass.hpp"
#include "Spring.hpp"

namespace Fsl
{
  class Grid
  {
    // NOLINTNEXTLINE(readability-identifier-naming)
    const int m_gridX;
    // NOLINTNEXTLINE(readability-identifier-naming)
    const int m_gridY;
    std::vector<Spring> m_springs;
    std::vector<PointMass> m_points;
    std::vector<PointMass> m_fixedPoints;

  public:
    //! @param gridResolution beware we will always use a even grid size, so if you supply a odd one it will become one bigger
    Grid(const Rect& rect, const Point2& gridResolution);

    Point2 GetGridSize() const
    {
      return {m_gridX, m_gridY};
    }

    void FixedUpdate();
    void Update(const DemoTime& demoTime, const Vector2& areaSize, IGridRender& render);
    void Draw(const GridRenderDrawContext& drawContext, IGridRender& render);

    void ApplyDirectedForce(const Vector3& force, const Vector3& position, const float radius);
    void ApplyImplosiveForce(const float force, const Vector3& position, const float radius);
    void ApplyExplosiveForce(const float force, const Vector3& position, const float radius);

  private:
    inline void SetPointMass(PointMass* pPoints, int x, int y, const PointMass& pointMass);
    inline PointMass* GetPointMass(PointMass* pPoints, int x, int y);

    void InitPoints(const Rect& rect, const Vector2& spacing);
    void InitSprings();
  };
}


#endif
