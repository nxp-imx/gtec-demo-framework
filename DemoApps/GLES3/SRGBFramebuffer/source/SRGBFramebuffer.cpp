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

#include "SRGBFramebuffer.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <GLES3/gl3.h>
#include <cmath>
#include "SharedData.hpp"

namespace Fsl
{
  using namespace GLES3;
  using namespace UI;

  namespace
  {
    const Vector3 DEFAULT_CAMERA_POSITION(4.0f, 1.0f, 7.0f);
    const Vector3 DEFAULT_CAMERA_TARGET(0.0f, 0.0f, 0.0f);
  }


  SRGBFramebuffer::SRGBFramebuffer(const DemoAppConfig& config)
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
    , m_state(State::Split2)
    , m_splitX(m_transitionCache, TransitionTimeSpan(400, TransitionTimeUnit::Milliseconds), TransitionType::Smooth)
    , m_splitSceneWidthL(m_transitionCache, TransitionTimeSpan(400, TransitionTimeUnit::Milliseconds), TransitionType::Smooth)
    , m_splitSceneWidthR(m_transitionCache, TransitionTimeSpan(400, TransitionTimeUnit::Milliseconds), TransitionType::Smooth)
    , m_splitSceneAlphaL(m_transitionCache, TransitionTimeSpan(200, TransitionTimeUnit::Milliseconds), TransitionType::Smooth)
    , m_splitSceneAlphaR(m_transitionCache, TransitionTimeSpan(200, TransitionTimeUnit::Milliseconds), TransitionType::Smooth)
  {
    const auto userTagEx = std::dynamic_pointer_cast<SharedData>(config.CustomConfig.AppRegistrationUserTag);
    bool hasSRGBFramebuffer = userTagEx && userTagEx->SRGBFramebufferEnabled;

    m_camera.SetPosition(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_TARGET, Vector3::Up());

    const auto contentManager = GetContentManager();

    PrepareTransition();
    PrepareLights();
    CreateUI(hasSRGBFramebuffer);
    CreateTextures(contentManager);
    m_program = CreateShader(contentManager, hasSRGBFramebuffer);
    CreateVertexArray(m_program);
  }


  SRGBFramebuffer::~SRGBFramebuffer() = default;


