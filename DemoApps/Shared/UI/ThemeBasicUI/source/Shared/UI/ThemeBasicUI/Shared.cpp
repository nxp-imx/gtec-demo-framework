/****************************************************************************************************************************************************
 * Copyright 2020, 2022, 2024 NXP
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
#include <FslBase/Log/Math/FmtPoint2.hpp>
#include <FslBase/Log/Math/FmtVector2.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Sprite/Font/TextureAtlasSpriteFont.hpp>
#include <FslGraphics/Sprite/ISpriteResourceManager.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslGraphics/TextureAtlas/TestAtlasTextureGenerator.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/App/UIAppConfig.hpp>
#include <FslSimpleUI/App/UIAppTextureResourceCreationInfo.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/CheckBox.hpp>
#include <Shared/UI/ThemeBasicUI/Shared.hpp>
// #include <FslSimpleUI/Base/Control/Experimental/Histogram.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/RadioButton.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Layout/ComplexStackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/UI/ThemeBasicUI/OptionParser.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr StringViewLite RadioGroupName1("Group1");
      constexpr StringViewLite RadioGroupName2A("Group2A");
      constexpr StringViewLite RadioGroupName2B("Group2B");

      constexpr IO::PathView SpriteAtlasName("SpritesUIAtlas/UIAtlas_160dpi.png");
      constexpr IO::PathView TestSpriteAtlasName("TestGraphics/TestGraphicsAtlas_160dpi.png");

      constexpr SpriteMaterialId SpriteAtlasMaterialId(UIAppConfig::MaterialId::CustomSpriteOffset);
      constexpr SpriteMaterialId TestAtlasMaterialId(UIAppConfig::MaterialId::CustomSpriteOffset + 1u);

      constexpr IO::PathView CatImageName("cat/Idle_1");
      constexpr IO::PathView DogImageName("dog/Idle_1");

      constexpr IO::PathView BasicNineSlice0("Scale/RedBlue/RedBlueNineSlice");
      constexpr IO::PathView NineSlice0("Scale/RedBlue/RedBlueNineSliceTrimL4T4R4B4");
    }

    Resources CreateResources(const std::shared_ptr<INativeGraphics>& nativeGraphics, const IContentManager& contentManager, const IO::Path& path,
                              const uint32_t /*imageDP*/)
    {
      IO::Path pathPNG(path + ".png");
      IO::Path pathBTA(path + ".bta");

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
      return resources;
    }
  }


  Shared::Shared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi",
                                                         UITestPatternMode::DisabledAllowSwitching))
    , m_graphics(config.DemoServiceProvider.Get<IGraphicsService>())
    , m_nativeBatch(m_graphics->GetNativeBatch2D())
    , m_displayMetrics(config.WindowMetrics)
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
    m_res160 = CreateResources(nativeGraphics, *contentManager, "SpritesUIAtlas/UIAtlas_160dpi", 160);

    FSLLOG3_INFO("Preparing UI");
    {    // Build a simple UI
      m_uiRecord = CreateUI(config.WindowMetrics.DensityDpi, enableTestPattern, enableUITestPattern);


      // Register the root layout with the window manager
      m_uiExtension->GetWindowManager()->Add(m_uiRecord.MainLayout);
    }

    OnConfigurationChanged(config.WindowMetrics);

    SetUITestPattern(enableUITestPattern);


    UpdateResourceScale();

    FSLLOG3_INFO("Ready");
  }


  Shared::~Shared() = default;


  void Shared::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& /*theEvent*/)
  {
    // if (theEvent->GetSource() == m_uiRecord.BtnSetDefaultValues)
    //{
    //  SetDefaultValues();
    //}
  }

  void Shared::OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& /*theEvent*/)
  {
    // if (theEvent->GetSource() == m_uiRecord.SwitchEmulateDpi)
    //{
    //  auto isEnabled = m_uiRecord.SwitchEmulateDpi->IsChecked();
    //  m_uiRecord.SliderDpi->SetEnabled(isEnabled);
    //  UpdateResourceScale();
    //}
    // else if (theEvent->GetSource() == m_uiRecord.SliderDpi)
    //{
    //  UpdateResourceScale();
    //}
    // else if (theEvent->GetSource() == m_uiRecord.SwitchUITestPattern)
    //{
    //  m_uiExtension->SetTestPattern(m_uiRecord.SwitchUITestPattern->IsChecked());
    //}
    // else if (theEvent->GetSource() == m_uiRecord.CheckForceTexDpi)
    //{
    //  auto isEnabled = m_uiRecord.CheckForceTexDpi->IsChecked();
    //  m_uiRecord.SliderTextureDpi->SetEnabled(isEnabled);
    //}
  }


  void Shared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }
    switch (event.GetKey())
    {
    case VirtualKey::Space:
      {
        SetDefaultValues();
        break;
      }
    case VirtualKey::U:
      SetUITestPattern(!m_uiTestPattern);
      break;
    default:
      break;
    }
  }


  void Shared::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    m_displayMetrics = windowMetrics;
    UpdateResourceScale();

    m_uiRecord.Stats.LabelResPx->SetContent(fmt::format("{}x{}px", windowMetrics.ExtentPx.Width.Value, windowMetrics.ExtentPx.Height.Value));
    m_uiRecord.Stats.LabelResDp->SetContent(fmt::format("{}x{}dp", windowMetrics.SizeDp.X, windowMetrics.SizeDp.Y));
    m_uiRecord.Stats.LabelDpi->SetContent(fmt::format("{}x{}", windowMetrics.ExactDpi.X, windowMetrics.ExactDpi.Y));
    m_uiRecord.Stats.LabelDensityDpi->SetContent(fmt::format("{}", windowMetrics.DensityDpi));
    m_uiRecord.Stats.LabelDensityScale->SetContent(fmt::format("{}", windowMetrics.DensityScaleFactor));
  }

  void Shared::Update(const DemoTime& /*demoTime*/)
  {
  }

  void Shared::Draw()
  {
    // if (!m_uiExtension->IsIdle())
    //{
    //  FSLLOG3_INFO("UI system is not idle");
    //}
    m_uiExtension->Draw();
  }

  void Shared::SetDefaultValues()
  {
  }


  Shared::UIRecord Shared::CreateUI(const uint32_t /*densityDpi*/, const bool /*enableTestPattern*/, const bool /*enableUITestPattern*/)
  {
    auto windowContext = m_uiExtension->GetContext();

    // auto& rSpriteResourceManager = m_uiExtension->GetSpriteResourceManager();

    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
    auto& uiFactory = *uiControlFactory;

    {    // Define a new atlas and the LocalConfig::SpriteAtlasMaterialId material
      UIAppTextureResourceCreationInfo creationInfo(PixelFormat::R8G8B8A8_UNORM, Texture2DFilterHint::Smooth);
      auto hAtlasTexture = m_uiExtension->CreateAtlasTexture(LocalConfig::SpriteAtlasName, creationInfo, true);
      m_uiExtension->AddSpriteMaterial(LocalConfig::SpriteAtlasMaterialId, hAtlasTexture, BlendState::AlphaBlend);
      hAtlasTexture = m_uiExtension->CreateAtlasTexture(LocalConfig::TestSpriteAtlasName, creationInfo, true);
      m_uiExtension->AddSpriteMaterial(LocalConfig::TestAtlasMaterialId, hAtlasTexture, BlendState::Opaque);
    }

    auto uiLeft = CreateUILeftSide(uiFactory, windowContext);
    auto uiRight = CreateUIRightSide(uiFactory, windowContext);
    auto uiMiddle = CreateUIMiddle(uiFactory, windowContext);

    auto leftSidebar = uiFactory.CreateLeftBar(uiLeft.Layout);
    auto rightSidebar = uiFactory.CreateRightBar(uiRight.Layout);

    auto mainLayout = std::make_shared<UI::GridLayout>(windowContext);
    mainLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    mainLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    mainLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    mainLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    mainLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    mainLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));

    mainLayout->AddChild(uiMiddle.Layout, 1, 0);
    mainLayout->AddChild(leftSidebar, 0, 0);
    mainLayout->AddChild(rightSidebar, 2, 0);

    UIRecord record;
    record.Stats = uiMiddle.Stats;
    record.MainLayout = mainLayout;
    return record;
  }

  Shared::UIRecordLeft Shared::CreateUILeftSide(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& windowContext)
  {
    auto stackButtons = std::make_shared<UI::StackLayout>(windowContext);
    {
      stackButtons->SetOrientation(UI::LayoutOrientation::Vertical);

      auto stackButtons0 = std::make_shared<UI::StackLayout>(windowContext);
      {
        stackButtons0->SetOrientation(UI::LayoutOrientation::Horizontal);
        auto btnText = uiFactory.CreateTextButton(UI::Theme::ButtonType::Text, "TextButton");
        auto btnOutlined = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "OutlinedButton");
        auto btnContained = uiFactory.CreateTextButton(UI::Theme::ButtonType::Contained, "ContainedButton");
        btnText->SetAlignmentY(UI::ItemAlignment::Center);
        stackButtons0->AddChild(btnText);
        stackButtons0->AddChild(btnOutlined);
        stackButtons0->AddChild(btnContained);
      }
      auto stackButtons1 = std::make_shared<UI::StackLayout>(windowContext);
      {
        stackButtons1->SetOrientation(UI::LayoutOrientation::Horizontal);
        auto btnText = uiFactory.CreateTextButton(UI::Theme::ButtonType::Text, "TextButton");
        auto btnOutlined = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "OutlinedButton");
        auto btnContained = uiFactory.CreateTextButton(UI::Theme::ButtonType::Contained, "ContainedButton");
        btnText->SetAlignmentY(UI::ItemAlignment::Center);
        btnText->SetEnabled(false);
        btnOutlined->SetEnabled(false);
        btnContained->SetEnabled(false);
        stackButtons1->AddChild(btnText);
        stackButtons1->AddChild(btnOutlined);
        stackButtons1->AddChild(btnContained);
      }

      stackButtons->AddChild(stackButtons0);
      stackButtons->AddChild(stackButtons1);
    }

    auto stackVertSlider = std::make_shared<UI::StackLayout>(windowContext);
    {
      auto sliderHorz0 = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, ConstrainedValue<int32_t>(25, 0, 100));
      auto sliderHorz1 = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, ConstrainedValue<int32_t>(25, 0, 100),
                                                UI::Theme::SliderConfig(UI::LayoutDirection::FarToNear));
      auto sliderHorz2 = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, ConstrainedValue<float>(0.25, -1.0f, 1.0f));
      auto sliderHorz3 = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, ConstrainedValue<float>(0.25, -1.0f, 1.0f),
                                                UI::Theme::SliderConfig(UI::LayoutDirection::FarToNear));

      auto sliderFmtValueHorz0 = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, ConstrainedValue<int32_t>(25, 0, 100));
      auto sliderFmtValueHorz1 = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, ConstrainedValue<int32_t>(25, 0, 100),
                                                                UI::Theme::SliderConfig(UI::LayoutDirection::FarToNear));
      auto sliderFmtValueHorz2 = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, ConstrainedValue<float>(0.25, -1.0f, 1.0f));
      auto sliderFmtValueHorz3 = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, ConstrainedValue<float>(0.25, -1.0f, 1.0f),
                                                                UI::Theme::SliderConfig(UI::LayoutDirection::FarToNear));
      auto sliderFmtValueHorz4 =
        uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, ConstrainedValue<float>(25.0f, 0.0f, 100.0f), "{:.1f}%");
      auto sliderFmtValueHorz5 =
        uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, ConstrainedValue<float>(25.0f, 0.0f, 100.0f), "{:.1f}%");
      sliderFmtValueHorz5->SetEnabled(false);

      stackVertSlider->SetOrientation(UI::LayoutOrientation::Vertical);
      stackVertSlider->SetAlignmentX(UI::ItemAlignment::Stretch);
      stackVertSlider->AddChild(sliderHorz0);
      stackVertSlider->AddChild(sliderHorz1);
      stackVertSlider->AddChild(sliderHorz2);
      stackVertSlider->AddChild(sliderHorz3);
      stackVertSlider->AddChild(sliderFmtValueHorz0);
      stackVertSlider->AddChild(sliderFmtValueHorz1);
      stackVertSlider->AddChild(sliderFmtValueHorz2);
      stackVertSlider->AddChild(sliderFmtValueHorz3);
      stackVertSlider->AddChild(sliderFmtValueHorz4);
      stackVertSlider->AddChild(sliderFmtValueHorz5);
    }


    auto stackCB = std::make_shared<UI::StackLayout>(windowContext);
    {
      stackCB->SetSpacing(DpSize1DF::Create(10.0f));
      stackCB->SetOrientation(UI::LayoutOrientation::Vertical);
      auto stackCB0 = std::make_shared<UI::StackLayout>(windowContext);
      {
        stackCB0->SetOrientation(UI::LayoutOrientation::Horizontal);
        auto switch0 = uiFactory.CreateCheckBox("CheckBox", true);
        auto switch1 = uiFactory.CreateCheckBox("CheckBox", false);
        auto switch2 = uiFactory.CreateCheckBox(true);
        auto switch3 = uiFactory.CreateCheckBox(false);
        stackCB0->AddChild(switch0);
        stackCB0->AddChild(switch1);
        stackCB0->AddChild(switch2);
        stackCB0->AddChild(switch3);
      }
      auto stackCB1 = std::make_shared<UI::StackLayout>(windowContext);
      {
        stackCB1->SetOrientation(UI::LayoutOrientation::Horizontal);
        auto switch0Disabled = uiFactory.CreateCheckBox("CheckBox", true);
        auto switch1Disabled = uiFactory.CreateCheckBox("CheckBox", false);
        auto switch2Disabled = uiFactory.CreateCheckBox(true);
        auto switch3Disabled = uiFactory.CreateCheckBox(false);
        switch0Disabled->SetEnabled(false);
        switch1Disabled->SetEnabled(false);
        switch2Disabled->SetEnabled(false);
        switch3Disabled->SetEnabled(false);
        stackCB1->AddChild(switch0Disabled);
        stackCB1->AddChild(switch1Disabled);
        stackCB1->AddChild(switch2Disabled);
        stackCB1->AddChild(switch3Disabled);
      }
      stackCB->AddChild(stackCB0);
      stackCB->AddChild(stackCB1);
    }

    auto stackRB = std::make_shared<UI::StackLayout>(windowContext);
    {
      stackRB->SetSpacing(DpSize1DF::Create(10.0f));
      stackRB->SetOrientation(UI::LayoutOrientation::Vertical);
      auto stackRB0 = std::make_shared<UI::StackLayout>(windowContext);
      {
        stackRB0->SetOrientation(UI::LayoutOrientation::Horizontal);
        auto radioGroup = uiFactory.CreateRadioGroup(LocalConfig::RadioGroupName1);
        auto switch0 = uiFactory.CreateRadioButton(radioGroup, "RadioButton", true);
        auto switch1 = uiFactory.CreateRadioButton(radioGroup, "RadioButton", false);
        auto switch2 = uiFactory.CreateRadioButton(radioGroup, false);
        auto switch3 = uiFactory.CreateRadioButton(radioGroup, false);
        auto switch4 = uiFactory.CreateRadioButton(radioGroup, false);
        stackRB0->AddChild(switch0);
        stackRB0->AddChild(switch1);
        stackRB0->AddChild(switch2);
        stackRB0->AddChild(switch3);
        stackRB0->AddChild(switch4);
      }
      auto stackRB1 = std::make_shared<UI::StackLayout>(windowContext);
      {
        stackRB1->SetOrientation(UI::LayoutOrientation::Horizontal);
        auto radioGroupA = uiFactory.CreateRadioGroup(LocalConfig::RadioGroupName2A);
        auto radioGroupB = uiFactory.CreateRadioGroup(LocalConfig::RadioGroupName2B);
        auto switch0Disabled = uiFactory.CreateRadioButton(radioGroupA, "RadioButton", true);
        auto switch1Disabled = uiFactory.CreateRadioButton(radioGroupA, "RadioButton", false);
        auto switch2Disabled = uiFactory.CreateRadioButton(radioGroupB, false);
        auto switch3Disabled = uiFactory.CreateRadioButton(radioGroupB, false);
        auto switch4Disabled = uiFactory.CreateRadioButton(radioGroupB, true);
        switch0Disabled->SetEnabled(false);
        switch1Disabled->SetEnabled(false);
        switch2Disabled->SetEnabled(false);
        switch3Disabled->SetEnabled(false);
        switch4Disabled->SetEnabled(false);
        stackRB1->AddChild(switch0Disabled);
        stackRB1->AddChild(switch1Disabled);
        stackRB1->AddChild(switch2Disabled);
        stackRB1->AddChild(switch3Disabled);
        stackRB1->AddChild(switch4Disabled);
      }
      stackRB->AddChild(stackRB0);
      stackRB->AddChild(stackRB1);
    }

    auto stackSwitch = std::make_shared<UI::StackLayout>(windowContext);
    {
      stackSwitch->SetSpacing(DpSize1DF::Create(10.0f));
      stackSwitch->SetOrientation(UI::LayoutOrientation::Vertical);
      auto stackSwitch0 = std::make_shared<UI::StackLayout>(windowContext);
      {
        stackSwitch0->SetOrientation(UI::LayoutOrientation::Horizontal);
        auto switch0 = uiFactory.CreateSwitch("Switch", true);
        auto switch1 = uiFactory.CreateSwitch("Switch", false);
        auto switch2 = uiFactory.CreateSwitch(true);
        auto switch3 = uiFactory.CreateSwitch(false);
        stackSwitch0->AddChild(switch0);
        stackSwitch0->AddChild(switch1);
        stackSwitch0->AddChild(switch2);
        stackSwitch0->AddChild(switch3);
      }
      auto stackSwitch1 = std::make_shared<UI::StackLayout>(windowContext);
      {
        stackSwitch1->SetOrientation(UI::LayoutOrientation::Horizontal);
        auto switch0Disabled = uiFactory.CreateSwitch("Switch", true);
        auto switch1Disabled = uiFactory.CreateSwitch("Switch", false);
        auto switch2Disabled = uiFactory.CreateSwitch(true);
        auto switch3Disabled = uiFactory.CreateSwitch(false);
        switch0Disabled->SetEnabled(false);
        switch1Disabled->SetEnabled(false);
        switch2Disabled->SetEnabled(false);
        switch3Disabled->SetEnabled(false);
        stackSwitch1->AddChild(switch0Disabled);
        stackSwitch1->AddChild(switch1Disabled);
        stackSwitch1->AddChild(switch2Disabled);
        stackSwitch1->AddChild(switch3Disabled);
      }
      stackSwitch->AddChild(stackSwitch0);
      stackSwitch->AddChild(stackSwitch1);
    }


    auto leftLayout = std::make_shared<UI::ComplexStackLayout>(windowContext);
    leftLayout->SetAlignmentX(UI::ItemAlignment::Near);
    leftLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    leftLayout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1));
    leftLayout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1));
    leftLayout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1));
    leftLayout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1));
    leftLayout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Auto));
    leftLayout->AddChild(stackButtons);
    leftLayout->AddChild(stackCB);
    leftLayout->AddChild(stackRB);
    leftLayout->AddChild(stackSwitch);
    leftLayout->AddChild(stackVertSlider);
    return {leftLayout};
  }

  Shared::UIRecordRight Shared::CreateUIRightSide(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& windowContext)
  {
    auto& rSpriteResourceManager = m_uiExtension->GetSpriteResourceManager();

    auto basicSpriteImageCat = rSpriteResourceManager.CreateBasicImageSprite(LocalConfig::SpriteAtlasMaterialId, LocalConfig::CatImageName);
    auto basicSpriteImageDog = rSpriteResourceManager.CreateBasicImageSprite(LocalConfig::SpriteAtlasMaterialId, LocalConfig::DogImageName);
    auto spriteImageCat = rSpriteResourceManager.CreateImageSprite(LocalConfig::SpriteAtlasMaterialId, LocalConfig::CatImageName);
    auto spriteImageDog = rSpriteResourceManager.CreateImageSprite(LocalConfig::SpriteAtlasMaterialId, LocalConfig::DogImageName);

    auto stackLabels = std::make_shared<UI::StackLayout>(windowContext);
    {
      stackLabels->SetOrientation(UI::LayoutOrientation::Vertical);
      auto label = uiFactory.CreateLabel(StringViewLite("Label"));
      auto labelDisabled = uiFactory.CreateLabel(StringViewLite("Label"));
      labelDisabled->SetEnabled(false);
      auto fmtValueLabel0 = uiFactory.CreateFmtValueLabel(static_cast<int32_t>(42));
      auto fmtValueLabel1 = uiFactory.CreateFmtValueLabel(MathHelper::PI);
      auto fmtValueLabel2 = uiFactory.CreateFmtValueLabel(static_cast<int32_t>(42), "Hello {} World");
      auto fmtValueLabel3 = uiFactory.CreateFmtValueLabel(MathHelper::PI, "Hello {:0.4f} world");
      stackLabels->AddChild(label);
      stackLabels->AddChild(labelDisabled);
      stackLabels->AddChild(fmtValueLabel0);
      stackLabels->AddChild(fmtValueLabel1);
      stackLabels->AddChild(fmtValueLabel2);
      stackLabels->AddChild(fmtValueLabel3);
    }


    auto stackHorzSlider = std::make_shared<UI::StackLayout>(windowContext);
    {
      auto sliderVert0 = uiFactory.CreateSlider(UI::LayoutOrientation::Vertical, ConstrainedValue<int32_t>(25, 0, 100));
      auto sliderVert1 = uiFactory.CreateSlider(UI::LayoutOrientation::Vertical, ConstrainedValue<int32_t>(25, 0, 100),
                                                UI::Theme::SliderConfig(UI::LayoutDirection::FarToNear));
      auto sliderVert2 = uiFactory.CreateSlider(UI::LayoutOrientation::Vertical, ConstrainedValue<float>(0.25f, -1.0f, 1.0f));
      auto sliderVert3 = uiFactory.CreateSlider(UI::LayoutOrientation::Vertical, ConstrainedValue<float>(0.25f, -1.0f, 1.0f),
                                                UI::Theme::SliderConfig(UI::LayoutDirection::FarToNear));

      stackHorzSlider->SetOrientation(UI::LayoutOrientation::Horizontal);
      stackHorzSlider->SetAlignmentY(UI::ItemAlignment::Stretch);
      stackHorzSlider->AddChild(sliderVert0);
      stackHorzSlider->AddChild(sliderVert1);
      stackHorzSlider->AddChild(sliderVert2);
      stackHorzSlider->AddChild(sliderVert3);
    }

    auto stackHorzSlider2 = std::make_shared<UI::StackLayout>(windowContext);
    {
      auto sliderFmtValueVert0 = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Vertical, ConstrainedValue<int32_t>(25, 0, 100));
      auto sliderFmtValueVert1 = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Vertical, ConstrainedValue<int32_t>(25, 0, 100),
                                                                UI::Theme::SliderConfig(UI::LayoutDirection::FarToNear));
      auto sliderFmtValueVert2 = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Vertical, ConstrainedValue<float>(0.25f, -1.0f, 1.0f));
      auto sliderFmtValueVert3 = uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Vertical, ConstrainedValue<float>(0.25f, -1.0f, 1.0f));
      auto sliderFmtValueVert4 =
        uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Vertical, ConstrainedValue<float>(25.0f, 0.0f, 100.0f), "{:.1f}%");
      auto sliderFmtValueVert5 =
        uiFactory.CreateSliderFmtValue(UI::LayoutOrientation::Vertical, ConstrainedValue<float>(25.0f, 0.0f, 100.0f), "{:.1f}%");
      sliderFmtValueVert5->SetEnabled(false);

      stackHorzSlider2->SetOrientation(UI::LayoutOrientation::Horizontal);
      stackHorzSlider2->SetAlignmentY(UI::ItemAlignment::Stretch);
      stackHorzSlider2->AddChild(sliderFmtValueVert0);
      stackHorzSlider2->AddChild(sliderFmtValueVert1);
      stackHorzSlider2->AddChild(sliderFmtValueVert2);
      stackHorzSlider2->AddChild(sliderFmtValueVert3);
      stackHorzSlider2->AddChild(sliderFmtValueVert4);
      stackHorzSlider2->AddChild(sliderFmtValueVert5);
    }

    auto stackMisc = std::make_shared<UI::StackLayout>(windowContext);
    {
      stackMisc->SetOrientation(UI::LayoutOrientation::Horizontal);

      FSLLOG3_WARNING("FIX: add histogram again");
      // auto histogram = std::make_shared<UI::Histogram<int32_t>>(windowContext);
      // std::vector<int32_t> data = {10, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 14};
      // histogram->SetBarTexture(uiFactory.GetButtonNineSliceTexture());
      // histogram->SetData(data, 5);
      // histogram->SetWidth(100);
      // histogram->SetHeight(100);

      auto background0 = std::make_shared<UI::Background>(windowContext);
      background0->SetBackground(uiFactory.GetResources().GetDialogNineSliceSprite());
      background0->SetBackgroundColor(uiFactory.GetThemePrimaryDarkColor());

      auto label = uiFactory.CreateLabel(StringViewLite("Hey people!"));
      label->SetAlignmentX(UI::ItemAlignment::Center);
      label->SetAlignmentY(UI::ItemAlignment::Center);
      auto background1 = std::make_shared<UI::Background>(windowContext);
      background1->SetBackground(uiFactory.GetResources().GetDialogNineSliceSprite());
      background1->SetContent(label);
      background1->SetBackgroundColor(uiFactory.GetThemePrimaryDarkColor());

      // stackMisc->AddChild(histogram);
      stackMisc->AddChild(background0);
      stackMisc->AddChild(background1);
    }

    auto stackImages = std::make_shared<UI::StackLayout>(windowContext);
    {
      stackImages->SetOrientation(UI::LayoutOrientation::Horizontal);

      auto image0 = uiFactory.CreateImage(basicSpriteImageCat);
      auto image1 = uiFactory.CreateImage(basicSpriteImageDog);
      auto image2 = uiFactory.CreateImage(spriteImageCat);
      auto image3 = uiFactory.CreateImage(spriteImageDog);

      stackImages->AddChild(image0);
      stackImages->AddChild(image1);
      stackImages->AddChild(image2);
      stackImages->AddChild(image3);
    }

    auto rightLayout = std::make_shared<UI::ComplexStackLayout>(windowContext);
    rightLayout->SetAlignmentX(UI::ItemAlignment::Near);
    rightLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    rightLayout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1));
    rightLayout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1));
    rightLayout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1));
    rightLayout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1));
    rightLayout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1));
    rightLayout->AddChild(stackLabels);
    rightLayout->AddChild(stackMisc);
    rightLayout->AddChild(stackImages);
    rightLayout->AddChild(stackHorzSlider);
    rightLayout->AddChild(stackHorzSlider2);

    return {rightLayout};
  }

  Shared::UIRecordMiddle Shared::CreateUIMiddle(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<UI::WindowContext>& windowContext)
  {
    auto& rSpriteResourceManager = m_uiExtension->GetSpriteResourceManager();

    auto basicNineSlice0 = rSpriteResourceManager.CreateBasicNineSliceSprite(LocalConfig::TestAtlasMaterialId, LocalConfig::BasicNineSlice0);
    auto nineSlice0 = rSpriteResourceManager.CreateNineSliceSprite(LocalConfig::TestAtlasMaterialId, LocalConfig::NineSlice0);

    auto image0A = uiFactory.CreateImage(basicNineSlice0);
    auto image0B = uiFactory.CreateImage(basicNineSlice0);
    auto image0C = uiFactory.CreateImage(basicNineSlice0);
    auto image0D = uiFactory.CreateImage(basicNineSlice0);
    image0A->SetAlignmentX(UI::ItemAlignment::Stretch);
    image0A->SetAlignmentY(UI::ItemAlignment::Stretch);
    image0B->SetAlignmentX(UI::ItemAlignment::Stretch);
    image0B->SetAlignmentY(UI::ItemAlignment::Stretch);
    image0C->SetAlignmentX(UI::ItemAlignment::Stretch);
    image0C->SetAlignmentY(UI::ItemAlignment::Stretch);
    image0D->SetAlignmentX(UI::ItemAlignment::Stretch);
    image0D->SetAlignmentY(UI::ItemAlignment::Stretch);

    auto image1A = uiFactory.CreateImage(nineSlice0);
    auto image1B = uiFactory.CreateImage(nineSlice0);
    auto image1C = uiFactory.CreateImage(nineSlice0);
    auto image1D = uiFactory.CreateImage(nineSlice0);
    image1A->SetAlignmentX(UI::ItemAlignment::Stretch);
    image1A->SetAlignmentY(UI::ItemAlignment::Stretch);
    image1B->SetAlignmentX(UI::ItemAlignment::Stretch);
    image1B->SetAlignmentY(UI::ItemAlignment::Stretch);
    image1C->SetAlignmentX(UI::ItemAlignment::Stretch);
    image1C->SetAlignmentY(UI::ItemAlignment::Stretch);
    image1D->SetAlignmentX(UI::ItemAlignment::Stretch);
    image1D->SetAlignmentY(UI::ItemAlignment::Stretch);

    constexpr float Spacing = 8.0f;

    auto gridLayout0 = std::make_shared<UI::GridLayout>(windowContext);
    gridLayout0->SetAlignmentX(UI::ItemAlignment::Stretch);
    gridLayout0->SetAlignmentY(UI::ItemAlignment::Stretch);
    gridLayout0->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, Spacing));
    gridLayout0->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    gridLayout0->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, Spacing));
    gridLayout0->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    gridLayout0->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, Spacing));
    gridLayout0->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, Spacing));
    gridLayout0->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    gridLayout0->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, Spacing));
    gridLayout0->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
    gridLayout0->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, Spacing));
    gridLayout0->AddChild(image0A, 1, 1);
    gridLayout0->AddChild(image0B, 3, 1);
    gridLayout0->AddChild(image0C, 1, 3);
    gridLayout0->AddChild(image0D, 3, 3);


    auto gridLayout1 = std::make_shared<UI::GridLayout>(windowContext);
    gridLayout1->SetAlignmentX(UI::ItemAlignment::Stretch);
    gridLayout1->SetAlignmentY(UI::ItemAlignment::Stretch);
    gridLayout1->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, Spacing));
    gridLayout1->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    gridLayout1->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    gridLayout1->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, Spacing));
    gridLayout1->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, Spacing));
    gridLayout1->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    gridLayout1->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
    gridLayout1->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Fixed, Spacing));
    gridLayout1->AddChild(image1A, 1, 1);
    gridLayout1->AddChild(image1B, 2, 1);
    gridLayout1->AddChild(image1C, 1, 2);
    gridLayout1->AddChild(image1D, 2, 2);

    auto stats = CreateStatsUI(uiFactory);

    auto layout = std::make_shared<UI::ComplexStackLayout>(windowContext);
    layout->SetOrientation(UI::LayoutOrientation::Vertical);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->SetAlignmentY(UI::ItemAlignment::Stretch);
    layout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1));
    layout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1));
    layout->AddChild(stats.Grid);
    layout->AddChild(gridLayout0);
    layout->AddChild(gridLayout1);

    return {layout, stats};
  }


  Shared::StatsUIRecord Shared::CreateStatsUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    auto windowContext = m_uiExtension->GetContext();

    auto labelResPxCaption = uiFactory.CreateLabel("Resolution: ");
    auto labelResDpCaption = uiFactory.CreateLabel("Resolution: ");
    auto labelDpiCaption = uiFactory.CreateLabel("Actual DPI: ");
    auto labelDensityDpiCaption = uiFactory.CreateLabel("Density DPI: ");
    auto labelDensityScaleCaption = uiFactory.CreateLabel("Density Scale: ");
    labelResPxCaption->SetAlignmentX(UI::ItemAlignment::Near);
    labelResPxCaption->SetAlignmentY(UI::ItemAlignment::Center);
    labelResDpCaption->SetAlignmentX(UI::ItemAlignment::Near);
    labelResDpCaption->SetAlignmentY(UI::ItemAlignment::Center);
    labelDpiCaption->SetAlignmentX(UI::ItemAlignment::Near);
    labelDpiCaption->SetAlignmentY(UI::ItemAlignment::Center);
    labelDensityDpiCaption->SetAlignmentX(UI::ItemAlignment::Near);
    labelDensityDpiCaption->SetAlignmentY(UI::ItemAlignment::Center);
    labelDensityScaleCaption->SetAlignmentX(UI::ItemAlignment::Near);
    labelDensityScaleCaption->SetAlignmentY(UI::ItemAlignment::Center);

    auto labelResPx = uiFactory.CreateLabel("");
    auto labelResDp = uiFactory.CreateLabel("");
    auto labelDpi = uiFactory.CreateLabel("");
    auto labelDensityDpi = uiFactory.CreateLabel("");
    auto labelDensityScale = uiFactory.CreateLabel("");
    labelResPx->SetAlignmentX(UI::ItemAlignment::Far);
    labelResPx->SetAlignmentY(UI::ItemAlignment::Center);
    labelResDp->SetAlignmentX(UI::ItemAlignment::Far);
    labelResDp->SetAlignmentY(UI::ItemAlignment::Center);
    labelDpi->SetAlignmentX(UI::ItemAlignment::Far);
    labelDpi->SetAlignmentY(UI::ItemAlignment::Center);
    labelDensityDpi->SetAlignmentX(UI::ItemAlignment::Far);
    labelDensityDpi->SetAlignmentY(UI::ItemAlignment::Center);
    labelDensityScale->SetAlignmentX(UI::ItemAlignment::Far);
    labelDensityScale->SetAlignmentY(UI::ItemAlignment::Center);

    auto gridLayout = std::make_shared<UI::GridLayout>(windowContext);
    gridLayout->SetAlignmentX(UI::ItemAlignment::Center);
    gridLayout->SetAlignmentY(UI::ItemAlignment::Center);
    gridLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    gridLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    gridLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    gridLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    gridLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    gridLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    gridLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    gridLayout->AddChild(labelResPxCaption, 0, 0);
    gridLayout->AddChild(labelResPx, 1, 0);
    gridLayout->AddChild(labelResDpCaption, 0, 1);
    gridLayout->AddChild(labelResDp, 1, 1);
    gridLayout->AddChild(labelDensityDpiCaption, 0, 2);
    gridLayout->AddChild(labelDensityDpi, 1, 2);
    gridLayout->AddChild(labelDensityScaleCaption, 0, 3);
    gridLayout->AddChild(labelDensityScale, 1, 3);
    gridLayout->AddChild(labelDpiCaption, 0, 4);
    gridLayout->AddChild(labelDpi, 1, 4);

    StatsUIRecord statsUI;
    statsUI.LabelResPx = labelResPx;
    statsUI.LabelResDp = labelResDp;
    statsUI.LabelDpi = labelDpi;
    statsUI.LabelDensityDpi = labelDensityDpi;
    statsUI.LabelDensityScale = labelDensityScale;
    statsUI.Grid = gridLayout;
    return statsUI;
  }

  void Shared::UpdateResourceScale()
  {
    // const uint32_t currentDensity = !m_uiRecord.SwitchEmulateDpi->IsChecked() ? m_displayMetrics.DensityDpi : m_uiRecord.SliderDpi->GetValue();
    const uint32_t currentDensity = m_displayMetrics.DensityDpi;

    const auto density = static_cast<float>(currentDensity);
    m_res160.UpdateDensity(density);
    m_activeDensity = currentDensity;
  }

  void Shared::SetUITestPattern(const bool enabled)
  {
    if (m_uiTestPattern == enabled)
    {
      return;
    }

    m_uiExtension->SetTestPattern(enabled);
    m_uiTestPattern = enabled;
  }
}
