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
#include <Shared/UI/Benchmark/Persistence/Debug/DebugAppRenderMethod.hpp>
#include <Shared/UI/Benchmark/Scene/Dialog/ResultDetailsDialogActivity.hpp>
#include <Shared/UI/Benchmark/TextConfig.hpp>
#include <utility>
#include "../Control/RenderOptionControlsFactory.hpp"

namespace Fsl::UI
{
  namespace
  {
    std::shared_ptr<Switch> CreateSwitch(Theme::IThemeControlFactory& uiFactory, StringViewLite strView, const bool checked)
    {
      auto ctrl = uiFactory.CreateSwitch(strView, checked);
      ctrl->SetEnabled(false);
      ctrl->FinishAnimation();
      return ctrl;
    }

    void PopulateGrid(GridLayout& rGrid, Theme::IThemeControlFactory& uiFactory, const uint32_t columnIndex, const uint32_t rowIndex,
                      std::optional<AppBenchmarkData> result)
    {
      if (result.has_value() && result.value().RenderInfo.has_value())
      {
        const auto resultValue = result.value().RenderInfo.value();
        auto settings0 = uiFactory.CreateLabel(TextConfig::HeaderSettings, Theme::FontType::Header);
        auto settings1 = CreateSwitch(uiFactory, TextConfig::GpuTimestamps, resultValue.BasicOptions.GpuTimestamps);
        auto settings2 = CreateSwitch(uiFactory, TextConfig::NoOpaqueMaterials, resultValue.BasicOptions.NoOpaqueMaterials);
        auto settings3 = CreateSwitch(uiFactory, TextConfig::OnDemandRendering, resultValue.BasicOptions.UseOnDemandRendering);
        auto settings4 = CreateSwitch(uiFactory, TextConfig::UseSdfFonts, resultValue.BasicOptions.UseSdfFonts);

        auto renderMethod0 = uiFactory.CreateLabel(TextConfig::HeaderRenderMethod, Theme::FontType::Header);
        auto renderMethod1 = uiFactory.CreateLabel(Debug::ToString(resultValue.RenderOptions.RenderMethod), Theme::FontType::Default);
        renderMethod1->SetEnabled(false);
        renderMethod1->FinishAnimation();
        auto renderOptions0 = uiFactory.CreateLabel(TextConfig::HeaderRenderOptions, Theme::FontType::Header);

        RenderOptionControls renderOptions = RenderOptionControlsFactory::CreateRenderMethodControls(uiFactory);

        renderOptions.SwitchFillBuffers->SetIsChecked(resultValue.RenderOptions.FillBuffers);
        renderOptions.SwitchBatch->SetIsChecked(resultValue.RenderOptions.Batch);
        renderOptions.SwitchDrawReorder->SetIsChecked(resultValue.RenderOptions.DrawReorder);
        renderOptions.SwitchPreferFastReorder->SetIsChecked(resultValue.RenderOptions.PreferFastReorder);
        renderOptions.SwitchDepthBuffer->SetIsChecked(resultValue.RenderOptions.DepthBuffer);
        renderOptions.SwitchMeshCaching->SetIsChecked(resultValue.RenderOptions.MeshCaching);

        renderOptions.SetEnabled(false);
        renderOptions.FinishAnimation();

        rGrid.AddChild(settings0, columnIndex, rowIndex);
        rGrid.AddChild(settings1, columnIndex, rowIndex + 1);
        rGrid.AddChild(settings2, columnIndex, rowIndex + 2);
        rGrid.AddChild(settings3, columnIndex, rowIndex + 3);
        rGrid.AddChild(settings4, columnIndex, rowIndex + 4);
        // skip 5
        rGrid.AddChild(renderMethod0, columnIndex, rowIndex + 6);
        rGrid.AddChild(renderMethod1, columnIndex, rowIndex + 7);
        // skip 8
        rGrid.AddChild(renderOptions0, columnIndex, rowIndex + 9);
        rGrid.AddChild(renderOptions.SwitchFillBuffers, columnIndex, rowIndex + 11);
        rGrid.AddChild(renderOptions.SwitchBatch, columnIndex, rowIndex + 12);
        rGrid.AddChild(renderOptions.SwitchDrawReorder, columnIndex, rowIndex + 13);
        rGrid.AddChild(renderOptions.SwitchPreferFastReorder, columnIndex, rowIndex + 14);
        rGrid.AddChild(renderOptions.SwitchDepthBuffer, columnIndex, rowIndex + 15);
        rGrid.AddChild(renderOptions.SwitchMeshCaching, columnIndex, rowIndex + 16);
      }
      else
      {
        rGrid.AddChild(uiFactory.CreateLabel("Not available", Theme::FontType::Header), columnIndex, rowIndex);
      }
    }
  }

