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

#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
#include <FslUtil/OpenGLES3/GLUtil.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/NativeBatch2D.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Base/Control/CheckBox.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/SliderAndValueLabel.hpp>
#include <FslSimpleUI/Base/Control/ValueLabel.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include "ParticleSystemBasicScene.hpp"
#include "PS/Draw/ParticleDrawPointsGLES3.hpp"
#include "PS/Draw/ParticleDrawQuadsGLES3.hpp"
#include "PS/Draw/ParticleDrawGeometryShaderGLES3.hpp"
#include "PS/Emit/BoxEmitter.hpp"
#include "PS/ParticleSystemOneArray.hpp"
#include "PS/ParticleSystemTwoArrays.hpp"
#include "PSGpu/ParticleSystemGLES3.hpp"
#include "PSGpu/ParticleSystemSnow.hpp"
#include <GLES3/gl3.h>
#include <iostream>

namespace Fsl
{
  using namespace GLES3;
  using namespace UI;

  namespace
  {
    const uint32_t PARTICLE_CAPACITY = 4;

    const float DEFAULT_ZOOM = 32;


    void BuildCube(GLVertexBuffer& rDst, const Vector3 dimensions)
    {
      // 0-3  0   0-3
      // |\|  |\   \|
      // 1-2  1-2   2
      const float x = dimensions.X * 0.5f;
      const float y = dimensions.Y * 0.5f;
      const float z = dimensions.Z * 0.5f;
      VertexPositionColorTexture vertices[6 * 6] = {
        // Front
        VertexPositionColorTexture(Vector3(-x, +y, +z), Color::White(), Vector2(0, 1)),
        VertexPositionColorTexture(Vector3(-x, -y, +z), Color::White(), Vector2(0, 0)),
        VertexPositionColorTexture(Vector3(+x, -y, +z), Color::White(), Vector2(1, 0)),
        VertexPositionColorTexture(Vector3(-x, +y, +z), Color::White(), Vector2(0, 1)),
        VertexPositionColorTexture(Vector3(+x, -y, +z), Color::White(), Vector2(1, 0)),
        VertexPositionColorTexture(Vector3(+x, +y, +z), Color::White(), Vector2(1, 1)),

        // Back
        VertexPositionColorTexture(Vector3(-x, +y, -z), Color::White(), Vector2(1, 1)),
        VertexPositionColorTexture(Vector3(+x, -y, -z), Color::White(), Vector2(0, 0)),
        VertexPositionColorTexture(Vector3(-x, -y, -z), Color::White(), Vector2(1, 0)),
        VertexPositionColorTexture(Vector3(-x, +y, -z), Color::White(), Vector2(1, 1)),
        VertexPositionColorTexture(Vector3(+x, +y, -z), Color::White(), Vector2(0, 1)),
        VertexPositionColorTexture(Vector3(+x, -y, -z), Color::White(), Vector2(0, 0)),

        // Right
        VertexPositionColorTexture(Vector3(+x, +y, +z), Color::White(), Vector2(0, 1)),
        VertexPositionColorTexture(Vector3(+x, -y, +z), Color::White(), Vector2(0, 0)),
        VertexPositionColorTexture(Vector3(+x, -y, -z), Color::White(), Vector2(1, 0)),
        VertexPositionColorTexture(Vector3(+x, +y, +z), Color::White(), Vector2(0, 1)),
        VertexPositionColorTexture(Vector3(+x, -y, -z), Color::White(), Vector2(1, 0)),
        VertexPositionColorTexture(Vector3(+x, +y, -z), Color::White(), Vector2(1, 1)),

        // Left
        VertexPositionColorTexture(Vector3(-x, +y, -z), Color::White(), Vector2(0, 1)),
        VertexPositionColorTexture(Vector3(-x, -y, -z), Color::White(), Vector2(0, 0)),
        VertexPositionColorTexture(Vector3(-x, -y, +z), Color::White(), Vector2(1, 0)),
        VertexPositionColorTexture(Vector3(-x, +y, -z), Color::White(), Vector2(0, 1)),
        VertexPositionColorTexture(Vector3(-x, -y, +z), Color::White(), Vector2(1, 0)),
        VertexPositionColorTexture(Vector3(-x, +y, +z), Color::White(), Vector2(1, 1)),

        // Top
        VertexPositionColorTexture(Vector3(-x, +y, -z), Color::White(), Vector2(0, 1)),
        VertexPositionColorTexture(Vector3(-x, +y, +z), Color::White(), Vector2(0, 0)),
        VertexPositionColorTexture(Vector3(+x, +y, +z), Color::White(), Vector2(1, 0)),
        VertexPositionColorTexture(Vector3(-x, +y, -z), Color::White(), Vector2(0, 1)),
        VertexPositionColorTexture(Vector3(+x, +y, +z), Color::White(), Vector2(1, 0)),
        VertexPositionColorTexture(Vector3(+x, +y, -z), Color::White(), Vector2(1, 1)),

        // Bottom
        VertexPositionColorTexture(Vector3(-x, -y, +z), Color::White(), Vector2(0, 1)),
        VertexPositionColorTexture(Vector3(-x, -y, -z), Color::White(), Vector2(0, 0)),
        VertexPositionColorTexture(Vector3(+x, -y, -z), Color::White(), Vector2(1, 0)),
        VertexPositionColorTexture(Vector3(-x, -y, +z), Color::White(), Vector2(0, 1)),
        VertexPositionColorTexture(Vector3(+x, -y, -z), Color::White(), Vector2(1, 0)),
        VertexPositionColorTexture(Vector3(+x, -y, +z), Color::White(), Vector2(1, 1)),
      };

      rDst.Reset(vertices, 6 * 6, GL_STATIC_DRAW);
    }
  }


