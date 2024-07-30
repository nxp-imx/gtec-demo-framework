/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

#include "Skybox.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture3.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <Shared/CubeMapping/API/OpenGLES3/SkyboxVertices.hpp>
#include <Shared/CubeMapping/API/OpenGLES3/TextureUtil.hpp>
#include <GLES3/gl3.h>
#include <array>
#include "OptionParser.hpp"

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    constexpr Vector3 DefaultCameraPosition(0.0f, 0.0f, 0.0f);
    constexpr Vector3 DefaultCameraTarget(0.0f, 0.0f, -4.0f);
  }


  Skybox::Skybox(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_keyboard(config.DemoServiceProvider.Get<IKeyboard>())
    , m_mouse(config.DemoServiceProvider.Get<IMouse>())
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_mouseCaptureEnabled(false)
    , m_rotationSpeed(5.4f, 0.0f)
  {
    const auto options = config.GetOptions<OptionParser>();

    m_camera.SetPosition(DefaultCameraPosition, DefaultCameraTarget, Vector3::Up());


    const auto contentManager = GetContentManager();

    IO::Path texture(options->GetScene() == SceneState::Scene1 ? "Textures/Cubemap/Yokohama3/Raw" : "Textures/Cubemap/Test/Raw");

    FSLLOG3_INFO("Preparing cubemap");
    m_resources.CubemapTexture = TextureUtil::CreateCubemapTextureFromSix(contentManager, texture, PixelFormat::R8G8B8A8_UNORM);
    FSLLOG3_INFO("Preparing shaders");
    m_resources.MainSkyboxProgram.Reset(contentManager->ReadAllText("skybox.vert"), contentManager->ReadAllText("skybox.frag"));
    m_resources.MainSkyboxMesh.Reset(m_resources.MainSkyboxProgram.Program);

    GL_CHECK_FOR_ERROR();
    FSLLOG3_INFO("Ready");
  }


  Skybox::~Skybox() = default;


  void Skybox::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    switch (event.GetButton())
    {
    case VirtualMouseButton::Right:
      {
        m_rightMouseDown = event.IsPressed();
        if (m_demoAppControl->TryEnableMouseCaptureMode(m_rightMouseDown))
        {
          m_mouseCaptureEnabled = m_rightMouseDown;
        }
        else
        {
          m_mouseCaptureEnabled = false;
        }
        event.Handled();
        break;
      }
    case VirtualMouseButton::Middle:
      if (event.IsPressed())
      {
        m_camera.SetPosition(DefaultCameraPosition, DefaultCameraTarget, Vector3::Up());
        event.Handled();
      }
      break;
    default:
      break;
    }
  }


  void Skybox::Update(const DemoTime& demoTime)
  {
    if (!m_rightMouseDown)
    {
      m_camera.RotateByRadians(m_rotationSpeed * demoTime.DeltaTime);
    }
    UpdateCameraControlInput(demoTime, m_keyboard->GetState());

    const float aspect = GetWindowAspectRatio();
    m_vertexUboData.MatProj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspect, 0.1f, 1000.0f);

    m_vertexUboData.MatView = m_camera.GetViewMatrix();
  }


  void Skybox::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DrawScene(m_resources.MainSkyboxProgram, m_resources.MainSkyboxMesh, m_resources.CubemapTexture, m_vertexUboData);
  }


  void Skybox::DrawScene(const SkyboxProgram& programInfo, const SkyboxMesh& mesh, const GLES3::GLTexture& texture, const VertexUBOData& uboData)
  {
    const auto& program = programInfo.Program;
    const auto& location = programInfo.Location;

    // Set the shader program
    glUseProgram(program.Get());

    // Load the matrices
    assert(location.ViewMatrix != GLValues::InvalidLocation);
    assert(location.ProjMatrix != GLValues::InvalidLocation);
    assert(location.SkyboxSampler != GLValues::InvalidLocation);

    glUniformMatrix4fv(location.ViewMatrix, 1, 0, uboData.MatView.DirectAccess());
    glUniformMatrix4fv(location.ProjMatrix, 1, 0, uboData.MatProj.DirectAccess());

    // Bind the vertex array
    mesh.VertexArray.Bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.Get());
    glDrawArrays(GL_TRIANGLES, 0, mesh.VertexBuffer.GetGLCapacity());
    // glDrawArrays(GL_TRIANGLES, 0, 6);

    mesh.VertexArray.Unbind();

    glUseProgram(0);
  }


  void Skybox::UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState)
  {
    {    // Mouse camera rotation
      const auto mouseState = m_mouse->GetState();

      if (!m_rightMouseDown)
      {
        const bool rotateCamera = mouseState.IsRightButtonPressed();
        m_camera.RotateViaPosition(rotateCamera, mouseState.Position);
      }
      else
      {
        if (mouseState.IsRightButtonPressed())
        {
          const auto rawPosition = Vector2(mouseState.RawPosition.X.Value, -mouseState.RawPosition.Y.Value);
          m_camera.Rotate(rawPosition);
        }
      }
    }

    // Keyboard camera movement
    const float movementSpeed = 2.0f * demoTime.DeltaTime;
    if (keyboardState.IsKeyDown(VirtualKey::W))
    {
      m_camera.MoveForward(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::S))
    {
      m_camera.MoveBackwards(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::A))
    {
      m_camera.MoveLeft(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::D))
    {
      m_camera.MoveRight(movementSpeed);
    }
  }
}
