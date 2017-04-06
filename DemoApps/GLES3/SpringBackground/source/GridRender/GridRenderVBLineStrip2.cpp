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

#include "GridRenderVBLineStrip2.hpp"
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/VectorHelper.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslGraphicsGLES3/GLCheck.hpp>
#include <FslGraphicsGLES3/NativeTexture2D.hpp>
#include <cassert>

namespace Fsl
{
  using namespace GLES3;

  //! The CatmullRom spline smoothing is now done in 3D in a initial line generation pass
  //! This method produces a better looking grid and has the benefit of being slightly faster to render multiple times as the line generation code
  //! isn't run on each draw call.
  //! The line generation pass outputs one vertex line list, which is then transfered to a vertex buffer and the rest is send off to the GPU to render.
  //! This is a lot faster than rendering via native batch and we save the entire CPU 3d->2d pass.
  GridRenderVBLineStrip2::GridRenderVBLineStrip2(const Point2& gridSize, const Point2& screenSize, const std::shared_ptr<IContentManager>& contentManager)
    : m_gridSize(gridSize)
    , m_gridFinalSize((2 * gridSize.X) - 1, (2 * gridSize.Y) - 1)
    , m_coordinates(((m_gridFinalSize.X + 2) * m_gridSize.Y) + ((m_gridFinalSize.Y + 2) * m_gridSize.X))
    , m_vertexBuffer(nullptr, m_coordinates.size(), VertexPositionColor::GetVertexDeclaration(), GL_STREAM_DRAW)
    , m_program(contentManager->ReadAllText("Shaders/LineShaderColor.vert"), contentManager->ReadAllText("Shaders/LineShaderColor.frag"))
    , m_locWorldViewProjection(GLValues::INVALID_LOCATION)
  {
    if ((gridSize.X & 1) != 0 || (gridSize.Y & 1) != 0)
      throw NotSupportedException("Even grid size required");

    m_locWorldViewProjection = glGetUniformLocation(m_program.Get(), "WorldViewProjection");
    //if (m_locWorldViewProjection < 0 || m_locAmbientColor < 0)
    //  throw GraphicsException("Shader did not contain the expected uniforms");


    const float screenWidth = static_cast<float>(screenSize.X);
    const float screenHeight = static_cast<float>(screenSize.Y);
    const float aspectRatio = screenWidth / screenHeight;
    const float fov = 60.0f;

    // Calc distance that z=0 has to be away from the camera for the width and height to match the resolution
    float yMax = std::tan(fov * MathHelper::PI / 360.0f);
    float depth = screenHeight / 2.0f / yMax;

    Matrix world = Matrix::CreateRotationX(MathHelper::TO_RADS * 180);
    Matrix view = Matrix::CreateTranslation(-screenWidth * 0.5f, screenHeight * 0.5f, -depth);
    Matrix projection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(fov), aspectRatio, 1.0f, 2000.0f);

    m_worldViewProjection = world * view * projection;

