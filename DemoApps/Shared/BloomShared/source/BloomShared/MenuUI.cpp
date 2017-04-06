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

#include <BloomShared/MenuUI.hpp>
#include <BloomShared/OptionParser.hpp>
#include <FslSimpleUI/Control/Label.hpp>

namespace Fsl
{
  using namespace UI;

  namespace
  {
    const int32_t SLIDER_RANGE = 1000;
  }


  MenuUI::MenuUI(const DemoAppConfig& config)
    : m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "MainAtlas"))
    , m_optionParser(config.GetOptions<OptionParser>())
    , m_sceneId(m_optionParser->GetSceneId())
    , m_rotateEnabled(m_optionParser->IsRotateEnabled())
    , m_blurEnabled(m_optionParser->IsBlurPassEnabled())
    , m_brightPassEnabled(m_optionParser->IsBrightPassEnabled())
    , m_renderFinalScene(m_optionParser->IsFinalSceneEnabled())
    , m_renderFinalBloom(m_optionParser->IsFinalBloomEnabled())
    , m_showBuffersEnabled(m_optionParser->IsShowBuffersEnabled())
    , m_scaleInputSequentially(m_optionParser->IsScaleInputSequentiallyEnabled())
    , m_kernelWeightMod(int32_t(1.0f * SLIDER_RANGE))
    , m_blendLevel(1.0f)
  {
    BuildUI();
  }


  MenuUI::~MenuUI()
  {
  }


  void MenuUI::OnKeyEvent(const KeyEvent& event)
  {
    if (!event.IsPressed())
      return;

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
      m_blendLevel = m_sliderBlend->GetValue() / float(SLIDER_RANGE);
    }
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

    m_cbMenuRotate = std::make_shared<CheckBox>(context);
    m_cbMenuRotate->SetText("Rotate");
    m_cbMenuRotate->SetCheckedTexture(texCheckBoxC);
    m_cbMenuRotate->SetUncheckedTexture(texCheckBoxU);

    m_cbMenuBlur = std::make_shared<CheckBox>(context);
    m_cbMenuBlur->SetText("Blur");
    m_cbMenuBlur->SetCheckedTexture(texCheckBoxC);
    m_cbMenuBlur->SetUncheckedTexture(texCheckBoxU);

    m_cbMenuBright = std::make_shared<CheckBox>(context);
    m_cbMenuBright->SetText("Brightness pass");
    m_cbMenuBright->SetCheckedTexture(texCheckBoxC);
    m_cbMenuBright->SetUncheckedTexture(texCheckBoxU);

    m_cbMenuFinalScene = std::make_shared<CheckBox>(context);
    m_cbMenuFinalScene->SetText("Final scene");
    m_cbMenuFinalScene->SetCheckedTexture(texCheckBoxC);
    m_cbMenuFinalScene->SetUncheckedTexture(texCheckBoxU);

    m_cbMenuFinalBloom = std::make_shared<CheckBox>(context);
    m_cbMenuFinalBloom->SetText("Final bloom");
    m_cbMenuFinalBloom->SetCheckedTexture(texCheckBoxC);
    m_cbMenuFinalBloom->SetUncheckedTexture(texCheckBoxU);

    m_cbMenuShowBuffers = std::make_shared<CheckBox>(context);
    m_cbMenuShowBuffers->SetText("Show buffers");
    m_cbMenuShowBuffers->SetCheckedTexture(texCheckBoxC);
    m_cbMenuShowBuffers->SetUncheckedTexture(texCheckBoxU);

    m_cbScaleInputSequentially = std::make_shared<CheckBox>(context);
    m_cbScaleInputSequentially->SetText("Scale sequentially");
    m_cbScaleInputSequentially->SetCheckedTexture(texCheckBoxC);
    m_cbScaleInputSequentially->SetUncheckedTexture(texCheckBoxU);


    m_sliderBlur = std::make_shared<SliderAndValueLabel>(context);
    m_sliderBlur->SetAlignmentX(ItemAlignment::Stretch);
    m_sliderBlur->SetValueLimits(1, SLIDER_RANGE);
    m_sliderBlur->SetBackgroundTexture(texSlider);
    m_sliderBlur->SetCursorTexture(texSliderCursor);
    m_sliderBlur->SetCursorPadding(sliderCursorPadding);
    m_sliderBlur->SetNineSlice(sliderNineSlice);

    m_sliderBlend = std::make_shared<SliderAndValueLabel>(context);
    m_sliderBlend->SetAlignmentX(ItemAlignment::Stretch);
    m_sliderBlend->SetValueLimits(0, SLIDER_RANGE);
    m_sliderBlend->SetBackgroundTexture(texSlider);
    m_sliderBlend->SetCursorTexture(texSliderCursor);
    m_sliderBlend->SetCursorPadding(sliderCursorPadding);
    m_sliderBlend->SetNineSlice(sliderNineSlice);

    //auto label1 = std::make_shared<Label>(context);
    //label1->SetContent("Kernel");
    //label1->SetWidth((200)
    //auto stack1 = std::make_shared<StackLayout>(context);
    //stack1->SetLayoutOrientation(LayoutOrientation::Horizontal);
    //stack1->Children.Add(label1);
    //stack1->Children.Add(m_sliderBlur);

    //auto label2 = std::make_shared<Label>(context);
    //label2->SetContent("Blend");
    //auto stack2 = std::make_shared<StackLayout>(context);
    //stack2->SetLayoutOrientation(LayoutOrientation::Horizontal);
    //stack2->Children.Add(label2);
    //stack2->Children.Add(m_sliderBlend);

    m_layoutMenu->Children.Add(m_cbMenuRotate);
    m_layoutMenu->Children.Add(m_cbMenuBlur);
    m_layoutMenu->Children.Add(m_cbMenuBright);
    m_layoutMenu->Children.Add(m_cbMenuFinalScene);
    m_layoutMenu->Children.Add(m_cbMenuFinalBloom);
    m_layoutMenu->Children.Add(m_cbMenuShowBuffers);
    m_layoutMenu->Children.Add(m_cbScaleInputSequentially);

    m_layoutMenu->Children.Add(m_sliderBlur);
    m_layoutMenu->Children.Add(m_sliderBlend);
    //m_layoutMenu->Children.Add(stack1);
    //m_layoutMenu->Children.Add(stack2);

    UpdateControls();
  }


  void MenuUI::SetRotateEnabled(bool enabled)
  {
    if (m_rotateEnabled == enabled)
      return;

    m_rotateEnabled = enabled;
    UpdateControls();
  }


  void MenuUI::SetBlurEnabled(bool enabled)
  {
    if (m_blurEnabled == enabled)
      return;

    m_blurEnabled = enabled;
    UpdateControls();
  }


  void MenuUI::SetBrightPassEnabled(bool enabled)
  {
    if (m_brightPassEnabled == enabled)
      return;

    m_brightPassEnabled = enabled;
    UpdateControls();
  }


  void MenuUI::SetFinalSceneEnabled(bool enabled)
  {
    if (m_renderFinalScene == enabled)
      return;

    m_renderFinalScene = enabled;
    UpdateControls();
  }


  void MenuUI::SetFinalBloomEnabled(bool enabled)
  {
    if (m_renderFinalBloom == enabled)
      return;

    m_renderFinalBloom = enabled;
    UpdateControls();
  }


  void MenuUI::SetShowBuffersEnabled(bool enabled)
  {
    if (m_showBuffersEnabled == enabled)
      return;

    m_showBuffersEnabled = enabled;
    UpdateControls();
  }


  void MenuUI::SetScaleInputSequentiallyEnabled(bool enabled)
  {
    if (m_scaleInputSequentially == enabled)
      return;

    m_scaleInputSequentially = enabled;
    UpdateControls();
  }


  int32_t MenuUI::GetKernelWeightRange() const
  {
    return SLIDER_RANGE;
  }


  void MenuUI::DestroyMenuUI()
  {
    if (!m_layoutMenu)
      return;

    // Close the menu window
    m_mainMenuStack->Children.Remove(m_layoutMenu);
    m_layoutMenu.reset();

    // Clear all points to controls from the main menu
    m_cbMenuRotate.reset();
    m_cbMenuBlur.reset();
    m_cbMenuBright.reset();
    m_cbMenuFinalScene.reset();
    m_cbMenuFinalBloom.reset();
    m_cbMenuShowBuffers.reset();

    m_sliderBlur.reset();
    m_sliderBlend.reset();
  }

  void MenuUI::UpdateControls()
  {
    if (m_cbMenuRotate)
      m_cbMenuRotate->SetIsChecked(m_rotateEnabled);
    if (m_cbMenuBlur)
      m_cbMenuBlur->SetIsChecked(m_blurEnabled);
    if (m_cbMenuBright)
      m_cbMenuBright->SetIsChecked(m_brightPassEnabled);
    if (m_cbMenuFinalScene)
      m_cbMenuFinalScene->SetIsChecked(m_renderFinalScene);
    if (m_cbMenuFinalBloom)
      m_cbMenuFinalBloom->SetIsChecked(m_renderFinalBloom);
    if (m_cbMenuShowBuffers)
      m_cbMenuShowBuffers->SetIsChecked(m_showBuffersEnabled);
    if (m_cbScaleInputSequentially)
      m_cbScaleInputSequentially->SetIsChecked(m_scaleInputSequentially);
    if (m_sliderBlur)
      m_sliderBlur->SetValue(m_kernelWeightMod);
    if (m_sliderBlend)
      m_sliderBlend->SetValue(int32_t(m_blendLevel * SLIDER_RANGE));
  }
}
