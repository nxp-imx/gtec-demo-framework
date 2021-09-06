/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <Shared/HDR/SkyboxTonemapping/MenuUI.hpp>
#include <FslBase/Bits/BitsUtil.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Layout/ComplexStackLayout.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr ConstrainedValue<float> Exposure(1.0f, 0.0f, 10.0f);
    }

    std::shared_ptr<UI::Switch> TryFindSourceCheckbox(const std::vector<std::shared_ptr<UI::Switch>>& checkboxes,
                                                      const std::shared_ptr<UI::IWindowId>& source)
    {
      for (auto checkbox : checkboxes)
      {
        if (checkbox == source)
        {
          return checkbox;
        }
      }
      return std::shared_ptr<UI::Switch>();
    }
  }

  MenuUI::MenuUI(const DemoAppConfig& config, const std::size_t entries)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))
    , m_nativeBatch(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D())
    , m_windowMetrics(config.WindowMetrics)

  {
    const auto options = config.GetOptions<OptionParser>();

    m_rootCanvas = CreateUI();

    PrepareTransition(m_render, entries);

    m_configWindow = CreateConfigDialog(m_render);
    m_rootCanvas->AddChild(m_configWindow);
    UpdateUIState();

    m_defaultFont = m_uiExtension->GetContext()->DefaultFont;

    SetStateViaFlags(options->GetSceneMask());
    ForceCompleteTransitions(m_render);
  }


  void MenuUI::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }
    switch (event.GetKey())
    {
    case VirtualKey::Code1:
      ToggleState(SceneFlags::Scene1);
      break;
    case VirtualKey::Code2:
      ToggleState(SceneFlags::Scene2);
      break;
    case VirtualKey::Code3:
      ToggleState(SceneFlags::Scene3);
      break;
    case VirtualKey::Code4:
      ToggleState(SceneFlags::Scene4);
      break;
    case VirtualKey::Code5:
      ToggleState(SceneFlags::Scene5);
      break;
    case VirtualKey::Code6:
      SetState(SceneFlags::Split, true);
      break;
    default:
      break;
    }
  }


  void MenuUI::OnContentChanged(const UI::RoutedEventArgs& /*args*/, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    if (theEvent->IsHandled())
    {
      return;
    }

    auto source = theEvent->GetSource();

    const auto checkbox = TryFindSourceCheckbox(m_checkboxes, source);
    if (checkbox)
    {
      UpdateStateBasedOnCheckboxes(checkbox);
    }
  }


  void MenuUI::Update(const DemoTime& demoTime)
  {
    UpdateSceneTransition(demoTime);
  }


  void MenuUI::Draw(const DemoTime& /*demoTime*/)
  {
    DrawLabels();

    // Calling this last allows the UI to draw on top of everything.
    // Beware that the UI drawing methods might alter the OpenGL state!
    m_uiExtension->Draw();
  }


  void MenuUI::UpdateExposureInput(const DemoTime& demoTime, const KeyboardState& keyboardState)
  {
    const float exposureAdd = 1.0f * demoTime.DeltaTime;
    if (keyboardState.IsKeyDown(VirtualKey::Q))
    {
      m_exposureSlider->SetValue(m_exposureSlider->GetValue() - exposureAdd);
    }
    else if (keyboardState.IsKeyDown(VirtualKey::E))
    {
      m_exposureSlider->SetValue(m_exposureSlider->GetValue() + exposureAdd);
    }
    if (keyboardState.IsKeyDown(VirtualKey::R))
    {
      m_exposureSlider->SetValue(LocalConfig::Exposure.Get());
    }
  }


  float MenuUI::GetExposure() const
  {
    return m_exposureSlider->GetValue();
  }


  void MenuUI::UpdateStateBasedOnCheckboxes(const std::shared_ptr<UI::Switch>& source)
  {
    if (!source)
    {
      return;
    }

    uint32_t flag = 1;
    uint32_t enabledFlags = 0;
    for (std::size_t i = 0; i < m_checkboxes.size(); ++i)
    {
      if (m_checkboxes[i]->IsChecked())
      {
        enabledFlags |= flag;
      }
      flag <<= 1;
    }
    // Cant disable all
    if (enabledFlags <= 0)
    {
      source->SetIsChecked(true);
      return;
    }

    SetStateViaFlags(enabledFlags);
  }


  void MenuUI::DrawLabels()
  {
    const auto* pFont = m_defaultFont.get();

    m_nativeBatch->Begin(BlendState::AlphaBlend);
    int32_t startX = 0;
    for (std::size_t i = 0; i < m_render.size(); ++i)
    {
      const auto endX = static_cast<int32_t>(m_render[i].SplitX.GetValue());
      const auto color = m_render[i].LabelAlpha.GetValue();
      if (color >= 0.001f && pFont != nullptr)
      {
        const Color fontColor(color, color, color, color);
        const auto dim = pFont->MeasureString(m_render[i].Name.c_str());
        const float position = ((static_cast<float>(endX - startX - dim.Width()) / 2.0f) + static_cast<float>(startX));
        m_nativeBatch->DrawString(*pFont, m_render[i].Name, Vector2(position, 0), fontColor);
      }
      startX = endX;
    }
    m_nativeBatch->End();
  }


  void MenuUI::ToggleState(const SceneFlags newState)
  {
    const bool currentState = (m_sceneRenderFlags & static_cast<uint32_t>(newState)) != 0;
    SetState(newState, !currentState);
  }


  void MenuUI::SetState(const SceneFlags newState, const bool enabled)
  {
    const auto newStateBits = static_cast<uint32_t>(newState);
    const bool currentState = (m_sceneRenderFlags & newStateBits) == newStateBits;
    if (currentState == enabled || (!enabled && BitsUtil::Count(m_sceneRenderFlags) == 1))
    {
      return;
    }

    uint32_t sceneRenderFlags = m_sceneRenderFlags;
    if (enabled)
    {
      sceneRenderFlags |= newStateBits;
    }
    else
    {
      sceneRenderFlags &= ~newStateBits;
    }

    SetStateViaFlags(sceneRenderFlags);
  }


  void MenuUI::SetStateViaFlags(const uint32_t flags, const bool forceInstant)
  {
    if (flags == m_sceneRenderFlags && !forceInstant)
    {
      return;
    }
    m_sceneRenderFlags = flags;

    const auto extent = m_windowMetrics.ExtentPx;
    const auto activeSegments = BitsUtil::Count(m_sceneRenderFlags);

    if (activeSegments > 0)
    {
      const auto segmentSize = static_cast<float>(extent.Width) / activeSegments;
      uint32_t sceneFlags = m_sceneRenderFlags;
      float currentSplitX = segmentSize;
      for (std::size_t i = 0; i < m_render.size(); ++i)
      {
        if ((sceneFlags & 1) == 0)
        {
          m_render[i].LabelAlpha.SetValue(0.0f);
          m_render[i].SplitX.SetValue(currentSplitX - segmentSize);
        }
        else
        {
          m_render[i].LabelAlpha.SetValue(1.0f);
          m_render[i].SplitX.SetValue(currentSplitX);
          currentSplitX += segmentSize;
        }
        sceneFlags >>= 1;
      }
    }
    else
    {
      for (std::size_t i = 0; i < m_render.size(); ++i)
      {
        m_render[i].LabelAlpha.SetValue(0);
        m_render[i].SplitX.SetValue(0);
      }
    }

    if (forceInstant)
    {
      for (std::size_t i = 0; i < m_render.size(); ++i)
      {
        m_render[i].LabelAlpha.ForceComplete();
        m_render[i].SplitX.ForceComplete();
      }
    }

    UpdateUIState();
  }


  void MenuUI::UpdateUIState()
  {
    assert(m_render.size() == m_checkboxes.size());

    uint32_t sceneFlags = m_sceneRenderFlags;
    for (std::size_t i = 0; i < m_render.size(); ++i)
    {
      const bool enabled = (sceneFlags & 1) != 0;
      m_checkboxes[i]->SetIsChecked(enabled);
      sceneFlags >>= 1;
    }
  }


  void MenuUI::UpdateSceneTransition(const DemoTime& demoTime)
  {
    const auto timespan = TransitionTimeSpan(demoTime.DeltaTimeInMicroseconds, TransitionTimeUnit::Microseconds);
    for (auto& rRecord : m_render)
    {
      rRecord.SplitX.Update(timespan);
      rRecord.LabelAlpha.Update(timespan);
    }
  }


  void MenuUI::PrepareTransition(std::vector<RenderRecord>& rRender, const std::size_t entries)
  {
    rRender.resize(entries);
    for (auto& rRecord : rRender)
    {
      rRecord.LabelAlpha.SetTransitionTime(m_transitionCache, TransitionTimeSpan(200, TransitionTimeUnit::Milliseconds), TransitionType::Smooth);
      rRecord.SplitX.SetTransitionTime(m_transitionCache, TransitionTimeSpan(400, TransitionTimeUnit::Milliseconds), TransitionType::Smooth);
    }
    rRender[Tonemapper::LinearNoGamma].Name = "LinearNoGamma";
    rRender[Tonemapper::Linear].Name = "Linear";
    rRender[Tonemapper::ReinhardLum].Name = "ReinhardLum";
    rRender[Tonemapper::Uncharted2Lum].Name = "Uncharted2Lum";
    rRender[Tonemapper::Custom].Name = "Custom";
  }


  void MenuUI::ForceCompleteTransitions(std::vector<RenderRecord>& rRender)
  {
    for (auto& rRecord : rRender)
    {
      rRecord.LabelAlpha.ForceComplete();
      rRecord.SplitX.ForceComplete();
    }
  }


  std::shared_ptr<UI::CanvasLayout> MenuUI::CreateUI()
  {
    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();

    // Create a 'root' layout we use the recommended fill layout as it will utilize all available space on the screen
    // We then add the 'player' stack to it and the label
    auto rootLayout = std::make_shared<UI::CanvasLayout>(context);
    rootLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    rootLayout->SetAlignmentY(UI::ItemAlignment::Stretch);

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(rootLayout);
    return rootLayout;
  }


  std::shared_ptr<UI::BaseWindow> MenuUI::CreateConfigDialog(const std::vector<RenderRecord>& render)
  {
    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();

    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
    auto& factory = *uiControlFactory;


    m_checkboxes.resize(render.size());
    for (std::size_t i = 0; i < m_checkboxes.size(); ++i)
    {
      m_checkboxes[i] = factory.CreateSwitch(render[i].Name);
      m_checkboxes[i]->SetAlignmentX(UI::ItemAlignment::Center);
    }

    auto labelExposure = factory.CreateLabel("Exposure");
    labelExposure->SetAlignmentX(UI::ItemAlignment::Near);
    labelExposure->SetAlignmentY(UI::ItemAlignment::Center);

    m_exposureSlider = factory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, LocalConfig::Exposure);
    m_exposureSlider->SetAlignmentX(UI::ItemAlignment::Stretch);


    auto stackLayout = std::make_shared<UI::ComplexStackLayout>(context);
    stackLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    stackLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    stackLayout->SetLayoutOrientation(UI::LayoutOrientation::Horizontal);
    for (std::size_t i = 0; i < m_checkboxes.size(); ++i)
    {
      stackLayout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
      stackLayout->AddChild(m_checkboxes[i]);
    }

    auto stackLayout2 = std::make_shared<UI::ComplexStackLayout>(context);
    stackLayout2->SetAlignmentX(UI::ItemAlignment::Stretch);
    stackLayout2->SetAlignmentY(UI::ItemAlignment::Stretch);
    stackLayout2->SetLayoutOrientation(UI::LayoutOrientation::Horizontal);
    stackLayout2->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Auto));
    stackLayout2->AddChild(labelExposure);
    stackLayout2->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    stackLayout2->AddChild(m_exposureSlider);

    auto mainStack = std::make_shared<UI::StackLayout>(context);
    mainStack->SetAlignmentX(UI::ItemAlignment::Stretch);
    mainStack->SetAlignmentY(UI::ItemAlignment::Stretch);
    mainStack->SetLayoutOrientation(UI::LayoutOrientation::Vertical);
    mainStack->AddChild(stackLayout);
    mainStack->AddChild(stackLayout2);

    auto bottomBar = factory.CreateBottomBar();
    bottomBar->SetContent(mainStack);
    return bottomBar;
  }
}
