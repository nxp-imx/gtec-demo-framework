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

#include "HDR01_BasicToneMapping.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslSimpleUI/Base/Control/Background9Slice.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <Shared/OpenGLES3/HDR/BasicScene/SimpleMeshUtil.hpp>
#include <GLES3/gl3.h>
#include <array>
#include <cmath>

namespace Fsl
{
  using namespace GLES3;
  using namespace UI;

  namespace
  {
    const Vector3 DEFAULT_CAMERA_POSITION(0.0f, 0.0f, 0.0f);
    const Vector3 DEFAULT_CAMERA_TARGET(0.0f, 0.0f, -4.0f);
  }


  HDR01_BasicToneMapping::HDR01_BasicToneMapping(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_menuUI(config)
    , m_keyboard(config.DemoServiceProvider.Get<IKeyboard>())
    , m_mouse(config.DemoServiceProvider.Get<IMouse>())
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_mouseCaptureEnabled(false)
  {
    RegisterExtension(m_menuUI.GetUIDemoAppExtension());


    m_camera.SetPosition(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_TARGET, Vector3::Up());

    const auto contentManager = GetContentManager();


    PrepareLights();
    CreateTextures(contentManager);
    m_programLDR = CreateShader(contentManager, false);
    m_programHDR = CreateShader(contentManager, true);
    m_meshTunnel = SimpleMeshUtil::CreateTunnelVertexArray(m_programLDR.Program);
  }


  HDR01_BasicToneMapping::~HDR01_BasicToneMapping() = default;


  void HDR01_BasicToneMapping::OnKeyEvent(const KeyEvent& event)
  {
    m_menuUI.OnKeyEvent(event);
  }


  void HDR01_BasicToneMapping::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    switch (event.GetButton())
    {
    case VirtualMouseButton::Right:
    {
      const bool mouseCapture = event.IsPressed();
      if (m_demoAppControl->TryEnableMouseCaptureMode(mouseCapture))
      {
        m_mouseCaptureEnabled = mouseCapture;
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
        m_camera.SetPosition(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_TARGET, Vector3::Up());
        event.Handled();
      }
      break;
    default:
      break;
    }
  }

  void HDR01_BasicToneMapping::Update(const DemoTime& demoTime)
  {
    m_menuUI.Update(demoTime);
    UpdateInput(demoTime);

    const auto screenResolution = GetScreenResolution();
    m_matrixModel = Matrix::GetIdentity();
    m_matrixView = m_camera.GetViewMatrix();
    float aspect = static_cast<float>(screenResolution.X) / screenResolution.Y;    // ok since we divide both by two when we show four screens
    m_matrixProjection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspect, 0.1f, 100.0f);
  }


  void HDR01_BasicToneMapping::Draw(const DemoTime& demoTime)
  {
    const auto screenResolution = GetScreenResolution();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto splitX = static_cast<GLint>(std::round(m_menuUI.SplitX.GetValue()));
    const GLint remainderX = screenResolution.X - splitX;

    const bool inTransition = !m_menuUI.SplitX.IsCompleted();
    const bool useClip = m_menuUI.GetState() == SceneState::Split2 || inTransition;
    const bool showingScene1 = useClip || m_menuUI.GetState() == SceneState::Scene1;
    const bool showingScene2 = useClip || m_menuUI.GetState() == SceneState::Scene2;

    if (useClip)
    {
      glEnable(GL_SCISSOR_TEST);
    }

    if (showingScene1)
    {
      if (useClip)
      {
        glScissor(0, 0, splitX, screenResolution.Y);
      }
      DrawScene(m_programLDR);
    }
    if (showingScene2)
    {
      if (useClip)
      {
        glScissor(splitX, 0, remainderX, screenResolution.Y);
      }
      DrawScene(m_programHDR);
    }

    if (useClip)
    {
      glDisable(GL_SCISSOR_TEST);
    }

    // Calling this last allows the UI to draw on top of everything.
    // Beware that the UI drawing methods might alter the OpenGL state!
    m_menuUI.Draw();
  }


  void HDR01_BasicToneMapping::UpdateInput(const DemoTime& demoTime)
  {
    const auto keyboardState = m_keyboard->GetState();
    UpdateCameraControlInput(demoTime, keyboardState);
    m_menuUI.UpdateExposureInput(demoTime, keyboardState);
  }


