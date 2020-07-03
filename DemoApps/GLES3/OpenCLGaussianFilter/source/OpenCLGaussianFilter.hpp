#ifndef GLES3_OPENCLGAUSSIANFILTER_OPENCLGAUSSIANFILTER_HPP
#define GLES3_OPENCLGAUSSIANFILTER_OPENCLGAUSSIANFILTER_HPP
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
#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <FslUtil/OpenGLES3/GLVertexAttribLink.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>

namespace Fsl
{
  class OpenCLGaussianFilter : public DemoAppGLES3
  {
    struct VertexUBOData
    {
      Matrix MatModel;
      Matrix MatProj;
    };

    struct Resources
    {
      GLES3::GLProgram Program;
      GLES3::GLTexture Texture;
      GLES3::GLVertexBuffer VertexBuffer;
      GLint ModelViewMatrixLoc = GLES3::GLValues::INVALID_LOCATION;
      GLint ProjMatrixLoc = GLES3::GLValues::INVALID_LOCATION;
    };

    Resources m_resources;
    VertexUBOData m_vertexUboData;

    Vector3 m_angle;
    Matrix m_matTranslate;
    std::array<GLES3::GLVertexAttribLink, 2> m_attribLink;

  public:
    explicit OpenCLGaussianFilter(const DemoAppConfig& config);
    ~OpenCLGaussianFilter() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;

  private:
    void PrepareMatrices(const Point2& currentSize);
  };
}

#endif
