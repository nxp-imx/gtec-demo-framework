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

#include "S08_EnvironmentMappingRefraction.hpp"
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Base/Service/Graphics/IGraphicsService.hpp>
#include <Shared/EnvironmentMapping/SphereMeshCreator.hpp>
#include <cassert>

namespace Fsl
{
  using namespace GLES2;

  namespace
  {
    EnvShaderInfo ExtractShaderInfo(GLuint hProgram)
    {
      EnvShaderInfo shaderInfo;
      shaderInfo.LocTransformMat = GL_CHECK(glGetUniformLocation(hProgram, "my_TransformMatrix"));
      shaderInfo.LocRadius = GL_CHECK(glGetUniformLocation(hProgram, "my_Radius"));
      shaderInfo.LocEyePos = GL_CHECK(glGetUniformLocation(hProgram, "my_EyePos"));
      shaderInfo.LocSamplerCb = GL_CHECK(glGetUniformLocation(hProgram, "samplerCb"));
      return shaderInfo;
    }
  }

  S08_EnvironmentMappingRefraction::S08_EnvironmentMappingRefraction(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_renderState(config.ScreenResolution)
    , m_programGlass()
    , m_programBgrnd()
    , m_cubeTexture()
    , m_vertexBuffer()
    , m_indexBuffer()
    , m_transformMatrix()
  {
    const std::shared_ptr<IContentManager> content = config.DemoServiceProvider.Get<IContentManager>();

    {
      Texture tex;
      content->Read(tex, "cubemap.dds", PixelFormat::R8G8B8_UNORM);
      GLTextureParameters params(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
      m_cubeTexture.Reset(tex, params, TextureFlags::GenerateMipMaps);
    }

    // Prepare the shader programs
    const std::string vertexShader = content->ReadAllText("Shader.vert");
    m_programGlass.Reset(vertexShader, content->ReadAllText("ShaderGlass.frag"));
    m_programBgrnd.Reset(vertexShader, content->ReadAllText("ShaderBgrnd.frag"));

    { // Create the sphere vertex & index buffer
      std::vector<uint16_t> m_indices;
      std::vector<VertexPosition> sphereVertices;
      SphereMeshCreator::Create(sphereVertices, m_indices, 40, 60);

      m_vertexBuffer.Reset(sphereVertices, GL_STATIC_DRAW);
      m_indexBuffer.Reset(m_indices.data(), m_indices.size(), GL_STATIC_DRAW);
    }

    // Get uniform locations
    // Beware that m_background.LocEyePos is unused by the background shader so its -1
    m_glass = ExtractShaderInfo(m_programGlass.Get());
    m_background = ExtractShaderInfo(m_programBgrnd.Get());

    // Set eye position.
    m_renderState.Eye = Vector3(0, 0, -3.8f);
  }


  S08_EnvironmentMappingRefraction::~S08_EnvironmentMappingRefraction()
  {
    // cleanup
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }


  void S08_EnvironmentMappingRefraction::Update(const DemoTime& demoTime)
  {
    m_renderState.Update(demoTime.DeltaTime);
    m_renderState.ExtractTransform(m_transformMatrix);
  }


  void S08_EnvironmentMappingRefraction::Draw(const DemoTime& demoTime)
  {
    glEnable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.Get());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.Get());
    m_vertexBuffer.EnableAttribArrays();

    // Render background sphere.
    glCullFace(GL_BACK);
    glUseProgram(m_programBgrnd.Get());
    glUniform1i(m_background.LocSamplerCb, 0);
    glUniform1f(m_background.LocRadius, 10.0f);
    // glUniform3fv(m_background.LocEyePos, 1, m_renderState.EyeVector.DirectAccess());
    glUniformMatrix4fv(m_background.LocTransformMat, 1, GL_FALSE, m_transformMatrix.DirectAccess());
    glDrawElements(GL_TRIANGLES, m_indexBuffer.GetCapacity(), m_indexBuffer.GetType(), 0);

    // Render glass ball.
    glCullFace(GL_FRONT);
    glUseProgram(m_programGlass.Get());
    glUniform1i(m_glass.LocSamplerCb, 0);
    glUniform1f(m_glass.LocRadius, 1.0f);
    glUniform3fv(m_glass.LocEyePos, 1, m_renderState.EyeVector.DirectAccess());
    glUniformMatrix4fv(m_glass.LocTransformMat, 1, GL_FALSE, m_transformMatrix.DirectAccess());
    glDrawElements(GL_TRIANGLES, m_indexBuffer.GetCapacity(), m_indexBuffer.GetType(), 0);

    m_vertexBuffer.DisableAttribArrays();
  }

}