  ParticleSystemBasicScene::ParticleSystemBasicScene(const DemoAppConfig& config, const std::shared_ptr<UIDemoAppExtension>& uiExtension)
    : AScene(config)
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_allowAdvancedTechniques(false)
    , m_camera(config.ScreenResolution)
    , m_locWorldViewMatrix(GLValues::INVALID_LOCATION)
    , m_locProjMatrix(GLValues::INVALID_LOCATION)
    , m_rotationSpeed(0.0f, 0.5f, 0.0f)
    , m_rotate(false)
    , m_particleSystemType(ParticleSystemType::GeometryShader)
  {
    m_camera.SetZoom(DEFAULT_ZOOM);

    auto contentManager = GetContentManager();

    m_allowAdvancedTechniques = GLUtil::HasExtension("GL_EXT_geometry_shader");
    if (m_allowAdvancedTechniques)
    {
      m_particleSystemGpu2 = std::make_shared<ParticleSystemSnow>(PARTICLE_CAPACITY, contentManager, Vector3(5, 5, 5), 0.8f);
    }
    SetParticleSystem(m_particleSystemType, true);


    BuildCube(m_vbCube, Vector3(1, 1, 1));


    {    // Load the textures
      GLTextureParameters textureParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      Bitmap bitmap;
      contentManager->Read(bitmap, "Particle.png", PixelFormat::R8G8B8A8_UNORM);
      m_texParticle.Reset(bitmap, textureParams, TextureFlags::GenerateMipMaps);
      contentManager->Read(bitmap, "ParticleSnow.png", PixelFormat::R8G8B8_UNORM);
      m_texParticleSnow.Reset(bitmap, textureParams, TextureFlags::GenerateMipMaps);
      contentManager->Read(bitmap, "GTEC.png", PixelFormat::R8G8B8_UNORM);
      m_texCube.Reset(bitmap, textureParams, TextureFlags::GenerateMipMaps);
    }

    {    // Prepare the default program
      m_program.Reset(contentManager->ReadAllText("Shader.vert"), contentManager->ReadAllText("Shader.frag"));
      const GLuint hProgram = m_program.Get();
      auto vertexDecl = VertexPositionColorTexture::GetVertexDeclaration();
      m_cubeAttribLink[0] =
        GLVertexAttribLink(glGetAttribLocation(hProgram, "VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
      m_cubeAttribLink[1] =
        GLVertexAttribLink(glGetAttribLocation(hProgram, "VertexColor"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0));
      m_cubeAttribLink[2] = GLVertexAttribLink(glGetAttribLocation(hProgram, "VertexTexCoord"),
                                               vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));

      m_locWorldViewMatrix = glGetUniformLocation(hProgram, "WorldView");
      m_locProjMatrix = glGetUniformLocation(hProgram, "Projection");
    }
  }


  ParticleSystemBasicScene::~ParticleSystemBasicScene() = default;


  void ParticleSystemBasicScene::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    switch (event.GetButton())
    {
    case VirtualMouseButton::Left:
    {
      if (event.IsPressed())
      {
        m_camera.BeginDrag(event.GetPosition());
      }
      else if (m_camera.IsDragging())
      {
        m_camera.EndDrag(event.GetPosition());
      }
      event.Handled();
    }
    break;
    case VirtualMouseButton::Right:
      if (event.IsPressed())
      {
        m_camera.ResetRotation();
        m_camera.SetZoom(DEFAULT_ZOOM);
        m_rotation = Vector3();
        event.Handled();
      }
      break;
    default:
      AScene::OnMouseButtonEvent(event);
      break;
    }
  }


