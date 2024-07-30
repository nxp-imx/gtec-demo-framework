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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslDataBinding/Base/Bind/ConverterBinding.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/KeyEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseMoveEvent.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Colors.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/SpriteNativeAreaCalc.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/App/UISpriteToTextureUtil.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/CheckBox.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/ScrollViewer.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/ComplexStackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/UI/Gestures/GesturesShared.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr IO::PathView MainUIAtlas("UIAtlasBig/UIAtlas_160dpi");

      constexpr PxValue Speed(2);
      constexpr PxSize1D FramePacingSize = PxSize1D::Create(50);

      constexpr ConstrainedValue<float> FlickDeceleration(200, 100, 400);
      constexpr ConstrainedValue<float> FlickAnimationTime(4, 1, 10);

      constexpr ConstrainedValue<float> BounceSpringStiffness(5, 1, 100);
      constexpr ConstrainedValue<float> BounceAnimationTime(1, 0.01f, 2);

      constexpr IO::PathView PathSdfAtlas("Font/SdfAtlas/FontAtlas.bta");
      constexpr IO::PathView PathSdfAtlasTexture("Font/SdfAtlas/FontAtlas.png");
      constexpr IO::PathView PathSdfAtlasFont("Font/SdfAtlas/FontAtlas_Font.fbk");
      constexpr IO::PathView PathFillTexture("Fill/Fill.png");

      constexpr const StringViewLite TextLine0("**Drag to scroll** The quick brown fox jumps over the lazy dog. 1234567890! Hello World.");
      constexpr const StringViewLite TextLine1("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
    }

    namespace IdleColor
    {
      constexpr UI::UIColor Idle = UI::UIColors::Green();
      constexpr UI::UIColor Busy = UI::UIColors::Red();
    }
  }


  GesturesShared::GesturesShared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), LocalConfig::MainUIAtlas))
    , m_profiler(config.DemoServiceProvider.Get<IProfilerService>())
    , m_cachedWindowMetrics(config.WindowMetrics)
    , m_spriteUnitConverter(config.WindowMetrics.DensityDpi)
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_renderSystem(m_graphics->GetBasicRenderSystem())
    , m_batch(m_graphics->GetNativeBatch2D())
    , m_add0Px(LocalConfig::Speed)
    , m_add1Px(LocalConfig::Speed)
  {
    const std::shared_ptr<IContentManager> contentManager = config.DemoServiceProvider.Get<IContentManager>();

    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);

    m_ui = CreateUI(*uiControlFactory);
    m_uiExtension->SetMainWindow(m_ui.MainLayout);

    m_fillSprite = uiControlFactory->GetResources().GetFillSprite();
    m_texFill = UISpriteToTextureUtil::ExtractFillTexture(*m_renderSystem, m_fillSprite);

    SetToDefaults();
  }


  GesturesShared::~GesturesShared() = default;


  void GesturesShared::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    if (windowMetrics != m_cachedWindowMetrics)
    {
      m_cachedWindowMetrics = windowMetrics;
      m_spriteUnitConverter.SetDensityDpi(m_cachedWindowMetrics.DensityDpi);

      assert(m_renderSystem);
      m_texFill = UISpriteToTextureUtil::ExtractFillTexture(*m_renderSystem, m_fillSprite);
    }
  }


  void GesturesShared::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
  }


  void GesturesShared::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
  }


  void GesturesShared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::Space:
      SetToDefaults();
      break;
    case VirtualKey::X:
      m_ui.CheckBoxShowFramePacingX->Toggle();
      break;
    case VirtualKey::Y:
      m_ui.CheckBoxShowFramePacingY->Toggle();
      break;
    default:
      break;
    }
  }


  void GesturesShared::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (theEvent->IsHandled())
    {
      return;
    }

    if (theEvent->GetSource() == m_ui.BtnSetDefaults)
    {
      SetToDefaults();
    }
  }


  void GesturesShared::Update(const DemoTime& demoTime)
  {
    RebuildStatsCache();

    const UI::UIColor idleColor = m_wasUIIdle ? IdleColor::Idle : IdleColor::Busy;
    m_ui.Stats.ImageIdle->SetContentColor(idleColor);
  }


  void GesturesShared::Draw(const DemoTime& demoTime)
  {
    UpdateDuringDrawFramePacingAnimation(m_ui.MoveableRects->WinGetContentRectanglePx());


    m_batch->Begin(BlendState::Opaque);

    Vector2 dstPositionPx;

    if (m_ui.CheckBoxShowFramePacingX->IsChecked())
    {
      m_batch->Draw(m_texFill, PxRectangle(m_position0Px, PxSize2D(LocalConfig::FramePacingSize, LocalConfig::FramePacingSize)), Colors::Cyan());
    }
    if (m_ui.CheckBoxShowFramePacingY->IsChecked())
    {
      m_batch->Draw(m_texFill, PxRectangle(m_position1Px, PxSize2D(LocalConfig::FramePacingSize, LocalConfig::FramePacingSize)), Colors::Cyan());
    }


    m_batch->End();

    m_uiExtension->Draw();
  }


  void GesturesShared::OnFrameSequenceEnd()
  {
    m_wasUIIdle = m_uiExtension->IsIdle();
  }


  void GesturesShared::UpdateDuringDrawFramePacingAnimation(const PxRectangle collisionRectanglePx)
  {
    const auto nearX = collisionRectanglePx.Left();
    const auto nearY = collisionRectanglePx.Top();
    const auto farX = collisionRectanglePx.Right() - LocalConfig::FramePacingSize;
    const auto farY = collisionRectanglePx.Bottom() - LocalConfig::FramePacingSize;

    m_position0Px.X += m_add0Px;
    m_position0Px.Y = farY;
    if (m_position0Px.X > farX)
    {
      m_position0Px.X = farX;
      m_add0Px = PxValue(-LocalConfig::Speed);
    }
    if (m_position0Px.X < nearX)
    {
      m_position0Px.X = nearX;
      m_add0Px = PxValue(LocalConfig::Speed);
    }

    m_position1Px.X = farX;
    m_position1Px.Y += m_add1Px;
    if (m_position1Px.Y > farY)
    {
      m_position1Px.Y = farY;
      m_add1Px = PxValue(-LocalConfig::Speed);
    }
    if (m_position1Px.Y < nearY)
    {
      m_position1Px.Y = nearY;
      m_add1Px = PxValue(LocalConfig::Speed);
    }
  }


  void GesturesShared::RebuildStatsCache()
  {
    m_resolutionBuffer1.clear();
    fmt::format_to(std::back_inserter(m_resolutionBuffer1), "{:.1f}x{:.1f}dp", m_cachedWindowMetrics.SizeDp.X, m_cachedWindowMetrics.SizeDp.Y);
    m_resolutionBuffer2.clear();
    fmt::format_to(std::back_inserter(m_resolutionBuffer2), "{}x{}px", m_cachedWindowMetrics.GetSizePx().RawWidth(),
                   m_cachedWindowMetrics.GetSizePx().RawHeight());
    m_resolutionBuffer3.clear();
    fmt::format_to(std::back_inserter(m_resolutionBuffer3), "DensityDpi: {}", m_cachedWindowMetrics.DensityDpi);

    auto averageFrameTime = m_profiler->GetAverageFrameTime();

    m_fpsBuffer1.clear();
    fmt::format_to(std::back_inserter(m_fpsBuffer1), "FPS: {:.0f}", averageFrameTime.GetFramePerSecond());
    m_fpsBuffer2.clear();
    fmt::format_to(std::back_inserter(m_fpsBuffer2), "Time: {:4.1f}", static_cast<double>(averageFrameTime.TotalTime) / 1000.0);

    m_ui.Stats.LblWinPx->SetContent(StringViewLite(m_resolutionBuffer1.data(), m_resolutionBuffer1.size()));
    m_ui.Stats.LblWinDp->SetContent(StringViewLite(m_resolutionBuffer2.data(), m_resolutionBuffer2.size()));
    m_ui.Stats.LblWinDpi->SetContent(StringViewLite(m_resolutionBuffer3.data(), m_resolutionBuffer3.size()));
    m_ui.Stats.LblFps->SetContent(StringViewLite(m_fpsBuffer1.data(), m_fpsBuffer1.size()));
    m_ui.Stats.LblFpsTime->SetContent(StringViewLite(m_fpsBuffer2.data(), m_fpsBuffer2.size()));
  }


  void GesturesShared::SetToDefaults()
  {
    m_ui.MoveableRects->SetToDefaults();
    m_ui.SliderBounceSpringStiffness->SetValue(LocalConfig::BounceSpringStiffness.Get());
    m_ui.SliderBounceAnimationTime->SetValue(LocalConfig::BounceAnimationTime.Get());
    m_ui.SliderFlickDecelerationDpPerSecond->SetValue(LocalConfig::FlickDeceleration.Get());
    m_ui.SliderFlickAnimationTime->SetValue(LocalConfig::FlickAnimationTime.Get());
    m_ui.CheckBoxShowFramePacingX->SetIsChecked(true);
    m_ui.CheckBoxShowFramePacingY->SetIsChecked(true);
  }


  GesturesShared::UIRecord GesturesShared::CreateUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    auto stats = CreateUIInfo(uiFactory);


    auto lblBounceSpringStiffness = uiFactory.CreateLabel("Stiffness");
    auto lblBounceAnimationTime = uiFactory.CreateLabel("Time");
    lblBounceSpringStiffness->SetAlignmentY(UI::ItemAlignment::Center);
    lblBounceAnimationTime->SetAlignmentY(UI::ItemAlignment::Center);

    auto fmtLblBounceSpringStiffness = uiFactory.CreateFmtValueLabel(1.0f);
    auto fmtLblBounceAnimationTime = uiFactory.CreateFmtValueLabel(1.0f);
    fmtLblBounceSpringStiffness->SetAlignmentX(UI::ItemAlignment::Far);
    fmtLblBounceSpringStiffness->SetAlignmentY(UI::ItemAlignment::Center);
    fmtLblBounceAnimationTime->SetAlignmentX(UI::ItemAlignment::Far);
    fmtLblBounceAnimationTime->SetAlignmentY(UI::ItemAlignment::Center);
    fmtLblBounceSpringStiffness->SetFormatString("{:.2f}");
    fmtLblBounceAnimationTime->SetFormatString("{:.2f}");

    auto sliderBounceSpringStiffness = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalConfig::BounceSpringStiffness);
    auto sliderBounceAnimationTime = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalConfig::BounceAnimationTime);

    auto bounceGridLayout = std::make_shared<UI::GridLayout>(uiFactory.GetContext());
    bounceGridLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    bounceGridLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    bounceGridLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    bounceGridLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    bounceGridLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 60.0f));
    bounceGridLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    bounceGridLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    bounceGridLayout->AddChild(lblBounceSpringStiffness, 0, 0);
    bounceGridLayout->AddChild(lblBounceAnimationTime, 0, 1);
    bounceGridLayout->AddChild(sliderBounceSpringStiffness, 1, 0);
    bounceGridLayout->AddChild(sliderBounceAnimationTime, 1, 1);
    bounceGridLayout->AddChild(fmtLblBounceSpringStiffness, 2, 0);
    bounceGridLayout->AddChild(fmtLblBounceAnimationTime, 2, 1);

    auto lblFlickDecelerationDpPerSecond = uiFactory.CreateLabel("Deceleration");
    auto lblFlickAnimationTime = uiFactory.CreateLabel("Time mod");
    lblFlickDecelerationDpPerSecond->SetAlignmentY(UI::ItemAlignment::Center);
    lblFlickAnimationTime->SetAlignmentY(UI::ItemAlignment::Center);

    auto fmtLblFlickDecelerationDpPerSecond = uiFactory.CreateFmtValueLabel(1.0f);
    auto fmtLblFlickAnimationTime = uiFactory.CreateFmtValueLabel(1.0f);
    fmtLblFlickDecelerationDpPerSecond->SetAlignmentX(UI::ItemAlignment::Far);
    fmtLblFlickDecelerationDpPerSecond->SetAlignmentY(UI::ItemAlignment::Center);
    fmtLblFlickAnimationTime->SetAlignmentX(UI::ItemAlignment::Far);
    fmtLblFlickAnimationTime->SetAlignmentY(UI::ItemAlignment::Center);
    fmtLblFlickDecelerationDpPerSecond->SetFormatString("{:.1f}");
    fmtLblFlickAnimationTime->SetFormatString("{:.1f}");

    auto sliderFlickDecelerationDpPerSecond = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalConfig::FlickDeceleration);
    auto sliderFlickAnimationTime = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalConfig::FlickAnimationTime);

    auto flickGridLayout = std::make_shared<UI::GridLayout>(uiFactory.GetContext());
    flickGridLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    flickGridLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    flickGridLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    flickGridLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    flickGridLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 60.0f));
    flickGridLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    flickGridLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    flickGridLayout->AddChild(lblFlickDecelerationDpPerSecond, 0, 0);
    flickGridLayout->AddChild(lblFlickAnimationTime, 0, 1);
    flickGridLayout->AddChild(sliderFlickDecelerationDpPerSecond, 1, 0);
    flickGridLayout->AddChild(sliderFlickAnimationTime, 1, 1);
    flickGridLayout->AddChild(fmtLblFlickDecelerationDpPerSecond, 2, 0);
    flickGridLayout->AddChild(fmtLblFlickAnimationTime, 2, 1);

    auto checkboxShowFramePacingX = uiFactory.CreateCheckBox("Show X", true);
    auto checkboxShowFramePacingY = uiFactory.CreateCheckBox("Show Y", true);

    auto lblBounceCaption = uiFactory.CreateLabel("Bounce parameters:", UI::Theme::FontType::Header);


    auto lblFlickCaption = uiFactory.CreateLabel("Flick parameters:", UI::Theme::FontType::Header);
    auto lblFramePacing = uiFactory.CreateLabel("Frame pacing animation:", UI::Theme::FontType::Header);

    auto btnSetDefaults = uiFactory.CreateTextButton(UI::Theme::ButtonType::Contained, "Set defaults");
    btnSetDefaults->SetAlignmentX(UI::ItemAlignment::Center);

    auto menuLayout = std::make_shared<UI::ComplexStackLayout>(uiFactory.GetContext());
    menuLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    menuLayout->SetOrientation(UI::LayoutOrientation::Vertical);
    menuLayout->AddChild(lblBounceCaption, UI::LayoutLength(UI::LayoutUnitType::Auto));
    menuLayout->AddChild(bounceGridLayout, UI::LayoutLength(UI::LayoutUnitType::Auto));
    menuLayout->AddChild(uiFactory.CreateDivider(UI::LayoutOrientation::Horizontal), UI::LayoutLength(UI::LayoutUnitType::Auto));
    menuLayout->AddChild(lblFlickCaption, UI::LayoutLength(UI::LayoutUnitType::Auto));
    menuLayout->AddChild(flickGridLayout, UI::LayoutLength(UI::LayoutUnitType::Auto));
    menuLayout->AddChild(uiFactory.CreateDivider(UI::LayoutOrientation::Horizontal), UI::LayoutLength(UI::LayoutUnitType::Auto));
    menuLayout->AddChild(lblFramePacing, UI::LayoutLength(UI::LayoutUnitType::Auto));
    menuLayout->AddChild(checkboxShowFramePacingX, UI::LayoutLength(UI::LayoutUnitType::Auto));
    menuLayout->AddChild(checkboxShowFramePacingY, UI::LayoutLength(UI::LayoutUnitType::Auto));
    menuLayout->AddChild(uiFactory.CreateDivider(UI::LayoutOrientation::Horizontal), UI::LayoutLength(UI::LayoutUnitType::Auto));
    menuLayout->AddChild(btnSetDefaults, UI::LayoutLength(UI::LayoutUnitType::Auto));
    menuLayout->AddChild(stats.MainLayout, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    menuLayout->SetMinWidth(DpSize1DF::Create(400));

    auto leftMenuBar = uiFactory.CreateLeftBar(menuLayout);


    auto bottomStack = std::make_shared<UI::StackLayout>(uiFactory.GetContext());
    bottomStack->SetOrientation(UI::LayoutOrientation::Horizontal);
    bottomStack->SetAlignmentY(UI::ItemAlignment::Center);
    bottomStack->SetSpacing(DpSize1DF::Create(4));
    for (std::size_t repeatIndex = 0; repeatIndex < 5; ++repeatIndex)
    {
      auto lblBottom = uiFactory.CreateLabel(LocalConfig::TextLine0);
      bottomStack->AddChild(lblBottom);
      lblBottom = uiFactory.CreateLabel(LocalConfig::TextLine1);
      bottomStack->AddChild(lblBottom);
    }

    auto bottomScrollViewerX = std::make_shared<UI::ScrollViewer>(uiFactory.GetContext());
    bottomScrollViewerX->SetContent(bottomStack);
    bottomScrollViewerX->SetScrollMode(UI::ScrollModeFlags::TranslateX);
    auto bottomMenuBar = uiFactory.CreateBottomBar(bottomScrollViewerX);

    auto moveableRects = std::make_shared<UI::MoveableRectangles>(uiFactory.GetContext());
    moveableRects->SetFillSprite(uiFactory.GetResources().GetFillSprite());

    auto mainLayout = std::make_shared<UI::ComplexStackLayout>(uiFactory.GetContext());
    mainLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    mainLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    mainLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    mainLayout->AddChild(leftMenuBar, UI::LayoutLength(UI::LayoutUnitType::Auto));
    mainLayout->AddChild(moveableRects, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));

    auto mainLayout2 = std::make_shared<UI::ComplexStackLayout>(uiFactory.GetContext());
    mainLayout2->SetOrientation(UI::LayoutOrientation::Vertical);
    mainLayout2->SetAlignmentX(UI::ItemAlignment::Stretch);
    mainLayout2->SetAlignmentY(UI::ItemAlignment::Stretch);
    mainLayout2->AddChild(mainLayout, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    mainLayout2->AddChild(bottomMenuBar, UI::LayoutLength(UI::LayoutUnitType::Auto));

    // Basic data binding
    {
      auto hSliderProperty = sliderFlickDecelerationDpPerSecond->GetPropertyHandle(UI::Slider<float>::PropertyValue);
      fmtLblFlickDecelerationDpPerSecond->SetBinding(UI::FmtValueLabel<float>::PropertyContent, hSliderProperty);
      moveableRects->SetBinding(UI::MoveableRectangles::PropertyDragFlickDeceleration, hSliderProperty);
      bottomScrollViewerX->SetBinding(UI::ScrollViewer::PropertyDragFlickDeceleration, hSliderProperty);

      hSliderProperty = sliderFlickAnimationTime->GetPropertyHandle(UI::Slider<float>::PropertyValue);
      fmtLblFlickAnimationTime->SetBinding(UI::FmtValueLabel<float>::PropertyContent, hSliderProperty);
      moveableRects->SetBinding(UI::MoveableRectangles::PropertyDragEndAnimTimeMod, hSliderProperty);
      bottomScrollViewerX->SetBinding(UI::ScrollViewer::PropertyDragEndAnimTimeMultiplier, hSliderProperty);


      hSliderProperty = sliderBounceSpringStiffness->GetPropertyHandle(UI::Slider<float>::PropertyValue);
      fmtLblBounceSpringStiffness->SetBinding(UI::FmtValueLabel<float>::PropertyContent, hSliderProperty);
      bottomScrollViewerX->SetBinding(UI::ScrollViewer::PropertyBounceSpringStiffness, hSliderProperty);

      hSliderProperty = sliderBounceAnimationTime->GetPropertyHandle(UI::Slider<float>::PropertyValue);
      fmtLblBounceAnimationTime->SetBinding(UI::FmtValueLabel<float>::PropertyContent, hSliderProperty);
      {
        auto converterBinding =
          std::make_shared<Fsl::DataBinding::ConverterBinding<TimeSpan, float>>([](const float value) { return TimeSpan::FromSeconds(value); });

        bottomScrollViewerX->SetBinding(UI::ScrollViewer::PropertyBounceAnimationTime, DataBinding::Binding(converterBinding, hSliderProperty));
      }
    }
    return {mainLayout2,
            sliderBounceSpringStiffness,
            sliderBounceAnimationTime,
            sliderFlickDecelerationDpPerSecond,
            sliderFlickAnimationTime,
            checkboxShowFramePacingX,
            checkboxShowFramePacingY,
            btnSetDefaults,
            moveableRects,
            stats};
  }


  GesturesShared::UIInfoRecord GesturesShared::CreateUIInfo(UI::Theme::IThemeControlFactory& uiFactory)
  {
    auto labelIdle = uiFactory.CreateLabel("UI idle");
    labelIdle->SetAlignmentY(UI::ItemAlignment::Center);
    auto imageIdle = uiFactory.CreateImage(uiFactory.GetResources().GetColorMarkerNineSliceSprite());
    imageIdle->SetAlignmentX(UI::ItemAlignment::Far);
    imageIdle->SetAlignmentY(UI::ItemAlignment::Center);
    imageIdle->SetContentColor(IdleColor::Busy);
    imageIdle->FinishAnimation();

    auto idleLayout = std::make_shared<UI::StackLayout>(uiFactory.GetContext());
    idleLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    idleLayout->SetSpacing(DpSize1DF::Create(4));
    idleLayout->AddChild(imageIdle);
    idleLayout->AddChild(labelIdle);

    auto lblWinPx = uiFactory.CreateLabel("WinPx");
    auto lblWinDp = uiFactory.CreateLabel("WinDp");
    auto lblWinDpi = uiFactory.CreateLabel("WinDpi");
    auto lblFps = uiFactory.CreateLabel("Fps");
    auto lblFpsTime = uiFactory.CreateLabel("FpsTime");

    auto layout = std::make_shared<UI::StackLayout>(uiFactory.GetContext());
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->SetAlignmentY(UI::ItemAlignment::Far);
    layout->AddChild(idleLayout);
    layout->AddChild(uiFactory.CreateDivider(UI::LayoutOrientation::Horizontal));
    layout->AddChild(lblFps);
    layout->AddChild(lblFpsTime);
    layout->AddChild(uiFactory.CreateDivider(UI::LayoutOrientation::Horizontal));
    layout->AddChild(lblWinPx);
    layout->AddChild(lblWinDp);
    layout->AddChild(lblWinDpi);

    return {layout, lblWinPx, lblWinDp, lblWinDpi, lblFps, lblFpsTime, imageIdle};
  }

}
