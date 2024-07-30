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

#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
// #include <FslGraphics/TextureAtlas/TestAtlasTextureGenerator.hpp>
#include <Shared/System/DFNativeBatch2D/TextureAtlasScene.hpp>
#include <algorithm>
#include <array>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const PxSize1D GridCellSize = PxSize1D::Create(128);

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
      constexpr IO::PathView PathMainAtlasSdfFont("Font/SdfAtlas/FontAtlas_Font.fbk");

      constexpr IO::PathView PathSdfAtlas("Font/SdfAtlas/FontAtlas.bta");
      constexpr IO::PathView PathSdfAtlasTexture("Font/SdfAtlas/FontAtlas.png");
    }

    constexpr const std::array<BlendState, 5> BlendStates = {BlendState::AlphaBlend, BlendState::NonPremultiplied, BlendState::Additive,
                                                             BlendState::Opaque, BlendState::Sdf};
    constexpr const std::array<const char*, 5> BlendStatesDesc = {"AlphaBlend (Premultiplied)", "NonPremultiplied AlphaBlend", "Additive", "Opaque",
                                                                  "Sdf"};
  }

  TextureAtlasScene::TextureAtlasScene(const ServiceProvider& serviceProvider, const SpriteNativeAreaCalc& spriteNativeAreaCalc,
                                       const uint32_t densityDpi)
    : m_graphics(serviceProvider.Get<IGraphicsService>())
    , m_batch(m_graphics->GetNativeBatch2D())
    , m_blendStateIndex(0)
    , m_blendState(BlendStates[m_blendStateIndex])
    , m_fillCenter(PxRectangle::Create(16, 16, 1, 1))
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
      if (m_blendStateIndex >= static_cast<int32_t>(BlendStates.size()))
      {
        m_blendStateIndex = 0;
      }
      m_blendState = BlendStates[m_blendStateIndex];
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


  PxSize1D TextureAtlasScene::GetGridOffsetY() const
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
    Point2 dstOffset = Point2(0, m_font.LineSpacingPx().RawValue());


    m_batch->Begin(m_blendState);

    if (m_sceneId == 0)
    {    // Draw the texture atlas
      const PxSize2D atlasSize = atlasTexture.GetSize();
      m_batch->Draw(atlasTexture, Vector2(windowSizePx.RawWidth() - atlasSize.RawWidth(), windowSizePx.RawHeight() - atlasSize.RawHeight()),
                    Colors::White());
    }
    DrawGrid(LocalConfig::GridCellSize, LocalConfig::GridCellSize, windowSizePx.Width(), windowSizePx.Height(), colorTargetBorder, colorTargetCenter,
             dstOffset);

    // Render the caption
    m_batch->DrawString(atlasTexture, m_font, BlendStatesDesc[m_blendStateIndex], Vector2(0, 0), Colors::White());

    if (m_sceneId == 0)
    {
      constexpr auto Size2Px = PxSize1D::Create(2);
      constexpr auto Size3Px = PxSize1D::Create(3);
      constexpr auto Size4Px = PxSize1D::Create(4);
      DrawStrings(atlasTexture, m_font, "Hello world", LocalConfig::GridCellSize * Size3Px);

      DrawAtlasTextureSimpleUsingDstPos(dstOffset);
      dstOffset.X += (LocalConfig::GridCellSize * Size3Px).RawValue();
      dstOffset.Y += (LocalConfig::GridCellSize * Size3Px).RawValue();
      DrawAtlasTextureSimpleUsingDstRectangle(dstOffset);

      dstOffset.X -= (LocalConfig::GridCellSize * Size3Px).RawValue();
      dstOffset.Y += ((LocalConfig::GridCellSize * Size4Px) + (LocalConfig::GridCellSize / Size2Px)).RawValue();

      // Render a simple test string
      m_batch->DrawString(atlasTexture, m_fontSmall, "The quick brown fox jumps over the lazy dog",
                          Vector2(dstOffset.X, dstOffset.Y - m_fontSmall.BaseLinePx().RawValue()), Colors::Cyan());

      {    // Render the sdf test string
        m_batch->ChangeTo(BlendState::Sdf);
        constexpr StringViewLite StrText("Hello World (sdf)");
        auto sizeTextPx = m_fontSdf.MeasureString(StrText);
        const float zoomFactor = ((std::cos(m_zoomFactorAngle) + 1.0f) / 2.0f) * 6.0f;
        BitmapFontConfig fontConfig(zoomFactor);
        auto sizeZoomedTextPx = m_fontSdf.MeasureString(StrText, fontConfig);
        m_batch->DrawString(m_texSdf, m_fontSdf, StrText, Vector2(windowSizePx.RawWidth() - sizeTextPx.RawWidth(), 0), Colors::White());
        m_batch->DrawString(m_texSdf, m_fontSdf, fontConfig, StrText,
                            Vector2(windowSizePx.RawWidth() - sizeZoomedTextPx.RawWidth(), windowSizePx.RawHeight() - sizeZoomedTextPx.RawHeight()),
                            Colors::White());
      }
    }
    else
    {
      DrawAtlasTextureSimpleRotationUsingDstPos(dstOffset);
    }

    m_batch->End();
  }


  void TextureAtlasScene::DrawStrings(const BaseTexture2D& atlasTexture, const TextureAtlasSpriteFont& font, const char* const psz,
                                      const PxSize1D areaWidth)
  {
    const float zoomValue1 = (std::sin(m_zoomAngle1) + 1.0f) / 2.0f;
    const float zoomValue2 = (std::sin(m_zoomAngle2) + 1.0f) / 2.0f;
    Vector2 zoom1(zoomValue1, zoomValue1);
    Vector2 zoom2(zoomValue1, zoomValue2);

    Vector2 dstPosText((LocalConfig::GridCellSize * PxSize1D::Create(7)).RawValue(),
                       (m_font.LineSpacingPx() + LocalConfig::GridCellSize * PxSize1D::Create(3)).RawValue());

    const float gridCellSizeDiv2 = (LocalConfig::GridCellSize.RawValue() / 2.0f);

    m_batch->DrawString(atlasTexture, font, psz, dstPosText, Colors::White());
    dstPosText.Y += gridCellSizeDiv2;

    const auto dimPx = font.MeasureString(psz);
    const auto emptyAreaX = static_cast<float>(areaWidth.RawValue() - dimPx.RawWidth());
    Vector2 originCenter(static_cast<float>(dimPx.RawWidth()) * 0.5f, static_cast<float>(dimPx.RawHeight()) * 0.5f);
    Vector2 originBottomRight(dimPx.RawWidth(), dimPx.RawHeight());

    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + (emptyAreaX * 0.5f), dstPosText.Y), Colors::White());
    dstPosText.Y += gridCellSizeDiv2;

    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + emptyAreaX, dstPosText.Y), Colors::White());
    dstPosText.Y += gridCellSizeDiv2;

    // Zoom: top-left
    m_batch->DrawString(atlasTexture, font, psz, dstPosText, Colors::White(), Vector2(), zoom1);
    dstPosText.Y += gridCellSizeDiv2;
    // Zoom: center
    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + (emptyAreaX * 0.5f) + originCenter.X, dstPosText.Y + originCenter.Y),
                        Colors::White(), originCenter, zoom1);
    dstPosText.Y += gridCellSizeDiv2;
    // Zoom: right
    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + emptyAreaX + originBottomRight.X, dstPosText.Y + originBottomRight.Y),
                        Colors::White(), originBottomRight, zoom1);
    dstPosText.Y += gridCellSizeDiv2;

    // Zoom: top-left
    m_batch->DrawString(atlasTexture, font, psz, dstPosText, Colors::White(), Vector2(), zoom2);
    dstPosText.Y += gridCellSizeDiv2;
    // Zoom: center
    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + (emptyAreaX * 0.5f) + originCenter.X, dstPosText.Y + originCenter.Y),
                        Colors::White(), originCenter, zoom2);
    dstPosText.Y += gridCellSizeDiv2;
    // Zoom: right
    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + emptyAreaX + originBottomRight.X, dstPosText.Y + originBottomRight.Y),
                        Colors::White(), originBottomRight, zoom2);
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
      const Vector2 halfF4(static_cast<float>(atlas4Size.RawWidth()) / 2.0f, static_cast<float>(atlas4Size.RawHeight()) / 2.0f);
      Vector2 offset0 = Vector2(dstOffset.X + LocalConfig::GridCellSize.RawValue() * 1, dstOffset.Y + LocalConfig::GridCellSize.RawValue());
      Vector2 offset1 = Vector2(dstOffset.X + LocalConfig::GridCellSize.RawValue() * 4, dstOffset.Y + LocalConfig::GridCellSize.RawValue());
      Vector2 offset2 = Vector2(dstOffset.X + LocalConfig::GridCellSize.RawValue() * 7, dstOffset.Y + LocalConfig::GridCellSize.RawValue());

      m_batch->Draw(m_atlasTexture4, offset0, Color(64, 64, 64, 255), 0.0f, Vector2(), Vector2::One());
      m_batch->Draw(m_atlasTexture4, offset0, Colors::White(), m_angle, Vector2(), Vector2::One());
      m_batch->Draw(m_atlasTexture4, offset0, Colors::Green(), m_angle, halfF4, Vector2::One());

      m_batch->Draw(m_atlasTexture4, offset1, Color(64, 64, 64, 255), 0.0f, Vector2(), zoom1);
      m_batch->Draw(m_atlasTexture4, offset1, Colors::White(), m_angle, Vector2(), zoom1);
      m_batch->Draw(m_atlasTexture4, offset1, Colors::Green(), m_angle, halfF4, zoom1);

      m_batch->Draw(m_atlasTexture4, offset2, Color(64, 64, 64, 255), 0.0f, Vector2(), zoom2);
      m_batch->Draw(m_atlasTexture4, offset2, Colors::White(), m_angle, Vector2(), zoom2);
      m_batch->Draw(m_atlasTexture4, offset2, Colors::Green(), m_angle, halfF4, zoom2);
    }

    {
      constexpr auto Size2Px = PxSize1D::Create(2);
      constexpr auto Size4Px = PxSize1D::Create(4);
      PxRectangle srcRect(atlas4Size.Width() / Size4Px, atlas4Size.Height() / Size4Px, atlas4Size.Width() / Size2Px, atlas4Size.Height() / Size2Px);
      const Vector2 halfF4(static_cast<float>(srcRect.RawWidth()) / 2.0f, static_cast<float>(srcRect.RawHeight()) / 2.0f);
      Vector2 offset0 = Vector2(dstOffset.X + LocalConfig::GridCellSize.RawValue() * 1, dstOffset.Y + (LocalConfig::GridCellSize.RawValue() * 3));
      Vector2 offset1 = Vector2(dstOffset.X + LocalConfig::GridCellSize.RawValue() * 4, dstOffset.Y + (LocalConfig::GridCellSize.RawValue() * 3));
      Vector2 offset2 = Vector2(dstOffset.X + LocalConfig::GridCellSize.RawValue() * 7, dstOffset.Y + (LocalConfig::GridCellSize.RawValue() * 3));

      m_batch->Draw(m_atlasTexture4, offset0, srcRect, Color(64, 64, 64, 255), 0.0f, Vector2(), Vector2::One());
      m_batch->Draw(m_atlasTexture4, offset0, srcRect, Colors::White(), m_angle, Vector2(), Vector2::One());
      m_batch->Draw(m_atlasTexture4, offset0, srcRect, Colors::Green(), m_angle, halfF4, Vector2::One());

      m_batch->Draw(m_atlasTexture4, offset1, srcRect, Color(64, 64, 64, 255), 0.0f, Vector2(), zoom1);
      m_batch->Draw(m_atlasTexture4, offset1, srcRect, Colors::White(), m_angle, Vector2(), zoom1);
      m_batch->Draw(m_atlasTexture4, offset1, srcRect, Colors::Green(), m_angle, halfF4, zoom1);

      m_batch->Draw(m_atlasTexture4, offset2, srcRect, Color(64, 64, 64, 255), 0.0f, Vector2(), zoom2);
      m_batch->Draw(m_atlasTexture4, offset2, srcRect, Colors::White(), m_angle, Vector2(), zoom2);
      m_batch->Draw(m_atlasTexture4, offset2, srcRect, Colors::Green(), m_angle, halfF4, zoom2);
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

    const auto maxSizeY = static_cast<float>(std::max(atlas1Size.RawHeight(), atlas2Size.RawHeight()));
    const Vector2 offsetY(0.0f, maxSizeY);

    constexpr PxSize1D Size2Px = PxSize1D::Create(2);
    constexpr PxSize1D Size4Px = PxSize1D::Create(4);

    const PxSize2D half1(atlas1Size.Width() / Size2Px, atlas1Size.Height() / Size2Px);
    const PxSize2D half2(atlas2Size.Width() / Size2Px, atlas2Size.Height() / Size2Px);
    const PxSize2D half3(atlas3Size.Width() / Size2Px, atlas3Size.Height() / Size2Px);
    const Vector2 halfF1(static_cast<float>(atlas1Size.RawWidth()) / 2.0f, static_cast<float>(atlas1Size.RawHeight()) / 2.0f);
    const Vector2 halfF2(static_cast<float>(atlas2Size.RawWidth()) / 2.0f, static_cast<float>(atlas2Size.RawHeight()) / 2.0f);
    const Vector2 halfF3(static_cast<float>(atlas3Size.RawWidth()) / 2.0f, static_cast<float>(atlas3Size.RawHeight()) / 2.0f);
    const Vector2 halfF4(static_cast<float>(atlas4Size.RawWidth()) / 2.0f, static_cast<float>(atlas4Size.RawHeight()) / 2.0f);

    const PxSize1D quarterX1 = atlas1Size.Width() / Size4Px;
    const PxSize1D quarterY1 = atlas1Size.Height() / Size4Px;
    const PxSize1D quarterX2 = atlas2Size.Width() / Size4Px;
    const PxSize1D quarterY2 = atlas2Size.Height() / Size4Px;

    const float spacing = 32;

    Vector2 offset1(dstOffset.X, dstOffset.Y);
    Vector2 offset2(offset1.X + static_cast<float>(m_atlasTexture1.GetSize().RawWidth()), offset1.Y);
    Vector2 offset3(offset2.X + static_cast<float>(m_atlasTexture2.GetSize().RawWidth()), offset2.Y);
    Vector2 offset4(dstOffset.X + LocalConfig::GridCellSize.RawValue() * 10, dstOffset.Y + LocalConfig::GridCellSize.RawValue());

    Vector2 offsetA(offset3.X + static_cast<float>(m_atlasTexture3.GetSize().RawWidth()) + spacing, offset3.Y);
    Vector2 offsetB(offsetA.X + static_cast<float>(m_atlasTexture1.GetSize().RawWidth() * 3), offsetA.Y);

    Rectangle srcRect(atlas4Size.RawWidth() / 4, atlas4Size.RawHeight() / 4, atlas4Size.RawWidth() / 2, atlas4Size.RawHeight() / 2);
    m_batch->Draw(m_atlasTexture4, offset4, Color(64, 64, 64, 255), 0.0f, Vector2(), Vector2::One());
    m_batch->Draw(m_atlasTexture4, offset4, Colors::White(), m_angle, Vector2(), Vector2::One());
    m_batch->Draw(m_atlasTexture4, offset4, Colors::Green(), m_angle, halfF4, Vector2::One());

    constexpr const int32_t NumEntries = 3;
    std::array<Vector2, NumEntries> offsets1;
    std::array<Vector2, NumEntries> offsets2;
    const float speed = 0.40f;
    for (int32_t i = 0; i < NumEntries; ++i)
    {
      offsets1[i] = Vector2(offsetA.X + static_cast<float>(i * atlas1Size.RawWidth()),
                            offsetA.Y + 30.0f + std::sin(m_angle + (static_cast<float>(i) * speed)) * 30.0f);
      offsets2[i] = Vector2(offsetB.X + static_cast<float>(i * atlas2Size.RawWidth()),
                            offsetB.Y + 30.0f + std::sin(m_angle + (static_cast<float>(i) * speed)) * 30.0f);
    }

    m_batch->Draw(m_atlasTexture1, offset1, Colors::White());
    m_batch->Draw(m_atlasTexture2, offset2, Colors::White());
    m_batch->Draw(m_atlasTexture1, offsets1.data(), NumEntries, Colors::White());
    m_batch->Draw(m_atlasTexture2, offsets2.data(), NumEntries, Colors::White());

    offset1.Y += maxSizeY;
    offset2.Y += maxSizeY;
    offset3.Y += maxSizeY;
    for (int i = 0; i < NumEntries; ++i)
    {
      offsets1[i] += offsetY;
      offsets2[i] += offsetY;
    }

    m_batch->Draw(m_atlasTexture1, offset1, PxRectangle(PxValue(0), PxValue(0), atlas1Size.Width(), half1.Height()), Colors::White());
    m_batch->Draw(m_atlasTexture2, offset2, PxRectangle(PxValue(0), PxValue(0), atlas2Size.Width(), half2.Height()), Colors::White());
    m_batch->Draw(m_atlasTexture1, offsets1.data(), NumEntries, PxRectangle(PxValue(0), PxValue(0), atlas1Size.Width(), half1.Height()),
                  Colors::White());
    m_batch->Draw(m_atlasTexture2, offsets2.data(), NumEntries, PxRectangle(PxValue(0), PxValue(0), atlas2Size.Width(), half2.Height()),
                  Colors::White());

    offset1.Y += maxSizeY / 2.0f;
    offset2.Y += maxSizeY / 2.0f;
    offset3.Y += maxSizeY / 2.0f;
    for (int i = 0; i < NumEntries; ++i)
    {
      offsets1[i] += offsetY / 2;
      offsets2[i] += offsetY / 2;
    }

    m_batch->Draw(m_atlasTexture1, offset1, PxRectangle(PxValue(0), half1.Height(), atlas1Size.Width(), atlas1Size.Height() - half1.Height()),
                  Colors::White());
    m_batch->Draw(m_atlasTexture2, offset2, PxRectangle(PxValue(0), half2.Height(), atlas2Size.Width(), atlas2Size.Height() - half2.Height()),
                  Colors::White());
    m_batch->Draw(m_atlasTexture1, offsets1.data(), NumEntries,
                  PxRectangle(PxValue(0), half1.Height(), atlas1Size.Width(), atlas1Size.Height() - half1.Width()), Colors::White());
    m_batch->Draw(m_atlasTexture2, offsets2.data(), NumEntries,
                  PxRectangle(PxValue(0), half2.Height(), atlas2Size.Width(), atlas2Size.Height() - half2.Width()), Colors::White());

    offset1.Y += maxSizeY / 2.0f;
    offset2.Y += maxSizeY / 2.0f;
    offset3.Y += maxSizeY / 2.0f;
    for (int i = 0; i < NumEntries; ++i)
    {
      offsets1[i] += offsetY / 2;
      offsets2[i] += offsetY / 2;
    }

    PxRectangle q1(quarterX1, quarterY1, atlas1Size.Width() - (quarterX1 * Size2Px), atlas1Size.Height() - (quarterY1 * Size2Px));
    PxRectangle q2(quarterX2, quarterY2, atlas2Size.Width() - (quarterX2 * Size2Px), atlas2Size.Height() - (quarterY2 * Size2Px));
    PxRectangleU32 q3(TypeConverter::UncheckedTo<PxValueU>(quarterX2), TypeConverter::UncheckedTo<PxValueU>(quarterY2),
                      TypeConverter::UncheckedTo<PxValueU>(atlas2Size.Width() - (quarterX2 * Size2Px)),
                      TypeConverter::UncheckedTo<PxValueU>(atlas2Size.Height() - (quarterY2 * Size2Px)));

    m_batch->Draw(m_atlasTexture1, offset1, q1, Colors::White());
    m_batch->Draw(m_atlasTexture2, offset2, q2, Colors::White());
    m_batch->Draw(m_atlasTexture1, offsets1.data(), NumEntries, q1, Colors::White());
    m_batch->Draw(m_atlasTexture2, offsets2.data(), NumEntries, q2, Colors::White());

    m_batch->Draw(m_atlasTexture2, offset3, q3, Colors::White(), BatchEffect::FlipHorizontal);
    m_batch->Draw(m_atlasTexture2, offset3 - Vector2(0.0f, q3.Height.Value), q3, Colors::White(), BatchEffect::FlipVertical);
    m_batch->Draw(m_atlasTexture2, offset3 - Vector2(0.0f, q3.Height.Value * 2), q3, Colors::White(), BatchEffect::Rotate90Clockwise);
    m_batch->Draw(m_atlasTexture2, offset3 - Vector2(0.0f, q3.Height.Value * 3), q3, Colors::White(), BatchEffect::Rotate180Clockwise);
    m_batch->Draw(m_atlasTexture2, offset3 - Vector2(0.0f, q3.Height.Value * 4), q3, Colors::White(), BatchEffect::Rotate270Clockwise);

    offset1.Y += maxSizeY;
    offset2.Y += maxSizeY;
    offset3.Y += maxSizeY;

    m_batch->Draw(m_atlasTexture1, offset1, Colors::White(), Vector2(), zoom1);
    m_batch->Draw(m_atlasTexture2, offset2, Colors::White(), Vector2(), zoom1);
    m_batch->Draw(m_atlasTexture1, offset3, q1, Colors::White(), Vector2(), zoom1);

    offset1.Y += maxSizeY;
    offset2.Y += maxSizeY;
    offset3.Y += maxSizeY;

    m_batch->Draw(m_atlasTexture1, offset1, Colors::White(), Vector2(), zoom2);
    m_batch->Draw(m_atlasTexture2, offset2, Colors::White(), Vector2(), zoom2);
    m_batch->Draw(m_atlasTexture1, offset3, q1, Colors::White(), Vector2(), zoom2);

    offset1.Y += maxSizeY;
    offset2.Y += maxSizeY;
    offset3.Y += maxSizeY;

    m_batch->Draw(m_atlasTexture1, Vector2(offset1.X + halfF1.X, offset1.Y + halfF1.Y), Colors::White(), Vector2(halfF1.X, halfF1.Y), zoom1);
    m_batch->Draw(m_atlasTexture3, Vector2(offset2.X + halfF3.X, offset2.Y + halfF3.Y), Colors::White(), Vector2(halfF3.X, halfF3.Y), zoom1);
    m_batch->Draw(m_atlasTexture1, Vector2(offset3.X + halfF1.X, offset3.Y + halfF1.Y), q1, Colors::White(),
                  Vector2(static_cast<float>(q1.RawWidth()) * 0.5f, static_cast<float>(q1.RawHeight()) * 0.5f), zoom1);

    offset1.Y += maxSizeY;
    offset2.Y += maxSizeY;
    offset3.Y += maxSizeY;

    m_batch->Draw(m_atlasTexture1, Vector2(offset1.X + halfF1.X, offset1.Y + halfF1.Y), Colors::White(), Vector2(halfF1.X, halfF1.Y), zoom2);
    m_batch->Draw(m_atlasTexture3, Vector2(offset2.X + halfF3.X, offset2.Y + halfF3.Y), Colors::White(), Vector2(halfF3.X, halfF3.Y), zoom2);
    m_batch->Draw(m_atlasTexture1, Vector2(offset3.X + halfF1.X, offset3.Y + halfF1.Y), q1, Colors::White(),
                  Vector2(static_cast<float>(q1.RawWidth()) * 0.5f, static_cast<float>(q1.RawHeight()) * 0.5f), zoom2);
  }


  void TextureAtlasScene::DrawAtlasTextureSimpleUsingDstRectangle(const Point2& dstOffset)
  {
    const float zoomValue1 = (std::sin(m_zoomAngle1) + 1.0f) / 2.0f;
    const float zoomValue2 = (std::sin(m_zoomAngle2) + 1.0f) / 2.0f;
    Vector2 zoom1(zoomValue1, zoomValue1);
    Vector2 zoom2(zoomValue1, zoomValue2);

    const PxSize2D atlas1Size = m_atlasTexture1.GetSize();
    const PxSize2D atlas2Size = m_atlasTexture1.GetSize();
    const PxSize1D maxSizeY = PxSize1D::Max(atlas1Size.Height(), atlas2Size.Height());
    const Vector2 offsetY(0.0f, maxSizeY.RawValue());

    const PxSize1D halfY1 = atlas1Size.Height() / PxSize1D::Create(2);
    const PxSize1D halfY2 = atlas2Size.Height() / PxSize1D::Create(2);

    const PxSize1D quarterX1 = atlas1Size.Width() / PxSize1D::Create(4);
    const PxSize1D quarterY1 = atlas1Size.Height() / PxSize1D::Create(4);
    const PxSize1D quarterX2 = atlas2Size.Width() / PxSize1D::Create(4);
    const PxSize1D quarterY2 = atlas2Size.Height() / PxSize1D::Create(4);

    auto offset1 = PxRectangle::Create(dstOffset.X, dstOffset.Y, static_cast<int32_t>(static_cast<float>(atlas1Size.RawWidth()) * zoomValue1),
                                       static_cast<int32_t>(static_cast<float>(atlas1Size.RawHeight()) * zoomValue1));
    auto offset2 = PxRectangle::Create(offset1.RawX() + m_atlasTexture1.GetSize().RawWidth(), offset1.RawY(),
                                       static_cast<int32_t>(static_cast<float>(atlas2Size.RawWidth()) * zoomValue1),
                                       static_cast<int32_t>(static_cast<float>(atlas2Size.RawHeight()) * zoomValue1));
    auto offset3 = PxRectangle::Create(offset2.RawX() + m_atlasTexture2.GetSize().RawWidth(), offset2.RawY(),
                                       static_cast<int32_t>(static_cast<float>(atlas1Size.RawWidth()) * zoomValue1),
                                       static_cast<int32_t>(static_cast<float>(atlas1Size.RawHeight()) * zoomValue2));
    auto offset4 = PxRectangle::Create(offset3.RawX() + m_atlasTexture1.GetSize().RawWidth(), offset3.RawY(),
                                       static_cast<int32_t>(static_cast<float>(atlas2Size.RawWidth()) * zoomValue1),
                                       static_cast<int32_t>(static_cast<float>(atlas2Size.RawHeight()) * zoomValue2));

    m_batch->Draw(m_atlasTexture1, offset1, Colors::White());
    m_batch->Draw(m_atlasTexture2, offset2, Colors::White());
    m_batch->Draw(m_atlasTexture1, offset3, Colors::White());
    m_batch->Draw(m_atlasTexture2, offset4, Colors::White());

    offset1.AddY(maxSizeY);
    offset2.AddY(maxSizeY);
    offset3.AddY(maxSizeY);
    offset4.AddY(maxSizeY);

    m_batch->Draw(m_atlasTexture1, offset1, PxRectangle(PxValue(0), PxValue(0), atlas1Size.Width(), halfY1), Colors::White());
    m_batch->Draw(m_atlasTexture2, offset2, PxRectangle(PxValue(0), PxValue(0), atlas2Size.Width(), halfY2), Colors::White());
    m_batch->Draw(m_atlasTexture1, offset3, PxRectangle(PxValue(0), PxValue(0), atlas1Size.Width(), halfY1), Colors::White());
    m_batch->Draw(m_atlasTexture2, offset4, PxRectangle(PxValue(0), PxValue(0), atlas2Size.Width(), halfY2), Colors::White());

    offset1.AddY(maxSizeY);
    offset2.AddY(maxSizeY);
    offset3.AddY(maxSizeY);
    offset4.AddY(maxSizeY);

    m_batch->Draw(m_atlasTexture1, offset1, PxRectangle(PxValue(0), halfY1, atlas1Size.Width(), atlas1Size.Height() - halfY1), Colors::White());
    m_batch->Draw(m_atlasTexture2, offset2, PxRectangle(PxValue(0), halfY2, atlas2Size.Width(), atlas2Size.Height() - halfY2), Colors::White());
    m_batch->Draw(m_atlasTexture1, offset3, PxRectangle(PxValue(0), halfY1, atlas1Size.Width(), atlas1Size.Height() - halfY1), Colors::White());
    m_batch->Draw(m_atlasTexture2, offset4, PxRectangle(PxValue(0), halfY2, atlas2Size.Width(), atlas2Size.Height() - halfY2), Colors::White());

    offset1.AddY(maxSizeY);
    offset2.AddY(maxSizeY);
    offset3.AddY(maxSizeY);
    offset4.AddY(maxSizeY);

    constexpr auto Size2Px = PxSize1D::Create(2);
    m_batch->Draw(m_atlasTexture1, offset1,
                  PxRectangle(quarterX1, quarterY1, atlas1Size.Width() - (quarterX1 * Size2Px), atlas1Size.Height() - (quarterY1 * Size2Px)),
                  Colors::White());
    m_batch->Draw(m_atlasTexture2, offset2,
                  PxRectangle(quarterX2, quarterY2, atlas2Size.Width() - (quarterX2 * Size2Px), atlas2Size.Height() - (quarterY2 * Size2Px)),
                  Colors::White());
    m_batch->Draw(m_atlasTexture1, offset3,
                  PxRectangle(quarterX1, quarterY1, atlas1Size.Width() - (quarterX1 * Size2Px), atlas1Size.Height() - (quarterY1 * Size2Px)),
                  Colors::White());
    m_batch->Draw(m_atlasTexture2, offset4,
                  PxRectangle(quarterX2, quarterY2, atlas2Size.Width() - (quarterX2 * Size2Px), atlas2Size.Height() - (quarterY2 * Size2Px)),
                  Colors::White());
  }

  void TextureAtlasScene::DrawGrid(const PxSize1D cellWidth, const PxSize1D cellHeight, const PxSize1D areaWidth, const PxSize1D areaHeight,
                                   const Color colorBorder, const Color colorCenter, const Point2 offset)
  {
    const PxValue startX = offset.X == 0 ? PxValue(0) : PxValue(offset.X) - cellWidth;
    const PxValue startY = offset.Y == 0 ? PxValue(0) : PxValue(offset.Y) - cellHeight;
    const PxSize1D xOffset = cellWidth / PxSize1D::Create(2);
    const PxSize1D yOffset = cellHeight / PxSize1D::Create(2);
    for (PxValue y = startY; y < areaHeight; y += cellHeight)
    {
      m_batch->Draw(m_atlasTextureFill, PxRectangle(PxValue(0), y, areaWidth, PxValue(1)), m_fillCenter, colorBorder);
      m_batch->Draw(m_atlasTextureFill, PxRectangle(PxValue(0), y + yOffset, areaWidth, PxValue(1)), m_fillCenter, colorCenter);
    }

    for (PxValue x = startX; x < areaWidth; x += cellWidth)
    {
      m_batch->Draw(m_atlasTextureFill, PxRectangle(x, PxValue(0), PxValue(1), areaHeight), m_fillCenter, colorBorder);
      m_batch->Draw(m_atlasTextureFill, PxRectangle(x + xOffset, PxValue(0), PxValue(1), areaHeight), m_fillCenter, colorCenter);
    }
  }


  void TextureAtlasScene::DrawTargetingRect(const PxRectangle& dstRect, const Color& colorBorder, const Color& colorCenter)
  {
    constexpr auto Size1Px = PxSize1D::Create(1);
    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.X(), dstRect.GetCenter().Y, dstRect.Width(), Size1Px), m_fillCenter, colorCenter);
    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.GetCenter().X, dstRect.Y(), Size1Px, dstRect.Height()), m_fillCenter, colorCenter);

    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.X(), dstRect.Y(), dstRect.Width(), Size1Px), m_fillCenter, colorBorder);
    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.X(), dstRect.Y(), Size1Px, dstRect.Height()), m_fillCenter, colorBorder);
  }


  void TextureAtlasScene::DrawRect(const PxRectangle& dstRect, const Color& color)
  {
    constexpr auto Size1Px = PxSize1D::Create(1);
    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.X(), dstRect.Y(), dstRect.Width(), Size1Px), m_fillCenter, color);
    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.X(), dstRect.Bottom(), dstRect.Width(), Size1Px), m_fillCenter, color);
    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.X(), dstRect.Y(), Size1Px, dstRect.Height()), m_fillCenter, color);
    m_batch->Draw(m_atlasTextureFill, PxRectangle(dstRect.Right(), dstRect.Y(), Size1Px, dstRect.Height()), m_fillCenter, color);
  }
}
