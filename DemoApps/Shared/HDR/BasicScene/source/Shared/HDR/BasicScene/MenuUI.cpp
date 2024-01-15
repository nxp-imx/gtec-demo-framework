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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <Shared/HDR/BasicScene/MenuUI.hpp>
#include <Shared/HDR/BasicScene/OptionParser.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr ConstrainedValue<float> Exposure(1.0f, 0.0f, 10.0f);
    }
  }


  MenuUI::MenuUI(const DemoAppConfig& config, const ColorSpace uiColorSpace)
    : m_uiColorSpace(uiColorSpace)
    , m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi",
                                                         UIDemoAppExtension::CreateConfig(m_uiColorSpace)))
    , m_state(SceneState::Invalid)
    , m_scene1LabelAlpha(m_transitionCache, TimeSpan::FromMilliseconds(200), TransitionType::Smooth)
    , m_scene2LabelAlpha(m_transitionCache, TimeSpan::FromMilliseconds(200), TransitionType::Smooth)
    , m_menuTextLDR("LDR scene")
    , m_menuTextHDR("HDR scene")
    , m_captionTopLeft("LDR")
    , m_captionTopRight("HDR")
    , SplitX(m_transitionCache, TimeSpan::FromMilliseconds(400), TransitionType::Smooth)
  {
    const auto options = config.GetOptions<OptionParser>();
    SetState(options->GetScene());

    BuildUI();
    PrepareTransition();

    // Start with the menu enabled
    ShowMenu(true);
  }


  MenuUI::~MenuUI() = default;


  void MenuUI::SetNoteLabel(const std::string& content)
  {
    if (!m_rootCanvas)
    {
      FSLLOG3_DEBUG_WARNING("No root canvas available");
      return;
    }
    if (!m_labelNote)
    {
      auto context = m_uiExtension->GetContext();
      m_labelNote = std::make_shared<UI::Label>(context);
      m_labelNote->SetAlignmentX(UI::ItemAlignment::Center);
      m_labelNote->SetAlignmentY(UI::ItemAlignment::Far);
      m_rootCanvas->AddChild(m_labelNote);
    }
    m_labelNote->SetContent(content);
  }

  void MenuUI::SetCaptionLeft(const std::string& content)
  {
    if (content == m_captionTopLeft)
    {
      return;
    }
    m_captionTopLeft = content;
    if (m_labelTopLeft)
    {
      m_labelTopLeft->SetContent(m_captionTopLeft);
    }
  }


  void MenuUI::SetCaptionRight(const std::string& content)
  {
    if (content == m_captionTopRight)
    {
      return;
    }
    m_captionTopRight = content;
    if (m_labelTopRight)
    {
      m_labelTopRight->SetContent(m_captionTopRight);
    }
  }


  void MenuUI::SetMenuTextLeft(const std::string& content)
  {
    if (content == m_menuTextLDR)
    {
      return;
    }
    m_menuTextLDR = content;
    if (m_checkboxLDR)
    {
      m_checkboxLDR->SetText(m_menuTextLDR);
    }
  }


  void MenuUI::SetMenuTextRight(const std::string& content)
  {
    if (content == m_menuTextHDR)
    {
      return;
    }
    m_menuTextHDR = content;
    if (m_checkboxHDR)
    {
      m_checkboxHDR->SetText(m_menuTextHDR);
    }
  }


  void MenuUI::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }
    const bool isPressed = event.IsPressed();

    switch (event.GetKey())
    {
    case VirtualKey::GamePadButtonLeftShoulder:
      m_gamepadState.B1 = isPressed;
      break;
    case VirtualKey::GamePadButtonRightShoulder:
      m_gamepadState.B2 = isPressed;
      break;
    case VirtualKey::GamePadButtonA:
      m_gamepadState.B3 = isPressed;
      break;
    default:
      break;
    }

    if (!isPressed)
    {
      return;
    }
    switch (event.GetKey())
    {
    case VirtualKey::Code1:
    case VirtualKey::GamePadDpadLeft:
      SetState(SceneState::Scene1);
      break;
    case VirtualKey::Code2:
    case VirtualKey::GamePadDpadRight:
      SetState(SceneState::Scene2);
      break;
    case VirtualKey::Code3:
    case VirtualKey::GamePadDpadUp:
      SetState(SceneState::Split2);
      break;
    default:
      break;
    }
  }


  void MenuUI::OnContentChanged(const UI::RoutedEventArgs& args, const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    FSL_PARAM_NOT_USED(args);

    if (theEvent->IsHandled())
    {
      return;
    }

    auto source = theEvent->GetSource();

    if (source == m_checkboxLDR)
    {
      UpdateStateBasedOnCheckboxes(m_checkboxLDR);
    }
    else if (source == m_checkboxHDR)
    {
      UpdateStateBasedOnCheckboxes(m_checkboxHDR);
    }
  }


  void MenuUI::ToggleMenu()
  {
    ShowMenu(!m_configWindow);
  }


  void MenuUI::ShowMenu(const bool enabled)
  {
    if (enabled)
    {
      if (m_configWindow)
      {
        return;
      }

      CreateMenuUI();
    }
    else
    {
      if (!m_configWindow)
      {
        return;
      }

      DestroyMenuUI();
    }
  }

  void MenuUI::Update(const DemoTime& demoTime)
  {
    SplitX.Update(TimeSpan(demoTime.ElapsedTime.Ticks()));
    m_scene1LabelAlpha.Update(TimeSpan(demoTime.ElapsedTime.Ticks()));
    m_scene2LabelAlpha.Update(TimeSpan(demoTime.ElapsedTime.Ticks()));

    const float alpha1 = m_scene1LabelAlpha.GetValue();
    const float alpha2 = m_scene2LabelAlpha.GetValue();
    if (m_labelTopLeft)
    {
      m_labelTopLeft->SetFontColor(Color(alpha1, alpha1, alpha1, alpha1));
    }
    if (m_labelTopRight)
    {
      m_labelTopRight->SetFontColor(Color(alpha2, alpha2, alpha2, alpha2));
    }
  }


  void MenuUI::Draw()
  {
    m_uiExtension->Draw();
  }


  void MenuUI::UpdateExposureInput(const DemoTime& demoTime, const KeyboardState& keyboardState)
  {
    const float exposureAdd = 1.0f * demoTime.DeltaTime;
    if (keyboardState.IsKeyDown(VirtualKey::Q) || m_gamepadState.B1)
    {
      m_exposureSlider->SetValue(m_exposureSlider->GetValue() - exposureAdd);
    }
    else if (keyboardState.IsKeyDown(VirtualKey::E) || m_gamepadState.B2)
    {
      m_exposureSlider->SetValue(m_exposureSlider->GetValue() + exposureAdd);
    }
    if (keyboardState.IsKeyDown(VirtualKey::R) || m_gamepadState.B3)
    {
      m_exposureSlider->SetValue(LocalConfig::Exposure.Get());
    }
  }


  float MenuUI::GetExposure() const
  {
    return m_exposureSlider->GetValue();
  }


  void MenuUI::SetExposure(const float value)
  {
    if (value == m_exposureSlider->GetValue())
    {
      return;
    }
    m_exposureSlider->SetValue(value);
  }


  void MenuUI::SetState(const SceneState newState)
  {
    if (newState == m_state)
    {
      return;
    }

    m_state = newState;

    switch (m_state)
    {
    case SceneState::Scene1:
      SplitX.SetValue(1.0f);
      m_scene1LabelAlpha.SetValue(1.0f);
      m_scene2LabelAlpha.SetValue(0.0f);
      break;
    case SceneState::Scene2:
      SplitX.SetValue(0.0f);
      m_scene1LabelAlpha.SetValue(0.0f);
      m_scene2LabelAlpha.SetValue(1.0f);
      break;
    case SceneState::Split2:
    default:
      SplitX.SetValue(0.5f);
      m_scene1LabelAlpha.SetValue(1.0f);
      m_scene2LabelAlpha.SetValue(1.0f);
      break;
    }

    UpdateUIState();
  }


  void MenuUI::UpdateUIState()
  {
    if (m_checkboxLDR)
    {
      m_checkboxLDR->SetIsChecked(m_state == SceneState::Scene1 || m_state == SceneState::Split2);
    }
    if (m_checkboxHDR)
    {
      m_checkboxHDR->SetIsChecked(m_state == SceneState::Scene2 || m_state == SceneState::Split2);
    }
  }

  void MenuUI::PrepareTransition()
  {
    SplitX.ForceComplete();
    m_scene1LabelAlpha.ForceComplete();
    m_scene2LabelAlpha.ForceComplete();
  }


  void MenuUI::BuildUI()
  {
    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();

    m_rootCanvas = CreateUI(context);

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(m_rootCanvas);
  }


  void MenuUI::CreateMenuUI()
  {
    if (m_configWindow)
    {
      return;
    }

    auto context = m_uiExtension->GetContext();
    m_configWindow = CreateConfigDialog(context, m_uiColorSpace);
    m_rootCanvas->AddChild(m_configWindow);
    UpdateUIState();
  }


  void MenuUI::DestroyMenuUI()
  {
    if (!m_configWindow)
    {
      return;
    }

    // Close the menu window
    m_rootCanvas->RemoveChild(m_configWindow);
    m_configWindow.reset();

    // Clear all pointers to controls from the main menu
    m_checkboxHDR.reset();
    m_checkboxLDR.reset();
    m_exposureSlider.reset();
  }


  void MenuUI::UpdateStateBasedOnCheckboxes(const std::shared_ptr<UI::Switch>& source)
  {
    if (!m_checkboxLDR || !m_checkboxHDR)
    {
      return;
    }

    bool scene1 = m_checkboxLDR->IsChecked();
    bool scene2 = m_checkboxHDR->IsChecked();

    if (!scene1 && !scene2)
    {
      // Error tried to disable both
      source->SetIsChecked(true);
      if (source == m_checkboxLDR)
      {
        scene1 = true;
      }
      else
      {
        scene2 = true;
      }
    }

    if (scene1 && scene2)
    {
      SetState(SceneState::Split2);
    }
    else if (scene1)
    {
      SetState(SceneState::Scene1);
    }
    else if (scene2)
    {
      SetState(SceneState::Scene2);
    }
  }


  std::shared_ptr<UI::CanvasLayout> MenuUI::CreateUI(const std::shared_ptr<UI::WindowContext>& context)
  {
    // Create a label to write stuff into when a button is pressed
    m_labelTopLeft = std::make_shared<UI::Label>(context);
    m_labelTopLeft->SetAlignmentX(UI::ItemAlignment::Near);
    m_labelTopLeft->SetAlignmentY(UI::ItemAlignment::Near);
    m_labelTopLeft->SetContent(m_captionTopLeft);

    m_labelTopRight = std::make_shared<UI::Label>(context);
    m_labelTopRight->SetAlignmentX(UI::ItemAlignment::Far);
    m_labelTopRight->SetAlignmentY(UI::ItemAlignment::Near);
    m_labelTopRight->SetContent(m_captionTopRight);

    // Create a 'root' layout we use the recommended fill layout as it will utilize all available space on the screen
    // We then add the 'player' stack to it and the label
    auto rootLayout = std::make_shared<UI::CanvasLayout>(context);
    rootLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    rootLayout->SetAlignmentY(UI::ItemAlignment::Stretch);
    rootLayout->AddChild(m_labelTopLeft);
    rootLayout->AddChild(m_labelTopRight);

    return rootLayout;
  }


  std::shared_ptr<UI::BaseWindow> MenuUI::CreateConfigDialog(const std::shared_ptr<UI::WindowContext>& context, const ColorSpace colorSpace)
  {
    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension, colorSpace);
    auto& factory = *uiControlFactory;

    m_checkboxLDR = factory.CreateSwitch(m_menuTextLDR);
    m_checkboxLDR->SetAlignmentX(UI::ItemAlignment::Near);
    m_checkboxLDR->SetAlignmentY(UI::ItemAlignment::Center);

    m_checkboxHDR = factory.CreateSwitch(m_menuTextHDR);
    m_checkboxHDR->SetAlignmentX(UI::ItemAlignment::Near);
    m_checkboxHDR->SetAlignmentY(UI::ItemAlignment::Center);

    auto labelExposure = factory.CreateLabel("Exposure");
    labelExposure->SetAlignmentX(UI::ItemAlignment::Near);
    labelExposure->SetAlignmentY(UI::ItemAlignment::Center);

    m_exposureSlider = factory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, LocalConfig::Exposure);
    m_exposureSlider->SetAlignmentX(UI::ItemAlignment::Stretch);

    auto layout = std::make_shared<UI::GridLayout>(context);
    layout->SetAlignmentX(UI::ItemAlignment::Stretch);
    layout->SetAlignmentY(UI::ItemAlignment::Stretch);
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 16));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Fixed, 16));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Auto));
    layout->AddColumnDefinition(UI::GridColumnDefinition(UI::GridUnitType::Star, 1.0f));
    layout->AddRowDefinition(UI::GridRowDefinition(UI::GridUnitType::Auto));
    layout->AddChild(m_checkboxLDR, 0, 0);
    layout->AddChild(m_checkboxHDR, 2, 0);
    layout->AddChild(labelExposure, 4, 0);
    layout->AddChild(m_exposureSlider, 5, 0);

    auto bottomBar = factory.CreateBottomBar();
    bottomBar->SetContent(layout);
    return bottomBar;
  }
}
