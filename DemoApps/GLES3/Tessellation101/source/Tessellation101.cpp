/****************************************************************************************************************************************************
* Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLUtil.hpp>
#include "Tessellation101.hpp"
// Because of inconsistency in khronos extension definition both the 31 and 2 headers are needed
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>

namespace Fsl
{
  using namespace GLES3;


  Tessellation101::Tessellation101(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_vertexBuffer()
    , m_pipeline()
    , m_shaderVert()
    , m_shaderControl()
    , m_shaderEval()
    , m_shaderFrag()
    , m_shaderGeom()
    , m_locWorldViewProjection(-1)
    , m_locTessLevelInner(-1)
    , m_locTessLevelOuter(-1)
    , m_showWireframe(true)
    , m_tessLevelInner(2.0)
    , m_tessLevelOuter(2.0)
  {
    if (!GLUtil::HasExtension("EXT_tessellation_shader"))
      throw NotSupportedException("EXT_tessellation_shader extension not supported");
    if (!GLUtil::HasExtension("EXT_geometry_shader"))
      throw NotSupportedException("EXT_geometry_shader extension not supported");

    const std::shared_ptr<IContentManager> contentManager = config.DemoServiceProvider.Get<IContentManager>();

    const std::string strControl = contentManager->ReadAllText("Tesselation.tesc");
    const std::string strEvaluation = contentManager->ReadAllText("Tesselation.tese");
    const std::string strFrag = contentManager->ReadAllText("Tesselation.frag");
    const std::string strVert = contentManager->ReadAllText("Tesselation.vert");
    const std::string strGeom = contentManager->ReadAllText("Tesselation.geom");

    //GLShader shaderVert(GL_VERTEX_SHADER, strVert);
    //GLShader shaderControl(GL_TESS_CONTROL_SHADER_EXT, strControl);
    //GLShader shaderEval(GL_TESS_EVALUATION_SHADER_EXT, strEvaluation);
    //GLShader shaderFrag(GL_FRAGMENT_SHADER, strFrag);
    //GLShader shaderGeom(GL_GEOMETRY_SHADER_EXT, strGeom);

    m_shaderVert.Reset(GL_VERTEX_SHADER, strVert);
    m_shaderControl.Reset(GL_TESS_CONTROL_SHADER_EXT, strControl);
    m_shaderEval.Reset(GL_TESS_EVALUATION_SHADER_EXT, strEvaluation);
    m_shaderGeom.Reset(GL_GEOMETRY_SHADER_EXT, strGeom);
    m_shaderFrag.Reset(GL_FRAGMENT_SHADER, strFrag);

    m_locTessLevelInner = glGetUniformLocation(m_shaderControl.Get(), "TessLevelInner");
    m_locTessLevelOuter = glGetUniformLocation(m_shaderControl.Get(), "TessLevelOuter");

    m_locWorldViewProjection = glGetUniformLocation(m_shaderEval.Get(), "WorldViewProjection");
    if (m_locTessLevelInner < 0 || m_locTessLevelOuter < 0 || m_locWorldViewProjection < 0)
      throw GraphicsException("Unable to locate uniform");

    // Set default values
    glProgramUniform1f(m_shaderControl.Get(), m_locTessLevelInner, m_tessLevelInner);
    glProgramUniform1f(m_shaderControl.Get(), m_locTessLevelOuter, m_tessLevelOuter);

    glUseProgram(0);
    m_pipeline.Reset(true);

    { // prepare the vertex buffer
      //const Point2 screenResolution = config.ScreenResolution;

      float magicX = 100;
      float magicY = 100;

      VertexPosition vertices[] =
      {
        VertexPosition(Vector3(-magicX, magicY, 0.0f)),
        VertexPosition(Vector3(-magicX, -magicY, 0.0f)),
        VertexPosition(Vector3(magicX, magicY, 0.0f)),

        VertexPosition(Vector3(magicX + 10, magicY, 0.0f)),
        VertexPosition(Vector3(-magicX + 10, -magicY, 0.0f)),
        VertexPosition(Vector3(magicX + 10, -magicY, 0.0f)),
      };

      m_vertexBuffer.Reset(vertices, 6, GL_STATIC_DRAW);
    }
  }


  Tessellation101::~Tessellation101()
  {

  }


  void Tessellation101::OnKeyEvent(const KeyEvent& event)
  {
    DemoAppGLES3::OnKeyEvent(event);

    if (!event.IsPressed())
      return;

    switch (event.GetKey())
    {
    case VirtualKey::W:
      event.Handled();
      m_showWireframe = !m_showWireframe;
      break;
    case VirtualKey::Code1:
      event.Handled();
      m_tessLevelInner -= 1.0f;
      if (m_tessLevelInner < 1)
        m_tessLevelInner = 1.0f;
      glProgramUniform1f(m_shaderControl.Get(), m_locTessLevelInner, m_tessLevelInner);
      break;
    case VirtualKey::Code2:
      event.Handled();
      m_tessLevelInner += 1.0f;
      glProgramUniform1f(m_shaderControl.Get(), m_locTessLevelInner, m_tessLevelInner);
      break;
    case VirtualKey::Code3:
      event.Handled();
      m_tessLevelOuter -= 1.0f;
      if (m_tessLevelOuter < 1)
        m_tessLevelOuter = 1.0f;
      glProgramUniform1f(m_shaderControl.Get(), m_locTessLevelOuter, m_tessLevelOuter);
      break;
    case VirtualKey::Code4:
      event.Handled();
      m_tessLevelOuter += 1.0f;
      glProgramUniform1f(m_shaderControl.Get(), m_locTessLevelOuter, m_tessLevelOuter);
      break;
    case VirtualKey::O:
      m_showWireframe = !m_showWireframe;
      break;
    default:
      break;
    }
  }


  void Tessellation101::Update(const DemoTime& demoTime)
  {
    const Point2 screenResolution = GetScreenResolution();

    const Matrix view = Matrix::CreateTranslation(0.0f, 0.0f, -350.0f);
    const Matrix projection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), screenResolution.X / (float)screenResolution.Y, 1, 1000.0f);
    const Matrix viewProjection = view * projection;

    glProgramUniformMatrix4fv(m_shaderEval.Get(), m_locWorldViewProjection, 1, GL_FALSE, viewProjection.DirectAccess());
  }


  void Tessellation101::Draw(const DemoTime& demoTime)
  {
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Do the actual tessellated draw
    {
      // Disable any active program
      glUseProgram(0);

      // Bind the pipeline
      m_pipeline.Bind();

      // configure it
      m_pipeline.UseProgramStages(m_shaderVert, GL_VERTEX_SHADER_BIT);
      m_pipeline.UseProgramStages(m_shaderControl, GL_TESS_CONTROL_SHADER_BIT_EXT);
      m_pipeline.UseProgramStages(m_shaderEval, GL_TESS_EVALUATION_SHADER_BIT_EXT);
      m_pipeline.UseProgramStages(m_shaderGeom, GL_GEOMETRY_SHADER_BIT_EXT, m_showWireframe);
      m_pipeline.UseProgramStages(m_shaderFrag, GL_FRAGMENT_SHADER_BIT);

      // Bind and enable the vertex buffer
      glBindBuffer(m_vertexBuffer.GetTarget(), m_vertexBuffer.Get());
      m_vertexBuffer.EnableAttribArrays();

      // Draw the patch
      glDrawArrays(GL_PATCHES_EXT, 0, 6);

      // Disable everything
      m_vertexBuffer.DisableAttribArrays();
      glBindBuffer(m_vertexBuffer.GetTarget(), 0);
      m_pipeline.BindClear();
    }
  }

}
