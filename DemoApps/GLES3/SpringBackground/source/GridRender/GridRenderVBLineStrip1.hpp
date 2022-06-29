#ifndef GRIDRENDER_GLES3_SPRINGBACKGROUND_GRIDRENDERVBLINESTRIP1_HPP
#define GRIDRENDER_GLES3_SPRINGBACKGROUND_GRIDRENDERVBLINESTRIP1_HPP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <cstddef>
#include "../IGridRender.hpp"

namespace Fsl
{
  class GridRenderVBLineStrip1 : public IGridRender
  {
    Point2 m_gridSize;
    Point2 m_gridFinalSize;
    std::vector<VertexPosition> m_coordinates;
    GLES3::GLVertexBuffer m_vertexBuffer;
    GLES3::GLProgram m_program;
    GLint m_locWorldViewProjection;
    GLint m_locAmbientColor;
    Matrix m_worldViewProjection;

  public:
    GridRenderVBLineStrip1(const Point2& gridSize, const Point2& screenSize, const std::shared_ptr<IContentManager>& contentManager);

    const char* GetName() const override;
    void Update(const DemoTime& demoTime, const Vector2& areaSize, const std::vector<PointMass>& points) override;
    void Draw(const GridRenderDrawContext& drawContext, const std::vector<PointMass>& points) override;

  private:
    void Calc3DCoordinates(std::vector<VertexPosition>& rDst, const std::vector<PointMass>& points);
    int32_t CreateLinesHorizontal(std::vector<VertexPosition>& rDst, const std::vector<PointMass>& points, const std::ptrdiff_t dstOffset);
    int32_t CreateLinesVertical(std::vector<VertexPosition>& rDst, const std::vector<PointMass>& points, const std::ptrdiff_t dstOffset);
  };
}


#endif
