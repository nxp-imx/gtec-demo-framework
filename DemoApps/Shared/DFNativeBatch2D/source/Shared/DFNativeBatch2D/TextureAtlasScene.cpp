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
#include <FslDemoApp/Base/Service/Graphics/IGraphicsService.hpp>
#include <Shared/DFNativeBatch2D/TextureAtlasScene.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
#include <algorithm>

namespace Fsl
{
  namespace
  {
    const int32_t NUM_BLENDSTATES = 4;
    BlendState g_blendStates[NUM_BLENDSTATES] = { BlendState::AlphaBlend, BlendState::NonPremultiplied, BlendState::Additive, BlendState::Opaque };
    const char* g_blendStatesDesc[NUM_BLENDSTATES] = { "AlphaBlend (Premultiplied)", "NonPremultiplied AlphaBlend", "Additive", "Opaque" };

    const int32_t GRID_CELL_SIZE = 128;
  }

  TextureAtlasScene::TextureAtlasScene(const ServiceProvider& serviceProvider)
    : m_graphics(serviceProvider.Get<IGraphicsService>())
    , m_batch(m_graphics->GetNativeBatch2D())
    , m_blendStateIndex(0)
    , m_blendState(g_blendStates[m_blendStateIndex])
    , m_font()
    , m_fontSmall()
    , m_atlasTexture1()
    , m_atlasTexture2()
    , m_atlasTexture3()
    , m_atlasTexture4()
    , m_atlasTextureFill()
    , m_fillCenter(16, 16, 1, 1)
    , m_angle(0.0f)
    , m_zoomAngle1(0.0f)
    , m_zoomAngle2(0.0f)
    , m_sceneId(0)
  {
    const std::shared_ptr<IContentManager> contentManager = serviceProvider.Get<IContentManager>();
    {
      BasicTextureAtlas atlas;
      contentManager->Read(atlas, "MainAtlas.bta");

      Fsl::Bitmap bitmap;
      contentManager->Read(bitmap, "MainAtlas.png", PixelFormat::R8G8B8A8_UNORM);
      Texture2D tex(m_graphics->GetNativeGraphics(), bitmap, Texture2DFilterHint::Smooth);
      AtlasTextureInfo atlasTextureInfo1 = TextureAtlasHelper::GetAtlasTextureInfo(atlas, "QuestquestGameIcons/cutepriest");
      AtlasTextureInfo atlasTextureInfo2 = TextureAtlasHelper::GetAtlasTextureInfo(atlas, "QuestquestGameIcons/boardgamewizard");
      AtlasTextureInfo atlasTextureInfo3 = TextureAtlasHelper::GetAtlasTextureInfo(atlas, "Test");
      AtlasTextureInfo atlasTextureInfo4 = TextureAtlasHelper::GetAtlasTextureInfo(atlas, "Test2");
      AtlasTextureInfo atlasTextureInfoFill = TextureAtlasHelper::GetAtlasTextureInfo(atlas, "Fill");
      m_atlasTexture1.Reset(tex, atlasTextureInfo1);
      m_atlasTexture2.Reset(tex, atlasTextureInfo2);
      m_atlasTexture3.Reset(tex, atlasTextureInfo3);
      m_atlasTexture4.Reset(tex, atlasTextureInfo4);
      m_atlasTextureFill.Reset(tex, atlasTextureInfoFill);

      BasicFontKerning fontBasicKerning;
      BasicFontKerning smallFontBasicKerning;
      contentManager->Read(fontBasicKerning, "MainAtlasFont.fbk");
      contentManager->Read(smallFontBasicKerning, "MainAtlasSmallFont.fbk");

      m_font.Reset(atlas, fontBasicKerning);
      m_fontSmall.Reset(atlas, smallFontBasicKerning);
    }
  }


