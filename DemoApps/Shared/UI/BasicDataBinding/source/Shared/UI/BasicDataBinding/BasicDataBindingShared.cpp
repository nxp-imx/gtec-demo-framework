/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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
#include <FslDataBinding/Base/Bind/ConverterBinding.hpp>
#include <FslDataBinding/Base/Bind/MultiConverterBinding.hpp>
#include <FslDataBinding/Base/Bind/TwoWayConverterBinding.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <Shared/UI/BasicDataBinding/BasicDataBindingShared.hpp>
// #include <FslSimpleUI/Base/Control/SimpleImageButton.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/UniformWrapLayout.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>


// #include <FslDataBinding/Base/DataBindingServiceDebug.hpp>
// #include <FslBase/IO/Path.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalColors
    {
      constexpr UI::UIColor ArrowColor(PackedColor32(0xFFC0C0C0));
    }

    namespace LocalConfig
    {
      constexpr IO::PathView MainUIAtlas("UIAtlas/UIAtlas_160dpi");

    }
    namespace LocalExample1
    {
      constexpr ConstrainedValue<int32_t> Slider1Constraint(40, 0, 100);
      constexpr ConstrainedValue<int32_t> Slider2Constraint(0, 0, 100);
      constexpr ConstrainedValue<int32_t> Slider3Constraint(0, -20, 120);
      constexpr ConstrainedValue<int32_t> Slider4Constraint(0, 20, 80);
    }

    namespace LocalExample3
    {
      constexpr ConstrainedValue<uint8_t> SliderColorChannelConstraint(255, 0, 255);
    }

    namespace LocalExample4
    {
      constexpr ConstrainedValue<uint8_t> Slider1Constraint(40, 0, 255);
      constexpr ConstrainedValue<float> Slider2Constraint(0, 0.0f, 1.0f);
      constexpr ConstrainedValue<int32_t> Slider3Constraint(0, 0, 255);
      constexpr ConstrainedValue<float> Slider4Constraint(0.0f, 0.0f, 255.0f);
    }

    // namespace LocalColor
    //{
    //   constexpr Color Profile0(0xFF7B9E05);     // green
    //   constexpr Color Profile1(0xFF882807);     // brown
    //   constexpr Color Profile2(0xFF614985);     // purple
    //   constexpr Color Profile3(0xFFC5A206);     // light yellow
    //   constexpr Color LightBlue(0xFF3488A7);    // light blue
    //   constexpr Color Profile5(0xFFCC7000);     // orange
    //   constexpr Color Profile6(0xFFA00000);     // red

    //}

    std::shared_ptr<UI::Image> CreateArrow(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<ImageSprite>& sprite)
    {
      auto image = uiFactory.CreateImage(sprite);
      image->SetAlignmentX(UI::ItemAlignment::Center);
      image->SetAlignmentY(UI::ItemAlignment::Center);
      image->SetContentColor(LocalColors::ArrowColor);
      return image;
    }

    std::shared_ptr<UI::Image> CreateSmallArrow(UI::Theme::IThemeControlFactory& uiFactory, const std::shared_ptr<ImageSprite>& sprite)
    {
      auto arrow = CreateArrow(uiFactory, sprite);
      arrow->SetHeight(UI::DpLayoutSize1D::Create(24));
      return arrow;
    }
  }


  BasicDataBindingShared::BasicDataBindingShared(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), LocalConfig::MainUIAtlas))
  {
    FSLLOG3_INFO("Preparing UI");
    {    // Build a simple UI
      auto uiFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
      m_uiRecord = CreateUI(*uiFactory);

      // Register the root layout with the window manager
      m_uiExtension->GetWindowManager()->Add(m_uiRecord.Main);

      // DataBinding::DataBindingServiceDebug::TryToDotFormatFile(IO::Path("g:\\test.dot"), *uiFactory->GetContext()->UIDataBindingService);
    }


    SetDefaultValues();
    OnConfigurationChanged(config.WindowMetrics);
  }


  BasicDataBindingShared::~BasicDataBindingShared() = default;


  void BasicDataBindingShared::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_uiRecord.Example1.BtnSub)
    {
      m_uiRecord.Example1.Slider->SubValue(10);
    }
    else if (theEvent->GetSource() == m_uiRecord.Example1.BtnAdd)
    {
      m_uiRecord.Example1.Slider->AddValue(10);
    }
  }

  void BasicDataBindingShared::OnContentChanged([[maybe_unused]] const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    // if (theEvent->GetSource() == m_uiRecord.DrawOutlineCheckBox || theEvent->GetSource() == m_uiRecord.DrawShadowCheckBox)
    //{
    //  UpdateLinkedUIState();
    //}
  }


  void BasicDataBindingShared::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }
    // switch (event.GetKey())
    //{
    ////case VirtualKey::Delete:
    ////  break;
    // default:
    //   break;
    // }
  }


  void BasicDataBindingShared::OnConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    FSL_PARAM_NOT_USED(windowMetrics);
  }

  void BasicDataBindingShared::Update(const DemoTime& demoTime)
  {
    FSL_PARAM_NOT_USED(demoTime);
  }

  void BasicDataBindingShared::Draw()
  {
    m_uiExtension->Draw();
  }


  void BasicDataBindingShared::SetDefaultValues()
  {
  }


  BasicDataBindingShared::UIRecord BasicDataBindingShared::CreateUI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    const auto& context = uiFactory.GetContext();
    auto exampleLayout = std::make_shared<UI::UniformWrapLayout>(context);
    exampleLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    exampleLayout->SetSpacing(DpSize2DF::Create(4, 4));

    auto example1 = CreateExample1UI(uiFactory);
    auto example2 = CreateExample2UI(uiFactory);
    auto example3 = CreateExample3UI(uiFactory);
    auto example4 = CreateExample4UI(uiFactory);

    {    // One way data binding
      auto stackLayout = std::make_shared<UI::StackLayout>(context);
      auto caption = uiFactory.CreateLabel("One way binding", UI::Theme::FontType::Header);
      stackLayout->AddChild(caption);
      stackLayout->AddChild(example1.Main);
      auto background = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, stackLayout);
      exampleLayout->AddChild(background);
    }
    {    // one way conversion data binding
      auto stackLayout = std::make_shared<UI::StackLayout>(context);
      auto caption = uiFactory.CreateLabel("One way binding with conversion", UI::Theme::FontType::Header);
      stackLayout->AddChild(caption);
      stackLayout->AddChild(example3.Main);
      auto background = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, stackLayout);
      // background->SetAlignmentX(UI::ItemAlignment::Stretch);
      // background->SetAlignmentY(UI::ItemAlignment::Stretch);
      exampleLayout->AddChild(background);
    }
    {    // Two way data binding
      auto stackLayout = std::make_shared<UI::StackLayout>(context);
      auto caption = uiFactory.CreateLabel("Two way binding", UI::Theme::FontType::Header);
      stackLayout->AddChild(caption);
      stackLayout->AddChild(example2.Main);
      auto background = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, stackLayout);
      exampleLayout->AddChild(background);
    }
    {    // Two way data conversion binding
      auto stackLayout = std::make_shared<UI::StackLayout>(context);
      auto caption = uiFactory.CreateLabel("Two way conversion binding", UI::Theme::FontType::Header);
      stackLayout->AddChild(caption);
      stackLayout->AddChild(example4.Main);
      auto background = uiFactory.CreateBackgroundWindow(UI::Theme::WindowType::Normal, stackLayout);
      exampleLayout->AddChild(background);
    }
    return {exampleLayout, example1, example2};
  }


  BasicDataBindingShared::UIExampleRecord1 BasicDataBindingShared::CreateExample1UI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    const auto& context = uiFactory.GetContext();
    const auto& resources = uiFactory.GetResources();

    const auto& spriteDownArrow = resources.GetIconSprite(UI::Theme::IconType::NavigationArrowDownward);
    const auto& spriteRightArrow = resources.GetIconSprite(UI::Theme::IconType::NavigationArrowForward);
    // const auto spriteContentAdd = resources.GetIconSprite(UI::Theme::IconType::ContentAdd);
    // const auto spriteContentRemove = resources.GetIconSprite(UI::Theme::IconType::ContentRemove);


    // auto btnAdd = uiFactory.CreateImageButton(spriteContentAdd);
    // auto btnSub = uiFactory.CreateImageButton(spriteContentRemove);
    auto btnAdd = uiFactory.CreateTextButton(UI::Theme::ButtonType::Text, "+");
    auto btnSub = uiFactory.CreateTextButton(UI::Theme::ButtonType::Text, "-");

    auto slider = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample1::Slider1Constraint);
    auto lbl0 = uiFactory.CreateFmtValueLabel(static_cast<int32_t>(0), "val {}");
    auto lbl1 = uiFactory.CreateFmtValueLabel(static_cast<int32_t>(0));
    auto lbl2 = uiFactory.CreateFmtValueLabel(static_cast<int32_t>(0), "{} val");

    slider->SetAlignmentX(UI::ItemAlignment::Stretch);
    slider->SetWidth(UI::DpLayoutSize1D::Create(200));
    lbl0->SetAlignmentX(UI::ItemAlignment::Near);
    lbl1->SetAlignmentX(UI::ItemAlignment::Center);
    lbl2->SetAlignmentX(UI::ItemAlignment::Far);

    auto sliderLayout = std::make_shared<UI::ComplexStackLayout>(context);
    sliderLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    sliderLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    sliderLayout->AddChild(btnSub, UI::LayoutLength(UI::LayoutUnitType::Auto));
    sliderLayout->AddChild(slider, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    sliderLayout->AddChild(btnAdd, UI::LayoutLength(UI::LayoutUnitType::Auto));

    auto slider2 = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample1::Slider2Constraint);
    auto slider3 = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample1::Slider3Constraint);
    auto slider4 = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample1::Slider4Constraint);

    auto lblSlider2 = uiFactory.CreateFmtValueLabel(static_cast<int32_t>(0));
    auto lblSlider3 = uiFactory.CreateFmtValueLabel(static_cast<int32_t>(0));
    auto lblSlider4 = uiFactory.CreateFmtValueLabel(static_cast<int32_t>(0));
    lblSlider2->SetAlignmentX(UI::ItemAlignment::Far);
    lblSlider2->SetAlignmentY(UI::ItemAlignment::Center);
    lblSlider3->SetAlignmentX(UI::ItemAlignment::Far);
    lblSlider3->SetAlignmentY(UI::ItemAlignment::Center);
    lblSlider4->SetAlignmentX(UI::ItemAlignment::Far);
    lblSlider4->SetAlignmentY(UI::ItemAlignment::Center);

    auto sliderGrid = std::make_shared<UI::GridLayout>(context);
    sliderGrid->SetAlignmentX(UI::ItemAlignment::Stretch);
    sliderGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    sliderGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    sliderGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 20));
    sliderGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    sliderGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    sliderGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    sliderGrid->AddChild(slider2, 0, 0);
    sliderGrid->AddChild(slider3, 0, 1);
    sliderGrid->AddChild(slider4, 0, 2);
    sliderGrid->AddChild(CreateSmallArrow(uiFactory, spriteRightArrow), 1, 0);
    sliderGrid->AddChild(CreateSmallArrow(uiFactory, spriteRightArrow), 1, 1);
    sliderGrid->AddChild(CreateSmallArrow(uiFactory, spriteRightArrow), 1, 2);
    sliderGrid->AddChild(lblSlider2, 2, 0);
    sliderGrid->AddChild(lblSlider3, 2, 1);
    sliderGrid->AddChild(lblSlider4, 2, 2);

    auto layout = std::make_shared<UI::StackLayout>(context);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->SetOrientation(UI::LayoutOrientation::Vertical);
    layout->AddChild(sliderLayout);
    layout->AddChild(CreateArrow(uiFactory, spriteDownArrow));
    layout->AddChild(lbl0);
    layout->AddChild(lbl1);
    layout->AddChild(lbl2);
    layout->AddChild(sliderGrid);


    // Basic data binding
    {    // setup a one way data binding from the 'slider' to these four controls
      const auto hSliderProperty = slider->GetPropertyHandle(UI::Slider<int32_t>::PropertyValue);
      lbl0->SetBinding(UI::FmtValueLabel<int32_t>::PropertyContent, hSliderProperty);
      lbl1->SetBinding(UI::FmtValueLabel<int32_t>::PropertyContent, hSliderProperty);
      lbl2->SetBinding(UI::FmtValueLabel<int32_t>::PropertyContent, hSliderProperty);
      slider2->SetBinding(UI::Slider<int32_t>::PropertyValue, hSliderProperty);
      slider3->SetBinding(UI::Slider<int32_t>::PropertyValue, hSliderProperty);
      slider4->SetBinding(UI::Slider<int32_t>::PropertyValue, hSliderProperty);
    }
    {    // setup a one way data binding between each of the already linked sliders to their label
      const auto hSliderProperty2 = slider2->GetPropertyHandle(UI::Slider<int32_t>::PropertyValue);
      const auto hSliderProperty3 = slider3->GetPropertyHandle(UI::Slider<int32_t>::PropertyValue);
      const auto hSliderProperty4 = slider4->GetPropertyHandle(UI::Slider<int32_t>::PropertyValue);
      lblSlider2->SetBinding(UI::FmtValueLabel<int32_t>::PropertyContent, hSliderProperty2);
      lblSlider3->SetBinding(UI::FmtValueLabel<int32_t>::PropertyContent, hSliderProperty3);
      lblSlider4->SetBinding(UI::FmtValueLabel<int32_t>::PropertyContent, hSliderProperty4);
    }
    return {layout, btnSub, slider, btnAdd};
  }

  BasicDataBindingShared::UIExampleRecord2 BasicDataBindingShared::CreateExample2UI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    const auto& context = uiFactory.GetContext();
    const auto& resources = uiFactory.GetResources();
    const auto spriteUpArrow = resources.GetIconSprite(UI::Theme::IconType::NavigationArrowUpward);
    const auto spriteDownArrow = resources.GetIconSprite(UI::Theme::IconType::NavigationArrowDownward);
    const auto spriteRightArrow = resources.GetIconSprite(UI::Theme::IconType::NavigationArrowForward);

    auto slider1 = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample1::Slider1Constraint);
    slider1->SetAlignmentX(UI::ItemAlignment::Stretch);
    slider1->SetWidth(UI::DpLayoutSize1D::Create(200));

    auto lblSlider1 = uiFactory.CreateFmtValueLabel(static_cast<int32_t>(0));
    lblSlider1->SetAlignmentX(UI::ItemAlignment::Far);
    lblSlider1->SetAlignmentY(UI::ItemAlignment::Center);

    auto sliderLayout = std::make_shared<UI::ComplexStackLayout>(context);
    sliderLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    sliderLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    sliderLayout->AddChild(slider1, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    sliderLayout->AddChild(CreateSmallArrow(uiFactory, spriteRightArrow), UI::LayoutLength(UI::LayoutUnitType::Auto));
    sliderLayout->AddChild(lblSlider1, UI::LayoutLength(UI::LayoutUnitType::Fixed, 20));


    auto twoWayArrows = std::make_shared<UI::StackLayout>(context);
    twoWayArrows->SetOrientation(UI::LayoutOrientation::Horizontal);
    twoWayArrows->SetAlignmentX(UI::ItemAlignment::Center);
    twoWayArrows->AddChild(CreateArrow(uiFactory, spriteUpArrow));
    twoWayArrows->AddChild(CreateArrow(uiFactory, spriteDownArrow));

    auto slider2 = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample1::Slider2Constraint);
    auto slider3 = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample1::Slider3Constraint);
    auto slider4 = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample1::Slider4Constraint);

    auto lblSlider2 = uiFactory.CreateFmtValueLabel(static_cast<int32_t>(0));
    auto lblSlider3 = uiFactory.CreateFmtValueLabel(static_cast<int32_t>(0));
    auto lblSlider4 = uiFactory.CreateFmtValueLabel(static_cast<int32_t>(0));
    lblSlider2->SetAlignmentX(UI::ItemAlignment::Far);
    lblSlider2->SetAlignmentY(UI::ItemAlignment::Center);
    lblSlider3->SetAlignmentX(UI::ItemAlignment::Far);
    lblSlider3->SetAlignmentY(UI::ItemAlignment::Center);
    lblSlider4->SetAlignmentX(UI::ItemAlignment::Far);
    lblSlider4->SetAlignmentY(UI::ItemAlignment::Center);

    auto sliderGrid = std::make_shared<UI::GridLayout>(context);
    sliderGrid->SetAlignmentX(UI::ItemAlignment::Stretch);
    sliderGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    sliderGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    sliderGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 20));
    sliderGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    sliderGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    sliderGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    sliderGrid->AddChild(slider2, 0, 0);
    sliderGrid->AddChild(slider3, 0, 1);
    sliderGrid->AddChild(slider4, 0, 2);
    sliderGrid->AddChild(CreateSmallArrow(uiFactory, spriteRightArrow), 1, 0);
    sliderGrid->AddChild(CreateSmallArrow(uiFactory, spriteRightArrow), 1, 1);
    sliderGrid->AddChild(CreateSmallArrow(uiFactory, spriteRightArrow), 1, 2);
    sliderGrid->AddChild(lblSlider2, 2, 0);
    sliderGrid->AddChild(lblSlider3, 2, 1);
    sliderGrid->AddChild(lblSlider4, 2, 2);

    auto layout = std::make_shared<UI::StackLayout>(context);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->SetOrientation(UI::LayoutOrientation::Vertical);
    layout->AddChild(sliderLayout);
    layout->AddChild(twoWayArrows);
    layout->AddChild(sliderGrid);


    // Basic data binding
    {    // setup a two way data binding from the 'slider' to these four controls
      const auto hSliderProperty = slider1->GetPropertyHandle(UI::Slider<int32_t>::PropertyValue);
      slider2->SetBinding(UI::Slider<int32_t>::PropertyValue, hSliderProperty, DataBinding::BindingMode::TwoWay);
      slider3->SetBinding(UI::Slider<int32_t>::PropertyValue, hSliderProperty, DataBinding::BindingMode::TwoWay);
      slider4->SetBinding(UI::Slider<int32_t>::PropertyValue, hSliderProperty, DataBinding::BindingMode::TwoWay);
    }
    {    // setup a one way data binding between each of the already linked sliders to their label
      const auto hSliderProperty1 = slider1->GetPropertyHandle(UI::Slider<int32_t>::PropertyValue);
      const auto hSliderProperty2 = slider2->GetPropertyHandle(UI::Slider<int32_t>::PropertyValue);
      const auto hSliderProperty3 = slider3->GetPropertyHandle(UI::Slider<int32_t>::PropertyValue);
      const auto hSliderProperty4 = slider4->GetPropertyHandle(UI::Slider<int32_t>::PropertyValue);
      lblSlider1->SetBinding(UI::FmtValueLabel<int32_t>::PropertyContent, hSliderProperty1);
      lblSlider2->SetBinding(UI::FmtValueLabel<int32_t>::PropertyContent, hSliderProperty2);
      lblSlider3->SetBinding(UI::FmtValueLabel<int32_t>::PropertyContent, hSliderProperty3);
      lblSlider4->SetBinding(UI::FmtValueLabel<int32_t>::PropertyContent, hSliderProperty4);
    }
    return {layout};
  }


  BasicDataBindingShared::UIExampleRecord3 BasicDataBindingShared::CreateExample3UI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    const auto& context = uiFactory.GetContext();
    const auto& resources = uiFactory.GetResources();
    const auto& spriteDownArrow = resources.GetIconSprite(UI::Theme::IconType::NavigationArrowDownward);
    const auto& spriteRightArrow = resources.GetIconSprite(UI::Theme::IconType::NavigationArrowForward);
    const auto& spriteImage = resources.GetIconSprite(UI::Theme::IconType::Fill);


    auto layout = std::make_shared<UI::GridLayout>(context);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 4));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    // layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 300));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 34));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));

    auto sliderR = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample3::SliderColorChannelConstraint);
    auto sliderG = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample3::SliderColorChannelConstraint);
    auto sliderB = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample3::SliderColorChannelConstraint);
    auto sliderA = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample3::SliderColorChannelConstraint);
    {
      auto labelR = uiFactory.CreateLabel("R:");
      auto labelG = uiFactory.CreateLabel("G:");
      auto labelB = uiFactory.CreateLabel("B:");
      auto labelA = uiFactory.CreateLabel("A:");
      labelR->SetAlignmentY(UI::ItemAlignment::Center);
      labelG->SetAlignmentY(UI::ItemAlignment::Center);
      labelB->SetAlignmentY(UI::ItemAlignment::Center);
      labelA->SetAlignmentY(UI::ItemAlignment::Center);

      auto lblSliderR = uiFactory.CreateFmtValueLabel(static_cast<uint8_t>(0));
      auto lblSliderG = uiFactory.CreateFmtValueLabel(static_cast<uint8_t>(0));
      auto lblSliderB = uiFactory.CreateFmtValueLabel(static_cast<uint8_t>(0));
      auto lblSliderA = uiFactory.CreateFmtValueLabel(static_cast<uint8_t>(0));
      lblSliderR->SetAlignmentX(UI::ItemAlignment::Far);
      lblSliderG->SetAlignmentX(UI::ItemAlignment::Far);
      lblSliderB->SetAlignmentX(UI::ItemAlignment::Far);
      lblSliderA->SetAlignmentX(UI::ItemAlignment::Far);
      lblSliderR->SetAlignmentY(UI::ItemAlignment::Center);
      lblSliderG->SetAlignmentY(UI::ItemAlignment::Center);
      lblSliderB->SetAlignmentY(UI::ItemAlignment::Center);
      lblSliderA->SetAlignmentY(UI::ItemAlignment::Center);

      layout->AddChild(labelR, 0, 0);
      layout->AddChild(labelG, 0, 1);
      layout->AddChild(labelB, 0, 2);
      layout->AddChild(labelA, 0, 3);

      layout->AddChild(sliderR, 2, 0);
      layout->AddChild(sliderG, 2, 1);
      layout->AddChild(sliderB, 2, 2);
      layout->AddChild(sliderA, 2, 3);

      layout->AddChild(CreateSmallArrow(uiFactory, spriteRightArrow), 3, 0);
      layout->AddChild(CreateSmallArrow(uiFactory, spriteRightArrow), 3, 1);
      layout->AddChild(CreateSmallArrow(uiFactory, spriteRightArrow), 3, 2);
      layout->AddChild(CreateSmallArrow(uiFactory, spriteRightArrow), 3, 3);

      layout->AddChild(lblSliderR, 4, 0);
      layout->AddChild(lblSliderG, 4, 1);
      layout->AddChild(lblSliderB, 4, 2);
      layout->AddChild(lblSliderA, 4, 3);

      {    // setup a one way data binding between each of the already linked sliders to their label
        const auto hSliderPropertyR = sliderR->GetPropertyHandle(UI::Slider<uint8_t>::PropertyValue);
        const auto hSliderPropertyG = sliderG->GetPropertyHandle(UI::Slider<uint8_t>::PropertyValue);
        const auto hSliderPropertyB = sliderB->GetPropertyHandle(UI::Slider<uint8_t>::PropertyValue);
        const auto hSliderPropertyA = sliderA->GetPropertyHandle(UI::Slider<uint8_t>::PropertyValue);
        lblSliderR->SetBinding(UI::FmtValueLabel<uint8_t>::PropertyContent, hSliderPropertyR);
        lblSliderG->SetBinding(UI::FmtValueLabel<uint8_t>::PropertyContent, hSliderPropertyG);
        lblSliderB->SetBinding(UI::FmtValueLabel<uint8_t>::PropertyContent, hSliderPropertyB);
        lblSliderA->SetBinding(UI::FmtValueLabel<uint8_t>::PropertyContent, hSliderPropertyA);
      }
    }

    auto mainLayout = std::make_shared<UI::StackLayout>(context);
    mainLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    mainLayout->SetOrientation(UI::LayoutOrientation::Vertical);
    mainLayout->AddChild(layout);
    mainLayout->AddChild(CreateArrow(uiFactory, spriteDownArrow));
    {
      auto image = uiFactory.CreateImage(spriteImage);
      image->SetScalePolicy(UI::ItemScalePolicy::Fit);
      image->SetAlignmentX(UI::ItemAlignment::Stretch);
      image->SetAlignmentY(UI::ItemAlignment::Center);

      auto fmtLblR = uiFactory.CreateFmtValueLabel(0.0f);
      auto fmtLblG = uiFactory.CreateFmtValueLabel(0.0f);
      auto fmtLblB = uiFactory.CreateFmtValueLabel(0.0f);
      auto fmtLblA = uiFactory.CreateFmtValueLabel(0.0f);
      fmtLblR->SetAlignmentX(UI::ItemAlignment::Far);
      fmtLblG->SetAlignmentX(UI::ItemAlignment::Far);
      fmtLblB->SetAlignmentX(UI::ItemAlignment::Far);
      fmtLblA->SetAlignmentX(UI::ItemAlignment::Far);

      auto lineLayout = std::make_shared<UI::ComplexStackLayout>(context);
      lineLayout->SetSpacing(DpSize1DF::Create(6));
      lineLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
      lineLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
      lineLayout->AddChild(uiFactory.CreateLabel("R:"), {});
      lineLayout->AddChild(fmtLblR, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
      lineLayout->AddChild(uiFactory.CreateLabel("G:"), {});
      lineLayout->AddChild(fmtLblG, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
      lineLayout->AddChild(uiFactory.CreateLabel("B:"), {});
      lineLayout->AddChild(fmtLblB, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
      lineLayout->AddChild(uiFactory.CreateLabel("A:"), {});
      lineLayout->AddChild(fmtLblA, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));

      mainLayout->AddChild(lineLayout);
      mainLayout->AddChild(image);

      {    // setup a one way data binding between each of the already linked sliders to their label
        auto converterBinding = std::make_shared<Fsl::DataBinding::ConverterBinding<float, uint8_t>>([](const uint8_t value)
                                                                                                     { return static_cast<float>(value) / 255.0f; });

        auto multiConverterBinding = std::make_shared<Fsl::DataBinding::MultiConverterBinding<UI::UIColor, uint8_t, uint8_t, uint8_t, uint8_t>>(
          [](const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) { return UI::UIColor::CreateR8G8B8A8(r, g, b, a); });

        const auto hSliderPropertyR = sliderR->GetPropertyHandle(UI::Slider<uint8_t>::PropertyValue);
        const auto hSliderPropertyG = sliderG->GetPropertyHandle(UI::Slider<uint8_t>::PropertyValue);
        const auto hSliderPropertyB = sliderB->GetPropertyHandle(UI::Slider<uint8_t>::PropertyValue);
        const auto hSliderPropertyA = sliderA->GetPropertyHandle(UI::Slider<uint8_t>::PropertyValue);
        fmtLblR->SetBinding(UI::FmtValueLabel<float>::PropertyContent, DataBinding::Binding(converterBinding, hSliderPropertyR));
        fmtLblG->SetBinding(UI::FmtValueLabel<float>::PropertyContent, DataBinding::Binding(converterBinding, hSliderPropertyG));
        fmtLblB->SetBinding(UI::FmtValueLabel<float>::PropertyContent, DataBinding::Binding(converterBinding, hSliderPropertyB));
        fmtLblA->SetBinding(UI::FmtValueLabel<float>::PropertyContent, DataBinding::Binding(converterBinding, hSliderPropertyA));

        image->SetBinding(UI::Image::PropertyContentColor,
                          DataBinding::Binding(multiConverterBinding, hSliderPropertyR, hSliderPropertyG, hSliderPropertyB, hSliderPropertyA));
      }
    }
    return {mainLayout};
  }

  BasicDataBindingShared::UIExampleRecord4 BasicDataBindingShared::CreateExample4UI(UI::Theme::IThemeControlFactory& uiFactory)
  {
    const auto& context = uiFactory.GetContext();
    const auto& resources = uiFactory.GetResources();
    const auto spriteUpArrow = resources.GetIconSprite(UI::Theme::IconType::NavigationArrowUpward);
    const auto spriteDownArrow = resources.GetIconSprite(UI::Theme::IconType::NavigationArrowDownward);
    const auto spriteRightArrow = resources.GetIconSprite(UI::Theme::IconType::NavigationArrowForward);

    auto slider1 = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample4::Slider1Constraint);
    slider1->SetAlignmentX(UI::ItemAlignment::Stretch);
    slider1->SetWidth(UI::DpLayoutSize1D::Create(200));

    auto lblSlider1 = uiFactory.CreateFmtValueLabel(static_cast<uint8_t>(0));
    lblSlider1->SetAlignmentX(UI::ItemAlignment::Far);
    lblSlider1->SetAlignmentY(UI::ItemAlignment::Center);

    const float minSpacingDp = 45;

    auto sliderLayout = std::make_shared<UI::ComplexStackLayout>(context);
    sliderLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    sliderLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    sliderLayout->AddChild(slider1, UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    sliderLayout->AddChild(CreateSmallArrow(uiFactory, spriteRightArrow), UI::LayoutLength(UI::LayoutUnitType::Auto));
    sliderLayout->AddChild(lblSlider1, UI::LayoutLength(UI::LayoutUnitType::Fixed, minSpacingDp));


    auto twoWayArrows = std::make_shared<UI::StackLayout>(context);
    twoWayArrows->SetOrientation(UI::LayoutOrientation::Horizontal);
    twoWayArrows->SetAlignmentX(UI::ItemAlignment::Center);
    twoWayArrows->AddChild(CreateArrow(uiFactory, spriteUpArrow));
    twoWayArrows->AddChild(CreateArrow(uiFactory, spriteDownArrow));

    auto slider2 = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample4::Slider2Constraint);
    auto slider3 = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample4::Slider3Constraint);
    auto slider4 = uiFactory.CreateSlider(UI::LayoutOrientation::Horizontal, LocalExample4::Slider4Constraint);

    auto lblSlider2 = uiFactory.CreateFmtValueLabel(static_cast<float>(0));
    auto lblSlider3 = uiFactory.CreateFmtValueLabel(static_cast<int32_t>(0));
    auto lblSlider4 = uiFactory.CreateFmtValueLabel(static_cast<float>(0));
    lblSlider2->SetAlignmentX(UI::ItemAlignment::Far);
    lblSlider2->SetAlignmentY(UI::ItemAlignment::Center);
    lblSlider3->SetAlignmentX(UI::ItemAlignment::Far);
    lblSlider3->SetAlignmentY(UI::ItemAlignment::Center);
    lblSlider4->SetAlignmentX(UI::ItemAlignment::Far);
    lblSlider4->SetAlignmentY(UI::ItemAlignment::Center);

    auto sliderGrid = std::make_shared<UI::GridLayout>(context);
    sliderGrid->SetAlignmentX(UI::ItemAlignment::Stretch);
    sliderGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    sliderGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    sliderGrid->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, minSpacingDp));
    sliderGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    sliderGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    sliderGrid->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    sliderGrid->AddChild(slider2, 0, 0);
    sliderGrid->AddChild(slider3, 0, 1);
    sliderGrid->AddChild(slider4, 0, 2);
    sliderGrid->AddChild(CreateSmallArrow(uiFactory, spriteRightArrow), 1, 0);
    sliderGrid->AddChild(CreateSmallArrow(uiFactory, spriteRightArrow), 1, 1);
    sliderGrid->AddChild(CreateSmallArrow(uiFactory, spriteRightArrow), 1, 2);
    sliderGrid->AddChild(lblSlider2, 2, 0);
    sliderGrid->AddChild(lblSlider3, 2, 1);
    sliderGrid->AddChild(lblSlider4, 2, 2);

    auto layout = std::make_shared<UI::StackLayout>(context);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->SetOrientation(UI::LayoutOrientation::Vertical);
    layout->AddChild(sliderLayout);
    layout->AddChild(twoWayArrows);
    layout->AddChild(sliderGrid);


    // Basic data binding
    {    // setup a two way data binding from the 'slider' to these four controls
      auto convertingBinding2 = std::make_shared<Fsl::DataBinding::TwoWayConverterBinding<float, uint8_t>>(
        [](const uint8_t value) { return static_cast<float>(value) / 255.0f; },
        [](const float value) { return static_cast<uint8_t>(std::clamp(static_cast<int32_t>(std::round(value * 255.0f)), 0, 255)); });

      auto convertingBinding3 = std::make_shared<Fsl::DataBinding::TwoWayConverterBinding<int32_t, uint8_t>>(
        [](const uint8_t value) { return value; }, [](const int32_t value) { return static_cast<uint8_t>(std::clamp(value, 0, 255)); });

      auto convertingBinding4 = std::make_shared<Fsl::DataBinding::TwoWayConverterBinding<float, uint8_t>>(
        [](const uint8_t value) { return static_cast<float>(value); },
        [](const float value) { return static_cast<uint8_t>(std::clamp(static_cast<int32_t>(std::round(value)), 0, 255)); });

      const auto hSliderProperty = slider1->GetPropertyHandle(UI::Slider<uint8_t>::PropertyValue);
      slider2->SetBinding(UI::Slider<float>::PropertyValue, DataBinding::Binding(convertingBinding2, hSliderProperty));
      slider3->SetBinding(UI::Slider<int32_t>::PropertyValue, DataBinding::Binding(convertingBinding3, hSliderProperty));
      slider4->SetBinding(UI::Slider<float>::PropertyValue, DataBinding::Binding(convertingBinding4, hSliderProperty));
    }
    {    // setup a one way data binding between each of the already linked sliders to their label
      const auto hSliderProperty1 = slider1->GetPropertyHandle(UI::Slider<uint8_t>::PropertyValue);
      const auto hSliderProperty2 = slider2->GetPropertyHandle(UI::Slider<float>::PropertyValue);
      const auto hSliderProperty3 = slider3->GetPropertyHandle(UI::Slider<int32_t>::PropertyValue);
      const auto hSliderProperty4 = slider4->GetPropertyHandle(UI::Slider<float>::PropertyValue);
      lblSlider1->SetBinding(UI::FmtValueLabel<uint8_t>::PropertyContent, hSliderProperty1);
      lblSlider2->SetBinding(UI::FmtValueLabel<float>::PropertyContent, hSliderProperty2);
      lblSlider3->SetBinding(UI::FmtValueLabel<int32_t>::PropertyContent, hSliderProperty3);
      lblSlider4->SetBinding(UI::FmtValueLabel<float>::PropertyContent, hSliderProperty4);
    }
    return {layout};
  }

}
