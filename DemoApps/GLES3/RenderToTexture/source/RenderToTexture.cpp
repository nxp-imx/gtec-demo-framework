/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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

#include "RenderToTexture.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr uint32_t TextureWidth = 1024;
      constexpr uint32_t TextureHeight = 1024;
      constexpr GLuint RpVertexLoc = 0;
      constexpr GLuint RpColorLoc = 1;
      constexpr GLuint MpVertexLoc = 0;
      constexpr GLuint MpTexCoordLoc = 1;
    }
    // Triangle Vertex Data
    constexpr const std::array<float, 18> g_triangleVertexPositions{0.0f,   100.0f, 0.0f, -100.0f, -100.0f, 0.0f, 100.0f, -100.0f, 0.0f,
                                                                    100.0f, 100.0f, 0.0f, -100.0f, 100.0f,  0.0f, 100.0f, -100.0,  0.0f};

    const std::array<float, 18> g_triangleVertexColors{
      1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,    // Red
      0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f     // Green
    };


    constexpr std::array<GLES3::GLBindAttribLocation, 2> g_rpShaderAttributeArray{
      GLES3::GLBindAttribLocation(LocalConfig::RpVertexLoc, "g_vPosition"), GLES3::GLBindAttribLocation(LocalConfig::RpColorLoc, "g_vColor")};

    // Cube Vertex Data
    const std::array<float, 72> g_cubeVertexPositions{
      1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
      -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f};


    const std::array<float, 48> g_cubeTexCoords{1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                                                1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                                                1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

    const std::array<GLuint, 36> g_cubeIndices{0,  1,  2,  0,  2,  3,  4,  5,  6,  4,  6,  7,  8,  9,  10, 8,  10, 11,
                                               12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};


    constexpr std::array<GLES3::GLBindAttribLocation, 2> g_mpShaderAttributeArray{
      GLES3::GLBindAttribLocation(LocalConfig::MpVertexLoc, "g_vPosition"), GLES3::GLBindAttribLocation(LocalConfig::MpTexCoordLoc, "g_vTexCoord")};
  }

  RenderToTexture::RenderToTexture(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))
    , m_angle(0.0f)
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);
    auto windowContext = m_uiExtension->GetContext();
    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
    auto& uiFactory = *uiControlFactory;

    auto leftBar = std::make_shared<UI::GridLayout>(windowContext);
    leftBar->SetAlignmentX(UI::ItemAlignment::Center);
    leftBar->SetAlignmentY(UI::ItemAlignment::Center);
    leftBar->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    leftBar->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    leftBar->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));

    m_usingExtensions = uiFactory.CreateSwitch("Use Extensions", false);
    m_usingExtensions->SetAlignmentX(UI::ItemAlignment::Stretch);
    leftBar->AddChild(m_usingExtensions, 0, 0);

    m_usingG2D = uiFactory.CreateSwitch("Use G2D", false);
    m_usingG2D->SetAlignmentX(UI::ItemAlignment::Stretch);
    leftBar->AddChild(m_usingG2D, 0, 1);

    auto background = uiFactory.CreateLeftBar(leftBar);

    // Create the root layout and add it to the window manager
    auto fillLayout = std::make_shared<UI::FillLayout>(windowContext);
    fillLayout->AddChild(background);

    // Register the root layout with the window manager
    m_uiExtension->GetWindowManager()->Add(fillLayout);

    // Initial Render Pass to Populate the texture
    PrepareTargetTexture(m_notMappedTargetTexture);
    Prepare3DFBO(m_notMappedFBO, m_notMappedTargetTexture);
#ifdef FSL_PLATFORM_YOCTO
    PrepareTargetTexture(m_mappedTargetTexture[static_cast<uint32_t>(TextureBacking::g2d)], TextureBacking::g2d);
    Prepare3DFBO(m_mappedFBO[static_cast<uint32_t>(TextureBacking::g2d)], m_mappedTargetTexture[static_cast<uint32_t>(TextureBacking::g2d)]);
    PrepareTargetTexture(m_mappedTargetTexture[static_cast<uint32_t>(TextureBacking::openGL)], TextureBacking::openGL);
    Prepare3DFBO(m_mappedFBO[static_cast<uint32_t>(TextureBacking::openGL)], m_mappedTargetTexture[static_cast<uint32_t>(TextureBacking::openGL)]);
