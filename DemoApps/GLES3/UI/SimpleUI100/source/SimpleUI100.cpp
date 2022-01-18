/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include "SimpleUI100.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Sprite/ISpriteResourceManager.hpp>
#include <FslSimpleUI/App/Theme/ThemeSelector.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/Control/Background.hpp>
#include <FslSimpleUI/Base/Control/SimpleImageButton.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Theme/Base/IThemeControlFactory.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>

namespace Fsl
{
  SimpleUI100::SimpleUI100(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_uiEventListener(this)    // The UI listener forwards call to 'this' object
    , m_uiExtension(
        std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "UIAtlas/UIAtlas_160dpi"))    // Prepare the extension
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);

    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();
    ISpriteResourceManager& spriteResourceManager = m_uiExtension->GetSpriteResourceManager();
    auto defaultMaterialId = m_uiExtension->GetDefaultMaterialId();

    auto uiControlFactory = UI::Theme::ThemeSelector::CreateControlFactory(*m_uiExtension);
    auto& factory = *uiControlFactory;

    auto spriteBack = spriteResourceManager.CreateImageSprite(defaultMaterialId, "Icon/Av/ic_skip_previous_white_48dp");
    auto spriteNext = spriteResourceManager.CreateImageSprite(defaultMaterialId, "Icon/Av/ic_skip_next_white_48dp");
    auto spritePlay = spriteResourceManager.CreateImageSprite(defaultMaterialId, "Icon/Av/ic_play_arrow_white_48dp");
    auto spriteStop = spriteResourceManager.CreateImageSprite(defaultMaterialId, "Icon/Av/ic_stop_white_48dp");

    // Allocate the four player buttons
    m_btnBack = factory.CreateImageButton(UI::Theme::ImageButtonType::Normal, spriteBack);
    m_btnNext = factory.CreateImageButton(UI::Theme::ImageButtonType::Normal, spriteNext);
    m_btnPlay = factory.CreateImageButton(UI::Theme::ImageButtonType::Normal, spritePlay);
    m_btnStop = factory.CreateImageButton(UI::Theme::ImageButtonType::Normal, spriteStop);

    // Create a horizontal stack layout and add the four buttons (in the desired order)
    auto playerStack = std::make_shared<UI::StackLayout>(context);
    playerStack->SetLayoutOrientation(UI::LayoutOrientation::Horizontal);
    playerStack->SetAlignmentX(UI::ItemAlignment::Center);
    playerStack->SetAlignmentY(UI::ItemAlignment::Far);
    playerStack->AddChild(m_btnBack);
    playerStack->AddChild(m_btnPlay);
    playerStack->AddChild(m_btnStop);
    playerStack->AddChild(m_btnNext);

    // Create a bottom bar for the buttons
    auto bar = factory.CreateBottomBar(playerStack);

    // Create a label to write stuff into when a button is pressed
    m_label = std::make_shared<UI::Label>(context);
    m_label->SetAlignmentX(UI::ItemAlignment::Center);
    m_label->SetAlignmentY(UI::ItemAlignment::Center);

    // Create a 'root' layout we use the recommended fill layout as it will utilize all available space on the screen
    // We then add the 'player' stack to it and the label
    auto fillLayout = std::make_shared<UI::FillLayout>(context);
    fillLayout->AddChild(bar);
    fillLayout->AddChild(m_label);

    // Finally add everything to the window manager (to ensure its seen)
    auto windowManager = m_uiExtension->GetWindowManager();
    windowManager->Add(fillLayout);
  }


  SimpleUI100::~SimpleUI100() = default;


  void SimpleUI100::OnSelect(const UI::RoutedEventArgs& /*args*/, const std::shared_ptr<UI::WindowSelectEvent>& theEvent)
  {
    if (theEvent->GetSource() == m_btnBack)
    {
      m_label->SetContent("Back pressed");
    }
    else if (theEvent->GetSource() == m_btnNext)
    {
      m_label->SetContent("Next pressed");
    }
    else if (theEvent->GetSource() == m_btnPlay)
    {
      m_label->SetContent("Play pressed");
    }
    else if (theEvent->GetSource() == m_btnStop)
    {
      m_label->SetContent("Stop pressed");
    }
  }


  void SimpleUI100::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);
    glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calling this last allows the UI to draw on top of everything.
    // Beware that the UI drawing methods might alter the OpenGL state!
    m_uiExtension->Draw();
  }
}
