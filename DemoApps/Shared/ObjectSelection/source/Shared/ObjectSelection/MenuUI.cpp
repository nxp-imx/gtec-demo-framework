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

#include <Shared/ObjectSelection/MenuUI.hpp>
#include <Shared/ObjectSelection/OptionParser.hpp>
#include <FslSimpleUI/Base/Control/Label.hpp>

namespace Fsl
{
  using namespace UI;

  namespace
  {
  }


  MenuUI::MenuUI(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "MainAtlas"))
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


  MenuUI::~MenuUI()
  {
  }


  void MenuUI::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
      return;

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
    case VirtualKey::M:
      ToggleMenu();
      event.Handled();
      break;
    default:
      break;
    }
  }


  void MenuUI::OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_btnMenu)
    {
      theEvent->Handled();
      ToggleMenu();
    }
  }


  void MenuUI::OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent)
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
    //else if (theEvent->GetSource() == m_cbMenuFinalBloom)
    //{
    //  theEvent->Handled();
    //  m_renderFinalBloom = m_cbMenuFinalBloom->IsChecked();
    //}
  }


  void MenuUI::ToggleMenu()
  {
    if (m_layoutMenu)
      DestroyMenuUI();
    else
      CreateMenuUI();
  }


  void MenuUI::Draw()
  {
    m_uiExtension->Draw();
  }


  void MenuUI::BuildUI()
  {
    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();
    AtlasTexture2D texMenu = m_uiExtension->GetAtlasTexture2D("Player/Pause");

    m_btnMenu = std::make_shared<ImageButton>(context);
    m_btnMenu->SetContent(texMenu);
    m_btnMenu->SetAlignmentX(ItemAlignment::Near);
    m_btnMenu->SetAlignmentY(ItemAlignment::Far);

    m_mainMenuStack = std::make_shared<StackLayout>(context);
    m_mainMenuStack->SetLayoutOrientation(LayoutOrientation::Vertical);
    m_mainMenuStack->SetAlignmentX(ItemAlignment::Near);
    m_mainMenuStack->SetAlignmentY(ItemAlignment::Near);

    auto internalStack = std::make_shared<StackLayout>(context);
    internalStack->SetLayoutOrientation(LayoutOrientation::Vertical);
    internalStack->SetAlignmentX(ItemAlignment::Near);
    internalStack->SetAlignmentY(ItemAlignment::Far);
    internalStack->Children.Add(m_mainMenuStack);
    internalStack->Children.Add(m_btnMenu);

    m_rootLayout = std::make_shared<FillLayout>(context);
    m_rootLayout->Children.Add(internalStack);

    // Finally add everything to the window manager (to ensure its seen)
    context->WindowManager->Add(m_rootLayout);
  }


  void MenuUI::CreateMenuUI()
  {
    if (m_layoutMenu)
      return;

    auto context = m_uiExtension->GetContext();
    AtlasTexture2D texCheckBoxC(m_uiExtension->GetAtlasTexture2D("CheckBoxC"));
    AtlasTexture2D texCheckBoxU(m_uiExtension->GetAtlasTexture2D("CheckBoxU"));
    AtlasTexture2D texSlider(m_uiExtension->GetAtlasTexture2D("Slider"));
    AtlasTexture2D texSliderCursor(m_uiExtension->GetAtlasTexture2D("SliderCursor"));
    ThicknessF sliderCursorPadding(13, 0, 13, 0);
    NineSlice sliderNineSlice(13, 0, 13, 0);


    // Create the outer stack for the menu
    m_layoutMenu = std::make_shared<StackLayout>(context);
    m_layoutMenu->SetLayoutOrientation(LayoutOrientation::Vertical);
    m_layoutMenu->SetAlignmentX(ItemAlignment::Near);
    m_layoutMenu->SetAlignmentY(ItemAlignment::Near);
    m_mainMenuStack->Children.Add(m_layoutMenu);

    m_cbMenuDrawNearPlaneMouse = std::make_shared<CheckBox>(context);
    m_cbMenuDrawNearPlaneMouse->SetText("NearPlaneMouseCross");
    m_cbMenuDrawNearPlaneMouse->SetCheckedTexture(texCheckBoxC);
    m_cbMenuDrawNearPlaneMouse->SetUncheckedTexture(texCheckBoxU);

    m_cbMenuDrawFarPlaneMouse = std::make_shared<CheckBox>(context);
    m_cbMenuDrawFarPlaneMouse->SetText("FarPlaneMouseCross");
    m_cbMenuDrawFarPlaneMouse->SetCheckedTexture(texCheckBoxC);
    m_cbMenuDrawFarPlaneMouse->SetUncheckedTexture(texCheckBoxU);

    m_cbMenuOrientedBoundingBox = std::make_shared<CheckBox>(context);
    m_cbMenuOrientedBoundingBox->SetText("OrientedBoundingBox");
    m_cbMenuOrientedBoundingBox->SetCheckedTexture(texCheckBoxC);
    m_cbMenuOrientedBoundingBox->SetUncheckedTexture(texCheckBoxU);

    m_cbMenuAxisAlignedBoundingBox = std::make_shared<CheckBox>(context);
    m_cbMenuAxisAlignedBoundingBox->SetText("AxisAlignedBoundingBox");
    m_cbMenuAxisAlignedBoundingBox->SetCheckedTexture(texCheckBoxC);
    m_cbMenuAxisAlignedBoundingBox->SetUncheckedTexture(texCheckBoxU);

    //m_cbMenuFinalBloom = std::make_shared<CheckBox>(context);
    //m_cbMenuFinalBloom->SetText("Final bloom");
    //m_cbMenuFinalBloom->SetCheckedTexture(texCheckBoxC);
    //m_cbMenuFinalBloom->SetUncheckedTexture(texCheckBoxU);

    m_layoutMenu->Children.Add(m_cbMenuOrientedBoundingBox);
    m_layoutMenu->Children.Add(m_cbMenuAxisAlignedBoundingBox);
    m_layoutMenu->Children.Add(m_cbMenuDrawNearPlaneMouse);
    m_layoutMenu->Children.Add(m_cbMenuDrawFarPlaneMouse);
    //m_layoutMenu->Children.Add(m_cbMenuFinalBloom);

    //m_layoutMenu->Children.Add(stack1);
    //m_layoutMenu->Children.Add(stack2);

    UpdateControls();
  }


  void MenuUI::SetDrawNearPlaneMouseEnabled(bool enabled)
  {
    if (m_drawNearPlaneMouse == enabled)
      return;

    m_drawNearPlaneMouse = enabled;
    UpdateControls();
  }


  void MenuUI::SetDrawFarPlaneMouseEnabled(bool enabled)
  {
    if (m_drawFarPlaneMouse == enabled)
      return;

    m_drawFarPlaneMouse = enabled;
    UpdateControls();
  }


  void MenuUI::SetDrawOrientedBoundingBoxEnabled(bool enabled)
  {
    if (m_drawOrientedBoundingBox == enabled)
      return;

    m_drawOrientedBoundingBox = enabled;
    UpdateControls();
  }


  void MenuUI::SetDrawAxisAlignedBoundingBoxEnabled(bool enabled)
  {
    if (m_drawAxisAlignedBoundingBox == enabled)
      return;

    m_drawAxisAlignedBoundingBox = enabled;
    UpdateControls();
  }


  //void MenuUI::SetFinalBloomEnabled(bool enabled)
  //{
  //  if (m_renderFinalBloom == enabled)
  //    return;

  //  m_renderFinalBloom = enabled;
  //  UpdateControls();
  //}


  void MenuUI::DestroyMenuUI()
  {
    if (!m_layoutMenu)
      return;

    // Close the menu window
    m_mainMenuStack->Children.Remove(m_layoutMenu);
    m_layoutMenu.reset();

    // Clear all points to controls from the main menu
    m_cbMenuDrawNearPlaneMouse.reset();
    m_cbMenuDrawFarPlaneMouse.reset();
    m_cbMenuOrientedBoundingBox.reset();
    m_cbMenuAxisAlignedBoundingBox.reset();
    //m_cbMenuFinalBloom.reset();
  }

  void MenuUI::UpdateControls()
  {
    if (m_cbMenuDrawNearPlaneMouse)
      m_cbMenuDrawNearPlaneMouse->SetIsChecked(m_drawNearPlaneMouse);
    if (m_cbMenuDrawFarPlaneMouse)
      m_cbMenuDrawFarPlaneMouse->SetIsChecked(m_drawFarPlaneMouse);
    if (m_cbMenuOrientedBoundingBox)
      m_cbMenuOrientedBoundingBox->SetIsChecked(m_drawOrientedBoundingBox);
    if (m_cbMenuAxisAlignedBoundingBox)
      m_cbMenuAxisAlignedBoundingBox->SetIsChecked(m_drawAxisAlignedBoundingBox);
    //if (m_cbMenuFinalBloom)
    //  m_cbMenuFinalBloom->SetIsChecked(m_renderFinalBloom);
  }
}
