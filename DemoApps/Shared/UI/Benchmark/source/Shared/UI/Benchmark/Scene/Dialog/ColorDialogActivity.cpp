/****************************************************************************************************************************************************
 * Copyright 2022, 2024 NXP
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

#include <FslDataBinding/Base/Bind/ConverterBinding.hpp>
#include <FslDataBinding/Base/Bind/MultiConverterBinding.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/KeyEvent.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/CheckBox.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/Slider.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <Shared/UI/Benchmark/App/SimpleDialogActivityFactory.hpp>
#include <Shared/UI/Benchmark/Scene/Dialog/ColorDialogActivity.hpp>
#include <Shared/UI/Benchmark/TextConfig.hpp>
#include <utility>

namespace Fsl::UI
{
  ColorDialogActivity::ColorDialogActivity(std::weak_ptr<IActivityStack> activityStack,
                                           const std::shared_ptr<Theme::IThemeControlFactory>& themeControlFactory, const UIColor initialColor)
    : DialogActivity(std::move(activityStack), themeControlFactory, std::make_shared<GridLayout>(themeControlFactory->GetContext()),
                     Theme::WindowType::DialogNormal, ItemAlignment::Far, ItemAlignment::Far)
  {
    const auto& context = themeControlFactory->GetContext();
    const auto& resources = themeControlFactory->GetResources();
    const auto spriteDownArrow = resources.GetIconSprite(Theme::IconType::NavigationArrowDownward);
    const auto spriteRightArrow = resources.GetIconSprite(Theme::IconType::NavigationArrowForward);
    const auto spriteImage = resources.GetIconSprite(Theme::IconType::Fill);

    auto layout = std::make_shared<UI::GridLayout>(context);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 4));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 300));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 34));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));

    const Color initialColorR8G8B8A8(initialColor.AsPackedColor32());
    auto sliderR =
      themeControlFactory->CreateSlider(UI::LayoutOrientation::Horizontal, ConstrainedValue<uint8_t>(initialColorR8G8B8A8.RawR(), 0, 255));
    auto sliderG =
      themeControlFactory->CreateSlider(UI::LayoutOrientation::Horizontal, ConstrainedValue<uint8_t>(initialColorR8G8B8A8.RawG(), 0, 255));
    auto sliderB =
      themeControlFactory->CreateSlider(UI::LayoutOrientation::Horizontal, ConstrainedValue<uint8_t>(initialColorR8G8B8A8.RawB(), 0, 255));
    auto sliderA =
      themeControlFactory->CreateSlider(UI::LayoutOrientation::Horizontal, ConstrainedValue<uint8_t>(initialColorR8G8B8A8.RawA(), 0, 255));
    {
      auto labelR = themeControlFactory->CreateLabel("R:");
      auto labelG = themeControlFactory->CreateLabel("G:");
      auto labelB = themeControlFactory->CreateLabel("B:");
      auto labelA = themeControlFactory->CreateLabel("A:");
      labelR->SetAlignmentY(UI::ItemAlignment::Center);
      labelG->SetAlignmentY(UI::ItemAlignment::Center);
      labelB->SetAlignmentY(UI::ItemAlignment::Center);
      labelA->SetAlignmentY(UI::ItemAlignment::Center);

      auto lblSliderR = themeControlFactory->CreateFmtValueLabel(static_cast<uint8_t>(0));
      auto lblSliderG = themeControlFactory->CreateFmtValueLabel(static_cast<uint8_t>(0));
      auto lblSliderB = themeControlFactory->CreateFmtValueLabel(static_cast<uint8_t>(0));
      auto lblSliderA = themeControlFactory->CreateFmtValueLabel(static_cast<uint8_t>(0));
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

      layout->AddChild(lblSliderR, 3, 0);
      layout->AddChild(lblSliderG, 3, 1);
      layout->AddChild(lblSliderB, 3, 2);
      layout->AddChild(lblSliderA, 3, 3);

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

    m_image = themeControlFactory->CreateImage(spriteImage);
    m_image->SetScalePolicy(UI::ItemScalePolicy::Fit);
    m_image->SetAlignmentX(UI::ItemAlignment::Far);
    m_image->SetAlignmentY(UI::ItemAlignment::Center);
    m_image->SetWidth(DpLayoutSize1D(32));
    m_image->SetWidth(DpLayoutSize1D(32));

    m_buttonBack = themeControlFactory->CreateTextButton(Theme::ButtonType::Contained, "Back");
    m_buttonBack->SetAlignmentX(ItemAlignment::Center);

    auto& rMainLayout = dynamic_cast<GridLayout&>(GetWindow());
    rMainLayout.AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    rMainLayout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    rMainLayout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    rMainLayout.AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    rMainLayout.AddChild(themeControlFactory->CreateLabel("Base color", Theme::FontType::Header), 0, 0);
    rMainLayout.AddChild(m_image, 0, 0);
    rMainLayout.AddChild(layout, 0, 1);
    rMainLayout.AddChild(m_buttonBack, 0, 2);


    {    // setup a one way data binding between each of the already linked sliders to their label
      auto multiConverterBinding = std::make_shared<Fsl::DataBinding::MultiConverterBinding<UI::UIColor, uint8_t, uint8_t, uint8_t, uint8_t>>(
        [](const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) { return UIColor(PackedColor32::CreateR8G8B8A8UNorm(r, g, b, a)); });

      const auto hSliderPropertyR = sliderR->GetPropertyHandle(UI::Slider<uint8_t>::PropertyValue);
      const auto hSliderPropertyG = sliderG->GetPropertyHandle(UI::Slider<uint8_t>::PropertyValue);
      const auto hSliderPropertyB = sliderB->GetPropertyHandle(UI::Slider<uint8_t>::PropertyValue);
      const auto hSliderPropertyA = sliderA->GetPropertyHandle(UI::Slider<uint8_t>::PropertyValue);

      m_image->SetBinding(UI::Image::PropertyContentColor,
                          DataBinding::Binding(multiConverterBinding, hSliderPropertyR, hSliderPropertyG, hSliderPropertyB, hSliderPropertyA));
    }
  }


  ColorDialogActivity::~ColorDialogActivity() = default;


  UI::UIColor ColorDialogActivity::GetCurrentColor() const
  {
    return m_image->GetContentColor();
  }


  void ColorDialogActivity::OnSelect(const std::shared_ptr<WindowSelectEvent>& theEvent)
  {
    if (m_state == State::Ready && !theEvent->IsHandled())
    {
      if (theEvent->GetSource() == m_buttonBack)
      {
        theEvent->Handled();
        DoScheduleClose();
      }
    }
    DialogActivity::OnSelect(theEvent);
  }


  void ColorDialogActivity::OnKeyEvent(const KeyEvent& theEvent)
  {
    if (m_state == State::Ready && !theEvent.IsHandled() && theEvent.IsPressed())
    {
      switch (theEvent.GetKey())
      {
      case VirtualKey::Escape:
        theEvent.Handled();
        DoScheduleClose();
        break;
      default:
        break;
      }
    }
    DialogActivity::OnKeyEvent(theEvent);
  }


  void ColorDialogActivity::DoScheduleClose()
  {
    assert(m_state == State::Ready);
    ScheduleCloseActivity(true);
    m_state = State::Closing;
  }
}
