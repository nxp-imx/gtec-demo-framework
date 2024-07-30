/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <Shared/ObjectSelection/MenuUI.hpp>
#include <Shared/ObjectSelection/OptionParser.hpp>

namespace Fsl
{
  MenuUI::MenuUI(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))
    , m_themeFactory(UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension))
    , m_optionParser(config.GetOptions<OptionParser>())
    , m_drawNearPlaneMouse(m_optionParser->IsNearPlaneMouseEnabled())
    , m_drawFarPlaneMouse(m_optionParser->IsFarPlaneMouseEnabled())
    , m_drawOrientedBoundingBox(m_optionParser->IsOrientedBoundingBoxEnabled())
    , m_drawAxisAlignedBoundingBox(m_optionParser->IsAxisAlignedBoundingBoxEnabled())
    , m_randomSeedEnabled(m_optionParser->IsRandomSeedEnabled())
  //, m_renderFinalBloom(m_optionParser->IsFinalBloomEnabled())
  {
    BuildUI();
  }


  MenuUI::~MenuUI() = default;


  void MenuUI::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::N:
      SetDrawNearPlaneMouseEnabled(!IsDrawNearPlaneMouseEnabled());
      event.Handled();
      break;
    case VirtualKey::F:
      SetDrawFarPlaneMouseEnabled(!IsDrawFarPlaneMouseEnabled());
      event.Handled();
      break;
    case VirtualKey::O:
      SetDrawOrientedBoundingBoxEnabled(!IsDrawOrientedBoundingBoxEnabled());
      event.Handled();
      break;
    case VirtualKey::B:
      SetDrawAxisAlignedBoundingBoxEnabled(!IsDrawAxisAlignedBoundingBoxEnabled());
      event.Handled();
      break;
    default:
      break;
    }
  }

  void MenuUI::OnContentChanged(const std::shared_ptr<UI::WindowContentChangedEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_cbMenuDrawNearPlaneMouse)
    {
      theEvent->Handled();
      m_drawNearPlaneMouse = m_cbMenuDrawNearPlaneMouse->IsChecked();
    }
    else if (theEvent->GetSource() == m_cbMenuDrawFarPlaneMouse)
    {
      theEvent->Handled();
      m_drawFarPlaneMouse = m_cbMenuDrawFarPlaneMouse->IsChecked();
    }
    else if (theEvent->GetSource() == m_cbMenuOrientedBoundingBox)
    {
      theEvent->Handled();
      m_drawOrientedBoundingBox = m_cbMenuOrientedBoundingBox->IsChecked();
    }
    else if (theEvent->GetSource() == m_cbMenuAxisAlignedBoundingBox)
    {
      theEvent->Handled();
      m_drawAxisAlignedBoundingBox = m_cbMenuAxisAlignedBoundingBox->IsChecked();
    }
    // else if (theEvent->GetSource() == m_cbMenuFinalBloom)
    //{
    //  theEvent->Handled();
    //  m_renderFinalBloom = m_cbMenuFinalBloom->IsChecked();
    //}
  }


  void MenuUI::Draw()
  {
    m_uiExtension->Draw();
  }


  void MenuUI::BuildUI()
  {
    UI::Theme::IThemeControlFactory& themeFactory = *m_themeFactory;

    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();

    m_rootLayout = std::make_shared<UI::FillLayout>(context);

    // Finally add everything to the window manager (to ensure its seen)
    m_uiExtension->GetWindowManager()->Add(m_rootLayout);

    auto stackLayout = std::make_shared<UI::ComplexStackLayout>(context);
    stackLayout->SetOrientation(UI::LayoutOrientation::Horizontal);
    stackLayout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    stackLayout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    stackLayout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    stackLayout->PushLayoutLength(UI::LayoutLength(UI::LayoutUnitType::Star, 1.0f));
    stackLayout->SetAlignmentX(UI::ItemAlignment::Stretch);
    stackLayout->SetAlignmentY(UI::ItemAlignment::Stretch);

    // Create the outer stack for the menu
    m_layoutMenu = themeFactory.CreateBottomBar(stackLayout);
    m_rootLayout->AddChild(m_layoutMenu);

    m_cbMenuDrawNearPlaneMouse = themeFactory.CreateSwitch("NearPlaneMouseCross");
    m_cbMenuDrawNearPlaneMouse->SetAlignmentX(UI::ItemAlignment::Center);
    m_cbMenuDrawFarPlaneMouse = themeFactory.CreateSwitch("FarPlaneMouseCross");
    m_cbMenuDrawFarPlaneMouse->SetAlignmentX(UI::ItemAlignment::Center);

    m_cbMenuOrientedBoundingBox = themeFactory.CreateSwitch("OrientedBoundingBox");
    m_cbMenuOrientedBoundingBox->SetAlignmentX(UI::ItemAlignment::Center);
    m_cbMenuAxisAlignedBoundingBox = themeFactory.CreateSwitch("AxisAlignedBoundingBox");
    m_cbMenuAxisAlignedBoundingBox->SetAlignmentX(UI::ItemAlignment::Center);

    stackLayout->AddChild(m_cbMenuOrientedBoundingBox);
    stackLayout->AddChild(m_cbMenuAxisAlignedBoundingBox);
    stackLayout->AddChild(m_cbMenuDrawNearPlaneMouse);
    stackLayout->AddChild(m_cbMenuDrawFarPlaneMouse);

    UpdateControls();
  }


  void MenuUI::SetDrawNearPlaneMouseEnabled(bool enabled)
  {
    if (m_drawNearPlaneMouse == enabled)
    {
      return;
    }

    m_drawNearPlaneMouse = enabled;
    UpdateControls();
  }


  void MenuUI::SetDrawFarPlaneMouseEnabled(bool enabled)
  {
    if (m_drawFarPlaneMouse == enabled)
    {
      return;
    }

    m_drawFarPlaneMouse = enabled;
    UpdateControls();
  }


  void MenuUI::SetDrawOrientedBoundingBoxEnabled(bool enabled)
  {
    if (m_drawOrientedBoundingBox == enabled)
    {
      return;
    }

    m_drawOrientedBoundingBox = enabled;
    UpdateControls();
  }


  void MenuUI::SetDrawAxisAlignedBoundingBoxEnabled(bool enabled)
  {
    if (m_drawAxisAlignedBoundingBox == enabled)
    {
      return;
    }

    m_drawAxisAlignedBoundingBox = enabled;
    UpdateControls();
  }


  void MenuUI::UpdateControls()
  {
    if (m_cbMenuDrawNearPlaneMouse)
    {
      m_cbMenuDrawNearPlaneMouse->SetIsChecked(m_drawNearPlaneMouse);
    }
    if (m_cbMenuDrawFarPlaneMouse)
    {
      m_cbMenuDrawFarPlaneMouse->SetIsChecked(m_drawFarPlaneMouse);
    }
    if (m_cbMenuOrientedBoundingBox)
    {
      m_cbMenuOrientedBoundingBox->SetIsChecked(m_drawOrientedBoundingBox);
    }
    if (m_cbMenuAxisAlignedBoundingBox)
    {
      m_cbMenuAxisAlignedBoundingBox->SetIsChecked(m_drawAxisAlignedBoundingBox);
    }
  }
}
