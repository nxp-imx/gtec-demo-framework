/****************************************************************************************************************************************************
 * Copyright 2018, 2022, 2024 NXP
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

#include "HDR04_HDRFramebuffer.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <Shared/HDR/BasicScene/API/OpenGLES3/FragmentUBODataCreator.hpp>
#include <Shared/HDR/BasicScene/API/OpenGLES3/SimpleMeshUtil.hpp>
#include <GLES3/gl3.h>
#include <algorithm>
#include <array>
#include <cmath>
#include "OptionParserEx.hpp"
#include "SharedData.hpp"

namespace Fsl
{
  using namespace GLES3;
  using namespace UI;

  namespace
  {
    constexpr Vector3 DefaultCameraPosition(0.0f, 0.0f, -20.0f);
    constexpr Vector3 DefaultCameraTarget(0.0f, 0.0f, -25.0f);

    GLES3::GLTexture CreateTexture(const std::shared_ptr<IContentManager>& contentManager)
    {
      // The KTX reader does not extract the origin, so we force a upper left
      auto tex =
        contentManager->ReadTexture("Textures/Bricks/Bricks_ETC2_rgb_flipped.ktx", PixelFormat::ETC2_R8G8B8_SRGB_BLOCK, BitmapOrigin::UpperLeft);
      // auto tex = contentManager->ReadTexture("Textures/Test.png", PixelFormat::R8G8B8A8_UNORM);
      // Then override it to match the default GL setting since we know that's the way the texture is stored in the file
      tex.OverrideOrigin(BitmapOrigin::LowerLeft);

      GLTextureParameters texParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      return {tex, texParams};
    }


    GLES3::GLFrameBuffer CreateHdrFrameBuffer(const PxSize2D& resolution)
    {
      GLTextureParameters params(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      GLTextureImageParameters texImageParams(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
      return {resolution, params, texImageParams, GL_DEPTH_COMPONENT16};
    }

    bool IsHDRFramebuffer(const std::shared_ptr<ITag>& tag)
    {
      const auto userTagEx = std::dynamic_pointer_cast<SharedData>(tag);
      return userTagEx && userTagEx->HDRFramebufferEnabled;
    }
  }


  HDR04_HDRFramebuffer::HDR04_HDRFramebuffer(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_hasHDRFramebuffer(IsHDRFramebuffer(config.CustomConfig.AppRegistrationUserTag))
    , m_menuUI(config, !m_hasHDRFramebuffer ? UI::UIColorSpace::SRGBNonLinear : UI::UIColorSpace::SRGBLinear)
    , m_keyboard(config.DemoServiceProvider.Get<IKeyboard>())
    , m_mouse(config.DemoServiceProvider.Get<IMouse>())
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_mouseCaptureEnabled(false)
    , m_useDebugPattern(true)
  {
    RegisterExtension(m_menuUI.GetUIDemoAppExtension());

    const auto options = config.GetOptions<OptionParserEx>();
    m_useDebugPattern = !options->IsPatternDisabled();

    if (!m_hasHDRFramebuffer)
    {
      m_menuUI.SetNoteLabel("HDRFramebuffer: Not available");
    }
    else
    {
      m_menuUI.SetNoteLabel("HDRFramebuffer: Enabled");
    }
    m_menuUI.SetCaptionLeft("Linear");
    m_menuUI.SetCaptionRight(m_hasHDRFramebuffer ? "Bad tonemapper" : "Tonemapped");
    m_menuUI.SetMenuTextLeft("Linear");
    m_menuUI.SetMenuTextRight("Tonemapped");

    m_camera.SetPosition(DefaultCameraPosition, DefaultCameraTarget, Vector3::Up());

    const auto contentManager = GetContentManager();

    m_fragmentUboData = FragmentUBODataCreator::PrepareLights();
    m_resources.TexSRGB = CreateTexture(contentManager);
    m_resources.Program = CreateShader(contentManager);

    const auto* const filenameTonemapLinear = (m_hasHDRFramebuffer ? "TonemapperLinear.frag" : "TonemapperLinearLDR.frag");
    const auto* const filenameTonemapLinearDebug = (m_hasHDRFramebuffer ? "TonemapperLinearDebug.frag" : "TonemapperLinearLDRDebug.frag");
    // NOTE: The Uncharted2Lum shader is really intended for LDR displays so it does not look nice on a real HDR framebuffer
    //       This will be changed in a upcoming update.
    const auto* const filenameTonemap = (m_hasHDRFramebuffer ? "TonemapperUncharted2Lum.frag" : "TonemapperUncharted2LumLDR.frag");
    const auto* const filenameTonemapDebug = (m_hasHDRFramebuffer ? "TonemapperUncharted2LumDebug.frag" : "TonemapperUncharted2LumLDRDebug.frag");

    m_resources.ProgramTonemapLinear = CreateTonemapShader(contentManager, filenameTonemapLinear);
    m_resources.ProgramTonemapLinearDebug = CreateTonemapShader(contentManager, filenameTonemapLinearDebug);
    m_resources.ProgramTonemap = CreateTonemapShader(contentManager, filenameTonemap);
    m_resources.ProgramTonemapDebug = CreateTonemapShader(contentManager, filenameTonemapDebug);

    m_resources.HdrFrameBuffer = CreateHdrFrameBuffer(config.WindowMetrics.GetSizePx());

    m_resources.MeshTunnel = SimpleMeshUtil::CreateTunnelVertexArray(m_resources.Program.Program);
    m_resources.MeshQuad = SimpleMeshUtil::CreateQuadVertexArray(m_resources.ProgramTonemapLinear.Program);
  }


  HDR04_HDRFramebuffer::~HDR04_HDRFramebuffer() = default;


  void HDR04_HDRFramebuffer::OnKeyEvent(const KeyEvent& event)
  {
    m_menuUI.OnKeyEvent(event);

    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::P:
    case VirtualKey::GamePadButtonY:
      m_useDebugPattern = !m_useDebugPattern;
      break;
    default:
      break;
    }
  }


  void HDR04_HDRFramebuffer::OnMouseButtonEvent(const MouseButtonEvent& event)
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
        m_camera.SetPosition(DefaultCameraPosition, DefaultCameraTarget, Vector3::Up());
        event.Handled();
      }
      break;
    default:
      break;
    }
  }

  void HDR04_HDRFramebuffer::Update(const DemoTime& demoTime)
  {
    UpdateInput(demoTime);
    m_menuUI.Update(demoTime);

    m_vertexUboData.MatModel = Matrix::GetIdentity();
    m_vertexUboData.MatView = m_camera.GetViewMatrix();
    const float aspect = GetWindowAspectRatio();    // ok since we divide both by two when we show four screens
    m_vertexUboData.MatProj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspect, 0.1f, 100.0f);
  }


  void HDR04_HDRFramebuffer::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    const auto widowSizePx = GetWindowSizePx();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Render into HDR framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_resources.HdrFrameBuffer.Get());

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto splitX = static_cast<GLint>(std::round(m_menuUI.SplitX.GetValue() * static_cast<float>(widowSizePx.RawWidth())));
    const GLint remainderX = std::min(std::max(widowSizePx.RawWidth() - splitX, 0), widowSizePx.RawWidth());

    const bool inTransition = !m_menuUI.SplitX.IsCompleted();
    const bool useClip = m_menuUI.GetState() == SceneState::Split2 || inTransition;
    const bool showingScene1 = useClip || m_menuUI.GetState() == SceneState::Scene1;
    const bool showingScene2 = useClip || m_menuUI.GetState() == SceneState::Scene2;

    DrawScene(m_resources.Program);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (useClip)
    {
      glEnable(GL_SCISSOR_TEST);
    }

    if (showingScene1)
    {
      if (useClip)
      {
        glScissor(0, 0, splitX, widowSizePx.RawHeight());
      }
      auto& rTonemapProgram = m_useDebugPattern ? m_resources.ProgramTonemapLinearDebug : m_resources.ProgramTonemapLinear;
      DrawTonemappedScene(rTonemapProgram, m_resources.HdrFrameBuffer);
    }
    if (showingScene2)
    {
      if (useClip)
      {
        glScissor(splitX, 0, remainderX, widowSizePx.RawHeight());
      }
      auto& rTonemapProgram = m_useDebugPattern ? m_resources.ProgramTonemapDebug : m_resources.ProgramTonemap;
      DrawTonemappedScene(rTonemapProgram, m_resources.HdrFrameBuffer);
    }

    if (useClip)
    {
      glDisable(GL_SCISSOR_TEST);
    }

    // Calling this last allows the UI to draw on top of everything.
    // Beware that the UI drawing methods might alter the OpenGL state!
    m_menuUI.Draw();
  }


  void HDR04_HDRFramebuffer::UpdateInput(const DemoTime& demoTime)
  {
    const auto keyboardState = m_keyboard->GetState();
    UpdateCameraControlInput(demoTime, keyboardState);
    m_menuUI.UpdateExposureInput(demoTime, keyboardState);
  }


  void HDR04_HDRFramebuffer::UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState)
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


  void HDR04_HDRFramebuffer::DrawScene(const ProgramInfo& programInfo)
  {
    const auto& program = programInfo.Program;
    const auto& location = programInfo.Location;

    // Set the shader program
    glUseProgram(program.Get());

    // Load the matrices
    assert(location.ModelMatrix != GLValues::InvalidLocation);
    assert(location.ViewMatrix != GLValues::InvalidLocation);
    assert(location.ProjMatrix != GLValues::InvalidLocation);
    assert(location.LightPositions != GLValues::InvalidLocation);
    assert(location.LightColors != GLValues::InvalidLocation);

    glUniformMatrix4fv(location.ModelMatrix, 1, 0, m_vertexUboData.MatModel.DirectAccess());
    glUniformMatrix4fv(location.ViewMatrix, 1, 0, m_vertexUboData.MatView.DirectAccess());
    glUniformMatrix4fv(location.ProjMatrix, 1, 0, m_vertexUboData.MatProj.DirectAccess());
    glUniform3fv(location.LightPositions, 4, m_fragmentUboData.LightPositions->DirectAccess());
    glUniform3fv(location.LightColors, 4, m_fragmentUboData.LightColors->DirectAccess());

    // Bind the vertex array
    m_resources.MeshTunnel.VertexArray.Bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_resources.TexSRGB.Get());
    glDrawArrays(GL_TRIANGLES, 0, m_resources.MeshTunnel.VertexBuffer.GetGLCapacity());

    m_resources.MeshTunnel.VertexArray.Unbind();
  }


  void HDR04_HDRFramebuffer::DrawTonemappedScene(const TonemapProgramInfo& programInfo, const GLTextureInfo& hdrFramebufferTexInfo)
  {
    const auto& program = programInfo.Program;
    const auto& location = programInfo.Location;

    // Set the shader program
    glUseProgram(program.Get());

    // The LDR shader dont use exposure
    if (location.Exposure != GLValues::InvalidLocation)
    {
      glUniform1f(location.Exposure, m_menuUI.GetExposure());
    }

    // Bind the vertex array
    m_resources.MeshQuad.VertexArray.Bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrFramebufferTexInfo.Handle);
    // glBindTexture(GL_TEXTURE_2D, m_texSRGB.Get());
    glDrawArrays(GL_TRIANGLES, 0, m_resources.MeshQuad.VertexBuffer.GetGLCapacity());

    m_resources.MeshQuad.VertexArray.Unbind();
  }


  HDR04_HDRFramebuffer::ProgramInfo HDR04_HDRFramebuffer::CreateShader(const std::shared_ptr<IContentManager>& contentManager)
  {
    ProgramInfo info;
    info.Program.Reset(contentManager->ReadAllText("Shader.vert"), contentManager->ReadAllText("Shader.frag"));

    // Get uniform locations (vertex shader)
    info.Location.ModelMatrix = info.Program.GetUniformLocation("g_matModel");
    info.Location.ViewMatrix = info.Program.GetUniformLocation("g_matView");
    info.Location.ProjMatrix = info.Program.GetUniformLocation("g_matProj");

    // Get uniform locations (fragment shader)
    info.Location.LightPositions = info.Program.GetUniformLocation("g_lightPositions");
    info.Location.LightColors = info.Program.GetUniformLocation("g_lightColors");
    return info;
  }


  HDR04_HDRFramebuffer::TonemapProgramInfo HDR04_HDRFramebuffer::CreateTonemapShader(const std::shared_ptr<IContentManager>& contentManager,
                                                                                     const IO::Path& fragmentShaderPath)
  {
    TonemapProgramInfo info;
    info.Program.Reset(contentManager->ReadAllText("Tonemapper.vert"), contentManager->ReadAllText(fragmentShaderPath));

    info.Location.Exposure = info.Program.TryGetUniformLocation("g_exposure");
    return info;
  }
}
