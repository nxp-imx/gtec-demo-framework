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

#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/KeyEvent.hpp>
#include <FslSimpleUI/Base/Control/BackgroundLabelButton.hpp>
#include <FslSimpleUI/Base/Control/CheckBox.hpp>
#include <FslSimpleUI/Base/Control/Image.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Control/RadioButton.hpp>
#include <FslSimpleUI/Base/Control/Slider.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/UniformStackLayout.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <Shared/UI/Benchmark/App/SimpleDialogActivityFactory.hpp>
#include <Shared/UI/Benchmark/Scene/Dialog/BenchConfigDialogActivity.hpp>
#include <Shared/UI/Benchmark/TextConfig.hpp>
#include "../Control/RenderOptionControlsFactory.hpp"

namespace Fsl::UI
{
  namespace
  {
    uint32_t LocateActiveRenderIndex(const ReadOnlySpan<RenderMethodInfo> renderMethods, const AppBenchSettings settings)
    {
      for (std::size_t i = 0; i < renderMethods.size(); ++i)
      {
        if (renderMethods[i].RenderMethod == settings.RenderMethod)
        {
          return NumericCast<uint32_t>(i);
        }
      }
      FSLLOG3_DEBUG_INFO("Could not locate the stored render method, using zero");
      return 0;
    }
  }

  void BenchConfigDialogActivity::RenderMethodUI::FinishAnimation()
  {
    for (auto& rEntry : Methods)
    {
      if (rEntry.RadioButton)
      {
        rEntry.RadioButton->FinishAnimation();
      }
    }
  }


  void BenchConfigDialogActivity::MainUI::FinishAnimation()
  {
    if (SwitchGpuTimestamps)
    {
      SwitchGpuTimestamps->FinishAnimation();
    }
    if (SwitchNoOpaqueMaterials)
    {
      SwitchNoOpaqueMaterials->FinishAnimation();
    }
    if (SwitchUseSdfFonts)
    {
      SwitchUseSdfFonts->FinishAnimation();
    }
    RenderMethod.FinishAnimation();
    RenderOptions.FinishAnimation();
  }


  BenchConfigDialogActivity::BenchConfigDialogActivity(std::weak_ptr<IActivityStack> activityStack,
                                                       const std::shared_ptr<Theme::IThemeControlFactory>& themeControlFactory,
                                                       const bool gpuTimestampsSupported, const ReadOnlySpan<RenderMethodInfo> renderMethods,
                                                       std::shared_ptr<AppBenchSettings> settings)
    : DialogActivity(std::move(activityStack), themeControlFactory, std::make_shared<GridLayout>(themeControlFactory->GetContext()),
                     Theme::WindowType::DialogNormal, ItemAlignment::Center, ItemAlignment::Center)
    , m_renderMethods(ReadOnlySpanUtil::ToVector(renderMethods))
    , m_activeRenderIndex(LocateActiveRenderIndex(renderMethods, *settings))
    , m_settings(std::move(settings))
  {
    if (!m_settings)
    {
      throw std::invalid_argument("settings can not be null");
    }
    if (m_settings->RenderOptions.size() != renderMethods.size())
    {
      throw std::invalid_argument("settings render options does not match the renderMethod size");
    }
    // If timestamps not supported, we disable the switch and force it to false
    if (!gpuTimestampsSupported)
    {
      m_settings->BasicOptions.GpuTimestamps = false;
    }


    m_mainUI = CreateMainUI(*themeControlFactory, dynamic_cast<GridLayout&>(GetWindow()), gpuTimestampsSupported, renderMethods, m_activeRenderIndex,
                            *m_settings);
    m_mainUI.RenderOptions.RestoreUISettings(m_renderMethods[m_activeRenderIndex].SystemInfo);
    SetSettings(*m_settings, m_activeRenderIndex);

    // Force finish the animations
    m_mainUI.FinishAnimation();
  }