  void HDR01_BasicToneMapping::UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState)
  {
    {    // Mouse camera rotation
      const auto mouseState = m_mouse->GetState();

      if (!m_mouseCaptureEnabled)
      {
        const bool rotateCamera = mouseState.IsRightButtonPressed();
        m_camera.RotateViaPosition(rotateCamera, mouseState.Position);
      }
      else
      {
        if (mouseState.IsRightButtonPressed())
        {
          const auto rawPosition = Vector2(mouseState.RawPosition.X, -mouseState.RawPosition.Y);
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


  void HDR01_BasicToneMapping::DrawScene(const ProgramInfo& programInfo)
  {
    const auto& program = programInfo.Program;
    const auto& location = programInfo.Location;

    // Set the shader program
    glUseProgram(program.Get());

    // Load the matrices
    assert(location.ModelMatrix != GLValues::INVALID_LOCATION);
    assert(location.ViewMatrix != GLValues::INVALID_LOCATION);
    assert(location.ProjMatrix != GLValues::INVALID_LOCATION);
    assert(location.LightPositions != GLValues::INVALID_LOCATION);
    assert(location.LightColors != GLValues::INVALID_LOCATION);

    glUniformMatrix4fv(location.ModelMatrix, 1, 0, m_matrixModel.DirectAccess());
    glUniformMatrix4fv(location.ViewMatrix, 1, 0, m_matrixView.DirectAccess());
    glUniformMatrix4fv(location.ProjMatrix, 1, 0, m_matrixProjection.DirectAccess());
    glUniform3fv(location.LightPositions, 4, m_lightPositions.data()->DirectAccess());
    glUniform3fv(location.LightColors, 4, m_lightColors.data()->DirectAccess());
    // The LDR shader dont use exposure
    if (location.Exposure != GLValues::INVALID_LOCATION)
    {
      glUniform1f(location.Exposure, m_menuUI.GetExposure());
    }

    // Bind the vertex array
    m_meshTunnel.VertexArray.Bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texSRGB.Get());
    glDrawArrays(GL_TRIANGLES, 0, m_meshTunnel.VertexBuffer.GetCapacity());

    m_meshTunnel.VertexArray.Unbind();
  }


  void HDR01_BasicToneMapping::PrepareLights()
  {
    // lighting info
    m_lightPositions = {Vector3(0.0f, 0.0f, -31.5f), Vector3(-1.4f, -1.9f, -9.0f), Vector3(0.0f, -1.8f, -4.0f),
                        // Vector3(0.0f, 0.0f, 0.0f)
                        Vector3(0.8f, -1.7f, -6.0f)};
    m_lightColors = {
      Vector3(200.0f, 200.0f, 200.0f), Vector3(0.1f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.2f), Vector3(0.0f, 0.1f, 0.0f),
      // Vector3(221.0f, 1.1f, 1.0f)
    };
  }


  void HDR01_BasicToneMapping::CreateTextures(const std::shared_ptr<IContentManager>& contentManager)
  {
    // The KTX reader does not extract the origin, so we force a upper left
    auto tex = contentManager->ReadTexture("Textures/Bricks_ETC2_rgb.ktx", PixelFormat::ETC2_R8G8B8_SRGB_BLOCK, BitmapOrigin::UpperLeft);
    // Then override it to match the default GL setting since we know thats the way the texture is stored in the file
    tex.OverrideOrigin(BitmapOrigin::LowerLeft);

    GLTextureParameters texParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    m_texSRGB.Reset(tex, texParams);
  }


  HDR01_BasicToneMapping::ProgramInfo HDR01_BasicToneMapping::CreateShader(const std::shared_ptr<IContentManager>& contentManager, const bool useHDR)
  {
    ProgramInfo info;
    const auto fragmentShaderName = useHDR ? "ShaderHDR.frag" : "ShaderLDR.frag";
    info.Program.Reset(contentManager->ReadAllText("Shader.vert"), contentManager->ReadAllText(fragmentShaderName));

    // Get uniform locations (vertex shader)
    info.Location.ModelMatrix = info.Program.GetUniformLocation("g_matModel");
    info.Location.ViewMatrix = info.Program.GetUniformLocation("g_matView");
    info.Location.ProjMatrix = info.Program.GetUniformLocation("g_matProj");

    // Get uniform locations (fragment shader)
    info.Location.LightPositions = info.Program.GetUniformLocation("g_lightPositions");
    info.Location.LightColors = info.Program.GetUniformLocation("g_lightColors");
    if (useHDR)
    {
      info.Location.Exposure = info.Program.GetUniformLocation("g_exposure");
    }
    return info;
  }
}
