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

#include "GridRenderVBLineStrip1.hpp"
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/VectorHelper.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/NativeTexture2D.hpp>
#include <cassert>

namespace Fsl
{
  using namespace GLES3;

  //! The CatmullRom spline smoothing is now done in 3D in a initial line generation pass
  //! This method produces a better looking grid and has the benefit of being slightly faster to render multiple times as the line generation code
  //! isn't run on each draw call.
  //! The line generation pass outputs one vertex line list, which is then transfered to a vertex buffer and the rest is send off to the GPU to
  //! render. This is a lot faster than rendering via native batch and we save the entire CPU 3d->2d pass.
  GridRenderVBLineStrip1::GridRenderVBLineStrip1(const Point2& gridSize, const Point2& screenSize,
                                                 const std::shared_ptr<IContentManager>& contentManager)
    : m_gridSize(gridSize)
    , m_gridFinalSize((2 * gridSize.X) - 1, (2 * gridSize.Y) - 1)
    , m_coordinates((m_gridFinalSize.X * m_gridSize.Y) + (m_gridFinalSize.Y * m_gridSize.X))
    , m_vertexBuffer(nullptr, m_coordinates.size(), VertexPosition::GetVertexDeclaration(), GL_STREAM_DRAW)
    , m_program(contentManager->ReadAllText("Shaders/LineShader.vert"), contentManager->ReadAllText("Shaders/LineShader.frag"))
    , m_locWorldViewProjection(GLValues::INVALID_LOCATION)
    , m_locAmbientColor(GLValues::INVALID_LOCATION)
  {
    if ((gridSize.X & 1) != 0 || (gridSize.Y & 1) != 0)
    {
      throw NotSupportedException("Even grid size required");
    }

    m_locWorldViewProjection = glGetUniformLocation(m_program.Get(), "WorldViewProjection");
    m_locAmbientColor = glGetUniformLocation(m_program.Get(), "AmbientColor");
    // if (m_locWorldViewProjection < 0 || m_locAmbientColor < 0)
    //  throw GraphicsException("Shader did not contain the expected uniforms");


    const auto screenWidth = static_cast<float>(screenSize.X);
    const auto screenHeight = static_cast<float>(screenSize.Y);
    const float aspectRatio = screenWidth / screenHeight;
    const float fov = 60.0f;

    // Calc distance that z=0 has to be away from the camera for the width and height to match the resolution
    float yMax = std::tan(fov * MathHelper::PI / 360.0f);
    float depth = screenHeight / 2.0f / yMax;

    Matrix world = Matrix::CreateRotationX(MathHelper::TO_RADS * 180);
    Matrix view = Matrix::CreateTranslation(-screenWidth * 0.5f, screenHeight * 0.5f, -depth);
    Matrix projection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(fov), aspectRatio, 1.0f, 2000.0f);

