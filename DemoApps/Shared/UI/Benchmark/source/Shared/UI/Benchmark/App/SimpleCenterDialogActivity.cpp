/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/CheckBox.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/Slider.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <Shared/UI/Benchmark/App/SimpleCenterDialogActivity.hpp>
// #include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/UniformStackLayout.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <Shared/UI/Benchmark/App/LocationTweaker.hpp>
#include <Shared/UI/Benchmark/App/SimpleDialogActivityFactory.hpp>
#include "AppConfig.hpp"

namespace Fsl::UI
{
  SimpleCenterDialogActivity::SimpleCenterDialogActivity(std::weak_ptr<IActivityStack> activityStack,
                                                         const std::shared_ptr<Theme::IThemeControlFactory>& themeControlFactory,
                                                         const Theme::WindowType windowType)
    : DialogActivity(std::move(activityStack), themeControlFactory, std::make_shared<GridLayout>(themeControlFactory->GetContext()), windowType,
                     ItemAlignment::Center)
  {
    SetWindowAlignmentY(LocationTweaker::GetItemAlignment(GetWeakActivityStack()));

    auto& rMainLayout = dynamic_cast<GridLayout&>(GetWindow());
    rMainLayout.AddColumnDefinition(GridColumnDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));

    auto caption = themeControlFactory->CreateLabel("Basic center dialog", Theme::FontType::Header);
    caption->SetAlignmentX(ItemAlignment::Center);

    m_buttonBack = themeControlFactory->CreateTextButton(Theme::ButtonType::Contained, "Back");
    m_buttonBack->SetAlignmentX(ItemAlignment::Stretch);
    m_buttonDetails = themeControlFactory->CreateTextButton(Theme::ButtonType::Outlined, "Details");
    m_buttonDetails->SetAlignmentX(ItemAlignment::Stretch);

    auto context = themeControlFactory->GetContext();
    auto bottomStack = std::make_shared<UniformStackLayout>(context);
    bottomStack->SetAlignmentX(ItemAlignment::Center);
    bottomStack->SetOrientation(LayoutOrientation::Horizontal);
    bottomStack->SetSpacing(DpSize1DF::Create(4.0f));
    if (ActivityCount() < AppConfig::MaxActivityCount)
    {
      bottomStack->AddChild(m_buttonDetails);
    }
    bottomStack->AddChild(m_buttonBack);

    auto content = std::make_shared<GridLayout>(context);
    {
      const auto label0 = themeControlFactory->CreateLabel("Dexterity");
      const auto label1 = themeControlFactory->CreateLabel("Luck");
      const auto label2 = themeControlFactory->CreateLabel("Perception");
      const auto label3 = themeControlFactory->CreateLabel("Strength");
      const auto label4 = themeControlFactory->CreateLabel("Wisdom");
      label0->SetAlignmentY(UI::ItemAlignment::Center);
      label1->SetAlignmentY(UI::ItemAlignment::Center);
      label2->SetAlignmentY(UI::ItemAlignment::Center);
      label3->SetAlignmentY(UI::ItemAlignment::Center);
      label4->SetAlignmentY(UI::ItemAlignment::Center);

      content->SetAlignmentX(ItemAlignment::Stretch);
      content->SetAlignmentY(ItemAlignment::Stretch);
      content->AddColumnDefinition(GridColumnDefinition(GridUnitType::Auto));
      content->AddColumnDefinition(GridColumnDefinition(GridUnitType::Fixed, 400));
      content->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
      content->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
      content->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
      content->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
      content->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
      content->AddChild(label0, 0, 0);
      content->AddChild(label1, 0, 1);
      content->AddChild(label2, 0, 2);
      content->AddChild(label3, 0, 3);
      content->AddChild(label4, 0, 4);
      content->AddChild(themeControlFactory->CreateSliderFmtValue(LayoutOrientation::Horizontal, ConstrainedValue<int32_t>(0, 0, 100)), 1, 0);
      content->AddChild(themeControlFactory->CreateSliderFmtValue(LayoutOrientation::Horizontal, ConstrainedValue<int32_t>(25, 0, 100)), 1, 1);
      content->AddChild(themeControlFactory->CreateSliderFmtValue(LayoutOrientation::Horizontal, ConstrainedValue<int32_t>(50, 0, 100)), 1, 2);
      content->AddChild(themeControlFactory->CreateSliderFmtValue(LayoutOrientation::Horizontal, ConstrainedValue<int32_t>(75, 0, 100)), 1, 3);
      content->AddChild(themeControlFactory->CreateSliderFmtValue(LayoutOrientation::Horizontal, ConstrainedValue<int32_t>(100, 0, 100)), 1, 4);
    }

    rMainLayout.AddChild(caption, 0, 0);
    rMainLayout.AddChild(themeControlFactory->CreateDivider(LayoutOrientation::Horizontal), 0, 1);
    rMainLayout.AddChild(content, 0, 2);
    rMainLayout.AddChild(themeControlFactory->CreateDivider(LayoutOrientation::Horizontal), 0, 3);
    rMainLayout.AddChild(bottomStack, 0, 4);
  }


  void SimpleCenterDialogActivity::OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
  {
    if (m_state == State::Ready && !theEvent->IsHandled())
    {
      if (theEvent->GetSource() == m_buttonBack)
      {
        ScheduleCloseActivity(true);
        m_state = State::Closing;
      }
      else if (theEvent->GetSource() == m_buttonDetails)
      {
        auto controlFactory = GetThemeControlFactory();
        PushActivity(SimpleDialogActivityFactory::CreateRightDialog(GetWeakActivityStack(), controlFactory));
      }
    }

    DialogActivity::OnSelect(args, theEvent);
  }
}
