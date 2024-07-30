/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <Shared/Bloom/MenuUI.hpp>
#include <Shared/Bloom/OptionParser.hpp>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr ConstrainedValue<float> BlurRange(1.0f, 0.001f, 1.0f);
      constexpr ConstrainedValue<float> BlendRange(1.0f, 0.001f, 1.0f);
    }
  }

  MenuUI::MenuUI(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))
    , m_optionParser(config.GetOptions<OptionParser>())
    , m_sceneId(m_optionParser->GetSceneId())
    , m_rotateEnabled(m_optionParser->IsRotateEnabled())
    , m_blurEnabled(m_optionParser->IsBlurPassEnabled())
    , m_brightPassEnabled(m_optionParser->IsBrightPassEnabled())
    , m_renderFinalScene(m_optionParser->IsFinalSceneEnabled())
    , m_renderFinalBloom(m_optionParser->IsFinalBloomEnabled())
    , m_showBuffersEnabled(m_optionParser->IsShowBuffersEnabled())
    , m_scaleInputSequentially(m_optionParser->IsScaleInputSequentiallyEnabled())
    , m_kernelWeightMod(1.0f)
    , m_blendLevel(1.0f)
  {
    BuildUI();
  }


  MenuUI::~MenuUI() = default;


  void MenuUI::OnKeyEvent(const KeyEvent& event)
  {
    if (!event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::R:
      SetRotateEnabled(!IsRotateEnabled());
      event.Handled();
      break;
    case VirtualKey::B:
      SetBlurEnabled(!IsBlurEnabled());
      event.Handled();
      break;
    case VirtualKey::L:
      SetBrightPassEnabled(!IsBrightPassEnabled());
      event.Handled();
      break;
    case VirtualKey::S:
      SetShowBuffersEnabled(!IsShowBuffersEnabled());
      event.Handled();
      break;
    case VirtualKey::Q:
      SetFinalSceneEnabled(!IsFinalSceneEnabled());
      event.Handled();
      break;
    case VirtualKey::W:
      SetFinalBloomEnabled(!IsFinalBloomEnabled());
      event.Handled();
      break;
    default:
      break;
    }
  }


  void MenuUI::OnSelect(const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    FSL_PARAM_NOT_USED(theEvent);
  }


  void MenuUI::OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_cbMenuRotate)
    {
      theEvent->Handled();
      m_rotateEnabled = m_cbMenuRotate->IsChecked();
    }
    else if (theEvent->GetSource() == m_cbMenuBlur)
    {
      theEvent->Handled();
      m_blurEnabled = m_cbMenuBlur->IsChecked();
    }
    else if (theEvent->GetSource() == m_cbMenuBright)
    {
      theEvent->Handled();
      m_brightPassEnabled = m_cbMenuBright->IsChecked();
    }
    else if (theEvent->GetSource() == m_cbMenuFinalScene)
    {
      theEvent->Handled();
      m_renderFinalScene = m_cbMenuFinalScene->IsChecked();
    }
    else if (theEvent->GetSource() == m_cbMenuFinalBloom)
    {
      theEvent->Handled();
      m_renderFinalBloom = m_cbMenuFinalBloom->IsChecked();
    }
    else if (theEvent->GetSource() == m_cbMenuShowBuffers)
    {
      theEvent->Handled();
      m_showBuffersEnabled = m_cbMenuShowBuffers->IsChecked();
    }
    else if (theEvent->GetSource() == m_cbScaleInputSequentially)
    {
      theEvent->Handled();
      m_scaleInputSequentially = m_cbScaleInputSequentially->IsChecked();
    }
    else if (theEvent->GetSource() == m_sliderBlur)
    {
      theEvent->Handled();
      m_kernelWeightMod = m_sliderBlur->GetValue();
    }
    else if (theEvent->GetSource() == m_sliderBlend)
    {
      theEvent->Handled();
      m_blendLevel = m_sliderBlend->GetValue();
    }
  }

  void MenuUI::Draw()
  {
    m_uiExtension->Draw();
  }


  void MenuUI::BuildUI()
  {
    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();

    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
    auto& factory = *uiControlFactory;

    auto layoutMenu = std::make_shared<UI::StackLayout>(context);
    {
      // Create the outer stack for the menu
      layoutMenu->SetOrientation(UI::LayoutOrientation::Vertical);
      layoutMenu->SetAlignmentX(UI::ItemAlignment::Near);
      layoutMenu->SetAlignmentY(UI::ItemAlignment::Near);

      m_cbMenuRotate = factory.CreateSwitch("Rotate", true);
      m_cbMenuRotate->SetAlignmentX(UI::ItemAlignment::Stretch);
      m_cbMenuBlur = factory.CreateSwitch("Blur", true);
      m_cbMenuBlur->SetAlignmentX(UI::ItemAlignment::Stretch);
      m_cbMenuBright = factory.CreateSwitch("Brightness pass", true);
      m_cbMenuBright->SetAlignmentX(UI::ItemAlignment::Stretch);
      m_cbMenuFinalScene = factory.CreateSwitch("Final scene", true);
      m_cbMenuFinalScene->SetAlignmentX(UI::ItemAlignment::Stretch);
      m_cbMenuFinalBloom = factory.CreateSwitch("Final bloom", true);
      m_cbMenuFinalBloom->SetAlignmentX(UI::ItemAlignment::Stretch);
      m_cbMenuShowBuffers = factory.CreateSwitch("Show buffers", false);
      m_cbMenuShowBuffers->SetAlignmentX(UI::ItemAlignment::Stretch);
      m_cbScaleInputSequentially = factory.CreateSwitch("Scale sequentially", true);
      m_cbScaleInputSequentially->SetAlignmentX(UI::ItemAlignment::Stretch);

      auto labelBlur = factory.CreateLabel("Blur");

      m_sliderBlur = factory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, LocalConfig::BlurRange, "{:.3f}");
      m_sliderBlur->SetAlignmentX(UI::ItemAlignment::Stretch);

      auto labelBlend = factory.CreateLabel("Blend");

      m_sliderBlend = factory.CreateSliderFmtValue(UI::LayoutOrientation::Horizontal, LocalConfig::BlendRange, "{:.3f}");
      m_sliderBlend->SetAlignmentX(UI::ItemAlignment::Stretch);

      layoutMenu->AddChild(m_cbMenuRotate);
      layoutMenu->AddChild(m_cbMenuBlur);
      layoutMenu->AddChild(m_cbMenuBright);
      layoutMenu->AddChild(m_cbMenuFinalScene);
      layoutMenu->AddChild(m_cbMenuFinalBloom);
      layoutMenu->AddChild(m_cbMenuShowBuffers);
      layoutMenu->AddChild(m_cbScaleInputSequentially);

      layoutMenu->AddChild(labelBlur);
      layoutMenu->AddChild(m_sliderBlur);
      layoutMenu->AddChild(labelBlend);
      layoutMenu->AddChild(m_sliderBlend);
      // m_layoutMenu->AddChild(stack1);
      // m_layoutMenu->AddChild(stack2);

      UpdateControls();
    }

    auto bar = factory.CreateLeftBar(layoutMenu, UI::Theme::BarType::Transparent);

    m_rootLayout = std::make_shared<UI::FillLayout>(context);
    m_rootLayout->AddChild(bar);

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(m_rootLayout);
  }


  void MenuUI::SetRotateEnabled(bool enabled)
  {
    if (m_rotateEnabled == enabled)
    {
      return;
    }

    m_rotateEnabled = enabled;
    UpdateControls();
  }


  void MenuUI::SetBlurEnabled(bool enabled)
  {
    if (m_blurEnabled == enabled)
    {
      return;
    }

    m_blurEnabled = enabled;
    UpdateControls();
  }


  void MenuUI::SetBrightPassEnabled(bool enabled)
  {
    if (m_brightPassEnabled == enabled)
    {
      return;
    }

    m_brightPassEnabled = enabled;
    UpdateControls();
  }


  void MenuUI::SetFinalSceneEnabled(bool enabled)
  {
    if (m_renderFinalScene == enabled)
    {
      return;
    }

    m_renderFinalScene = enabled;
    UpdateControls();
  }


  void MenuUI::SetFinalBloomEnabled(bool enabled)
  {
    if (m_renderFinalBloom == enabled)
    {
      return;
    }

    m_renderFinalBloom = enabled;
    UpdateControls();
  }


  void MenuUI::SetShowBuffersEnabled(bool enabled)
  {
    if (m_showBuffersEnabled == enabled)
    {
      return;
    }

    m_showBuffersEnabled = enabled;
    UpdateControls();
  }


  void MenuUI::SetScaleInputSequentiallyEnabled(bool enabled)
  {
    if (m_scaleInputSequentially == enabled)
    {
      return;
    }

    m_scaleInputSequentially = enabled;
    UpdateControls();
  }


  void MenuUI::UpdateControls()
  {
    if (m_cbMenuRotate)
    {
      m_cbMenuRotate->SetIsChecked(m_rotateEnabled);
    }
    if (m_cbMenuBlur)
    {
      m_cbMenuBlur->SetIsChecked(m_blurEnabled);
    }
    if (m_cbMenuBright)
    {
      m_cbMenuBright->SetIsChecked(m_brightPassEnabled);
    }
    if (m_cbMenuFinalScene)
    {
      m_cbMenuFinalScene->SetIsChecked(m_renderFinalScene);
    }
    if (m_cbMenuFinalBloom)
    {
      m_cbMenuFinalBloom->SetIsChecked(m_renderFinalBloom);
    }
    if (m_cbMenuShowBuffers)
    {
      m_cbMenuShowBuffers->SetIsChecked(m_showBuffersEnabled);
    }
    if (m_cbScaleInputSequentially)
    {
      m_cbScaleInputSequentially->SetIsChecked(m_scaleInputSequentially);
    }
    if (m_sliderBlur)
    {
      m_sliderBlur->SetValue(m_kernelWeightMod);
    }
    if (m_sliderBlend)
    {
      m_sliderBlend->SetValue(m_blendLevel);
    }
  }
}
