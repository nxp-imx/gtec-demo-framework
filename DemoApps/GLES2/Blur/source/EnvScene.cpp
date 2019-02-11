/*
 * This code was created by Jeff Molofee '99
 * (ported to Linux by Ti Leggett '01)
 * (ported to i.mx51, i.mx31 and x11 by Freescale '10)
 * (ported to the Freescale demo framework by Freescale '14)
 * (improved port to the Freescale demo framework by Freescale '15)
 * If you've found this code useful, please let him know.
 *
 * Visit Jeff at http://nehe.gamedev.net/
 *
 */

#include "EnvScene.hpp"
#include <FslUtil/OpenGLES2/GLCheck.hpp>

#include <FslBase/Math/MathHelper.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <Shared/EnvironmentMapping/SphereMeshCreator.hpp>
#include <cassert>

namespace Fsl
{
  using namespace GLES2;

  namespace
  {
    void CreateCubemapTexture(GLTexture& rTexture, const std::shared_ptr<IContentManager>& contentManager, const std::string& cubeDirName)
    {
      std::vector<uint8_t> rawData;

      Bitmap posX, negX, posY, negY, posZ, negZ;

      contentManager->Read(posX, IO::Path::Combine(cubeDirName, "PosX.jpg"), PixelFormat::R8G8B8_UNORM);
      contentManager->Read(negX, IO::Path::Combine(cubeDirName, "NegX.jpg"), PixelFormat::R8G8B8_UNORM);
      contentManager->Read(negY, IO::Path::Combine(cubeDirName, "PosY.jpg"), PixelFormat::R8G8B8_UNORM);
      contentManager->Read(posY, IO::Path::Combine(cubeDirName, "NegY.jpg"), PixelFormat::R8G8B8_UNORM);
      contentManager->Read(posZ, IO::Path::Combine(cubeDirName, "PosZ.jpg"), PixelFormat::R8G8B8_UNORM);
      contentManager->Read(negZ, IO::Path::Combine(cubeDirName, "NegZ.jpg"), PixelFormat::R8G8B8_UNORM);

      GLTextureParameters params(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
      rTexture.SetData(posX, negX, posY, negY, posZ, negZ, params, TextureFlags::GenerateMipMaps);
    }


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


  EnvScene::EnvScene(const DemoAppConfig& config)
    : m_renderState(config.ScreenResolution)
  {
    const std::shared_ptr<IContentManager> content = config.DemoServiceProvider.Get<IContentManager>();

    std::string strPath;

    // strPath = "Brudslojan"; // Nice
    // strPath = "LarnacaCastle"; // Nice
    // strPath = "SanFrancisco3"; // nice
    strPath = "Textures/Cubemap/Stairs/Raw";
    // strPath = "SaintLazarusChurch2"; // Nice
    // strPath = "SaintLazarusChurch3"; // Nice

    CreateCubemapTexture(m_cubeTexture, content, strPath);

    // Prepare the shader programs
    const std::string vertexShader = content->ReadAllText("Shader.vert");
    m_programGlass.Reset(vertexShader, content->ReadAllText("ShaderGlass.frag"));
    m_programBgrnd.Reset(vertexShader, content->ReadAllText("ShaderBgrnd.frag"));

    {    // Create the sphere vertex & index buffer
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


  EnvScene::~EnvScene()
  {
    // cleanup
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }


  void EnvScene::Update(const DemoTime& demoTime)
  {
    m_renderState.Update(demoTime.DeltaTime);

    // m_renderState.ProjMatrix *= Matrix::CreateTranslation(Vector3(1, 0, 0));
    m_renderState.ExtractTransform(m_transformMatrix);
  }


  void EnvScene::Draw()
  {
    glViewport(0, 0, m_renderState.GetScreenResolution().X, m_renderState.GetScreenResolution().Y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // If enabled, cull polygons based on their winding in window coordinates
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.Get());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.Get());
    m_vertexBuffer.EnableAttribArrays();

    // Render mirror ball.
    {
      glCullFace(GL_FRONT);
      glUseProgram(m_programGlass.Get());
      glUniform1i(m_glass.LocSamplerCb, 0);
      glUniform1f(m_glass.LocRadius, 1.0f);
      glUniform3fv(m_glass.LocEyePos, 1, m_renderState.EyeVector.DirectAccess());
      glUniformMatrix4fv(m_glass.LocTransformMat, 1, GL_FALSE, m_transformMatrix.DirectAccess());
      glDrawElements(GL_TRIANGLES, m_indexBuffer.GetCapacity(), m_indexBuffer.GetType(), nullptr);
    }

    // Render background sphere.
    {
      glCullFace(GL_BACK);
      glUseProgram(m_programBgrnd.Get());
      glUniform1i(m_background.LocSamplerCb, 0);
      glUniform1f(m_background.LocRadius, 10.0f);
      // glUniform3fv(m_background.LocEyePos, 1, m_renderState.EyeVector.DirectAccess());
      glUniformMatrix4fv(m_background.LocTransformMat, 1, GL_FALSE, m_transformMatrix.DirectAccess());
      glDrawElements(GL_TRIANGLES, m_indexBuffer.GetCapacity(), m_indexBuffer.GetType(), nullptr);
    }
    m_vertexBuffer.DisableAttribArrays();
  }
}
