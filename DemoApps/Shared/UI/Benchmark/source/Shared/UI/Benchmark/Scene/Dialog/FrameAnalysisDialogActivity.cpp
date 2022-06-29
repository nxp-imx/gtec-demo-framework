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
#include <Shared/UI/Benchmark/App/SimpleDialogActivityFactory.hpp>
#include <Shared/UI/Benchmark/Scene/Dialog/FrameAnalysisDialogActivity.hpp>
#include <Shared/UI/Benchmark/TextConfig.hpp>
#include <utility>

namespace Fsl::UI
{
  FrameAnalysisDialogActivity::FrameAnalysisDialogActivity(std::weak_ptr<IActivityStack> activityStack,
                                                           const std::shared_ptr<Theme::IThemeControlFactory>& themeControlFactory,
                                                           const Theme::WindowType windowType, const uint32_t maxDrawCalls)
    : DialogActivity(std::move(activityStack), themeControlFactory, std::make_shared<GridLayout>(themeControlFactory->GetContext()), windowType,
                     ItemAlignment::Far, ItemAlignment::Far)
  {
    auto& rMainLayout = dynamic_cast<GridLayout&>(GetWindow());
    rMainLayout.AddColumnDefinition(GridColumnDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));

    auto caption = themeControlFactory->CreateLabel("Frame analysis", Theme::FontType::Header);
    caption->SetAlignmentX(ItemAlignment::Center);

    m_buttonOK = themeControlFactory->CreateTextButton(Theme::ButtonType::Contained, "Back");
    m_buttonOK->SetAlignmentX(ItemAlignment::Center);

    auto context = themeControlFactory->GetContext();
    auto content = std::make_shared<StackLayout>(context);
    {
      m_drawCallSlider =
        themeControlFactory->CreateSliderFmtValue(LayoutOrientation::Horizontal, ConstrainedValue<uint32_t>(maxDrawCalls, 0, maxDrawCalls));
      m_drawCallSlider->SetWidth(DpLayoutSize1D(DpValue(200)));

      content->SetAlignmentX(ItemAlignment::Stretch);
      content->SetAlignmentY(ItemAlignment::Stretch);
      content->SetOrientation(LayoutOrientation::Vertical);
      content->AddChild(m_drawCallSlider);
    }

    rMainLayout.AddChild(caption, 0, 0);
    rMainLayout.AddChild(themeControlFactory->CreateDivider(LayoutOrientation::Horizontal), 0, 1);
    rMainLayout.AddChild(content, 0, 2);
    rMainLayout.AddChild(themeControlFactory->CreateDivider(LayoutOrientation::Horizontal), 0, 3);
    rMainLayout.AddChild(m_buttonOK, 0, 4);
  }


  void FrameAnalysisDialogActivity::SetMaxDrawCalls(const uint32_t maxDrawCalls)
  {
    m_drawCallSlider->SetRange(0, maxDrawCalls);
  }


  uint32_t FrameAnalysisDialogActivity::GetCurrentDrawCalls() const
  {
    return m_drawCallSlider->GetValue();
  }


  void FrameAnalysisDialogActivity::OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent)
  {
    if (m_state == State::Ready && !theEvent->IsHandled())
    {
      if (theEvent->GetSource() == m_drawCallSlider)
      {
        theEvent->Handled();
      }
    }
    DialogActivity::OnContentChanged(args, theEvent);
  }


  void FrameAnalysisDialogActivity::OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
  {
    if (m_state == State::Ready && !theEvent->IsHandled())
    {
      if (theEvent->GetSource() == m_buttonOK)
      {
        theEvent->Handled();
        DoScheduleClose();
      }
    }
    DialogActivity::OnSelect(args, theEvent);
  }


  void FrameAnalysisDialogActivity::OnKeyEvent(const KeyEvent& theEvent)
  {
    if (m_state == State::Ready && !theEvent.IsHandled() && theEvent.IsPressed())
    {
      switch (theEvent.GetKey())
      {
      case VirtualKey::Escape:
        theEvent.Handled();
        DoScheduleClose();
        break;
      case VirtualKey::LeftArrow:
        theEvent.Handled();
        m_drawCallSlider->SubValue(1);
        break;
      case VirtualKey::RightArrow:
        theEvent.Handled();
        m_drawCallSlider->AddValue(1);
        break;
      default:
        break;
      }
    }
    DialogActivity::OnKeyEvent(theEvent);
  }


  void FrameAnalysisDialogActivity::DoScheduleClose()
  {
    assert(m_state == State::Ready);
    ScheduleCloseActivity(true);
    m_state = State::Closing;
  }
}
