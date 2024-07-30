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

#include "SRGBFramebuffer.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/Layout/ComplexStackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <GLES3/gl3.h>
#include <array>
#include <cmath>
#include "SharedData.hpp"

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    constexpr Vector3 DefaultCameraPosition(4.0f, 1.0f, 7.0f);
    constexpr Vector3 DefaultCameraTarget(0.0f, 0.0f, 0.0f);

    bool IsSRGBFrameBuffer(const std::shared_ptr<ITag>& tag)
    {
      const auto userTagEx = std::dynamic_pointer_cast<SharedData>(tag);
      return userTagEx && userTagEx->SRGBFramebufferEnabled;
    }


    IO::Path GetAtlasName(const bool hasSRGBFramebuffer, const IO::PathView path)
    {
      return IO::Path::Combine(hasSRGBFramebuffer ? "Linear" : "NonLinear", path);
    }
  }


  SRGBFramebuffer::SRGBFramebuffer(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_hasSRGBFramebuffer(IsSRGBFrameBuffer(config.CustomConfig.AppRegistrationUserTag))
    , m_colorSpace(!m_hasSRGBFramebuffer ? UI::UIColorSpace::SRGBNonLinear : UI::UIColorSpace::SRGBLinear)
    , m_uiEventListener(this)    // The UI listener forwards call to 'this' object
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(),
                                                         GetAtlasName(m_hasSRGBFramebuffer, IO::PathView("UIAtlas/UIAtlas_160dpi")),
                                                         UIDemoAppExtension::CreateConfig(m_colorSpace)))    // Prepare the extension
    , m_keyboard(config.DemoServiceProvider.Get<IKeyboard>())
    , m_mouse(config.DemoServiceProvider.Get<IMouse>())
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_mouseCaptureEnabled(false)
    , m_hModelViewMatrixLoc(GLValues::InvalidLocation)
    , m_hProjMatrixLoc(GLValues::InvalidLocation)
    , m_hLightPositions(GLValues::InvalidLocation)
    , m_hLightColors(GLValues::InvalidLocation)
    , m_hViewPos(GLValues::InvalidLocation)
    , m_state(State::Split2)
    , m_splitX(TimeSpan::FromMilliseconds(400), TransitionType::Smooth)
    , m_splitSceneWidthL(TimeSpan::FromMilliseconds(400), TransitionType::Smooth)
    , m_splitSceneWidthR(TimeSpan::FromMilliseconds(400), TransitionType::Smooth)
    , m_splitSceneAlphaL(TimeSpan::FromMilliseconds(200), TransitionType::Smooth)
    , m_splitSceneAlphaR(TimeSpan::FromMilliseconds(200), TransitionType::Smooth)
  {
    m_camera.SetPosition(DefaultCameraPosition, DefaultCameraTarget, Vector3::Up());

    const auto contentManager = GetContentManager();

    PrepareTransition();
    PrepareLights();
    CreateUI();
    CreateTextures(contentManager);
    m_program = CreateShader(contentManager, m_hasSRGBFramebuffer);
    CreateVertexArray(m_program);

    UpdateUIToState();
    UpdateSceneTransition(DemoTime());
    m_splitX.ForceComplete();
    m_splitSceneWidthL.ForceComplete();
    m_splitSceneWidthR.ForceComplete();
    m_splitSceneAlphaL.ForceComplete();
    m_splitSceneAlphaR.ForceComplete();
  }


  SRGBFramebuffer::~SRGBFramebuffer() = default;


  void SRGBFramebuffer::OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_leftCB || theEvent->GetSource() == m_rightCB)
    {
      const bool isCheckedL = m_leftCB->IsChecked();
      const bool isCheckedR = m_rightCB->IsChecked();
      if (isCheckedL)
      {
        if (isCheckedR)
        {
          SetState(State::Split2);
        }
        else
        {
          SetState(State::Scene1);
        }
      }
      else if (isCheckedR)
      {
        SetState(State::Scene2);
      }
      else
      {
        // If none is checked we force update the UI
        UpdateUIToState();
      }
    }
  }


  void SRGBFramebuffer::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::Code1:
      SetState(State::Scene1);
      break;
    case VirtualKey::Code2:
      SetState(State::Scene2);
      break;
    case VirtualKey::Code3:
    case VirtualKey::Space:
      SetState(State::Split2);
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
        m_camera.SetPosition(DefaultCameraPosition, DefaultCameraTarget, Vector3::Up());
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

    const auto windowSizePx = GetWindowSizePx();
    auto matrixWorld = Matrix::GetIdentity();
    auto matrixView = m_camera.GetViewMatrix();

    const float aspectL = m_splitSceneWidthL.GetValue() / static_cast<float>(windowSizePx.RawHeight());
    m_matrixProjectionL = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspectL, 0.1f, 100.0f);
    m_matrixWorldViewL = matrixWorld * matrixView;

    const float aspectR = m_splitSceneWidthR.GetValue() / static_cast<float>(windowSizePx.RawHeight());
    m_matrixProjectionR = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspectR, 0.1f, 100.0f);
    m_matrixWorldViewR = matrixWorld * matrixView;
  }


  void SRGBFramebuffer::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

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
          const auto rawPosition = Vector2(mouseState.RawPosition.X.Value, -mouseState.RawPosition.Y.Value);
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
    const auto resPxf = TypeConverter::UncheckedTo<Vector2>(GetWindowSizePx());

    switch (m_state)
    {
    case State::Scene1:
      m_splitX.SetValue(resPxf.X);
      m_splitSceneWidthL.SetValue(resPxf.X);
      m_splitSceneWidthR.SetValue(0.0f);
      m_splitSceneAlphaL.SetValue(1.0f);
      m_splitSceneAlphaR.SetValue(0.0f);
      break;
    case State::Scene2:
      m_splitX.SetValue(0.0f);
      m_splitSceneWidthL.SetValue(0.0f);
      m_splitSceneWidthR.SetValue(resPxf.X);
      m_splitSceneAlphaL.SetValue(0.0f);
      m_splitSceneAlphaR.SetValue(1.0f);
      break;
    case State::Split2:
    default:
      m_splitX.SetValue(resPxf.X / 2.0f);
      m_splitSceneWidthL.SetValue(resPxf.X / 2.0f);
      m_splitSceneWidthR.SetValue(resPxf.X / 2.0f);
      m_splitSceneAlphaL.SetValue(1.0f);
      m_splitSceneAlphaR.SetValue(1.0f);
      break;
    }
    m_splitX.Update(TimeSpan(demoTime.ElapsedTime.Ticks()));
    m_splitSceneWidthL.Update(TimeSpan(demoTime.ElapsedTime.Ticks()));
    m_splitSceneWidthR.Update(TimeSpan(demoTime.ElapsedTime.Ticks()));
    m_splitSceneAlphaL.Update(TimeSpan(demoTime.ElapsedTime.Ticks()));
    m_splitSceneAlphaR.Update(TimeSpan(demoTime.ElapsedTime.Ticks()));

    const float alphaL = m_splitSceneAlphaL.GetValue();
    const float alphaR = m_splitSceneAlphaR.GetValue();
    m_labelLeft->SetFontColor(UI::UIColor(alphaL, alphaL, alphaL, alphaL));
    m_labelRight->SetFontColor(UI::UIColor(alphaR, alphaR, alphaR, alphaR));
  }


  void SRGBFramebuffer::DrawScenes()
  {
    const auto windowSizePx = GetWindowSizePx();

    // Set the shader program
    glUseProgram(m_program.Get());

    // Load the matrices
    assert(m_hModelViewMatrixLoc != GLValues::InvalidHandle);
    assert(m_hProjMatrixLoc != GLValues::InvalidHandle);
    assert(m_hLightPositions != GLValues::InvalidHandle);
    assert(m_hLightColors != GLValues::InvalidHandle);
    assert(m_hViewPos != GLValues::InvalidHandle);

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
    const GLint remainderX = windowSizePx.RawWidth() - splitX;

    glBindTexture(GL_TEXTURE_2D, m_texLinear.Get());


    // left (gamma correction enabled, rgb texture)
    glViewport(0, 0, splitX, windowSizePx.RawHeight());
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texSRGB.Get());

    // Bottom right (gamma correction, srgb texture)
    glUniformMatrix4fv(m_hModelViewMatrixLoc, 1, 0, m_matrixWorldViewR.DirectAccess());
    glUniformMatrix4fv(m_hProjMatrixLoc, 1, 0, m_matrixProjectionR.DirectAccess());
    glViewport(splitX, 0, remainderX, windowSizePx.RawHeight());
    // glUniform1i(m_hGamma, true);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_vertexArray.Unbind();

    // Restore the viewport
    glViewport(0, 0, windowSizePx.RawWidth(), windowSizePx.RawHeight());
  }


  void SRGBFramebuffer::PrepareTransition()
  {
    // Force set the initial value so we dont begin with a transition
    const auto resPxf = TypeConverter::UncheckedTo<Vector2>(GetWindowSizePx());
    m_splitX.SetActualValue(resPxf.X / 2.0f);
    m_splitSceneWidthL.SetActualValue(resPxf.X / 2.0f);
    m_splitSceneWidthR.SetActualValue(resPxf.X / 2.0f);
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
    const auto* const fragmentShaderFile = hasSRGBFramebuffer ? "Shader.frag" : "GammaCorrection.frag";
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
    const std::array<VertexPositionNormalTexture, 6> vertices = {
      VertexPositionNormalTexture(Vector3(x0, y, z0), normal, Vector2(u0, v0)),
      VertexPositionNormalTexture(Vector3(x0, y, z1), normal, Vector2(u0, v1)),
      VertexPositionNormalTexture(Vector3(x1, y, z1), normal, Vector2(u1, v1)),

      VertexPositionNormalTexture(Vector3(x0, y, z0), normal, Vector2(u0, v0)),
      VertexPositionNormalTexture(Vector3(x1, y, z1), normal, Vector2(u1, v1)),
      VertexPositionNormalTexture(Vector3(x1, y, z0), normal, Vector2(u1, v0)),
    };

    constexpr auto VertexDecl = VertexPositionNormalTexture::GetVertexDeclarationArray();
    std::vector<GLES3::GLVertexAttribLink> attribLink(3);
    attribLink[0] =
      GLVertexAttribLink(program.GetAttribLocation("VertexPosition"), VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    attribLink[1] = GLVertexAttribLink(program.GetAttribLocation("VertexNormal"), VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Normal, 0));
    attribLink[2] =
      GLVertexAttribLink(program.GetAttribLocation("VertexTexCoord"), VertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));


    m_vertexBuffer.Reset(vertices, GL_STATIC_DRAW);

    // Prepare the vertex arrays
    m_vertexArray.Reset(true);
    m_vertexArray.Bind();

    // Set up VBO Vertex Attribute information
    GL_CHECK(glBindBuffer(m_vertexBuffer.GetTarget(), m_vertexBuffer.Get()));

    // - We assume that the vertex format is listed in the same order as the shader requires them.
    m_vertexBuffer.EnableAttribArrays(attribLink);
  }


  void SRGBFramebuffer::CreateUI()
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);

    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();

    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension, m_colorSpace);
    auto& factory = *uiControlFactory;

    // Create a label to write stuff into when a button is pressed

    m_labelLeft = factory.CreateLabel("RGB texture, gamma correction");
    m_labelLeft->SetAlignmentX(UI::ItemAlignment::Near);
    m_labelLeft->SetAlignmentY(UI::ItemAlignment::Near);

    m_labelRight = factory.CreateLabel("SRGB texture, gamma correction (Correct)");
    m_labelRight->SetAlignmentX(UI::ItemAlignment::Far);
    m_labelRight->SetAlignmentY(UI::ItemAlignment::Near);

    auto label1 = factory.CreateLabel(m_hasSRGBFramebuffer ? "SRGB framebuffer" : "SRGB framebuffer not available. Emulating output using shader");
    label1->SetAlignmentX(UI::ItemAlignment::Center);
    label1->SetAlignmentY(UI::ItemAlignment::Center);

    auto leftCB = factory.CreateSwitch("Incorrect");
    auto rightCB = factory.CreateSwitch("Correct");
    leftCB->SetAlignmentX(UI::ItemAlignment::Center);
    rightCB->SetAlignmentX(UI::ItemAlignment::Center);
    auto controls = std::make_shared<UI::ComplexStackLayout>(context);
    controls->SetAlignmentX(UI::ItemAlignment::Stretch);
    controls->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star));
    controls->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Auto));
    controls->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star));
    controls->SetOrientation(UI::LayoutOrientation::Horizontal);
    controls->AddChild(leftCB);
    controls->AddChild(label1);
    controls->AddChild(rightCB);


    auto bottomBar = factory.CreateBottomBar(controls);

    // Create a 'root' layout we use the recommended fill layout as it will utilize all available space on the screen
    // We then add the 'player' stack to it and the label
    auto fillLayout = std::make_shared<UI::FillLayout>(context);
    fillLayout->AddChild(m_labelLeft);
    fillLayout->AddChild(m_labelRight);
    fillLayout->AddChild(bottomBar);

    m_leftCB = leftCB;
    m_rightCB = rightCB;

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(fillLayout);
  }

  void SRGBFramebuffer::SetState(State state)
  {
    if (state == m_state)
    {
      return;
    }

    m_state = state;
    UpdateUIToState();
  }

  void SRGBFramebuffer::UpdateUIToState()
  {
    switch (m_state)
    {
    case State::Scene1:
      m_leftCB->SetIsChecked(true);
      m_rightCB->SetIsChecked(false);
      break;
    case State::Scene2:
      m_leftCB->SetIsChecked(false);
      m_rightCB->SetIsChecked(true);
      break;
    case State::Split2:
      m_leftCB->SetIsChecked(true);
      m_rightCB->SetIsChecked(true);
      break;
    default:
      FSLLOG3_WARNING("Unsupported");
      break;
    }
  }

}
