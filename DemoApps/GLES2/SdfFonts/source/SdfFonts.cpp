/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2025 NXP
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

#include "SdfFonts.hpp"
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <Shared/SdfFonts/AppHelper.hpp>
#include <GLES2/gl2.h>
#include <cassert>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const float DefaultZPos = 0.0f;

      constexpr const IO::PathView NormalFontAtlasTexturePath("Bitmap.png");
      constexpr const IO::PathView NormalFontPath("Bitmap_SoftMaskFont.nbf");

      constexpr const IO::PathView SdfFontAtlasTexturePath("Sdf.png");
      constexpr const IO::PathView SdfFontPath("Sdf_SdfFont.nbf");

      constexpr const IO::PathView MtsdfFontAtlasTexturePath("Mtsdf.png");
      constexpr const IO::PathView MtsdfFontPath("Mtsdf_MtsdfFont.nbf");

      constexpr const IO::PathView TextVertShader("Text.vert");
      constexpr const IO::PathView TextFragShader("Text.frag");

      constexpr const IO::PathView TextSdfFragShader("Text-sdf.frag");
      constexpr const IO::PathView TextSdfOutlineFragShader("Text-sdfOutline.frag");
      constexpr const IO::PathView TextSdfShadowFragShader("Text-sdfDropShadow.frag");
      constexpr const IO::PathView TextSdfShadowAndOutlineFragShader("Text-sdfDropShadowAndOutline.frag");
      constexpr const IO::PathView TextSdfContoursFragShader("Text-sdfContours.frag");

      constexpr const IO::PathView TextMtsdfFragShader("Text-mtsdf.frag");
      constexpr const IO::PathView TextMtsdfOutlineFragShader("Text-mtsdfOutline.frag");
      constexpr const IO::PathView TextMtsdfShadowFragShader("Text-mtsdfDropShadow.frag");
      constexpr const IO::PathView TextMtsdfShadowAndOutlineFragShader("Text-mtsdfDropShadowAndOutline.frag");
      constexpr const IO::PathView TextMtsdfContoursFragShader("Text-mtsdfContours.frag");

      constexpr const StringViewLite TextLine0("The quick brown fox jumps over the lazy dog! Hello World.");
      // constexpr StringViewLite TextLine1("abcdefghijklmnopqrstuvwxyz");
      // constexpr StringViewLite TextLine2("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
      // constexpr StringViewLite TextLine3("0123456789!\".:,;(){}");
    }

    GLES2::GLTexture ReadTexture(const IContentManager& contentManager, const IO::Path& path)
    {
      GLES2::GLTextureParameters params(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
      auto bitmap = contentManager.ReadBitmap(path, PixelFormat::R8G8B8A8_UNORM);
      return {bitmap, params};
    }
  }


  SdfFonts::SdfFonts(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_shared(config)
    , m_nativeBatch(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D())
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_shared.GetUIDemoAppExtension());


    auto contentManager = GetContentManager();


    const PxSize1D line0YPx = PxSize1D::Create(0);
    const SpriteNativeAreaCalc& spriteNativeAreaCalc = m_shared.GetUIDemoAppExtension()->GetSpriteNativeAreaCalc();
    const uint32_t densityDpi = config.WindowMetrics.DensityDpi;

    m_resources.ShaderNormal = GenerateShaderRecord(*contentManager, LocalConfig::TextVertShader, LocalConfig::TextFragShader);

    m_resources.Normal =
      PrepareExample(*contentManager, line0YPx, m_resources.ShaderNormal, LocalConfig::NormalFontPath, LocalConfig::NormalFontAtlasTexturePath,
                     LocalConfig::TextLine0, spriteNativeAreaCalc, densityDpi, m_positionsScratchpad);
    const auto line1YPx = m_resources.Normal.Font.Font.LineSpacingPx();
    m_resources.Sdf =
      PrepareExample(*contentManager, line1YPx, m_resources.ShaderNormal, LocalConfig::SdfFontPath, LocalConfig::SdfFontAtlasTexturePath,
                     LocalConfig::TextLine0, spriteNativeAreaCalc, densityDpi, m_positionsScratchpad);
    m_resources.Mtsdf =
      PrepareExample(*contentManager, line1YPx, m_resources.ShaderNormal, LocalConfig::MtsdfFontPath, LocalConfig::MtsdfFontAtlasTexturePath,
                     LocalConfig::TextLine0, spriteNativeAreaCalc, densityDpi, m_positionsScratchpad);

    m_resources.ShadersSdf.Normal = GenerateShaderRecord(*contentManager, LocalConfig::TextVertShader, LocalConfig::TextSdfFragShader);
    m_resources.ShadersSdf.Outline = GenerateShaderRecord(*contentManager, LocalConfig::TextVertShader, LocalConfig::TextSdfOutlineFragShader);
    m_resources.ShadersSdf.Shadow = GenerateShaderRecord(*contentManager, LocalConfig::TextVertShader, LocalConfig::TextSdfShadowFragShader);
    m_resources.ShadersSdf.ShadowAndOutline =
      GenerateShaderRecord(*contentManager, LocalConfig::TextVertShader, LocalConfig::TextSdfShadowAndOutlineFragShader);
    m_resources.ShadersSdf.Contours = GenerateShaderRecord(*contentManager, LocalConfig::TextVertShader, LocalConfig::TextSdfContoursFragShader);

    m_resources.ShadersMtsdf.Normal = GenerateShaderRecord(*contentManager, LocalConfig::TextVertShader, LocalConfig::TextMtsdfFragShader);
    m_resources.ShadersMtsdf.Outline = GenerateShaderRecord(*contentManager, LocalConfig::TextVertShader, LocalConfig::TextMtsdfOutlineFragShader);
    m_resources.ShadersMtsdf.Shadow = GenerateShaderRecord(*contentManager, LocalConfig::TextVertShader, LocalConfig::TextMtsdfShadowFragShader);
    m_resources.ShadersMtsdf.ShadowAndOutline =
      GenerateShaderRecord(*contentManager, LocalConfig::TextVertShader, LocalConfig::TextMtsdfShadowAndOutlineFragShader);
    m_resources.ShadersMtsdf.Contours = GenerateShaderRecord(*contentManager, LocalConfig::TextVertShader, LocalConfig::TextMtsdfContoursFragShader);

    m_resources.FillTexture = m_shared.GetFillTexture();

    FSLLOG3_INFO("Ready");
  }


  SdfFonts::~SdfFonts() = default;

  void SdfFonts::OnKeyEvent(const KeyEvent& event)
  {
    DemoAppGLES2::OnKeyEvent(event);

    m_shared.OnKeyEvent(event);
  }


  void SdfFonts::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    DemoAppGLES2::ConfigurationChanged(windowMetrics);

    m_shared.OnConfigurationChanged(windowMetrics);
  }


  void SdfFonts::Update(const DemoTime& demoTime)
  {
    // Setup the shader
    const auto screenWidth = static_cast<float>(GetWindowSizePx().RawWidth());
    const auto screenHeight = static_cast<float>(GetWindowSizePx().RawHeight());
    const float screenOffsetX = screenWidth / 2.0f;
    const float screenOffsetY = screenHeight / 2.0f;

    m_resources.Projection = Matrix::CreateTranslation(-screenOffsetX, -screenOffsetY, 1.0f) * Matrix::CreateRotationX(MathHelper::ToRadians(180)) *
                             Matrix::CreateOrthographic(screenWidth, screenHeight, 1.0f, 10.0f);

    m_shared.Update(demoTime);
  }


  void SdfFonts::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    const PxSize2D currentSizePx = GetWindowSizePx();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto contentOffset = m_shared.GetContentOffset();
    const auto fontDrawConfig = m_shared.GetFontDrawConfig();
    const auto fontScale = PxSize1DF::Create(fontDrawConfig.FontScale);
    const auto fontSdfMode = m_shared.GetSdfMode();
    const auto& fontSdfShader = SelectShaderRecord(fontSdfMode, fontDrawConfig.Type);

    auto& rSdfRecord = fontDrawConfig.Type == SdfType::Sdf ? m_resources.Sdf : m_resources.Mtsdf;

    const PxPoint2 line0Px(contentOffset.X, contentOffset.Y);
    const PxPoint2 line1Px(contentOffset.X, line0Px.Y + m_resources.Normal.Font.Font.LineSpacingPx());
    const PxPoint2 line2Px(contentOffset.X, line1Px.Y + rSdfRecord.Font.Font.LineSpacingPx());
    const PxPoint2 line3Px(
      contentOffset.X, line2Px.Y + TypeConverter::UncheckedChangeTo<PxSize1D>(PxSize1DF(m_resources.Normal.Font.Font.LineSpacingPx()) * fontScale));

    const bool enableKerning = m_shared.GetKerningEnabled();
    const BitmapFontConfig fontConfigNormal(1.0f, enableKerning);
    const BitmapFontConfig fontConfigScaled(fontDrawConfig.FontScale, enableKerning);

    RegenerateMeshOnDemand(m_resources.Normal.Mesh, line0Px, m_resources.ShaderNormal, m_resources.Normal.Font, fontConfigNormal,
                           LocalConfig::TextLine0, m_positionsScratchpad);
    RegenerateMeshOnDemand(rSdfRecord.Mesh, line1Px, fontSdfShader, rSdfRecord.Font, fontConfigNormal, LocalConfig::TextLine0, m_positionsScratchpad);
    RegenerateMeshOnDemand(m_resources.Normal.ScaledMesh, line2Px, fontSdfShader, m_resources.Normal.Font, fontConfigScaled, LocalConfig::TextLine0,
                           m_positionsScratchpad);
    RegenerateMeshOnDemand(rSdfRecord.ScaledMesh, line3Px, fontSdfShader, rSdfRecord.Font, fontConfigScaled, LocalConfig::TextLine0,
                           m_positionsScratchpad);

    const auto baseLine0Px = line0Px + PxPoint2(PxValue(0), m_resources.Normal.Font.Font.BaseLinePx());
    const auto baseLine1Px = line1Px + PxPoint2(PxValue(0), rSdfRecord.Font.Font.BaseLinePx());
    const auto baseLine2Px =
      line2Px + PxPoint2(PxValue(0), TypeConverter::UncheckedChangeTo<PxSize1D>((PxSize1DF(m_resources.Normal.Font.Font.BaseLinePx()) * fontScale)));
    const auto baseLine3Px =
      line3Px + PxPoint2(PxValue(0), TypeConverter::UncheckedChangeTo<PxSize1D>((PxSize1DF(rSdfRecord.Font.Font.BaseLinePx()) * fontScale)));

    // Draw baselines
    {
      constexpr auto BaseLineColor = Color(0xFF404040);

      m_nativeBatch->Begin();
      m_nativeBatch->DebugDrawLine(m_resources.FillTexture, baseLine0Px, PxPoint2(baseLine0Px.X + currentSizePx.Width(), baseLine0Px.Y),
                                   BaseLineColor);
      m_nativeBatch->DebugDrawLine(m_resources.FillTexture, baseLine1Px, PxPoint2(baseLine1Px.X + currentSizePx.Width(), baseLine1Px.Y),
                                   BaseLineColor);
      m_nativeBatch->DebugDrawLine(m_resources.FillTexture, baseLine2Px, PxPoint2(baseLine2Px.X + currentSizePx.Width(), baseLine2Px.Y),
                                   BaseLineColor);
      m_nativeBatch->DebugDrawLine(m_resources.FillTexture, baseLine3Px, PxPoint2(baseLine3Px.X + currentSizePx.Width(), baseLine3Px.Y),
                                   BaseLineColor);
      m_nativeBatch->End();
    }

    DrawMeshes(fontDrawConfig, fontSdfShader);

    // Draw bounding boxes
    if (m_shared.GetBoundingBoxesEnabled())
    {
      m_nativeBatch->Begin(BlendState::Opaque);
      m_shared.DrawBoundingBoxes(*m_nativeBatch, line0Px, LocalConfig::TextLine0, m_resources.Normal.Font.Font, fontConfigNormal,
                                 m_positionsScratchpad);
      m_shared.DrawBoundingBoxes(*m_nativeBatch, line1Px, LocalConfig::TextLine0, rSdfRecord.Font.Font, fontConfigNormal, m_positionsScratchpad);
      m_shared.DrawBoundingBoxes(*m_nativeBatch, line2Px, LocalConfig::TextLine0, m_resources.Normal.Font.Font, fontConfigScaled,
                                 m_positionsScratchpad);
      m_shared.DrawBoundingBoxes(*m_nativeBatch, line3Px, LocalConfig::TextLine0, rSdfRecord.Font.Font, fontConfigScaled, m_positionsScratchpad);
      m_nativeBatch->End();
    }
    m_shared.Draw();
  }

  void SdfFonts::DrawMeshes(const FontDrawConfig& fontDrawConfig, const SdfFonts::ShaderRecord& fontSdfShader)
  {
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);    // pre-multiplied alpha
    // glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    // normal alpha
    // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    FontDrawConfig drawConfig(fontDrawConfig);
    drawConfig.FontScale = 1.0f;

    {    // draw normal
      const auto& example = m_resources.Normal;
      DrawTextMesh(example.Mesh, example.Font, m_resources.ShaderNormal, m_resources.Projection, drawConfig);
      DrawTextMesh(example.ScaledMesh, example.Font, m_resources.ShaderNormal, m_resources.Projection, fontDrawConfig);
    }

    // draw sdf
    {
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    // normal alpha

      const auto& example = fontDrawConfig.Type == SdfType::Sdf ? m_resources.Sdf : m_resources.Mtsdf;

      DrawTextMesh(example.Mesh, example.Font, fontSdfShader, m_resources.Projection, drawConfig);
      DrawTextMesh(example.ScaledMesh, example.Font, fontSdfShader, m_resources.Projection, fontDrawConfig);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
  }

  void SdfFonts::DrawTextMesh(const MeshRecord& mesh, const FontRecord& fontRecord, const ShaderRecord& shader, const Matrix& projection,
                              const FontDrawConfig& fontDrawConfig)
  {
    // Set the shader program
    glUseProgram(shader.Program.Get());

    // Set the active texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontRecord.Texture.Get());

    // Load the matrices
    assert(shader.Location.ProjMatrix != GLES2::GLValues::InvalidLocation);
    assert(shader.Location.Texture != GLES2::GLValues::InvalidLocation);
    glUniformMatrix4fv(shader.Location.ProjMatrix, 1, 0, projection.DirectAccess());

    if (shader.Location.OutlineDistance != GLES2::GLValues::InvalidLocation)
    {
      const auto outlineDistance = (fontDrawConfig.OutlineDistance > 0.0f ? 0.5f * fontDrawConfig.OutlineDistance : 0.0f);
      glUniform1f(shader.Location.OutlineDistance, outlineDistance);
    }

    const float fontSdfSpread = std::max(fontRecord.Font.GetSdfParams().DistanceRange, 1.0f);
    if (shader.Location.Smoothing != GLES2::GLValues::InvalidLocation)
    {
      const auto smoothing = 0.25f / (fontSdfSpread * fontDrawConfig.FontScale);
      glUniform1f(shader.Location.Smoothing, smoothing);
    }
    if (shader.Location.ShadowOffset != GLES2::GLValues::InvalidLocation)
    {
      const auto maxOffsetX = fontSdfSpread / static_cast<float>(fontRecord.Texture.GetSize().RawWidth());
      const auto maxOffsetY = fontSdfSpread / static_cast<float>(fontRecord.Texture.GetSize().RawHeight());
      const auto shadowOffsetX = fontDrawConfig.ShadowOffset.X != 0.0f ? maxOffsetX * fontDrawConfig.ShadowOffset.X : 0.0f;
      const auto shadowOffsetY = fontDrawConfig.ShadowOffset.Y != 0.0f ? -(maxOffsetY * fontDrawConfig.ShadowOffset.Y) : 0.0f;
      glUniform2f(shader.Location.ShadowOffset, shadowOffsetX, shadowOffsetY);
    }
    if (shader.Location.ShadowSmoothing != GLES2::GLValues::InvalidLocation)
    {
      const auto shadowSmoothing = (fontDrawConfig.ShadowSmoothing > 0.0f ? 0.5f * fontDrawConfig.ShadowSmoothing : 0.0f);
      glUniform1f(shader.Location.ShadowSmoothing, shadowSmoothing);
    }
    if (shader.Location.ContourScale != GLES2::GLValues::InvalidLocation)
    {
      glUniform1f(shader.Location.ContourScale, fontDrawConfig.ContourScale);
    }

    glUniform1i(shader.Location.Texture, 0);


    glBindBuffer(mesh.VB.GetTarget(), mesh.VB.Get());
    glBindBuffer(mesh.IB.GetTarget(), mesh.IB.Get());

    // enable the attribs
    mesh.VB.EnableAttribArrays(mesh.AttribLink);

    glDrawElements(GL_TRIANGLES, mesh.IB.GetGLCapacity(), mesh.IB.GetType(), nullptr);

    mesh.VB.DisableAttribArrays(mesh.AttribLink);
  }


  SdfFonts::FontRecord SdfFonts::PrepareFont(const IContentManager& contentManager, const IO::Path& bitmapFontPath,
                                             const IO::Path& fontAtlasTexturePath, const SpriteNativeAreaCalc& spriteNativeAreaCalc,
                                             const uint32_t densityDpi)
  {
    auto texture = ReadTexture(contentManager, fontAtlasTexturePath);
    auto font =
      AppHelper::ReadFont(spriteNativeAreaCalc, TypeConverter::To<PxExtent2D>(texture.GetSize()), contentManager, bitmapFontPath, densityDpi);

    return {std::move(texture), std::move(font)};
  }


  SdfFonts::ExampleRecord SdfFonts::PrepareExample(const IContentManager& contentManager, const PxSize1D lineYPx, const ShaderRecord& shader,
                                                   const IO::Path& bitmapFontPath, const IO::Path& fontAtlasTexturePath,
                                                   const StringViewLite& strView, const SpriteNativeAreaCalc& spriteNativeAreaCalc,
                                                   const uint32_t densityDpi, std::vector<SpriteFontGlyphPosition>& rPositionsScratchpad)
  {
    FSLLOG3_INFO("Preparing example");
    ExampleRecord result;

    FSLLOG3_INFO("- Loading font");
    result.Font = PrepareFont(contentManager, bitmapFontPath, fontAtlasTexturePath, spriteNativeAreaCalc, densityDpi);

    FSLLOG3_INFO("- Generating mesh");
    const BitmapFontConfig fontConfig(1.0f);
    result.Mesh = GenerateMesh(PxPoint2(PxValue(0), lineYPx), result.Font, fontConfig, strView, rPositionsScratchpad);
    result.ScaledMesh = GenerateMesh(PxPoint2(PxValue(0), lineYPx), result.Font, fontConfig, strView, rPositionsScratchpad);
    return result;
  }

  SdfFonts::ShaderRecord SdfFonts::GenerateShaderRecord(const IContentManager& contentManager, const IO::Path& vertShaderPath,
                                                        const IO::Path& fragShaderPath)
  {
    FSLLOG3_INFO("- Loading shaders '{}' & '{}'", vertShaderPath, fragShaderPath);
    ShaderRecord result;
    result.Program.Reset(contentManager.ReadAllText(vertShaderPath), contentManager.ReadAllText(fragShaderPath));
    result.Location.OutlineDistance = result.Program.TryGetUniformLocation("g_outlineDistance");
    result.Location.ProjMatrix = result.Program.GetUniformLocation("g_matModelViewProj");
    result.Location.Smoothing = result.Program.TryGetUniformLocation("g_smoothing");
    result.Location.ShadowOffset = result.Program.TryGetUniformLocation("g_shadowOffset");
    result.Location.ShadowSmoothing = result.Program.TryGetUniformLocation("g_shadowSmoothing");
    result.Location.ContourScale = result.Program.TryGetUniformLocation("g_contourScale");
    result.Location.Texture = result.Program.GetUniformLocation("Texture");

    assert(result.Location.ProjMatrix != GLES2::GLValues::InvalidLocation);
    assert(result.Location.Texture != GLES2::GLValues::InvalidLocation);
    return result;
  }

  SdfFonts::MeshRecord SdfFonts::GenerateMesh(const PxPoint2& dstPositionPx, const FontRecord& fontRecord, const BitmapFontConfig& fontConfig,
                                              const StringViewLite& strView, std::vector<SpriteFontGlyphPosition>& rPositionsScratchpad)
  {
    const TextureAtlasSpriteFont& font = fontRecord.Font;
    const PxSize2D fontTextureSize = fontRecord.Texture.GetSize();

    std::vector<VertexPositionTexture> vertices(strView.size() * 4);
    std::vector<uint16_t> indices(strView.size() * (3 * 2));

    if (strView.size() > rPositionsScratchpad.size())
    {
      rPositionsScratchpad.resize(strView.size());
    }

    // Extract the render rules
    auto scratchpadSpan = SpanUtil::AsSpan(rPositionsScratchpad);
    const bool gotRules = font.ExtractRenderRules(scratchpadSpan, strView);
    const auto positionsSpan = scratchpadSpan.subspan(0, gotRules ? strView.size() : 0);

    AppHelper::GenerateVertices(SpanUtil::AsSpan(vertices), dstPositionPx, positionsSpan, LocalConfig::DefaultZPos, fontTextureSize);
    AppHelper::GenerateIndices(SpanUtil::AsSpan(indices), positionsSpan);

    GLES2::GLVertexBuffer vb(vertices, GL_STATIC_DRAW);
    // Attrib links are not generated here, but we expect RegenerateMeshOnDemand to handle it
    return {dstPositionPx, fontConfig, std::move(vertices), std::move(vb), GLES2::GLIndexBuffer(indices, GL_STATIC_DRAW), {}};
  }


  void SdfFonts::RegenerateMeshOnDemand(MeshRecord& rMeshRecord, const PxPoint2& dstPositionPx, const ShaderRecord& shader,
                                        const FontRecord& fontRecord, const BitmapFontConfig fontConfig, const StringViewLite& strView,
                                        std::vector<SpriteFontGlyphPosition>& rPositionsScratchpad)
  {
    bool shaderChanged = rMeshRecord.CachedShader != shader.Program.Get();
    if (rMeshRecord.Offset == dstPositionPx && rMeshRecord.FontConfig == fontConfig && !shaderChanged)
    {
      return;
    }
    rMeshRecord.Offset = dstPositionPx;
    rMeshRecord.FontConfig = fontConfig;
    rMeshRecord.CachedShader = shader.Program.Get();

    const TextureAtlasSpriteFont& font = fontRecord.Font;
    const PxSize2D fontTextureSize = fontRecord.Texture.GetSize();

    if (strView.size() > rPositionsScratchpad.size())
    {
      rPositionsScratchpad.resize(strView.size());
    }
    auto scratchpadSpan = SpanUtil::AsSpan(rPositionsScratchpad);
    const bool gotRules = font.ExtractRenderRules(scratchpadSpan, strView, fontConfig);
    const auto positionsSpan = scratchpadSpan.subspan(0, gotRules ? strView.size() : 0);

    AppHelper::GenerateVertices(SpanUtil::AsSpan(rMeshRecord.Vertices), dstPositionPx, positionsSpan, LocalConfig::DefaultZPos, fontTextureSize);
    rMeshRecord.VB.SetData(0, rMeshRecord.Vertices.data(), rMeshRecord.Vertices.size());


    if (shaderChanged)
    {
      FSLLOG3_VERBOSE4("Updating attrib links");
      // Regenerate the attrib links to match the active shader (not really good for performance but we rarely do this)
      rMeshRecord.AttribLink = {GLES2::GLVertexAttribLink(shader.Program.GetAttribLocation("VertexPosition"),
                                                          rMeshRecord.VB.GetVertexElementIndex(VertexElementUsage::Position, 0)),
                                GLES2::GLVertexAttribLink(shader.Program.TryGetAttribLocation("VertexTextureCoord"),
                                                          rMeshRecord.VB.GetVertexElementIndex(VertexElementUsage::TextureCoordinate, 0))};
    }
  }

  const SdfFonts::ShaderRecord& SdfFonts::SelectShaderRecord(const SdfFontMode fontSdfMode, const SdfType fontSdfType)
  {
    const FontShaderRecord& shaders = fontSdfType == SdfType::Sdf ? m_resources.ShadersSdf : m_resources.ShadersMtsdf;

    switch (fontSdfMode)
    {
    case SdfFontMode::Outline:
      return shaders.Outline;
    case SdfFontMode::Shadow:
      return shaders.Shadow;
    case SdfFontMode::ShadowAndOutline:
      return shaders.ShadowAndOutline;
    case SdfFontMode::Contours:
      return shaders.Contours;
    default:
      return shaders.Normal;
    }
  }

}
