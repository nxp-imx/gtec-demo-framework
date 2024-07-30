/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include "SpatialHashGrid2D.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/FrameInfo.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr uint32_t InitialLineCapacity = 4096u;
    }
  }


  SpatialHashGrid2D::SpatialHashGrid2D(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_shared(config)
    , m_resources(LocalConfig::InitialLineCapacity)
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_shared.GetUIDemoAppExtension());

    const auto contentManager = GetContentManager();
    m_resources.LineProgram = CreateLineProgram(contentManager);
  }


  void SpatialHashGrid2D::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_shared.ConfigurationChanged(windowMetrics);
  }


  void SpatialHashGrid2D::Update(const DemoTime& demoTime)
  {
    m_shared.Update(demoTime);

    m_vertexUboData.MatViewProjection = m_shared.GetProjectionMatrix();
  }


  void SpatialHashGrid2D::Draw(const FrameInfo& frameInfo)
  {
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_resources.LineProgram.Program.Get());
    glUniformMatrix4fv(m_resources.LineProgram.LocWorldViewProjection, 1, 0, m_vertexUboData.MatViewProjection.DirectAccess());

    // Draw all the lines
    m_resources.LineDraw.Draw(m_shared.GetLineBuilder().GetVertexSpan(), m_resources.LineProgram.Links);
    m_shared.Draw(frameInfo.Time);
  }

  SpatialHashGrid2D::ProgramInfo SpatialHashGrid2D::CreateLineProgram(const std::shared_ptr<IContentManager>& contentManager)
  {
    constexpr auto VertexDecl = Graphics3D::LineBuilder::vertex_type::GetVertexDeclarationArray();
    ProgramInfo info;
    info.Program.Reset(contentManager->ReadAllText("VertexColorLine.vert"), contentManager->ReadAllText("VertexColorLine.frag"));
    info.LocWorldViewProjection = info.Program.GetUniformLocation("WorldViewProjection");
    info.Links[0] = GLES3::GLVertexAttribLink(info.Program.GetAttribLocation("VertexPosition"),
                                              VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u));
    info.Links[1] =
      GLES3::GLVertexAttribLink(info.Program.GetAttribLocation("VertexColor"), VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0u));
    return info;
  }
}
