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

#include "HDR03_SkyboxTonemapping.hpp"
#include <FslBase/Bits/BitsUtil.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslSimpleUI/Base/Control/Background9Slice.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <Shared/OpenGLES3/CubeMapping/TextureUtil.hpp>
#include <GLES3/gl3.h>
#include <array>

namespace Fsl
{
  using namespace GLES3;
  using namespace UI;

  namespace
  {
    const Vector3 DEFAULT_CAMERA_POSITION(0.0f, 0.0f, 0.0f);
    const Vector3 DEFAULT_CAMERA_TARGET(-4.0f, 0.0f, 0.0f);

    const float MIN_EXPOSURE = 0.0f;
    const float MAX_EXPOSURE = 10.0f;
    const float START_EXPOSURE = 1.0f;


    std::shared_ptr<UI::CheckBox> TryFindSourceCheckbox(const std::vector<std::shared_ptr<UI::CheckBox>>& checkboxes,
                                                        const std::shared_ptr<IWindowId>& source)
    {
      for (auto checkbox : checkboxes)
      {
        if (checkbox == source)
        {
          return checkbox;
        }
      }
      return std::shared_ptr<UI::CheckBox>();
    }


    std::shared_ptr<CheckBox> CreateCheckbox(const std::shared_ptr<UI::WindowContext>& context, const AtlasTexture2D& texCheckBoxC,
                                             const AtlasTexture2D& texCheckBoxU, const std::string& text)
    {
      auto checkbox = std::make_shared<CheckBox>(context);
      checkbox->SetAlignmentX(ItemAlignment::Near);
      checkbox->SetAlignmentY(ItemAlignment::Center);
      checkbox->SetText(text);
      checkbox->SetCheckedTexture(texCheckBoxC);
      checkbox->SetUncheckedTexture(texCheckBoxU);
      return checkbox;
    }
  }