    m_worldViewProjection = world * view * projection;
  }


  const char* GridRenderVBLineStrip1::GetName() const
  {
    return "VB+line strips, Catmull-Rom spline";
  }


  void GridRenderVBLineStrip1::Update(const DemoTime& demoTime, const Vector2& areaSize, const std::vector<PointMass>& points)
  {
    Calc3DCoordinates(m_coordinates, points);
  }


  void GridRenderVBLineStrip1::Draw(const GridRenderDrawContext& drawContext, const std::vector<PointMass>& points)
  {
    Color color(0.12f, 0.12f, 0.55f, 0.33f);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glUseProgram(m_program.Get());
    if (m_locWorldViewProjection >= 0)
    {
      glUniformMatrix4fv(m_locWorldViewProjection, 1, 0u, m_worldViewProjection.DirectAccess());
    }
    if (m_locAmbientColor >= 0)
    {
      glUniform4fv(m_locAmbientColor, 1, color.ToVector4().DirectAccess());
    }

    auto& vb = m_vertexBuffer;
    glBindBuffer(vb.GetTarget(), vb.Get());
    vb.EnableAttribArrays();
    glDrawArrays(GL_LINE_STRIP, 0, vb.GetCapacity());
    vb.DisableAttribArrays();
  }


  void GridRenderVBLineStrip1::Calc3DCoordinates(std::vector<VertexPosition>& rDst, const std::vector<PointMass>& points)
  {
    int32_t pointsWritten = CreateLinesHorizontal(rDst, points, 0);
    CreateLinesVertical(rDst, points, pointsWritten);
    m_vertexBuffer.SetData(0, m_coordinates.data(), m_coordinates.size());
  }


  int32_t GridRenderVBLineStrip1::CreateLinesHorizontal(std::vector<VertexPosition>& rDst, const std::vector<PointMass>& points,
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
      const PointMass* const pSrcEnd = pSrcLeft + (srcStride * (srcGridMaxY & 0xFFFFFFFE));
      VertexPosition* pDstLeft = rDst.data() + dstOffset;
      VertexPosition* pDstRight = pDstLeft + dstGridMaxX - 2;
      while (pSrcLeft < pSrcEnd)
      {
        // lines from left to right
        assert(pDstLeft < pDstEnd);
        assert(pDstRight < pDstEnd);
        assert((pDstLeft + 2) < pDstEnd);
        assert((pDstRight + 1) < pDstEnd);
        pDstLeft[0].Position = pSrcLeft[0].m_position;
        pDstLeft[1].Position =
          VectorHelper::CatmullRom(pSrcLeft[0].m_position, pSrcLeft[0].m_position, pSrcLeft[1].m_position, pSrcLeft[2].m_position, 0.5f);
        pDstLeft[2].Position = pSrcLeft[1].m_position;

        pDstRight[0].Position =
          VectorHelper::CatmullRom(pSrcRight[0].m_position, pSrcRight[1].m_position, pSrcRight[2].m_position, pSrcRight[2].m_position, 0.5f);
        pDstRight[1].Position = pSrcRight[2].m_position;

        pSrcLeft += srcStride;
        pSrcRight += srcStride;
        pDstLeft += dstStride;
        pDstRight += dstStride;

        // flip the output so we do the lines from right to left
        assert(pSrcLeft < pSrcEnd);
        assert(pDstLeft < pDstEnd);
        assert(pDstRight < pDstEnd);
        assert((pDstLeft + 2) < pDstEnd);
        assert((pDstRight + 1) < pDstEnd);
        assert((pDstRight - 1) >= rDst.data());
        pDstRight[-1].Position = pSrcLeft[1].m_position;
        pDstRight[0].Position =
          VectorHelper::CatmullRom(pSrcLeft[0].m_position, pSrcLeft[0].m_position, pSrcLeft[1].m_position, pSrcLeft[2].m_position, 0.5f);
        pDstRight[1].Position = pSrcLeft[0].m_position;

        pDstLeft[0].Position = pSrcRight[2].m_position;
        pDstLeft[1].Position =
          VectorHelper::CatmullRom(pSrcRight[0].m_position, pSrcRight[1].m_position, pSrcRight[2].m_position, pSrcRight[2].m_position, 0.5f);

        pSrcLeft += srcStride;
        pSrcRight += srcStride;
        pDstLeft += dstStride;
        pDstRight += dstStride;
      }
    }

    {    // horizontal pass - transfer all existing coordinates and spawn the new horizontal coordinates
      const PointMass* pSrc = points.data();
      const PointMass* const pSrcEnd = pSrc + (srcStride * (srcGridMaxY & 0xFFFFFFFE));
      // +3 to skip the three points written in the border handling code
      VertexPosition* pDstLeft = rDst.data() + dstOffset + 3;
      VertexPosition* pDstRight = rDst.data() + dstOffset + dstGridMaxX - 4 + dstStride;
      const int constrainedGridMaxX = srcGridMaxX - 3;
      const auto dstStride2 = dstStride * 2;

      while (pSrc < pSrcEnd)
      {
        assert(pDstLeft < pDstEnd);
        // lines from left to right
        for (int x = 0; x < constrainedGridMaxX; ++x)
        {
          assert((pDstLeft + (x * 2)) < pDstEnd);
          assert((pDstLeft + (x * 2) + 1) < pDstEnd);
          pDstLeft[x * 2].Position =
            VectorHelper::CatmullRom(pSrc[x].m_position, pSrc[x + 1].m_position, pSrc[x + 2].m_position, pSrc[x + 3].m_position, 0.5f);
          pDstLeft[(x * 2) + 1].Position = pSrc[x + 2].m_position;
        }
        pSrc += srcStride;
        assert(pSrc < pSrcEnd);
        // lines from right to left
        for (int x = 0; x < constrainedGridMaxX; ++x)
        {
          assert((pDstRight - (x * 2)) >= rDst.data());
          assert((pDstRight - ((x * 2) + 1)) >= rDst.data());
          assert((pDstRight - (x * 2)) < pDstEnd);
          assert((pDstRight - ((x * 2) + 1)) < pDstEnd);

          pDstRight[-(x * 2)].Position =
            VectorHelper::CatmullRom(pSrc[x].m_position, pSrc[x + 1].m_position, pSrc[x + 2].m_position, pSrc[x + 3].m_position, 0.5f);
          pDstRight[-((x * 2) + 1)].Position = pSrc[x + 2].m_position;
        }
        pSrc += srcStride;
        pDstLeft += dstStride2;
        pDstRight += dstStride2;
      }
    }
    return (m_gridFinalSize.X * m_gridSize.Y);
  }

  int32_t GridRenderVBLineStrip1::CreateLinesVertical(std::vector<VertexPosition>& rDst, const std::vector<PointMass>& points,
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

      VertexPosition* pDstTop = rDst.data() + dstOffset;
      VertexPosition* pDstBottom = pDstTop + (dstGridMaxY - 2);
      for (int x = 0; x < srcGridMaxX; ++x)
      {
        assert(pDstTop < pDstEnd);
        assert((pDstTop + 2) < pDstEnd);
        assert((pDstBottom - 1) >= (rDst.data() + dstOffset));
        assert(pDstBottom < pDstEnd);
        assert(pDstBottom < (pDstTop + dstStride));
        assert((pDstBottom + 1) < (pDstTop + dstStride));

        // Bottom to top
        pDstBottom[1] = pSrcTop[x].m_position;
        pDstBottom[0] = VectorHelper::CatmullRom(pSrcTop[x].m_position, pSrcTop[x].m_position, pSrcTop[x + srcStride].m_position,
                                                 pSrcTop[x + (2 * srcStride)].m_position, 0.5f);
        pDstBottom[-1] = pSrcTop[x + srcStride].m_position;

        pDstTop[1] = VectorHelper::CatmullRom(pSrcBottom[x].m_position, pSrcBottom[x + srcStride].m_position,
                                              pSrcBottom[x + (srcStride * 2)].m_position, pSrcBottom[x + (srcStride * 2)].m_position, 0.5f);
        pDstTop[0] = pSrcBottom[x + (srcStride * 2)].m_position;

        pDstTop += dstStride;
        pDstBottom += dstStride;

        ++x;

        // Top to bottom
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
      VertexPosition* pDstLeftStart = rDst.data() + dstOffset + 3;
      VertexPosition* pDstRightStart = rDst.data() + dstOffset + dstGridMaxY - 5;

      for (int x = 0; x < srcGridMaxX; ++x)
      {
        // Write lines from bottom to top
        const PointMass* pSrc = pSrcStart;
        VertexPosition* pDst = pDstRightStart;
        while (pSrc < pSrcConstrainedEnd)
        {
          assert(pDst < pDstEnd);
          assert((pDst + 1) < pDstEnd);
          pDst[1].Position = VectorHelper::CatmullRom(pSrc[x].m_position, pSrc[x + srcStride].m_position, pSrc[x + (2 * srcStride)].m_position,
                                                      pSrc[x + (3 * srcStride)].m_position, 0.5f);
          pDst[0].Position = pSrc[x + (2 * srcStride)].m_position;

          pSrc += srcStride;
          pDst -= 2;
        }
        ++x;
        pDstLeftStart += dstStride;
        pDstRightStart += dstStride;
        // Write lines from top to bottom
        pSrc = pSrcStart;
        pDst = pDstLeftStart;
        while (pSrc < pSrcConstrainedEnd)
        {
          assert(pDst < pDstEnd);
          assert((pDst + 1) < pDstEnd);
          pDst[0].Position = VectorHelper::CatmullRom(pSrc[x].m_position, pSrc[x + srcStride].m_position, pSrc[x + (2 * srcStride)].m_position,
                                                      pSrc[x + (3 * srcStride)].m_position, 0.5f);
          pDst[1].Position = pSrc[x + (2 * srcStride)].m_position;

          pSrc += srcStride;
          pDst += 2;
        }
        pDstLeftStart += dstStride;
        pDstRightStart += dstStride;
      }
    }
    return (m_gridFinalSize.Y * m_gridSize.X);
  }
}
