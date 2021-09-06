/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
//#include <FslGraphics/TextureAtlas/TestAtlasTextureGenerator.hpp>
#include <Shared/System/DFNativeBatch2D/TextureAtlasScene.hpp>
#include <algorithm>
#include <array>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const int32_t GridCellSize = 128;

      // constexpr IO::PathView FontTexturePath("");

      constexpr IO::PathView PathMainAtlas("TextureAtlas/MainAtlas.bta");
      constexpr IO::PathView PathMainAtlasTexture("TextureAtlas/MainAtlas.png");


      constexpr IO::PathView PathTexture1("QuestquestGameIcons/cutepriest");
      constexpr IO::PathView PathTexture2("QuestquestGameIcons/boardgamewizard");
      constexpr IO::PathView PathTexture3("Test");
      constexpr IO::PathView PathTexture4("Test2");
      constexpr IO::PathView PathTextureFill("Fill");

      constexpr IO::PathView PathMainAtlasFont("TextureAtlas/MainAtlasFont.fbk");
      constexpr IO::PathView PathMainAtlasSmallFont("TextureAtlas/MainAtlasSmallFont.fbk");
      constexpr IO::PathView PathMainAtlasSdfFont("BasicUI/Font/SdfAtlas/FontAtlas_Font.fbk");

      constexpr IO::PathView PathSdfAtlas("BasicUI/Font/SdfAtlas/FontAtlas.bta");
      constexpr IO::PathView PathSdfAtlasTexture("BasicUI/Font/SdfAtlas/FontAtlas.png");
    }

    constexpr const std::array<BlendState, 5> g_blendStates = {BlendState::AlphaBlend, BlendState::NonPremultiplied, BlendState::Additive,
                                                               BlendState::Opaque, BlendState::Sdf};
    constexpr const std::array<const char*, 5> g_blendStatesDesc = {"AlphaBlend (Premultiplied)", "NonPremultiplied AlphaBlend", "Additive", "Opaque",
                                                                    "Sdf"};
  }

  TextureAtlasScene::TextureAtlasScene(const ServiceProvider& serviceProvider, const SpriteNativeAreaCalc& spriteNativeAreaCalc,
                                       const uint32_t densityDpi)
    : m_graphics(serviceProvider.Get<IGraphicsService>())
    , m_batch(m_graphics->GetNativeBatch2D())
    , m_blendStateIndex(0)
    , m_blendState(g_blendStates[m_blendStateIndex])
    , m_fillCenter(16, 16, 1, 1)
    , m_angle(0.0f)
    , m_zoomAngle1(0.0f)
    , m_zoomAngle2(0.0f)
    , m_sceneId(0)
  {
    const std::shared_ptr<IContentManager> contentManager = serviceProvider.Get<IContentManager>();
    {
      BasicTextureAtlas atlas;
      contentManager->Read(atlas, LocalConfig::PathMainAtlas);

      BasicTextureAtlas sdfAtlas;
      contentManager->Read(sdfAtlas, LocalConfig::PathSdfAtlas);

      Fsl::Bitmap bitmap;
      contentManager->Read(bitmap, LocalConfig::PathMainAtlasTexture, PixelFormat::R8G8B8A8_UNORM);
      // TestAtlasTextureGenerator::PatchWithTestPattern(bitmap, atlas);

      Texture2D tex(m_graphics->GetNativeGraphics(), bitmap, Texture2DFilterHint::Smooth);

      contentManager->Read(bitmap, LocalConfig::PathSdfAtlasTexture, PixelFormat::R8G8B8A8_UNORM);
      m_texSdf = Texture2D(m_graphics->GetNativeGraphics(), bitmap, Texture2DFilterHint::Smooth);

      AtlasTextureInfo atlasTextureInfo1 = TextureAtlasHelper::GetAtlasTextureInfo(atlas, LocalConfig::PathTexture1);
      AtlasTextureInfo atlasTextureInfo2 = TextureAtlasHelper::GetAtlasTextureInfo(atlas, LocalConfig::PathTexture2);
      AtlasTextureInfo atlasTextureInfo3 = TextureAtlasHelper::GetAtlasTextureInfo(atlas, LocalConfig::PathTexture3);
      AtlasTextureInfo atlasTextureInfo4 = TextureAtlasHelper::GetAtlasTextureInfo(atlas, LocalConfig::PathTexture4);
      AtlasTextureInfo atlasTextureInfoFill = TextureAtlasHelper::GetAtlasTextureInfo(atlas, LocalConfig::PathTextureFill);
      m_atlasTexture1.Reset(tex, atlasTextureInfo1);
      m_atlasTexture2.Reset(tex, atlasTextureInfo2);
      m_atlasTexture3.Reset(tex, atlasTextureInfo3);
      m_atlasTexture4.Reset(tex, atlasTextureInfo4);
      m_atlasTextureFill.Reset(tex, atlasTextureInfoFill);

      BasicFontKerning fontBasicKerning;
      BasicFontKerning smallFontBasicKerning;
      BasicFontKerning sdfFontBasicKerning;
      contentManager->Read(fontBasicKerning, LocalConfig::PathMainAtlasFont);
      contentManager->Read(smallFontBasicKerning, LocalConfig::PathMainAtlasSmallFont);
      contentManager->Read(sdfFontBasicKerning, LocalConfig::PathMainAtlasSdfFont);

      m_font.Reset(spriteNativeAreaCalc, tex.GetExtent(), atlas, fontBasicKerning, densityDpi);
      m_fontSmall.Reset(spriteNativeAreaCalc, tex.GetExtent(), atlas, smallFontBasicKerning, densityDpi);
      m_fontSdf.Reset(spriteNativeAreaCalc, m_texSdf.GetExtent(), sdfAtlas, sdfFontBasicKerning, densityDpi);
    }
  }


  void TextureAtlasScene::OnKeyEvent(const KeyEvent& event)
  {
    if (!event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::Space:
      event.Handled();
      ++m_blendStateIndex;
      if (m_blendStateIndex >= static_cast<int32_t>(g_blendStates.size()))
      {
        m_blendStateIndex = 0;
      }
      m_blendState = g_blendStates[m_blendStateIndex];
      break;
    case VirtualKey::Code1:
      m_sceneId = 0;
      event.Handled();
      break;
    case VirtualKey::Code2:
      m_sceneId = 1;
      event.Handled();
      break;
    default:
      break;
    }
  }


  int32_t TextureAtlasScene::GetGridOffsetY() const
  {
    return m_font.LineSpacingPx();
  }


  void TextureAtlasScene::Update(const DemoTime& demoTime)
  {
    m_angle += 1.0f * demoTime.DeltaTime;
    m_zoomAngle1 += 1.0f * demoTime.DeltaTime;
    m_zoomAngle2 += 0.75f * demoTime.DeltaTime;
    m_zoomFactorAngle += 0.5f * demoTime.DeltaTime;
    m_angle = MathHelper::WrapAngle(m_angle);
    m_zoomAngle1 = MathHelper::WrapAngle(m_zoomAngle1);
    m_zoomAngle2 = MathHelper::WrapAngle(m_zoomAngle2);
    m_zoomFactorAngle = MathHelper::WrapAngle(m_zoomFactorAngle);
  }


  void TextureAtlasScene::Draw(const PxSize2D& windowSizePx)
  {
    Color colorTargetBorder(64, 64, 64, 255);
    Color colorTargetCenter(100, 100, 100, 255);

    const auto& atlasTexture = m_atlasTexture1.GetAtlasTexture();
    Point2 dstOffset = Point2(0, m_font.LineSpacingPx());


    m_batch->Begin(m_blendState);

    if (m_sceneId == 0)
    {    // Draw the texture atlas
      const PxSize2D atlasSize = atlasTexture.GetSize();
      m_batch->Draw(atlasTexture, Vector2(windowSizePx.Width() - atlasSize.Width(), windowSizePx.Height() - atlasSize.Height()), Color::White());
    }
    DrawGrid(LocalConfig::GridCellSize, LocalConfig::GridCellSize, windowSizePx.Width(), windowSizePx.Height(), colorTargetBorder, colorTargetCenter,
             dstOffset);

    // Render the caption
    m_batch->DrawString(atlasTexture, m_font, g_blendStatesDesc[m_blendStateIndex], Vector2(0, 0), Color::White());

    if (m_sceneId == 0)
    {
      DrawStrings(atlasTexture, m_font, "Hello world", LocalConfig::GridCellSize * 3);

      DrawAtlasTextureSimpleUsingDstPos(dstOffset);
      dstOffset.X += LocalConfig::GridCellSize * 3;
      dstOffset.Y += LocalConfig::GridCellSize * 3;
      DrawAtlasTextureSimpleUsingDstRectangle(dstOffset);

      dstOffset.X -= LocalConfig::GridCellSize * 3;
      dstOffset.Y += (LocalConfig::GridCellSize * 4) + (LocalConfig::GridCellSize / 2);

      // Render a simple test string
      m_batch->DrawString(atlasTexture, m_fontSmall, "The quick brown fox jumps over the lazy dog",
                          Vector2(dstOffset.X, dstOffset.Y - m_fontSmall.BaseLinePx()), Color::Cyan());

      {    // Render the sdf test string
        m_batch->ChangeTo(BlendState::Sdf);
        constexpr StringViewLite strText("Hello World (sdf)");
        auto sizeTextPx = m_fontSdf.MeasureString(strText);
        const float zoomFactor = ((std::cos(m_zoomFactorAngle) + 1.0f) / 2.0f) * 6.0f;
        BitmapFontConfig fontConfig(zoomFactor);
        auto sizeZoomedTextPx = m_fontSdf.MeasureString(strText, fontConfig);
        m_batch->DrawString(m_texSdf, m_fontSdf, strText, Vector2(windowSizePx.Width() - sizeTextPx.Width(), 0), Color::White());
        m_batch->DrawString(m_texSdf, m_fontSdf, fontConfig, strText,
                            Vector2(windowSizePx.Width() - sizeZoomedTextPx.Width(), windowSizePx.Height() - sizeZoomedTextPx.Height()),
                            Color::White());
      }
    }
    else
    {
      DrawAtlasTextureSimpleRotationUsingDstPos(dstOffset);
    }

    m_batch->End();
  }


  void TextureAtlasScene::DrawStrings(const BaseTexture2D& atlasTexture, const TextureAtlasSpriteFont& font, const char* const psz,
                                      const int32_t areaWidth)
  {
    const float zoomValue1 = (std::sin(m_zoomAngle1) + 1.0f) / 2.0f;
    const float zoomValue2 = (std::sin(m_zoomAngle2) + 1.0f) / 2.0f;
    Vector2 zoom1(zoomValue1, zoomValue1);
    Vector2 zoom2(zoomValue1, zoomValue2);

    Vector2 dstPosText(LocalConfig::GridCellSize * 7, m_font.LineSpacingPx() + LocalConfig::GridCellSize * 3);

    const float gridCellSizeDiv2 = (LocalConfig::GridCellSize / 2.0f);

    m_batch->DrawString(atlasTexture, font, psz, dstPosText, Color::White());
    dstPosText.Y += gridCellSizeDiv2;

    const auto dimPx = font.MeasureString(psz);
    const auto emptyAreaX = float(areaWidth - dimPx.Width());
    Vector2 originCenter(dimPx.Width() * 0.5f, dimPx.Height() * 0.5f);
    Vector2 originBottomRight(dimPx.Width(), dimPx.Height());

    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + (emptyAreaX * 0.5f), dstPosText.Y), Color::White());
    dstPosText.Y += gridCellSizeDiv2;

    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + emptyAreaX, dstPosText.Y), Color::White());
    dstPosText.Y += gridCellSizeDiv2;

    // Zoom: top-left
    m_batch->DrawString(atlasTexture, font, psz, dstPosText, Color::White(), Vector2(), zoom1);
    dstPosText.Y += gridCellSizeDiv2;
    // Zoom: center
    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + (emptyAreaX * 0.5f) + originCenter.X, dstPosText.Y + originCenter.Y),
                        Color::White(), originCenter, zoom1);
    dstPosText.Y += gridCellSizeDiv2;
    // Zoom: right
    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + emptyAreaX + originBottomRight.X, dstPosText.Y + originBottomRight.Y),
                        Color::White(), originBottomRight, zoom1);
    dstPosText.Y += gridCellSizeDiv2;

    // Zoom: top-left
    m_batch->DrawString(atlasTexture, font, psz, dstPosText, Color::White(), Vector2(), zoom2);
    dstPosText.Y += gridCellSizeDiv2;
    // Zoom: center
    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + (emptyAreaX * 0.5f) + originCenter.X, dstPosText.Y + originCenter.Y),
                        Color::White(), originCenter, zoom2);
    dstPosText.Y += gridCellSizeDiv2;
    // Zoom: right
    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + emptyAreaX + originBottomRight.X, dstPosText.Y + originBottomRight.Y),
                        Color::White(), originBottomRight, zoom2);
    dstPosText.Y += gridCellSizeDiv2;
  }


  void TextureAtlasScene::DrawAtlasTextureSimpleRotationUsingDstPos(const Point2& dstOffset)
  {
    const float zoomValue1 = (std::sin(m_zoomAngle1) + 1.0f) / 2.0f;
    const float zoomValue2 = (std::sin(m_zoomAngle2) + 1.0f) / 2.0f;
    Vector2 zoom1(zoomValue1, zoomValue1);
    Vector2 zoom2(zoomValue1, zoomValue2);

    const PxSize2D atlas4Size = m_atlasTexture4.GetSize();

    {
      const Vector2 halfF4(atlas4Size.Width() / 2.0f, atlas4Size.Height() / 2.0f);
      Vector2 offset0 = Vector2(dstOffset.X + LocalConfig::GridCellSize * 1, dstOffset.Y + LocalConfig::GridCellSize);
      Vector2 offset1 = Vector2(dstOffset.X + LocalConfig::GridCellSize * 4, dstOffset.Y + LocalConfig::GridCellSize);
      Vector2 offset2 = Vector2(dstOffset.X + LocalConfig::GridCellSize * 7, dstOffset.Y + LocalConfig::GridCellSize);

      m_batch->Draw(m_atlasTexture4, offset0, Color(64, 64, 64, 255), 0.0f, Vector2(), Vector2::One());
      m_batch->Draw(m_atlasTexture4, offset0, Color::White(), m_angle, Vector2(), Vector2::One());
      m_batch->Draw(m_atlasTexture4, offset0, Color::Green(), m_angle, halfF4, Vector2::One());

      m_batch->Draw(m_atlasTexture4, offset1, Color(64, 64, 64, 255), 0.0f, Vector2(), zoom1);
      m_batch->Draw(m_atlasTexture4, offset1, Color::White(), m_angle, Vector2(), zoom1);
      m_batch->Draw(m_atlasTexture4, offset1, Color::Green(), m_angle, halfF4, zoom1);

      m_batch->Draw(m_atlasTexture4, offset2, Color(64, 64, 64, 255), 0.0f, Vector2(), zoom2);
      m_batch->Draw(m_atlasTexture4, offset2, Color::White(), m_angle, Vector2(), zoom2);
      m_batch->Draw(m_atlasTexture4, offset2, Color::Green(), m_angle, halfF4, zoom2);
    }

    {
      PxRectangle srcRect(atlas4Size.Width() / 4, atlas4Size.Height() / 4, atlas4Size.Width() / 2, atlas4Size.Height() / 2);
      const Vector2 halfF4(srcRect.Width() / 2.0f, srcRect.Height() / 2.0f);
      Vector2 offset0 = Vector2(dstOffset.X + LocalConfig::GridCellSize * 1, dstOffset.Y + (LocalConfig::GridCellSize * 3));
      Vector2 offset1 = Vector2(dstOffset.X + LocalConfig::GridCellSize * 4, dstOffset.Y + (LocalConfig::GridCellSize * 3));
      Vector2 offset2 = Vector2(dstOffset.X + LocalConfig::GridCellSize * 7, dstOffset.Y + (LocalConfig::GridCellSize * 3));

      m_batch->Draw(m_atlasTexture4, offset0, srcRect, Color(64, 64, 64, 255), 0.0f, Vector2(), Vector2::One());
      m_batch->Draw(m_atlasTexture4, offset0, srcRect, Color::White(), m_angle, Vector2(), Vector2::One());
      m_batch->Draw(m_atlasTexture4, offset0, srcRect, Color::Green(), m_angle, halfF4, Vector2::One());

      m_batch->Draw(m_atlasTexture4, offset1, srcRect, Color(64, 64, 64, 255), 0.0f, Vector2(), zoom1);
      m_batch->Draw(m_atlasTexture4, offset1, srcRect, Color::White(), m_angle, Vector2(), zoom1);
      m_batch->Draw(m_atlasTexture4, offset1, srcRect, Color::Green(), m_angle, halfF4, zoom1);

      m_batch->Draw(m_atlasTexture4, offset2, srcRect, Color(64, 64, 64, 255), 0.0f, Vector2(), zoom2);
      m_batch->Draw(m_atlasTexture4, offset2, srcRect, Color::White(), m_angle, Vector2(), zoom2);
      m_batch->Draw(m_atlasTexture4, offset2, srcRect, Color::Green(), m_angle, halfF4, zoom2);
    }
  }


  void TextureAtlasScene::DrawAtlasTextureSimpleUsingDstPos(const Point2& dstOffset)
  {
    const float zoomValue1 = (std::sin(m_zoomAngle1) + 1.0f) / 2.0f;
    const float zoomValue2 = (std::sin(m_zoomAngle2) + 1.0f) / 2.0f;
    Vector2 zoom1(zoomValue1, zoomValue1);
    Vector2 zoom2(zoomValue1, zoomValue2);

    const PxSize2D atlas1Size = m_atlasTexture1.GetSize();
    const PxSize2D atlas2Size = m_atlasTexture2.GetSize();
    const PxSize2D atlas3Size = m_atlasTexture3.GetSize();
    const PxSize2D atlas4Size = m_atlasTexture4.GetSize();

    const auto maxSizeY = static_cast<float>(std::max(atlas1Size.Height(), atlas2Size.Height()));
    const Vector2 offsetY(0.0f, maxSizeY);


    const Point2 half1(atlas1Size.Width() / 2, atlas1Size.Height() / 2);
    const Point2 half2(atlas2Size.Width() / 2, atlas2Size.Height() / 2);
    const Point2 half3(atlas3Size.Width() / 2, atlas3Size.Height() / 2);
    const Vector2 halfF1(atlas1Size.Width() / 2.0f, atlas1Size.Height() / 2.0f);
    const Vector2 halfF2(atlas2Size.Width() / 2.0f, atlas2Size.Height() / 2.0f);
    const Vector2 halfF3(atlas3Size.Width() / 2.0f, atlas3Size.Height() / 2.0f);
    const Vector2 halfF4(atlas4Size.Width() / 2.0f, atlas4Size.Height() / 2.0f);

    const int quarterX1 = atlas1Size.Width() / 4;
    const int quarterY1 = atlas1Size.Height() / 4;
    const int quarterX2 = atlas2Size.Width() / 4;
    const int quarterY2 = atlas2Size.Height() / 4;

    const float spacing = 32;

    Vector2 offset1(dstOffset.X, dstOffset.Y);
    Vector2 offset2(offset1.X + m_atlasTexture1.GetSize().Width(), offset1.Y);
    Vector2 offset3(offset2.X + m_atlasTexture2.GetSize().Width(), offset2.Y);
    Vector2 offset4(dstOffset.X + LocalConfig::GridCellSize * 10, dstOffset.Y + LocalConfig::GridCellSize);

    Vector2 offsetA(offset3.X + static_cast<float>(m_atlasTexture3.GetSize().Width()) + spacing, offset3.Y);
    Vector2 offsetB(offsetA.X + static_cast<float>(m_atlasTexture1.GetSize().Width() * 3), offsetA.Y);

    Rectangle srcRect(atlas4Size.Width() / 4, atlas4Size.Height() / 4, atlas4Size.Width() / 2, atlas4Size.Height() / 2);
    m_batch->Draw(m_atlasTexture4, offset4, Color(64, 64, 64, 255), 0.0f, Vector2(), Vector2::One());
    m_batch->Draw(m_atlasTexture4, offset4, Color::White(), m_angle, Vector2(), Vector2::One());
    m_batch->Draw(m_atlasTexture4, offset4, Color::Green(), m_angle, halfF4, Vector2::One());

    constexpr const int32_t numEntries = 3;
    std::array<Vector2, numEntries> offsets1;
    std::array<Vector2, numEntries> offsets2;
    const float speed = 0.40f;
    for (int32_t i = 0; i < numEntries; ++i)
    {
      offsets1[i] = Vector2(offsetA.X + static_cast<float>(i * atlas1Size.Width()), offsetA.Y + 30.0f + std::sin(m_angle + (i * speed)) * 30.0f);
      offsets2[i] = Vector2(offsetB.X + static_cast<float>(i * atlas2Size.Width()), offsetB.Y + 30.0f + std::sin(m_angle + (i * speed)) * 30.0f);
    }

    m_batch->Draw(m_atlasTexture1, offset1, Color::White());
    m_batch->Draw(m_atlasTexture2, offset2, Color::White());
    m_batch->Draw(m_atlasTexture1, offsets1.data(), numEntries, Color::White());
    m_batch->Draw(m_atlasTexture2, offsets2.data(), numEntries, Color::White());

    offset1.Y += maxSizeY;
    offset2.Y += maxSizeY;
    offset3.Y += maxSizeY;
    for (int i = 0; i < numEntries; ++i)
    {
      offsets1[i] += offsetY;
      offsets2[i] += offsetY;
    }

    m_batch->Draw(m_atlasTexture1, offset1, PxRectangle(0, 0, atlas1Size.Width(), half1.Y), Color::White());
    m_batch->Draw(m_atlasTexture2, offset2, PxRectangle(0, 0, atlas2Size.Width(), half2.Y), Color::White());
    m_batch->Draw(m_atlasTexture1, offsets1.data(), numEntries, PxRectangle(0, 0, atlas1Size.Width(), half1.Y), Color::White());
    m_batch->Draw(m_atlasTexture2, offsets2.data(), numEntries, PxRectangle(0, 0, atlas2Size.Width(), half2.Y), Color::White());

    offset1.Y += maxSizeY / 2.0f;
    offset2.Y += maxSizeY / 2.0f;
    offset3.Y += maxSizeY / 2.0f;
    for (int i = 0; i < numEntries; ++i)
    {
      offsets1[i] += offsetY / 2;
      offsets2[i] += offsetY / 2;
    }

    m_batch->Draw(m_atlasTexture1, offset1, PxRectangle(0, half1.Y, atlas1Size.Width(), atlas1Size.Height() - half1.Y), Color::White());
    m_batch->Draw(m_atlasTexture2, offset2, PxRectangle(0, half2.Y, atlas2Size.Width(), atlas2Size.Height() - half2.Y), Color::White());
    m_batch->Draw(m_atlasTexture1, offsets1.data(), numEntries, PxRectangle(0, half1.Y, atlas1Size.Width(), atlas1Size.Height() - half1.Y),
                  Color::White());
    m_batch->Draw(m_atlasTexture2, offsets2.data(), numEntries, PxRectangle(0, half2.Y, atlas2Size.Width(), atlas2Size.Height() - half2.Y),
                  Color::White());

    offset1.Y += maxSizeY / 2.0f;
    offset2.Y += maxSizeY / 2.0f;
    offset3.Y += maxSizeY / 2.0f;
    for (int i = 0; i < numEntries; ++i)
    {
      offsets1[i] += offsetY / 2;
      offsets2[i] += offsetY / 2;
    }

    PxRectangle q1(quarterX1, quarterY1, atlas1Size.Width() - (quarterX1 * 2), atlas1Size.Height() - (quarterY1 * 2));
    PxRectangle q2(quarterX2, quarterY2, atlas2Size.Width() - (quarterX2 * 2), atlas2Size.Height() - (quarterY2 * 2));
    PxRectangleU32 q3(quarterX2, quarterY2, atlas2Size.Width() - (quarterX2 * 2), atlas2Size.Height() - (quarterY2 * 2));

    m_batch->Draw(m_atlasTexture1, offset1, q1, Color::White());
    m_batch->Draw(m_atlasTexture2, offset2, q2, Color::White());
    m_batch->Draw(m_atlasTexture1, offsets1.data(), numEntries, q1, Color::White());
    m_batch->Draw(m_atlasTexture2, offsets2.data(), numEntries, q2, Color::White());

    m_batch->Draw(m_atlasTexture2, offset3, q3, Color::White(), BatchEffect::FlipHorizontal);
    m_batch->Draw(m_atlasTexture2, offset3 - Vector2(0.0f, q3.Height), q3, Color::White(), BatchEffect::FlipVertical);
    m_batch->Draw(m_atlasTexture2, offset3 - Vector2(0.0f, q3.Height * 2), q3, Color::White(), BatchEffect::Rotate90Clockwise);
    m_batch->Draw(m_atlasTexture2, offset3 - Vector2(0.0f, q3.Height * 3), q3, Color::White(), BatchEffect::Rotate180Clockwise);
    m_batch->Draw(m_atlasTexture2, offset3 - Vector2(0.0f, q3.Height * 4), q3, Color::White(), BatchEffect::Rotate270Clockwise);

    offset1.Y += maxSizeY;
    offset2.Y += maxSizeY;
    offset3.Y += maxSizeY;

    m_batch->Draw(m_atlasTexture1, offset1, Color::White(), Vector2(), zoom1);
    m_batch->Draw(m_atlasTexture2, offset2, Color::White(), Vector2(), zoom1);
    m_batch->Draw(m_atlasTexture1, offset3, q1, Color::White(), Vector2(), zoom1);

    offset1.Y += maxSizeY;
    offset2.Y += maxSizeY;
    offset3.Y += maxSizeY;

    m_batch->Draw(m_atlasTexture1, offset1, Color::White(), Vector2(), zoom2);
    m_batch->Draw(m_atlasTexture2, offset2, Color::White(), Vector2(), zoom2);
    m_batch->Draw(m_atlasTexture1, offset3, q1, Color::White(), Vector2(), zoom2);

    offset1.Y += maxSizeY;
    offset2.Y += maxSizeY;
    offset3.Y += maxSizeY;

    m_batch->Draw(m_atlasTexture1, Vector2(offset1.X + halfF1.X, offset1.Y + halfF1.Y), Color::White(), Vector2(halfF1.X, halfF1.Y), zoom1);
    m_batch->Draw(m_atlasTexture3, Vector2(offset2.X + halfF3.X, offset2.Y + halfF3.Y), Color::White(), Vector2(halfF3.X, halfF3.Y), zoom1);
    m_batch->Draw(m_atlasTexture1, Vector2(offset3.X + halfF1.X, offset3.Y + halfF1.Y), q1, Color::White(),
                  Vector2(q1.Width() * 0.5f, q1.Height() * 0.5f), zoom1);

    offset1.Y += maxSizeY;
    offset2.Y += maxSizeY;
    offset3.Y += maxSizeY;

    m_batch->Draw(m_atlasTexture1, Vector2(offset1.X + halfF1.X, offset1.Y + halfF1.Y), Color::White(), Vector2(halfF1.X, halfF1.Y), zoom2);
    m_batch->Draw(m_atlasTexture3, Vector2(offset2.X + halfF3.X, offset2.Y + halfF3.Y), Color::White(), Vector2(halfF3.X, halfF3.Y), zoom2);
    m_batch->Draw(m_atlasTexture1, Vector2(offset3.X + halfF1.X, offset3.Y + halfF1.Y), q1, Color::White(),
                  Vector2(q1.Width() * 0.5f, q1.Height() * 0.5f), zoom2);
  }


  void TextureAtlasScene::DrawAtlasTextureSimpleUsingDstRectangle(const Point2& dstOffset)
  {
    const float zoomValue1 = (std::sin(m_zoomAngle1) + 1.0f) / 2.0f;
    const float zoomValue2 = (std::sin(m_zoomAngle2) + 1.0f) / 2.0f;
    Vector2 zoom1(zoomValue1, zoomValue1);
    Vector2 zoom2(zoomValue1, zoomValue2);

    const PxSize2D atlas1Size = m_atlasTexture1.GetSize();
    const PxSize2D atlas2Size = m_atlasTexture1.GetSize();
    const int maxSizeY = std::max(atlas1Size.Height(), atlas2Size.Height());
    const Vector2 offsetY(0.0f, maxSizeY);

    const int halfY1 = atlas1Size.Height() / 2;
    const int halfY2 = atlas2Size.Height() / 2;

    const int quarterX1 = atlas1Size.Width() / 4;
    const int quarterY1 = atlas1Size.Height() / 4;
    const int quarterX2 = atlas2Size.Width() / 4;
    const int quarterY2 = atlas2Size.Height() / 4;

    PxRectangle offset1(dstOffset.X, dstOffset.Y, int32_t(atlas1Size.Width() * zoomValue1), int32_t(atlas1Size.Height() * zoomValue1));
    PxRectangle offset2(offset1.X() + m_atlasTexture1.GetSize().Width(), offset1.Y(), int32_t(atlas2Size.Width() * zoomValue1),
                        int32_t(atlas2Size.Height() * zoomValue1));
    PxRectangle offset3(offset2.X() + m_atlasTexture2.GetSize().Width(), offset2.Y(), int32_t(atlas1Size.Width() * zoomValue1),
                        int32_t(atlas1Size.Height() * zoomValue2));
    PxRectangle offset4(offset3.X() + m_atlasTexture1.GetSize().Width(), offset3.Y(), int32_t(atlas2Size.Width() * zoomValue1),
                        int32_t(atlas2Size.Height() * zoomValue2));

    m_batch->Draw(m_atlasTexture1, offset1, Color::White());
    m_batch->Draw(m_atlasTexture2, offset2, Color::White());
    m_batch->Draw(m_atlasTexture1, offset3, Color::White());
    m_batch->Draw(m_atlasTexture2, offset4, Color::White());

    offset1.AddY(maxSizeY);
    offset2.AddY(maxSizeY);
    offset3.AddY(maxSizeY);
    offset4.AddY(maxSizeY);

    m_batch->Draw(m_atlasTexture1, offset1, PxRectangle(0, 0, atlas1Size.Width(), halfY1), Color::White());
    m_batch->Draw(m_atlasTexture2, offset2, PxRectangle(0, 0, atlas2Size.Width(), halfY2), Color::White());
    m_batch->Draw(m_atlasTexture1, offset3, PxRectangle(0, 0, atlas1Size.Width(), halfY1), Color::White());
    m_batch->Draw(m_atlasTexture2, offset4, PxRectangle(0, 0, atlas2Size.Width(), halfY2), Color::White());

    offset1.AddY(maxSizeY);
    offset2.AddY(maxSizeY);
    offset3.AddY(maxSizeY);
    offset4.AddY(maxSizeY);

    m_batch->Draw(m_atlasTexture1, offset1, PxRectangle(0, halfY1, atlas1Size.Width(), atlas1Size.Height() - halfY1), Color::White());
    m_batch->Draw(m_atlasTexture2, offset2, PxRectangle(0, halfY2, atlas2Size.Width(), atlas2Size.Height() - halfY2), Color::White());
    m_batch->Draw(m_atlasTexture1, offset3, PxRectangle(0, halfY1, atlas1Size.Width(), atlas1Size.Height() - halfY1), Color::White());
    m_batch->Draw(m_atlasTexture2, offset4, PxRectangle(0, halfY2, atlas2Size.Width(), atlas2Size.Height() - halfY2), Color::White());

    offset1.AddY(maxSizeY);
    offset2.AddY(maxSizeY);
    offset3.AddY(maxSizeY);
    offset4.AddY(maxSizeY);

    m_batch->Draw(m_atlasTexture1, offset1,
                  PxRectangle(quarterX1, quarterY1, atlas1Size.Width() - (quarterX1 * 2), atlas1Size.Height() - (quarterY1 * 2)), Color::White());
    m_batch->Draw(m_atlasTexture2, offset2,
                  PxRectangle(quarterX2, quarterY2, atlas2Size.Width() - (quarterX2 * 2), atlas2Size.Height() - (quarterY2 * 2)), Color::White());
    m_batch->Draw(m_atlasTexture1, offset3,
                  PxRectangle(quarterX1, quarterY1, atlas1Size.Width() - (quarterX1 * 2), atlas1Size.Height() - (quarterY1 * 2)), Color::White());
    m_batch->Draw(m_atlasTexture2, offset4,
                  PxRectangle(quarterX2, quarterY2, atlas2Size.Width() - (quarterX2 * 2), atlas2Size.Height() - (quarterY2 * 2)), Color::White());
  }

  void TextureAtlasScene::DrawGrid(const int32_t cellWidth, const int32_t cellHeight, const int32_t areaWidth, const int32_t areaHeight,
                                   const Color& colorBorder, const Color& colorCenter, const Point2& offset)
  {
    const int32_t startX = offset.X == 0 ? 0 : offset.X - cellWidth;
    const int32_t startY = offset.Y == 0 ? 0 : offset.Y - cellHeight;
    const int32_t xOffset = cellWidth / 2;
    const int32_t yOffset = cellHeight / 2;
    for (int32_t y = startY; y < areaHeight; y += cellHeight)
    {
      m_batch->Draw(m_atlasTextureFill, PxRectangle(0, y, areaWidth, 1), m_fillCenter, colorBorder);
      m_batch->Draw(m_atlasTextureFill, PxRectangle(0, y + yOffset, areaWidth, 1), m_fillCenter, colorCenter);
    }

    for (int32_t x = startX; x < areaWidth; x += cellWidth)
    {
      m_batch->Draw(m_atlasTextureFill, PxRectangle(x, 0, 1, areaHeight), m_fillCenter, colorBorder);
      m_batch->Draw(m_atlasTextureFill, PxRectangle(x + xOffset, 0, 1, areaHeight), m_fillCenter, colorCenter);
    }
  }


  void TextureAtlasScene::DrawTargetingRect(const PxRectangle& dstRect, const Color& colorBorder, const Color& colorCenter)
  {
    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.X(), dstRect.GetCenter().Y, dstRect.Width(), 1), m_fillCenter, colorCenter);
    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.GetCenter().X, dstRect.Y(), 1, dstRect.Height()), m_fillCenter, colorCenter);

    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.X(), dstRect.Y(), dstRect.Width(), 1), m_fillCenter, colorBorder);
    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.X(), dstRect.Y(), 1, dstRect.Height()), m_fillCenter, colorBorder);
  }


  void TextureAtlasScene::DrawRect(const PxRectangle& dstRect, const Color& color)
  {
    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.X(), dstRect.Y(), dstRect.Width(), 1), m_fillCenter, color);
    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.X(), dstRect.Bottom(), dstRect.Width(), 1), m_fillCenter, color);
    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.X(), dstRect.Y(), 1, dstRect.Height()), m_fillCenter, color);
    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.Right(), dstRect.Y(), 1, dstRect.Height()), m_fillCenter, color);
  }
}