  void BenchConfigDialogActivity::OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent)
  {
    if (m_state == State::Ready && !theEvent->IsHandled())
    {
      {
        const auto count = NumericCast<uint32_t>(m_mainUI.RenderMethod.Methods.size());
        for (uint32_t i = 0; !theEvent->IsHandled() && i < count; ++i)
        {
          if (theEvent->GetSource() == m_mainUI.RenderMethod.Methods[i].RadioButton)
          {
            theEvent->Handled();
            if (m_activeRenderIndex != i)
            {
              // Store the current settings for the render (so its the same when we switch back)
              m_mainUI.RenderOptions.StoreUISettings(m_renderMethods[m_activeRenderIndex].SystemInfo);
              m_activeRenderIndex = i;
              // Restore the settings of the just selected UI
              m_mainUI.RenderOptions.RestoreUISettings(m_renderMethods[m_activeRenderIndex].SystemInfo);
            }
          }
        }
      }
    }
    DialogActivity::OnContentChanged(args, theEvent);
  }


  void BenchConfigDialogActivity::OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
  {
    if (m_state == State::Ready && !theEvent->IsHandled())
    {
      if (theEvent->GetSource() == m_mainUI.ButtonOK)
      {
        theEvent->Handled();
        DoScheduleClose(true);
      }
      else if (theEvent->GetSource() == m_mainUI.ButtonBack)
      {
        theEvent->Handled();
        DoScheduleClose(false);
      }
    }
    DialogActivity::OnSelect(args, theEvent);
  }


  void BenchConfigDialogActivity::OnKeyEvent(const KeyEvent& theEvent)
  {
    if (m_state == State::Ready && !theEvent.IsHandled())
    {
      switch (theEvent.GetKey())
      {
      case VirtualKey::Escape:
        theEvent.Handled();
        DoScheduleClose(false);
        break;
      case VirtualKey::Return:
        theEvent.Handled();
        DoScheduleClose(true);
        break;
      default:
        break;
      }
    }
    DialogActivity::OnKeyEvent(theEvent);
  }


  BenchConfigDialogActivity::MainUI BenchConfigDialogActivity::CreateMainUI(Theme::IThemeControlFactory& uiFactory, GridLayout& rMainLayout,
                                                                            const bool gpuTimestampsSupported,
                                                                            const ReadOnlySpan<RenderMethodInfo> renderMethods,
                                                                            const uint32_t activeRenderIndex, const AppBenchSettings& settings)
  {
    auto context = uiFactory.GetContext();

    rMainLayout.AddColumnDefinition(GridColumnDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));
    rMainLayout.AddRowDefinition(GridRowDefinition(GridUnitType::Auto));

    auto caption = uiFactory.CreateLabel("Bench settings", Theme::FontType::Header);
    caption->SetAlignmentX(ItemAlignment::Center);


    auto buttonOK = uiFactory.CreateTextButton(Theme::ButtonType::Contained, "Run");
    buttonOK->SetAlignmentX(ItemAlignment::Stretch);
    auto buttonBack = uiFactory.CreateTextButton(Theme::ButtonType::Outlined, "Back");
    buttonBack->SetAlignmentX(ItemAlignment::Stretch);

    auto buttonStack = std::make_shared<UI::UniformStackLayout>(context);
    buttonStack->SetOrientation(UI::LayoutOrientation::Horizontal);
    buttonStack->SetAlignmentX(UI::ItemAlignment::Center);
    buttonStack->AddChild(buttonOK);
    buttonStack->AddChild(buttonBack);

    std::shared_ptr<Switch> switchGpuTimestamps;
    std::shared_ptr<Switch> switchNoOpaqueMaterials;
    std::shared_ptr<Switch> switchDrawCache;
    std::shared_ptr<Switch> switchOnDemandRendering;
    std::shared_ptr<Switch> switchUseSdfFonts;
    auto content0 = std::make_shared<StackLayout>(context);
    {
      switchGpuTimestamps = uiFactory.CreateSwitch(TextConfig::GpuTimestamps, settings.BasicOptions.GpuTimestamps);
      switchNoOpaqueMaterials = uiFactory.CreateSwitch(TextConfig::NoOpaqueMaterials, settings.BasicOptions.NoOpaqueMaterials);
      switchDrawCache = uiFactory.CreateSwitch(TextConfig::UseDrawCache, settings.BasicOptions.UseDrawCache);
      switchOnDemandRendering = uiFactory.CreateSwitch(TextConfig::OnDemandRendering, settings.BasicOptions.UseOnDemandRendering);
      switchUseSdfFonts = uiFactory.CreateSwitch(TextConfig::UseSdfFonts, settings.BasicOptions.UseSdfFonts);

      if (!gpuTimestampsSupported)
      {
        assert(!settings.BasicOptions.GpuTimestamps);
        switchGpuTimestamps->SetEnabled(false);
        switchGpuTimestamps->SetIsChecked(false);
      }

      content0->SetAlignmentX(ItemAlignment::Stretch);
      content0->SetAlignmentY(ItemAlignment::Stretch);
      content0->SetOrientation(LayoutOrientation::Vertical);
      content0->AddChild(uiFactory.CreateLabel(TextConfig::HeaderOptions, Theme::FontType::Header));
      content0->AddChild(switchGpuTimestamps);
      content0->AddChild(switchNoOpaqueMaterials);
      content0->AddChild(switchDrawCache);
      content0->AddChild(switchOnDemandRendering);
      content0->AddChild(switchUseSdfFonts);
    }

    RenderMethodUI renderMethod(renderMethods.size());
    auto content1 = std::make_shared<StackLayout>(context);
    {
      content1->SetAlignmentX(ItemAlignment::Stretch);
      content1->SetAlignmentY(ItemAlignment::Stretch);
      content1->SetOrientation(LayoutOrientation::Vertical);
      content1->AddChild(uiFactory.CreateLabel(TextConfig::HeaderRenderMethod, Theme::FontType::Header));
      auto renderMethodGroup = uiFactory.CreateRadioGroup("RenderMethod");
      for (std::size_t i = 0; i < renderMethods.size(); ++i)
      {
        renderMethod.Methods[i] = RenderMethodRecord(renderMethods[i].RenderMethod,
                                                     uiFactory.CreateRadioButton(renderMethodGroup, renderMethods[i].Name, i == activeRenderIndex));
        content1->AddChild(renderMethod.Methods[i].RadioButton);
      }
    }

    RenderOptionControls renderOptions = RenderOptionControlsFactory::CreateRenderMethodControls(uiFactory);
    auto content2 = std::make_shared<StackLayout>(context);
    {
      content2->SetAlignmentX(ItemAlignment::Stretch);
      content2->SetAlignmentY(ItemAlignment::Stretch);
      content2->SetOrientation(LayoutOrientation::Vertical);
      content2->AddChild(uiFactory.CreateLabel(TextConfig::HeaderRenderOptions, Theme::FontType::Header));
      content2->AddChild(renderOptions.SwitchFillBuffers);
      content2->AddChild(renderOptions.SwitchBatch);
      content2->AddChild(renderOptions.SwitchDrawReorder);
      content2->AddChild(renderOptions.SwitchPreferFastReorder);
      content2->AddChild(renderOptions.SwitchDepthBuffer);
      content2->AddChild(renderOptions.SwitchMeshCaching);
    }

    auto sharedSizeGroup = std::make_shared<UI::LayoutSharedSizeGroup>();
    auto contentStack = std::make_shared<ComplexStackLayout>(context);
    contentStack->SetOrientation(LayoutOrientation::Horizontal);
    contentStack->SetSpacing(DpSize1DF::Create(8));
    contentStack->AddChild(content0, UI::LayoutLength(UI::LayoutUnitType::Auto, sharedSizeGroup));
    contentStack->AddChild(uiFactory.CreateDivider(LayoutOrientation::Vertical), {});
    contentStack->AddChild(content1, UI::LayoutLength(UI::LayoutUnitType::Auto, sharedSizeGroup));
    contentStack->AddChild(uiFactory.CreateDivider(LayoutOrientation::Vertical), {});
    contentStack->AddChild(content2, UI::LayoutLength(UI::LayoutUnitType::Auto, sharedSizeGroup));

    rMainLayout.AddChild(caption, 0, 0);
    rMainLayout.AddChild(uiFactory.CreateDivider(LayoutOrientation::Horizontal), 0, 1);
    rMainLayout.AddChild(contentStack, 0, 2);
    rMainLayout.AddChild(uiFactory.CreateDivider(LayoutOrientation::Horizontal), 0, 3);
    rMainLayout.AddChild(buttonStack, 0, 4);
    return {buttonOK,          buttonBack,   switchGpuTimestamps, switchNoOpaqueMaterials, switchDrawCache, switchOnDemandRendering,
            switchUseSdfFonts, renderMethod, renderOptions};
  }

  void BenchConfigDialogActivity::DoScheduleClose(const bool completed)
  {
    assert(m_state == State::Ready);
    ScheduleCloseActivity(completed);
    m_state = State::Closing;
    *m_settings = RetrieveSettings();
  }


  void BenchConfigDialogActivity::SetSettings(const AppBenchSettings& settings, const uint32_t activeRenderIndex)
  {
    m_mainUI.SwitchGpuTimestamps->SetIsChecked(settings.BasicOptions.GpuTimestamps);
    m_mainUI.SwitchNoOpaqueMaterials->SetIsChecked(settings.BasicOptions.NoOpaqueMaterials);
    m_mainUI.SwitchUseSdfFonts->SetIsChecked(settings.BasicOptions.UseSdfFonts);

    assert(activeRenderIndex < settings.RenderOptions.size());

    const auto& renderOptions = settings.RenderOptions[activeRenderIndex];
    auto& rSystemInfo = m_renderMethods[activeRenderIndex].SystemInfo;

    // Render options
    rSystemInfo.Set(RenderOptionFlags::Batch, renderOptions.Batch);
    rSystemInfo.Set(RenderOptionFlags::FillBuffers, renderOptions.FillBuffers);
    rSystemInfo.Set(RenderOptionFlags::DepthBuffer, renderOptions.DepthBuffer);
    rSystemInfo.Set(RenderOptionFlags::DrawReorder, renderOptions.DrawReorder);
    rSystemInfo.Set(RenderOptionFlags::MeshCaching, renderOptions.MeshCaching);
    rSystemInfo.Set(RenderOptionFlags::PreferFastReorder, renderOptions.PreferFastReorder);

    m_mainUI.RenderOptions.RestoreUISettings(m_renderMethods[m_activeRenderIndex].SystemInfo);
  }


  AppBenchSettings BenchConfigDialogActivity::RetrieveSettings()
  {
    // - Extract the current settings
    m_mainUI.RenderOptions.StoreUISettings(m_renderMethods[m_activeRenderIndex].SystemInfo);

    std::array<AppRenderOptions, 2> renderOptions;
    if (m_renderMethods.size() != renderOptions.size())
    {
      throw InternalErrorException("render methods does not match render options");
    }

    // Run through all the stored settings and convert them to AppRenderOptions.
    for (std::size_t i = 0; i < m_renderMethods.size(); ++i)
    {
      renderOptions[i] = AppRenderOptions(
        m_renderMethods[i].RenderMethod, UI::RenderOptionFlagsUtil::IsEnabled(m_renderMethods[i].SystemInfo.Settings, RenderOptionFlags::Batch),
        UI::RenderOptionFlagsUtil::IsEnabled(m_renderMethods[i].SystemInfo.Settings, RenderOptionFlags::FillBuffers),
        UI::RenderOptionFlagsUtil::IsEnabled(m_renderMethods[i].SystemInfo.Settings, RenderOptionFlags::DepthBuffer),
        UI::RenderOptionFlagsUtil::IsEnabled(m_renderMethods[i].SystemInfo.Settings, RenderOptionFlags::DrawReorder),
        UI::RenderOptionFlagsUtil::IsEnabled(m_renderMethods[i].SystemInfo.Settings, RenderOptionFlags::MeshCaching),
        UI::RenderOptionFlagsUtil::IsEnabled(m_renderMethods[i].SystemInfo.Settings, RenderOptionFlags::PreferFastReorder));
    }

    auto renderMethod = AppRenderMethod::FlexImBatch;
    for (const auto& entry : m_mainUI.RenderMethod.Methods)
    {
      if (entry.RadioButton->IsChecked())
      {
        renderMethod = entry.RenderMethod;
        break;
      }
    }

    return {AppRenderBasicOptions(m_mainUI.SwitchGpuTimestamps->IsChecked(), m_mainUI.SwitchNoOpaqueMaterials->IsChecked(),
                                  m_mainUI.switchDrawCache->IsChecked(), m_mainUI.switchOnDemandRendering->IsChecked(),
                                  m_mainUI.SwitchUseSdfFonts->IsChecked()),
            renderMethod, renderOptions};
  }
}
