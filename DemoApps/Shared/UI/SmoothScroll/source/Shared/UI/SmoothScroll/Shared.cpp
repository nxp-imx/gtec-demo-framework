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

#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/IO/FmtPathView.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/FmtVector2.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Font/BitmapFont.hpp>
#include <FslGraphics/Sprite/Font/TextureAtlasSpriteFont.hpp>
#include <FslGraphics/TextureAtlas/TestAtlasTextureGenerator.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/ComplexStackLayout.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/UI/SmoothScroll/OptionParser.hpp>
#include <Shared/UI/SmoothScroll/Shared.hpp>
#include <cassert>

namespace Fsl
{
  namespace LocalConfig
  {
    constexpr const float SliderSpeedSlow = 0.01f;
    constexpr const float SliderSpeed = 0.10f;
    constexpr const float SliderSpeedFast = 1.00f;

    constexpr ConstrainedValue<float> ScrollSpeed(2.1f, 0.0f, 50.0f);

    constexpr IO::PathView MainUIAtlas("UIAtlas/UIAtlas_160dpi");
    constexpr IO::PathView FontUIAtlas("CustomAtlas/Font72Atlas");

    constexpr StringViewLite TextLine0("The quick brown fox jumps over the lazy dog!");
    constexpr StringViewLite TextLine1("abcdefghijklmnopqrstuvwxyz");
    constexpr StringViewLite TextLine2("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    constexpr StringViewLite TextLine3("0123456789!\".:,;(){}");

    constexpr const bool DrawText = true;
    constexpr const bool DrawImage = true;
    constexpr const bool TestPattern = false;
  }

  namespace
  {
    std::shared_ptr<AtlasFont> CreateFont(const IContentManager& contentManager, const SpriteNativeAreaCalc& spriteNativeAreaCalc,
                                          const uint32_t densityDpi, const IO::Path& path, const Texture2D& atlasTexture)
    {
      return std::make_shared<AtlasFont>(
        atlasTexture, TextureAtlasSpriteFont(spriteNativeAreaCalc, atlasTexture.GetExtent(), contentManager.ReadBitmapFont(path), densityDpi));
    }


    Resources CreateResources(const std::shared_ptr<INativeGraphics>& nativeGraphics, const IContentManager& contentManager,
                              const SpriteNativeAreaCalc& spriteNativeAreaCalc, const IO::Path& path, const uint32_t imageDpi,
                              const uint32_t densityDpi)
    {
      IO::Path pathPNG(path + ".png");
      IO::Path pathBTA(path + ".bta");
      IO::Path pathFBK(path + "_Font.nbf");

      Resources resources;

      // Prepare the texture atlas mappings
      BasicTextureAtlas textureAtlas;
      FSLLOG3_INFO("- Loading '{}'", pathBTA);
      contentManager.Read(textureAtlas, pathBTA);
      resources.AtlasMap = TextureAtlasMap(textureAtlas);

      // Prepare the texture
      FSLLOG3_INFO("- Loading '{}'", pathPNG);
      auto atlasBitmap = contentManager.ReadBitmap(pathPNG, PixelFormat::R8G8B8A8_UNORM);
      resources.AtlasTexture.Reset(nativeGraphics, atlasBitmap, Texture2DFilterHint::Smooth);

      // Prepare a test pattern texture (to make it easy to switch)
      TestAtlasTextureGenerator::PatchWithTestPattern(atlasBitmap, textureAtlas);
      resources.AtlasTestTexture.Reset(nativeGraphics, atlasBitmap, Texture2DFilterHint::Smooth);

      // Prepare the bitmap font
      FSLLOG3_INFO("- Loading '{}'", pathFBK);
      resources.Font = CreateFont(contentManager, spriteNativeAreaCalc, densityDpi, pathFBK, resources.AtlasTexture);
      resources.Density = imageDpi;
      resources.FontConfig = BitmapFontConfig(resources.ResolutionDensityScale);
      return resources;
    }


    PxClipRectangle ExtractWindowRectangle(const std::shared_ptr<UI::BaseWindow>& mainLayout, const std::shared_ptr<UI::BaseWindow>& window)
    {
      auto offsetTopLeftPx = mainLayout->PointFrom(window.get(), PxPoint2());
      return {offsetTopLeftPx, window->RenderSizePx()};
    }
  }