  void SRGBFramebuffer::OnKeyEvent(const KeyEvent& event)
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
    case VirtualKey::Space:
      m_state = State::Split2;
      break;
    default:
      break;
    }
  }


  void SRGBFramebuffer::OnMouseButtonEvent(const MouseButtonEvent& event)
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


  void SRGBFramebuffer::Update(const DemoTime& demoTime)
  {
    UpdateInput(demoTime);
    UpdateSceneTransition(demoTime);

    const auto screenResolution = GetScreenResolution();
    auto matrixWorld = Matrix::GetIdentity();
    auto matrixView = m_camera.GetViewMatrix();

    const float aspectL = m_splitSceneWidthL.GetValue() / screenResolution.Y;
    m_matrixProjectionL = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspectL, 0.1f, 100.0f);
    m_matrixWorldViewL = matrixWorld * matrixView;

    const float aspectR = m_splitSceneWidthR.GetValue() / screenResolution.Y;
    m_matrixProjectionR = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspectR, 0.1f, 100.0f);
    m_matrixWorldViewR = matrixWorld * matrixView;
  }


  void SRGBFramebuffer::Draw(const DemoTime& demoTime)
  {
    GL_CHECK(glUseProgram(m_program.Get()));

    glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DrawScenes();

    // Calling this last allows the UI to draw on top of everything.
    // Beware that the UI drawing methods might alter the OpenGL state!
    m_uiExtension->Draw();
  }


  void SRGBFramebuffer::UpdateInput(const DemoTime& demoTime)
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

  void SRGBFramebuffer::UpdateSceneTransition(const DemoTime& demoTime)
  {
    const auto res = Vector2(GetScreenResolution().X, GetScreenResolution().Y);

    switch (m_state)
    {
    case State::Scene1:
      m_splitX.SetValue(res.X);
      m_splitSceneWidthL.SetValue(res.X);
      m_splitSceneWidthR.SetValue(0.0f);
      m_splitSceneAlphaL.SetValue(1.0f);
      m_splitSceneAlphaR.SetValue(0.0f);
      break;
    case State::Scene2:
      m_splitX.SetValue(0.0f);
      m_splitSceneWidthL.SetValue(0.0f);
      m_splitSceneWidthR.SetValue(res.X);
      m_splitSceneAlphaL.SetValue(0.0f);
      m_splitSceneAlphaR.SetValue(1.0f);
      break;
    case State::Split2:
    default:
      m_splitX.SetValue(res.X / 2.0f);
      m_splitSceneWidthL.SetValue(res.X / 2.0f);
      m_splitSceneWidthR.SetValue(res.X / 2.0f);
      m_splitSceneAlphaL.SetValue(1.0f);
      m_splitSceneAlphaR.SetValue(1.0f);
      break;
    }
    m_splitX.Update(TransitionTimeSpan(demoTime.DeltaTimeInMicroseconds, TransitionTimeUnit::Microseconds));
    m_splitSceneWidthL.Update(TransitionTimeSpan(demoTime.DeltaTimeInMicroseconds, TransitionTimeUnit::Microseconds));
    m_splitSceneWidthR.Update(TransitionTimeSpan(demoTime.DeltaTimeInMicroseconds, TransitionTimeUnit::Microseconds));
    m_splitSceneAlphaL.Update(TransitionTimeSpan(demoTime.DeltaTimeInMicroseconds, TransitionTimeUnit::Microseconds));
    m_splitSceneAlphaR.Update(TransitionTimeSpan(demoTime.DeltaTimeInMicroseconds, TransitionTimeUnit::Microseconds));

    const float alphaL = m_splitSceneAlphaL.GetValue();
    const float alphaR = m_splitSceneAlphaR.GetValue();
    m_labelLeft->SetFontColor(Color(alphaL, alphaL, alphaL, alphaL));
    m_labelRight->SetFontColor(Color(alphaR, alphaR, alphaR, alphaR));
  }


  void SRGBFramebuffer::DrawScenes()
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

    glUniformMatrix4fv(m_hModelViewMatrixLoc, 1, 0, m_matrixWorldViewL.DirectAccess());
    glUniformMatrix4fv(m_hProjMatrixLoc, 1, 0, m_matrixProjectionL.DirectAccess());
    glUniform3fv(m_hLightPositions, 4, m_lightPositions.data()->DirectAccess());
    glUniform3fv(m_hLightColors, 4, m_lightColors.data()->DirectAccess());
    glUniform3fv(m_hViewPos, 1, m_camera.GetPosition().DirectAccess());

    // Bind the vertex array
    m_vertexArray.Bind();

    // Android build complains about std::round (so this makes all happy)
    using namespace std;
    const auto splitX = static_cast<GLint>(round(m_splitX.GetValue()));
    const GLint remainderX = screenResolution.X - splitX;

    glBindTexture(GL_TEXTURE_2D, m_texLinear.Get());


    // left (gamma correction enabled, rgb texture)
    glViewport(0, 0, splitX, screenResolution.Y);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texSRGB.Get());

    // Bottom right (gamma correction, srgb texture)
    glUniformMatrix4fv(m_hModelViewMatrixLoc, 1, 0, m_matrixWorldViewR.DirectAccess());
    glUniformMatrix4fv(m_hProjMatrixLoc, 1, 0, m_matrixProjectionR.DirectAccess());
    glViewport(splitX, 0, remainderX, screenResolution.Y);
    // glUniform1i(m_hGamma, true);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_vertexArray.Unbind();

    // Restore the viewport
    glViewport(0, 0, screenResolution.X, screenResolution.Y);
  }


  void SRGBFramebuffer::PrepareTransition()
  {
    // Force set the initial value so we dont begin with a transition
    const auto res = Vector2(GetScreenResolution().X, GetScreenResolution().Y);
    m_splitX.SetActualValue(res.X / 2.0f);
    m_splitSceneWidthL.SetActualValue(res.X / 2.0f);
    m_splitSceneWidthR.SetActualValue(res.X / 2.0f);
    m_splitSceneAlphaL.SetActualValue(1.0f);
    m_splitSceneAlphaL.SetActualValue(1.0f);
  }


  void SRGBFramebuffer::PrepareLights()
  {
    // lighting info
    m_lightPositions = {Vector3(0.0f, 0.0f, -3.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 3.0f)};
    m_lightColors = {Vector3(0.25f, 0.25f, 0.25f), Vector3(0.50f, 0.50f, 0.50f), Vector3(0.75f, 0.75f, 0.75f), Vector3(1.00f, 1.00f, 1.00f)};
  }


  void SRGBFramebuffer::CreateTextures(const std::shared_ptr<IContentManager>& contentManager)
  {
    // The KTX reader does not extract the origin, so we force a upper left
    auto tex =
      contentManager->ReadTexture("Textures/WoodFloor/Floor_ETC2_RGB_flipped.ktx", PixelFormat::ETC2_R8G8B8_UNORM_BLOCK, BitmapOrigin::UpperLeft);
    // Then override it to match the default GL setting since we know thats the way the texture is stored in the file
    tex.OverrideOrigin(BitmapOrigin::LowerLeft);

    GLTextureParameters texParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    // m_texLinear.Reset(tex, texParams, TextureFlags::GenerateMipMaps);
    m_texLinear.Reset(tex, texParams);
    tex.SetCompatiblePixelFormat(PixelFormat::ETC2_R8G8B8_SRGB_BLOCK);
    m_texSRGB.Reset(tex, texParams);
  }


  GLES3::GLProgram SRGBFramebuffer::CreateShader(const std::shared_ptr<IContentManager>& contentManager, const bool hasSRGBFramebuffer)
  {
    const auto fragmentShaderFile = hasSRGBFramebuffer ? "Shader.frag" : "GammaCorrection.frag";
    GLProgram program(contentManager->ReadAllText("Shader.vert"), contentManager->ReadAllText(fragmentShaderFile));

    // Get uniform locations (vertex shader)
    m_hModelViewMatrixLoc = program.GetUniformLocation("g_matModelView");
    m_hProjMatrixLoc = program.GetUniformLocation("g_matProj");

    // Get uniform locations (fragment shader)
    m_hLightPositions = program.GetUniformLocation("lightPositions");
    m_hLightColors = program.GetUniformLocation("lightColors");
    m_hViewPos = program.GetUniformLocation("viewPos");
    return program;
  }


  void SRGBFramebuffer::CreateVertexArray(const GLES3::GLProgram& program)
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

    // Set up VBO Vertex Attribute information
    GL_CHECK(glBindBuffer(m_vertexBuffer.GetTarget(), m_vertexBuffer.Get()));

    // - We assume that the vertex format is listed in the same order as the shader requires them.
    m_vertexBuffer.EnableAttribArrays(attribLink);
  }


  void SRGBFramebuffer::CreateUI(const bool hasSRGBFramebuffer)
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);

    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();

    // Create a label to write stuff into when a button is pressed

    m_labelLeft = std::make_shared<Label>(context);
    m_labelLeft->SetAlignmentX(ItemAlignment::Near);
    m_labelLeft->SetAlignmentY(ItemAlignment::Near);
    m_labelLeft->SetContent("RGB texture, gamma correction");

    m_labelRight = std::make_shared<Label>(context);
    m_labelRight->SetAlignmentX(ItemAlignment::Far);
    m_labelRight->SetAlignmentY(ItemAlignment::Near);
    m_labelRight->SetContent("SRGB texture, gamma correction (Correct)");

    auto infoArea = std::make_shared<StackLayout>(context);
    infoArea->SetLayoutOrientation(LayoutOrientation::Vertical);
    infoArea->SetAlignmentX(ItemAlignment::Center);
    infoArea->SetAlignmentY(ItemAlignment::Far);
    if (!hasSRGBFramebuffer)
    {
      auto label1 = std::make_shared<Label>(context);
      label1->SetAlignmentX(ItemAlignment::Center);
      label1->SetContent("SRGB framebuffer not available");

      auto label2 = std::make_shared<Label>(context);
      label2->SetAlignmentX(ItemAlignment::Center);
      label2->SetContent("Emulating output using shader");

      infoArea->AddChild(label1);
      infoArea->AddChild(label2);
    }
    else
    {
      auto label1 = std::make_shared<Label>(context);
      label1->SetAlignmentX(ItemAlignment::Center);
      label1->SetContent("SRGB framebuffer");

      infoArea->AddChild(label1);
    }

    // Create a 'root' layout we use the recommended fill layout as it will utilize all available space on the screen
    // We then add the 'player' stack to it and the label
    auto fillLayout = std::make_shared<FillLayout>(context);
    fillLayout->AddChild(m_labelLeft);
    fillLayout->AddChild(m_labelRight);
    fillLayout->AddChild(infoArea);


    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(fillLayout);
  }
}
