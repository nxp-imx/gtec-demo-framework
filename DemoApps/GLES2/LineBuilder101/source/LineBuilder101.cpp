/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include "LineBuilder101.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/FrameInfo.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <GLES2/gl2.h>

namespace Fsl
{
  using namespace GLES2;

  LineBuilder101::LineBuilder101(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_example(config.DemoServiceProvider)
  {
    const auto contentManager = GetContentManager();
    m_resources.LineProgram = CreateLineProgram(contentManager);
  }


  void LineBuilder101::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    m_example.OnMouseButtonEvent(event);
  }

  void LineBuilder101::Update(const DemoTime& demoTime)
  {
    m_example.Update(demoTime, GetWindowSizePx());

    m_vertexUboData.MatViewProjection = m_example.GetViewMatrix() * m_example.GetProjectionMatrix();
  }


  void LineBuilder101::Draw(const FrameInfo& frameInfo)
  {
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_resources.LineProgram.Program.Get());
    glUniformMatrix4fv(m_resources.LineProgram.LocWorldViewProjection, 1, 0, m_vertexUboData.MatViewProjection.DirectAccess());

    // Draw all the lines
    m_resources.LineDraw.Draw(m_example.GetLineBuilder().GetVertexSpan(), m_resources.LineProgram.Links);

    // Allow the example to draw a bit too
    m_example.Draw(frameInfo.Time);
  }

  LineBuilder101::ProgramInfo LineBuilder101::CreateLineProgram(const std::shared_ptr<IContentManager>& contentManager)
  {
    constexpr auto vertexDecl = Graphics3D::LineBuilder::vertex_type::GetVertexDeclarationArray();
    ProgramInfo info;
    info.Program.Reset(contentManager->ReadAllText("VertexColorLine.vert"), contentManager->ReadAllText("VertexColorLine.frag"));
    info.LocWorldViewProjection = info.Program.GetUniformLocation("WorldViewProjection");
    info.Links[0] =
      GLVertexAttribLink(info.Program.GetAttribLocation("VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u));
    info.Links[1] =
      GLVertexAttribLink(info.Program.GetAttribLocation("VertexColor"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0u));
    return info;
  }
}