  ResultDetailsDialogActivity::ResultDetailsDialogActivity(std::weak_ptr<IActivityStack> activityStack,
                                                           const std::shared_ptr<Theme::IThemeControlFactory>& themeControlFactory,
                                                           std::optional<AppBenchmarkData> benchNewResult,
                                                           std::optional<AppBenchmarkData> benchOldResult)
    : DialogActivity(std::move(activityStack), themeControlFactory, std::make_shared<GridLayout>(themeControlFactory->GetContext()),
                     Theme::WindowType::DialogNormal, ItemAlignment::Center, ItemAlignment::Center)
  {
    auto context = themeControlFactory->GetContext();
    auto& uiFactory = *themeControlFactory;

    auto contentLayout = std::make_shared<GridLayout>(context);
    contentLayout->AddColumnDefinition(GridColumnDefinition(GridUnitType::Auto));
    contentLayout->AddColumnDefinition(GridColumnDefinition(GridUnitType::Fixed, 20));
    contentLayout->AddColumnDefinition(GridColumnDefinition(GridUnitType::Auto));
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));         // 0
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));         // 1
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));         // 2
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));         // 3
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));         // 4
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Fixed, 16));    // 5
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));         // 6
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));         // 7
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Fixed, 16));    // 8
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));         // 9
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));         // 10
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));         // 11
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));         // 12
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));         // 13
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));         // 14
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));         // 15
    contentLayout->AddRowDefinition(GridRowDefinition(GridUnitType::Auto));         // 16

    PopulateGrid(*contentLayout, uiFactory, 0, 0, std::move(benchNewResult));
    PopulateGrid(*contentLayout, uiFactory, 2, 0, std::move(benchOldResult));

    auto& rMainLayout = dynamic_cast<GridLayout&>(GetWindow());
    rMainLayout.AddColumnDefinition(GridColumnDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));

    auto caption = themeControlFactory->CreateLabel("Details", Theme::FontType::Header);
    caption->SetAlignmentX(ItemAlignment::Center);

    m_buttonBack = themeControlFactory->CreateTextButton(Theme::ButtonType::Contained, "Back");
    m_buttonBack->SetAlignmentX(ItemAlignment::Center);

    rMainLayout.AddChild(caption, 0, 0);
    rMainLayout.AddChild(themeControlFactory->CreateDivider(LayoutOrientation::Horizontal), 0, 1);
    rMainLayout.AddChild(contentLayout, 0, 2);
    rMainLayout.AddChild(themeControlFactory->CreateDivider(LayoutOrientation::Horizontal), 0, 3);
    rMainLayout.AddChild(m_buttonBack, 0, 4);
  }


  void ResultDetailsDialogActivity::OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
  {
    if (m_state == State::Ready && !theEvent->IsHandled())
    {
      if (theEvent->GetSource() == m_buttonBack)
      {
        theEvent->Handled();
        DoScheduleClose();
      }
    }
    DialogActivity::OnSelect(args, theEvent);
  }


  void ResultDetailsDialogActivity::OnKeyEvent(const KeyEvent& theEvent)
  {
    if (m_state == State::Ready && !theEvent.IsHandled())
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


  void ResultDetailsDialogActivity::DoScheduleClose()
  {
    assert(m_state == State::Ready);
    ScheduleCloseActivity(true);
    m_state = State::Closing;
  }

}
