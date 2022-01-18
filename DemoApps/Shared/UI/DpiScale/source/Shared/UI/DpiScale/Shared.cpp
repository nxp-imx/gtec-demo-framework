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

#include <Shared/UI/DpiScale/Shared.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/FmtPoint2.hpp>
#include <FslBase/Log/Math/FmtVector2.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/IO/FmtPathView.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Span/SpanUtil.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Font/BitmapFont.hpp>
#include <FslGraphics/Sprite/Font/TextureAtlasSpriteFont.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslGraphics/TextureAtlas/TestAtlasTextureGenerator.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/App/UISpriteToTextureUtil.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/UI/DpiScale/OptionParser.hpp>
#include <cassert>

namespace Fsl
{
  // NOTE:
  // emulate 95dp is good for showing "non-pixel perfect" rendering issues with the current font.
  // emulate 474 is good for showing the difference between the various string rendering techniques
  // emulate 400dp with forced 480dp is good for finding rounding errors on the baseline
  // emulate 423dp with forced 480dp

  namespace LocalConfig
  {
    constexpr Color DividerColor(0x80FFFFFF);

    constexpr uint32_t SliderDpiTickMin = 0u;
    constexpr uint32_t SliderDpiTickMax = 640u;
    constexpr uint32_t SliderDpiTick = 40u;
    constexpr uint32_t SliderDpiTickSlow = 1u;

    constexpr ConstrainedValue<uint32_t> SliderForceTextureDensity(160u, 160u, 640u);
    constexpr uint32_t SliderForceTextureDensityTick = 160u;

    constexpr ConstrainedValue<uint32_t> SliderDownscalePreference(97, 0, 100);
    constexpr uint32_t SliderDownscalePreferenceTick = 1u;

    constexpr bool ShowScaleExample = true;

    constexpr StringViewLite TextLine0("The quick brown fox jumps over the lazy dog!");
    constexpr StringViewLite TextLine1("abcdefghijklmnopqrstuvwxyz");
    constexpr StringViewLite TextLine2("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    constexpr StringViewLite TextLine3("0123456789!\".:,;(){}");

    constexpr StringViewLite TextLine("_|.,\" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!:;(){}");
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

    std::shared_ptr<UI::Label> CreateLabel(const std::shared_ptr<UI::WindowContext>& context, const std::string& text, const UI::ItemAlignment alignX,
                                           const UI::ItemAlignment alignY)
    {
      auto label = std::make_shared<UI::Label>(context);
      label->SetContent(text);
      label->SetAlignmentX(alignX);
      label->SetAlignmentY(alignY);
      return label;
    }
  }


