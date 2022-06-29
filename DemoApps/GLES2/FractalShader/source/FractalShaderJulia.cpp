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

#include "FractalShaderJulia.hpp"
#include <FslBase/String/StringUtil.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <Shared/FractalShader/OptionParser.hpp>
#include <Shared/FractalShader/ShaderUtil.hpp>
#include <GLES2/gl2.h>
#include <array>
#include <iostream>
#include <string>

namespace Fsl
{
  using namespace GLES2;

  FractalShaderJulia::FractalShaderJulia(const DemoAppConfig& config)
    : m_screenResolution(config.ScreenResolution)
    , m_config(config.GetOptions<OptionParser>()->GetConfig())
    , m_locCoordinate(GLValues::INVALID_LOCATION)
    , m_helper(m_config, m_config.TheAnimationMode, config.DemoServiceProvider)
  {
    const std::shared_ptr<IContentManager> contentManager = config.DemoServiceProvider.Get<IContentManager>();
    m_program.Reset(contentManager->ReadAllText("Shader.vert"), ShaderUtil::GetFragmentShader(m_config, contentManager, 2));

    const GLuint hProgram = m_program.Get();
    glUseProgram(hProgram);

    m_locCoordinate = glGetUniformLocation(hProgram, "Coordinate");

    const float aspect = (static_cast<float>(m_screenResolution.Y) / static_cast<float>(m_screenResolution.X));
    const float scaleX = 2.5f;
    const float scaleY = scaleX * aspect;
    const float u1 = (-1.0f) * scaleX;
    const float u2 = (1.0f) * scaleX;
    const float v1 = (-1.0f) * scaleY;
    const float v2 = (1.0f) * scaleY;

    const std::array<VertexPositionTexture, 4> vertices = {
      VertexPositionTexture(Vector3(-1.0f, 1.0f, 0.0f), Vector2(u1, v2)),
      VertexPositionTexture(Vector3(-1.0f, -1.0f, 0.0f), Vector2(u1, v1)),
      VertexPositionTexture(Vector3(1.0f, 1.0f, 0.0f), Vector2(u2, v2)),
      VertexPositionTexture(Vector3(1.0f, -1.0f, 0.0f), Vector2(u2, v1)),
    };

    m_vertexBuffer.Reset(vertices, GL_STATIC_DRAW);
    glViewport(0, 0, m_screenResolution.X, m_screenResolution.Y);
  }


  FractalShaderJulia::~FractalShaderJulia() = default;


  void FractalShaderJulia::Update(const DemoTime& demoTime)
  {
    m_helper.Update(demoTime);
    const Vector2 location = m_helper.GetLocation();

    const GLuint hProgram = m_program.Get();
    glUseProgram(hProgram);
    glUniform2f(m_locCoordinate, location.X, location.Y);
  }


  void FractalShaderJulia::Draw()
  {
    const GLuint hProgram = m_program.Get();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(hProgram);

    glClear(GL_COLOR_BUFFER_BIT);

    glBindBuffer(m_vertexBuffer.GetTarget(), m_vertexBuffer.Get());
    m_vertexBuffer.EnableAttribArrays();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_vertexBuffer.DisableAttribArrays();

    glBindBuffer(m_vertexBuffer.GetTarget(), 0);


    m_helper.Draw(m_screenResolution);
  }
}
