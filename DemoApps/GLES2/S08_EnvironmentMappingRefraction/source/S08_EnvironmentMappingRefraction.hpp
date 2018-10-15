#ifndef GLES2_S08_ENVIRONMENTMAPPINGREFRACTION_S08_ENVIRONMENTMAPPINGREFRACTION_HPP
#define GLES2_S08_ENVIRONMENTMAPPINGREFRACTION_S08_ENVIRONMENTMAPPINGREFRACTION_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslDemoApp/OpenGLES2/DemoAppGLES2.hpp>
#include <FslUtil/OpenGLES2/GLProgram.hpp>
#include <FslUtil/OpenGLES2/GLTexture.hpp>
#include <FslUtil/OpenGLES2/GLVertexBuffer.hpp>
#include <FslUtil/OpenGLES2/GLIndexBuffer.hpp>

#include <Shared/EnvironmentMapping/EnvState.hpp>
#include <Shared/EnvironmentMapping/EnvShaderInfo.hpp>
#include <vector>

namespace Fsl
{
  class S08_EnvironmentMappingRefraction : public DemoAppGLES2
  {
    EnvState m_renderState;
    GLES2::GLProgram m_programGlass;
    GLES2::GLProgram m_programBgrnd;
    GLES2::GLTexture m_cubeTexture;
    GLES2::GLVertexBuffer m_vertexBuffer;
    GLES2::GLIndexBuffer m_indexBuffer;

    EnvShaderInfo m_glass;
    EnvShaderInfo m_background;

    Matrix m_transformMatrix;

  public:
    S08_EnvironmentMappingRefraction(const DemoAppConfig& config);
    ~S08_EnvironmentMappingRefraction() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;
  };
}

#endif
