/*
 * This code was created by Jeff Molofee '99
 * (ported to Linux by Ti Leggett '01)
 * (ported to i.mx51, i.mx31 and x11 by Freescale '10)
 * (ported to the Freescale demo framework by Freescale '14)
 * If you've found this code useful, please let him know.
 *
 * Visit Jeff at http://nehe.gamedev.net/
 *
 */

#include "S07_EnvironmentMapping.hpp"
#include <FslBase/Math/MathHelper.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
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


  S07_EnvironmentMapping::S07_EnvironmentMapping(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_renderState(config.WindowMetrics.GetSizePx())
    , m_fast(false)
  {
    const std::shared_ptr<IContentManager> content = config.DemoServiceProvider.Get<IContentManager>();

    {
      Texture tex;
      content->Read(tex, "Textures/Cubemap/Yokohama3/Yokohama3_512.dds", PixelFormat::R8G8B8_UNORM);
      GLTextureParameters params(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
      m_cubeTexture.Reset(tex, params);
    }

    // Prepare the shader programs
    const std::string vertexShader = content->ReadAllText("Shader.vert");
    m_programGlass.Reset(vertexShader, content->ReadAllText("ShaderGlass.frag"));
    m_programBgrnd.Reset(vertexShader, content->ReadAllText("ShaderBgrnd.frag"));

    {    // Create the sphere vertex & index buffer
      std::vector<uint16_t> indices;
      std::vector<VertexPosition> sphereVertices;
      SphereMeshCreator::Create(sphereVertices, indices, 40, 60);

      m_vertexBuffer.Reset(sphereVertices, GL_STATIC_DRAW);
      m_indexBuffer.Reset(indices.data(), indices.size(), GL_STATIC_DRAW);
    }

    // Get uniform locations
    // Beware that m_background.LocEyePos is unused by the background shader so its -1
    m_glass = ExtractShaderInfo(m_programGlass.Get());
    m_background = ExtractShaderInfo(m_programBgrnd.Get());

    // Set eye position.
    m_renderState.Eye = Vector3(0, 0, -3.8f);

    if (m_fast)
    {
      // Don't do depth comparisons nor update the depth buffer
      glDisable(GL_DEPTH_TEST);
      glDepthFunc(GL_ALWAYS);
      glDepthMask(GL_FALSE);
    }
    else
    {
      glEnable(GL_DEPTH_TEST);
    }
  }

  S07_EnvironmentMapping::~S07_EnvironmentMapping()
  {
    // cleanup
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }


  void S07_EnvironmentMapping::Update(const DemoTime& demoTime)
  {
    m_renderState.Update(demoTime.DeltaTime);
    m_renderState.ExtractTransform(m_transformMatrix);
  }


  void S07_EnvironmentMapping::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    // If enabled, cull polygons based on their winding in window coordinates
    glEnable(GL_CULL_FACE);

    if (!m_fast)
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.Get());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.Get());
    m_vertexBuffer.EnableAttribArrays();

    if (m_fast)
    {
      // Render background sphere.
      glCullFace(GL_BACK);
      glUseProgram(m_programBgrnd.Get());
      glUniform1i(m_background.LocSamplerCb, 0);
      glUniform1f(m_background.LocRadius, 10.0f);
      // glUniform3fv(m_background.LocEyePos, 1, m_renderState.EyeVector.DirectAccess());
      glUniformMatrix4fv(m_background.LocTransformMat, 1, GL_FALSE, m_transformMatrix.DirectAccess());
      glDrawElements(GL_TRIANGLES, m_indexBuffer.GetGLCapacity(), m_indexBuffer.GetType(), nullptr);
    }

    // Render mirror ball.
    glCullFace(GL_FRONT);
    glUseProgram(m_programGlass.Get());
    glUniform1i(m_glass.LocSamplerCb, 0);
    glUniform1f(m_glass.LocRadius, 1.0f);
    glUniform3fv(m_glass.LocEyePos, 1, m_renderState.EyeVector.DirectAccess());
    glUniformMatrix4fv(m_glass.LocTransformMat, 1, GL_FALSE, m_transformMatrix.DirectAccess());
    glDrawElements(GL_TRIANGLES, m_indexBuffer.GetGLCapacity(), m_indexBuffer.GetType(), nullptr);

    if (!m_fast)
    {
      // Render background sphere.
      glCullFace(GL_BACK);
      glUseProgram(m_programBgrnd.Get());
      glUniform1i(m_background.LocSamplerCb, 0);
      glUniform1f(m_background.LocRadius, 10.0f);
      // glUniform3fv(m_background.LocEyePos, 1, m_renderState.EyeVector.DirectAccess());
      glUniformMatrix4fv(m_background.LocTransformMat, 1, GL_FALSE, m_transformMatrix.DirectAccess());
      glDrawElements(GL_TRIANGLES, m_indexBuffer.GetGLCapacity(), m_indexBuffer.GetType(), nullptr);
    }
    m_vertexBuffer.DisableAttribArrays();
  }
}
