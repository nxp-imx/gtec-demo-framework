/****************************************************************************************************************************************************
 * Copyright 2021-2022, 2024 NXP
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
#include <FslBase/Math/Pixel/PxViewport.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/App/UIDemoAppExtensionCreateInfo.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/CheckBox.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/RadioButton.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/Layout/ComplexStackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/UniformStackLayout.hpp>
#include <FslSimpleUI/Base/Layout/UniformWrapLayout.hpp>
#include <FslSimpleUI/Base/UIColors.hpp>
#include <FslSimpleUI/Controls/Charts/AreaChart.hpp>
#include <FslSimpleUI/Theme/Base/IThemeFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/UI/Benchmark/Activity/ActivityStack.hpp>
#include <Shared/UI/Benchmark/App/SimpleDialogActivityFactory.hpp>
#include <Shared/UI/Benchmark/App/TestApp.hpp>


namespace Fsl
{
  TestApp::TestApp(const UIDemoAppExtensionCreateInfo& createInfo)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<CustomUIDemoAppExtension>(createInfo, m_uiEventListener.GetListener(), "UIAtlasBig/UIAtlas_160dpi"))
    , m_windowMetrics(createInfo.WindowMetrics)
    , m_background(createInfo.WindowMetrics, createInfo.DemoServiceProvider)
  {
    m_controlFactory =
      UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension, false, createInfo.RenderCreateInfo.MaterialCreateInfo.DisableOpaqueMaterials);

    // if (true)
    //{
    m_uiProfile = CreateUI(*m_controlFactory);
    m_uiExtension->SetMainWindow(m_uiProfile.Layout);
    //}
    // else
    //{
    //  auto lbl0 = m_controlFactory->CreateLabel("L0");
    //  auto lbl1 = m_controlFactory->CreateLabel("L1");
    //  auto img2 = m_controlFactory->CreateImage(m_controlFactory->GetResources().GetBasicFillSprite());
    //  auto lbl3 = m_controlFactory->CreateLabel("L3");
    //  lbl1->SetAlignmentX(UI::ItemAlignment::Far);
    //  img2->SetContentColor(UI::UIColors::Black());
    //  auto custom = std::make_shared<UI::FillLayout>(m_controlFactory->GetContext());
    //  custom->AddChild(lbl0);
    //  custom->AddChild(lbl1);
    //  custom->AddChild(img2);
    //  custom->AddChild(lbl3);
    //  m_uiExtension->SetMainWindow(custom);
    //}


    // m_uiProfile.ActivityStack->Push(std::make_shared<UI::SimpleDialogActivity>(m_uiProfile.ActivityStack, m_controlFactory));

    // FSLLOG3_INFO("Ready");
  }


  TestApp::~TestApp() = default;


  void TestApp::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (!theEvent->IsHandled() && m_uiProfile.ActivityStack->IsEmpty())
    {
      if (theEvent->GetSource() == m_uiProfile.ButtonShowDialog0)
      {
        auto activity = UI::SimpleDialogActivityFactory::CreateLeftDialog(m_uiProfile.ActivityStack, m_controlFactory);
        m_uiProfile.ActivityStack->Push(activity);
      }
      else if (theEvent->GetSource() == m_uiProfile.ButtonShowDialog1)
      {
        auto activity = UI::SimpleDialogActivityFactory::CreateCenterDialog(m_uiProfile.ActivityStack, m_controlFactory);
        m_uiProfile.ActivityStack->Push(activity);
      }
      else if (theEvent->GetSource() == m_uiProfile.ButtonShowDialog2)
      {
        auto activity = UI::SimpleDialogActivityFactory::CreateRightDialog(m_uiProfile.ActivityStack, m_controlFactory);
        m_uiProfile.ActivityStack->Push(activity);
      }
    }
  }

  void TestApp::OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& /*theEvent*/)
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


  void TestApp::OnKeyEvent(const KeyEvent& event)
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
    default:
      break;
    }
  }


  UI::UIColor TestApp::GetRootColor() const
  {
    return m_uiProfile.Layout ? m_uiProfile.Layout->GetBaseColor() : UI::UIColors::White();
  }

  bool TestApp::TrySetRootColor(const UI::UIColor color)
  {
    return m_uiProfile.Layout->SetBaseColor(color);
  }

  const UI::IRenderSystemBase& TestApp::GetRenderSystem() const
  {
    return m_uiExtension->GetRenderSystem();
  }


  UI::IRenderSystemBase* TestApp::TryGetRenderSystem()
  {
    return m_uiExtension->TryGetRenderSystem();
  }


  void TestApp::SetCustomViewport(const PxViewport& viewportPx)
  {
    m_uiExtension->SYS_SetRenderSystemViewport(viewportPx);
    m_background.SYS_SetRenderSystemViewport(viewportPx);

    DemoWindowMetrics metrics(TypeConverter::To<PxExtent2D>(viewportPx.GetSize()), m_windowMetrics.ExactDpi, m_windowMetrics.DensityDpi);
    OnConfigurationChanged(metrics);
  }


  void TestApp::SetUseDrawCache(const bool useDrawCache)
  {
    m_uiExtension->SetUseDrawCache(useDrawCache);
  }


  bool TestApp::IsUIIdle() const
  {
    return m_uiExtension->IsIdle();
  }


  void TestApp::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    FSL_PARAM_NOT_USED(windowMetrics);
    m_background.ConfigurationChanged(windowMetrics);
    m_windowMetrics = windowMetrics;
    // UpdateResourceScale();
  }

  void TestApp::Update(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
    // const bool hasActivities = !m_uiProfile.ActivityStack->IsEmpty();

    if (m_uiProfile.ActivityStack)
    {
      m_uiProfile.AppLayout->SetBaseColor(m_uiProfile.ActivityStack->GetDesiredParentColor());
    }
  }

  void TestApp::Draw()
  {
    m_background.Draw();

    // if (!m_uiExtension->IsIdle())
    //{
    //  FSLLOG3_INFO("UI system is not idle");
    //}
    m_uiExtension->Draw();
  }


  void TestApp::OnDrawSkipped()
  {
  }


  void TestApp::SetDefaultValues()
  {
  }


  TestApp::MainUI TestApp::CreateUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    auto context = m_uiExtension->GetContext();

    auto winLayout0 = CreateSwitchUI(uiFactory);
    auto winLayout1 = CreateCheckBoxUI(uiFactory);
    auto winLayout2 = CreateRadioButtonUI(uiFactory);
    auto winLayout3 = CreateSliderUI(uiFactory, UI::LayoutOrientation::Horizontal);
    auto winLayout4 = CreateSliderUI(uiFactory, UI::LayoutOrientation::Vertical);
    auto winLayout5 = CreateTextButtonUI(uiFactory);
    auto winLayout6 = CreateFmtValueLabelUI(uiFactory);
    auto winLayout7 = CreateBasicImageUI(uiFactory);
    auto winLayout8 = CreateLabelUI(uiFactory);

    // auto winLayout8 = CreateSwitchUI(uiFactory);
    auto winLayout9 = CreateCheckBoxUI(uiFactory);
    auto winLayout10 = CreateRadioButtonUI(uiFactory);
    auto winLayout11 = CreateSliderUI2(uiFactory, UI::LayoutOrientation::Horizontal);
    auto winLayout12 = CreateSliderUI2(uiFactory, UI::LayoutOrientation::Vertical);
    auto winLayout13 = CreateTextButtonUI(uiFactory);
    auto winLayout14 = CreateFmtValueLabelUI(uiFactory);
    auto winLayout15 = CreateBasicImageUI(uiFactory);

    auto winLayout16 = CreateSwitchUI(uiFactory);
    auto winLayout17 = CreateCheckBoxUI(uiFactory);
    auto winLayout18 = CreateRadioButtonUI(uiFactory);
    auto winLayout19 = CreateSliderUI(uiFactory, UI::LayoutOrientation::Horizontal);
    auto winLayout20 = CreateSliderUI(uiFactory, UI::LayoutOrientation::Vertical);
    auto winLayout21 = CreateTextButtonUI(uiFactory);
    auto winLayout22 = CreateFmtValueLabelUI(uiFactory);
    auto winLayout23 = CreateBasicImageUI(uiFactory);

    auto labelCaption = uiFactory.CreateLabel("UI Benchmark", UI::Theme::FontType::Header);
    labelCaption->SetAlignmentX(UI::ItemAlignment::Center);
    auto topBar = uiFactory.CreateTopBar(labelCaption);


    auto window0 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout0.Layout, UI::ItemAlignment::Stretch);
    auto window1 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout1.Layout, UI::ItemAlignment::Stretch);
    auto window2 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout2.Layout, UI::ItemAlignment::Stretch);
    auto window3 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout3.Layout, UI::ItemAlignment::Stretch);
    auto window4 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout4.Layout, UI::ItemAlignment::Stretch);
    auto window5 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout5.Layout, UI::ItemAlignment::Stretch);
    auto window6 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout6.Layout, UI::ItemAlignment::Stretch);

    auto window7 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout7.Layout, UI::ItemAlignment::Stretch);
    auto window8 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout8.Layout, UI::ItemAlignment::Stretch);
    auto window9 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout9.Layout, UI::ItemAlignment::Stretch);
    auto window10 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout10.Layout, UI::ItemAlignment::Stretch);
    auto window11 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout11.Layout, UI::ItemAlignment::Stretch);
    auto window12 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout12.Layout, UI::ItemAlignment::Stretch);
    auto window13 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout13.Layout, UI::ItemAlignment::Stretch);

    auto window14 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout14.Layout, UI::ItemAlignment::Stretch);
    auto window15 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout15.Layout, UI::ItemAlignment::Stretch);
    auto window16 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout16.Layout, UI::ItemAlignment::Stretch);
    auto window17 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout17.Layout, UI::ItemAlignment::Stretch);
    auto window18 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout18.Layout, UI::ItemAlignment::Stretch);
    auto window19 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout19.Layout, UI::ItemAlignment::Stretch);
    auto window20 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout20.Layout, UI::ItemAlignment::Stretch);

    auto window21 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout21.Layout, UI::ItemAlignment::Stretch);
    auto window22 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout22.Layout, UI::ItemAlignment::Stretch);
    auto window23 = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, winLayout23.Layout, UI::ItemAlignment::Stretch);

    auto middleLayout = std::make_shared<UI::UniformWrapLayout>(context);
    middleLayout->SetAlignmentX(UI::ItemAlignment::Center);
    middleLayout->SetAlignmentY(UI::ItemAlignment::Center);
    middleLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    middleLayout->SetSpacing(DpSize2DF::Create(4.0f, 4.0f));
    middleLayout->AddChild(window0);
    middleLayout->AddChild(window1);
    middleLayout->AddChild(window2);
    middleLayout->AddChild(window3);
    middleLayout->AddChild(window4);
    middleLayout->AddChild(window5);
    middleLayout->AddChild(window6);
    middleLayout->AddChild(window7);
    middleLayout->AddChild(window8);
    middleLayout->AddChild(window9);
    middleLayout->AddChild(window10);
    middleLayout->AddChild(window11);
    middleLayout->AddChild(window12);
    middleLayout->AddChild(window13);
    middleLayout->AddChild(window14);
    middleLayout->AddChild(window15);
    middleLayout->AddChild(window16);
    middleLayout->AddChild(window17);
    middleLayout->AddChild(window18);
    middleLayout->AddChild(window19);
    middleLayout->AddChild(window20);
    middleLayout->AddChild(window21);
    middleLayout->AddChild(window22);
    middleLayout->AddChild(window23);

    auto btnShowDialog0 = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Left");
    auto btnShowDialog1 = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Center");
    auto btnShowDialog2 = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Right");
    btnShowDialog0->SetAlignmentX(UI::ItemAlignment::Stretch);
    btnShowDialog1->SetAlignmentX(UI::ItemAlignment::Stretch);
    btnShowDialog2->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto bottomLayout = std::make_shared<UI::UniformStackLayout>(context);
    bottomLayout->SetAlignmentX(UI::ItemAlignment::Center);
    bottomLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    bottomLayout->AddChild(btnShowDialog0);
    bottomLayout->AddChild(btnShowDialog1);
    bottomLayout->AddChild(btnShowDialog2);

    auto bottomBar = uiFactory.CreateBottomBar(bottomLayout);


    auto mainLayout = std::make_shared<UI::GridLayout>(context);
    mainLayout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    mainLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    mainLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Star, 1.0f));
    mainLayout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    mainLayout->AddChild(topBar, 0, 0);
    mainLayout->AddChild(middleLayout, 0, 1);
    mainLayout->AddChild(bottomBar, 0, 2);

    auto fillLayout = std::make_shared<UI::FillLayout>(context);
    fillLayout->AddChild(mainLayout);

    auto activityStack = std::make_shared<UI::ActivityStack>(context);
    fillLayout->AddChild(activityStack);

    return {fillLayout, mainLayout, activityStack, btnShowDialog0, btnShowDialog1, btnShowDialog2};
  }

  TestApp::BasicUI TestApp::CreateSwitchUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    auto context = m_uiExtension->GetContext();

    auto winLabel = uiFactory.CreateLabel("Switches", UI::Theme::FontType::Header);
    winLabel->SetAlignmentX(UI::ItemAlignment::Center);
    auto win0 = uiFactory.CreateSwitch("Switch #0", true);
    auto win1 = uiFactory.CreateSwitch("Switch #1", false);
    auto win2 = uiFactory.CreateSwitch("Switch #2", true);

    auto layout = std::make_shared<UI::StackLayout>(context);
    layout->SetOrientation(UI::LayoutOrientation::Vertical);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->AddChild(winLabel);
    layout->AddChild(win0);
    layout->AddChild(win1);
    layout->AddChild(win2);
    return {layout};
  }

  TestApp::BasicUI TestApp::CreateCheckBoxUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    auto context = m_uiExtension->GetContext();

    auto winLabel = uiFactory.CreateLabel("CheckBoxes", UI::Theme::FontType::Header);
    winLabel->SetAlignmentX(UI::ItemAlignment::Center);
    auto win0 = uiFactory.CreateCheckBox("CheckBox #0", true);
    auto win1 = uiFactory.CreateCheckBox("CheckBox #1", false);
    auto win2 = uiFactory.CreateCheckBox("CheckBox #2", true);

    auto layout = std::make_shared<UI::StackLayout>(context);
    layout->SetOrientation(UI::LayoutOrientation::Vertical);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->AddChild(winLabel);
    layout->AddChild(win0);
    layout->AddChild(win1);
    layout->AddChild(win2);
    return {layout};
  }

  TestApp::BasicUI TestApp::CreateRadioButtonUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    auto context = m_uiExtension->GetContext();

    auto winLabel = uiFactory.CreateLabel("RadioButtons", UI::Theme::FontType::Header);
    winLabel->SetAlignmentX(UI::ItemAlignment::Center);
    auto group = uiFactory.CreateRadioGroup("rg0");
    auto win0 = uiFactory.CreateRadioButton(group, "RadioButton #0", false);
    auto win1 = uiFactory.CreateRadioButton(group, "RadioButton #1", true);
    auto win2 = uiFactory.CreateRadioButton(group, "RadioButton #2", false);

    auto layout = std::make_shared<UI::StackLayout>(context);
    layout->SetOrientation(UI::LayoutOrientation::Vertical);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->AddChild(winLabel);
    layout->AddChild(win0);
    layout->AddChild(win1);
    layout->AddChild(win2);
    return {layout};
  }

  TestApp::BasicUI TestApp::CreateSliderUI(UI::Theme::IThemeControlFactory& uiFactory, const UI::LayoutOrientation orientation)
  {
    auto context = m_uiExtension->GetContext();

    const StringViewLite caption(orientation == UI::LayoutOrientation::Horizontal ? "Horz sliders" : "Vert sliders");

    auto winLabel = uiFactory.CreateLabel(caption, UI::Theme::FontType::Header);
    winLabel->SetAlignmentX(UI::ItemAlignment::Center);
    auto win0 = uiFactory.CreateSliderFmtValue(orientation, ConstrainedValue<int32_t>(0, 0, 100));
    auto win1 = uiFactory.CreateSliderFmtValue(orientation, ConstrainedValue<int32_t>(50, 0, 100));
    auto win2 = uiFactory.CreateSliderFmtValue(orientation, ConstrainedValue<int32_t>(80, 0, 100));


    auto layout = std::make_shared<UI::ComplexStackLayout>(context);
    layout->SetOrientation(UI::LayoutOrientation::Vertical);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->SetAlignmentY(UI::ItemAlignment::Stretch);
    layout->AddChild(winLabel, UI::LayoutLength(UI::LayoutUnitType::Auto));

    if (orientation == UI::LayoutOrientation::Horizontal)
    {
      layout->AddChild(win0, UI::LayoutLength(UI::LayoutUnitType::Auto));
      layout->AddChild(win1, UI::LayoutLength(UI::LayoutUnitType::Auto));
      layout->AddChild(win2, UI::LayoutLength(UI::LayoutUnitType::Auto));
    }
    else
    {
      auto layout0 = std::make_shared<UI::StackLayout>(context);
      layout0->SetOrientation(UI::LayoutOrientation::Horizontal);
      layout0->SetAlignmentX(UI::ItemAlignment::Center);
      layout0->SetAlignmentY(UI::ItemAlignment::Stretch);
      layout0->AddChild(win0);
      layout0->AddChild(win1);
      layout0->AddChild(win2);

      layout->AddChild(layout0, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    }
    return {layout};
  }

  TestApp::BasicUI TestApp::CreateSliderUI2(UI::Theme::IThemeControlFactory& uiFactory, const UI::LayoutOrientation orientation)
  {
    auto context = m_uiExtension->GetContext();

    const StringViewLite caption(orientation == UI::LayoutOrientation::Horizontal ? "Horz sliders" : " Vert sliders");

    auto winLabel = uiFactory.CreateLabel(caption, UI::Theme::FontType::Header);
    winLabel->SetAlignmentX(UI::ItemAlignment::Center);
    auto win0 = uiFactory.CreateSlider(orientation, ConstrainedValue<int32_t>(0, 0, 100));
    auto win1 = uiFactory.CreateSlider(orientation, ConstrainedValue<int32_t>(50, 0, 100));
    auto win2 = uiFactory.CreateSlider(orientation, ConstrainedValue<int32_t>(80, 0, 100));


    auto layout = std::make_shared<UI::ComplexStackLayout>(context);
    layout->SetOrientation(UI::LayoutOrientation::Vertical);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->SetAlignmentY(UI::ItemAlignment::Stretch);
    layout->AddChild(winLabel, UI::LayoutLength(UI::LayoutUnitType::Auto));

    if (orientation == UI::LayoutOrientation::Horizontal)
    {
      layout->AddChild(win0, UI::LayoutLength(UI::LayoutUnitType::Auto));
      layout->AddChild(win1, UI::LayoutLength(UI::LayoutUnitType::Auto));
      layout->AddChild(win2, UI::LayoutLength(UI::LayoutUnitType::Auto));
    }
    else
    {
      auto layout0 = std::make_shared<UI::StackLayout>(context);
      layout0->SetOrientation(UI::LayoutOrientation::Horizontal);
      layout0->SetAlignmentX(UI::ItemAlignment::Center);
      layout0->SetAlignmentY(UI::ItemAlignment::Stretch);
      layout0->AddChild(win0);
      layout0->AddChild(win1);
      layout0->AddChild(win2);

      layout->AddChild(layout0, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    }
    return {layout};
  }

  TestApp::BasicUI TestApp::CreateTextButtonUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    auto context = m_uiExtension->GetContext();

    auto winLabel = uiFactory.CreateLabel("Text buttons", UI::Theme::FontType::Header);
    winLabel->SetAlignmentX(UI::ItemAlignment::Center);
    auto win0 = uiFactory.CreateTextButton(UI::Theme::ButtonType::Contained, "Button #0");
    auto win1 = uiFactory.CreateTextButton(UI::Theme::ButtonType::Outlined, "Button #1");
    auto win2 = uiFactory.CreateTextButton(UI::Theme::ButtonType::Text, "Button #2");
    win0->SetAlignmentX(UI::ItemAlignment::Stretch);
    win1->SetAlignmentX(UI::ItemAlignment::Stretch);
    win2->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto layout = std::make_shared<UI::StackLayout>(context);
    layout->SetOrientation(UI::LayoutOrientation::Vertical);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->AddChild(winLabel);
    layout->AddChild(win0);
    layout->AddChild(win1);
    layout->AddChild(win2);
    return {layout};
  }

  TestApp::BasicUI TestApp::CreateFmtValueLabelUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    auto context = m_uiExtension->GetContext();

    auto winLabel = uiFactory.CreateLabel("Labels", UI::Theme::FontType::Header);
    winLabel->SetAlignmentX(UI::ItemAlignment::Center);
    auto win0 = uiFactory.CreateFmtValueLabel(32, "Value1: {}");
    auto win1 = uiFactory.CreateFmtValueLabel(3.14f, "Value2: {}. pyq");
    auto win2 = uiFactory.CreateFmtValueLabel(42, "There are {} items");
    // auto win3 = uiFactory.CreateLabel("Normal label 'pyq,;'", UI::Theme::FontType::Header);
    // auto win4 = uiFactory.CreateLabel("Normal label 'CTP,;'", UI::Theme::FontType::Header);
    auto win3 = uiFactory.CreateLabel("Normal label 'pyq,;'");
    auto win4 = uiFactory.CreateLabel("Normal label 'CTP,;'");

    auto contentLayout = std::make_shared<UI::StackLayout>(context);
    contentLayout->SetOrientation(UI::LayoutOrientation::Vertical);
    contentLayout->SetAlignmentY(UI::ItemAlignment::Center);
    contentLayout->AddChild(win0);
    contentLayout->AddChild(win1);
    contentLayout->AddChild(win2);
    contentLayout->AddChild(win3);
    contentLayout->AddChild(win4);

    auto layout = std::make_shared<UI::ComplexStackLayout>(context);
    layout->SetOrientation(UI::LayoutOrientation::Vertical);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->SetAlignmentY(UI::ItemAlignment::Stretch);
    layout->AddChild(winLabel, UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(contentLayout, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    return {layout};
  }

  TestApp::BasicUI TestApp::CreateBasicImageUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    auto context = m_uiExtension->GetContext();

    auto testSprite = uiFactory.GetResources().GetBasicTestSprite();

    auto winLabel = uiFactory.CreateLabel("BasicImages", UI::Theme::FontType::Header);
    winLabel->SetAlignmentX(UI::ItemAlignment::Center);
    auto win0 = uiFactory.CreateImage(testSprite);
    auto win1 = uiFactory.CreateImage(testSprite);
    auto win2 = uiFactory.CreateImage(testSprite);
    win0->SetAlignmentX(UI::ItemAlignment::Near);
    win1->SetAlignmentX(UI::ItemAlignment::Center);
    win2->SetAlignmentX(UI::ItemAlignment::Far);

    auto layout = std::make_shared<UI::StackLayout>(context);
    layout->SetOrientation(UI::LayoutOrientation::Vertical);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->AddChild(winLabel);
    layout->AddChild(win0);
    layout->AddChild(win1);
    layout->AddChild(win2);
    return {layout};
  }

  TestApp::BasicUI TestApp::CreateLabelUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    auto context = m_uiExtension->GetContext();

    auto fillSprite = uiFactory.GetResources().GetBasicMiniFillSprite(true);

    constexpr UI::UIColor Col0(PackedColor32(0xFF303030));
    constexpr UI::UIColor Col1(PackedColor32(0xFF202020));
    // constexpr Color col0(0x60606060);
    // constexpr Color col1(0x40404040);

    auto winLabel = uiFactory.CreateLabel("Labels", UI::Theme::FontType::Header);
    winLabel->SetAlignmentX(UI::ItemAlignment::Center);
    auto win0 = uiFactory.CreateLabel("CBwfgjpqy", UI::Theme::FontType::Default);
    auto win1 = uiFactory.CreateLabel("cbWFGJPQY", UI::Theme::FontType::Default);
    auto win2 = uiFactory.CreateDivider(UI::LayoutOrientation::Horizontal);
    win2->SetAlignmentX(UI::ItemAlignment::Stretch);
    auto win3 = uiFactory.CreateLabel("CBwfgjpqy", UI::Theme::FontType::Header);
    auto win4 = uiFactory.CreateLabel("cbWFGJPQY", UI::Theme::FontType::Header);

    auto img0 = uiFactory.CreateImage(fillSprite);
    img0->SetScalePolicy(UI::ItemScalePolicy::Fit);
    img0->SetAlignmentX(UI::ItemAlignment::Stretch);
    img0->SetAlignmentY(UI::ItemAlignment::Stretch);
    img0->SetContentColor(Col0);

    auto img1 = uiFactory.CreateImage(fillSprite);
    img1->SetScalePolicy(UI::ItemScalePolicy::Fit);
    img1->SetAlignmentX(UI::ItemAlignment::Stretch);
    img1->SetAlignmentY(UI::ItemAlignment::Stretch);
    img1->SetContentColor(Col1);

    // auto img2 = uiFactory.CreateImage(fillSprite);
    // img2->SetScalePolicy(UI::ItemScalePolicy::Fit);
    // img2->SetAlignmentX(UI::ItemAlignment::Stretch);
    // img2->SetAlignmentY(UI::ItemAlignment::Stretch);
    // img2->SetContentColor(col0);

    auto img3 = uiFactory.CreateImage(fillSprite);
    img3->SetScalePolicy(UI::ItemScalePolicy::Fit);
    img3->SetAlignmentX(UI::ItemAlignment::Stretch);
    img3->SetAlignmentY(UI::ItemAlignment::Stretch);
    img3->SetContentColor(Col0);

    auto img4 = uiFactory.CreateImage(fillSprite);
    img4->SetScalePolicy(UI::ItemScalePolicy::Fit);
    img4->SetAlignmentX(UI::ItemAlignment::Stretch);
    img4->SetAlignmentY(UI::ItemAlignment::Stretch);
    img4->SetContentColor(Col1);


    auto back0 = std::make_shared<UI::FillLayout>(context);
    back0->SetAlignmentX(UI::ItemAlignment::Near);
    back0->AddChild(img0);
    back0->AddChild(win0);

    auto back1 = std::make_shared<UI::FillLayout>(context);
    back1->SetAlignmentX(UI::ItemAlignment::Near);
    back1->AddChild(img1);
    back1->AddChild(win1);

    // auto back2 = std::make_shared<UI::FillLayout>(context);
    // back2->AddChild(img2);
    // back2->AddChild(win2);

    auto back3 = std::make_shared<UI::FillLayout>(context);
    back3->SetAlignmentX(UI::ItemAlignment::Near);
    back3->AddChild(img3);
    back3->AddChild(win3);

    auto back4 = std::make_shared<UI::FillLayout>(context);
    back4->SetAlignmentX(UI::ItemAlignment::Near);
    back4->AddChild(img4);
    back4->AddChild(win4);

    auto contentLayout = std::make_shared<UI::StackLayout>(context);
    contentLayout->SetOrientation(UI::LayoutOrientation::Vertical);
    contentLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    contentLayout->SetAlignmentY(UI::ItemAlignment::Center);
    contentLayout->AddChild(back0);
    contentLayout->AddChild(back1);
    contentLayout->AddChild(win2);
    contentLayout->AddChild(back3);
    contentLayout->AddChild(back4);

    auto layout = std::make_shared<UI::ComplexStackLayout>(context);
    layout->SetOrientation(UI::LayoutOrientation::Vertical);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->SetAlignmentY(UI::ItemAlignment::Stretch);
    layout->AddChild(winLabel, UI::LayoutLength(UI::LayoutUnitType::Auto));
    layout->AddChild(contentLayout, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    return {layout};
  }

}