    {
      Vector4 color(0.12f, 0.12f, 0.55f, 0.33f);
      VertexPositionColor defaultVertex(Vector3(), color);
      for (std::size_t i = 0; i < m_coordinates.size(); ++i)
        m_coordinates[i] = defaultVertex;

      const std::ptrdiff_t dstStrideX = m_gridFinalSize.X + 2;
      {
        VertexPositionColor* pDst = m_coordinates.data();
        const VertexPositionColor*const pDstEnd = pDst + (dstStrideX * m_gridSize.Y);
        assert(pDstEnd <= (m_coordinates.data() + m_coordinates.size()));
        while (pDst < pDstEnd)
        {
          pDst[0].Color = Vector4();
          pDst[dstStrideX - 1].Color = Vector4();
          pDst += dstStrideX;
        }
      }
      {
        const std::ptrdiff_t dstStrideY = m_gridFinalSize.Y + 2;
        VertexPositionColor* pDst = m_coordinates.data() + (dstStrideX * m_gridSize.Y);
        const VertexPositionColor*const pDstEnd = m_coordinates.data() + m_coordinates.size();
        while (pDst < pDstEnd)
        {
          pDst[0].Color = Vector4();
          pDst[dstStrideY - 1].Color = Vector4();
          pDst += dstStrideY;
        }
      }
    }
  }


  const char* GridRenderVBLineStrip2::GetName() const
  {
    return "VB+line strips 2, Catmull-Rom spline";
  }


  void GridRenderVBLineStrip2::Update(const DemoTime& demoTime, const Vector2& areaSize, const std::vector<PointMass>& points)
  {
    Calc3DCoordinates(m_coordinates, points);
  }


  void GridRenderVBLineStrip2::Draw(const GridRenderDrawContext& drawContext, const std::vector<PointMass>& points)
  {
    Color color(0.12f, 0.12f, 0.55f, 0.33f);

    //glLineWidth(4);
    //glLineWidth(3);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glUseProgram(m_program.Get());
    if (m_locWorldViewProjection >= 0)
      glUniformMatrix4fv(m_locWorldViewProjection, 1, false, m_worldViewProjection.DirectAccess());

    auto& vb = m_vertexBuffer;
    glBindBuffer(vb.GetTarget(), vb.Get());
    vb.EnableAttribArrays();
    glDrawArrays(GL_LINE_STRIP, 0, vb.GetCapacity());
    vb.DisableAttribArrays();
  }


  void GridRenderVBLineStrip2::Calc3DCoordinates(std::vector<VertexPositionColor>& rDst, const std::vector<PointMass>& points)
  {
    int32_t pointsWritten = 0;
    pointsWritten += CreateLinesHorizontal(rDst, points, pointsWritten);
    pointsWritten += CreateLinesVertical(rDst, points, pointsWritten);
    m_vertexBuffer.SetData(0, m_coordinates.data(), m_coordinates.size());
  }


  int32_t GridRenderVBLineStrip2::CreateLinesHorizontal(std::vector<VertexPositionColor>& rDst, const std::vector<PointMass>& points, const std::ptrdiff_t dstOffset)
  {
    const int srcGridMaxX = m_gridSize.X;
    const int srcGridMaxY = m_gridSize.Y;
    const int dstGridMaxX = m_gridFinalSize.X;
    //const int dstGridMaxY = m_gridFinalSize.Y;
    const std::ptrdiff_t srcStride = srcGridMaxX;
    const std::ptrdiff_t dstStride = dstGridMaxX + 2;

#ifndef NDEBUG
    const auto*const pDstEnd = rDst.data() + rDst.size();
#endif

    // Create the horizontal border points as they are a special case
    {
      const PointMass* pSrcLeft = points.data();
      const PointMass* pSrcRight = pSrcLeft + srcGridMaxX - 3;
      const PointMass*const pSrcEnd = pSrcLeft + (srcStride * srcGridMaxY);
      VertexPositionColor* pDstLeft = rDst.data() + dstOffset;
      VertexPositionColor* pDstRight = pDstLeft + dstGridMaxX - 3 + 1;
      while (pSrcLeft < pSrcEnd)
      {
        // lines from left to right
        assert(pSrcLeft < pSrcEnd);
        assert(pSrcRight < pSrcEnd);
        assert((pSrcLeft + 2) < pSrcEnd);
        assert((pSrcRight + 2) < pSrcEnd);
        assert(pDstLeft < pDstEnd);
        assert(pDstRight < pDstEnd);
        assert((pDstLeft + 3) < pDstEnd);
        assert((pDstRight + 3) < pDstEnd);
        pDstLeft[0].Position = pSrcLeft[0].m_position;
        pDstLeft[1].Position = pSrcLeft[0].m_position;
        pDstLeft[2].Position = VectorHelper::CatmullRom(pSrcLeft[0].m_position, pSrcLeft[0].m_position, pSrcLeft[1].m_position, pSrcLeft[2].m_position, 0.5f);
        pDstLeft[3].Position = pSrcLeft[1].m_position;

        pDstRight[1].Position = VectorHelper::CatmullRom(pSrcRight[0].m_position, pSrcRight[1].m_position, pSrcRight[2].m_position, pSrcRight[2].m_position, 0.5f);
        pDstRight[2].Position = pSrcRight[2].m_position;
        pDstRight[3].Position = pSrcRight[2].m_position;


        pSrcLeft += srcStride;
        pSrcRight += srcStride;
        pDstLeft += dstStride;
        pDstRight += dstStride;

        // flip the output so we do the lines from right to left
        assert(pSrcLeft < pSrcEnd);
        assert(pSrcRight < pSrcEnd);
        assert((pSrcLeft + 2) < pSrcEnd);
        assert((pSrcRight + 2) < pSrcEnd);
        assert(pDstLeft < pDstEnd);
        assert(pDstRight < pDstEnd);
        assert((pDstLeft + 2) < pDstEnd);
        assert((pDstRight + 3) < pDstEnd);
        pDstRight[0].Position = pSrcLeft[1].m_position;
        pDstRight[1].Position = VectorHelper::CatmullRom(pSrcLeft[0].m_position, pSrcLeft[0].m_position, pSrcLeft[1].m_position, pSrcLeft[2].m_position, 0.5f);
        pDstRight[2].Position = pSrcLeft[0].m_position;
        pDstRight[3].Position = pSrcLeft[0].m_position;

        pDstLeft[0].Position = pSrcRight[2].m_position;
        pDstLeft[1].Position = pSrcRight[2].m_position;
        pDstLeft[2].Position = VectorHelper::CatmullRom(pSrcRight[0].m_position, pSrcRight[1].m_position, pSrcRight[2].m_position, pSrcRight[2].m_position, 0.5f);

        pSrcLeft += srcStride;
        pSrcRight += srcStride;
        pDstLeft += dstStride;
        pDstRight += dstStride;
      }
    }

    { // horizontal pass - transfer all existing coordinates and spawn the new horizontal coordinates
      const PointMass* pSrc = points.data();
      const PointMass*const pSrcEnd = pSrc + (srcStride * srcGridMaxY);
      // +3 to skip the three points written in the border handling code
      VertexPositionColor* pDstLeft = rDst.data() + dstOffset + 3 + 1;
      VertexPositionColor* pDstRight = rDst.data() + dstOffset + dstGridMaxX - 4 + 1 + dstStride;
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
          pDstLeft[x * 2].Position = VectorHelper::CatmullRom(pSrc[x].m_position, pSrc[x + 1].m_position, pSrc[x + 2].m_position, pSrc[x + 3].m_position, 0.5f);
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

          pDstRight[-(x * 2)].Position = VectorHelper::CatmullRom(pSrc[x].m_position, pSrc[x + 1].m_position, pSrc[x + 2].m_position, pSrc[x + 3].m_position, 0.5f);
          pDstRight[-((x * 2) + 1)].Position = pSrc[x + 2].m_position;
        }
        pSrc += srcStride;
        pDstLeft += dstStride2;
        pDstRight += dstStride2;
      }
    }
    return ((m_gridFinalSize.X + 2) * m_gridSize.Y);

  }

  int32_t GridRenderVBLineStrip2::CreateLinesVertical(std::vector<VertexPositionColor>& rDst, const std::vector<PointMass>& points, const std::ptrdiff_t dstOffset)
  {
    const int srcGridMaxX = m_gridSize.X;
    const int srcGridMaxY = m_gridSize.Y;
    //const int dstGridMaxX = m_gridFinalSize.X;
    const int dstGridMaxY = m_gridFinalSize.Y;
    const std::ptrdiff_t srcStride = srcGridMaxX;
    const std::ptrdiff_t dstStride = dstGridMaxY + 2;

#ifndef NDEBUG
    const auto*const pDstEnd = rDst.data() + rDst.size();
#endif

    // Create the vertical border points as they are a special case
    {
      const PointMass* pSrcTop = points.data();
      const PointMass* pSrcBottom = pSrcTop + (srcStride * (srcGridMaxY - 3));

      VertexPositionColor* pDstTop = rDst.data() + dstOffset;
      VertexPositionColor* pDstBottom = pDstTop + (dstGridMaxY - 3 + 1);
      for (int x = 0; x < srcGridMaxX; ++x)
      {
        assert(pDstTop < pDstEnd);
        assert((pDstTop + 1) < pDstEnd);
        assert((pDstTop + dstStride) <= pDstEnd);
        assert(pDstBottom < (pDstTop + dstStride));
        assert((pDstBottom + 3) < (pDstTop + dstStride));

        // Bottom to top
        pDstBottom[3].Position = pSrcTop[x].m_position;
        pDstBottom[2].Position = pSrcTop[x].m_position;
        pDstBottom[1].Position = VectorHelper::CatmullRom(pSrcTop[x].m_position, pSrcTop[x].m_position, pSrcTop[x + srcStride].m_position, pSrcTop[x + (2 * srcStride)].m_position, 0.5f);
        pDstBottom[0].Position = pSrcTop[x + srcStride].m_position;

        pDstTop[2].Position = VectorHelper::CatmullRom(pSrcBottom[x].m_position, pSrcBottom[x + srcStride].m_position, pSrcBottom[x + (srcStride * 2)].m_position, pSrcBottom[x + (srcStride * 2)].m_position, 0.5f);
        pDstTop[1].Position = pSrcBottom[x + (srcStride * 2)].m_position;
        pDstTop[0].Position = pSrcBottom[x + (srcStride * 2)].m_position;

        pDstTop += dstStride;
        pDstBottom += dstStride;

        ++x;

        // Top to bottom
        assert(pDstTop < pDstEnd);
        assert((pDstTop + 2) < pDstEnd);
        assert((pDstTop + dstStride) <= pDstEnd);
        assert(pDstBottom < (pDstTop + dstStride));
        assert((pDstBottom + 3) < (pDstTop + dstStride));

        pDstTop[0].Position = pSrcTop[x].m_position;
        pDstTop[1].Position = pSrcTop[x].m_position;
        pDstTop[2].Position = VectorHelper::CatmullRom(pSrcTop[x].m_position, pSrcTop[x].m_position, pSrcTop[x + srcStride].m_position, pSrcTop[x + (2 * srcStride)].m_position, 0.5f);
        pDstTop[3].Position = pSrcTop[x + srcStride].m_position;

        pDstBottom[1].Position = VectorHelper::CatmullRom(pSrcBottom[x].m_position, pSrcBottom[x + srcStride].m_position, pSrcBottom[x + (srcStride * 2)].m_position, pSrcBottom[x + (srcStride * 2)].m_position, 0.5f);
        pDstBottom[2].Position = pSrcBottom[x + (srcStride * 2)].m_position;
        pDstBottom[3].Position = pSrcBottom[x + (srcStride * 2)].m_position;

        pDstTop += dstStride;
        pDstBottom += dstStride;
      }
    }

    { // vertical pass - transfer all existing coordinates and spawn the new vertical coordinates
      // +3 to skip the three points written in the border handling code
      const PointMass*const pSrcStart = points.data();
      const PointMass*const pSrcConstrainedEnd = pSrcStart + (srcStride * (srcGridMaxY - 3));
      VertexPositionColor* pDstLeftStart = rDst.data() + dstOffset + 3 + 1;
      VertexPositionColor* pDstRightStart = rDst.data() + dstOffset + dstGridMaxY - 5 + 1;

      for (int x = 0; x < srcGridMaxX; ++x)
      {
        // Write lines from bottom to top
        const PointMass* pSrc = pSrcStart;
        VertexPositionColor* pDst = pDstRightStart;
        while (pSrc < pSrcConstrainedEnd)
        {
          assert(pDst < pDstEnd);
          assert((pDst+1) < pDstEnd);
          pDst[1].Position = VectorHelper::CatmullRom(pSrc[x].m_position, pSrc[x + srcStride].m_position, pSrc[x + (2 * srcStride)].m_position, pSrc[x + (3 * srcStride)].m_position, 0.5f);
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
          pDst[0].Position = VectorHelper::CatmullRom(pSrc[x].m_position, pSrc[x + srcStride].m_position, pSrc[x + (2 * srcStride)].m_position, pSrc[x + (3 * srcStride)].m_position, 0.5f);
          pDst[1].Position = pSrc[x + (2 * srcStride)].m_position;

          pSrc += srcStride;
          pDst += 2;
        }
        pDstLeftStart += dstStride;
        pDstRightStart += dstStride;
      }
    }
    return ((m_gridFinalSize.Y + 2) * m_gridSize.X);
  }
}