  Shared::Shared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), LocalConfig::MainUIAtlas))
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_nativeBatch(m_graphics->GetNativeBatch2D())
  {
    auto contentManager = config.DemoServiceProvider.Get<IContentManager>();

    auto nativeGraphics = config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeGraphics();
    m_res480 = CreateResources(nativeGraphics, *contentManager, m_uiExtension->GetSpriteNativeAreaCalc(), LocalConfig::FontUIAtlas, 480, 160);

    m_texCircle = AtlasTexture2D(m_res480.AtlasTexture, m_res480.AtlasMap.GetAtlasTextureInfo("Control/White/FloatingSmallRoundButtonN"));
    m_texCircleTest = AtlasTexture2D(m_res480.AtlasTestTexture, m_res480.AtlasMap.GetAtlasTextureInfo("Control/White/FloatingSmallRoundButtonN"));

    m_ui = CreateUI();

    auto optionParser = config.GetOptions<OptionParser>();
    m_ui.TestPatternCheckBox->SetIsChecked(optionParser->GetEnableTestPattern());
    m_ui.Slider->SetValue(optionParser->GetSpeed());
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
    case VirtualKey::T:
      m_ui.TestPatternCheckBox->Toggle();
      break;
    case VirtualKey::I:
      m_ui.DrawImageCheckBox->Toggle();
      break;
    case VirtualKey::D:
      m_ui.DrawTextCheckBox->Toggle();
      break;
    case VirtualKey::UpArrow:
      IncreaseSlider(LocalConfig::SliderSpeedFast);
      break;
    case VirtualKey::DownArrow:
      DecreaseSlider(LocalConfig::SliderSpeedFast);
      break;
    case VirtualKey::LeftArrow:
      DecreaseSlider(LocalConfig::SliderSpeed);
      break;
    case VirtualKey::RightArrow:
      IncreaseSlider(LocalConfig::SliderSpeed);
      break;
    case VirtualKey::Q:
      DecreaseSlider(LocalConfig::SliderSpeedSlow);
      break;
    case VirtualKey::E:
      IncreaseSlider(LocalConfig::SliderSpeedSlow);
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
    if (theEvent->GetSource() == m_ui.BtnDefault)
    {
      SetDefaultValues();
    }
  }

  void Shared::OnConfigurationChanged(const DemoWindowMetrics& /*windowMetrics*/)
  {
  }

  void Shared::FixedUpdate(const DemoTime& /*demoTime*/)
  {
    m_position0 += m_ui.Slider->GetValue() * static_cast<float>(m_direction0);
    m_position1 += m_ui.Slider->GetValue() * static_cast<float>(m_direction1);
  }


  void Shared::Draw()
  {
    const bool drawCircle = m_ui.DrawImageCheckBox->IsChecked();
    const bool drawText = m_ui.DrawTextCheckBox->IsChecked();

    auto rectTopLeftPx = ExtractWindowRectangle(m_ui.MainLayout, m_ui.DummyTopLeft);
    auto rectTopRightPx = ExtractWindowRectangle(m_ui.MainLayout, m_ui.DummyTopRight);
    auto rectMiddleTopPx = ExtractWindowRectangle(m_ui.MainLayout, m_ui.DummyMiddleTop);
    auto rectMiddleBottomPx = ExtractWindowRectangle(m_ui.MainLayout, m_ui.DummyMiddleBottom);
    const bool useTestAtlas = m_ui.TestPatternCheckBox->IsChecked();
    const auto& fontAtlasTexture = !useTestAtlas ? m_res480.Font->GetAtlasTexture() : m_res480.AtlasTestTexture;
    const auto& bitmapFont = m_res480.Font->GetTextureAtlasSpriteFont();
    const auto& fontConfig = m_res480.FontConfig;

    const auto& circleTexture = !useTestAtlas ? m_texCircle : m_texCircleTest;

    Vector2 position0(static_cast<float>(rectTopLeftPx.Left()), m_position0 + static_cast<float>(rectTopLeftPx.Top()));
    Vector2 position1(static_cast<float>(rectTopRightPx.Left()), std::round(m_position0) + static_cast<float>(rectTopRightPx.Top()));
    Vector2 position2(m_position1 + static_cast<float>(rectMiddleTopPx.Left()), static_cast<float>(rectMiddleTopPx.Top()));
    Vector2 position3(std::round(m_position1) + static_cast<float>(rectMiddleBottomPx.Left()), static_cast<float>(rectMiddleBottomPx.Top()));

    const int32_t linesHeightPx = bitmapFont.LineSpacingPx() * 4;
    const int32_t linewidth = bitmapFont.MeasureString(LocalConfig::TextLine0).Width();

    if (m_position0 < static_cast<float>(rectTopLeftPx.Top()))
    {
      m_position0 = static_cast<float>(rectTopLeftPx.Top());
      m_direction0 = 1;
    }
    if (m_position0 > static_cast<float>(rectTopLeftPx.Bottom() - linesHeightPx))
    {
      m_position0 = static_cast<float>(rectTopLeftPx.Bottom() - linesHeightPx);
      m_direction0 = -1;
    }

    if (m_position1 < static_cast<float>(rectMiddleBottomPx.Left()))
    {
      m_position1 = static_cast<float>(rectMiddleBottomPx.Left());
      m_direction1 = 1;
    }
    if (m_position1 > static_cast<float>(rectMiddleBottomPx.Right() - linewidth))
    {
      m_position1 = static_cast<float>(rectMiddleBottomPx.Right() - linewidth);
      m_direction1 = -1;
    }

    m_nativeBatch->Begin();

    {
      INativeBatch2D& rNativeBatch = *m_nativeBatch;

      if (drawCircle)
      {
        PxRectangleU32 srcCircleRect(0, 0, m_texCircle.GetExtent().Width, m_texCircle.GetExtent().Height);
        const uint8_t color = (drawText ? 0x20 : 0xFF);
        Color circleColor(color, color, color, color);
        rNativeBatch.Draw(circleTexture, position0, srcCircleRect, circleColor, rectTopLeftPx);
        rNativeBatch.Draw(circleTexture, position1, srcCircleRect, circleColor, rectTopRightPx);
        rNativeBatch.Draw(circleTexture, position2, srcCircleRect, circleColor, rectMiddleTopPx);
        rNativeBatch.Draw(circleTexture, position3, srcCircleRect, circleColor, rectMiddleBottomPx);
      }

      if (drawText)
      {
        constexpr auto textColor = Color::White();
        DrawText(rNativeBatch, fontAtlasTexture, bitmapFont, fontConfig, position0, textColor, rectTopLeftPx);
        DrawText(rNativeBatch, fontAtlasTexture, bitmapFont, fontConfig, position1, textColor, rectTopRightPx);
        DrawText(rNativeBatch, fontAtlasTexture, bitmapFont, fontConfig, position2, textColor, rectMiddleTopPx);
        DrawText(rNativeBatch, fontAtlasTexture, bitmapFont, fontConfig, position3, textColor, rectMiddleBottomPx);
      }
    }

    m_nativeBatch->End();

    m_uiExtension->Draw();
  }


  void Shared::SetDefaultValues()
  {
    m_ui.Slider->SetValue(LocalConfig::ScrollSpeed.Get());
    m_ui.DrawTextCheckBox->SetIsChecked(LocalConfig::DrawText);
    m_ui.DrawImageCheckBox->SetIsChecked(LocalConfig::DrawImage);
    m_ui.TestPatternCheckBox->SetIsChecked(LocalConfig::TestPattern);
  }


  void Shared::DecreaseSlider(const float speed)
  {
    m_ui.Slider->SetValue(m_ui.Slider->GetValue() - speed);
  }


  void Shared::IncreaseSlider(const float speed)
  {
    m_ui.Slider->SetValue(m_ui.Slider->GetValue() + speed);
  }


  void Shared::DrawText(INativeBatch2D& rNativeBatch, const BaseTexture2D& srcTexture, const TextureAtlasSpriteFont& font,
                        const BitmapFontConfig& fontConfig, const Vector2& positionPxf, const Color& fontColor, const PxClipRectangle& clipRectPx)
  {
    Vector2 pos0Pxf(positionPxf.X, positionPxf.Y);
    Vector2 pos1Pxf(positionPxf.X, positionPxf.Y + static_cast<float>(font.LineSpacingPx()));
    Vector2 pos2Pxf(positionPxf.X, positionPxf.Y + static_cast<float>(font.LineSpacingPx() * 2));
    Vector2 pos3Pxf(positionPxf.X, positionPxf.Y + static_cast<float>(font.LineSpacingPx() * 3));

    rNativeBatch.DrawString(srcTexture, font, fontConfig, LocalConfig::TextLine0, pos0Pxf, fontColor, clipRectPx);
    rNativeBatch.DrawString(srcTexture, font, fontConfig, LocalConfig::TextLine1, pos1Pxf, fontColor, clipRectPx);
    rNativeBatch.DrawString(srcTexture, font, fontConfig, LocalConfig::TextLine2, pos2Pxf, fontColor, clipRectPx);
    rNativeBatch.DrawString(srcTexture, font, fontConfig, LocalConfig::TextLine3, pos3Pxf, fontColor, clipRectPx);
  }


  Shared::SimpleUI Shared::CreateUI()
  {
    auto windowContext = m_uiExtension->GetContext();
    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
    UI::Theme::IThemeControlFactory& uiFactory = *uiControlFactory;

    auto labelSlider = uiFactory.CreateLabel("Scroll speed:");
    labelSlider->SetAlignmentY(UI::ItemAlignment::Center);

    auto slider = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, LocalConfig::ScrollSpeed);
    slider->SetAlignmentX(UI::ItemAlignment::Stretch);
    slider->SetAlignmentY(UI::ItemAlignment::Center);

    auto btnDefault = uiFactory.CreateTextButton(UI::Theme::ButtonType::Contained, "Set defaults");

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
    bottomGrid->AddChild(slider, 2, 0);
    bottomGrid->AddChild(btnDefault, 3, 0);

    auto drawTextCheckbox = uiFactory.CreateSwitch("Draw text", LocalConfig::DrawText);
    drawTextCheckbox->SetAlignmentX(UI::ItemAlignment::Center);

    auto drawImageCheckbox = uiFactory.CreateSwitch("Draw image", LocalConfig::DrawImage);
    drawImageCheckbox->SetAlignmentX(UI::ItemAlignment::Center);

    auto testPatternCheckBox = uiFactory.CreateSwitch("TestPattern", LocalConfig::TestPattern);
    testPatternCheckBox->SetAlignmentX(UI::ItemAlignment::Center);

    auto checkboxStack = std::make_shared<UI::StackLayout>(windowContext);
    checkboxStack->SetAlignmentX(UI::ItemAlignment::Center);
    checkboxStack->SetOrientation(UI::LayoutOrientation::Horizontal);
    checkboxStack->AddChild(drawTextCheckbox);
    checkboxStack->AddChild(drawImageCheckbox);
    checkboxStack->AddChild(testPatternCheckBox);

    auto bottomStack = std::make_shared<UI::StackLayout>(windowContext);
    bottomStack->SetAlignmentX(UI::ItemAlignment::Stretch);
    bottomStack->SetAlignmentY(UI::ItemAlignment::Stretch);
    bottomStack->SetOrientation(UI::LayoutOrientation::Vertical);
    bottomStack->AddChild(checkboxStack);
    bottomStack->AddChild(bottomGrid);

    auto background = uiFactory.CreateBottomBar(bottomStack);

    // We use the full fill texture here to get a gradient rendered.
    auto uiDividerSprite = uiFactory.GetResources().GetDividerNineSliceSprite();

    auto dummyTopLeft = std::make_shared<UI::BaseWindow>(windowContext);
    dummyTopLeft->SetAlignmentX(UI::ItemAlignment::Stretch);
    dummyTopLeft->SetAlignmentY(UI::ItemAlignment::Stretch);
    auto dummyTopRight = std::make_shared<UI::BaseWindow>(windowContext);
    dummyTopRight->SetAlignmentX(UI::ItemAlignment::Stretch);
    dummyTopRight->SetAlignmentY(UI::ItemAlignment::Stretch);
    auto dummyMiddleTop = std::make_shared<UI::BaseWindow>(windowContext);
    dummyMiddleTop->SetAlignmentX(UI::ItemAlignment::Stretch);
    dummyMiddleTop->SetAlignmentY(UI::ItemAlignment::Stretch);
    auto dummyMiddleBottom = std::make_shared<UI::BaseWindow>(windowContext);
    dummyMiddleBottom->SetAlignmentX(UI::ItemAlignment::Stretch);
    dummyMiddleBottom->SetAlignmentY(UI::ItemAlignment::Stretch);

    auto imageLineTopMiddle = uiFactory.CreateImage(uiDividerSprite);
    imageLineTopMiddle->SetAlignmentX(UI::ItemAlignment::Stretch);
    imageLineTopMiddle->SetAlignmentY(UI::ItemAlignment::Stretch);
    imageLineTopMiddle->SetRotateImageCW(true);

    auto imageLineMiddle0 = uiFactory.CreateImage(uiDividerSprite);
    imageLineMiddle0->SetAlignmentX(UI::ItemAlignment::Stretch);
    imageLineMiddle0->SetAlignmentY(UI::ItemAlignment::Stretch);

    auto imageLineMiddle1 = uiFactory.CreateImage(uiDividerSprite);
    imageLineMiddle1->SetAlignmentX(UI::ItemAlignment::Stretch);
    imageLineMiddle1->SetAlignmentY(UI::ItemAlignment::Stretch);

    auto topLeftLabel = uiFactory.CreateLabel("Sub pixel");
    auto topRightLabel = uiFactory.CreateLabel("Per pixel");
    auto middleTopLabel = uiFactory.CreateLabel("Sub pixel");
    auto middleBottomLabel = uiFactory.CreateLabel("Per pixel");
    middleTopLabel->SetAlignmentX(UI::ItemAlignment::Far);
    middleBottomLabel->SetAlignmentX(UI::ItemAlignment::Far);


    // Create the root layout and add it to the window manager
    auto topLayout = std::make_shared<UI::GridLayout>(windowContext);
    topLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    topLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    topLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    topLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    topLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    topLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
    topLayout->AddChild(dummyTopLeft, 0, 0);
    topLayout->AddChild(imageLineTopMiddle, 1, 0);
    topLayout->AddChild(dummyTopRight, 2, 0);
    topLayout->AddChild(topLeftLabel, 0, 0);
    topLayout->AddChild(topRightLabel, 2, 0);

    // Create the root layout and add it to the window manager
    auto mainLayout = std::make_shared<UI::GridLayout>(windowContext);
    mainLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    mainLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    mainLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    mainLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
    mainLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    mainLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 0.25f));
    mainLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    mainLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 0.25f));
    mainLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    mainLayout->AddChild(topLayout, 0, 0);
    mainLayout->AddChild(imageLineMiddle0, 0, 1);
    mainLayout->AddChild(dummyMiddleTop, 0, 2);
    mainLayout->AddChild(imageLineMiddle1, 0, 3);
    mainLayout->AddChild(dummyMiddleBottom, 0, 4);
    mainLayout->AddChild(background, 0, 5);

    mainLayout->AddChild(middleTopLabel, 0, 2);
    mainLayout->AddChild(middleBottomLabel, 0, 4);


    // Register the root layout with the window manager
    m_uiExtension->GetWindowManager()->Add(mainLayout);

    SimpleUI ui;

    ui.DummyTopLeft = std::move(dummyTopLeft);
    ui.DummyTopRight = std::move(dummyTopRight);
    ui.DummyMiddleTop = std::move(dummyMiddleTop);
    ui.DummyMiddleBottom = std::move(dummyMiddleBottom);
    ui.DrawTextCheckBox = std::move(drawTextCheckbox);
    ui.DrawImageCheckBox = std::move(drawImageCheckbox);
    ui.TestPatternCheckBox = std::move(testPatternCheckBox);
    ui.BtnDefault = std::move(btnDefault);
    ui.Slider = std::move(slider);
    ui.MainLayout = std::move(mainLayout);
    return ui;
  }
}