#else
    FSLLOG3_WARNING("Mapping extensions not available on current Platform, falling back to regular texture storage");
    PrepareTargetTexture(m_mappedTargetTexture[0]);
    Prepare3DFBO(m_mappedFBO[0], m_mappedTargetTexture[0]);
    PrepareTargetTexture(m_mappedTargetTexture[1]);
    Prepare3DFBO(m_mappedFBO[1], m_mappedTargetTexture[1]);
    m_usingG2D->SetEnabled(false);
    m_usingExtensions->SetEnabled(false);
#endif
    Prepare3DRenderPassShader();

    // The Main pass will draw a cube with the FBO texture mapped to each of its faces.
    PrepareCubeShader();

    GL_CHECK(glEnable(GL_DEPTH_TEST));
  }


  // NOLINTNEXTLINE(modernize-use-equals-default)
  RenderToTexture::~RenderToTexture()
  {
#ifdef FSL_PLATFORM_YOCTO
    DestroyContiguousBuffer();
#endif
  }


  void RenderToTexture::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::LeftArrow:
      if (m_usingExtensions->IsEnabled())
      {
        m_usingExtensions->Toggle();
      }
      break;

    case VirtualKey::RightArrow:
      if (m_usingG2D->IsEnabled())
      {
        m_usingG2D->Toggle();
      }
      break;

    default:
      break;
    }
  }


  void RenderToTexture::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);
    if (m_usingExtensions->IsChecked())
    {
      if (m_usingG2D->IsChecked())
      {
        Draw3DRenderPass(m_mappedFBO[static_cast<uint32_t>(TextureBacking::g2d)]);
        DrawMainPass(m_mappedTargetTexture[static_cast<uint32_t>(TextureBacking::g2d)]);
      }
      else
      {
        Draw3DRenderPass(m_mappedFBO[static_cast<uint32_t>(TextureBacking::openGL)]);
        DrawMainPass(m_mappedTargetTexture[static_cast<uint32_t>(TextureBacking::openGL)]);
      }
    }
    else
    {
      Draw3DRenderPass(m_notMappedFBO);
      DrawMainPass(m_notMappedTargetTexture);
    }

    m_uiExtension->Draw();
  }

  void RenderToTexture::PrepareCubeShader()
  {
    auto contentManager = GetContentManager();
    m_mainProgram.Reset(contentManager->ReadAllText("finalPass.vert"), contentManager->ReadAllText("finalPass.frag"),
                        ReadOnlySpanUtil::AsSpan(g_mpShaderAttributeArray));

    m_MVPLoc = m_mainProgram.GetUniformLocation("g_matMVP");
    m_samplerLoc = m_mainProgram.GetUniformLocation("g_texSampler");
  }

  void RenderToTexture::DrawMainPass(GLuint& texture)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(m_mainProgram.Get());

    const PxSize2D currentSizePx = GetWindowSizePx();
    glViewport(0, 0, currentSizePx.Width(), currentSizePx.Height());
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_model = glm::mat4(1.0f);
    m_view = glm::lookAt(glm::vec3(2.0f, 4.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_proj =
      glm::perspective(glm::radians(39.6f), static_cast<float>(currentSizePx.Width()) / static_cast<float>(currentSizePx.Height()), 0.1f, 1000.0f);
    m_MVP = m_proj * m_view * m_model;

    glUniformMatrix4fv(m_MVPLoc, 1, 0, glm::value_ptr(m_MVP));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(m_samplerLoc, 0);

    glVertexAttribPointer(LocalConfig::MpVertexLoc, 3, GL_FLOAT, 0, 0, g_cubeVertexPositions.data());
    glEnableVertexAttribArray(LocalConfig::MpVertexLoc);

    glVertexAttribPointer(LocalConfig::MpTexCoordLoc, 2, GL_FLOAT, 0, 0, g_cubeTexCoords.data());
    glEnableVertexAttribArray(LocalConfig::MpTexCoordLoc);

    glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, g_cubeIndices.data());

    glDisableVertexAttribArray(LocalConfig::MpVertexLoc);
    glDisableVertexAttribArray(LocalConfig::MpTexCoordLoc);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
  }


  void RenderToTexture::PrepareTargetTexture(GLuint& texture)
  {
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, LocalConfig::TextureWidth, LocalConfig::TextureHeight);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void RenderToTexture::PrepareTargetTexture(GLuint& texture, const Fsl::TextureBacking& textureBackingScheme)
  {
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    MapBufferToTexture(textureBackingScheme);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void RenderToTexture::MapBufferToTexture(const Fsl::TextureBacking& textureBackingScheme)
  {
#ifdef FSL_PLATFORM_YOCTO
    if (TextureBacking::g2d == textureBackingScheme)
    {
      CreateContiguousBuffer();
      glTexDirectVIVMap(GL_TEXTURE_2D, LocalConfig::TextureWidth, LocalConfig::TextureHeight, GL_RGBA, &m_mappedBuffer.g2dBuffer->buf_vaddr,
                        (uint*)&m_mappedBuffer.g2dBuffer->buf_paddr);
      glTexDirectInvalidateVIV(GL_TEXTURE_2D);
    }
    else
    {
      glTexDirectVIV(GL_TEXTURE_2D, LocalConfig::TextureWidth, LocalConfig::TextureHeight, GL_RGBA, (GLvoid**)&m_mappedBuffer.glBuffer);
      std::memset(m_mappedBuffer.glBuffer, 0xFF, LocalConfig::TextureWidth * LocalConfig::TextureHeight * 4);
      glTexDirectInvalidateVIV(GL_TEXTURE_2D);
    }
#else
    FSL_PARAM_NOT_USED(textureBackingScheme);
#endif
  }

  void RenderToTexture::Prepare3DFBO(GLuint& fbo, GLuint& targetTexture)
  {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, targetTexture, 0);

    glGenRenderbuffers(1, &m_fboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, LocalConfig::TextureWidth, LocalConfig::TextureHeight);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);
    glDrawBuffers(1, &m_fboOutputBuffers[0]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void RenderToTexture::Prepare3DRenderPassShader()
  {
    auto contentManager = GetContentManager();
    m_renderPassProgram.Reset(contentManager->ReadAllText("renderPass.vert"), contentManager->ReadAllText("renderPass.frag"),
                              ReadOnlySpanUtil::AsSpan(g_rpShaderAttributeArray));

    m_rpMVPLoc = m_renderPassProgram.GetUniformLocation("g_matMVP");
  }

  void RenderToTexture::Draw3DRenderPass(GLuint fbo)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glUseProgram(m_renderPassProgram.Get());
    glViewport(0, 0, LocalConfig::TextureWidth, LocalConfig::TextureHeight);
    glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_rpModel = glm::mat4(1.0f);
    m_rpModel = glm::rotate(m_rpModel, glm::radians(m_angle), glm::vec3(0.0f, 1.0f, 0.0f));
    m_rpView = glm::lookAt(glm::vec3(0.0f, 0.0f, -400.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_rpProj = glm::perspective(glm::radians(39.6f), static_cast<float>(LocalConfig::TextureWidth) / static_cast<float>(LocalConfig::TextureHeight),
                                0.1f, 1000.0f);
    m_rpMVP = m_rpProj * m_rpView * m_rpModel;

    glUniformMatrix4fv(m_rpMVPLoc, 1, 0, glm::value_ptr(m_rpMVP));

    glVertexAttribPointer(LocalConfig::RpVertexLoc, 3, GL_FLOAT, 0, 0, g_triangleVertexPositions.data());
    glEnableVertexAttribArray(LocalConfig::RpVertexLoc);

    glVertexAttribPointer(LocalConfig::RpColorLoc, 4, GL_FLOAT, 0, 0, g_triangleVertexColors.data());
    glEnableVertexAttribArray(LocalConfig::RpColorLoc);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(LocalConfig::RpVertexLoc);
    glDisableVertexAttribArray(LocalConfig::RpColorLoc);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);

    m_angle += 1.0;
  }


#ifdef FSL_PLATFORM_YOCTO
  void RenderToTexture::CreateContiguousBuffer()
  {
    if (g2d_open(&m_mappedBuffer.g2dHandle) == -1 || m_mappedBuffer.g2dHandle == NULL)
    {
      throw GraphicsException("Could not initialize G2D");
    }
    m_mappedBuffer.g2dBuffer = g2d_alloc(LocalConfig::TextureWidth * LocalConfig::TextureHeight * 4, 0);
    std::memset(m_mappedBuffer.g2dBuffer->buf_vaddr, 0xFF, LocalConfig::TextureWidth * LocalConfig::TextureHeight * 4);
  }

  void RenderToTexture::DestroyContiguousBuffer()
  {
    if (m_mappedBuffer.g2dBuffer != nullptr)
    {
      g2d_free(m_mappedBuffer.g2dBuffer);
    }
    if (m_mappedBuffer.g2dHandle != nullptr)
    {
      g2d_close(m_mappedBuffer.g2dHandle);
    }
  }
#endif
}