  Shared::Shared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi",
                                                         UITestPatternMode::DisabledAllowSwitching))
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_renderSystem(m_graphics->GetBasicRenderSystem())
    , m_nativeBatch(m_graphics->GetNativeBatch2D())
    , m_displayMetrics(config.WindowMetrics)
    , m_exampleYPosition(m_transitionCache, TransitionTimeSpan(400, TransitionTimeUnit::Milliseconds), TransitionType::Smooth)
  {
    auto optionParser = config.GetOptions<OptionParser>();
    const bool enableTestPattern = optionParser->GetEnableTestPattern();
    const bool enableUITestPattern = optionParser->GetEnableUITestPattern();

    FSLLOG3_INFO("windowMetrics.ExactDpi: {}", config.WindowMetrics.ExactDpi);
    FSLLOG3_INFO("windowMetrics.DensityDpi: {}", config.WindowMetrics.DensityDpi);
    FSLLOG3_INFO("windowMetrics.DensityScaleFactor: {}", config.WindowMetrics.DensityScaleFactor);

    auto nativeGraphics = config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeGraphics();
    auto contentManager = config.DemoServiceProvider.Get<IContentManager>();

    FSLLOG3_INFO("Preparing resources");
    const auto& spriteNativeAreaCalc = m_uiExtension->GetSpriteNativeAreaCalc();
    m_res160 = CreateResources(nativeGraphics, *contentManager, spriteNativeAreaCalc, "UIAtlas/UIAtlas_160dpi", 160, 160);
    m_res320 = CreateResources(nativeGraphics, *contentManager, spriteNativeAreaCalc, "UIAtlas/UIAtlas_320dpi", 320, 160);
    m_res480 = CreateResources(nativeGraphics, *contentManager, spriteNativeAreaCalc, "UIAtlas/UIAtlas_480dpi", 480, 160);
    m_res640 = CreateResources(nativeGraphics, *contentManager, spriteNativeAreaCalc, "UIAtlas/UIAtlas_640dpi", 640, 160);

    FSLLOG3_INFO("Preparing UI");
    {    // Build a simple UI
      auto windowContext = m_uiExtension->GetContext();

      auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
      UI::Theme::IThemeControlFactory& uiFactory = *uiControlFactory;

      // We use the full fill texture here to get a gradient rendered.
      m_fillSprite = uiFactory.GetResources().GetFillSprite();

      auto dividerSprite = uiFactory.GetResources().GetDividerNineSliceSprite();

      m_uiRecord = CreateUI(windowContext, uiFactory, config.WindowMetrics.DensityDpi, enableTestPattern, enableUITestPattern);

      auto background = uiFactory.CreateLeftBar(m_uiRecord.MainLayout);

      auto imageLine = uiFactory.CreateImage(dividerSprite);
      imageLine->SetContent(dividerSprite);
      imageLine->SetAlignmentX(UI::ItemAlignment::Stretch);
      imageLine->SetAlignmentY(UI::ItemAlignment::Stretch);
      imageLine->SetRotateImageCW(true);
      imageLine->SetContentColor(LocalConfig::DividerColor);

      auto imageLineHorizontal = uiFactory.CreateImage(dividerSprite);
      imageLineHorizontal->SetAlignmentX(UI::ItemAlignment::Stretch);
      imageLineHorizontal->SetAlignmentY(UI::ItemAlignment::Stretch);
      imageLineHorizontal->SetContentColor(LocalConfig::DividerColor);

      m_dummyDp = std::make_shared<UI::BaseWindow>(windowContext);
      m_dummyDp->SetAlignmentX(UI::ItemAlignment::Stretch);
      m_dummyDp->SetAlignmentY(UI::ItemAlignment::Stretch);
      m_dummyPx = std::make_shared<UI::BaseWindow>(windowContext);
      m_dummyPx->SetAlignmentX(UI::ItemAlignment::Stretch);
      m_dummyPx->SetAlignmentY(UI::ItemAlignment::Stretch);

      m_dummyText = std::make_shared<UI::BaseWindow>(windowContext);
      m_dummyText->SetAlignmentX(UI::ItemAlignment::Stretch);
      m_dummyText->SetAlignmentY(UI::ItemAlignment::Stretch);

      auto middleGrid = std::make_shared<UI::GridLayout>(windowContext);
      middleGrid->SetAlignmentX(UI::ItemAlignment::Stretch);
      middleGrid->SetAlignmentY(UI::ItemAlignment::Stretch);
      middleGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
      middleGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto, 1.0f));
      middleGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
      middleGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
      middleGrid->AddChild(m_dummyPx, 0, 0);
      middleGrid->AddChild(imageLineHorizontal, 0, 1);
      middleGrid->AddChild(m_dummyText, 0, 2);
      // Due to the 'scroll offscreen effect' we limit this grid to the maximum available space it gets.
      // If we dont do this it might cause other layout elements with stretch to be bigger than the screen
      middleGrid->SetLimitToAvailableSpace(true);

      auto compareGrid = std::make_shared<UI::GridLayout>(windowContext);
      compareGrid->SetAlignmentX(UI::ItemAlignment::Stretch);
      compareGrid->SetAlignmentY(UI::ItemAlignment::Stretch);
      compareGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
      compareGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
      compareGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
      compareGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
      compareGrid->AddChild(middleGrid, 0, 0);
      compareGrid->AddChild(imageLine, 1, 0);
      compareGrid->AddChild(m_dummyDp, 2, 0);

      // Create the root layout and add it to the window manager
      m_mainLayout = std::make_shared<UI::GridLayout>(windowContext);
      m_mainLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
      m_mainLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
      m_mainLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
      m_mainLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
      m_mainLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
      m_mainLayout->AddChild(background, 0, 0);
      m_mainLayout->AddChild(compareGrid, 1, 0);

      // Register the root layout with the window manager
      m_uiExtension->GetWindowManager()->Add(m_mainLayout);
    }

    OnConfigurationChanged(config.WindowMetrics);

    m_uiExtension->SetTestPattern(enableUITestPattern);

    UpdateResourceScale();

    FSLLOG3_INFO("Ready");
  }


  Shared::~Shared() = default;


  void Shared::OnSelect(const UI::RoutedEventArgs& /*args*/, const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_uiRecord.BtnSetDefaultValues)
    {
      SetDefaultValues();
    }
  }

  void Shared::OnContentChanged(const UI::RoutedEventArgs& /*args*/, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_uiRecord.CheckBoxEmulateDpi)
    {
      auto isEnabled = m_uiRecord.CheckBoxEmulateDpi->IsChecked();
      m_uiRecord.SliderDpi->SetEnabled(isEnabled);
      UpdateResourceScale();
    }
    else if (theEvent->GetSource() == m_uiRecord.SliderDpi)
    {
      UpdateResourceScale();
    }
    else if (theEvent->GetSource() == m_uiRecord.CheckBoxUITestPattern)
    {
      m_uiExtension->SetTestPattern(m_uiRecord.CheckBoxUITestPattern->IsChecked());
    }
    else if (theEvent->GetSource() == m_uiRecord.CheckForceTexDpi)
    {
      auto isEnabled = m_uiRecord.CheckForceTexDpi->IsChecked();
      m_uiRecord.SliderTextureDpi->SetEnabled(isEnabled);
    }
  }


  void Shared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }
    switch (event.GetKey())
    {
    case VirtualKey::Backspace:
      //// FIX: test only
      // m_uiRecord.CheckBoxTestPattern->SetEnabled(!m_uiRecord.CheckBoxTestPattern->IsEnabled());
      break;
    case VirtualKey::Space:
    {
      SetDefaultValues();
      break;
    }
    case VirtualKey::P:
      m_uiRecord.CheckBoxPixelPerfect->Toggle();
      break;
    case VirtualKey::T:
      m_uiRecord.CheckBoxTestPattern->Toggle();
      break;
    case VirtualKey::U:
      m_uiRecord.CheckBoxUITestPattern->Toggle();
      break;
    case VirtualKey::D:
      m_uiRecord.CheckBoxEmulateDpi->Toggle();
      break;
    case VirtualKey::Delete:
      if (m_uiRecord.SliderDpi->IsEnabled())
      {
        auto value = m_uiRecord.SliderDpi->GetValue();
        m_uiRecord.SliderDpi->SetValue(value >= LocalConfig::SliderDpiTickSlow ? value - LocalConfig::SliderDpiTickSlow : 0u);
      }
      break;
    case VirtualKey::PageDown:
      if (m_uiRecord.SliderDpi->IsEnabled())
      {
        auto value = m_uiRecord.SliderDpi->GetValue();
        m_uiRecord.SliderDpi->SetValue(value + LocalConfig::SliderDpiTickSlow);
      }
      break;
    case VirtualKey::LeftArrow:
      if (m_uiRecord.SliderDpi->IsEnabled())
      {
        auto value = m_uiRecord.SliderDpi->GetValue();
        m_uiRecord.SliderDpi->SetValue(value >= LocalConfig::SliderDpiTick ? value - LocalConfig::SliderDpiTick : 0u);
      }
      break;
    case VirtualKey::RightArrow:
      if (m_uiRecord.SliderDpi->IsEnabled())
      {
        auto value = m_uiRecord.SliderDpi->GetValue();
        m_uiRecord.SliderDpi->SetValue(value + LocalConfig::SliderDpiTick);
      }
      break;
    case VirtualKey::F:
      m_uiRecord.CheckForceTexDpi->Toggle();
      break;
    case VirtualKey::DownArrow:
      if (m_uiRecord.SliderTextureDpi->IsEnabled())
      {
        auto value = m_uiRecord.SliderTextureDpi->GetValue();
        m_uiRecord.SliderTextureDpi->SetValue(value >= LocalConfig::SliderForceTextureDensityTick ? value - LocalConfig::SliderForceTextureDensityTick
                                                                                                  : 0u);
      }
      break;
    case VirtualKey::UpArrow:
      if (m_uiRecord.SliderTextureDpi->IsEnabled())
      {
        auto value = m_uiRecord.SliderTextureDpi->GetValue();
        m_uiRecord.SliderTextureDpi->SetValue(value + LocalConfig::SliderForceTextureDensityTick);
      }
      break;
    case VirtualKey::Q:
      if (m_uiRecord.SliderDownscalePreference->IsEnabled())
      {
        auto value = m_uiRecord.SliderDownscalePreference->GetValue();
        value = (value >= LocalConfig::SliderDownscalePreferenceTick ? (value - LocalConfig::SliderDownscalePreferenceTick) : 0u);
        m_uiRecord.SliderDownscalePreference->SetValue(value);
      }
      break;
    case VirtualKey::E:
      if (m_uiRecord.SliderDownscalePreference->IsEnabled())
      {
        auto value = m_uiRecord.SliderDownscalePreference->GetValue();
        m_uiRecord.SliderDownscalePreference->SetValue(value + LocalConfig::SliderDownscalePreferenceTick);
      }
      break;
    default:
      break;
    }
  }


  void Shared::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_displayMetrics = windowMetrics;
    UpdateResourceScale();

    m_uiRecord.LabelResPx->SetContent(fmt::format("{}x{}px", windowMetrics.ExtentPx.Width, windowMetrics.ExtentPx.Height));
    m_uiRecord.LabelResDp->SetContent(fmt::format("{}x{}dp", windowMetrics.DpSize.X, windowMetrics.DpSize.Y));
    m_uiRecord.LabelDpi->SetContent(fmt::format("{}x{}", windowMetrics.ExactDpi.X, windowMetrics.ExactDpi.Y));
    m_uiRecord.LabelDensityDpi->SetContent(fmt::format("{}", windowMetrics.DensityDpi));
    m_uiRecord.LabelDensityScale->SetContent(fmt::format("{}", windowMetrics.DensityScaleFactor));

    assert(m_renderSystem);
    m_texFill = UISpriteToTextureUtil::ExtractFillTexture(*m_renderSystem, m_fillSprite);
  }

  void Shared::Update(const DemoTime& demoTime)
  {
    const SpriteUnitConverter& unitConverter = m_uiExtension->GetContext()->UnitConverter;

    // const uint32_t desiredTexDensity = !m_uiRecord.CheckForceTexDpi->IsChecked() ? m_activeDensity : m_uiRecord.SliderTextureDpi->GetValue();
    // const auto& resources = SelectResource(desiredTexDensity, m_uiRecord.SliderDownscalePreference->GetValue() / 100.0f);
    // m_dummyText->SetHeight(CalcTextAreaHeightPx(resources));


    {    // Apply height and possible animation to the dummy control
      auto dummyPxAreaHeight = CalcPxAreaHeightPx(m_res160);

      const bool showScaleExample = m_uiRecord.CheckBoxShowScaleExample->IsChecked();
      m_exampleYPosition.SetValue(showScaleExample ? 0.0f : float(std::max(m_displayMetrics.GetSizePx().Height() - dummyPxAreaHeight, 0)));

      dummyPxAreaHeight += int32_t(std::round(m_exampleYPosition.GetValue()));
      m_dummyPx->SetHeight(UI::DpLayoutSize1D(unitConverter.PxToDpf(dummyPxAreaHeight)));
    }

    {    // update the transition values
      TransitionTimeSpan transitionTimeSpan(demoTime.ElapsedTime.Ticks());
      m_exampleYPosition.Update(transitionTimeSpan);
    }
  }

  void Shared::Draw()
  {
    const bool useTestAtlas = m_uiRecord.CheckBoxTestPattern->IsChecked();

    auto offsetDpWinPx = m_mainLayout->PointFrom(m_dummyDp.get(), PxPoint2());
    auto offsetpxWinPx = m_mainLayout->PointFrom(m_dummyPx.get(), PxPoint2());
    auto offsetTextWinPx = m_mainLayout->PointFrom(m_dummyText.get(), PxPoint2());

    auto rectDpWinPx = PxRectangle2D(offsetDpWinPx, m_dummyDp->RenderExtentPx());
    auto rectPxWinPx = PxRectangle2D(offsetpxWinPx, m_dummyPx->RenderExtentPx());
    auto rectTextWinPx = PxRectangle2D(offsetTextWinPx, m_dummyText->RenderExtentPx());

    const bool pixelPerfect = m_uiRecord.CheckBoxPixelPerfect->IsChecked();
    const uint32_t desiredTexDensity = !m_uiRecord.CheckForceTexDpi->IsChecked() ? m_activeDensity : m_uiRecord.SliderTextureDpi->GetValue();


    m_nativeBatch->Begin();

    const auto& resources = SelectResource(desiredTexDensity, m_uiRecord.SliderDownscalePreference->GetValue() / 100.0f);

    if (pixelPerfect)
    {
      DrawExampleDp(*m_nativeBatch, rectDpWinPx, resources, "DP Rendering", useTestAtlas);
    }
    else
    {
      DrawExampleDpFloat(*m_nativeBatch, rectDpWinPx, resources, "DP Rendering", useTestAtlas);
    }
    // DrawExamplePx(*m_nativeBatch, offsetPxWinPx, endPxWinPx, resources, "PX Rendering", useTestAtlas);
    DrawExamplePx(*m_nativeBatch, rectPxWinPx, m_res160, "PX Rendering", useTestAtlas);


    DrawTextArea(*m_nativeBatch, rectTextWinPx, resources, useTestAtlas);

    m_nativeBatch->End();

    m_uiExtension->Draw();
  }

  void Shared::SetDefaultValues()
  {
    m_uiRecord.CheckBoxPixelPerfect->SetIsChecked(true);
    m_uiRecord.CheckBoxTestPattern->SetIsChecked(false);
    m_uiRecord.CheckBoxUITestPattern->SetIsChecked(false);
    m_uiRecord.CheckBoxEmulateDpi->SetIsChecked(false);
    m_uiRecord.SliderDpi->SetValue(m_displayMetrics.DensityDpi);
    m_uiRecord.CheckForceTexDpi->SetIsChecked(false);
    m_uiRecord.SliderTextureDpi->SetValue(LocalConfig::SliderForceTextureDensity.Get());
    m_uiRecord.SliderDownscalePreference->SetValue(LocalConfig::SliderDownscalePreference.Get());
    m_uiRecord.CheckBoxShowScaleExample->SetIsChecked(LocalConfig::ShowScaleExample);
  }

  void Shared::DrawExamplePx(INativeBatch2D& rNativeBatch, const PxRectangle2D& dstRect, const Resources& resources, const StringViewLite& caption,
                             const bool useTestAtlas)
  {
    const auto dstAreaSizePx = TypeConverter::UncheckedTo<PxPoint2>(dstRect.Extent);

    constexpr const Color fontColor = Color::White();
    auto dstPositionPx = dstRect.Offset;

    const auto& fontAtlasTexture = !useTestAtlas ? resources.Font->GetAtlasTexture() : resources.AtlasTestTexture;
    const auto& bitmapFont = resources.Font->GetTextureAtlasSpriteFont();

    const int32_t lineSpacingPx = bitmapFont.LineSpacingPx();


    {    // Draw the caption
      auto dstPositionCaptionPx = dstPositionPx;
      auto captionSizePx = bitmapFont.MeasureString(caption);

      dstPositionCaptionPx.X += (dstAreaSizePx.X - captionSizePx.Width()) / 2;

      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, caption, TypeConverter::UncheckedTo<Vector2>(dstPositionCaptionPx), fontColor);
    }
    {    // Draw the first line
      dstPositionPx.Y += lineSpacingPx * 2;
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, LocalConfig::TextLine0, TypeConverter::UncheckedTo<Vector2>(dstPositionPx), fontColor);
    }
    {    // Draw the second line
      dstPositionPx.Y += lineSpacingPx;
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, LocalConfig::TextLine1, TypeConverter::UncheckedTo<Vector2>(dstPositionPx), fontColor);
    }
    {    // Draw the third line
      dstPositionPx.Y += lineSpacingPx;
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, LocalConfig::TextLine2, TypeConverter::UncheckedTo<Vector2>(dstPositionPx), fontColor);
    }
    {    // Draw the fourth line
      dstPositionPx.Y += lineSpacingPx;
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, LocalConfig::TextLine3, TypeConverter::UncheckedTo<Vector2>(dstPositionPx), fontColor);
    }
    {    // Draw the first info line
      dstPositionPx.Y += lineSpacingPx * 2;

      m_fmtScratchpad.clear();
      fmt::format_to(std::back_inserter(m_fmtScratchpad), "TexDensity: {}", resources.Density);
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, StringViewLite(m_fmtScratchpad.data(), m_fmtScratchpad.size()),
                              TypeConverter::UncheckedTo<Vector2>(dstPositionPx), fontColor);
    }
    {    // Draw the second info line
      dstPositionPx.Y += lineSpacingPx;
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, "Scale: 1.0", TypeConverter::UncheckedTo<Vector2>(dstPositionPx), fontColor);
    }
  }

  void Shared::DrawExampleDp(INativeBatch2D& rNativeBatch, const PxRectangle2D& dstRect, const Resources& resources, const StringViewLite& caption,
                             const bool useTestAtlas)
  {
    const auto dstAreaSizePx = TypeConverter::UncheckedTo<PxPoint2>(dstRect.Extent);

    constexpr const Color fontColor = Color::White();
    auto dstPositionPx = dstRect.Offset;

    const auto& fontAtlasTexture = !useTestAtlas ? resources.Font->GetAtlasTexture() : resources.AtlasTestTexture;
    const auto& bitmapFont = resources.Font->GetTextureAtlasSpriteFont();

    const auto& fontConfig = resources.FontConfig;
    const auto lineSpacingPx = static_cast<int32_t>(std::round(bitmapFont.LineSpacingPx() * resources.ResolutionDensityScale));


    {    // Draw the caption
      auto dstPositionCaptionPx = dstPositionPx;
      auto captionSizePx = bitmapFont.MeasureString(caption, fontConfig);

      dstPositionCaptionPx.X += (dstAreaSizePx.X - captionSizePx.Width()) / 2;

      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, fontConfig, caption, TypeConverter::UncheckedTo<Vector2>(dstPositionCaptionPx),
                              fontColor);
    }
    {    // Draw the first line
      dstPositionPx.Y += lineSpacingPx * 2;
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, fontConfig, LocalConfig::TextLine0, TypeConverter::UncheckedTo<Vector2>(dstPositionPx),
                              fontColor);
    }
    {    // Draw the second line
      dstPositionPx.Y += lineSpacingPx;
      constexpr StringViewLite text(LocalConfig::TextLine1);
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, fontConfig, text, TypeConverter::UncheckedTo<Vector2>(dstPositionPx), fontColor);
    }
    {    // Draw the third line
      dstPositionPx.Y += lineSpacingPx;
      constexpr StringViewLite text(LocalConfig::TextLine2);
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, fontConfig, text, TypeConverter::UncheckedTo<Vector2>(dstPositionPx), fontColor);
    }
    {    // Draw the fourth line
      dstPositionPx.Y += lineSpacingPx;
      constexpr StringViewLite text(LocalConfig::TextLine3);
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, fontConfig, text, TypeConverter::UncheckedTo<Vector2>(dstPositionPx), fontColor);
    }
    {    // Draw the first info line
      dstPositionPx.Y += lineSpacingPx * 2;

      m_fmtScratchpad.clear();
      fmt::format_to(std::back_inserter(m_fmtScratchpad), "TexDensity: {}", resources.Density);
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, fontConfig, StringViewLite(m_fmtScratchpad.data(), m_fmtScratchpad.size()),
                              TypeConverter::UncheckedTo<Vector2>(dstPositionPx), fontColor);
    }
    {    // Draw the second info line
      dstPositionPx.Y += lineSpacingPx;

      m_fmtScratchpad.clear();
      fmt::format_to(std::back_inserter(m_fmtScratchpad), "Scale: {}", resources.ResolutionDensityScale);
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, fontConfig, StringViewLite(m_fmtScratchpad.data(), m_fmtScratchpad.size()),
                              TypeConverter::UncheckedTo<Vector2>(dstPositionPx), fontColor);
    }
  }

  //! This is basically DrawExampleDp but where all calculations are done in floats
  void Shared::DrawExampleDpFloat(INativeBatch2D& rNativeBatch, const PxRectangle2D& dstRect, const Resources& resources,
                                  const StringViewLite& caption, const bool useTestAtlas)
  {
    const Vector2 dstAreaSizePx = TypeConverter::UncheckedTo<Vector2>(dstRect.Extent);

    constexpr const Color fontColor = Color::White();
    Vector2 dstPositionPx = TypeConverter::UncheckedTo<Vector2>(dstRect.Offset);

    const auto& fontAtlasTexture = !useTestAtlas ? resources.Font->GetAtlasTexture() : resources.AtlasTestTexture;
    const auto& bitmapFont = resources.Font->GetTextureAtlasSpriteFont();

    Vector2 origin;
    Vector2 densityScale(resources.ResolutionDensityScale, resources.ResolutionDensityScale);
    const float lineSpacingPx = bitmapFont.LineSpacingPx() * resources.ResolutionDensityScale;


    {    // Draw the caption
      Vector2 dstPositionCaptionPx = dstPositionPx;
      auto res = bitmapFont.MeasureString(caption);
      Vector2 captionSizePx(res.Width() * resources.ResolutionDensityScale, res.Height() * resources.ResolutionDensityScale);

      dstPositionCaptionPx.X += (dstAreaSizePx.X - captionSizePx.X) / 2.0f;

      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, caption, dstPositionCaptionPx, fontColor, origin, densityScale);
    }
    {    // Draw the first line
      dstPositionPx.Y += lineSpacingPx * 2;
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, LocalConfig::TextLine0, dstPositionPx, fontColor, origin, densityScale);
    }
    {    // Draw the second line
      dstPositionPx.Y += lineSpacingPx;
      constexpr StringViewLite text(LocalConfig::TextLine1);
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, text, dstPositionPx, fontColor, origin, densityScale);
    }
    {    // Draw the third line
      dstPositionPx.Y += lineSpacingPx;
      constexpr StringViewLite text(LocalConfig::TextLine2);
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, text, dstPositionPx, fontColor, origin, densityScale);
    }
    {    // Draw the fourth line
      dstPositionPx.Y += lineSpacingPx;
      constexpr StringViewLite text(LocalConfig::TextLine3);
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, text, dstPositionPx, fontColor, origin, densityScale);
    }
    {    // Draw the first info line
      dstPositionPx.Y += lineSpacingPx * 2;

      m_fmtScratchpad.clear();
      fmt::format_to(std::back_inserter(m_fmtScratchpad), "TexDensity: {}", resources.Density);
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, StringViewLite(m_fmtScratchpad.data(), m_fmtScratchpad.size()), dstPositionPx, fontColor,
                              origin, densityScale);
    }
    {    // Draw the second info line
      dstPositionPx.Y += lineSpacingPx;

      m_fmtScratchpad.clear();
      fmt::format_to(std::back_inserter(m_fmtScratchpad), "Scale: {}", resources.ResolutionDensityScale);
      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, StringViewLite(m_fmtScratchpad.data(), m_fmtScratchpad.size()), dstPositionPx, fontColor,
                              origin, densityScale);
    }
  }


  int32_t Shared::CalcPxAreaHeightPx(const Resources& resources) const
  {
    const auto& bitmapFont = resources.Font->GetTextureAtlasSpriteFont();
    return bitmapFont.LineSpacingPx() * 10;
  }


  int32_t Shared::CalcTextAreaHeightPx(const Resources& resources) const
  {
    const auto& bitmapFont = resources.Font->GetTextureAtlasSpriteFont();
    auto lineSpacingPx = bitmapFont.LineSpacingPx();
    auto scaledLineSpacingPx = bitmapFont.LineSpacingPx(resources.FontConfig);
    return lineSpacingPx + (scaledLineSpacingPx * 7);
  }


  void Shared::DrawTextArea(INativeBatch2D& rNativeBatch, const PxRectangle2D& dstRect, const Resources& resources, const bool useTestAtlas)
  {
    const auto clipRectPxf = TypeConverter::UncheckedTo<PxClipRectangle>(dstRect);
    const auto dstAreaSizePx = TypeConverter::UncheckedTo<PxPoint2>(dstRect.Extent);
    const auto& fontAtlasTexture = !useTestAtlas ? resources.Font->GetAtlasTexture() : resources.AtlasTestTexture;
    const auto& bitmapFont = resources.Font->GetTextureAtlasSpriteFont();
    constexpr StringViewLite text = LocalConfig::TextLine;
    constexpr const Color fontColor = Color::White();
    auto fontConfig = resources.FontConfig;
    fontConfig.Kerning = false;

    // Ensure capacity for our glyphs
    if (m_glyphScratchpad.size() < text.size())
    {
      m_glyphScratchpad.resize(text.size());
    }

    PxPoint2 dstPositionPx = dstRect.Offset;

    const auto scaledLineSpacingPx = bitmapFont.LineSpacingPx(fontConfig);
    const auto scaledBaseLinePx = bitmapFont.BaseLinePx(fontConfig);

    constexpr auto baseLineColor = Color(0xFF404040);
    {
      auto baseLinePx = dstPositionPx + PxPoint2(0, scaledBaseLinePx);
      auto baseLine0Px = baseLinePx;
      auto baseLine1Px = baseLinePx + PxPoint2(dstAreaSizePx.X, 0);
      rNativeBatch.DebugDrawLine(m_texFill, baseLine0Px, baseLine1Px, baseLineColor);

      DrawTextNaiveScaling(rNativeBatch, fontAtlasTexture, bitmapFont, fontConfig, text, dstPositionPx, fontColor, clipRectPxf);
      dstPositionPx.Y += scaledLineSpacingPx;
    }
    {
      auto baseLinePx = dstPositionPx + PxPoint2(0, scaledBaseLinePx);
      auto baseLine0Px = baseLinePx;
      auto baseLine1Px = baseLinePx + PxPoint2(dstAreaSizePx.X, 0);
      rNativeBatch.DebugDrawLine(m_texFill, baseLine0Px, baseLine1Px, baseLineColor);

      DrawTextDstRoundedToFullPixels(rNativeBatch, fontAtlasTexture, bitmapFont, fontConfig, text, dstPositionPx, fontColor, clipRectPxf);
      dstPositionPx.Y += scaledLineSpacingPx;
    }
    {
      auto baseLinePx = dstPositionPx + PxPoint2(0, scaledBaseLinePx);
      auto baseLine0Px = baseLinePx;
      auto baseLine1Px = baseLinePx + PxPoint2(dstAreaSizePx.X, 0);
      rNativeBatch.DebugDrawLine(m_texFill, baseLine0Px, baseLine1Px, baseLineColor);

      DrawTextBaseLineAware(rNativeBatch, fontAtlasTexture, bitmapFont, fontConfig, text, dstPositionPx, fontColor, clipRectPxf);
      dstPositionPx.Y += scaledLineSpacingPx;
    }
    {
      auto baseLinePx = dstPositionPx + PxPoint2(0, scaledBaseLinePx);
      auto baseLine0Px = baseLinePx;
      auto baseLine1Px = baseLinePx + PxPoint2(dstAreaSizePx.X, 0);
      rNativeBatch.DebugDrawLine(m_texFill, baseLine0Px, baseLine1Px, baseLineColor);

      DrawTextAlmostPixelPerfect(rNativeBatch, fontAtlasTexture, bitmapFont, fontConfig, text, dstPositionPx, fontColor, clipRectPxf);
      dstPositionPx.Y += scaledLineSpacingPx;
    }
    {
      auto baseLinePx = dstPositionPx + PxPoint2(0, scaledBaseLinePx);
      auto baseLine0Px = baseLinePx;
      auto baseLine1Px = baseLinePx + PxPoint2(dstAreaSizePx.X, 0);
      rNativeBatch.DebugDrawLine(m_texFill, baseLine0Px, baseLine1Px, baseLineColor);

      DrawTextAlmostPixelPerfect2(rNativeBatch, fontAtlasTexture, bitmapFont, fontConfig, text, dstPositionPx, fontColor, clipRectPxf);
      dstPositionPx.Y += scaledLineSpacingPx;
    }
    {
      auto baseLinePx = dstPositionPx + PxPoint2(0, scaledBaseLinePx);
      auto baseLine0Px = baseLinePx;
      auto baseLine1Px = baseLinePx + PxPoint2(dstAreaSizePx.X, 0);
      rNativeBatch.DebugDrawLine(m_texFill, baseLine0Px, baseLine1Px, baseLineColor);

      DrawTextPixelPerfect(rNativeBatch, fontAtlasTexture, bitmapFont, fontConfig, text, dstPositionPx, fontColor, clipRectPxf);
      dstPositionPx.Y += scaledLineSpacingPx;
    }
    {
      auto baseLinePx = dstPositionPx + PxPoint2(0, scaledBaseLinePx);
      auto baseLine0Px = baseLinePx;
      auto baseLine1Px = baseLinePx + PxPoint2(dstAreaSizePx.X, 0);
      rNativeBatch.DebugDrawLine(m_texFill, baseLine0Px, baseLine1Px, baseLineColor);

      DrawTextRenderRules(rNativeBatch, fontAtlasTexture, bitmapFont, fontConfig, text, dstPositionPx, fontColor, clipRectPxf);
      dstPositionPx.Y += scaledLineSpacingPx;
    }
    {    // finally do a comparison with the NativeBatch rendering (which should be equal to the two above renderings)
      auto baseLinePx = dstPositionPx + PxPoint2(0, scaledBaseLinePx);
      auto baseLine0Px = baseLinePx;
      auto baseLine1Px = baseLinePx + PxPoint2(dstAreaSizePx.X, 0);
      rNativeBatch.DebugDrawLine(m_texFill, baseLine0Px, baseLine1Px, baseLineColor);

      rNativeBatch.DrawString(fontAtlasTexture, bitmapFont, fontConfig, text, TypeConverter::UncheckedTo<Vector2>(dstPositionPx), fontColor,
                              clipRectPxf);
      dstPositionPx.Y += scaledLineSpacingPx;
    }


    {
      auto baseLinePx = dstPositionPx + PxPoint2(0, bitmapFont.BaseLinePx());
      auto baseLine0Px = baseLinePx;
      auto baseLine1Px = baseLinePx + PxPoint2(dstAreaSizePx.X, 0);
      rNativeBatch.DebugDrawLine(m_texFill, baseLine0Px, baseLine1Px, baseLineColor);

      // Draw the font unscaled
      DrawText(rNativeBatch, fontAtlasTexture, bitmapFont, text, dstPositionPx, fontColor, clipRectPxf);
      dstPositionPx.Y += bitmapFont.LineSpacingPx();
    }
  }

  void Shared::DrawText(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont,
                        const StringViewLite& text, const PxPoint2& dstPositionPx, const Color& fontColor, const PxClipRectangle& clipRectPxf)
  {
    // Extract the render rules
    auto glyphScratchpadSpan = SpanUtil::AsSpan(m_glyphScratchpad);
    if (!bitmapFont.ExtractRenderRules(glyphScratchpadSpan, text))
    {
      return;
    }

    auto dstPositionPxf = TypeConverter::UncheckedTo<PxVector2>(dstPositionPx);
    const INativeTexture2D* pNativeTexFont = texFont.TryGetNativePointer();
    if (pNativeTexFont != nullptr)
    {
      for (std::size_t i = 0; i < text.size(); ++i)
      {
        const auto& glyph = glyphScratchpadSpan[i];
        if (glyph.TextureArea.X1 > glyph.TextureArea.X0)
        {
          rNativeBatch.Draw(*pNativeTexFont, glyph.TextureArea, PxAreaRectangleF::AddLocation(dstPositionPxf, glyph.DstRectPxf), fontColor,
                            clipRectPxf);
        }
      }
    }
  }

  // 1
  void Shared::DrawTextNaiveScaling(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont,
                                    const BitmapFontConfig& fontConfig, const StringViewLite& text, const PxPoint2& dstPositionPx,
                                    const Color& fontColor, const PxClipRectangle& clipRectPxf)
  {
    const auto fontScale = std::max(fontConfig.Scale, 0.0f);
    if (fontScale <= 0)
    {
      return;
    }
    // Simple and slow text rendering
    // Pro:
    // - Simple to implement
    // Con:
    // - not pixel perfect so the text is blurry
    // - naive scaling (this means that characters will start rendering at sub pixel locations)
    auto dstPositionPxf = TypeConverter::UncheckedTo<Vector2>(dstPositionPx);
    for (std::size_t i = 0; i < text.size(); ++i)
    {
      const auto& fontChar = bitmapFont.GetBitmapFontChar(text[i]).CharInfo;
      if (fontChar.SrcTextureRectPx.Width > 0u && fontChar.SrcTextureRectPx.Height > 0u)
      {
        const float dstXPxf = dstPositionPxf.X + (float(fontChar.OffsetPx.X) * fontScale);
        const float dstYPxf = dstPositionPxf.Y + (float(fontChar.OffsetPx.Y) * fontScale);
        const float scaledfontCharWidthPxf = float(fontChar.SrcTextureRectPx.Width) * fontScale;
        const float scaledfontCharHeightPxf = float(fontChar.SrcTextureRectPx.Height) * fontScale;

        PxAreaRectangleF fDstRectPx(dstXPxf, dstYPxf, scaledfontCharWidthPxf, scaledfontCharHeightPxf);
        rNativeBatch.Draw(texFont, fDstRectPx, TypeConverter::To<PxRectangleU32>(fontChar.SrcTextureRectPx), fontColor, clipRectPxf);
      }
      dstPositionPxf.X += float(fontChar.XAdvancePx) * fontScale;
    }
  }

  // 2
  void Shared::DrawTextDstRoundedToFullPixels(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont,
                                              const BitmapFontConfig& fontConfig, const StringViewLite& text, const PxPoint2& dstPositionPx,
                                              const Color& fontColor, const PxClipRectangle& clipRectPxf)
  {
    const auto fontScale = std::max(fontConfig.Scale, 0.0f);
    if (fontScale <= 0)
    {
      return;
    }
    // Simple and slow text rendering
    // We ensure that each glyph is starts and ends at a full pixel
    // Pro:
    // - pixel perfect
    // Con:
    // - glyph width and height might be slightly off
    // - Glyphs do not hit the correct position on the baseline.
    // - Line width compared to the naively scaled one is off
    auto dstXPosPx = dstPositionPx.X;
    for (std::size_t i = 0; i < text.size(); ++i)
    {
      const auto& fontCharInfo = bitmapFont.GetBitmapFontChar(text[i]);
      if (fontCharInfo.RenderInfo.TextureArea.X1 > fontCharInfo.RenderInfo.TextureArea.X0)
      {
        const auto scaledXStartPx = int32_t(std::round(float(fontCharInfo.CharInfo.OffsetPx.X) * fontScale));
        const auto scaledYStartPx = int32_t(std::round(float(fontCharInfo.CharInfo.OffsetPx.Y) * fontScale));
        const auto dstXPx = dstXPosPx + scaledXStartPx;
        const auto dstYPx = dstPositionPx.Y + scaledYStartPx;
        const auto scaledGlyphWidthPx = int32_t(std::round(float(fontCharInfo.CharInfo.SrcTextureRectPx.Width) * fontScale));
        const auto scaledGlyphHeightPx = int32_t(std::round(float(fontCharInfo.CharInfo.SrcTextureRectPx.Height) * fontScale));

        PxAreaRectangleF fDstRectPx(float(dstXPx), float(dstYPx), float(scaledGlyphWidthPx), static_cast<float>(scaledGlyphHeightPx));
        rNativeBatch.Draw(texFont, fDstRectPx, TypeConverter::To<PxRectangleU32>(fontCharInfo.CharInfo.SrcTextureRectPx), fontColor, clipRectPxf);
      }
      dstXPosPx += int32_t(std::round(float(fontCharInfo.CharInfo.XAdvancePx) * fontScale));
    }
  }

  // 3
  void Shared::DrawTextBaseLineAware(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont,
                                     const BitmapFontConfig& fontConfig, const StringViewLite& text, const PxPoint2& dstPositionPx,
                                     const Color& fontColor, const PxClipRectangle& clipRectPxf)
  {
    const auto fontScale = std::max(fontConfig.Scale, 0.0f);
    if (fontScale <= 0)
    {
      return;
    }

    const auto baseLinePx = bitmapFont.BaseLinePx();
    const auto scaledBaseLinePx = bitmapFont.BaseLinePx(fontConfig);
    // Simple and slow text rendering
    // We ensure that each glyph is starts and ends at a full pixel
    // - pixel perfect
    // - good scaling
    // - fontWidth & layoutWidth is scaled and rounded to a full pixel (so the font is pixel perfect in x)
    // - for y we use pixel perfect scaled baseline
    // Cons:
    // - glyph width and height might be slightly off
    // - Line width compared to the naively scaled one is off
    // - Simple kerning is inaccurate
    // - very small glyphs can be reduced to a size of zero
    auto dstXPosPx = dstPositionPx.X;
    for (std::size_t i = 0; i < text.size(); ++i)
    {
      const auto& fontChar = bitmapFont.GetBitmapFontChar(text[i]).CharInfo;
      if (fontChar.SrcTextureRectPx.Width > 0u && fontChar.SrcTextureRectPx.Height > 0u)
      {
        // we store the kerning offset in a int32_t to ensure that the "-" operation doesn't underflow (due to unsigned subtraction)
        const int32_t glyphYOffsetPx = fontChar.OffsetPx.Y;
        const auto scaledXStartPx = int32_t(std::round(float(fontChar.OffsetPx.X) * fontScale));
        const auto scaledYStartPx = int32_t(std::round((scaledBaseLinePx + (float(glyphYOffsetPx - baseLinePx) * fontScale))));

        const auto dstXPx = dstXPosPx + scaledXStartPx;
        const auto dstYPx = dstPositionPx.Y + scaledYStartPx;
        const auto scaledGlyphWidthPx = int32_t(std::round(float(fontChar.SrcTextureRectPx.Width) * fontScale));
        const auto scaledGlyphHeightPx = int32_t(std::round(float(fontChar.SrcTextureRectPx.Height) * fontScale));

        PxAreaRectangleF fDstRectPx(float(dstXPx), float(dstYPx), float(scaledGlyphWidthPx), static_cast<float>(scaledGlyphHeightPx));
        rNativeBatch.Draw(texFont, fDstRectPx, TypeConverter::To<PxRectangleU32>(fontChar.SrcTextureRectPx), fontColor, clipRectPxf);
      }
      dstXPosPx += int32_t(std::round(float(fontChar.XAdvancePx) * fontScale));
    }
  }

  // 4
  void Shared::DrawTextAlmostPixelPerfect(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont,
                                          const BitmapFontConfig& fontConfig, const StringViewLite& text, const PxPoint2& dstPositionPx,
                                          const Color& fontColor, const PxClipRectangle& clipRectPxf)
  {
    const auto fontScale = std::max(fontConfig.Scale, 0.0f);
    if (fontScale <= 0)
    {
      return;
    }

    const auto baseLinePx = bitmapFont.BaseLinePx();
    const auto scaledBaseLinePx = bitmapFont.BaseLinePx(fontConfig);
    // Slow text rendering
    // We ensure that each glyph is starts and ends at a full pixel
    // Pro:
    // - pixel perfect
    // - precise scaling
    // - fontWidth & layoutWidth is scaled and rounded to a full pixel (so the font is pixel perfect in x)
    // - for y we are close to a pixel perfect scaled baseline
    // Cons:
    // - on some scalings the glyph alignment to the baseline is off by one pixel
    // - glyph width and height might be slightly off
    // - the simple kerning is slightly off
    // - very small glyphs can be reduced to a size of zero
    auto dstXPosPxf = float(dstPositionPx.X);
    for (std::size_t i = 0; i < text.size(); ++i)
    {
      const auto& fontChar = bitmapFont.GetBitmapFontChar(text[i]).CharInfo;
      if (fontChar.SrcTextureRectPx.Width > 0u && fontChar.SrcTextureRectPx.Height > 0u)
      {
        // calc distance from original baseline to the startY, then scale it
        // then add the distance to the scaled baseline and round it (this ensures we have high accuracy)
        // we store the kerning offset in a int32_t to ensure that the "-" operation doesn't underflow (due to unsigned subtraction)
        const int32_t glyphYOffsetPx = fontChar.OffsetPx.Y;
        const auto scaledYStartPx = int32_t(std::round((scaledBaseLinePx + (float(glyphYOffsetPx - baseLinePx) * fontScale))));

        // We use the sub-pixel precision of dstXPosPxf and apply the 'kerning' before we round which makes the final position much more precise
        const auto dstXPx = int32_t(std::round(dstXPosPxf + (float(fontChar.OffsetPx.X) * fontScale)));
        const auto dstXEndPx = int32_t(std::round(dstXPosPxf + (float(fontChar.OffsetPx.X + fontChar.SrcTextureRectPx.Width) * fontScale)));
        const auto scaledGlyphHeightPx = int32_t(std::round(float(fontChar.SrcTextureRectPx.Height) * fontScale));
        const auto dstYPx = dstPositionPx.Y + scaledYStartPx;
        const auto dstYEndPx = dstYPx + scaledGlyphHeightPx;

        assert(dstXPx <= dstXEndPx);
        assert(dstYPx <= dstYEndPx);
        auto dstRectPx = PxAreaRectangleF::FromLeftTopRightBottom(float(dstXPx), float(dstYPx), float(dstXEndPx), float(dstYEndPx));
        rNativeBatch.Draw(texFont, dstRectPx, TypeConverter::To<PxRectangleU32>(fontChar.SrcTextureRectPx), fontColor, clipRectPxf);
      }
      dstXPosPxf += float(fontChar.XAdvancePx) * fontScale;
    }
  }

  // 5
  void Shared::DrawTextAlmostPixelPerfect2(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont,
                                           const BitmapFontConfig& fontConfig, const StringViewLite& text, const PxPoint2& dstPositionPx,
                                           const Color& fontColor, const PxClipRectangle& clipRectPxf)
  {
    const auto fontScale = std::max(fontConfig.Scale, 0.0f);
    if (fontScale <= 0)
    {
      return;
    }

    const auto baseLinePx = bitmapFont.BaseLinePx();
    const auto scaledBaseLinePx = bitmapFont.BaseLinePx(fontConfig);
    // Slow text rendering
    // We ensure that each glyph is starts and ends at a full pixel
    // Pro:
    // - pixel perfect
    // - precise scaling
    // - fontWidth & layoutWidth is scaled and rounded to a full pixel (so the font is pixel perfect in x)
    // - for y we use pixel perfect scaled baseline
    // Cons:
    // - glyph width and height might be slightly off
    // - the simple kerning is slightly off
    // - very small glyphs can be reduced to a size of zero
    auto dstXPosPxf = float(dstPositionPx.X);
    for (std::size_t i = 0; i < text.size(); ++i)
    {
      const auto& fontChar = bitmapFont.GetBitmapFontChar(text[i]).CharInfo;
      if (fontChar.SrcTextureRectPx.Width > 0u && fontChar.SrcTextureRectPx.Height > 0u)
      {
        // calc distance from original baseline to the startY, then scale it
        // then add the distance to the scaled baseline and round it (this ensures we have high accuracy)
        // we store the kerning offset in a int32_t to ensure that the "-" operation doesn't underflow (due to unsigned subtraction)
        const int32_t glyphYOffsetPx = fontChar.OffsetPx.Y;
        const auto glyphHeight = int32_t(fontChar.SrcTextureRectPx.Height);
        const auto scaledYStartPx = int32_t(std::round((scaledBaseLinePx + (float(glyphYOffsetPx - baseLinePx) * fontScale))));
        const auto scaledYEndPx = int32_t(std::round((scaledBaseLinePx + (float(glyphYOffsetPx + glyphHeight - baseLinePx) * fontScale))));

        // We use the sub-pixel precision of dstXPosPxf and apply the 'kerning' before we round which makes the final position much more precise
        const auto dstXPx = int32_t(std::round(dstXPosPxf + (float(fontChar.OffsetPx.X) * fontScale)));
        auto dstXEndPx = int32_t(std::round(dstXPosPxf + (float(fontChar.OffsetPx.X + fontChar.SrcTextureRectPx.Width) * fontScale)));
        auto dstYPx = dstPositionPx.Y + scaledYStartPx;
        const auto dstYEndPx = dstPositionPx.Y + scaledYEndPx;

        dstXEndPx = std::max(dstXPx, dstXEndPx);
        dstYPx = std::min(dstYPx, dstYEndPx);

        auto fDstRectPx = PxAreaRectangleF::FromLeftTopRightBottom(float(dstXPx), float(dstYPx), float(dstXEndPx), float(dstYEndPx));
        rNativeBatch.Draw(texFont, fDstRectPx, TypeConverter::To<PxRectangleU32>(fontChar.SrcTextureRectPx), fontColor, clipRectPxf);
      }
      dstXPosPxf += float(fontChar.XAdvancePx) * fontScale;
    }
  }

  // 6
  void Shared::DrawTextPixelPerfect(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont,
                                    const BitmapFontConfig& fontConfig, const StringViewLite& text, const PxPoint2& dstPositionPx,
                                    const Color& fontColor, const PxClipRectangle& clipRectPxf)
  {
    const auto fontScale = std::max(fontConfig.Scale, 0.0f);
    if (fontScale <= 0)
    {
      return;
    }

    const auto baseLinePx = bitmapFont.BaseLinePx();
    const auto scaledBaseLinePx = bitmapFont.BaseLinePx(fontConfig);
    // Slow text rendering
    // We ensure that each glyph is starts and ends at a full pixel
    // Pro:
    // - pixel perfect
    // - precise scaling
    // - fontWidth & layoutWidth is scaled and rounded to a full pixel (so the font is pixel perfect in x)
    // - for y we use pixel perfect scaled baseline
    // Cons:
    // - glyph width and height might be slightly off
    // - the simple kerning is slightly off
    auto dstXPosPxf = float(dstPositionPx.X);
    for (std::size_t i = 0; i < text.size(); ++i)
    {
      const auto& fontChar = bitmapFont.GetBitmapFontChar(text[i]).CharInfo;
      if (fontChar.SrcTextureRectPx.Width > 0u && fontChar.SrcTextureRectPx.Height > 0u)
      {
        // calc distance from original baseline to the startY, then scale it
        // then add the distance to the scaled baseline and round it (this ensures we have high accuracy)
        // we store the kerning offset in a int32_t to ensure that the "-" operation doesn't underflow (due to unsigned subtraction)
        const int32_t glyphYOffsetPx = fontChar.OffsetPx.Y;
        const auto glyphHeight = int32_t(fontChar.SrcTextureRectPx.Height);
        const auto scaledYStartPx = int32_t(std::round((scaledBaseLinePx + (float(glyphYOffsetPx - baseLinePx) * fontScale))));
        const auto scaledYEndPx = int32_t(std::round((scaledBaseLinePx + (float(glyphYOffsetPx + glyphHeight - baseLinePx) * fontScale))));

        // We use the sub-pixel precision of dstXPosPxf and apply the 'kerning' before we round which makes the final position much more precise
        const auto dstXPx = int32_t(std::round(dstXPosPxf + (float(fontChar.OffsetPx.X) * fontScale)));
        auto dstXEndPx = int32_t(std::round(dstXPosPxf + (float(fontChar.OffsetPx.X + fontChar.SrcTextureRectPx.Width) * fontScale)));
        auto dstYPx = dstPositionPx.Y + scaledYStartPx;
        const auto dstYEndPx = dstPositionPx.Y + scaledYEndPx;

        dstXEndPx = dstXEndPx > dstXPx ? dstXEndPx : (dstXEndPx + 1);
        dstYPx = dstYPx < dstYEndPx ? dstYPx : (dstYEndPx - 1);

        //{ // Calc some basic stats
        // auto scaledSize = float(glyphHeight) * fontScale;
        // auto finalSize = (dstYEndPx - dstYPx);
        // auto dist = std::abs(finalSize - scaledSize);
        // if (dist > maxErrorDistance)
        //  maxErrorDistance = dist;
        //}

        auto fDstRectPx = PxAreaRectangleF::FromLeftTopRightBottom(float(dstXPx), float(dstYPx), float(dstXEndPx), float(dstYEndPx));
        rNativeBatch.Draw(texFont, fDstRectPx, TypeConverter::To<PxRectangleU32>(fontChar.SrcTextureRectPx), fontColor, clipRectPxf);
      }
      dstXPosPxf += float(fontChar.XAdvancePx) * fontScale;
    }
    // FSLLOG3_INFO("MaxDistance: {}", maxErrorDistance);
  }

  // 7
  void Shared::DrawTextRenderRules(INativeBatch2D& rNativeBatch, const BaseTexture2D& texFont, const TextureAtlasSpriteFont& bitmapFont,
                                   const BitmapFontConfig& fontConfig, const StringViewLite& text, const PxPoint2& dstPositionPx,
                                   const Color& fontColor, const PxClipRectangle& clipRectPxf)
  {
    // The code below ought to do the same as
    // rNativeBatch.DrawString(texFont, bitmapFont, fontConfig, text, TypeConverter::UncheckedTo<Vector2>(dstPositionPx), fontColor, clipRectPxf);

    // Extract the render rules
    auto glyphScratchpadSpan = SpanUtil::AsSpan(m_glyphScratchpad);
    if (!bitmapFont.ExtractRenderRules(glyphScratchpadSpan, text, fontConfig))
    {
      return;
    }

    auto dstPositionPxf = TypeConverter::To<PxVector2>(dstPositionPx);

    // Simple and slow text rendering
    // - the RenderRules now ensure that the glyphs start at full pixel coordinates
    // - not pixel perfect
    // - naive scaling (this means that characters will start rendering at sub pixel locations)
    const INativeTexture2D* pNativeTexFont = texFont.TryGetNativePointer();
    if (pNativeTexFont != nullptr)
    {
      for (std::size_t i = 0; i < text.size(); ++i)
      {
        const auto& glyph = glyphScratchpadSpan[i];
        if (glyph.TextureArea.X1 > glyph.TextureArea.X0)
        {
          rNativeBatch.Draw(*pNativeTexFont, glyph.TextureArea, PxAreaRectangleF::AddLocation(dstPositionPxf, glyph.DstRectPxf), fontColor,
                            clipRectPxf);
        }
      }
    }
  }


  Shared::UIRecord Shared::CreateUI(const std::shared_ptr<UI::WindowContext>& context, UI::Theme::IThemeControlFactory& rUIFactory,
                                    const uint32_t densityDpi, const bool enableTestPattern, const bool enableUITestPattern)
  {
    UIRecord record;


    ConstrainedValue<uint32_t> sliderValue(densityDpi, LocalConfig::SliderDpiTickMin, LocalConfig::SliderDpiTickMax);
    auto sliderDpi = rUIFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, sliderValue, "{}");
    sliderDpi->SetEnabled(false);

    auto sliderTextureDpi = rUIFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, LocalConfig::SliderForceTextureDensity);
    sliderTextureDpi->SetTickFrequency(LocalConfig::SliderForceTextureDensityTick);
    sliderTextureDpi->SetEnabled(false);

    auto sliderDownscalePreference =
      rUIFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, LocalConfig::SliderDownscalePreference, "{}%");
    sliderDownscalePreference->SetTickFrequency(LocalConfig::SliderDownscalePreferenceTick);

    auto labelResPxCaption = CreateLabel(context, "Resolution: ", UI::ItemAlignment::Near, UI::ItemAlignment::Center);
    auto labelResDpCaption = CreateLabel(context, "Resolution: ", UI::ItemAlignment::Near, UI::ItemAlignment::Center);
    auto labelDpiCaption = CreateLabel(context, "Actual DPI: ", UI::ItemAlignment::Near, UI::ItemAlignment::Center);
    auto labelDensityDpiCaption = CreateLabel(context, "Density DPI: ", UI::ItemAlignment::Near, UI::ItemAlignment::Center);
    auto labelDensityScaleCaption = CreateLabel(context, "Density Scale: ", UI::ItemAlignment::Near, UI::ItemAlignment::Center);
    auto labelDownscalePreferenceCaption = CreateLabel(context, "Downscale preference", UI::ItemAlignment::Near, UI::ItemAlignment::Center);

    auto labelResPx = CreateLabel(context, "", UI::ItemAlignment::Far, UI::ItemAlignment::Center);
    auto labelResDp = CreateLabel(context, "", UI::ItemAlignment::Far, UI::ItemAlignment::Center);
    auto labelDpi = CreateLabel(context, "", UI::ItemAlignment::Far, UI::ItemAlignment::Center);
    auto labelDensityDpi = CreateLabel(context, "", UI::ItemAlignment::Far, UI::ItemAlignment::Center);
    auto labelDensityScale = CreateLabel(context, "", UI::ItemAlignment::Far, UI::ItemAlignment::Center);

    auto gridLayout2 = std::make_shared<UI::GridLayout>(context);
    gridLayout2->SetAlignmentX(UI::ItemAlignment::Stretch);
    gridLayout2->SetAlignmentY(UI::ItemAlignment::Center);
    gridLayout2->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    gridLayout2->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    gridLayout2->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    gridLayout2->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    gridLayout2->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    gridLayout2->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    gridLayout2->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    gridLayout2->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    gridLayout2->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    gridLayout2->AddChild(labelResPxCaption, 1, 0);
    gridLayout2->AddChild(labelResPx, 2, 0);
    gridLayout2->AddChild(labelResDpCaption, 1, 1);
    gridLayout2->AddChild(labelResDp, 2, 1);
    gridLayout2->AddChild(labelDensityDpiCaption, 1, 2);
    gridLayout2->AddChild(labelDensityDpi, 2, 2);
    gridLayout2->AddChild(labelDensityScaleCaption, 1, 3);
    gridLayout2->AddChild(labelDensityScale, 2, 3);
    gridLayout2->AddChild(labelDpiCaption, 1, 4);
    gridLayout2->AddChild(labelDpi, 2, 4);

    auto checkBoxPixelPerfect = rUIFactory.CreateSwitch("Pixel perfect", true);
    checkBoxPixelPerfect->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto checkBoxTest = rUIFactory.CreateSwitch("Use test pattern", enableTestPattern);
    checkBoxTest->SetAlignmentX(UI::ItemAlignment::Stretch);
    // checkBoxTest->SetEnabled(false);

    auto checkBoxUITest = rUIFactory.CreateSwitch("Use UI test pattern", enableUITestPattern);
    checkBoxUITest->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto checkBoxEmulateDpi = rUIFactory.CreateSwitch("Emulate dpi");
    checkBoxEmulateDpi->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto checkForceTexDpi = rUIFactory.CreateSwitch("Force texture dpi");
    checkForceTexDpi->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto checkBoxShowScaleExample = rUIFactory.CreateSwitch("Show text rendering", LocalConfig::ShowScaleExample);
    checkBoxShowScaleExample->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto btnSetDefaultValues = rUIFactory.CreateTextButton(UI::Theme::ButtonType::Contained, "Set default values");
    btnSetDefaultValues->SetAlignmentX(UI::ItemAlignment::Center);
    btnSetDefaultValues->SetAlignmentY(UI::ItemAlignment::Far);

    auto complexStack = std::make_shared<UI::ComplexStackLayout>(context);
    complexStack->SetLayoutOrientation(UI::LayoutOrientation::Vertical);
    complexStack->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Auto));
    complexStack->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Auto));
    complexStack->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Auto));
    complexStack->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Auto));
    complexStack->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Auto));
    complexStack->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Auto));
    complexStack->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Auto));
    complexStack->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Auto));
    complexStack->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Auto));
    complexStack->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Auto));
    complexStack->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Auto));
    complexStack->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star));
    complexStack->AddChild(gridLayout2);
    complexStack->AddChild(checkBoxPixelPerfect);
    complexStack->AddChild(checkBoxTest);
    complexStack->AddChild(checkBoxUITest);
    complexStack->AddChild(checkBoxEmulateDpi);
    complexStack->AddChild(sliderDpi);
    complexStack->AddChild(checkForceTexDpi);
    complexStack->AddChild(sliderTextureDpi);
    complexStack->AddChild(labelDownscalePreferenceCaption);
    complexStack->AddChild(sliderDownscalePreference);
    complexStack->AddChild(checkBoxShowScaleExample);
    complexStack->AddChild(btnSetDefaultValues);
    complexStack->SetAlignmentX(UI::ItemAlignment::Stretch);
    complexStack->SetAlignmentY(UI::ItemAlignment::Stretch);


    record.LabelResPx = labelResPx;
    record.LabelResDp = labelResDp;
    record.LabelDpi = labelDpi;
    record.LabelDensityDpi = labelDensityDpi;
    record.LabelDensityScale = labelDensityScale;
    record.CheckBoxPixelPerfect = checkBoxPixelPerfect;
    record.CheckBoxTestPattern = checkBoxTest;
    record.CheckBoxUITestPattern = checkBoxUITest;
    record.CheckBoxEmulateDpi = checkBoxEmulateDpi;
    record.SliderDpi = sliderDpi;
    record.CheckForceTexDpi = checkForceTexDpi;
    record.SliderTextureDpi = sliderTextureDpi;
    record.SliderDownscalePreference = sliderDownscalePreference;
    record.CheckBoxShowScaleExample = checkBoxShowScaleExample;
    record.BtnSetDefaultValues = btnSetDefaultValues;
    record.MainLayout = complexStack;
    return record;
  }

  void Shared::UpdateResourceScale()
  {
    const uint32_t currentDensity = !m_uiRecord.CheckBoxEmulateDpi->IsChecked() ? m_displayMetrics.DensityDpi : m_uiRecord.SliderDpi->GetValue();

    const auto density = float(currentDensity);
    m_res160.UpdateDensity(density);
    m_res320.UpdateDensity(density);
    m_res480.UpdateDensity(density);
    m_res640.UpdateDensity(density);
    m_activeDensity = currentDensity;
  }

  const Resources& Shared::SelectResource(const uint32_t density, const float preferenceWeight)
  {
    const float actualWeight = MathHelper::Clamp(1.0f - preferenceWeight, 0.0f, 1.0f);
    if (density < 160)
    {
      return m_res160;
    }
    if (density < 320)
    {
      return (float(density - 160u) / 160.0f) <= actualWeight ? m_res160 : m_res320;
    }
    if (density < 480)
    {
      return (float(density - 320u) / 160.0f) <= actualWeight ? m_res320 : m_res480;
    }
    if (density < 640)
    {
      return (float(density - 480u) / 160.0f) <= actualWeight ? m_res480 : m_res640;
    }
    return m_res640;
  }
}
