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

#include "Grid.hpp"
#include <FslBase/Math/VectorHelper.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>
#include <cassert>

namespace Fsl
{
  // The grid simulation was inspired by Geometry Wars and this tutorial
  // http://gamedevelopment.tutsplus.com/tutorials/make-a-neon-vector-shooter-for-ios-the-warping-grid--gamedev-14637
  //
  Grid::Grid(const Rect& rect, const Point2& gridResolution)
    : m_gridX(gridResolution.X + (gridResolution.X & 1))
    , m_gridY(gridResolution.Y + (gridResolution.Y & 1))
    , m_points(m_gridX * m_gridY)
    , m_fixedPoints(m_gridX * m_gridY)    // TODO: reduce the amount of fixed points we need to allocate
  {
    assert((m_gridX & 1) == 0);
    assert((m_gridY & 1) == 0);

    const Vector2 spacing(rect.Width() / (m_gridX - 1), rect.Height() / (m_gridY - 1));
    InitPoints(rect, spacing);
    InitSprings();
  }


  void Grid::FixedUpdate()
  {
    for (size_t i = 0; i < m_springs.size(); i++)
    {
      m_springs[i].FixedUpdate();
    }

    for (int i = 0; i < m_gridX * m_gridY; i++)
    {
      m_points[i].FixedUpdate();
    }
  }


  void Grid::Update(const DemoTime& demoTime, const Vector2& areaSize, IGridRender& render)
  {
    render.Update(demoTime, areaSize, m_points);
  }

  void Grid::Draw(const GridRenderDrawContext& drawContext, IGridRender& render)
  {
    render.Draw(drawContext, m_points);
  }


  void Grid::SetPointMass(PointMass* pPoints, int x, int y, const PointMass& pointMass)
  {
    assert(x >= 0 && x <= m_gridX);
    assert(y >= 0 && y <= m_gridY);
    pPoints[x + (y * m_gridX)] = pointMass;
  }


  PointMass* Grid::GetPointMass(PointMass* pPoints, int x, int y)
  {
    assert(x >= 0 && x <= m_gridX);
    assert(y >= 0 && y <= m_gridY);
    return &pPoints[x + (y * m_gridX)];
  }


  void Grid::ApplyDirectedForce(const Vector3& force, const Vector3& position, const float radius)
  {
    // TODO: optimize this. It seems fairly easy as we know that all the points are on a ordered 2d grid.
    //       but its complicated by the fact that the points can be pushed around quite a bit
    const int count = m_gridX * m_gridY;
    const float squaredRadius = radius * radius;

    for (int i = 0; i < count; ++i)
    {
      const float distanceSquared = Vector3::DistanceSquared(position, m_points[i].m_position);
      if (distanceSquared < squaredRadius)
      {
        m_points[i].ApplyForce(10.0f * force / (10 + std::sqrt(distanceSquared)));
      }
    }
  }

  void Grid::ApplyImplosiveForce(const float force, const Vector3& position, const float radius)
  {
    // TODO: optimize this. It seems fairly easy as we know that all the points are on a ordered 2d grid.
    //       but its complicated by the fact that the points can be pushed around quite a bit
    const int count = m_gridX * m_gridY;
    const float squaredRadius = radius * radius;

    for (int i = 0; i < count; ++i)
    {
      float dist2 = Vector3::DistanceSquared(position, m_points[i].m_position);
      if (dist2 < squaredRadius)
      {
        m_points[i].ApplyForce(10.0f * force * (position - m_points[i].m_position) / (100 + dist2));
        m_points[i].IncreaseDamping(0.6f);
      }
    }
  }

  void Grid::ApplyExplosiveForce(const float force, const Vector3& position, const float radius)
  {
    // TODO: optimize this. It seems fairly easy as we know that all the points are on a ordered 2d grid.
    //       but its complicated by the fact that the points can be pushed around quite a bit
    const int count = m_gridX * m_gridY;
    const float squaredRadius = radius * radius;

    for (int i = 0; i < count; ++i)
    {
      float dist2 = Vector3::DistanceSquared(position, m_points[i].m_position);
      if (dist2 < squaredRadius)
      {
        m_points[i].ApplyForce(100 * force * (m_points[i].m_position - position) / (10000 + dist2));
        m_points[i].IncreaseDamping(0.6f);
      }
    }
  }

  void Grid::InitPoints(const Rect& rect, const Vector2& spacing)
  {
    const float xPosStart = rect.TopLeft().X;
    float yPos = rect.TopLeft().Y;
    for (int y = 0; y < m_gridY; ++y)
    {
      float xPos = xPosStart;
      for (int x = 0; x < m_gridX; ++x)
      {
        SetPointMass(m_points.data(), x, y, PointMass(Vector3(xPos, yPos, 0), 1));
        SetPointMass(m_fixedPoints.data(), x, y, PointMass(Vector3(xPos, yPos, 0), 0));
        xPos += spacing.X;
      }
      yPos += spacing.Y;
    }
  }

  void Grid::InitSprings()
  {
    // TODO: we can limit the required amount of allocated 'stabilizer points by just allocating the ones we need and then picking
    //       them one be one from the pool since there is no dependency on their grid position

    // link the point masses with springs
    const int32_t endX = m_gridX - 1;
    const int32_t endY = m_gridY - 1;
    {    // Setup the fairly rigid border
      const float stiffness = 0.1f;
      const float damping = 0.1f;
      for (int32_t y = 0; y < m_gridY; ++y)
      {
        m_springs.emplace_back(GetPointMass(m_fixedPoints.data(), 0, y), GetPointMass(m_points.data(), 0, y), stiffness, damping);
        m_springs.emplace_back(GetPointMass(m_fixedPoints.data(), endX, y), GetPointMass(m_points.data(), endX, y), stiffness, damping);
      }
      for (int32_t x = 0; x < m_gridX; ++x)
      {
        m_springs.emplace_back(GetPointMass(m_fixedPoints.data(), x, 0), GetPointMass(m_points.data(), x, 0), stiffness, damping);
        m_springs.emplace_back(GetPointMass(m_fixedPoints.data(), x, endY), GetPointMass(m_points.data(), x, endY), stiffness, damping);
      }
    }
    {    // Add stabilizer springs at regular intervals
      for (int y = 3; y < endY; y += 3)
      {
        for (int x = 3; x < endX; x += 3)
        {
          m_springs.emplace_back(GetPointMass(m_fixedPoints.data(), x, y), GetPointMass(m_points.data(), x, y), 0.002f, 0.02f);
        }
      }
    }
    {    // Add the main springs
      for (int y = 0; y < m_gridY; ++y)
      {
        for (int x = 0; x < m_gridX; ++x)
        {
          if (x > 0)
          {
            m_springs.emplace_back(GetPointMass(m_points.data(), x - 1, y), GetPointMass(m_points.data(), x, y), 0.28f, 0.06f);
          }

          if (y > 0)
          {
            m_springs.emplace_back(GetPointMass(m_points.data(), x, y - 1), GetPointMass(m_points.data(), x, y), 0.28f, 0.06f);
          }
        }
      }
    }
  }
}