  void ParticleSystemBasicScene::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    if (m_camera.IsDragging())
    {
      m_camera.Drag(event.GetPosition());
      event.Handled();
    }
  }


  void ParticleSystemBasicScene::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    m_camera.AddZoom(event.GetDelta() * -0.001f);
  }


  void ParticleSystemBasicScene::Update(const DemoTime& demoTime)
  {
    if (m_particleSystem)
    {
      m_particleSystem->Update(demoTime);
    }

    if (m_particleSystemGpu)
    {
      m_particleSystemGpu->Update(demoTime);
    }

    if (m_particleSystemGpu2)
    {
      m_particleSystemGpu2->Update(demoTime);
    }

    const Point2 screenResolution = GetScreenResolution();

    if (m_rotate)
    {
      m_rotation += m_rotationSpeed * demoTime.DeltaTime;
    }

    m_particleDrawContext.MatrixWorld =
      Matrix::CreateRotationX(m_rotation.X) * Matrix::CreateRotationY(m_rotation.Y) * Matrix::CreateRotationZ(m_rotation.Z);
    m_particleDrawContext.MatrixView = m_camera.GetViewMatrix();
    m_particleDrawContext.MatrixProjection =
      Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), screenResolution.X / static_cast<float>(screenResolution.Y), 1, 1000.0f);
    m_particleDrawContext.MatrixWorldView = m_particleDrawContext.MatrixWorld * m_particleDrawContext.MatrixView;
    m_particleDrawContext.MatrixWorldViewProjection = m_particleDrawContext.MatrixWorldView * m_particleDrawContext.MatrixProjection;
    m_particleDrawContext.ScreenAspectRatio = GetScreenResolution().X / static_cast<float>(GetScreenResolution().Y);
  }


  void ParticleSystemBasicScene::Draw()
  {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    // glDisable(GL_CULL_FACE);

    glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    DrawCube();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    // glBlendFunc(GL_ONE, GL_ONE);
    DrawParticles();

    glDisable(GL_DEPTH_TEST);


    if (m_particleSystemGpu)
    {
      m_particleSystemGpu->Draw(m_particleDrawContext);
    }

    if (m_particleSystemGpu2)
    {
      // glBlendFunc(GL_ONE, GL_ONE);
      // glActiveTexture(GL_TEXTURE0);
      // glBindTexture(GL_TEXTURE_2D, m_texParticleSnow.Get());

      glBlendFunc(GL_ONE, GL_ONE);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_texParticleSnow.Get());
      m_particleSystemGpu2->Draw(m_particleDrawContext);
    }
  }


  void ParticleSystemBasicScene::DrawCube()
  {
    const GLuint hProgram = m_program.Get();

    // Set the shader program
    glUseProgram(hProgram);

    // Load the matrices
    glUniformMatrix4fv(m_locWorldViewMatrix, 1, 0, m_particleDrawContext.MatrixWorldView.DirectAccess());
    glUniformMatrix4fv(m_locProjMatrix, 1, 0, m_particleDrawContext.MatrixProjection.DirectAccess());

    // Select Our Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texCube.Get());

    // bind the vertex buffer and enable the attrib array
    glBindBuffer(m_vbCube.GetTarget(), m_vbCube.Get());
    m_vbCube.EnableAttribArrays(m_cubeAttribLink, sizeof(m_cubeAttribLink) / sizeof(GLES3::GLVertexAttribLink));
    glDrawArrays(GL_TRIANGLES, 0, m_vbCube.GetCapacity());
  }


  void ParticleSystemBasicScene::DrawParticles()
  {
    // Select Our Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texParticle.Get());
    // glBindTexture(GL_TEXTURE_2D, m_texCube.Get());

    if (m_particleSystem)
    {
      glDepthMask(GL_FALSE);
      m_particleSystem->Draw(m_particleDrawContext);
      glDepthMask(GL_TRUE);
    }
  }


  void ParticleSystemBasicScene::SetParticleSystem(const ParticleSystemType type, const bool force)
  {
    if (type == m_particleSystemType && !force)
    {
      return;
    }

    m_particleSystemType = type;
    m_particleSystem.reset();

    std::shared_ptr<IParticleDraw> particleDraw;

    auto typeEx = type;
    if (typeEx == ParticleSystemType::GeometryShader && !m_allowAdvancedTechniques)
    {
      typeEx = ParticleSystemType::Instancing;
    }

    switch (typeEx)
    {
    case ParticleSystemType::Points:
      particleDraw = std::make_shared<ParticleDrawPointsGLES3>(GetContentManager(), PARTICLE_CAPACITY, ParticleSystemOneArray::ParticleRecordSize());
      break;
    case ParticleSystemType::GeometryShader:
      particleDraw =
        std::make_shared<ParticleDrawGeometryShaderGLES3>(GetContentManager(), PARTICLE_CAPACITY, ParticleSystemOneArray::ParticleRecordSize());
      break;
    case ParticleSystemType::Instancing:
    default:
      particleDraw = std::make_shared<ParticleDrawQuadsGLES3>(GetContentManager(), PARTICLE_CAPACITY);
      break;
    }
    if (particleDraw)
    {
      m_particleSystem = std::make_shared<ParticleSystemOneArray>(particleDraw, PARTICLE_CAPACITY);
      m_boxEmitter.reset(new BoxEmitter());
      m_particleSystem->AddEmitter(m_boxEmitter);
    }
  }
}
