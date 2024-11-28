/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include "AntiAliasing.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxRectangle.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslDemoApp/Base/FrameInfo.hpp>
#include <FslDemoHost/EGL/Config/Service/IEGLHostInfo.hpp>
#include <FslGraphics/NativeTextureAreaUtilEx.hpp>
#include <FslGraphics/Vertices/VertexPosition2.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslUtil/OpenGLES3/GLUtil.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <GLES3/gl3.h>
#include <array>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr IO::PathView CarModelPath("Models/FuturisticCar/Futuristic_Car.3ds");
      constexpr IO::PathView CarModelShaderPath("");
      constexpr IO::PathView CarModelTextureDiffuse("Models/FuturisticCar/Futuristic_Car_C.jpg");
      constexpr IO::PathView CarModelTextureSpecular("Models/FuturisticCar/Futuristic_Car_S.jpg");
      constexpr IO::PathView CarModelTextureNormal("Models/FuturisticCar/Futuristic_Car_N.jpg");

      constexpr IO::PathView KnightModelPath("Models/Knight2/armor.obj");
      constexpr IO::PathView KnightModelShaderPath("");
      constexpr IO::PathView KnightModelTextureDiffuse("Models/Knight2/armor_default_color.jpg");
      constexpr IO::PathView KnightModelTextureSpecular("Models/Knight2/armor_default_metalness.jpg");
      constexpr IO::PathView KnightModelTextureNormal("Models/Knight2/armor_default_nmap.jpg");

      constexpr float CarModelScale = 1.0f;
      constexpr float KnightModelScale = 0.75f;
    }

    constexpr std::array<VertexPosition2, 3> PostProcessVertices = {
      VertexPosition2(-1.0f, -1.0f),    // Bottom-left
      VertexPosition2(3.0f, -1.0f),     // Bottom-right (overdraws)
      VertexPosition2(-1.0f, 3.0f)      // Top-left (overdraws)
    };


    constexpr SupportedSampleCountFlags ToSupportedSampleCountFlags(const int32_t maxMSAASamples) noexcept
    {
      SupportedSampleCountFlags res = SupportedSampleCountFlags::NoFlags;
      res |= maxMSAASamples >= 1 ? SupportedSampleCountFlags::Samples1 : SupportedSampleCountFlags::NoFlags;
      res |= maxMSAASamples >= 2 ? SupportedSampleCountFlags::Samples2 : SupportedSampleCountFlags::NoFlags;
      res |= maxMSAASamples >= 4 ? SupportedSampleCountFlags::Samples4 : SupportedSampleCountFlags::NoFlags;
      res |= maxMSAASamples >= 8 ? SupportedSampleCountFlags::Samples8 : SupportedSampleCountFlags::NoFlags;
      res |= maxMSAASamples >= 16 ? SupportedSampleCountFlags::Samples16 : SupportedSampleCountFlags::NoFlags;
      res |= maxMSAASamples >= 32 ? SupportedSampleCountFlags::Samples32 : SupportedSampleCountFlags::NoFlags;
      res |= maxMSAASamples >= 64 ? SupportedSampleCountFlags::Samples64 : SupportedSampleCountFlags::NoFlags;
      return res;
    }


    AntiAliasingConfig CreateConfig() noexcept
    {
      const int32_t maxMSAASamples = GLES3::GLUtil::Get<GLint>(GL_MAX_SAMPLES);
      FSLLOG3_INFO("GL_MAX_SAMPLES: {}", maxMSAASamples);
      return AntiAliasingConfig(ToSupportedSampleCountFlags(maxMSAASamples));
    }

    NativeTextureArea ApplyZoomFactor(const NativeTextureArea& texCoord, const UI::Custom::ZoomInfo& zoomInfo, const float zoomFactor)
    {
      const PxSize1D deltaXPx = zoomInfo.WindowAreaPx.Width();
      const PxSize1D deltaYPx = zoomInfo.WindowAreaPx.Height();
      const PxValue relativeXPx = zoomInfo.WindowMousePositionPx.X - zoomInfo.WindowAreaPx.Left();
      const PxValue relativeYPx = zoomInfo.WindowMousePositionPx.Y - zoomInfo.WindowAreaPx.Top();

      const float percentageX = MathHelper::Clamp(static_cast<float>(relativeXPx.Value) / static_cast<float>(deltaXPx.RawValue()), 0.0f, 1.0f);
      const float percentageY = MathHelper::Clamp(static_cast<float>(relativeYPx.Value) / static_cast<float>(deltaYPx.RawValue()), 0.0f, 1.0f);

      const float du = texCoord.X1 - texCoord.X0;
      const float dv = texCoord.Y1 - texCoord.Y0;
      const float cursorDeltaU = (du * percentageX);
      const float cursorDeltaV = (dv * percentageY);
      const float cursorU = texCoord.X0 + cursorDeltaU;
      const float cursorV = texCoord.Y0 + cursorDeltaV;

      const float lenU0 = (cursorDeltaU) / zoomFactor;
      const float lenV0 = (cursorDeltaV) / zoomFactor;
      const float lenU1 = (du - cursorDeltaU) / zoomFactor;
      const float lenV1 = (dv - cursorDeltaV) / zoomFactor;

      return {cursorU - lenU0, cursorV - lenV0, cursorU + lenU1, cursorV + lenV1};
    }


    Matrix CreateMatWorldViewProjection(const PxSize2D sizePx)
    {
      const auto screenWidth = static_cast<float>(sizePx.RawWidth());
      const auto screenHeight = static_cast<float>(sizePx.RawHeight());
      const float screenOffsetX = screenWidth / 2.0f;
      const float screenOffsetY = screenHeight / 2.0f;

      return Matrix::CreateTranslation(-screenOffsetX, -screenOffsetY, 1.0f) * Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
             Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);
    }


    CustomUIConfig CreateCustomUIConfig()
    {
      const bool isExtAvailable = GLES3::GLUtil::HasExtension("GL_EXT_multisampled_render_to_texture");
      return CustomUIConfig(true, isExtAvailable, false);
    }
  }


  AntiAliasing::AntiAliasing(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_shared(config, CreateConfig(), CreateCustomUIConfig())
    , m_renderModelCar(*GetContentManager(), LocalConfig::CarModelPath, LocalConfig::CarModelShaderPath, LocalConfig::CarModelTextureDiffuse,
                       LocalConfig::CarModelTextureSpecular, LocalConfig::CarModelTextureNormal, LocalConfig::CarModelScale)
    , m_renderModelKnight(*GetContentManager(), LocalConfig::KnightModelPath, LocalConfig::KnightModelShaderPath,
                          LocalConfig::KnightModelTextureDiffuse, LocalConfig::KnightModelTextureSpecular, LocalConfig::KnightModelTextureNormal,
                          LocalConfig::KnightModelScale)
  {
    const int32_t maxRenderBufferSize = GLES3::GLUtil::Get<GLint>(GL_MAX_RENDERBUFFER_SIZE);
    FSLLOG3_INFO("MaxRenderBufferSize: {}", maxRenderBufferSize);

    if (maxRenderBufferSize < config.WindowMetrics.GetSizePx().RawWidth() || maxRenderBufferSize < config.WindowMetrics.GetSizePx().RawHeight())
    {
      throw NotSupportedException("This samples requires that the MaxRenderBufferSize can contain a full-screen sized buffer");
    }

    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_shared.GetUIDemoAppExtension());

    const auto contentManager = GetContentManager();
    m_resources.Offscreen =
      CreateFramebufferResources(config.WindowMetrics.GetSizePx(), m_shared.GetActiveAntiAliasingMethod(), m_shared.UseMSAAFastResolve());
    m_resources.Line.ProgramInfo = CreateLineProgram(contentManager);
    m_resources.DividerLine.ProgramInfo = CreateLineProgram(contentManager);
    m_resources.PostProcess = CreatePostProcessResources(contentManager);
    m_resources.Zoom = CreateZoomResources(contentManager, config.WindowMetrics);
  }


  AntiAliasing::~AntiAliasing() = default;


  void AntiAliasing::OnKeyEvent(const KeyEvent& event)
  {
    base_type::OnKeyEvent(event);

    m_shared.OnKeyEvent(event);
  }


  void AntiAliasing::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    base_type::ConfigurationChanged(windowMetrics);
    m_shared.OnConfigurationChanged(windowMetrics);
    RecreateFrameBufferIfNecessary();
  }


  void AntiAliasing::Update(const DemoTime& demoTime)
  {
    base_type::Update(demoTime);

    m_shared.Update(demoTime);
    m_renderModelCar.Update(demoTime, m_shared.IsPaused());
    m_renderModelKnight.Update(demoTime, m_shared.IsPaused());

    m_linesVertexUboData.MatViewProjection = m_shared.GetViewMatrix() * m_shared.GetProjectionMatrix();
    m_zoomVertexUboData.MatWorldViewProjection = CreateMatWorldViewProjection(GetWindowMetrics().GetSizePx());

    RecreateFrameBufferIfNecessary();
  }


  void AntiAliasing::Resolve(const DemoTime& demoTime)
  {
    base_type::Resolve(demoTime);
    m_shared.Resolve(demoTime);
  }


  void AntiAliasing::Draw(const FrameInfo& frameInfo)
  {
    base_type::Draw(frameInfo);

    DrawToMultisampleFB(frameInfo.Time);

    const auto zoomInfo = m_shared.GetResolvedZoomInfo();

    const bool useCustomZoom = m_resources.Offscreen.ScalePx != PxSize1D::Create(1);
    if (useCustomZoom)
    {
      DrawToZoomFB(zoomInfo, m_resources.Offscreen.ZoomFramebuffer, m_resources.Zoom, m_resources.Offscreen.GetCustomZoomFB(),
                   m_resources.Offscreen.ScalePx);
    }

    DrawFinalComposite(zoomInfo, GetWindowMetrics().GetSizePx(), useCustomZoom);

    // Allow the shared part to draw as well
    m_shared.Draw(frameInfo.Time);
  }


  GLES3::GLFrameBuffer AntiAliasing::CreateNoAAFramebuffer(const PxSize2D sizePx)
  {
    GLES3::GLTextureParameters texParams;
    GLES3::GLTextureImageParameters texImageParams(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);

    GLES3::GLFrameBuffer framebuffer;
    framebuffer.Reset(sizePx, texParams, texImageParams, GL_DEPTH_COMPONENT16);
    return framebuffer;
  }


  GLES3::GLFrameBuffer AntiAliasing::CreateMSAAResolveFramebuffer(const PxSize2D sizePx)
  {
    GLES3::GLTextureParameters texParams;
    GLES3::GLTextureImageParameters texImageParams(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);

    GLES3::GLFrameBuffer framebuffer;
    framebuffer.Reset(sizePx, texParams, texImageParams);
    return framebuffer;
  }


  GLES3::GLMultisampleFrameBuffer AntiAliasing::CreateMSAAFramebuffer(const PxSize2D sizePx, const GLsizei maxSamples)
  {
    GLES3::GLMultisampleFrameBuffer framebuffer;
    framebuffer.Reset(sizePx, maxSamples, GL_RGB8, GL_DEPTH_COMPONENT16);
    return framebuffer;
  }


  GLES3::GLMultisampleFrameBufferTextureExt AntiAliasing::CreateMSAAFastResolveFramebuffer(const PxSize2D sizePx, const int32_t maxSamples)
  {
    GLES3::GLMultisampleFrameBufferTextureExt framebuffer;
    GLES3::GLTextureImageParameters texImageParams(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
    framebuffer.Reset(sizePx, maxSamples, texImageParams, GL_DEPTH_COMPONENT16);
    return framebuffer;
  }

  GLES3::GLFrameBuffer AntiAliasing::CreateSSAAFramebuffer(const PxSize2D sizePx, const int32_t maxSamples)
  {
    GLES3::GLTextureParameters texParams(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    GLES3::GLTextureImageParameters texImageParams(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);

    const PxSize2D scaledSizePx(sizePx * PxSize2D::Create(maxSamples, maxSamples));

    GLES3::GLFrameBuffer framebuffer;
    framebuffer.Reset(scaledSizePx, texParams, texImageParams, GL_DEPTH_COMPONENT16);
    return framebuffer;
  }


  AntiAliasing::FrameBufferResources AntiAliasing::CreateFramebufferResources(const PxSize2D sizePx, const AntiAliasingMethod aaMethod,
                                                                              const bool useFastResolve)
  {
    switch (aaMethod)
    {
    case AntiAliasingMethod::Disabled:
      return {aaMethod, sizePx, CreateNoAAFramebuffer(sizePx)};
    case AntiAliasingMethod::MSAA2X:
      if (!useFastResolve)
      {
        return {aaMethod, sizePx, CreateMSAAResolveFramebuffer(sizePx), CreateMSAAFramebuffer(sizePx, 2)};
      }
      return {aaMethod, sizePx, CreateMSAAFastResolveFramebuffer(sizePx, 2)};
    case AntiAliasingMethod::MSAA4X:
      if (!useFastResolve)
      {
        return {aaMethod, sizePx, CreateMSAAResolveFramebuffer(sizePx), CreateMSAAFramebuffer(sizePx, 4)};
      }
      return {aaMethod, sizePx, CreateMSAAFastResolveFramebuffer(sizePx, 4)};
    case AntiAliasingMethod::MSAA8X:
      if (!useFastResolve)
      {
        return {aaMethod, sizePx, CreateMSAAResolveFramebuffer(sizePx), CreateMSAAFramebuffer(sizePx, 8)};
      }
      return {aaMethod, sizePx, CreateMSAAFastResolveFramebuffer(sizePx, 8)};
    case AntiAliasingMethod::SSAA2X:
      return {aaMethod, sizePx, CreateSSAAFramebuffer(sizePx, 2), PxSize1D::Create(2)};
    }
    throw NotSupportedException("The requested AA method was unsupported");
  }

  AntiAliasing::LineProgramInfo AntiAliasing::CreateLineProgram(const std::shared_ptr<IContentManager>& contentManager)
  {
    constexpr auto VertexDecl = Graphics3D::LineBuilder::vertex_type::GetVertexDeclarationArray();
    LineProgramInfo info;
    info.Program.Reset(contentManager->ReadAllText("VertexColorLine.vert"), contentManager->ReadAllText("VertexColorLine.frag"));
    info.LocWorldViewProjection = info.Program.GetUniformLocation("WorldViewProjection");
    info.Links[0] = GLES3::GLVertexAttribLink(info.Program.GetAttribLocation("VertexPosition"),
                                              VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u));
    info.Links[1] =
      GLES3::GLVertexAttribLink(info.Program.GetAttribLocation("VertexColor"), VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0u));
    return info;
  }


  AntiAliasing::PostProcessProgramInfo AntiAliasing::CreatePostProcessProgram(const std::shared_ptr<IContentManager>& contentManager)
  {
    constexpr auto VertexDecl = VertexPosition2::GetVertexDeclarationArray();
    PostProcessProgramInfo info;
    info.Program.Reset(contentManager->ReadAllText("FullscreenTriangle.vert"), contentManager->ReadAllText("FullscreenTriangle.frag"));
    info.LocTexture = info.Program.GetUniformLocation("u_texture");
    info.Links[0] = GLES3::GLVertexAttribLink(info.Program.GetAttribLocation("VertexPosition"),
                                              VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u));
    return info;
  }


  AntiAliasing::PostProcessResources AntiAliasing::CreatePostProcessResources(const std::shared_ptr<IContentManager>& contentManager)
  {
    GLES3::GLVertexBuffer vertexBuffer(PostProcessVertices, GL_STATIC_DRAW);
    return {CreatePostProcessProgram(contentManager), std::move(vertexBuffer)};
  }


  AntiAliasing::ZoomProgramInfo AntiAliasing::CreateZoomProgram(const std::shared_ptr<IContentManager>& contentManager)
  {
    constexpr auto VertexDecl = VertexPositionTexture::GetVertexDeclarationArray();
    ZoomProgramInfo info;
    info.Program.Reset(contentManager->ReadAllText("Zoom.vert"), contentManager->ReadAllText("Zoom.frag"));
    info.LocWorldViewProjection = info.Program.GetUniformLocation("WorldViewProjection");
    info.LocTexture = info.Program.GetUniformLocation("u_texture");
    info.Links[0] = GLES3::GLVertexAttribLink(info.Program.GetAttribLocation("VertexPosition"),
                                              VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u));
    info.Links[1] = GLES3::GLVertexAttribLink(info.Program.GetAttribLocation("VertexTexCoord"),
                                              VertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0u));
    return info;
  }

  AntiAliasing::ZoomResources AntiAliasing::CreateZoomResources(const std::shared_ptr<IContentManager>& contentManager,
                                                                const DemoWindowMetrics& windowMetrics)
  {
    constexpr std::array<VertexPositionTexture, 6> Vertices = {};
    GLES3::GLVertexBuffer vertexBuffer(Vertices, GL_DYNAMIC_DRAW);
    GLES3::GLVertexBuffer vertexBufferCustomZoom(Vertices, GL_DYNAMIC_DRAW);
    return {CreateZoomProgram(contentManager), std::move(vertexBuffer), std::move(vertexBufferCustomZoom)};
  }


  void AntiAliasing::RecreateFrameBufferIfNecessary()
  {
    auto& rSceneFB = m_resources.Offscreen;

    const auto currentSizePx = GetWindowMetrics().GetSizePx();
    const auto activeAntiAliasingMethod = m_shared.GetActiveAntiAliasingMethod();
    if (activeAntiAliasingMethod != rSceneFB.ActiveAntiAliasingMethod || rSceneFB.GetSize() != currentSizePx ||
        m_shared.UseMSAAFastResolve() != rSceneFB.IsUsingFastResolve())
    {
      rSceneFB.Reset();
      rSceneFB = CreateFramebufferResources(currentSizePx, activeAntiAliasingMethod, m_shared.UseMSAAFastResolve());
    }
  }


  PxSize2D AntiAliasing::ApplyViewport(const ContentWindowId contentWindowId)
  {
    const PxSize2D fullSizePx = m_resources.Offscreen.ScaledSizePx;
    const PxRectangle windowPx = m_shared.GetContentWindowRectangleInScreenspace(contentWindowId) * m_resources.Offscreen.ScalePx;

    // Convert rect to GL
    const int32_t convertedTop = fullSizePx.RawHeight() - (windowPx.RawTop() + windowPx.RawHeight());

    glViewport(windowPx.RawLeft(), convertedTop, windowPx.RawWidth(), windowPx.RawHeight());
    return windowPx.GetSize();
  }


  void AntiAliasing::RenderLines(LineDrawResources& rLine, const PxSize1D scalePx, const Fsl::Graphics3D::LineBuilder& lineBuilder,
                                 const Matrix& matrixWorldViewProjection)
  {
    glUseProgram(rLine.ProgramInfo.Program.Get());
    glUniformMatrix4fv(rLine.ProgramInfo.LocWorldViewProjection, 1, 0, matrixWorldViewProjection.DirectAccess());
    rLine.Render.Draw(lineBuilder.GetVertexSpan(), rLine.ProgramInfo.Links);
  }


  void AntiAliasing::GenerateCustomZoomVertices(GLES3::GLVertexBuffer& rVertexBuffer, const PxSize2D textureSizePx, const PxSize1D texScalePx,
                                                const UI::Custom::ZoomInfo& zoomInfo)
  {
    std::array<VertexPositionTexture, 6> vertices = {};

    const auto x0 = 0.0f;
    const auto x1 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawWidth());
    const auto y0 = 0.0f;
    const auto y1 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawHeight());
    const float zpos = 0.0f;

    const NativeTextureArea texCoord =
      NativeTextureAreaUtil::CalcNativeTextureAreaYFlippedClipped(zoomInfo.ScreenSpaceAreaPx * texScalePx, textureSizePx);

    vertices[0] = VertexPositionTexture(Vector3(x0, y1, zpos), Vector2(texCoord.X0, texCoord.Y1));
    vertices[1] = VertexPositionTexture(Vector3(x0, y0, zpos), Vector2(texCoord.X0, texCoord.Y0));
    vertices[2] = VertexPositionTexture(Vector3(x1, y0, zpos), Vector2(texCoord.X1, texCoord.Y0));

    vertices[3] = VertexPositionTexture(Vector3(x1, y0, zpos), Vector2(texCoord.X1, texCoord.Y0));
    vertices[4] = VertexPositionTexture(Vector3(x1, y1, zpos), Vector2(texCoord.X1, texCoord.Y1));
    vertices[5] = VertexPositionTexture(Vector3(x0, y1, zpos), Vector2(texCoord.X0, texCoord.Y1));

    rVertexBuffer.SetData(0, SpanUtil::AsReadOnlySpan(vertices));
  }


  void AntiAliasing::GenerateCustomZoomVertices2(GLES3::GLVertexBuffer& rVertexBuffer, const PxSize2D textureSizePx,
                                                 const UI::Custom::ZoomInfo& zoomInfo)
  {
    std::array<VertexPositionTexture, 6> vertices = {};

    const auto x0 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawLeft());
    const auto x1 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawRight());
    const auto y0 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawTop());
    const auto y1 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawBottom());
    const float zpos = 0.0f;

    const PxRectangle rectPx(PxPoint2(), zoomInfo.ScreenSpaceAreaPx.GetSize());
    // const PxRectangle rectPx(PxPoint2(), textureSizePx);
    NativeTextureArea texCoord = NativeTextureAreaUtil::CalcNativeTextureAreaYFlippedClipped(rectPx, textureSizePx);
    texCoord = ApplyZoomFactor(texCoord, zoomInfo, zoomInfo.ZoomFactor);

    vertices[0] = VertexPositionTexture(Vector3(x0, y1, zpos), Vector2(texCoord.X0, texCoord.Y1));
    vertices[1] = VertexPositionTexture(Vector3(x0, y0, zpos), Vector2(texCoord.X0, texCoord.Y0));
    vertices[2] = VertexPositionTexture(Vector3(x1, y0, zpos), Vector2(texCoord.X1, texCoord.Y0));

    vertices[3] = VertexPositionTexture(Vector3(x1, y0, zpos), Vector2(texCoord.X1, texCoord.Y0));
    vertices[4] = VertexPositionTexture(Vector3(x1, y1, zpos), Vector2(texCoord.X1, texCoord.Y1));
    vertices[5] = VertexPositionTexture(Vector3(x0, y1, zpos), Vector2(texCoord.X0, texCoord.Y1));

    rVertexBuffer.SetData(0, SpanUtil::AsReadOnlySpan(vertices));
  }


  void AntiAliasing::GenerateZoomVertices(GLES3::GLVertexBuffer& rVertexBuffer, const PxSize2D textureSizePx, const PxSize1D texScalePx,
                                          const UI::Custom::ZoomInfo& zoomInfo)
  {
    std::array<VertexPositionTexture, 6> vertices = {};

    const auto x0 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawLeft());
    const auto x1 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawRight());
    const auto y0 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawTop());
    const auto y1 = static_cast<float>(zoomInfo.ScreenSpaceAreaPx.RawBottom());
    const float zpos = 0.0f;

    NativeTextureArea texCoord = NativeTextureAreaUtil::CalcNativeTextureAreaYFlippedClipped(zoomInfo.ScreenSpaceAreaPx * texScalePx, textureSizePx);
    texCoord = ApplyZoomFactor(texCoord, zoomInfo, zoomInfo.ZoomFactor);

    vertices[0] = VertexPositionTexture(Vector3(x0, y1, zpos), Vector2(texCoord.X0, texCoord.Y1));
    vertices[1] = VertexPositionTexture(Vector3(x0, y0, zpos), Vector2(texCoord.X0, texCoord.Y0));
    vertices[2] = VertexPositionTexture(Vector3(x1, y0, zpos), Vector2(texCoord.X1, texCoord.Y0));

    vertices[3] = VertexPositionTexture(Vector3(x1, y0, zpos), Vector2(texCoord.X1, texCoord.Y0));
    vertices[4] = VertexPositionTexture(Vector3(x1, y1, zpos), Vector2(texCoord.X1, texCoord.Y1));
    vertices[5] = VertexPositionTexture(Vector3(x0, y1, zpos), Vector2(texCoord.X0, texCoord.Y1));

    rVertexBuffer.SetData(0, SpanUtil::AsReadOnlySpan(vertices));
  }


  void AntiAliasing::DrawToMultisampleFB(const DemoTime& demoTime)
  {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    {    // Render to the framebuffer
      auto& rLine = m_resources.Line;
      auto& rDividerLine = m_resources.DividerLine;
      const auto& scene = m_resources.Offscreen;

      const GLuint hFB = scene.GetFrameBufferHandle();
      const PxSize2D fbSizePx = scene.GetFrameBufferSize();

      glBindFramebuffer(GL_FRAMEBUFFER, hFB);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      auto windowSizePx = ApplyViewport(ContentWindowId::Car);

      m_renderModelCar.Draw(demoTime, windowSizePx);

      windowSizePx = ApplyViewport(ContentWindowId::Knight);
      m_renderModelKnight.Draw(demoTime, windowSizePx);

      ApplyViewport(ContentWindowId::WireframeCube);
      RenderLines(rLine, scene.ScalePx, m_shared.GetLineBuilder(), m_linesVertexUboData.MatViewProjection);

      glViewport(0, 0, fbSizePx.RawWidth(), fbSizePx.RawHeight());
      const auto dividerLinesMatrix = CreateMatWorldViewProjection(fbSizePx);
      RenderLines(rDividerLine, scene.ScalePx, m_shared.GetDividerLineBuilder(), dividerLinesMatrix);

      if (scene.ResolvedRequired())
      {
        ResolveFrameBuffer(scene.GetResolveFBHandle(), scene.GetResolveFBSize(), hFB, fbSizePx);
      }

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
  }


  void AntiAliasing::DrawToZoomFB(const UI::Custom::ZoomInfo& zoomInfo, GLES3::GLFrameBuffer& rZoomFramebuffer, ZoomResources& rZoomResources,
                                  const GLES3::GLFrameBuffer& srcFramebuffer, const PxSize1D srcTexScalePx)
  {
    const PxSize2D zoomAreaSizePx = zoomInfo.MaxSizePx;
    if (!rZoomFramebuffer.IsValid())
    {
      rZoomFramebuffer = CreateMSAAResolveFramebuffer(zoomAreaSizePx);
    }
    else if (zoomAreaSizePx.Width() > rZoomFramebuffer.GetSize().Width() || zoomAreaSizePx.Height() > rZoomFramebuffer.GetSize().Height())
    {
      rZoomFramebuffer.Reset();
      rZoomFramebuffer = CreateMSAAResolveFramebuffer(zoomAreaSizePx);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, rZoomFramebuffer.Get());
    glViewport(0, rZoomFramebuffer.GetSize().RawHeight() - zoomAreaSizePx.RawHeight(), zoomAreaSizePx.RawWidth(), zoomAreaSizePx.RawHeight());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto& rVB = rZoomResources.VertexBufferCustomZoom;
    GenerateCustomZoomVertices(rVB, srcFramebuffer.GetSize(), srcTexScalePx, zoomInfo);

    const Matrix matWorldViewProjection = CreateMatWorldViewProjection(zoomAreaSizePx);
    DrawZoomArea(srcFramebuffer.GetTextureInfo().Handle, rZoomResources.ProgramInfo, rVB, matWorldViewProjection, true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }


  void AntiAliasing::DrawFinalComposite(const UI::Custom::ZoomInfo& zoomInfo, const PxSize2D windowSizePx, const bool useCustomZoom)
  {
    glViewport(0, 0, windowSizePx.RawWidth(), windowSizePx.RawHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the off-screen framebuffer to the screen
    DrawFrameBuffer(m_resources.PostProcess, m_resources.Offscreen.GetTextureInfo().Handle);

    if (zoomInfo.Enabled)
    {
      if (!useCustomZoom)
      {
        const auto& textureInfo = m_resources.Offscreen.GetTextureInfo();
        GenerateZoomVertices(m_resources.Zoom.VertexBuffer, m_resources.Offscreen.GetTextureSize(), m_resources.Offscreen.ScalePx, zoomInfo);
        DrawZoomArea(textureInfo.Handle, m_resources.Zoom.ProgramInfo, m_resources.Zoom.VertexBuffer, m_zoomVertexUboData.MatWorldViewProjection);
      }
      else
      {
        const auto& textureFB = m_resources.Offscreen.ZoomFramebuffer;
        GenerateCustomZoomVertices2(m_resources.Zoom.VertexBuffer, textureFB.GetSize(), zoomInfo);
        DrawZoomArea(textureFB.GetTextureInfo().Handle, m_resources.Zoom.ProgramInfo, m_resources.Zoom.VertexBuffer,
                     m_zoomVertexUboData.MatWorldViewProjection);
      }
    }
  }


  void AntiAliasing::ResolveFrameBuffer(const GLuint hDst, const PxSize2D dstSizePx, const GLuint hSrc, const PxSize2D srcSizePx)
  {
    // Bind the MSAA framebuffer for reading (source)
    glBindFramebuffer(GL_READ_FRAMEBUFFER, hSrc);
    GL_CHECK_FOR_ERROR();

    // Bind the resolved framebuffer for drawing (destination)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hDst);
    GL_CHECK_FOR_ERROR();

    // Blit (resolve) the multi-sampled framebuffer into the single-sample framebuffer
    glBlitFramebuffer(0, 0, srcSizePx.RawWidth(), srcSizePx.RawHeight(), 0, 0, dstSizePx.RawWidth(), dstSizePx.RawHeight(), GL_COLOR_BUFFER_BIT,
                      GL_NEAREST);
  }


  void AntiAliasing::DrawFrameBuffer(const PostProcessResources& resources, const GLuint hTexture)
  {
    glUseProgram(resources.ProgramInfo.Program.Get());
    glUniform1i(resources.ProgramInfo.LocTexture, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindBuffer(resources.VertexBuffer.GetTarget(), resources.VertexBuffer.Get());
    resources.VertexBuffer.EnableAttribArrays(resources.ProgramInfo.Links);
    glDrawArrays(GL_TRIANGLES, 0, resources.VertexBuffer.GetGLCapacity());
    resources.VertexBuffer.DisableAttribArrays(resources.ProgramInfo.Links);
    glBindBuffer(resources.VertexBuffer.GetTarget(), 0);
  }


  void AntiAliasing::DrawZoomArea(const GLuint hZoomTexture, const ZoomProgramInfo& programInfo, const GLES3::GLVertexBuffer& vertexBuffer,
                                  const Matrix& matWorldViewProjection, const bool useLinearFiltering)
  {
    glUseProgram(programInfo.Program.Get());
    glUniform1i(programInfo.LocTexture, 0);
    glUniformMatrix4fv(programInfo.LocWorldViewProjection, 1, GL_FALSE, matWorldViewProjection.DirectAccess());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hZoomTexture);
    if (useLinearFiltering)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    glBindBuffer(vertexBuffer.GetTarget(), vertexBuffer.Get());
    vertexBuffer.EnableAttribArrays(programInfo.Links);
    glDrawArrays(GL_TRIANGLES, 0, vertexBuffer.GetGLCapacity());
    vertexBuffer.DisableAttribArrays(programInfo.Links);
    glBindBuffer(vertexBuffer.GetTarget(), 0);
  }
}