  HDR03_SkyboxTonemapping::HDR03_SkyboxTonemapping(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_uiEventListener(this)    // The UI listener forwards call to 'this' object
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "MainAtlas"))    // Prepare the extension
    , m_nativeBatch(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D())
    , m_keyboard(config.DemoServiceProvider.Get<IKeyboard>())
    , m_mouse(config.DemoServiceProvider.Get<IMouse>())
    , m_demoAppControl(config.DemoServiceProvider.Get<IDemoAppControl>())
    , m_mouseCaptureEnabled(false)
    , m_exposure(START_EXPOSURE)
    , m_sceneRenderFlags(0)
    , m_rotationSpeed(5.4f, 0.0f)
  {
    const auto options = config.GetOptions<OptionParser>();

    m_camera.SetPosition(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_TARGET, Vector3::Up());

    const auto contentManager = GetContentManager();

    m_rootCanvas = CreateUI();


    PrepareScene(contentManager, m_scene);
    CreateTonemappers(m_programTonemap, contentManager);
    PrepareTransition(m_render, m_programTonemap.size());

    m_hdrFrameBuffer = CreateHdrFrameBuffer();

    m_meshQuad = CreateQuadVertexArray(m_programTonemap.front().Program);

    m_configWindow = CreateConfigDialog(m_render);
    m_rootCanvas->AddChild(m_configWindow);
    UpdateUIState();

    m_defaultFont = m_uiExtension->GetContext()->DefaultFont;

    SetStateViaFlags(options->GetSceneMask());
    ForceCompleteTransitions(m_render);
  }


  HDR03_SkyboxTonemapping::~HDR03_SkyboxTonemapping() = default;


  void HDR03_SkyboxTonemapping::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }
    switch (event.GetKey())
    {
    case VirtualKey::Code1:
      ToggleState(SceneFlags::Scene1);
      break;
    case VirtualKey::Code2:
      ToggleState(SceneFlags::Scene2);
      break;
    case VirtualKey::Code3:
      ToggleState(SceneFlags::Scene3);
      break;
    case VirtualKey::Code4:
      ToggleState(SceneFlags::Scene4);
      break;
    case VirtualKey::Code5:
      ToggleState(SceneFlags::Scene5);
      break;
    case VirtualKey::Code6:
      SetState(SceneFlags::Split, true);
      break;
    default:
      break;
    }
  }


  void HDR03_SkyboxTonemapping::OnMouseButtonEvent(const MouseButtonEvent& event)
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


  void HDR03_SkyboxTonemapping::OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    if (theEvent->IsHandled())
    {
      return;
    }

    auto source = theEvent->GetSource();

    if (source == m_exposureSlider)
    {
      m_exposure = m_exposureSlider->GetValue();
      theEvent->Handled();
    }
    else
    {
      const auto checkbox = TryFindSourceCheckbox(m_checkboxes, source);
      if (checkbox)
      {
        UpdateStateBasedOnCheckboxes(checkbox);
      }
    }
  }

  void HDR03_SkyboxTonemapping::Update(const DemoTime& demoTime)
  {
    UpdateInput(demoTime);
    UpdateSceneTransition(demoTime);

    const auto screenResolution = GetScreenResolution();
    m_matrixModel = Matrix::GetIdentity();
    m_matrixView = m_camera.GetViewMatrix();
    float aspect = static_cast<float>(screenResolution.X) / screenResolution.Y;    // ok since we divide both by two when we show four screens
    m_matrixProjection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspect, 0.1f, 100.0f);
  }


  void HDR03_SkyboxTonemapping::Draw(const DemoTime& demoTime)
  {
    const auto screenResolution = GetScreenResolution();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Render into HDR framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFrameBuffer.Get());

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DrawScene(m_scene, m_matrixView, m_matrixProjection);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_SCISSOR_TEST);

    assert(m_render.size() == m_programTonemap.size());
    GLint startX = 0;
    for (std::size_t i = 0; i < m_render.size(); ++i)
    {
      auto endX = static_cast<GLint>(m_render[i].SplitX.GetValue());
      glScissor(startX, 0, endX - startX, screenResolution.Y);
      startX = endX;
      DrawTonemappedScene(m_programTonemap[i], m_hdrFrameBuffer);
    }

    glDisable(GL_SCISSOR_TEST);

    DrawLabels();

    // Calling this last allows the UI to draw on top of everything.
    // Beware that the UI drawing methods might alter the OpenGL state!
    m_uiExtension->Draw();
  }


  void HDR03_SkyboxTonemapping::ToggleState(const SceneFlags newState)
  {
    const bool currentState = (m_sceneRenderFlags & static_cast<uint32_t>(newState)) != 0;
    SetState(newState, !currentState);
  }


  void HDR03_SkyboxTonemapping::SetState(const SceneFlags newState, const bool enabled)
  {
    const auto newStateBits = static_cast<uint32_t>(newState);
    const bool currentState = (m_sceneRenderFlags & newStateBits) == newStateBits;
    if (currentState == enabled || (!enabled && BitsUtil::Count(m_sceneRenderFlags) == 1))
    {
      return;
    }

    uint32_t sceneRenderFlags = m_sceneRenderFlags;
    if (enabled)
    {
      sceneRenderFlags |= newStateBits;
    }
    else
    {
      sceneRenderFlags &= ~newStateBits;
    }

    SetStateViaFlags(sceneRenderFlags);
  }

  void HDR03_SkyboxTonemapping::SetStateViaFlags(const uint32_t flags)
  {
    if (flags == m_sceneRenderFlags)
    {
      return;
    }
    m_sceneRenderFlags = flags;

    const auto resolution = GetScreenResolution();
    const auto activeSegments = BitsUtil::Count(m_sceneRenderFlags);

    if (activeSegments > 0)
    {
      const auto segmentSize = static_cast<float>(resolution.X) / activeSegments;
      uint32_t sceneFlags = m_sceneRenderFlags;
      float currentSplitX = segmentSize;
      for (std::size_t i = 0; i < m_render.size(); ++i)
      {
        if ((sceneFlags & 1) == 0)
        {
          m_render[i].LabelAlpha.SetValue(0.0f);
          m_render[i].SplitX.SetValue(currentSplitX - segmentSize);
        }
        else
        {
          m_render[i].LabelAlpha.SetValue(1.0f);
          m_render[i].SplitX.SetValue(currentSplitX);
          currentSplitX += segmentSize;
        }
        sceneFlags >>= 1;
      }
    }
    else
    {
      for (std::size_t i = 0; i < m_render.size(); ++i)
      {
        m_render[i].LabelAlpha.SetValue(0);
        m_render[i].SplitX.SetValue(0);
      }
    }

    UpdateUIState();
  }


  void HDR03_SkyboxTonemapping::UpdateUIState()
  {
    assert(m_render.size() == m_checkboxes.size());

    uint32_t sceneFlags = m_sceneRenderFlags;
    for (std::size_t i = 0; i < m_render.size(); ++i)
    {
      const bool enabled = (sceneFlags & 1) != 0;
      m_checkboxes[i]->SetIsChecked(enabled);
      sceneFlags >>= 1;
    }
  }


  void HDR03_SkyboxTonemapping::UpdateStateBasedOnCheckboxes(const std::shared_ptr<UI::CheckBox>& source)
  {
    if (!source)
    {
      return;
    }

    uint32_t flag = 1;
    uint32_t enabledFlags = 0;
    for (std::size_t i = 0; i < m_checkboxes.size(); ++i)
    {
      if (m_checkboxes[i]->IsChecked())
      {
        enabledFlags |= flag;
      }
      flag <<= 1;
    }
    // Cant disable all
    if (enabledFlags <= 0)
    {
      source->SetIsChecked(true);
      return;
    }

    SetStateViaFlags(enabledFlags);
  }


  void HDR03_SkyboxTonemapping::UpdateInput(const DemoTime& demoTime)
  {
    if (!m_mouseCaptureEnabled)
    {
      m_camera.RotateByRadians(m_rotationSpeed * demoTime.DeltaTime);
    }

    const auto keyboardState = m_keyboard->GetState();
    UpdateCameraControlInput(demoTime, keyboardState);
    UpdateExposureInput(demoTime, keyboardState);
  }


  void HDR03_SkyboxTonemapping::UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState)
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


  void HDR03_SkyboxTonemapping::UpdateExposureInput(const DemoTime& demoTime, const KeyboardState& keyboardState)
  {
    const float exposureAdd = 1.0f * demoTime.DeltaTime;
    const float oldExposure = m_exposure;
    if (keyboardState.IsKeyDown(VirtualKey::Q))
    {
      m_exposure -= exposureAdd;
      if (m_exposure < MIN_EXPOSURE)
      {
        m_exposure = MIN_EXPOSURE;
      }
    }
    else if (keyboardState.IsKeyDown(VirtualKey::E))
    {
      m_exposure += exposureAdd;
      if (m_exposure > MAX_EXPOSURE)
      {
        m_exposure = MAX_EXPOSURE;
      }
    }
    if (keyboardState.IsKeyDown(VirtualKey::R))
    {
      m_exposure = START_EXPOSURE;
    }
    // Update the slider on demand
    if (m_exposureSlider && m_exposure != oldExposure)
    {
      m_exposureSlider->SetValue(m_exposure);
    }
  }


  void HDR03_SkyboxTonemapping::UpdateSceneTransition(const DemoTime& demoTime)
  {
    const auto timespan = TransitionTimeSpan(demoTime.DeltaTimeInMicroseconds, TransitionTimeUnit::Microseconds);
    for (auto& rRecord : m_render)
    {
      rRecord.SplitX.Update(timespan);
      rRecord.LabelAlpha.Update(timespan);
    }
  }


  void HDR03_SkyboxTonemapping::DrawLabels()
  {
    const auto font = m_defaultFont->GetAtlasBitmapFont();
    const auto atlasTex = m_defaultFont->GetAtlasTexture();

    m_nativeBatch->Begin(BlendState::AlphaBlend);
    GLint startX = 0;
    for (std::size_t i = 0; i < m_render.size(); ++i)
    {
      const auto endX = static_cast<GLint>(m_render[i].SplitX.GetValue());
      const auto color = m_render[i].LabelAlpha.GetValue();
      if (color >= 0.001f)
      {
        const Color fontColor(color, color, color, color);
        const auto dim = font.MeasureString(m_render[i].Name.c_str());
        const auto position = static_cast<float>(((endX - startX - dim.X) / 2.0f) + startX);
        m_nativeBatch->DrawString(atlasTex, font, m_render[i].Name, Vector2(position, 0), fontColor);
      }
      startX = endX;
    }
    m_nativeBatch->End();
  }


  void HDR03_SkyboxTonemapping::DrawScene(const Scene& scene, const Matrix& matrixView, const Matrix& matrixProjection)
  {
    const auto& program = scene.Program.Program;
    const auto& location = scene.Program.Location;
    const auto& mesh = scene.Mesh;
    const auto& texture = scene.CubemapTexture;

    // Set the shader program
    glUseProgram(program.Get());

    // Load the matrices
    assert(location.ViewMatrix != GLValues::INVALID_LOCATION);
    assert(location.ProjMatrix != GLValues::INVALID_LOCATION);
    assert(location.SkyboxSampler != GLValues::INVALID_LOCATION);

    glUniformMatrix4fv(location.ViewMatrix, 1, 0, matrixView.DirectAccess());
    glUniformMatrix4fv(location.ProjMatrix, 1, 0, matrixProjection.DirectAccess());

    // Bind the vertex array
    mesh.VertexArray.Bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.Get());
    glDrawArrays(GL_TRIANGLES, 0, mesh.VertexBuffer.GetCapacity());
    // glDrawArrays(GL_TRIANGLES, 0, 6);

    mesh.VertexArray.Unbind();

    glUseProgram(0);
  }


  void HDR03_SkyboxTonemapping::DrawTonemappedScene(const TonemapProgramInfo& programInfo, const GLTextureInfo& hdrFramebufferTexInfo)
  {
    const auto& program = programInfo.Program;
    const auto& location = programInfo.Location;

    // Set the shader program
    glUseProgram(program.Get());

    // The LDR shader dont use exposure
    if (location.Exposure != GLValues::INVALID_LOCATION)
    {
      glUniform1f(location.Exposure, m_exposure);
    }

    // Bind the vertex array
    m_meshQuad.VertexArray.Bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrFramebufferTexInfo.Handle);
    glDrawArrays(GL_TRIANGLES, 0, m_meshQuad.VertexBuffer.GetCapacity());

    m_meshQuad.VertexArray.Unbind();
  }


  void HDR03_SkyboxTonemapping::PrepareTransition(std::vector<RenderRecord>& rRender, const std::size_t entries)
  {
    rRender.resize(entries);
    for (auto& rRecord : rRender)
    {
      rRecord.LabelAlpha.SetTransitionTime(m_transitionCache, TransitionTimeSpan(200, TransitionTimeUnit::Milliseconds), TransitionType::Smooth);
      rRecord.SplitX.SetTransitionTime(m_transitionCache, TransitionTimeSpan(400, TransitionTimeUnit::Milliseconds), TransitionType::Smooth);
    }
    rRender[Tonemapper::LinearNoGamma].Name = "LinearNoGamma";
    rRender[Tonemapper::Linear].Name = "Linear";
    rRender[Tonemapper::ReinhardLum].Name = "ReinhardLum";
    rRender[Tonemapper::Uncharted2Lum].Name = "Uncharted2Lum";
    rRender[Tonemapper::Custom].Name = "Custom";
  }


  void HDR03_SkyboxTonemapping::ForceCompleteTransitions(std::vector<RenderRecord>& rRender)
  {
    for (auto& rRecord : rRender)
    {
      rRecord.LabelAlpha.ForceComplete();
      rRecord.SplitX.ForceComplete();
    }
  }


  void HDR03_SkyboxTonemapping::PrepareScene(const std::shared_ptr<IContentManager>& contentManager, Scene& rScene)
  {
    FSLLOG("Preparing scene");
    FSLLOG("- loading cubemaps")
    GLTextureParameters3 texParams(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    std::string texture = "test";
    // rScene.CubemapTexture = TextureUtil::CreateCubemapTextureFromSix(contentManager, "floral_tent/1024", PixelFormat::R16G16B16A16_SFLOAT);
    rScene.CubemapTexture = TextureUtil::CreateCubemapTextureFromSix(contentManager, "lookout/1024", PixelFormat::R16G16B16A16_SFLOAT);

    FSLLOG("- loading shaders")
    rScene.Program.Reset(contentManager->ReadAllText("skybox.vert"), contentManager->ReadAllText("skybox.frag"));
    rScene.Mesh.Reset(rScene.Program.Program);
  }


  void HDR03_SkyboxTonemapping::CreateTonemappers(std::vector<TonemapProgramInfo>& rVector, const std::shared_ptr<IContentManager>& contentManager)
  {
    const std::size_t count = Tonemapper::COUNT;
    rVector.resize(count);
    for (std::size_t i = 0; i < count; ++i)
    {
      rVector[i] = CreateTonemapShader(contentManager, static_cast<Tonemapper>(i));
    }
  }


  HDR03_SkyboxTonemapping::TonemapProgramInfo HDR03_SkyboxTonemapping::CreateTonemapShader(const std::shared_ptr<IContentManager>& contentManager,
                                                                                           const Tonemapper tonemapper)
  {
    TonemapProgramInfo info;
    const auto fragmentShaderName = GetTonemapperShaderName(tonemapper);

    info.Program.Reset(contentManager->ReadAllText("Tonemapper.vert"), contentManager->ReadAllText(fragmentShaderName));

    info.Location.Exposure = info.Program.TryGetUniformLocation("g_exposure");
    return info;
  }


  const char* HDR03_SkyboxTonemapping::GetTonemapperShaderName(const Tonemapper tonemapper)
  {
    switch (tonemapper)
    {
    case Tonemapper::LinearNoGamma:
      return "TonemapperLinearNoGamma.frag";
    case Tonemapper::Linear:
      return "TonemapperLinear.frag";
    case Tonemapper::ReinhardLum:
      return "TonemapperReinhardLum.frag";
    case Tonemapper::Uncharted2Lum:
      return "TonemapperUncharted2Lum.frag";
    case Tonemapper::Custom:
      return "TonemapperCustom.frag";
    default:
      throw NotSupportedException("Unsupported tonemapper");
    }
  }


  GLES3::GLFrameBuffer HDR03_SkyboxTonemapping::CreateHdrFrameBuffer()
  {
    const auto resolution = GetScreenResolution();
    GLTextureParameters params(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    GLTextureImageParameters texImageParams(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    return GLFrameBuffer(resolution, params, texImageParams, GL_DEPTH_COMPONENT16);
  }


  HDR03_SkyboxTonemapping::SimpleMesh HDR03_SkyboxTonemapping::CreateQuadVertexArray(const GLES3::GLProgram& program)
  {
    // B D
    // |\|
    // A C
    // A = 1.0

    const float size = 1.0f;
    const float x0 = -size;
    const float x1 = size;
    const float y0 = -size;
    const float y1 = size;
    const float zPos = 0.0f;

    const float u0 = 0.0f;
    const float u1 = 1.0f;
    const float v0 = 0.0f;
    const float v1 = 1.0f;

    std::array<VertexPositionTexture, 6> vertices = {
      // Floor
      VertexPositionTexture(Vector3(x0, y1, zPos), Vector2(u0, v1)), VertexPositionTexture(Vector3(x0, y0, zPos), Vector2(u0, v0)),
      VertexPositionTexture(Vector3(x1, y0, zPos), Vector2(u1, v0)),

      VertexPositionTexture(Vector3(x0, y1, zPos), Vector2(u0, v1)), VertexPositionTexture(Vector3(x1, y0, zPos), Vector2(u1, v0)),
      VertexPositionTexture(Vector3(x1, y1, zPos), Vector2(y1, v1)),
    };

    auto vertexDecl = VertexPositionTexture::GetVertexDeclaration();
    std::vector<GLES3::GLVertexAttribLink> attribLink(2);
    attribLink[0] =
      GLVertexAttribLink(program.GetAttribLocation("VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    attribLink[1] =
      GLVertexAttribLink(program.GetAttribLocation("VertexTexCoord"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));

    GLVertexBuffer vertexBuffer(vertices.data(), vertices.size(), GL_STATIC_DRAW);

    // Prepare the vertex arrays
    GLVertexArray vertexArray(true);
    vertexArray.Bind();
    {
      // Set up VBO Vertex Attribute information
      GL_CHECK(glBindBuffer(vertexBuffer.GetTarget(), vertexBuffer.Get()));
      vertexBuffer.EnableAttribArrays(attribLink);
    }
    vertexArray.Unbind();
    return SimpleMesh(std::move(vertexBuffer), std::move(vertexArray));
  }

  std::shared_ptr<CanvasLayout> HDR03_SkyboxTonemapping::CreateUI()
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);

    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();

    // Create a 'root' layout we use the recommended fill layout as it will utilize all available space on the screen
    // We then add the 'player' stack to it and the label
    auto rootLayout = std::make_shared<CanvasLayout>(context);
    rootLayout->SetAlignmentX(ItemAlignment::Stretch);
    rootLayout->SetAlignmentY(ItemAlignment::Stretch);

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(rootLayout);
    return rootLayout;
  }

  std::shared_ptr<BaseWindow> HDR03_SkyboxTonemapping::CreateConfigDialog(const std::vector<RenderRecord>& render)
  {
    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();

    auto texCheckBoxC = m_uiExtension->GetAtlasTexture2D("CheckBoxC");
    auto texCheckBoxU = m_uiExtension->GetAtlasTexture2D("CheckBoxU");

    m_checkboxes.resize(render.size());
    for (std::size_t i = 0; i < m_checkboxes.size(); ++i)
    {
      m_checkboxes[i] = CreateCheckbox(context, texCheckBoxC, texCheckBoxU, render[i].Name);
    }

    auto texSlider = m_uiExtension->GetAtlasTexture2D("Slider");
    auto texSliderCursor = m_uiExtension->GetAtlasTexture2D("SliderCursor");
    NineSlice sliderNineSlice(13, 0, 13, 0);
    ThicknessF sliderCursorPadding(13, 0, 13, 0);

    auto labelExposure = std::make_shared<Label>(context);
    labelExposure->SetAlignmentX(ItemAlignment::Near);
    labelExposure->SetAlignmentY(ItemAlignment::Center);
    labelExposure->SetContent("Exposure");

    m_exposureSlider = std::make_shared<FloatSliderAndValueLabel>(context);
    m_exposureSlider->SetAlignmentX(ItemAlignment::Stretch);
    m_exposureSlider->SetBackgroundTexture(texSlider);
    m_exposureSlider->SetCursorTexture(texSliderCursor);
    m_exposureSlider->SetCursorPadding(sliderCursorPadding);
    m_exposureSlider->SetNineSlice(sliderNineSlice);
    m_exposureSlider->SetValueLimits(MIN_EXPOSURE, MAX_EXPOSURE);
    m_exposureSlider->SetValue(m_exposure);

    auto stackLayout = std::make_shared<StackLayout>(context);
    for (std::size_t i = 0; i < m_checkboxes.size(); ++i)
    {
      stackLayout->AddChild(m_checkboxes[i]);
    }
    stackLayout->AddChild(labelExposure);
    stackLayout->AddChild(m_exposureSlider);

    AtlasTexture2D texBackground = m_uiExtension->GetAtlasTexture2D("Background9R");

    auto window = std::make_shared<Background9Slice>(context);
    window->SetAlignmentY(ItemAlignment::Far);
    window->SetBackground(texBackground);
    window->SetNineSlice(NineSlice(32, 32, 32, 32));
    window->SetPadding(ThicknessF(32, 32, 32, 32));
    window->SetContent(stackLayout);
    return window;
  }
}
