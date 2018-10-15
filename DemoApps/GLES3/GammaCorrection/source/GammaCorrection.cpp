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

#include "GammaCorrection.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <GLES3/gl3.h>

namespace Fsl
{
  using namespace GLES3;
  using namespace UI;

  namespace
  {
    const Vector3 DEFAULT_CAMERA_POSITION(4.0f, 1.0f, 7.0f);
    const Vector3 DEFAULT_CAMERA_TARGET(0.0f, 0.0f, 0.0f);
  }

  GammaCorrection::GammaCorrection(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_uiEventListener(this)    // The UI listener forwards call to 'this' object
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "MainAtlas"))    // Prepare the extension
    , m_keyboard(config.DemoServiceProvider.Get<IKeyboard>())
    , m_mouse(config.DemoServiceProvider.Get<IMouse>())
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_mouseCaptureEnabled(false)
    , m_hModelViewMatrixLoc(GLValues::INVALID_LOCATION)
    , m_hProjMatrixLoc(GLValues::INVALID_LOCATION)
    , m_hLightPositions(GLValues::INVALID_LOCATION)
    , m_hLightColors(GLValues::INVALID_LOCATION)
    , m_hViewPos(GLValues::INVALID_LOCATION)
    , m_hGamma(GLValues::INVALID_LOCATION)
    , m_state(State::Split4)
    , m_splitX(m_transitionCache, TransitionTimeSpan(400, TransitionTimeUnit::Milliseconds), TransitionType::Smooth)
    , m_splitY(m_transitionCache, TransitionTimeSpan(400, TransitionTimeUnit::Milliseconds), TransitionType::Smooth)
    , m_scene1LabelAlpha(m_transitionCache, TransitionTimeSpan(200, TransitionTimeUnit::Milliseconds), TransitionType::Smooth)
    , m_scene2LabelAlpha(m_transitionCache, TransitionTimeSpan(200, TransitionTimeUnit::Milliseconds), TransitionType::Smooth)
    , m_scene3LabelAlpha(m_transitionCache, TransitionTimeSpan(200, TransitionTimeUnit::Milliseconds), TransitionType::Smooth)
    , m_scene4LabelAlpha(m_transitionCache, TransitionTimeSpan(200, TransitionTimeUnit::Milliseconds), TransitionType::Smooth)
  {
    m_camera.SetPosition(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_TARGET, Vector3::Up());

    const auto contentManager = GetContentManager();

    PrepareTransition();
    PrepareLights();
    CreateUI();
    CreateTextures(contentManager);
    m_program = CreateShader(contentManager);
    CreateVertexArray(m_program);
  }


  GammaCorrection::~GammaCorrection() = default;


  void GammaCorrection::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::Code1:
      m_state = State::Scene1;
      break;
    case VirtualKey::Code2:
      m_state = State::Scene2;
      break;
    case VirtualKey::Code3:
      m_state = State::Scene3;
      break;
    case VirtualKey::Code4:
      m_state = State::Scene4;
      break;
    case VirtualKey::Code5:
    case VirtualKey::Space:
      m_state = State::Split4;
      break;
    default:
      break;
    }
  }


  void GammaCorrection::OnMouseButtonEvent(const MouseButtonEvent& event)
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


  void GammaCorrection::Update(const DemoTime& demoTime)
  {
    UpdateInput(demoTime);
    UpdateSceneTransition(demoTime);

    const auto screenResolution = GetScreenResolution();
    auto matrixWorld = Matrix::GetIdentity();
    auto matrixView = m_camera.GetViewMatrix();
    float aspect = static_cast<float>(screenResolution.X) / screenResolution.Y;    // ok since we divide both by two when we show four screens
    m_matrixProjection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspect, 0.1f, 100.0f);
    m_matrixWorldView = matrixWorld * matrixView;
  }


  void GammaCorrection::Draw(const DemoTime& demoTime)
  {
    glUseProgram(m_program.Get());

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DrawScenes();

    // Calling this last allows the UI to draw on top of everything.
    // Beware that the UI drawing methods might alter the OpenGL state!
    m_uiExtension->Draw();
  }


  void GammaCorrection::UpdateInput(const DemoTime& demoTime)
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

    {    // Keyboard camera movement
      const float movementSpeed = 2.0f * demoTime.DeltaTime;
      auto keyboardState = m_keyboard->GetState();

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

  void GammaCorrection::UpdateSceneTransition(const DemoTime& demoTime)
  {
    const auto res = Vector2(GetScreenResolution().X, GetScreenResolution().Y);

    switch (m_state)
    {
    case State::Scene1:
      m_splitX.SetValue(res.X);
      m_splitY.SetValue(0.0f);
      m_scene1LabelAlpha.SetValue(1.0f);
      m_scene2LabelAlpha.SetValue(0.0f);
      m_scene3LabelAlpha.SetValue(0.0f);
      m_scene4LabelAlpha.SetValue(0.0f);
      break;
    case State::Scene2:
      m_splitX.SetValue(0.0f);
      m_splitY.SetValue(0.0f);
      m_scene1LabelAlpha.SetValue(0.0f);
      m_scene2LabelAlpha.SetValue(1.0f);
      m_scene3LabelAlpha.SetValue(0.0f);
      m_scene4LabelAlpha.SetValue(0.0f);
      break;
    case State::Scene3:
      m_splitX.SetValue(res.X);
      m_splitY.SetValue(res.Y);
      m_scene1LabelAlpha.SetValue(0.0f);
      m_scene2LabelAlpha.SetValue(0.0f);
      m_scene3LabelAlpha.SetValue(1.0f);
      m_scene4LabelAlpha.SetValue(0.0f);
      break;
    case State::Scene4:
      m_splitX.SetValue(0.0f);
      m_splitY.SetValue(res.Y);
      m_scene1LabelAlpha.SetValue(0.0f);
      m_scene2LabelAlpha.SetValue(0.0f);
      m_scene3LabelAlpha.SetValue(0.0f);
      m_scene4LabelAlpha.SetValue(1.0f);
      break;
    case State::Split4:
    default:
      m_splitX.SetValue(res.X / 2.0f);
      m_splitY.SetValue(res.Y / 2.0f);
      m_scene1LabelAlpha.SetValue(1.0f);
      m_scene2LabelAlpha.SetValue(1.0f);
      m_scene3LabelAlpha.SetValue(1.0f);
      m_scene4LabelAlpha.SetValue(1.0f);
      break;
    }
    m_splitX.Update(TransitionTimeSpan(demoTime.DeltaTimeInMicroseconds, TransitionTimeUnit::Microseconds));
    m_splitY.Update(TransitionTimeSpan(demoTime.DeltaTimeInMicroseconds, TransitionTimeUnit::Microseconds));
    m_scene1LabelAlpha.Update(TransitionTimeSpan(demoTime.DeltaTimeInMicroseconds, TransitionTimeUnit::Microseconds));
    m_scene2LabelAlpha.Update(TransitionTimeSpan(demoTime.DeltaTimeInMicroseconds, TransitionTimeUnit::Microseconds));
    m_scene3LabelAlpha.Update(TransitionTimeSpan(demoTime.DeltaTimeInMicroseconds, TransitionTimeUnit::Microseconds));
    m_scene4LabelAlpha.Update(TransitionTimeSpan(demoTime.DeltaTimeInMicroseconds, TransitionTimeUnit::Microseconds));

    const float alpha1 = m_scene1LabelAlpha.GetValue();
    const float alpha2 = m_scene2LabelAlpha.GetValue();
    const float alpha3 = m_scene3LabelAlpha.GetValue();
    const float alpha4 = m_scene4LabelAlpha.GetValue();
    m_labelTopLeft->SetFontColor(Color(alpha1, alpha1, alpha1, alpha1));
    m_labelTopRight->SetFontColor(Color(alpha2, alpha2, alpha2, alpha2));
    m_labelBottomLeft->SetFontColor(Color(alpha3, alpha3, alpha3, alpha3));
    m_labelBottomRight->SetFontColor(Color(alpha4, alpha4, alpha4, alpha4));
  }


  void GammaCorrection::DrawScenes()
  {
    const auto screenResolution = GetScreenResolution();

    // Set the shader program
    glUseProgram(m_program.Get());

    // Load the matrices
    assert(m_hModelViewMatrixLoc != GLValues::INVALID_HANDLE);
    assert(m_hProjMatrixLoc != GLValues::INVALID_HANDLE);
    assert(m_hLightPositions != GLValues::INVALID_HANDLE);
    assert(m_hLightColors != GLValues::INVALID_HANDLE);
    assert(m_hViewPos != GLValues::INVALID_HANDLE);

    glUniformMatrix4fv(m_hModelViewMatrixLoc, 1, 0, m_matrixWorldView.DirectAccess());
    glUniformMatrix4fv(m_hProjMatrixLoc, 1, 0, m_matrixProjection.DirectAccess());
    glUniform3fv(m_hLightPositions, 4, m_lightPositions.data()->DirectAccess());
    glUniform3fv(m_hLightColors, 4, m_lightColors.data()->DirectAccess());
    glUniform3fv(m_hViewPos, 1, m_camera.GetPosition().DirectAccess());

    // Bind the vertex array
    m_vertexArray.Bind();

    const auto splitX = static_cast<GLint>(std::round(m_splitX.GetValue()));
    const auto splitY = static_cast<GLint>(std::round(m_splitY.GetValue()));
    const GLint remainderX = screenResolution.X - splitX;
    const GLint remainderY = screenResolution.Y - splitY;

    glBindTexture(GL_TEXTURE_2D, m_texLinear.Get());

    // bottom left (no gamma correction, rgb texture)
    glViewport(0, 0, splitX, splitY);
    glUniform1i(m_hGamma, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // top right (gamma correction enabled, rgb texture)
    glViewport(splitX, splitY, remainderX, remainderY);
    glUniform1i(m_hGamma, 1);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texSRGB.Get());

    // Bottom right (gamma correction, srgb texture)
    glViewport(splitX, 0, remainderX, splitY);
    // glUniform1i(m_hGamma, true);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // top left (no gamma correction, srgb texture)
    glViewport(0, splitY, splitX, remainderY);
    glUniform1i(m_hGamma, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_vertexArray.Unbind();

    // Restore the viewport
    glViewport(0, 0, screenResolution.X, screenResolution.Y);
  }


  void GammaCorrection::PrepareTransition()
  {
    // Force set the initial value so we dont begin with a transition
    const auto res = Vector2(GetScreenResolution().X, GetScreenResolution().Y);
    m_splitX.SetActualValue(res.X / 2.0f);
    m_splitY.SetActualValue(res.Y / 2.0f);
    m_scene1LabelAlpha.SetActualValue(1.0f);
    m_scene2LabelAlpha.SetActualValue(1.0f);
    m_scene3LabelAlpha.SetActualValue(1.0f);
    m_scene4LabelAlpha.SetActualValue(1.0f);
  }


  void GammaCorrection::PrepareLights()
  {
    // lighting info
    m_lightPositions = {Vector3(0.0f, 0.0f, -3.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 3.0f)};
    m_lightColors = {Vector3(0.25f, 0.25f, 0.25f), Vector3(0.50f, 0.50f, 0.50f), Vector3(0.75f, 0.75f, 0.75f), Vector3(1.00f, 1.00f, 1.00f)};
  }


  void GammaCorrection::CreateTextures(const std::shared_ptr<IContentManager>& contentManager)
  {
    // The KTX reader does not extract the origin, so we force a upper left
    auto tex = contentManager->ReadTexture("Floor/Floor_ETC2_RGB.ktx", PixelFormat::ETC2_R8G8B8_UNORM_BLOCK, BitmapOrigin::UpperLeft);
    // Then override it to match the default GL setting since we know thats the way the texture is stored in the file
    tex.OverrideOrigin(BitmapOrigin::LowerLeft);

    GLTextureParameters texParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    // m_texLinear.Reset(tex, texParams, TextureFlags::GenerateMipMaps);
    m_texLinear.Reset(tex, texParams);
    tex.SetCompatiblePixelFormat(PixelFormat::ETC2_R8G8B8_SRGB_BLOCK);
    m_texSRGB.Reset(tex, texParams);
  }


  GLES3::GLProgram GammaCorrection::CreateShader(const std::shared_ptr<IContentManager>& contentManager)
  {
    GLProgram program(contentManager->ReadAllText("GammaCorrection.vert"), contentManager->ReadAllText("GammaCorrection.frag"));

    // Get uniform locations (vertex shader)
    m_hModelViewMatrixLoc = program.GetUniformLocation("g_matModelView");
    m_hProjMatrixLoc = program.GetUniformLocation("g_matProj");
    // Get uniform locations (fragment shader)
    m_hLightPositions = program.GetUniformLocation("lightPositions");
    m_hLightColors = program.GetUniformLocation("lightColors");
    m_hViewPos = program.GetUniformLocation("viewPos");
    m_hGamma = program.GetUniformLocation("gamma");
    return program;
  }


  void GammaCorrection::CreateVertexArray(const GLES3::GLProgram& program)
  {
    // B D
    // |\|
    // A C
    // A = 1.0
    const float x0 = -10.0f;
    const float x1 = 10.0f;
    const float y = -0.5f;
    const float z0 = -10.0f;
    const float z1 = 10.0f;
    const float u0 = 0.0f;
    const float u1 = 10.0f;
    const float v0 = 10.0f;
    const float v1 = 0.0f;
    const Vector3 normal(0.0f, 1.0f, 0.0f);
    VertexPositionNormalTexture vertices[6] = {
      VertexPositionNormalTexture(Vector3(x0, y, z0), normal, Vector2(u0, v0)),
      VertexPositionNormalTexture(Vector3(x0, y, z1), normal, Vector2(u0, v1)),
      VertexPositionNormalTexture(Vector3(x1, y, z1), normal, Vector2(u1, v1)),

      VertexPositionNormalTexture(Vector3(x0, y, z0), normal, Vector2(u0, v0)),
      VertexPositionNormalTexture(Vector3(x1, y, z1), normal, Vector2(u1, v1)),
      VertexPositionNormalTexture(Vector3(x1, y, z0), normal, Vector2(u1, v0)),
    };

    auto vertexDecl = VertexPositionNormalTexture::GetVertexDeclaration();
    std::vector<GLES3::GLVertexAttribLink> attribLink(3);
    attribLink[0] =
      GLVertexAttribLink(program.GetAttribLocation("VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    attribLink[1] = GLVertexAttribLink(program.GetAttribLocation("VertexNormal"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Normal, 0));
    attribLink[2] =
      GLVertexAttribLink(program.GetAttribLocation("VertexTexCoord"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));


    m_vertexBuffer.Reset(vertices, sizeof(vertices) / sizeof(VertexPositionNormalTexture), GL_STATIC_DRAW);

    // Prepare the vertex arrays
    m_vertexArray.Reset(true);
    m_vertexArray.Bind();
    {
      // Set up VBO Vertex Attribute information
      GL_CHECK(glBindBuffer(m_vertexBuffer.GetTarget(), m_vertexBuffer.Get()));

      // - We assume that the vertex format is listed in the same order as the shader requires them.
      m_vertexBuffer.EnableAttribArrays(attribLink);
    }
    m_vertexArray.Unbind();
  }


  void GammaCorrection::CreateUI()
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);

    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();

    // Create a label to write stuff into when a button is pressed
    m_labelTopLeft = std::make_shared<Label>(context);
    m_labelTopLeft->SetAlignmentX(ItemAlignment::Near);
    m_labelTopLeft->SetAlignmentY(ItemAlignment::Near);
    m_labelTopLeft->SetContent("SRGB texture, no gamma correction");

    m_labelTopRight = std::make_shared<Label>(context);
    m_labelTopRight->SetAlignmentX(ItemAlignment::Far);
    m_labelTopRight->SetAlignmentY(ItemAlignment::Near);
    m_labelTopRight->SetContent("RGB texture, gamma correction");

    m_labelBottomLeft = std::make_shared<Label>(context);
    m_labelBottomLeft->SetAlignmentX(ItemAlignment::Near);
    m_labelBottomLeft->SetAlignmentY(ItemAlignment::Far);
    m_labelBottomLeft->SetContent("RGB texture, no gamma correction");

    m_labelBottomRight = std::make_shared<Label>(context);
    m_labelBottomRight->SetAlignmentX(ItemAlignment::Far);
    m_labelBottomRight->SetAlignmentY(ItemAlignment::Far);
    m_labelBottomRight->SetContent("SRGB texture, gamma correction (Correct)");

    // Create a 'root' layout we use the recommended fill layout as it will utilize all available space on the screen
    // We then add the 'player' stack to it and the label
    auto fillLayout = std::make_shared<FillLayout>(context);
    fillLayout->AddChild(m_labelTopLeft);
    fillLayout->AddChild(m_labelTopRight);
    fillLayout->AddChild(m_labelBottomLeft);
    fillLayout->AddChild(m_labelBottomRight);

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(fillLayout);
  }
}