  void TextureAtlasScene::OnKeyEvent(const KeyEvent& event)
  {
    if (!event.IsPressed())
      return;

    switch (event.GetKey())
    {
    case VirtualKey::Space:
      event.Handled();
      ++m_blendStateIndex;
      if (m_blendStateIndex >= NUM_BLENDSTATES)
        m_blendStateIndex = 0;
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
    return m_font.LineSpacing();
  }


  void TextureAtlasScene::Update(const DemoTime& demoTime)
  {
    m_angle += 1.0f * demoTime.DeltaTime;
    m_zoomAngle1 += 1.0f * demoTime.DeltaTime;
    m_zoomAngle2 += 0.75f * demoTime.DeltaTime;
  }


  void TextureAtlasScene::Draw(const Point2& screenResolution)
  {
    Color colorTargetBorder(64, 64, 64, 255);
    Color colorTargetCenter(100, 100, 100, 255);

    const Texture2D atlasTexture = m_atlasTexture1.GetAtlasTexture();
    Point2 dstOffset = Point2(0, m_font.LineSpacing());

    m_batch->Begin(m_blendState);

    if ( m_sceneId == 0)
    { // Draw the texture atlas
      const Point2 atlasSize = atlasTexture.GetSize();
      m_batch->Draw(atlasTexture, Vector2(screenResolution.X - atlasSize.X, screenResolution.Y - atlasSize.Y), Color::White());
    }
    DrawGrid(GRID_CELL_SIZE, GRID_CELL_SIZE, screenResolution.X, screenResolution.Y, colorTargetBorder, colorTargetCenter, dstOffset);

    // Render the caption
    m_batch->DrawString(atlasTexture, m_font, g_blendStatesDesc[m_blendStateIndex], Vector2(0, 0), Color::White());


    if (m_sceneId == 0)
    {
      DrawStrings(atlasTexture, m_font, "Hello world", GRID_CELL_SIZE * 3);

      DrawAtlasTextureSimpleUsingDstPos(dstOffset);
      dstOffset.X += GRID_CELL_SIZE * 3;
      dstOffset.Y += GRID_CELL_SIZE * 3;
      DrawAtlasTextureSimpleUsingDstRectangle(dstOffset);

      dstOffset.X -= GRID_CELL_SIZE * 3;
      dstOffset.Y += (GRID_CELL_SIZE * 4) + (GRID_CELL_SIZE / 2);

      // Render a simple test string
      m_batch->DrawString(atlasTexture, m_fontSmall, "The quick brown fox jumps over the lazy dog", Vector2(dstOffset.X, dstOffset.Y - m_fontSmall.BaseLine()), Color::Cyan());
    }
    else
    {
      DrawAtlasTextureSimpleRotationUsingDstPos(dstOffset);
    }

    m_batch->End();

  }


  void TextureAtlasScene::DrawStrings(const Texture2D& atlasTexture, const TextureAtlasBitmapFont& font, const char*const psz, const int32_t areaWidth)
  {
    const float zoomValue1 = (std::sin(m_zoomAngle1) + 1.0f) / 2.0f;
    const float zoomValue2 = (std::sin(m_zoomAngle2) + 1.0f) / 2.0f;
    Vector2 zoom1(zoomValue1, zoomValue1);
    Vector2 zoom2(zoomValue1, zoomValue2);

    Vector2 dstPosText(GRID_CELL_SIZE * 7, m_font.LineSpacing() + GRID_CELL_SIZE * 3);

    m_batch->DrawString(atlasTexture, font, psz, dstPosText, Color::White());
    dstPosText.Y += (GRID_CELL_SIZE / 2);

    const Point2 dim = font.MeasureString(psz);
    const float emptyAreaX = float(areaWidth - dim.X);
    Vector2 originCenter(dim.X * 0.5f, dim.Y * 0.5f);
    Vector2 originBottomRight(dim.X, dim.Y);

    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + (emptyAreaX * 0.5f), dstPosText.Y), Color::White());
    dstPosText.Y += (GRID_CELL_SIZE / 2);

    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + emptyAreaX, dstPosText.Y), Color::White());
    dstPosText.Y += (GRID_CELL_SIZE / 2);

    // Zoom: top-left
    m_batch->DrawString(atlasTexture, font, psz, dstPosText, Color::White(), Vector2(), zoom1);
    dstPosText.Y += (GRID_CELL_SIZE / 2);
    // Zoom: center
    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + (emptyAreaX * 0.5f) + originCenter.X, dstPosText.Y + originCenter.Y), Color::White(), originCenter, zoom1);
    dstPosText.Y += (GRID_CELL_SIZE / 2);
    // Zoom: right
    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + emptyAreaX + originBottomRight.X, dstPosText.Y + originBottomRight.Y), Color::White(), originBottomRight, zoom1);
    dstPosText.Y += (GRID_CELL_SIZE / 2);

    // Zoom: top-left
    m_batch->DrawString(atlasTexture, font, psz, dstPosText, Color::White(), Vector2(), zoom2);
    dstPosText.Y += (GRID_CELL_SIZE / 2);
    // Zoom: center
    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + (emptyAreaX * 0.5f) + originCenter.X, dstPosText.Y + originCenter.Y), Color::White(), originCenter, zoom2);
    dstPosText.Y += (GRID_CELL_SIZE / 2);
    // Zoom: right
    m_batch->DrawString(atlasTexture, font, psz, Vector2(dstPosText.X + emptyAreaX + originBottomRight.X, dstPosText.Y + originBottomRight.Y), Color::White(), originBottomRight, zoom2);
    dstPosText.Y += (GRID_CELL_SIZE / 2);
  }


  void TextureAtlasScene::DrawAtlasTextureSimpleRotationUsingDstPos(const Point2& dstOffset)
  {
    const float zoomValue1 = (std::sin(m_zoomAngle1) + 1.0f) / 2.0f;
    const float zoomValue2 = (std::sin(m_zoomAngle2) + 1.0f) / 2.0f;
    Vector2 zoom1(zoomValue1, zoomValue1);
    Vector2 zoom2(zoomValue1, zoomValue2);

    const Point2 atlas4Size = m_atlasTexture4.GetSize();

    {
      const Vector2 halfF4(atlas4Size.X / 2.0f, atlas4Size.Y / 2.0f);
      Vector2 offset0 = Vector2(dstOffset.X + GRID_CELL_SIZE * 1, dstOffset.Y + GRID_CELL_SIZE);
      Vector2 offset1 = Vector2(dstOffset.X + GRID_CELL_SIZE * 4, dstOffset.Y + GRID_CELL_SIZE);
      Vector2 offset2 = Vector2(dstOffset.X + GRID_CELL_SIZE * 7, dstOffset.Y + GRID_CELL_SIZE);

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
      Rectangle srcRect(atlas4Size.X / 4, atlas4Size.Y / 4, atlas4Size.X / 2, atlas4Size.Y / 2);
      const Vector2 halfF4(srcRect.Width() / 2.0f, srcRect.Height() / 2.0f);
      Vector2 offset0 = Vector2(dstOffset.X + GRID_CELL_SIZE * 1, dstOffset.Y + (GRID_CELL_SIZE * 3));
      Vector2 offset1 = Vector2(dstOffset.X + GRID_CELL_SIZE * 4, dstOffset.Y + (GRID_CELL_SIZE*3));
      Vector2 offset2 = Vector2(dstOffset.X + GRID_CELL_SIZE * 7, dstOffset.Y + (GRID_CELL_SIZE*3));

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

    const Point2 atlas1Size = m_atlasTexture1.GetSize();
    const Point2 atlas2Size = m_atlasTexture2.GetSize();
    const Point2 atlas3Size = m_atlasTexture3.GetSize();
    const Point2 atlas4Size = m_atlasTexture4.GetSize();

    const int maxSizeY = std::max(atlas1Size.Y, atlas2Size.Y);
    const Vector2 offsetY(0.0f, maxSizeY);


    const Point2 half1(atlas1Size.X / 2, atlas1Size.Y / 2);
    const Point2 half2(atlas2Size.X / 2, atlas2Size.Y / 2);
    const Point2 half3(atlas3Size.X / 2, atlas3Size.Y / 2);
    const Vector2 halfF1(atlas1Size.X / 2.0f, atlas1Size.Y / 2.0f);
    const Vector2 halfF2(atlas2Size.X / 2.0f, atlas2Size.Y / 2.0f);
    const Vector2 halfF3(atlas3Size.X / 2.0f, atlas3Size.Y / 2.0f);
    const Vector2 halfF4(atlas4Size.X / 2.0f, atlas4Size.Y / 2.0f);

    const int quarterX1 = atlas1Size.X / 4;
    const int quarterY1 = atlas1Size.Y / 4;
    const int quarterX2 = atlas2Size.X / 4;
    const int quarterY2 = atlas2Size.Y / 4;

    const float spacing = 32;
    const int numEntries = 3;

    Vector2 offset1 = Vector2(dstOffset.X, dstOffset.Y);
    Vector2 offset2 = Vector2(offset1.X + m_atlasTexture1.GetSize().X, offset1.Y);
    Vector2 offset3 = Vector2(offset2.X + m_atlasTexture2.GetSize().X, offset2.Y);
    Vector2 offset4 = Vector2(dstOffset.X + GRID_CELL_SIZE * 10, dstOffset.Y + GRID_CELL_SIZE);

    Vector2 offsetA = Vector2(offset3.X + m_atlasTexture3.GetSize().X + spacing, offset3.Y);
    Vector2 offsetB = Vector2(offsetA.X + m_atlasTexture1.GetSize().X * 3, offsetA.Y);

    Rectangle srcRect(atlas4Size.X / 4, atlas4Size.Y / 4, atlas4Size.X / 2, atlas4Size.Y / 2);
    m_batch->Draw(m_atlasTexture4, offset4, Color(64, 64, 64, 255), 0.0f, Vector2(), Vector2::One());
    m_batch->Draw(m_atlasTexture4, offset4, Color::White(), m_angle, Vector2(), Vector2::One());
    m_batch->Draw(m_atlasTexture4, offset4, Color::Green(), m_angle, halfF4, Vector2::One());

    Vector2 offsets1[numEntries];
    Vector2 offsets2[numEntries];
    const float speed = 0.40f;
    for (int i = 0; i < numEntries; ++i)
    {
      offsets1[i] = Vector2(offsetA.X + (i * atlas1Size.X), offsetA.Y + 30.0f + std::sin(m_angle + i*speed) * 30.0f);
      offsets2[i] = Vector2(offsetB.X + (i * atlas2Size.X), offsetB.Y + 30.0f + std::sin(m_angle + i*speed) * 30.0f);
    }

    m_batch->Draw(m_atlasTexture1, offset1, Color::White());
    m_batch->Draw(m_atlasTexture2, offset2, Color::White());
    m_batch->Draw(m_atlasTexture1, offsets1, numEntries, Color::White());
    m_batch->Draw(m_atlasTexture2, offsets2, numEntries, Color::White());

    offset1.Y += maxSizeY;
    offset2.Y += maxSizeY;
    offset3.Y += maxSizeY;
    for (int i = 0; i < numEntries; ++i)
    {
      offsets1[i] += offsetY;
      offsets2[i] += offsetY;
    }

    m_batch->Draw(m_atlasTexture1, offset1, Rectangle(0, 0, atlas1Size.X, half1.Y), Color::White());
    m_batch->Draw(m_atlasTexture2, offset2, Rectangle(0, 0, atlas2Size.X, half2.Y), Color::White());
    m_batch->Draw(m_atlasTexture1, offsets1, numEntries, Rectangle(0, 0, atlas1Size.X, half1.Y), Color::White());
    m_batch->Draw(m_atlasTexture2, offsets2, numEntries, Rectangle(0, 0, atlas2Size.X, half2.Y), Color::White());

    offset1.Y += maxSizeY / 2;
    offset2.Y += maxSizeY / 2;
    offset3.Y += maxSizeY / 2;
    for (int i = 0; i < numEntries; ++i)
    {
      offsets1[i] += offsetY / 2;
      offsets2[i] += offsetY / 2;
    }

    m_batch->Draw(m_atlasTexture1, offset1, Rectangle(0, half1.Y, atlas1Size.X, atlas1Size.Y - half1.Y), Color::White());
    m_batch->Draw(m_atlasTexture2, offset2, Rectangle(0, half2.Y, atlas2Size.X, atlas2Size.Y - half2.Y), Color::White());
    m_batch->Draw(m_atlasTexture1, offsets1, numEntries, Rectangle(0, half1.Y, atlas1Size.X, atlas1Size.Y - half1.Y), Color::White());
    m_batch->Draw(m_atlasTexture2, offsets2, numEntries, Rectangle(0, half2.Y, atlas2Size.X, atlas2Size.Y - half2.Y), Color::White());

    offset1.Y += maxSizeY / 2;
    offset2.Y += maxSizeY / 2;
    offset3.Y += maxSizeY / 2;
    for (int i = 0; i < numEntries; ++i)
    {
      offsets1[i] += offsetY / 2;
      offsets2[i] += offsetY / 2;
    }

    Rectangle q1 = Rectangle(quarterX1, quarterY1, atlas1Size.X - (quarterX1 * 2), atlas1Size.Y - (quarterY1 * 2));
    Rectangle q2 = Rectangle(quarterX2, quarterY2, atlas2Size.X - (quarterX2 * 2), atlas2Size.Y - (quarterY2 * 2));

    m_batch->Draw(m_atlasTexture1, offset1, q1, Color::White());
    m_batch->Draw(m_atlasTexture2, offset2, q2, Color::White());
    m_batch->Draw(m_atlasTexture1, offsets1, numEntries, q1, Color::White());
    m_batch->Draw(m_atlasTexture2, offsets2, numEntries, q2, Color::White());

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
    m_batch->Draw(m_atlasTexture1, Vector2(offset3.X + halfF1.X, offset3.Y + halfF1.Y), q1, Color::White(), Vector2(q1.Width() * 0.5f, q1.Height() * 0.5f), zoom1);

    offset1.Y += maxSizeY;
    offset2.Y += maxSizeY;
    offset3.Y += maxSizeY;

    m_batch->Draw(m_atlasTexture1, Vector2(offset1.X + halfF1.X, offset1.Y + halfF1.Y), Color::White(), Vector2(halfF1.X, halfF1.Y), zoom2);
    m_batch->Draw(m_atlasTexture3, Vector2(offset2.X + halfF3.X, offset2.Y + halfF3.Y), Color::White(), Vector2(halfF3.X, halfF3.Y), zoom2);
    m_batch->Draw(m_atlasTexture1, Vector2(offset3.X + halfF1.X, offset3.Y + halfF1.Y), q1, Color::White(), Vector2(q1.Width() * 0.5f, q1.Height() * 0.5f), zoom2);
  }


  void TextureAtlasScene::DrawAtlasTextureSimpleUsingDstRectangle(const Point2& dstOffset)
  {
    const float zoomValue1 = (std::sin(m_zoomAngle1) + 1.0f) / 2.0f;
    const float zoomValue2 = (std::sin(m_zoomAngle2) + 1.0f) / 2.0f;
    Vector2 zoom1(zoomValue1, zoomValue1);
    Vector2 zoom2(zoomValue1, zoomValue2);

    const Point2 atlas1Size = m_atlasTexture1.GetSize();
    const Point2 atlas2Size = m_atlasTexture1.GetSize();
    const int maxSizeY = std::max(atlas1Size.Y, atlas2Size.Y);
    const Vector2 offsetY(0.0f, maxSizeY);

    const int halfY1 = atlas1Size.Y / 2;
    const int halfY2 = atlas2Size.Y / 2;

    const int quarterX1 = atlas1Size.X / 4;
    const int quarterY1 = atlas1Size.Y / 4;
    const int quarterX2 = atlas2Size.X / 4;
    const int quarterY2 = atlas2Size.Y / 4;

    Rectangle offset1 = Rectangle(dstOffset.X, dstOffset.Y, int32_t(atlas1Size.X * zoomValue1), int32_t(atlas1Size.Y * zoomValue1));
    Rectangle offset2 = Rectangle(offset1.X() + m_atlasTexture1.GetSize().X, offset1.Y(), int32_t(atlas2Size.X * zoomValue1), int32_t(atlas2Size.Y * zoomValue1));
    Rectangle offset3 = Rectangle(offset2.X() + m_atlasTexture2.GetSize().X, offset2.Y(), int32_t(atlas1Size.X * zoomValue1), int32_t(atlas1Size.Y * zoomValue2));
    Rectangle offset4 = Rectangle(offset3.X() + m_atlasTexture1.GetSize().X, offset3.Y(), int32_t(atlas2Size.X * zoomValue1), int32_t(atlas2Size.Y * zoomValue2));

    m_batch->Draw(m_atlasTexture1, offset1, Color::White());
    m_batch->Draw(m_atlasTexture2, offset2, Color::White());
    m_batch->Draw(m_atlasTexture1, offset3, Color::White());
    m_batch->Draw(m_atlasTexture2, offset4, Color::White());

    offset1.AddY(maxSizeY);
    offset2.AddY(maxSizeY);
    offset3.AddY(maxSizeY);
    offset4.AddY(maxSizeY);

    m_batch->Draw(m_atlasTexture1, offset1, Rectangle(0, 0, atlas1Size.X, halfY1), Color::White());
    m_batch->Draw(m_atlasTexture2, offset2, Rectangle(0, 0, atlas2Size.X, halfY2), Color::White());
    m_batch->Draw(m_atlasTexture1, offset3, Rectangle(0, 0, atlas1Size.X, halfY1), Color::White());
    m_batch->Draw(m_atlasTexture2, offset4, Rectangle(0, 0, atlas2Size.X, halfY2), Color::White());

    offset1.AddY(maxSizeY);
    offset2.AddY(maxSizeY);
    offset3.AddY(maxSizeY);
    offset4.AddY(maxSizeY);

    m_batch->Draw(m_atlasTexture1, offset1, Rectangle(0, halfY1, atlas1Size.X, atlas1Size.Y - halfY1), Color::White());
    m_batch->Draw(m_atlasTexture2, offset2, Rectangle(0, halfY2, atlas2Size.X, atlas2Size.Y - halfY2), Color::White());
    m_batch->Draw(m_atlasTexture1, offset3, Rectangle(0, halfY1, atlas1Size.X, atlas1Size.Y - halfY1), Color::White());
    m_batch->Draw(m_atlasTexture2, offset4, Rectangle(0, halfY2, atlas2Size.X, atlas2Size.Y - halfY2), Color::White());

    offset1.AddY(maxSizeY);
    offset2.AddY(maxSizeY);
    offset3.AddY(maxSizeY);
    offset4.AddY(maxSizeY);

    m_batch->Draw(m_atlasTexture1, offset1, Rectangle(quarterX1, quarterY1, atlas1Size.X - (quarterX1 * 2), atlas1Size.Y - (quarterY1 * 2)), Color::White());
    m_batch->Draw(m_atlasTexture2, offset2, Rectangle(quarterX2, quarterY2, atlas2Size.X - (quarterX2 * 2), atlas2Size.Y - (quarterY2 * 2)), Color::White());
    m_batch->Draw(m_atlasTexture1, offset3, Rectangle(quarterX1, quarterY1, atlas1Size.X - (quarterX1 * 2), atlas1Size.Y - (quarterY1 * 2)), Color::White());
    m_batch->Draw(m_atlasTexture2, offset4, Rectangle(quarterX2, quarterY2, atlas2Size.X - (quarterX2 * 2), atlas2Size.Y - (quarterY2 * 2)), Color::White());
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
      m_batch->Draw(m_atlasTextureFill, Rectangle(0, y, areaWidth, 1), m_fillCenter, colorBorder);
      m_batch->Draw(m_atlasTextureFill, Rectangle(0, y + yOffset, areaWidth, 1), m_fillCenter, colorCenter);
    }

    for (int32_t x = startX; x < areaWidth; x += cellWidth)
    {
      m_batch->Draw(m_atlasTextureFill, Rectangle(x, 0, 1, areaHeight), m_fillCenter, colorBorder);
      m_batch->Draw(m_atlasTextureFill, Rectangle(x+ xOffset, 0, 1, areaHeight), m_fillCenter, colorCenter);
    }
  }


  void TextureAtlasScene::DrawTargetingRect(const Rectangle& dstRect, const Color& colorBorder, const Color& colorCenter)
  {
    m_batch->Draw(m_atlasTextureFill, Rectangle(dstRect.X(), dstRect.GetCenter().Y, dstRect.Width(), 1), m_fillCenter, colorCenter);
    m_batch->Draw(m_atlasTextureFill, Rectangle(dstRect.GetCenter().X, dstRect.Y(), 1, dstRect.Height()), m_fillCenter, colorCenter);

    m_batch->Draw(m_atlasTextureFill, Rectangle(dstRect.X(), dstRect.Y(), dstRect.Width(), 1), m_fillCenter, colorBorder);
    m_batch->Draw(m_atlasTextureFill, Rectangle(dstRect.X(), dstRect.Y(), 1, dstRect.Height()), m_fillCenter, colorBorder);
  }


  void TextureAtlasScene::DrawRect(const Rectangle& dstRect, const Color& color)
  {
    m_batch->Draw(m_atlasTextureFill, Rectangle(dstRect.X(), dstRect.Y(), dstRect.Width(), 1), m_fillCenter, color);
    m_batch->Draw(m_atlasTextureFill, Rectangle(dstRect.X(), dstRect.Bottom(), dstRect.Width(), 1), m_fillCenter, color);
    m_batch->Draw(m_atlasTextureFill, Rectangle(dstRect.X(), dstRect.Y(), 1, dstRect.Height()), m_fillCenter, color);
    m_batch->Draw(m_atlasTextureFill, Rectangle(dstRect.Right(), dstRect.Y(), 1, dstRect.Height()), m_fillCenter, color);
  }
}
