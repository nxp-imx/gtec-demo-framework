/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <Shared/UI/PixelPerfect/Shared.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslSimpleUI/Base/Control/BackgroundNineSlice.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/LabelNineSliceButton.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr const float DefaultOffset = 0.5f;
      constexpr const float SliderSpeed = 0.05f;
    }
  }

  Shared::Shared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_nativeBatch(m_graphics->GetNativeBatch2D())
  {
    auto contentManager = config.DemoServiceProvider.Get<IContentManager>();

    {    // Prepare the test graphics atlas
      BasicTextureAtlas textureAtlas;
      contentManager->Read(textureAtlas, "TestGraphics/TestGraphicsAtlas.bta");
      m_testAtlas.AtlasMap = TextureAtlasMap(textureAtlas);

      auto texture = contentManager->ReadTexture("TestGraphics/TestGraphicsAtlas.png", PixelFormat::R8G8B8A8_UNORM);
      m_testAtlas.Texture.Reset(m_graphics->GetNativeGraphics(), texture, Texture2DFilterHint::Smooth);

      // Extract the atlas textures we need
      m_testTextures.RedBlue160dp = m_testAtlas.GetAtlasTexture("Scale/RedBlue/Test64x64_160dpi");
      m_testTextures.RedBlue240dp = m_testAtlas.GetAtlasTexture("Scale/RedBlue/Test64x64_240dpi");
      m_testTextures.RedBlue320dp = m_testAtlas.GetAtlasTexture("Scale/RedBlue/Test64x64_320dpi");
      m_testTextures.RedBlue480dp = m_testAtlas.GetAtlasTexture("Scale/RedBlue/Test64x64_480dpi");
      m_testTextures.RedBlue640dp = m_testAtlas.GetAtlasTexture("Scale/RedBlue/Test64x64_640dpi");

      m_testTextures.Text160dp = m_testAtlas.GetAtlasTexture("Scale/Text2/Test64x64_160dpi");
      m_testTextures.Text240dp = m_testAtlas.GetAtlasTexture("Scale/Text2/Test64x64_240dpi");
      m_testTextures.Text320dp = m_testAtlas.GetAtlasTexture("Scale/Text2/Test64x64_320dpi");
      m_testTextures.Text480dp = m_testAtlas.GetAtlasTexture("Scale/Text2/Test64x64_480dpi");
      m_testTextures.Text640dp = m_testAtlas.GetAtlasTexture("Scale/Text2/Test64x64_640dpi");
    }


    {    // Build a simple UI
      auto windowContext = m_uiExtension->GetContext();
      auto uiFactory = UI::Theme::BasicThemeFactory(windowContext, m_uiExtension->GetSpriteResourceManager(), m_uiExtension->GetDefaultMaterialId());

      auto labelSlider = uiFactory.CreateLabel("Pixel offset:");
      labelSlider->SetAlignmentY(UI::ItemAlignment::Center);

      m_slider = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, ConstrainedValue<float>(LocalConfig::DefaultOffset, 0.0f, 1.0f));
      m_slider->SetAlignmentY(UI::ItemAlignment::Center);

      m_btnDefault = uiFactory.CreateTextButton(UI::Theme::ButtonType::Contained, "Set defaults");

      auto bottomGrid = std::make_shared<UI::GridLayout>(windowContext);
      bottomGrid->SetAlignmentX(UI::ItemAlignment::Stretch);
      bottomGrid->SetAlignmentY(UI::ItemAlignment::Stretch);
      bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 10));
      bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
      bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1));
      bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
      bottomGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 10));
      bottomGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
      bottomGrid->AddChild(labelSlider, 1, 0);
      bottomGrid->AddChild(m_slider, 2, 0);
      bottomGrid->AddChild(m_btnDefault, 3, 0);

      auto background = uiFactory.CreateBottomBar(bottomGrid);

      // Create the root layout and add it to the window manager
      auto fillLayout = std::make_shared<UI::FillLayout>(windowContext);
      fillLayout->AddChild(background);

      // Register the root layout with the window manager
      m_uiExtension->GetWindowManager()->Add(fillLayout);
    }
  }


  Shared::~Shared() = default;


  void Shared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::LeftArrow:
      DecreaseSlider();
      break;
    case VirtualKey::RightArrow:
      IncreaseSlider();
      break;
    case VirtualKey::Space:
      SetDefaultValues();
      break;
    default:
      break;
    }
  }


  void Shared::OnSelect(const UI::RoutedEventArgs& /*args*/, const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_btnDefault)
    {
      SetDefaultValues();
    }
  }


  void Shared::Draw()
  {
    m_nativeBatch->Begin();

    constexpr const PxPoint2 spacePx(20, 20);

    const float errorOffsetPxf = m_slider->GetValue();

    PxPoint2 offsetPx = spacePx;
    auto sizePx = DrawScaleMatrix(offsetPx, spacePx, m_testTextures.Text320dp, Color::White(), errorOffsetPxf);
    offsetPx.X += sizePx.X;
    sizePx = DrawScaleMatrix(offsetPx, spacePx, m_testTextures.RedBlue320dp, Color::White(), errorOffsetPxf);
    m_nativeBatch->End();

    m_uiExtension->Draw();
  }

  PxPoint2 Shared::DrawScaleMatrix(const PxPoint2& offsetPx, const PxPoint2& spacePx, const AtlasTexture2D& srcTexture, const Color& color,
                                   const float errorOffsetPxf)
  {
    //              FULL SIZE   FULL SIZE    -0.5 size    -0.5 size
    // Y       X  | 0.0, 0.0f | 0.5f, 0.0f | 0.0f, 0.5f | 0.5f, 0.5f
    //------------|-----------|------------|------------|------------
    // 0.0f, 0.0f |           |            |            |
    // 0.5f, 0.0f |           |            |            |
    // 0.0f, 0.5f |           |            |            |
    // 0.5f, 0.5f |           |            |            |

    // constexpr const int GRID_X_SPACE = 20;
    // constexpr const int GRID_Y_SPACE = 20;

    auto font = m_uiExtension->GetContext()->DefaultFont;

    const auto texSize = srcTexture.GetSize();

    const PxPoint2 cellSize(texSize + spacePx);

    const int offsetYPx = offsetPx.Y + font->GetInfo().ScaledLineSpacingPx;

    const int gridX0Correct = (offsetPx.X);
    const int gridX1Correct = (offsetPx.X + (cellSize.X * 1));
    const int gridX2Correct = (offsetPx.X + (cellSize.X * 2));
    const int gridX3Correct = (offsetPx.X + (cellSize.X * 3));
    const int gridX4Correct = (offsetPx.X + (cellSize.X * 4));

    const int gridY0Correct = (offsetYPx);
    const int gridY1Correct = (offsetYPx + (cellSize.Y * 1));
    const int gridY2Correct = (offsetYPx + (cellSize.Y * 2));
    const int gridY3Correct = (offsetYPx + (cellSize.Y * 3));
    const int gridY4Correct = (offsetYPx + (cellSize.Y * 4));


    const auto gridX0 = static_cast<float>(gridX0Correct);
    const auto gridX1 = static_cast<float>(gridX1Correct) + errorOffsetPxf;
    const auto gridX2 = static_cast<float>(gridX2Correct);
    const auto gridX3 = static_cast<float>(gridX3Correct) + errorOffsetPxf;

    const auto gridY0 = static_cast<float>(gridY0Correct);
    const auto gridY1 = static_cast<float>(gridY1Correct) + errorOffsetPxf;
    const auto gridY2 = static_cast<float>(gridY2Correct);
    const auto gridY3 = static_cast<float>(gridY3Correct) + errorOffsetPxf;

    const auto gridWidth0 = static_cast<float>(texSize.Width());
    const auto gridWidth1 = static_cast<float>(texSize.Width());
    const auto gridWidth2 = static_cast<float>(texSize.Width()) - errorOffsetPxf;
    const auto gridWidth3 = static_cast<float>(texSize.Width()) - errorOffsetPxf;

    const auto gridHeight0 = static_cast<float>(texSize.Height());
    const auto gridHeight1 = static_cast<float>(texSize.Height());
    const auto gridHeight2 = static_cast<float>(texSize.Height()) - errorOffsetPxf;
    const auto gridHeight3 = static_cast<float>(texSize.Height()) - errorOffsetPxf;

    // Correct
    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX0, gridY0, gridWidth0, gridHeight0), color);

    // Various incorrect
    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX1, gridY0, gridWidth1, gridHeight0), color);
    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX2, gridY0, gridWidth2, gridHeight0), color);
    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX3, gridY0, gridWidth3, gridHeight0), color);

    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX0, gridY1, gridWidth0, gridHeight1), color);
    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX1, gridY1, gridWidth1, gridHeight1), color);
    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX2, gridY1, gridWidth2, gridHeight1), color);
    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX3, gridY1, gridWidth3, gridHeight1), color);

    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX0, gridY2, gridWidth0, gridHeight2), color);
    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX1, gridY2, gridWidth1, gridHeight2), color);
    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX2, gridY2, gridWidth2, gridHeight2), color);
    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX3, gridY2, gridWidth3, gridHeight2), color);

    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX0, gridY3, gridWidth0, gridHeight3), color);
    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX1, gridY3, gridWidth1, gridHeight3), color);
    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX2, gridY3, gridWidth2, gridHeight3), color);
    m_nativeBatch->Draw(srcTexture, PxAreaRectangleF(gridX3, gridY3, gridWidth3, gridHeight3), color);

    const auto* pFont = font.get();
    if (pFont != nullptr)
    {
      Vector2 dstPos(float(gridX0Correct), float(offsetPx.Y));

      m_buffer.clear();
      fmt::format_to(m_buffer, "0.00-{:.2f}", gridWidth0);

      m_nativeBatch->DrawString(*pFont, StringViewLite(m_buffer.data(), m_buffer.size()), dstPos, Color::White());
      dstPos.X = float(gridX1Correct);
      m_buffer.clear();
      fmt::format_to(m_buffer, "{:.2f}-{:.2f}", errorOffsetPxf, errorOffsetPxf + gridWidth1);
      m_nativeBatch->DrawString(*pFont, StringViewLite(m_buffer.data(), m_buffer.size()), dstPos, Color::White());
      dstPos.X = float(gridX2Correct);
      m_buffer.clear();
      fmt::format_to(m_buffer, "0.00-{:.2f}", gridWidth2);
      m_nativeBatch->DrawString(*pFont, StringViewLite(m_buffer.data(), m_buffer.size()), dstPos, Color::White());
      dstPos.X = float(gridX3Correct);
      m_buffer.clear();
      fmt::format_to(m_buffer, "{:.2f}-{:.2f}", errorOffsetPxf, errorOffsetPxf + gridWidth3);
      m_nativeBatch->DrawString(*pFont, StringViewLite(m_buffer.data(), m_buffer.size()), dstPos, Color::White());
    }
    return {gridX4Correct, gridY4Correct};
  }

  void Shared::SetDefaultValues()
  {
    m_slider->SetValue(LocalConfig::DefaultOffset);
  }


  void Shared::DecreaseSlider()
  {
    m_slider->SetValue(m_slider->GetValue() - LocalConfig::SliderSpeed);
  }


  void Shared::IncreaseSlider()
  {
    m_slider->SetValue(m_slider->GetValue() + LocalConfig::SliderSpeed);
  }

}
