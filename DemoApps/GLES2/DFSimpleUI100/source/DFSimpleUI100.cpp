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

#include "DFSimpleUI100.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <FslSimpleUI/Base/Layout/StackLayout.hpp>
#include <FslSimpleUI/Base/Layout/FillLayout.hpp>
#include <GLES2/gl2.h>

namespace Fsl
{
  using namespace UI;

  DFSimpleUI100::DFSimpleUI100(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_uiEventListener(this)                                                                                     // The UI listener forwards call to 'this' object
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "MainAtlas"))   // Prepare the extension
  {
    // Give the UI a chance to intercept the various DemoApp events.
    RegisterExtension(m_uiExtension);

    // Next up we prepare the actual UI
    auto context = m_uiExtension->GetContext();

    // Fetch some textures from the default texture atlas loaded by the UI extension (this is the 'MainAtlas')
    AtlasTexture2D texBack = m_uiExtension->GetAtlasTexture2D("Player/Back");
    AtlasTexture2D texNext = m_uiExtension->GetAtlasTexture2D("Player/Next");
    AtlasTexture2D texPlay = m_uiExtension->GetAtlasTexture2D("Player/Play");
    AtlasTexture2D texStop = m_uiExtension->GetAtlasTexture2D("Player/Stop");

    // Allocate the four player buttons
    m_btnBack = std::make_shared<ImageButton>(context);
    m_btnNext = std::make_shared<ImageButton>(context);
    m_btnPlay = std::make_shared<ImageButton>(context);
    m_btnStop = std::make_shared<ImageButton>(context);

    // Set the texture for each button
    m_btnBack->SetContent(texBack);
    m_btnNext->SetContent(texNext);
    m_btnPlay->SetContent(texPlay);
    m_btnStop->SetContent(texStop);

    // Create a horizontal stack layout and add the four buttons (in the desired order)
    auto playerStack = std::make_shared<StackLayout>(context);
    playerStack->SetLayoutOrientation(LayoutOrientation::Horizontal);
    playerStack->SetAlignmentX(ItemAlignment::Center);
    playerStack->SetAlignmentY(ItemAlignment::Far);
    playerStack->Children.Add(m_btnBack);
    playerStack->Children.Add(m_btnPlay);
    playerStack->Children.Add(m_btnStop);
    playerStack->Children.Add(m_btnNext);

    // Create a label to write stuff into when a button is pressed
    m_label = std::make_shared<Label>(context);
    m_label->SetAlignmentX(ItemAlignment::Center);
    m_label->SetAlignmentY(ItemAlignment::Center);

    // Create a 'root' layout we use the recommended fill layout as it will utilize all available space on the screen
    // We then add the 'player' stack to it and the label
    auto fillLayout = std::make_shared<FillLayout>(context);
    fillLayout->Children.Add(playerStack);
    fillLayout->Children.Add(m_label);

    // Finally add everything to the window manager (to ensure its seen)
    context->WindowManager->Add(fillLayout);
  }


  DFSimpleUI100::~DFSimpleUI100()
  {
  }


  void DFSimpleUI100::OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent)
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


  void DFSimpleUI100::Draw(const DemoTime& demoTime)
  {
    glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calling this last allows the UI to draw on top of everything.
    // Beware that the UI drawing methods might alter the OpenGL state!
    m_uiExtension->Draw();
  }

}
